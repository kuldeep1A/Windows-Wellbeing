import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'wwplugin_platform_interface.dart';

/// An implementation of [WwpluginPlatform] that uses method channels.
class MethodChannelWwplugin extends WwpluginPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('wwplugin');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
