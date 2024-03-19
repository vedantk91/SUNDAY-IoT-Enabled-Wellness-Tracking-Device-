import 'package:flutter/material.dart';


import 'package:firebase_database/firebase_database.dart';


import 'package:fl_chart/fl_chart.dart';


import 'package:intl/intl.dart';


class UVDataPoint {

  final int index;


  final double value;


  final DateTime timestamp;


  UVDataPoint(this.index, this.value, this.timestamp);

}


class UVDisplayPage extends StatefulWidget {

  @override

  _UVDisplayPageState createState() => _UVDisplayPageState();

}


class _UVDisplayPageState extends State<UVDisplayPage> {

  final databaseRef = FirebaseDatabase.instance.ref('receivedData');


  List<UVDataPoint> uvData = [];


  int dataIndex = 0;


  double cumulativeUV = 0;


  static const double uvRequirement = 350; // Cumulative UV requirement


  String uvInference = '';


  @override

  void initState() {

    super.initState();


    fetchUVData();

  }


  void fetchUVData() {

    databaseRef.onValue.listen((DatabaseEvent event) {

      final data = Map<dynamic, dynamic>.from(event.snapshot.value as Map);


      if (data.isNotEmpty) {

        setState(() {

          uvData.clear();


          dataIndex = 0;


          cumulativeUV = 0;


          data.forEach((key, value) {

            final timestamp =

                DateTime.fromMillisecondsSinceEpoch(value['timestamp']);


            final uvIndex = double.parse(RegExp(r'UV Index: (\d+),')

                    .firstMatch(value['data'])

                    ?.group(1) ??

                '0');


            cumulativeUV += uvIndex;


            uvData.add(UVDataPoint(dataIndex++, uvIndex, timestamp));

          });


          if (cumulativeUV < uvRequirement) {

            uvInference =

                'UV exposure is below the recommended level. Consider spending time in the afternoon from 2pm to 3pm.';

          } else {

            uvInference = 'UV exposure requirement is met. Good job!';

          }

        });

      }

    });

  }


  @override

  Widget build(BuildContext context) {

    return Scaffold(

      appBar: AppBar(title: Text('UV Index Chart')),

      body: Column(

        children: [

          Expanded(

            flex: 3,

            child: uvData.isNotEmpty

                ? UVLineChart(uvData)

                : Center(child: const Text('Loading UV data...')),

          ),

          Padding(

            padding: const EdgeInsets.all(10.0),

            child: Text(

              uvInference,

              style: TextStyle(

                  fontSize: 16,

                  fontWeight: FontWeight.bold,

                  color: Colors.orange),

            ),

          ),

        ],

      ),

    );

  }

}


class UVLineChart extends StatelessWidget {

  final List<UVDataPoint> uvData;


  UVLineChart(this.uvData);


  @override

  Widget build(BuildContext context) {

    return AspectRatio(

      aspectRatio: 2,

      child: LineChart(

        LineChartData(

          lineBarsData: [

            LineChartBarData(

              spots: uvData

                  .map((dp) => FlSpot(dp.index.toDouble(), dp.value))

                  .toList(),

              isCurved: true,

              barWidth: 2,

              color: Colors.orange,

              dotData: FlDotData(show: false),

              belowBarData: BarAreaData(show: false),

            ),

          ],


          minY: 0,


          maxY: 12, // Considering UV index might go up to 11+


          titlesData: FlTitlesData(

            bottomTitles: AxisTitles(

              sideTitles: SideTitles(

                showTitles: true,

                getTitlesWidget: (value, meta) {

                  if (value.toInt() % 10 == 0) {

                    final date = uvData[value.toInt()].timestamp;


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

              tooltipBgColor: Colors.orangeAccent,

              getTooltipItems: (touchedSpots) {

                return touchedSpots.map((touchedSpot) {

                  final timestamp = uvData[touchedSpot.spotIndex].timestamp;


                  return LineTooltipItem(

                    DateFormat('HH:mm:ss').format(timestamp) +

                        '\nUV Index: ${touchedSpot.y}',

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

