import 'package:flutter/material.dart';

import 'package:get/get.dart';

import '../controllers/bluetooth_controller.dart';


class DataDisplayPage extends StatelessWidget {

  final BluetoothController bluetoothController = Get.find();


  // Function to parse the AQI from the data string

  String parseAqi(String dataString) {

    final aqiRegex = RegExp(r'AQI:\s*(\d+)');

    final match = aqiRegex.firstMatch(dataString);

    return match != null ? match.group(1) ?? 'N/A' : 'N/A';

  }


  @override

  Widget build(BuildContext context) {

    return Scaffold(

      appBar: AppBar(

        title: const Text('Bluetooth Data'),

      ),

      body: Center(

        child: Obx(() {

          final List<String> data =

              bluetoothController.allReceivedData.toList();

          return ListView.builder(

            itemCount: data.length,

            itemBuilder: (context, index) {

              // Parse the AQI value from each data string

              String aqiValue = parseAqi(data[index]);

              return ListTile(

                // Display the parsed AQI value

                title: Text('AQI: $aqiValue'),

              );

            },

          );

        }),

      ),

    );

  }

}

