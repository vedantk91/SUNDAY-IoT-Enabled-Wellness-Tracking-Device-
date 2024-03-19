
import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:pec_auto_1/AqiDisplayPage.dart';
import 'package:pec_auto_1/LightExposurePage.dart';
import 'package:pec_auto_1/UVDisplayPage.dart';
import 'package:pec_auto_1/firebase_options.dart';
import 'package:pec_auto_1/scan_page.dart';
import 'package:pec_auto_1/settings_page.dart';
import 'package:pec_auto_1/tutorial_page.dart';
import 'package:pec_auto_1/view/home_page.dart';
import 'package:pec_auto_1/welcome_page.dart';
import 'about_page.dart';
import 'controllers/bluetooth_controller.dart';
import 'controllers/connection_page.dart';
import 'dashboard_page.dart';
import 'data_display_page.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform, // Initialize Firebase
  );
  setupController();
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'SUNDAY APP',
      theme: ThemeData(
        primarySwatch: Colors.green,
      ),
      initialRoute: '/',
      routes: {
        '/': (context) => WelcomePage(),
        '/home': (context) => const HomePage(),
        '/dashboard': (context) => DashBoardPage(),
        '/connections': (context) => ConnectionsPage(),
        '/settings': (context) => const SettingPage(),
        // '/scan': (context) => ScanPage(),
        '/tutorial': (context) => const TutorialPage(),
        '/about': (context) => const AboutPage(),
        '/data_display': (context) => DataDisplayPage(),
        '/aqi_display': (context) => AqiDisplayPage(),
        '/uv_display': (context) => UVDisplayPage(),
        '/light_exposure': (context) => LightExposurePage(),
      },
    );
  }
}

void setupController() {
  Get.put(BluetoothController());
}
