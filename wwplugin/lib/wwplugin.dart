
import 'wwplugin_platform_interface.dart';

class Wwplugin {
  Future<String?> getPlatformVersion() {
    return WwpluginPlatform.instance.getPlatformVersion();
  }
}
