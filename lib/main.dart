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
  static const MethodChannel _channel = MethodChannel('wwplugin');
  List<String> _applications = [];

  Future<void> getActiveApplicationList() async {
    try {
      // Invoke the method to get the application list
      final List<dynamic>? applications =
          await _channel.invokeListMethod('getActiveApplicationList');

      // Update the _applications list and refresh the UI
      setState(() {
        _applications = applications?.map((e) => e.toString()).toList() ?? [];
      });
      for (String app in _applications) {
        print(app);
      }
      print("-------- total ${_applications.length} ------------\n");
    } on PlatformException catch (e) {
      print("Failed to get active application list: '${e.message}'.");
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Windows Plugin Using Now'),
        ),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              ElevatedButton(
                onPressed: getActiveApplicationList,
                child: const Text('Refresh active application list'),
              ),
              Expanded(
                // Use ListView.builder to create a scrollable list
                child: ListView.builder(
                  itemCount: _applications.length,
                  itemBuilder: (context, index) {
                    // Display each application in a ListTile
                    return ListTile(
                      title: Text(_applications[index]),
                    );
                  },
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
