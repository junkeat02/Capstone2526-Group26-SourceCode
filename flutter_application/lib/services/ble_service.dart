import 'dart:async';
import 'dart:typed_data';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'dart:convert';

class BleService {
  static const String serviceUuid = "91bad492-b950-4226-aa2b-4ede9fa42f59";
  static const String charUuid = "cba1d466-344c-4be3-ab3f-189f80dd7518";

  BluetoothDevice? targetDevice;
  // MISSING 1: Define the writeCharacteristic variable
  BluetoothCharacteristic? writeCharacteristic; 

  StreamSubscription? _scanSubscription;
  Timer? _timeoutTimer;

  final _dataController = StreamController<Map<String, dynamic>>.broadcast();
  Stream<Map<String, dynamic>> get healthDataStream => _dataController.stream;

  bool get isConnected => targetDevice != null;

  // ... startScan and disconnect methods remain the same ...

  void _connect(BluetoothDevice device, Function(bool) onLoading, Function(String)? onError) async {
    try {
      await device.connect(autoConnect: false, license: License.free);
      targetDevice = device;

      device.connectionState.listen((state) {
        if (state == BluetoothConnectionState.disconnected) {
          targetDevice = null;
          writeCharacteristic = null; // Clear characteristic on disconnect
          onLoading(false);
        }
      });

      List<BluetoothService> services = await device.discoverServices();
      for (var s in services) {
        if (s.uuid.toString().toLowerCase() == serviceUuid) {
          for (var c in s.characteristics) {
            // MISSING 2: Identify and save the characteristic for writing
            // In many ESP32 setups, the same UUID is used for both Notify and Write.
            // If your Write UUID is different, change this check.
            if (c.uuid.toString().toLowerCase() == charUuid) {
              
              // If it supports Notify, set it up
              if (c.properties.notify) {
                await c.setNotifyValue(true);
                c.lastValueStream.listen(_parseData);
              }

              // Save it to our writeCharacteristic variable so sendResetSignal can use it
              if (c.properties.write || c.properties.writeWithoutResponse) {
                writeCharacteristic = c;
              }
            }
          }
        }
      }
    } catch (e) {
      targetDevice = null;
      if (onError != null) onError("Connection failed: $e");
    } finally {
      onLoading(false);
    }
  }
  void _parseData(List<int> value) {
    if (value.length < 10 || value[0] != 0x11) return;
    _dataController.add({
      'hr': (ByteData.sublistView(Uint8List.fromList(value), 2, 4).getUint16(0, Endian.little) / 100),
      'spo2': value[4],
      'ppg': value[5],
      'steps': value[6],
      'inactivity': value[7],
      'fall': value[8],
      'battery': value[9],
    });
  }

  void startScan(Function(bool) onLoading, {Function(String)? onError}) async {
    // 1. Check if Bluetooth is even on
    if (await FlutterBluePlus.adapterState.first != BluetoothAdapterState.on) {
      if (onError != null) onError("Bluetooth is turned off");
      return;
    }

    onLoading(true);

    // 2. Start scanning
    try {
      // We scan and look for the specific Service UUID of your ESP32
      await FlutterBluePlus.startScan(
        withServices: [Guid(serviceUuid)], 
        timeout: const Duration(seconds: 10),
      );
    } catch (e) {
      if (onError != null) onError("Scan failed: $e");
      onLoading(false);
      return;
    }

    // 3. Listen for scan results
    _scanSubscription = FlutterBluePlus.scanResults.listen((results) {
      for (ScanResult r in results) {
        // If we find the device with our Service UUID
        if (r.advertisementData.serviceUuids.contains(Guid(serviceUuid))) {
          FlutterBluePlus.stopScan();
          _scanSubscription?.cancel();
          _connect(r.device, onLoading, onError); // Proceed to connect
          break;
        }
      }
    }, onError: (e) {
      if (onError != null) onError("Scan error: $e");
      onLoading(false);
    });

    // 4. Handle timeout if no device is found
    _timeoutTimer?.cancel();
    _timeoutTimer = Timer(const Duration(seconds: 11), () {
      if (targetDevice == null) {
        FlutterBluePlus.stopScan();
        onLoading(false);
        if (onError != null) onError("Device not found. Make sure it is on.");
      }
    });
  }

  void disconnect() async {
    if (targetDevice != null) {
      await targetDevice!.disconnect();
      targetDevice = null;
      writeCharacteristic = null;
    }
  }

Future<void> sendResetSignal() async {
    // MISSING 3: Ensure the device is connected and characteristic is found
    if (targetDevice == null || writeCharacteristic == null) {
      print("Cannot send reset: Device not connected or characteristic not found.");
      return;
    }

    try {
      // Encode "R" to bytes (UTF-8)
      List<int> bytes = utf8.encode("R"); 
      
      // Write to the device. 
      // Note: Use 'withoutResponse: true' if your ESP32 is set to 
      // BLECharacteristic::PROPERTY_WRITE_NR
      await writeCharacteristic!.write(bytes, withoutResponse: false);
      
      print("Reset signal 'R' sent to ESP32 successfully.");
    } catch (e) {
      print("Error sending reset signal: $e");
    }
  }
}