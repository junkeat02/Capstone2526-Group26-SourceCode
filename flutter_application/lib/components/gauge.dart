import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';

class SpO2GaugeComponent extends StatelessWidget {
  final double spo2;
  final double size;

  const SpO2GaugeComponent({
    super.key,
    required this.spo2,
    this.size = 220,
  });

  Color _getColor() {
    if (spo2 >= 95) return Colors.greenAccent;
    if (spo2 >= 90) return Colors.orangeAccent;
    return Colors.redAccent;
  }

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: size,
      height: size,
      child: SfRadialGauge(
        axes: [
          RadialAxis(
            minimum: 0,
            maximum: 100,
            showTicks: false,
            showLabels: false,
            axisLineStyle: AxisLineStyle(
              thickness: 0.15,
              thicknessUnit: GaugeSizeUnit.factor,
              color: Colors.grey.shade800,
            ),
            ranges: [
              GaugeRange(
                startValue: 0,
                endValue: 90,
                color: Colors.redAccent.withValues(alpha: 0.4),
              ),
              GaugeRange(
                startValue: 90,
                endValue: 95,
                color: Colors.orangeAccent.withValues(alpha: 0.4),
              ),
              GaugeRange(
                startValue: 95,
                endValue: 100,
                color: Colors.greenAccent.withValues(alpha: 0.4),
              ),
            ],
            pointers: [
              RangePointer(
                value: spo2,
                width: 0.15,
                sizeUnit: GaugeSizeUnit.factor,
                color: _getColor(),
                cornerStyle: CornerStyle.bothCurve,
              ),
            ],
            annotations: [
              GaugeAnnotation(
                widget: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    Text(
                      "${spo2.toStringAsFixed(1)}%",
                      style: TextStyle(
                        fontSize: 32,
                        fontWeight: FontWeight.bold,
                        color: _getColor(),
                      ),
                    ),
                    const SizedBox(height: 4),
                    const Text(
                      "SpO₂",
                      style: TextStyle(
                        fontSize: 14,
                        color: Colors.white70,
                      ),
                    ),
                  ],
                ),
                angle: 90,
                positionFactor: 0.1,
              )
            ],
          )
        ],
      ),
    );
  }
}
