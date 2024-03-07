import 'package:flutter_test/flutter_test.dart';
import 'package:wwplugin/wwplugin.dart';
import 'package:wwplugin/wwplugin_platform_interface.dart';
import 'package:wwplugin/wwplugin_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockWwpluginPlatform
    with MockPlatformInterfaceMixin
    implements WwpluginPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final WwpluginPlatform initialPlatform = WwpluginPlatform.instance;

  test('$MethodChannelWwplugin is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelWwplugin>());
  });

  test('getPlatformVersion', () async {
    Wwplugin wwpluginPlugin = Wwplugin();
    MockWwpluginPlatform fakePlatform = MockWwpluginPlatform();
    WwpluginPlatform.instance = fakePlatform;

    expect(await wwpluginPlugin.getPlatformVersion(), '42');
  });
}
