#include "include/wwplugin/wwplugin_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "wwplugin_plugin.h"

void WwpluginPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  wwplugin::WwpluginPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
