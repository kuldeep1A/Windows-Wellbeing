// wwplugin_plugin.cpp

#include "wwplugin_plugin.h"
#include <VersionHelpers.h>
#include <windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#include <memory>
#include <sstream>
#include <vector>

namespace wwplugin
{
  std::vector<std::string> WwpluginPlugin::GetRunningProcessNames()
  {
    std::vector<std::string> processNames;

    DWORD processesArray[1024];
    DWORD bytesReturned;

    if (EnumProcesses(processesArray, sizeof(processesArray), &bytesReturned))
    {
      size_t count = bytesReturned / sizeof(DWORD);
      for (size_t i = 0; i < count; ++i)
      {
        DWORD pid = processesArray[i];
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

        if (hProcess != nullptr)
        {
          TCHAR processName[MAX_PATH];
          if (GetModuleBaseName(hProcess, nullptr, processName, sizeof(processName) / sizeof(TCHAR)))
          {
            // Convert TCHAR to std::string
            std::string convertedName;
#ifdef UNICODE
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, processName, -1, nullptr, 0, nullptr, nullptr);
            convertedName.resize(size_needed);
            WideCharToMultiByte(CP_UTF8, 0, processName, -1, &convertedName[0], size_needed, nullptr, nullptr);
#else
            convertedName = processName;
#endif
            processNames.push_back(convertedName);
          }
          CloseHandle(hProcess);
        }
      }
    }
    return processNames;
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
      auto processNames = GetRunningProcessNames();

      flutter::EncodableList encodableNames;
      for (const auto &name : processNames)
      {
        encodableNames.push_back(flutter::EncodableValue(name));
      }

      result->Success(flutter::EncodableValue(encodableNames));
    }
    else
    {
      result->NotImplemented();
    }
  }

}