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

#include <vector>
#include <psapi.h>
#include <string>

// Add the pragma comment directive to include version.lib
#pragma comment(lib, "version.lib")

namespace wwplugin
{

  BOOL IsAppWindow(HWND hwnd)
  {
    return IsWindowVisible(hwnd) && GetWindowTextLengthW(hwnd) > 0;
  }

  std::wstring GetApplicationDescription(const std::wstring &filePath)
  {
    DWORD dwSize = GetFileVersionInfoSize(filePath.c_str(), NULL);
    if (dwSize == 0)
      return L"";

    std::vector<BYTE> data(dwSize);
    if (!GetFileVersionInfo(filePath.c_str(), 0, dwSize, data.data()))
      return L"";

    void *pVersionInfo = nullptr;
    UINT len = 0;
    if (!VerQueryValue(data.data(), L"\\", &pVersionInfo, &len))
      return L"";

    VS_FIXEDFILEINFO *pFileInfo = static_cast<VS_FIXEDFILEINFO *>(pVersionInfo);
    if (pFileInfo->dwSignature != 0xFEEF04BD)
      return L"";

    wchar_t *pDescription = nullptr;
    UINT descLen = 0;
    if (VerQueryValue(data.data(), L"\\StringFileInfo\\040904b0\\FileDescription", reinterpret_cast<void **>(&pDescription), &descLen))
    {
      return std::wstring(pDescription, descLen);
    }

    return L"";
  }

  BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
  {
    if (IsAppWindow(hwnd))
    {
      std::vector<std::wstring> *windowTitles = reinterpret_cast<std::vector<std::wstring> *>(lParam);

      DWORD processId;
      GetWindowThreadProcessId(hwnd, &processId);

      // Open the process with elevated privileges
      HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
      if (processHandle != NULL)
      {
        WCHAR processName[MAX_PATH];
        DWORD size = MAX_PATH;
        if (QueryFullProcessImageName(processHandle, 0, processName, &size) != 0)
        {
          std::wstring appDescription = GetApplicationDescription(processName);
          if (!appDescription.empty())
          {
            appDescription.erase(appDescription.size() - 1);
            windowTitles->push_back(appDescription);
          }
        }
        CloseHandle(processHandle);
      }
    }
    return TRUE; // Continue enumeration
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

  void WwpluginPlugin::HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                                        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    if (method_call.method_name().compare("getActiveApplicationList") == 0)
    {
      std::vector<std::wstring> windowTitles;
      EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windowTitles));

      std::vector<flutter::EncodableValue> encodableWindowTitles;
      for (const auto &title : windowTitles)
      {
        encodableWindowTitles.push_back(flutter::EncodableValue(WideStringToUTF8(title)));
      }

      result->Success(flutter::EncodableValue(encodableWindowTitles));
    }
    else
    {
      result->NotImplemented();
    }
  }

  std::string WwpluginPlugin::WideStringToUTF8(const std::wstring &wstr)
  {
    if (wstr.empty())
      return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
  }

} // namespace wwplugin
