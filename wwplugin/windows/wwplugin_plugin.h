#ifndef FLUTTER_PLUGIN_WWPLUGIN_PLUGIN_H_
#define FLUTTER_PLUGIN_WWPLUGIN_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace wwplugin {

class WwpluginPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  WwpluginPlugin();

  virtual ~WwpluginPlugin();

  // Disallow copy and assign.
  WwpluginPlugin(const WwpluginPlugin&) = delete;
  WwpluginPlugin& operator=(const WwpluginPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace wwplugin

#endif  // FLUTTER_PLUGIN_WWPLUGIN_PLUGIN_H_
