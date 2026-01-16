import 'package:flutter_local_notifications/flutter_local_notifications.dart';

class NotificationService {
  final _plugin = FlutterLocalNotificationsPlugin();

  Future<void> init() async {
    const android = AndroidInitializationSettings('@mipmap/ic_launcher');
    await _plugin.initialize(const InitializationSettings(android: android));
  }

  Future<void> showEmergency(String title, String body) async {
    const details = AndroidNotificationDetails('emergency', 'Alerts', importance: Importance.max, priority: Priority.high);
    await _plugin.show(1, title, body, const NotificationDetails(android: details));
  }
}