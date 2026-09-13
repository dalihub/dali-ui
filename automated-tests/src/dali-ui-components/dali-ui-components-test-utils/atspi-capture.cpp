/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "atspi-capture.h"

#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h>
#include <dali/public-api/actors/layer.h>
// Test-only dependency on the CURRENT adaptor transport. Do not duplicate its
// DBusWrapper declaration: the older toolkit fixture has a different ABI.
// The harness's src/common/assert.h shadows the standard header and its assert
// returns an int from the containing function. Keep assertions active, but use
// DALi's assertion only while parsing the adaptor constructors/void methods.
#include <cassert>
#pragma push_macro("assert")
#undef assert
#define assert(condition) DALI_ASSERT_ALWAYS(condition)
#include <dali/internal/accessibility/bridge/dbus/dbus-wrapper-gdbus.h>
#pragma pop_macro("assert")
#include <dali-ui/ui-timer.h>
#include <stdexcept>

namespace ComponentsTest
{
namespace
{
// Keep the real GDBus message/variant encoder. Only external endpoints, calls,
// registrations and the final send are replaced; Components, Foundation and
// BridgeObject filtering/coalescing/serialization all execute unchanged.
class CaptureTransport : public GdbusDBusWrapper
{
  struct EndpointObject : Object
  {
    std::string bus, path;
  };
  struct EndpointProxy : Proxy
  {
    std::string bus, path, interface;
  };

public:
  std::vector<AtspiStateEvent> events;
  bool                       enabled{false};

