import 'package:flutter/material.dart';

import 'package:firebase_database/firebase_database.dart';

import 'package:fl_chart/fl_chart.dart';

import 'package:intl/intl.dart';


class DataPoint {

  final int index;

  final double value;

  final DateTime timestamp;


  DataPoint(this.index, this.value, this.timestamp);

}


class AqiDisplayPage extends StatefulWidget {

  @override

  _AqiDisplayPageState createState() => _AqiDisplayPageState();

}


class _AqiDisplayPageState extends State<AqiDisplayPage> {

  final databaseRef = FirebaseDatabase.instance.ref('receivedData');

  List<DataPoint> aqiData = [];

  int dataIndex = 0;

  List<String> aqiWarnings = []; // List to store warnings


  @override

  void initState() {

    super.initState();

    fetchAqiData();

  }


  void fetchAqiData() {

    databaseRef.onValue.listen((DatabaseEvent event) {

      final data = Map<dynamic, dynamic>.from(event.snapshot.value as Map);

      if (data.isNotEmpty) {

        setState(() {

          aqiData.clear();

          dataIndex = 0;

          aqiWarnings.clear(); // Clear previous warnings


          data.forEach((key, value) {

            final timestamp =
                DateTime.fromMillisecondsSinceEpoch(value['timestamp']);

            final aqi = double.parse(
                RegExp(r'AQI: (\d+),').firstMatch(value['data'])?.group(1) ??
                    '0');


            if (aqi >= 4) {

              String warning =
                  'The air quality you encountered at ${DateFormat('HH:mm:ss').format(timestamp)} was not good (AQI: $aqi).';

              aqiWarnings.add(warning);

            }


            aqiData.add(DataPoint(dataIndex++, aqi, timestamp));

          });

        });

      }

    });

  }


  @override

  Widget build(BuildContext context) {

    return Scaffold(

      appBar: AppBar(title: Text('AQI Chart')),

      body: Column(

        children: [

          Expanded(

            flex: 3, // Increase the size of the chart

            child: aqiData.isNotEmpty

                ? AqiLineChart(aqiData)

                : Center(child: const Text('Loading AQI data...')),

          ),

          if (aqiWarnings.isNotEmpty)

            Expanded(

              flex: 1, // Allocate space for warnings

              child: ListView(

                children: aqiWarnings.map((warning) => Text(warning)).toList(),

              ),

            ),

        ],

      ),

    );

  }

}


class AqiLineChart extends StatelessWidget {

  final List<DataPoint> aqiData;


  AqiLineChart(this.aqiData);


  @override

  Widget build(BuildContext context) {

    return AspectRatio(

      aspectRatio: 2,

      child: LineChart(

        LineChartData(

          lineBarsData: [

            LineChartBarData(

              spots: aqiData
                  .map((dp) => FlSpot(dp.index.toDouble(), dp.value))
                  .toList(),

              isCurved: true,

              barWidth: 2,

              color: Colors.blue,

              dotData: FlDotData(show: false),

              belowBarData: BarAreaData(show: false),

            ),

          ],

          minY: 0,

          maxY: 6,

          titlesData: FlTitlesData(

            bottomTitles: AxisTitles(

              sideTitles: SideTitles(

                showTitles: true,

                getTitlesWidget: (value, meta) {

                  if (value.toInt() % 10 == 0) {

                    final date = aqiData[value.toInt()].timestamp;

                    return Padding(

                      padding: const EdgeInsets.only(top: 10.0),

                      child: Text(DateFormat('HH:mm:ss').format(date),

                          style: TextStyle(fontSize: 10)),

                    );

                  } else {

                    return Text('');

                  }

                },

                interval: 1,

              ),

            ),

            leftTitles: AxisTitles(

              sideTitles: SideTitles(

                showTitles: true,

                getTitlesWidget: (value, meta) {

                  return Text(value.toString(), style: TextStyle(fontSize: 10));

                },

                interval: 1,

              ),

            ),

          ),

          gridData: FlGridData(show: true),

          borderData: FlBorderData(show: true),

          lineTouchData: LineTouchData(

            touchTooltipData: LineTouchTooltipData(

              tooltipBgColor: Colors.blueAccent,

              getTooltipItems: (touchedSpots) {

                return touchedSpots.map((touchedSpot) {

                  final timestamp = aqiData[touchedSpot.spotIndex].timestamp;

                  return LineTooltipItem(

                    DateFormat('HH:mm:ss').format(timestamp) +

                        '\nAQI: ${touchedSpot.y}',

                    const TextStyle(color: Colors.white),

                  );

                }).toList();

              },

            ),

          ),

        ),

      ),

    );

  }

}

