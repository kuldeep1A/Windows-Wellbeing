// wwplugin_plugin.h

#pragma once

#include <flutter/encodable_value.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

namespace wwplugin
{
  class WwpluginPlugin : public flutter::Plugin
  {
  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    WwpluginPlugin();

    virtual ~WwpluginPlugin();

  private:
    void HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &call,
                          std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    std::vector<std::string> GetRunningProcessNames();
  };
} // namespace wwplugin
