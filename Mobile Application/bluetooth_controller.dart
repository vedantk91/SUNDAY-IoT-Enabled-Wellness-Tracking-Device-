import 'dart:async';
import 'dart:convert';
import 'dart:typed_data';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:get/get.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:permission_handler/permission_handler.dart';

enum DeviceConnectionStatus { connecting, connected, notConnected }

class BluetoothController extends GetxController {
  final FlutterBluetoothSerial bluetooth = FlutterBluetoothSerial.instance;
  final FirebaseDatabase database =
      FirebaseDatabase.instance; // Firebase instance

  final RxSet<BluetoothDevice> devicesList = <BluetoothDevice>{}.obs;
  final RxMap<BluetoothDevice, DeviceConnectionStatus> connectionStatus =
      <BluetoothDevice, DeviceConnectionStatus>{}.obs;

  String receivedString = "";

  final Rx<String> receivedData = ''.obs;
  final RxList<String> allReceivedData = <String>[].obs;

  var allDataReceived = false.obs;

  @override
  void onInit() {
    super.onInit();
    initializeBluetooth();
  }

  Future<void> initializeBluetooth() async {
    try {
      await FlutterBluetoothSerial.instance.requestEnable();
    } catch (e) {
      print('Error enabling Bluetooth: $e');
    }
  }

  Future<void> scanDevices() async {
    devicesList.clear();
    await [
      Permission.bluetoothConnect,
      Permission.bluetoothScan,
      Permission.bluetoothAdvertise
    ].request();

    try {
      bool? isDiscovering = await bluetooth.isDiscovering;
      if (isDiscovering != null && isDiscovering) {
        await bluetooth.cancelDiscovery();
      }

      if (await Permission.bluetoothScan.request().isGranted) {
        bluetooth.startDiscovery().listen((result) {
          BluetoothDevice device = result.device;
          devicesList.add(device);
        });

        await Future.delayed(const Duration(seconds: 25));
        await bluetooth.cancelDiscovery();
      } else {
        print('Bluetooth Scan permission denied');
      }
    } catch (e) {
      print('Error scanning for devices: $e');
    }
  }

  Future<void> connectToDevice(BluetoothDevice device) async {
    try {
      await bluetooth.cancelDiscovery();

      connectionStatus[device] = DeviceConnectionStatus.connecting;

      BluetoothConnection connection =
          await BluetoothConnection.toAddress(device.address);

      connectionStatus[device] = DeviceConnectionStatus.connected;

      connection.input?.listen((Uint8List data) {
        onDataReceived(data);
      });

      print('Connected to device: ${device.name}');
    } catch (e) {
      print('Error connecting to device: ${device.name}: $e');
      connectionStatus[device] = DeviceConnectionStatus.notConnected;
    }
  }

  void onDataReceived(Uint8List data) {
    // Convert the received data into a string
    String stringData = utf8.decode(data);
    receivedData(stringData);

    // Add the string data to the list of all received data
    allReceivedData.add(stringData);

    // Send data to Firebase Realtime Database
    sendDataToFirebase(stringData);
  }

  void sendDataToFirebase(String data) {
    // Get a reference to the 'receivedData' node in the database
    DatabaseReference dataRef = database.ref('receivedData').push();

    dataRef.set({
      'data': data,
      'timestamp': ServerValue.timestamp, // Firebase server timestamp
    });
  }

  @override
  void onClose() {
    super.onClose();
  }

  List<double> convertCsvToDoubleList(String csvContent) {
    List<String> values = csvContent.split(',');
    return values.map((value) => double.tryParse(value) ?? 0.0).toList();
  }
}
