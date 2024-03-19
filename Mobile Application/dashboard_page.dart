import 'package:flutter/material.dart';
import 'package:pec_auto_1/AqiDisplayPage.dart'; // Import AqiDisplayPage

class DashBoardPage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Dashboard'),
      ),
      body: Center(
        child: GridView.count(
          crossAxisCount: 2,
          children: [
            DashboardOption(
                icon: Icons.settings, label: 'Settings', route: '/settings'),
            DashboardOption(
                icon: Icons.bluetooth,
                label: 'Connections',
                route: '/connections'),
            DashboardOption(
                icon: Icons.document_scanner_outlined,
                label: 'Scan',
                route: '/scan'),
            DashboardOption(
                icon: Icons.school, label: 'Tutorial', route: '/tutorial'),
            DashboardOption(icon: Icons.info, label: 'About', route: '/about'),
            DashboardOption(
                icon: Icons.show_chart,
                label: 'AQI Chart',
                route: '/aqiChart'), // Updated for AQI Chart
            DashboardOption(
                icon: Icons.wb_sunny, label: 'UV Chart', route: '/uv_display'),
            DashboardOption(
                icon: Icons.lightbulb_outline,
                label: 'Light Exposure',
                route: '/light_exposure'), // New option for Light Exposure
          ],
        ),
      ),
    );
  }
}

class DashboardOption extends StatelessWidget {
  final IconData icon;
  final String label;
  final String route;

  const DashboardOption(
      {super.key,
      required this.icon,
      required this.label,
      required this.route});

  @override
  Widget build(BuildContext context) {
    return InkWell(
      onTap: () {
        if (route == '/aqiChart') {
          Navigator.push(context,
              MaterialPageRoute(builder: (context) => AqiDisplayPage()));
        } else {
          Navigator.pushNamed(context, route);
        }
      },
      child: Card(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(icon, size: 50, color: Colors.blue),
            const SizedBox(height: 10),
            Text(label,
                style: const TextStyle(
                    color: Colors.blueAccent, fontWeight: FontWeight.bold)),
          ],
        ),
      ),
    );
  }
}
