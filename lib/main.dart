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

  Future<void> getInstalledApps() async {
    try {
      final List<dynamic> apps =
          await platform.invokeMethod('getInstalledApps');
      print("res: $apps");
    } on PlatformException catch (e) {
      print("Failed to get installed apps: '${e.message}'.");
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
            onPressed: getInstalledApps,
            child: const Text('Get Installed Apps'),
          ),
        ),
      ),
    );
  }
}