  ConnectionPtr dbus_connection_get_impl(ConnectionType) override
  {
    return mConnection;
  }
  ConnectionPtr dbus_address_connection_get_impl(const std::string& address) override
  {
    if(address != "capture:atspi")
    {
      throw std::runtime_error("Unexpected AT-SPI bus address");
    }
    return mConnection;
  }
  std::string dbus_connection_unique_name_get_impl(const ConnectionPtr&) override
  {
    return ":1.42";
  }
  ObjectPtr dbus_object_get_impl(const ConnectionPtr&, const std::string& bus, const std::string& path) override
  {
    auto object  = std::make_shared<EndpointObject>();
    object->bus  = bus;
    object->path = path;
    return object;
  }
  ProxyPtr dbus_proxy_get_impl(const ObjectPtr& object, const std::string& interface) override
  {
    auto endpoint    = std::static_pointer_cast<EndpointObject>(object);
    auto proxy       = std::make_shared<EndpointProxy>();
    proxy->bus       = endpoint->bus;
    proxy->path      = endpoint->path;
    proxy->interface = interface;
    return proxy;
  }
  ProxyPtr dbus_proxy_copy_impl(const ProxyPtr& proxy) override
  {
    return proxy;
  }
  std::string dbus_proxy_interface_get_impl(const ProxyPtr& proxy) override
  {
    return std::static_pointer_cast<EndpointProxy>(proxy)->interface;
  }
  MessagePtr dbus_proxy_method_call_new_impl(const ProxyPtr& proxy, const std::string& method) override
  {
    auto endpoint = std::static_pointer_cast<EndpointProxy>(proxy);
    return create(g_dbus_message_new_method_call(endpoint->bus.c_str(), endpoint->path.c_str(), endpoint->interface.c_str(), method.c_str()), true);
  }
  MessagePtr dbus_proxy_send_and_block_impl(const ProxyPtr&, const MessagePtr& message) override
  {
    auto        request   = get(message);
    std::string path      = g_dbus_message_get_path(request);
    std::string interface = g_dbus_message_get_interface(request);
    std::string member    = g_dbus_message_get_member(request);
    GVariant*   body      = nullptr;
    if(path == "/org/a11y/bus" && interface == "org.a11y.Bus" && member == "GetAddress")
    {
      body = g_variant_new("(s)", "capture:atspi");
    }
    else if(path == "/org/a11y/bus" && interface == "org.freedesktop.DBus.Properties" && member == "Get")
    {
      const gchar* propertyInterface;
      const gchar* property;
      g_variant_get(g_dbus_message_get_body(request), "(&s&s)", &propertyInterface, &property);
      if(std::string(propertyInterface) != "org.a11y.Status" ||
         (std::string(property) != "IsEnabled" && std::string(property) != "ScreenReaderEnabled"))
      {
        throw std::runtime_error("Unexpected AT-SPI status property");
      }
      body = g_variant_new("(v)", g_variant_new_boolean(enabled));
    }
    else if(path == "/org/a11y/atspi/accessible/root" && interface == "org.a11y.atspi.Socket" && member == "Embed")
    {
      body = g_variant_new("((so))", ":1.43", "/org/a11y/atspi/accessible/root");
    }
    else if(path == "/org/a11y/atspi/accessible/root" && interface == "org.a11y.atspi.Socket" && member == "Unembed")
    {
      body = g_variant_new("()");
    }
    else if(interface == "org.a11y.atspi.Registry" && member == "GetRegisteredEvents")
    {
      body = g_variant_new("(@a(ss))", g_variant_new_array(G_VARIANT_TYPE("(ss)"), nullptr, 0));
    }
    else
    {
      throw std::runtime_error("Unexpected AT-SPI RPC: " + path + " " + interface + "." + member);
    }
    auto reply = g_dbus_message_new();
    g_dbus_message_set_message_type(reply, G_DBUS_MESSAGE_TYPE_METHOD_RETURN);
    g_dbus_message_set_body(reply, body);
    return create(reply, true);
  }
  PendingPtr dbus_proxy_send_impl(const ProxyPtr& proxy, const MessagePtr& message, const SendCallback& callback) override
  {
    callback(dbus_proxy_send_and_block_impl(proxy, message));
    return std::make_shared<Pending>();
  }
  void dbus_proxy_signal_handler_add_impl(const ProxyPtr&, const std::string&, const std::function<void(const MessagePtr&)>&) override
  {
  }
  void add_property_changed_event_listener_impl(const ProxyPtr&, const std::string&, const std::string&, std::function<void(const void*)>) override
  {
  }
  void add_interface_impl(bool, const std::string&, const ConnectionPtr&, std::vector<std::function<void()>>&, const std::string&, std::vector<MethodInfo>&, std::vector<PropertyInfo>&, std::vector<SignalInfo>&) override
  {
  }
  void dbus_name_request_impl(const ConnectionPtr&, const std::string&) override
  {
  }
  void dbus_name_release_impl(const ConnectionPtr&, const std::string&) override
  {
  }
  PendingPtr dbus_connection_send_impl(const ConnectionPtr&, const MessagePtr& message) override
  {
    auto wireMessage = get(message);
    if(g_strcmp0(g_dbus_message_get_interface(wireMessage), "org.a11y.atspi.Event.Object") == 0 &&
       g_strcmp0(g_dbus_message_get_member(wireMessage), "StateChanged") == 0)
    {
      auto body = g_dbus_message_get_body(wireMessage);
      if(!body || !g_variant_is_of_type(body, G_VARIANT_TYPE("(siiv(so))")))
      {
        throw std::runtime_error("Malformed serialized AT-SPI StateChanged body");
      }
      const gchar *state, *bus, *path;
      gint32       value, reserved;
      GVariant*    payload;
      g_variant_get(body, "(&siiv(&s&o))", &state, &value, &reserved, &payload, &bus, &path);
      if(!g_variant_is_of_type(payload, G_VARIANT_TYPE_INT32))
      {
        g_variant_unref(payload);
        throw std::runtime_error("Unexpected serialized AT-SPI StateChanged variant");
      }
      events.push_back({g_dbus_message_get_path(wireMessage), g_dbus_message_get_interface(wireMessage),
                        g_dbus_message_get_member(wireMessage), g_dbus_message_get_signature(wireMessage),
                        state, value, reserved, g_variant_get_int32(payload), bus, path});
      g_variant_unref(payload);
    }
    return std::make_shared<Pending>();
  }

private:
  ConnectionPtr mConnection{std::make_shared<Connection>()};
};
}

struct AtspiCapture::Impl
{
  CaptureTransport* transport{nullptr};
  bool              started{false};
};

AtspiCapture::AtspiCapture()
: mImpl(std::make_unique<Impl>())
{
  auto transport   = std::make_unique<CaptureTransport>();
  mImpl->transport = transport.get();
  DBusWrapper::Install(std::move(transport));
}

AtspiCapture::~AtspiCapture()
{
  Stop();
  DBusWrapper::Install({});
}

void AtspiCapture::Start(Dali::Integration::Scene scene)
{
  if(mImpl->started)
  {
    return;
  }
  auto bridge = Dali::Integration::Accessibility::Bridge::GetCurrentBridge();
  if(!bridge)
  {
    throw std::runtime_error("No accessibility bridge for emission capture");
  }
  // Ensure partial initialization is also shut down if an RPC/schema check
  // throws. Resume before installing the window: an inactive status read may
  // otherwise ForceDown and clear the just-added top-level window.
  mImpl->started = true;
  mImpl->transport->enabled = true;
  bridge->ApplicationResumed();
  bridge->AddTopLevelWindow(Dali::Accessibility::Accessible::Get(scene.GetRootLayer()));
  bridge->SetApplicationName("ComponentsAtspiCapture");
  bridge->Initialize();
  if(!bridge->IsUp())
  {
    throw std::runtime_error("Accessibility bridge did not activate");
  }
  Clear();
}

void AtspiCapture::Stop()
{
  if(mImpl->started)
  {
    mImpl->transport->enabled = false;
    auto bridge = Dali::Integration::Accessibility::Bridge::GetCurrentBridge();
    bridge->ApplicationPaused();
    bridge->ForceDown();
    mImpl->started = false;
  }
}

void AtspiCapture::Tick(unsigned int count)
{
  for(unsigned int i = 0; i < count; ++i)
  {
    Test::EmitGlobalTimerSignal();
  }
}

void AtspiCapture::Clear()
{
  mImpl->transport->events.clear();
}

const std::vector<AtspiStateEvent>& AtspiCapture::Events() const
{
  return mImpl->transport->events;
}
}
