#include "wwplugin_plugin.h"
#include <windows.h>
#include <vector>

namespace wwplugin
{
  struct AppInfo
  {
    std::string displayName;
  };
  std::vector<AppInfo> GetInstalledApplications()
  {
    std::vector<AppInfo> installedApps;
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
      char appName[1024];
      DWORD appNameLen, index = 0;

      while ((appNameLen = sizeof(appName), RegEnumKeyExA(hKey, index++, appName, &appNameLen, nullptr, nullptr, nullptr, nullptr)) != ERROR_NO_MORE_ITEMS)
      {
        AppInfo appInfo;
        appInfo.displayName = std::string(appName);

        installedApps.push_back(appInfo);
      }
      RegCloseKey(hKey);  
    }
    return installedApps;
  }

  // static
  void WwpluginPlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar)
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
        // Create a map to represent each application with its information
        std::map<std::string, flutter::EncodableValue> appInfoMap;
        appInfoMap["displayName"] = flutter::EncodableValue(app.displayName);
        // Add more fields to the map if additional information is available

        encodableApps.push_back(flutter::EncodableValue(appInfoMap));
      }

      result->Success(flutter::EncodableValue(encodableApps));
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace wwplugin
