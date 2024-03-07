import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'wwplugin_method_channel.dart';

abstract class WwpluginPlatform extends PlatformInterface {
  /// Constructs a WwpluginPlatform.
  WwpluginPlatform() : super(token: _token);

  static final Object _token = Object();

  static WwpluginPlatform _instance = MethodChannelWwplugin();

  /// The default instance of [WwpluginPlatform] to use.
  ///
  /// Defaults to [MethodChannelWwplugin].
  static WwpluginPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [WwpluginPlatform] when
  /// they register themselves.
  static set instance(WwpluginPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
