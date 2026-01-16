import 'package:url_launcher/url_launcher.dart';
import 'package:geolocator/geolocator.dart';
import 'package:flutter_phone_direct_caller/flutter_phone_direct_caller.dart';

class EmergencyService {
  static Future<void> triggerEmergency(String phoneNumber) async {
    
    // 1. Get Location
    String locationMsg = "";
    try {
      Position position = await Geolocator.getCurrentPosition(
          desiredAccuracy: LocationAccuracy.high);
      
      // FIXED: Official Google Maps URL format for pins
      // Use https://www.google.com/maps/search/?api=1&query=lat,long
      locationMsg = "\nLocation: https://www.google.com/maps/search/?api=1&query=${position.latitude},${position.longitude}";
    } catch (e) {
      locationMsg = "\nLocation: Not available.";
    }

    // 2. Make the Direct Call
    // This starts the phone call immediately
    await FlutterPhoneDirectCaller.callNumber(phoneNumber);

    // 3. Wait for 2.5 seconds
    // This allows the call to stabilize before switching screens to WhatsApp
    await Future.delayed(const Duration(milliseconds: 2500));

    // 4. Send WhatsApp Message
    // WhatsApp requires numbers WITHOUT '+' or spaces (e.g., 60123456789)
    String cleanNumber = phoneNumber.replaceAll(RegExp(r'[\+\s]'), '');
    String message = "EMERGENCY ALERT: FOVIAN has detected a fall! $locationMsg";

    // Use the universal wa.me link for better compatibility
    final Uri whatsappUri = Uri.parse(
      "https://wa.me/$cleanNumber?text=${Uri.encodeComponent(message)}"
    );

    if (await canLaunchUrl(whatsappUri)) {
      // Launch in an external application (the WhatsApp app)
      await launchUrl(whatsappUri, mode: LaunchMode.externalNonBrowserApplication);
    } else {
      print("Could not launch WhatsApp.");
    }
  }
}