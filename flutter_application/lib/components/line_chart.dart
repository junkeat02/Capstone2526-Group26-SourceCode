import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';

class LineChartComponent extends StatelessWidget {
  final List<FlSpot> spots;
  final String label;
  final int input;
  final Color theme;
  final Color background;
  final double height;
  final double width;

  const LineChartComponent({
    super.key,
    required this.spots,
    required this.label,
    required this.input,
    this.theme =  Colors.blueAccent,
    this.background =const Color.fromARGB(255, 3, 56, 100), 
    this.height = 220,
    this.width = 200,
  });

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      height: height,
      width: width,
      child: Container(
        decoration: BoxDecoration(
          color: background,
          borderRadius: BorderRadius.circular(20),
        ),
        child:Stack(
        alignment: Alignment.center,
        children: [
          LineChart(
            LineChartData(
              gridData: FlGridData(show: false),
              titlesData: FlTitlesData(show: false),
              borderData: FlBorderData(show: false),
              lineTouchData: LineTouchData(enabled: false),

              minY: 40,
              maxY: 160,

              lineBarsData: [
                LineChartBarData(
                  spots: spots,
                  isCurved: true,
                  color: Colors.redAccent,
                  barWidth: 2,
                  dotData: FlDotData(show: false),
                  belowBarData: BarAreaData(show: false),
                ),
              ],
            ),
          ),

          // ❤️ CENTER HEART RATE
          Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              Text(
                "$input",
                style: TextStyle(
                  fontSize: 48,
                  fontWeight: FontWeight.bold,
                  color: theme,
                ),
              ),
              Text(
                label,
                style: TextStyle(
                  fontSize: 16,
                  color: theme,
                ),
              ),
            ],
          ),
        ],
      ),
      )
    );
  }
}
