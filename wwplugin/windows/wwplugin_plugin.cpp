#include "wwplugin_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

namespace wwplugin
{

  std::vector<std::string> GetInstalledApplications()
  {
    std::vector<std::string> installedApps;
    HKEY hKey;
    // Open the key where installed applications are listed under Windows
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
      char appName[1024];
      DWORD appNameLen, index = 0;

      // Enumerate all applications listed under the uninstall key
      while ((appNameLen = sizeof(appName), RegEnumKeyExA(hKey, index++, appName, &appNameLen, nullptr, nullptr, nullptr, nullptr)) != ERROR_NO_MORE_ITEMS)
      {
        installedApps.push_back(std::string(appName));
      }
      RegCloseKey(hKey);
    }
    return installedApps;
  }
  // static
  void WwpluginPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "wwplugin",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<WwpluginPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
  }

  WwpluginPlugin::WwpluginPlugin() {}

  WwpluginPlugin::~WwpluginPlugin() {}

  void WwpluginPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    if (method_call.method_name().compare("getInstalledApps") == 0)
    {
      auto apps = GetInstalledApplications();
      std::vector<flutter::EncodableValue> encodableApps;

      for (const auto &app : apps)
      {
        encodableApps.push_back(flutter::EncodableValue(app));
      }

      result->Success(flutter::EncodableValue(encodableApps));
    }
    else
    {
      result->NotImplemented();
    }
  }
} // namespace wwplugin
