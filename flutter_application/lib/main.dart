import 'package:flutter/material.dart';
import 'pages/login_page.dart';

void main() {
  runApp(const FovianApp());
}

class FovianApp extends StatelessWidget {
  const FovianApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      theme: ThemeData.dark(),
      home: const LoginPage(),
    );
  }
}
