import 'package:flutter/services.dart';

class Core {
  static const MethodChannel _channel = MethodChannel('wwplugin');

  Future<List<String>> getActiveApplicationList() async {
    final List<dynamic>? applications =
        await _channel.invokeListMethod('getActiveApplicationList');
    List<String> appList =
        applications?.toSet().map((dynamic app) => app as String).toList() ??
            [];
    appList = appList.map((String app) {
      if (app == "ApplicationFrameHost") {
        return 'Settings';
      } else if (app == 'WindowsTerminal.exe') {
        return 'Termials';
      } else {
        return app;
      }
    }).toList();
    return appList;
  }
}
