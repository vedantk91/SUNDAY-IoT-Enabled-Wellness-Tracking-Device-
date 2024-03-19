import 'package:flutter/material.dart';


import 'package:firebase_database/firebase_database.dart';


import 'package:fl_chart/fl_chart.dart';


class LightData {

  final int naturalLightCount;


  final int artificialLightCount;


  LightData(

      {required this.naturalLightCount, required this.artificialLightCount});

}


class LightExposurePage extends StatefulWidget {

  @override

  _LightExposurePageState createState() => _LightExposurePageState();

}


class _LightExposurePageState extends State<LightExposurePage> {

  final databaseRef = FirebaseDatabase.instance.ref('receivedData');


  LightData lightData =

      LightData(naturalLightCount: 0, artificialLightCount: 0);


  @override

  void initState() {

    super.initState();


    fetchLightData();

  }


  void fetchLightData() {

    databaseRef.onValue.listen((DatabaseEvent event) {

      final data = Map<dynamic, dynamic>.from(event.snapshot.value as Map);


      int naturalLight = 0;


      int artificialLight = 0;


      if (data.isNotEmpty) {

        data.forEach((key, value) {

          var parsedData = value['data'].toString();


          if (parsedData.contains('Natural Light: 1')) naturalLight++;


          if (parsedData.contains('Artificial Light: 1')) artificialLight++;

        });


        setState(() {

          lightData = LightData(

              naturalLightCount: naturalLight,

              artificialLightCount: artificialLight);

        });

      }

    });

  }


  @override

  Widget build(BuildContext context) {

    int total = lightData.naturalLightCount + lightData.artificialLightCount;


    double naturalLightPercentage =

        total > 0 ? (lightData.naturalLightCount / total) * 100 : 0;


    double artificialLightPercentage =

        total > 0 ? (lightData.artificialLightCount / total) * 100 : 0;


    double chartSize =

        MediaQuery.of(context).size.width * 0.65; // 65% of screen width


    return Scaffold(

      appBar: AppBar(title: Text('Light Exposure')),

      body: SingleChildScrollView(

        child: Column(

          mainAxisAlignment: MainAxisAlignment.center,

          children: [

            _buildPieChartSection('Your Light Exposure', naturalLightPercentage,

                artificialLightPercentage, chartSize),

            _legendWidget(),

            SizedBox(height: 10),

            _buildPieChartSection('Ideal Light Exposure', 15, 85, chartSize),

            _legendWidget(),

            SizedBox(height: 20),

            _inferenceText(naturalLightPercentage),

          ],

        ),

      ),

    );

  }


  Widget _buildPieChartSection(String title, double naturalLightPercentage,

      double artificialLightPercentage, double chartSize) {

    return Padding(

      padding: const EdgeInsets.symmetric(vertical: 20),

      child: Column(

        children: [

          Text(title,

              style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold)),

          SizedBox(height: 10),

          SizedBox(

            width: chartSize,

            height: chartSize,

            child: PieChart(

              PieChartData(

                sections: [

                  PieChartSectionData(

                    value: naturalLightPercentage,

                    color: Colors.green,

                    title: '${naturalLightPercentage.toStringAsFixed(1)}%',

                    radius: chartSize / 2,

                    titleStyle: TextStyle(

                        fontSize: 16,

                        fontWeight: FontWeight.bold,

                        color: Colors.white),

                  ),

                  PieChartSectionData(

                    value: artificialLightPercentage,

                    color: Colors.red,

                    title: '${artificialLightPercentage.toStringAsFixed(1)}%',

                    radius: chartSize / 2,

                    titleStyle: TextStyle(

                        fontSize: 16,

                        fontWeight: FontWeight.bold,

                        color: Colors.white),

                  ),

                ],

                sectionsSpace: 0,

                centerSpaceRadius: chartSize / 20,

              ),

            ),

          ),

        ],

      ),

    );

  }


  Widget _legendWidget() {

    return Row(

      mainAxisAlignment: MainAxisAlignment.center,

      children: [

        _legendItem('Natural Light', Colors.green),

        SizedBox(width: 10),

        _legendItem('Artificial Light', Colors.red),

      ],

    );

  }


  Widget _legendItem(String title, Color color) {

    return Row(

      children: [

        Container(

          width: 16,

          height: 16,

          color: color,

        ),

        SizedBox(width: 4),

        Text(title),

      ],

    );

  }


  Widget _inferenceText(double naturalLightPercentage) {

    String inferenceText;


    if (naturalLightPercentage >= 15) {

      inferenceText =

          'Great! You have met your natural light exposure requirements.';

    } else {

      inferenceText =

          'Consider spending more time outdoors to meet your natural light exposure requirements.';

    }


    return Text(

      inferenceText,

      textAlign: TextAlign.center,

      style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),

    );

  }

}

