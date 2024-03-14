import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});
  final String title;

  @override
  State<MyHomePage> createState() => _MyAppState();
}

class _MyAppState extends State<MyHomePage> {
  static const platform = MethodChannel('wwplugin');

  Future<void> getLast24HoursUsageAppsDetails() async {
    try {
      final List<dynamic> apps =
          await platform.invokeMethod('getLast24HoursUsageAppsDetails');
      for (var app in apps) {
        // Assuming appName, pid, totalTime, creationTime, exitTime are keys in the map
        final String appName = app["appName"] ?? "Unknown";
        final int pid = app["pid"] ?? -1;
        final int totalTime = app["totalTime"] ?? -1;
        final int creationTimeRaw = app["creationTime"] ?? -1;
        final int exitTimeRaw = app["exitTime"] ?? -1;

        // Convert raw file time to DateTime (if applicable)
        // Note: Conversion depends on how you're representing time. The following is a basic example.
        final DateTime creationTime = DateTime.fromMillisecondsSinceEpoch(
            creationTimeRaw ~/ 10,
            isUtc: true);
        final DateTime exitTime =
            DateTime.fromMillisecondsSinceEpoch(exitTimeRaw ~/ 10, isUtc: true);

        print('App Name: $appName');
        print('PID: $pid');
        print(
            'Total Time: $totalTime seconds'); // Assuming totalTime is in seconds
        print('Creation Time: $creationTime');
        print('Exit Time: $exitTime');
        print('-------------------------'); // Separator for readability
      }
    } on PlatformException catch (e) {
      print("Failed to get app usage details: '${e.message}'.");
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Windows Apps List Example'),
        ),
        body: Center(
          child: ElevatedButton(
            onPressed: getLast24HoursUsageAppsDetails,
            child: const Text('Get Installed Apps'),
          ),
        ),
      ),
    );
  }
}
