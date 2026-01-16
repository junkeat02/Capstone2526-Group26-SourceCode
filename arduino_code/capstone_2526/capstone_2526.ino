#include "main.h"
#include "pulseOximeterMax.h"
#include "mpuRotAngle.h"
#include "BLEsetup.h"
#include "oledDisplay.h"

// Battery level pin setup
const float max_analog = 4096;
int analog_battery = 0;
float actual_battery = 0.0;
int current_battery_percent = 0;
int last_battery_percent = 0;
const unsigned long batt_interval = 10000;
unsigned long last_batt = 0;



// Sensors' instances
pulseOximeterMax* pulseOximeter = nullptr;
mpuRotAngle* mpuAngle = nullptr;
BLEConnection bleConnect;
OledDisplay oledDisplay;



// MAX30102
const unsigned long pulse_interval = 1000;
unsigned long last_pulse = 0;
float current_heartrate = 0.0;
uint8_t current_spo2 = 0;
uint8_t current_ppg = 0;

const unsigned long max_update_interval = 10;  // ms (100 Hz)
unsigned long last_max_update = 0;


// MPU6050 - Motion and Fall Detection
const unsigned long angle_interval = 100;
unsigned long last_angle = 0;
float angle_x = 0.0;

// Fall detection variables
bool fallDetected = false;
int stepsAtFall = 0;
int stepCount = 0;
bool emergencyWaiting = false;

// Fall detection state variables
volatile bool bleResetTriggered = false; // Must be volatile for callbacks
unsigned long fall_impact_time = 0; // Dedicated timer for fall duration
// The time threshold for demo
const unsigned long FALL_CONFIRMATION_TIME = 5000;

// Step detection
// Inactivity tracking
const long INACTIVE_TH = 1 * 60 * 1000;  // 1 minute for testing
unsigned long latest_move = 0;
bool inactivity = 0;

// Bluetooth
const unsigned long ble_interval = 1000;
unsigned long last_ble = 0;

// OLED display timing
const unsigned long oled_interval = 2000;
unsigned long last_oled = 0;
int display_screen = 0;  // 0: steps, 1: heart rate


void setup() {
	Serial.begin(115200);
	delay(1000);

	Wire.begin(SDA_PIN, SCL_PIN);
	Wire.setClock(100000);

#if ENABLE_OLED
	if (!oledDisplay.begin(0x3C)) {
		while (true)
			;
	}
	oledDisplay.showBootScreen("Health Monitor");
	delay(2000);

#endif



#if ENABLE_MPU6050
	mpuAngle = new mpuRotAngle();
	while (!mpuAngle->initial_setup()) {
		delay(500);
	}

#endif



#if ENABLE_MAX30102
	pulseOximeter = new pulseOximeterMax();
	while (!pulseOximeter->initial_setup()) {
		delay(500);
	}
#endif



#if ENABLE_BLE
	bleConnect.initial_setup();
#endif

	latest_move = millis();
}





void loop() {
#if ENABLE_MAX30102
	max_update_loop();
#endif



#if ENABLE_MPU6050
	mpuAngle->update();
#endif



#if ENABLE_MAX30102
	max_loop();
#endif



#if ENABLE_MPU6050
	mpu_loop();
#endif

#if ENABLE_BATTERY
	batt_level();
#endif

#if ENABLE_INACTIVITY
	check_inactivity();
#endif

#if ENABLE_OLED
	oled_loop();
#endif

#if ENABLE_BLE
	ble_loop();
#endif
}



void max_update_loop() {
	unsigned long now = millis();
	if (now - last_max_update >= max_update_interval) {
		pulseOximeter->update();
		last_max_update = now;
	}
}

void max_loop() {
	if (millis() - last_pulse >= pulse_interval) {
float rawIR = pulseOximeter->getRawIR();
float rawRed = pulseOximeter->getRawRed();

current_ppg = rawIR;

// ✅ NO FINGER DETECTION
if (rawIR < 5000) {  
    current_heartrate = 0;
    current_spo2 = 0;
} else {
    current_heartrate = lowPassFilter(pulseOximeter->getHeartRate(), LOW_PASS_ALPHA);
    current_spo2 = pulseOximeter->getSpO2();
}


#ifdef DEBUG_MODE
#ifdef TEST_MAX
		Serial.print("Heart rate: ");
		Serial.print(current_heartrate);
		Serial.print(", SpO2: ");
		Serial.print(current_spo2);
		Serial.print("%");
		Serial.print(", PPG: ");
		Serial.println(current_ppg);
		Serial.print("IR=");
		Serial.print(rawIR);
		Serial.print(" RED=");
		Serial.print(rawRed);
		Serial.print(" HR=");
		Serial.print(current_heartrate);
		Serial.print(" SpO2=");
		Serial.println(current_spo2);
#endif
#endif
		last_pulse = millis();
	}
}


