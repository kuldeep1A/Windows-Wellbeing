#include "wwplugin_plugin.h"
#include <VersionHelpers.h>
#include <windows.h>
#include <Psapi.h>
#include <iostream>
#include <unordered_map>

namespace wwplugin
{
  std::unordered_map<std::string, FILETIME> processStartTimes;
  std::string ConvertTCHARToString(const TCHAR *tcharString)
  {
#ifdef UNICODE
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, tcharString, -1, nullptr, 0, nullptr, nullptr);
    std::string convertedName(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, tcharString, -1, &convertedName[0], size_needed, nullptr, nullptr);
#else
    std::string convertedName(tcharString);
#endif
    return convertedName;
  }

  void SampleProcesses()
  {
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
            std::string appName = ConvertTCHARToString(processName);

            auto it = processStartTimes.find(appName);

            if (it == processStartTimes.end())
            {
              FILETIME startTime;
              GetProcessTimes(hProcess, &startTime, nullptr, nullptr, nullptr);

              processStartTimes[appName] = startTime;
            }
            else
            {
              FILETIME now, creationTime, exitTime, kernelTime, userTime;
              GetSystemTimeAsFileTime(&now);
              GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime);

              ULARGE_INTEGER start, current;
              start.LowPart = it->second.dwLowDateTime;
              start.HighPart = it->second.dwHighDateTime;
              current.LowPart = now.dwLowDateTime;
              current.HighPart = now.dwHighDateTime;

              ULONGLONG duration = current.QuadPart - start.QuadPart;

              duration /= 10000000;

              std::cout << "Application: " << appName << ", Usage Time: " << duration << " seconds\n";
            }
          }
          CloseHandle(hProcess);
        }
      }
    }
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
      SampleProcesses();
      result->Success(flutter::EncodableValue("Sampled processes."));
    }
    else
    {
      result->NotImplemented();
    }
  }

}