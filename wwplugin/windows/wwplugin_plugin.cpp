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

  std::vector<std::wstring> GetInstalledApplications()
  {
    std::vector<std::wstring> installedApps;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
      wchar_t appName[1024];
      DWORD appNameLen, index = 0;
      while ((appNameLen = sizeof(appName), RegEnumKeyExW(hKey, index++, appName, &appNameLen, nullptr, nullptr, nullptr, nullptr)) != ERROR_NO_MORE_ITEMS)
      {
        installedApps.push_back(std::wstring(appName));
      }
      RegCloseKey(hKey);
    }
    return installedApps;
  }

  std::wstring ConvertTCharToWideString(const TCHAR *tcharString)
  {
#ifdef UNICODE
    return std::wstring(tcharString);
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(tcharString);
#endif
  }

  std::vector<AppUsageDetails> GetLast24HoursUsageAppsDetails()
  {
    std::vector<AppUsageDetails> usageAppsDetails;
    SYSTEMTIME currentTime;
    GetSystemTime(&currentTime);
    FILETIME fileTime;
    SystemTimeToFileTime(&currentTime, &fileTime);
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
            if (CompareFileTime(&creationTime, &fileTime) > 0)
            {
              AppUsageDetails details;
              TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
              HMODULE hMod;
              DWORD cbNeeded;
              if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
              {
                GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                details.appName = ConvertTCharToWideString(szProcessName);
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
        registrar->messenger(), L"wwplugin",
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

  void WwpluginPlugin::HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                                        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    const wchar_t *targetMethodName = L"getLast24HoursUsageAppsDetails";

    const std::wstring &methodName = method_call.method_name();
    size_t methodNameLength = methodName.length();

    bool isTargetMethod = true;
    for (size_t i = 0; i < methodNameLength; ++i)
    {
      if (methodName[i] != targetMethodName[i])
      {
        isTargetMethod = false;
        break;
      }
    }

    if (isTargetMethod && methodNameLength == wcslen(targetMethodName))
    {
      auto details = GetLast24HoursUsageAppsDetails();
      std::vector<flutter::EncodableValue> encodableDetails;

      for (const auto &detail : details)
      {
        flutter::EncodableMap appDetails;
        appDetails[flutter::EncodableValue(L"appName")] = flutter::EncodableValue(detail.appName);
        appDetails[flutter::EncodableValue(L"pid")] = flutter::EncodableValue(static_cast<int>(detail.pid));
        appDetails[flutter::EncodableValue(L"totalTime")] = flutter::EncodableValue(static_cast<int64_t>(detail.totalTime));
        appDetails[flutter::EncodableValue(L"creationTime")] = flutter::EncodableValue(detail.creationTime.dwLowDateTime | (static_cast<int64_t>(detail.creationTime.dwHighDateTime) << 32));
        appDetails[flutter::EncodableValue(L"exitTime")] = flutter::EncodableValue(detail.exitTime.dwLowDateTime | (static_cast<int64_t>(detail.exitTime.dwHighDateTime) << 32));

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
