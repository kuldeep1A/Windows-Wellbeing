import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'core/core.dart';

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
  Core core = Core();
  List<String> _applications = [];

  Future<void> getCore() async {
    final List<String> applications = await core.getActiveApplicationList();

    setState(() {
      _applications = applications;
    });

    for (String app in _applications) {
      print("${app.length} app $app ");
    }

    print("---\n\n");
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
                onPressed: getCore,
                child: Text(
                    'Refresh active application list ${_applications.length}'),
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