void mpu_loop() {
    if (millis() - last_angle >= angle_interval) {
        mpuAngle->update();

        // 1. Initial Impact Check
        if (mpuAngle->detectFall() && !fallDetected) {
            Serial.println("IMPACT DETECTED!");
            fallDetected = true;
            emergencyWaiting = true;
            fall_impact_time = millis(); // Store the SPECIFIC time of impact
            stepsAtFall = stepCount;
            oledDisplay.startFallFlash(); 
        }

        // 2. Recovery Check (Steps)
        mpuAngle->detectStep();
        int currentSteps = mpuAngle->getStepCount();
        
        if (currentSteps > stepCount) {
            stepCount = currentSteps;
            
            // Check recovery based on the impact time
            unsigned long timeSinceImpact = millis() - fall_impact_time;

            if (fallDetected && timeSinceImpact > 3000) { 
                Serial.println("Recovery detected!");
                resetSystemAlert();
            }

            latest_move = millis(); // This now ONLY handles inactivity
            inactivity = 0;
        }
        last_angle = millis();
    }

    // 3. The Timer Logic - USE THE NEW fall_impact_time
    if (fallDetected && emergencyWaiting) {
        if (millis() - fall_impact_time >= FALL_CONFIRMATION_TIME) {
            Serial.println("!!! FINAL ALERT: NO RECOVERY !!!");
            emergencyWaiting = false; 
            // The OLED continues to flash because fallDetected is still true
        }
    }
}
// Helper function to reset flags
// Add this function anywhere in your main .ino
void resetSystemAlert() {
    fallDetected = false;
    emergencyWaiting = false;
    bleResetTriggered = false;
    inactivity = 0;
    latest_move = millis();
    
    // This is the key command that tells the OLED to go back to normal screens
    oledDisplay.stopFallFlash(); 
    
    mpuAngle->resetFallDetection();
    Serial.println("System Reset Successful.");
}

void ble_loop() {
	if (millis() - last_ble > ble_interval) {
		// Send all data including fall detection status
		bleConnect.send_data(current_heartrate, current_spo2, current_ppg, stepCount, inactivity, fallDetected, current_battery_percent);
#ifdef TEST_BLE
		Serial.println("BLE sent the data.");
#endif
		last_ble = millis();
	}
// 2. Check if the BLE Callback detected a reset command
  if (bleResetTriggered) {
    Serial.println("Action: Resetting Fall Alert via BLE");
    resetSystemAlert(); // This function clears fallDetected and bleResetTriggered
  }
}



void batt_level() {
	analog_battery = analogRead(batt_reader);
	actual_battery = (analog_battery / max_analog) * MAX_INPUT * VOLT_DIVIDER;
	current_battery_percent = int(actual_battery / MAX_BATT * 100);  // Convert to percentage
	if (current_battery_percent > last_battery_percent) {
		current_battery_percent = last_battery_percent;
	}
	if (millis() - last_batt > batt_interval) {

#ifdef DEBUG_MODE
#ifdef TEST_BATT
		Serial.printf("Raw battery level: %d\n", analog_battery);
		Serial.printf("Battery voltage: %.2fV\n", actual_battery);
		Serial.printf("Battery level: %d%%\n", current_battery_percent);
#endif
#endif
		last_battery_percent = current_battery_percent;
		last_batt = millis();
	}
}



void check_inactivity() {
  unsigned long currentTime = millis();
  
  // Only trigger if we aren't already flagged as inactive
  if (inactivity == 0 && (currentTime - latest_move >= INACTIVE_TH)) {
    if (!oledDisplay.isCurrentlyScrolling()) {
      oledDisplay.startScroll("TIME TO MOVE UP!");
      inactivity = 1; // Mark as inactive
      
      // We don't update latest_move here anymore! 
      // We wait for a physical step to reset latest_move.
#ifdef DEBUG_MODE
      Serial.println("Inactivity detected - waiting for steps to reset");
#endif
    }
  }
}


void oled_loop() {
	// Handle fall flash animation (highest priority)
	if (fallDetected && oledDisplay.isFallFlashing()) {
		oledDisplay.updateFallFlash();
		return;  // Skip other display updates during fall alert
	}
	// Handle inactivity scroll animation
	if (oledDisplay.isCurrentlyScrolling()) {
		oledDisplay.updateScroll();
		return;  // Skip other display updates during scrolling
	}

	// Regular screen updates (alternate between screens)
	if (millis() - last_oled >= oled_interval) {
		switch (display_screen) {
			case 0:
				// Show step screen (you'll need to implement step counting in MPU class)
				oledDisplay.showStepScreen(stepCount);
				display_screen = 1;
				break;

			case 1:
				// Show heart rate and SpO2 screen
				if (current_heartrate > 0 && current_spo2 > 0) {
					oledDisplay.showHeartScreen((int)current_heartrate, current_spo2);
				} else {
					// If no heart rate detected, show steps instead
					oledDisplay.showStepScreen(stepCount);
				}
				display_screen = 0;

				break;
		}
		last_oled = millis();
	}
}



float lowPassFilter(float input, float alpha) {
	static float last_output = 0.0f;
	static bool initialised = false;
	if (!initialised) {
		last_output = input;
		initialised = true;
		return last_output;
	}
	last_output = alpha * input + (1 - alpha) * last_output;

	return last_output;
}
