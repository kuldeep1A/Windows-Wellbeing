#include "wwplugin_plugin.h"

#include <windows.h>
#include <VersionHelpers.h>
#include <psapi.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <string>
#include <vector>

#pragma comment(lib, "psapi.lib")

namespace wwplugin
{
  struct AppUsageDetails
  {
    std::wstring appName; // Use wstring for wide characters
    DWORD pid;
    ULONGLONG totalTime;
    FILETIME creationTime;
    FILETIME exitTime;
  };


  std::vector<AppUsageDetails> GetLast24HoursUsageAppsDetails()
  {
    std::vector<AppUsageDetails> usageAppsDetails;
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime); // Get current system time

    // Subtract 24 hours from the current time
    ULARGE_INTEGER uli;
    uli.LowPart = fileTime.dwLowDateTime;
    uli.HighPart = fileTime.dwHighDateTime;
    uli.QuadPart -= 24LL * 60LL * 60LL * 10000000LL;
    fileTime.dwLowDateTime = uli.LowPart;
    fileTime.dwHighDateTime = uli.HighPart;

    DWORD processes[1024];
    DWORD needed;
    if (EnumProcesses(processes, sizeof(processes), &needed))
    {
      DWORD numProcesses = needed / sizeof(DWORD);
      for (DWORD i = 0; i < numProcesses; i++)
      {
        DWORD pid = processes[i];
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (hProcess)
        {
          FILETIME creationTime, exitTime, kernelTime, userTime;
          if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime))
          {
            // Check if the process was created in the last 24 hours
            if (CompareFileTime(&creationTime, &fileTime) > 0)
            {
              AppUsageDetails details;
              WCHAR szProcessName[MAX_PATH] = L"<unknown>"; // Use WCHAR for wide characters
              HMODULE hMod;
              DWORD cbNeeded;
              if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
              {
                GetModuleBaseNameW(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(WCHAR));
                details.appName = szProcessName; // Directly assign the WCHAR string
                details.pid = pid;
                ULONGLONG totalTime = (uli.QuadPart - ((ULONGLONG)exitTime.dwHighDateTime << 32 | exitTime.dwLowDateTime)) / 10000000LL;
                details.totalTime = totalTime;
                details.creationTime = creationTime;
                details.exitTime = exitTime;
                usageAppsDetails.push_back(details);
              }
            }
          }
          CloseHandle(hProcess);
        }
      }
    }
    return usageAppsDetails;
  }

  void WwpluginPlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar)
  {
    auto channel = std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
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

  std::string WideStringToNarrowString(const std::wstring &wstr)
  {
    if (wstr.empty())
      return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
  }

  void WwpluginPlugin::HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                                        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    // The original line with the error:
    // const std::wstring &methodName = method_call.method_name(); // This is incorrect if method_name() returns std::string or std::u8string

    // Corrected approach: Convert std::string (or std::u8string) to std::wstring
    std::string narrowMethodName = method_call.method_name(); // Assuming method_name() returns std::string
    std::wstring methodName(narrowMethodName.begin(), narrowMethodName.end());

    // If method_name() returns std::u8string (in newer versions), you might need to adjust the conversion process accordingly

    const wchar_t *targetMethodName = L"getLast24HoursUsageAppsDetails";

    bool isTargetMethod = (methodName == targetMethodName); // Simplified comparison

    if (isTargetMethod)
    {
      auto details = GetLast24HoursUsageAppsDetails();
      std::vector<flutter::EncodableValue> encodableDetails;

      for (const auto &detail : details)
      {
        flutter::EncodableMap appDetails;
        appDetails[flutter::EncodableValue("appName")] = flutter::EncodableValue(WideStringToNarrowString(detail.appName));
        appDetails[flutter::EncodableValue(L"pid")] = flutter::EncodableValue(static_cast<int>(detail.pid));
        appDetails[flutter::EncodableValue(L"totalTime")] = flutter::EncodableValue(static_cast<int64_t>(detail.totalTime));
        appDetails[flutter::EncodableValue(L"creationTime")] = flutter::EncodableValue(detail.creationTime.dwLowDateTime | (static_cast<int64_t>(detail.creationTime.dwHighDateTime) << 32));
        appDetails[flutter::EncodableValue(L"exitTime")] = flutter::EncodableValue(detail.exitTime.dwLowDateTime | (static_cast<int64_t>(detail.exitTime.dwHighDateTime) << 32));

        // Debug print liner
        std::wstring debugMessage = L"App Name: " + detail.appName + L", PID: " + std::to_wstring(detail.pid) + L"\n";
        OutputDebugString(debugMessage.c_str());
        encodableDetails.push_back(flutter::EncodableValue(appDetails));
      }

      result->Success(flutter::EncodableValue(encodableDetails));
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace wwplugin
