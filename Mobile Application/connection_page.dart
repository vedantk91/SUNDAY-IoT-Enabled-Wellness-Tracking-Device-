import 'package:flutter/material.dart';
import 'package:get/get.dart';
import '../controllers/bluetooth_controller.dart';
import '../scan_page.dart';

class ConnectionsPage extends StatelessWidget {
  final BluetoothController bluetoothController =
      Get.find<BluetoothController>();

  //const ConnectionsPage({super.key});

  get csvData => '';

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: Text('Bluetooth'),
        ),
        body: Column(children: [
          ElevatedButton(
            onPressed: () => bluetoothController.scanDevices(),
            child: const Text('Scan Devices'),
          ),
          SizedBox(height: 16),
          Expanded(
            child: Obx(() {
              final devicesList = bluetoothController.devicesList.toList();
              if (devicesList.isNotEmpty) {
                return ListView.builder(
                  itemCount: devicesList.length,
                  itemBuilder: (context, index) {
                    final device = devicesList[index];
                    final connectionStatus =
                        bluetoothController.connectionStatus[device] ??
                            DeviceConnectionStatus.notConnected;
                    final bool isConnecting =
                        connectionStatus == DeviceConnectionStatus.connecting;
                    final bool isConnected =
                        connectionStatus == DeviceConnectionStatus.connected;

                    return ListTile(
                      title: Text(device.name ?? 'Unknown Device'),
                      subtitle: Text(device.address),
                      trailing: ElevatedButton(
                        onPressed: isConnected || isConnecting
                            ? null
                            : () async {
                                await bluetoothController
                                    .connectToDevice(device);
                              },
                        child: isConnecting
                            ? const CircularProgressIndicator()
                            : Text(isConnected ? 'Connected' : 'Connect'),
                      ),
                    );
                  },
                );
              } else {
                return const Text('No devices found.');
              }
            }),
          )
        ]));
  }
}
