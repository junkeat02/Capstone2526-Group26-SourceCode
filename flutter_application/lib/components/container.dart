import 'package:flutter/material.dart';

class DataContainer extends StatelessWidget {
  final Widget child;
  final Color background;
  final double horizontalPadding;
  final double verticalPadding;

  const DataContainer({
    super.key,
    required this.child,
    this.background = const Color.fromARGB(255, 3, 56, 100),
    this.horizontalPadding = 60,
    this.verticalPadding = 20,
  });

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Container(
        padding: EdgeInsets.symmetric(vertical: verticalPadding, horizontal: horizontalPadding),
        decoration: BoxDecoration(
          color: background,
          borderRadius: BorderRadius.circular(20),
        ),
        child: child,

      ),
    );
  }
}