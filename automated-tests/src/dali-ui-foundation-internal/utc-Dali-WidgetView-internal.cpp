/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */
// Install the adaptor test mocks before including production headers.
#include <dali-ui-test-suite-utils.h>

#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/widget-view-integ.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/views/widget/widget-view-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/text/style/font-attributes.h>
#include <dali-ui-foundation/public-api/views/widget/widget-view.h>
#include <dali-ui/ui-environment-variable.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/widget-engine/widget-engine-plugin.h>
#include <dali/devel-api/adaptor-framework/widget-engine/widget-engine.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/public-api/object/weak-handle.h>
#include <dlfcn.h>
#include <test-native-image.h>
#include <array>

using namespace Dali;
using Dali::Ui::WidgetView;

Dali::TestNativeImagePointer GetWidgetTestNativeImage(const Dali::NativeImage& image);

namespace
{
constexpr const char* APP_ID = "test.widget.viewer";
using Event                  = WidgetEngineInstancePlugin::EventType;
enum Count
{
  ENGINES_CREATED,
  ENGINES_DESTROYED,
  INSTANCES_CREATED,
  INSTANCES_DESTROYED,
  RETRIES,
  ACTIVE_ENGINES
};
void* library;
int (*getCount)(int);
void (*emitEvent)(int, Event);
void (*emitFrame)(int, Dali::NativeImagePtr);
bool (*isPaused)(int);
int (*getDimension)(int, int);
int (*getTouchEventCount)(int);
int (*getCancelledTouchCount)(int);
int (*getKeyEventCount)(int);
int (*getWheelEventCount)(int);
const char* (*getLastKey)(int);
int (*getLastWheelDelta)(int);
std::array<int, 6> initialCounts;

int CountSinceStartup(Count count)
{
  return getCount(count) - initialCounts[count];
}

WidgetView NewWidget(const std::string& widgetId = "test.widget", const std::string& appId = APP_ID)
{
  return WidgetView::New(Dali::String(appId.c_str()), Dali::String(widgetId.c_str()), "content", 64, 64, 1.0f);
}

void ProcessTouch(UiTestApplication& application, PointState::Type state, uint32_t time)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(Vector2(32.0f, 32.0f));
  point.SetDeviceId(1);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  application.ProcessEvent(touchEvent);
}

struct WheelObserver : ConnectionTracker
{
  bool OnWheel(Actor, WheelEvent)
  {
    ++received;
    forwarded = getWheelEventCount(0);
    return consume;
  }

  int  received{0};
  int  forwarded{0};
  bool consume{false};
};

struct Signals : ConnectionTracker
{
  void Added(WidgetView)
  {
    ++added;
  }
  void Deleted(WidgetView)
  {
    ++deleted;
  }
  void Faulted(WidgetView)
  {
    ++faulted;
  }
  int added{0};
  int deleted{0};
  int faulted{0};
};

struct LocalizationLookup
{
  void Apply(BaseHandle, const Dali::String&)
  {
    if(enabled)
    {
      enabled = false;
      view    = NewWidget("nested.widget");
    }
  }
  WidgetView view;
  bool       enabled{false};
};
} // namespace

void utc_dali_widget_view_internal_startup(void)
{
  test_return_value = TET_UNDEF;
  const char* path  = ADDON_LIBS_PATH "/libwidget-view-test-plugin.so";
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_WIDGET_ENGINE_PLUGIN", path);
  library = dlopen(path, RTLD_NOW);
  DALI_ASSERT_ALWAYS(library);
  getCount               = reinterpret_cast<int (*)(int)>(dlsym(library, "GetWidgetTestCount"));
  emitEvent              = reinterpret_cast<void (*)(int, Event)>(dlsym(library, "EmitWidgetTestEvent"));
  emitFrame              = reinterpret_cast<void (*)(int, Dali::NativeImagePtr)>(dlsym(library, "EmitWidgetTestFrame"));
  isPaused               = reinterpret_cast<bool (*)(int)>(dlsym(library, "IsWidgetTestPaused"));
  getDimension           = reinterpret_cast<int (*)(int, int)>(dlsym(library, "GetWidgetTestDimension"));
  getTouchEventCount     = reinterpret_cast<int (*)(int)>(dlsym(library, "GetWidgetTestTouchEventCount"));
  getCancelledTouchCount = reinterpret_cast<int (*)(int)>(dlsym(library, "GetWidgetTestCancelledTouchCount"));
  getKeyEventCount       = reinterpret_cast<int (*)(int)>(dlsym(library, "GetWidgetTestKeyEventCount"));
  getWheelEventCount     = reinterpret_cast<int (*)(int)>(dlsym(library, "GetWidgetTestWheelEventCount"));
  getLastKey             = reinterpret_cast<const char* (*)(int)>(dlsym(library, "GetWidgetTestLastKey"));
  getLastWheelDelta      = reinterpret_cast<int (*)(int)>(dlsym(library, "GetWidgetTestLastWheelDelta"));
  DALI_ASSERT_ALWAYS(getCount && emitEvent && emitFrame && isPaused && getDimension && getTouchEventCount);
  DALI_ASSERT_ALWAYS(getCancelledTouchCount && getKeyEventCount && getWheelEventCount && getLastKey && getLastWheelDelta);
  for(size_t i = 0; i < initialCounts.size(); ++i) initialCounts[i] = getCount(i);
}

void utc_dali_widget_view_internal_cleanup(void)
{
  dlclose(library);
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_WIDGET_ENGINE_PLUGIN", nullptr);
  test_return_value = TET_PASS;
}

int UtcDaliWidgetViewHandleAndTypeRegistrationP(void)
{
  UiTestApplication application;
  WidgetView        empty;
  DALI_TEST_CHECK(!empty);
  DALI_TEST_CHECK(!WidgetView::DownCast(Actor::New()));
  auto type = TypeRegistry::Get().GetTypeInfo("WidgetView");
  DALI_TEST_CHECK(type);
  // A type registry has no application/widget IDs with which to create a view.
  DALI_TEST_CHECK(!type.CreateInstance());
  auto       view = NewWidget();
  WidgetView copy(view);
  empty = copy;
  view.Reset();
  copy.Reset();
  DALI_TEST_CHECK(empty);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_CREATED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 0, TEST_LOCATION);
  empty.Reset();
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewRegisteredPropertiesP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  DALI_TEST_EQUALS(view.GetProperty<Dali::String>(WidgetView::Property::WIDGET_ID), view.GetWidgetId(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<Dali::String>(WidgetView::Property::CONTENT_INFO), view.GetContentInfo(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<Dali::String>(WidgetView::Property::TITLE), view.GetTitle(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPropertyIndex("widgetStateFaulted"), static_cast<Property::Index>(WidgetView::Property::WIDGET_STATE_FAULTED), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPropertyIndex("keepWidgetSize"), static_cast<Property::Index>(WidgetView::Property::KEEP_WIDGET_SIZE), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPropertyIndex("loadingText"), static_cast<Property::Index>(Ui::Integration::WidgetView::Property::LOADING_TEXT), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPropertyIndex("retryText"), static_cast<Property::Index>(Ui::Integration::WidgetView::Property::RETRY_TEXT), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPropertyIndex("effect"), static_cast<Property::Index>(Ui::Integration::WidgetView::Property::EFFECT), TEST_LOCATION);
  view.SetProperty(view.GetPropertyIndex("loadingText"), false);
  DALI_TEST_CHECK(!view.IsLoadingTextVisible());
  view.SetProperty(Ui::Integration::WidgetView::Property::LOADING_TEXT, true);
  DALI_TEST_CHECK(view.IsLoadingTextVisible());
  DALI_TEST_CHECK(view.GetProperty(Ui::Integration::WidgetView::Property::EFFECT).GetType() == Property::NONE);
  DALI_TEST_CHECK(view.GetProperty(Ui::Integration::WidgetView::Property::RETRY_TEXT).GetType() == Property::NONE);
  END_TEST;
}

int UtcDaliWidgetViewNamedActionsP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  application.RunIdles();
  const Property::Map attributes;
  DALI_TEST_CHECK(view.DoAction("pauseWidget", attributes));
  DALI_TEST_CHECK(isPaused(0));
  DALI_TEST_CHECK(view.DoAction("resumeWidget", attributes));
  DALI_TEST_CHECK(!isPaused(0));
  DALI_TEST_CHECK(view.CancelTouchEvent());
  DALI_TEST_CHECK(view.DoAction("cancelTouchEvent", attributes));
  DALI_TEST_EQUALS(getCancelledTouchCount(0), 2, TEST_LOCATION);
  emitEvent(0, Event::FAULTED);
  DALI_TEST_CHECK(view.DoAction("activateFaultedWidget", attributes));
  DALI_TEST_CHECK(!view.IsWidgetFaulted());
  DALI_TEST_EQUALS(CountSinceStartup(RETRIES), 1, TEST_LOCATION);
  DALI_TEST_CHECK(!view.DoAction("unknownWidgetAction", attributes));
  END_TEST;
}

int UtcDaliWidgetViewNamedSignalsAndEventOrderingP(void)
{
  UiTestApplication        application;
  auto                     view = NewWidget();
  ConnectionTracker        tracker;
  std::vector<std::string> events;
  for(const char* name : {"widgetAdded", "widgetDeleted", "widgetCreationAborted", "widgetContentUpdated", "widgetUpdatePeriodChanged", "widgetFaulted"})
  {
    DALI_TEST_CHECK(view.ConnectSignal(&tracker, name, [&events, name]()
    { events.emplace_back(name); }));
  }
  DALI_TEST_CHECK(!view.ConnectSignal(&tracker, "unknownWidgetSignal", []() {}));
  application.RunIdles();
  emitEvent(0, Event::ADDED); // Replayed ADDED must not be emitted twice.
  emitEvent(0, Event::CONTENT_UPDATED);
  emitEvent(0, Event::UPDATE_PERIOD_CHANGED);
  emitEvent(0, Event::FAULTED);
  emitEvent(0, Event::FAULTED); // No second deleted signal without another ADDED.
  emitEvent(0, Event::ADDED);
  const std::vector<std::string> expected{"widgetAdded", "widgetContentUpdated", "widgetUpdatePeriodChanged", "widgetDeleted", "widgetFaulted", "widgetFaulted", "widgetAdded"};
  DALI_TEST_CHECK(events == expected);
  tracker.DisconnectAll();
  emitEvent(0, Event::CONTENT_UPDATED);
  DALI_TEST_CHECK(events == expected);
  END_TEST;
}

int UtcDaliWidgetViewCreationAbortedStateP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  ConnectionTracker tracker;
  int               aborted = 0;
  view.WidgetCreationAbortedSignal().Connect(&tracker, [&aborted](WidgetView)
  { ++aborted; });
  application.RunIdles();
  emitEvent(0, Event::CREATION_ABORTED);
  DALI_TEST_EQUALS(aborted, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!view.IsLoadingTextVisible());
  DALI_TEST_CHECK(view.IsRetryTextVisible());
  DALI_TEST_CHECK(!view.GetChildAt(0).GetChildAt(0).GetProperty<bool>(Actor::Property::VISIBLE));
  END_TEST;
}

int UtcDaliWidgetViewPreviewCornersFollowViewP(void)
{
  UiTestApplication application;
  auto              view    = NewWidget();
  auto              preview = Ui::ImageView::DownCast(view.GetChildAt(0).GetChildAt(0));
  view.SetCornerRadius(Vector4(1.0f, 2.0f, 3.0f, 4.0f));
  view.SetCornerRadiusPolicy(Ui::CornerRadiusPolicy::ABSOLUTE);
  view.SetCornerSquareness(Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  DALI_TEST_EQUALS(preview.GetCornerRadius(), view.GetCornerRadius(), TEST_LOCATION);
  DALI_TEST_CHECK(preview.GetCornerRadiusPolicy() == Ui::CornerRadiusPolicy::ABSOLUTE);
  DALI_TEST_EQUALS(preview.GetCornerSquareness(), view.GetCornerSquareness(), TEST_LOCATION);
  view.SetCornerRadiusPolicyRelative();
  DALI_TEST_CHECK(preview.IsCornerRadiusPolicyRelative());
  END_TEST;
}

int UtcDaliWidgetViewTextStyleValidationP(void)
{
  UiTestApplication application;
  auto              view    = NewWidget();
  auto              labels  = view.GetChildAt(0).GetChildAt(1);
  auto              loading = Ui::Label::DownCast(labels.GetChildAt(0));
  auto              retry   = Ui::Label::DownCast(labels.GetChildAt(1));
  Property::Map     properties;
  properties.Insert("fontStyle", "{\"weight\":\"bold\",\"width\":\"condensed\",\"slant\":\"italic\"}");
  view.SetProperty(Ui::Integration::WidgetView::Property::LOADING_TEXT, properties);
  view.SetProperty(Ui::Integration::WidgetView::Property::RETRY_TEXT, properties);
  DALI_TEST_CHECK(loading.GetFontWeight() == Ui::Text::FontWeight::BOLD);
  DALI_TEST_CHECK(retry.GetFontSlant() == Ui::Text::FontSlant::ITALIC);
  properties["fontStyle"] = "{\"weight\":\"normal\"}";
  view.SetProperty(Ui::Integration::WidgetView::Property::LOADING_TEXT, properties);
  view.SetProperty(Ui::Integration::WidgetView::Property::RETRY_TEXT, properties);
  DALI_TEST_CHECK(loading.GetFontWidth() == Ui::Text::FontWidth::NORMAL);
  DALI_TEST_CHECK(retry.GetFontSlant() == Ui::Text::FontSlant::NORMAL);
  properties["fontStyle"] = true;
  view.SetProperty(Ui::Integration::WidgetView::Property::LOADING_TEXT, properties);
  properties["fontStyle"] = "{}";
  view.SetProperty(Ui::Integration::WidgetView::Property::RETRY_TEXT, properties);
  DALI_TEST_CHECK(loading.GetFontWeight() == Ui::Text::FontWeight::NORMAL);
  DALI_TEST_CHECK(retry.GetFontWeight() == Ui::Text::FontWeight::NORMAL);
  END_TEST;
}

int UtcDaliWidgetViewInheritedVisibilityControlsPauseP(void)
{
  UiTestApplication application;
  auto              parent = Ui::View::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(200.0f);
  auto view = NewWidget();
  parent.Add(view);
  application.GetScene().Add(parent);
  const auto settle = [&application]()
  {
    for(int frame = 0; frame < 3; ++frame)
    {
      application.SendNotification();
      application.Render();
    }
  };
  settle();
  DALI_TEST_CHECK(!isPaused(0));
  parent.SetProperty(Actor::Property::VISIBLE, false);
  settle();
  DALI_TEST_CHECK(isPaused(0));
  parent.SetProperty(Actor::Property::VISIBLE, true);
  settle();
  DALI_TEST_CHECK(!isPaused(0));
  view.PauseWidget();
  parent.SetProperty(Actor::Property::VISIBLE, false);
  parent.SetProperty(Actor::Property::VISIBLE, true);
  settle();
  DALI_TEST_CHECK(isPaused(0));
  END_TEST;
}

int UtcDaliWidgetViewInputReachesProviderP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.SetPivot(Pivot::TOP_LEFT);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Ui::FocusManager::Get().SetCurrentFocusView(view));
  Dali::Integration::KeyEvent key("a", "a", "a", 38, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "keyboard", Device::Class::KEYBOARD, Device::Subclass::NONE);
  application.ProcessEvent(key);
  DALI_TEST_EQUALS(getKeyEventCount(0), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::String(getLastKey(0)), Dali::String("a"), TEST_LOCATION);
  Dali::Integration::WheelEvent wheel(Dali::Integration::WheelEvent::MOUSE_WHEEL, 0, 0, Vector2(32.0f, 32.0f), -2, 110);
  application.ProcessEvent(wheel);
  DALI_TEST_EQUALS(getWheelEventCount(0), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(getLastWheelDelta(0), -2, TEST_LOCATION);

  // Provider forwarding must also happen exactly once with application listeners.
  WheelObserver observer;
  WheelObserver ancestor;
  view.WheelEventSignal().Connect(&observer, &WheelObserver::OnWheel);
  application.GetScene().GetRootLayer().WheelEventSignal().Connect(&ancestor, &WheelObserver::OnWheel);
  wheel.delta     = 3;
  wheel.timeStamp = 120;
  application.ProcessEvent(wheel);
  DALI_TEST_EQUALS(getWheelEventCount(0), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(getLastWheelDelta(0), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.received, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.forwarded, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(ancestor.received, 1, TEST_LOCATION);

  // The application can consume the event after forwarding to the provider.
  observer.consume = true;
  wheel.timeStamp  = 130;
  application.ProcessEvent(wheel);
  DALI_TEST_EQUALS(getWheelEventCount(0), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.received, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.forwarded, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(ancestor.received, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewProviderSizeFollowsArrangeP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  view.SetRequestedWidth(120.0f);
  view.SetRequestedHeight(80.0f);
  // A request is measurement input; the provider follows actual layout output.
  DALI_TEST_EQUALS(getDimension(0, 0), 64, TEST_LOCATION);
  auto measured = view.Measure(300.0f, 200.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  DALI_TEST_EQUALS(getDimension(0, 0), 120, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 1), 80, TEST_LOCATION);
  const int resizeCount = getDimension(0, 2);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  DALI_TEST_EQUALS(getDimension(0, 2), resizeCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewMeasureMatchParentP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  view.SetRequestedWidth(Ui::MATCH_PARENT);
  view.SetRequestedHeight(Ui::MATCH_PARENT);
  auto size = view.Measure(200.0f, 100.0f);
  DALI_TEST_EQUALS(size.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewAccessibilityWithoutProviderP(void)
{
  UiTestApplication application;
  auto              view       = NewWidget();
  auto*             accessible = Dali::Accessibility::Accessible::Get(view);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_CHECK(accessible->GetChildren().empty());
  DALI_TEST_CHECK(accessible->GetAttributes().count("child_bus") == 0u);
  Dali::Integration::Accessibility::Bridge::EnabledSignal().Emit();
  DALI_TEST_CHECK(accessible->GetChildren().empty());
  Dali::Integration::Accessibility::Bridge::DisabledSignal().Emit();
  DALI_TEST_CHECK(accessible->GetChildren().empty());
  END_TEST;
}

int UtcDaliWidgetViewFramesReplaceVisualAndClearOverlaysP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  application.GetScene().Add(view);
  application.RunIdles();
  auto& data = Ui::Internal::ViewDataImpl::Get(Ui::Internal::GetImplementation(view));
  emitFrame(0, {});
  DALI_TEST_CHECK(!data.GetVisual(WidgetView::Property::WIDGET_ID));
  DALI_TEST_CHECK(view.IsLoadingTextVisible());

  Property::Map shader;
  shader.Insert("fragmentShader", "void main() { gl_FragColor = texture2D(sTexture, vTexCoord); }");
  Property::Map effect;
  effect.Insert("shader", shader);
  view.SetProperty(Ui::Integration::WidgetView::Property::EFFECT, effect); // Settings made before the first frame must be applied.
  auto first         = NativeImage::New(64, 64, NativeImage::COLOR_DEPTH_32);
  auto firstResource = GetWidgetTestNativeImage(*first);
  emitFrame(0, first);
  application.SendNotification();
  application.Render();
  auto firstVisual = data.GetVisual(WidgetView::Property::WIDGET_ID);
  DALI_TEST_CHECK(firstVisual);
  DALI_TEST_CHECK(Ui::GetImplementation(firstVisual).IsUsingCustomShader());
  DALI_TEST_CHECK(!view.IsLoadingTextVisible());
  DALI_TEST_CHECK(!view.IsRetryTextVisible());
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  auto firstTexture = firstVisual.GetRenderer().GetTextures().GetTexture(0);

  auto second = NativeImage::New(120, 80, NativeImage::COLOR_DEPTH_32);
  emitFrame(0, second);
  application.SendNotification();
  application.Render();
  auto secondVisual = data.GetVisual(WidgetView::Property::WIDGET_ID);
  DALI_TEST_CHECK(secondVisual != firstVisual);
  DALI_TEST_CHECK(secondVisual.GetRenderer().GetTextures().GetTexture(0) != firstTexture);
  Vector2 naturalSize;
  secondVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(120.0f, 80.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(Ui::GetImplementation(secondVisual).IsUsingCustomShader());
  emitFrame(0, {});
  DALI_TEST_CHECK(data.GetVisual(WidgetView::Property::WIDGET_ID) == secondVisual);
  DALI_TEST_CHECK(firstResource->mExtensionCreateCalls > 0);
  END_TEST;
}

int UtcDaliWidgetViewPreviewToggleAfterFirstFrameP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  application.GetScene().Add(view);
  application.RunIdles();
  auto preview = view.GetChildAt(0).GetChildAt(0);
  DALI_TEST_CHECK(preview.GetProperty<bool>(Actor::Property::VISIBLE));

  // Loading remains active even when its text is disabled.
  view.SetLoadingTextVisible(false);
  view.SetPreviewEnabled(false);
  DALI_TEST_CHECK(!preview.GetProperty<bool>(Actor::Property::VISIBLE));
  view.SetPreviewEnabled(true);
  DALI_TEST_CHECK(preview.GetProperty<bool>(Actor::Property::VISIBLE));
  view.SetProperty(WidgetView::Property::PREVIEW, false);
  DALI_TEST_CHECK(!preview.GetProperty<bool>(Actor::Property::VISIBLE));
  view.SetProperty(WidgetView::Property::PREVIEW, true);
  DALI_TEST_CHECK(preview.GetProperty<bool>(Actor::Property::VISIBLE));

  auto image = NativeImage::New(64, 64, NativeImage::COLOR_DEPTH_32);
  emitFrame(0, image);
  application.SendNotification();
  application.Render();
  auto& data   = Ui::Internal::ViewDataImpl::Get(Ui::Internal::GetImplementation(view));
  auto  visual = data.GetVisual(WidgetView::Property::WIDGET_ID);
  DALI_TEST_CHECK(visual);
  DALI_TEST_CHECK(!preview.GetProperty<bool>(Actor::Property::VISIBLE));

  // A static widget sends no further frame to hide an incorrectly shown preview.
  view.SetPreviewEnabled(false);
  view.SetPreviewEnabled(true);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.IsPreviewEnabled());
  DALI_TEST_CHECK(!preview.GetProperty<bool>(Actor::Property::VISIBLE));
  DALI_TEST_CHECK(data.GetVisual(WidgetView::Property::WIDGET_ID) == visual);

  view.SetProperty(WidgetView::Property::PREVIEW, false);
  view.SetProperty(WidgetView::Property::PREVIEW, true);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.GetProperty<bool>(WidgetView::Property::PREVIEW));
  DALI_TEST_CHECK(!preview.GetProperty<bool>(Actor::Property::VISIBLE));
  DALI_TEST_CHECK(data.GetVisual(WidgetView::Property::WIDGET_ID) == visual);
  END_TEST;
}

int UtcDaliWidgetViewPreviewToggleAcrossFaultAndRetryP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  application.GetScene().Add(view);
  application.RunIdles();
  auto preview = view.GetChildAt(0).GetChildAt(0);
  auto image   = NativeImage::New(64, 64, NativeImage::COLOR_DEPTH_32);
  emitFrame(0, image);
  emitEvent(0, Event::FAULTED);
  DALI_TEST_CHECK(view.IsRetryTextVisible());
  view.SetPreviewEnabled(false);
  view.SetPreviewEnabled(true);
  DALI_TEST_CHECK(!preview.GetProperty<bool>(Actor::Property::VISIBLE));
  view.SetProperty(WidgetView::Property::PREVIEW, false);
  view.SetProperty(WidgetView::Property::PREVIEW, true);
  DALI_TEST_CHECK(!preview.GetProperty<bool>(Actor::Property::VISIBLE));

  view.SetPreviewEnabled(false);
  view.ActivateFaultedWidget();
  DALI_TEST_CHECK(view.IsLoadingTextVisible());
  DALI_TEST_CHECK(!preview.GetProperty<bool>(Actor::Property::VISIBLE));
  view.SetPreviewEnabled(true);
  DALI_TEST_CHECK(preview.GetProperty<bool>(Actor::Property::VISIBLE));
  auto recovered = NativeImage::New(64, 64, NativeImage::COLOR_DEPTH_32);
  emitFrame(0, recovered);
  DALI_TEST_CHECK(view.IsPreviewEnabled());
  DALI_TEST_CHECK(!preview.GetProperty<bool>(Actor::Property::VISIBLE));
  END_TEST;
}

int UtcDaliWidgetViewFrameReleaseOnFaultAndReattachP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  application.GetScene().Add(view);
  application.RunIdles();
  auto image    = NativeImage::New(64, 64, NativeImage::COLOR_DEPTH_32);
  auto resource = GetWidgetTestNativeImage(*image);
  auto settle   = [&application]()
  {
    for(int frame = 0; frame < 4; ++frame)
    {
      application.RunIdles();
      application.SendNotification();
      application.Render();
    }
  };
  emitFrame(0, image);
  settle();
  DALI_TEST_EQUALS(resource->mExtensionCreateCalls, 1, TEST_LOCATION);
  view.Unparent();
  settle();
  DALI_TEST_EQUALS(resource->mExtensionDestroyCalls, 0, TEST_LOCATION);
  application.GetScene().Add(view);
  settle();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(resource->mExtensionCreateCalls, 1, TEST_LOCATION);

  emitEvent(0, Event::FAULTED);
  settle();
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(resource->mExtensionDestroyCalls, 1, TEST_LOCATION);
  DALI_TEST_CHECK(view.IsRetryTextVisible());
  view.ActivateFaultedWidget();
  DALI_TEST_CHECK(view.IsLoadingTextVisible());
  auto recovered         = NativeImage::New(64, 64, NativeImage::COLOR_DEPTH_32);
  auto recoveredResource = GetWidgetTestNativeImage(*recovered);
  emitFrame(0, recovered);
  settle();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!view.IsRetryTextVisible());
  DALI_TEST_CHECK(!view.IsLoadingTextVisible());
  view.Unparent();
  view.Reset();
  settle();
  DALI_TEST_EQUALS(recoveredResource->mExtensionDestroyCalls, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewUnsupportedTouchDoesNotReachProviderP(void)
{
  UiTestApplication application;
  auto              view           = NewWidget();
  auto&             implementation = static_cast<Dali::CustomActorImpl&>(Ui::Internal::GetImplementation(view));
  DALI_TEST_CHECK(implementation.HasIntrinsicTouchHandling());
  auto empty = TouchEvent::New(100);
  DALI_TEST_CHECK(!implementation.OnTouchEvent(empty));
  auto pen = TouchEvent::New(110);
  pen.AddPoint(1, PointState::DOWN, Vector2(32, 32), Device::Class::PEN, Device::Subclass::NONE, "pen", MouseButton::INVALID);
  DALI_TEST_CHECK(!implementation.OnTouchEvent(pen));
  DALI_TEST_EQUALS(getTouchEventCount(0), 0, TEST_LOCATION);
  auto mouse = TouchEvent::New(120);
  mouse.AddPoint(1, PointState::DOWN, Vector2(32, 32), Device::Class::MOUSE, Device::Subclass::NONE, "mouse", MouseButton::PRIMARY);
  DALI_TEST_CHECK(implementation.OnTouchEvent(mouse));
  DALI_TEST_EQUALS(getTouchEventCount(0), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewSignalMayReleaseLastHandleP(void)
{
  UiTestApplication      application;
  auto                   view = NewWidget();
  WeakHandle<WidgetView> weak(view);
  ConnectionTracker      tracker;
  view.WidgetContentUpdatedSignal().Connect(&tracker, [&view](WidgetView)
  { view.Reset(); });
  application.RunIdles();
  emitEvent(0, Event::CONTENT_UPDATED);
  DALI_TEST_CHECK(!weak.GetHandle());
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewSharesEngineP(void)
{
  UiTestApplication application;
  auto              first  = NewWidget();
  auto              second = NewWidget("second.widget");
  DALI_TEST_CHECK(first && second && first != second);
  DALI_TEST_CHECK(WidgetView::DownCast(first) == first);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_CREATED), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(first.GetProperty<float>(WidgetView::Property::UPDATE_PERIOD), 1.0f, TEST_LOCATION);
  first.Reset();
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  DALI_TEST_CHECK(second.PauseWidget());
  second.Reset();
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewApiAndPropertiesShareStateP(void)
{
  UiTestApplication application;
  auto              view     = NewWidget();
  const WidgetView& readOnly = view;
  DALI_TEST_EQUALS(readOnly.GetWidgetId(), Dali::String("test.widget"), TEST_LOCATION);
  DALI_TEST_EQUALS(readOnly.GetInstanceId(), view.GetProperty<Dali::String>(WidgetView::Property::INSTANCE_ID), TEST_LOCATION);
  DALI_TEST_EQUALS(readOnly.GetContentInfo(), Dali::String("content"), TEST_LOCATION);
  DALI_TEST_EQUALS(readOnly.GetTitle(), Dali::String("test.widget"), TEST_LOCATION);
  DALI_TEST_EQUALS(readOnly.GetUpdatePeriod(), 1.0f, TEST_LOCATION);
  DALI_TEST_CHECK(!readOnly.IsWidgetFaulted());

  view.SetPreviewEnabled(false);
  DALI_TEST_CHECK(!view.GetProperty<bool>(WidgetView::Property::PREVIEW));
  view.SetProperty(WidgetView::Property::PREVIEW, true);
  DALI_TEST_CHECK(readOnly.IsPreviewEnabled());
  view.SetKeepWidgetSizeEnabled(true);
  DALI_TEST_CHECK(view.GetProperty<bool>(WidgetView::Property::KEEP_WIDGET_SIZE));
  view.SetProperty(WidgetView::Property::KEEP_WIDGET_SIZE, false);
  DALI_TEST_CHECK(!readOnly.IsKeepWidgetSizeEnabled());

  view.SetLoadingTextVisible(false);
  DALI_TEST_CHECK(!view.GetProperty<bool>(Ui::Integration::WidgetView::Property::LOADING_TEXT));
  view.SetProperty(Ui::Integration::WidgetView::Property::LOADING_TEXT, true);
  DALI_TEST_CHECK(readOnly.IsLoadingTextVisible());
  view.SetRetryTextVisible(true);
  DALI_TEST_CHECK(readOnly.IsRetryTextVisible());
  Property::Map retry;
  retry.Insert("textVisible", false);
  view.SetProperty(Ui::Integration::WidgetView::Property::RETRY_TEXT, retry);
  DALI_TEST_CHECK(!readOnly.IsRetryTextVisible());

  // State transitions must preserve settings from both entry points.
  application.RunIdles();
  emitEvent(0, Event::FAULTED);
  DALI_TEST_CHECK(readOnly.IsWidgetFaulted());
  DALI_TEST_CHECK(view.GetProperty<bool>(WidgetView::Property::WIDGET_STATE_FAULTED));
  DALI_TEST_CHECK(!readOnly.IsLoadingTextVisible());
  DALI_TEST_CHECK(!readOnly.IsRetryTextVisible());
  view.ActivateFaultedWidget();
  DALI_TEST_CHECK(!readOnly.IsWidgetFaulted());
  DALI_TEST_CHECK(readOnly.IsLoadingTextVisible());
  DALI_TEST_CHECK(readOnly.IsPreviewEnabled());
  END_TEST;
}

int UtcDaliWidgetViewTextApisPreservePropertyBehaviorP(void)
{
  UiTestApplication application;
  auto              view    = NewWidget();
  auto              labels  = view.GetChildAt(0).GetChildAt(1);
  auto              loading = Ui::Label::DownCast(labels.GetChildAt(0));
  auto              retry   = Ui::Label::DownCast(labels.GetChildAt(1));
  DALI_TEST_CHECK(loading && retry);
  const Dali::String defaultLoading = loading.GetText();
  const Dali::String defaultRetry   = retry.GetText();

  Property::Map            text;
  Ui::Text::FontAttributes font;
  font.SetSize(24.0f);
  font.SetWeight(Ui::Text::FontWeight::BOLD);
  view.SetLoadingText("Loading custom widget");
  view.SetLoadingTextFont(font);
  view.SetLoadingTextColor(Ui::UiColor(Color::RED));
  DALI_TEST_EQUALS(loading.GetText(), Dali::String("Loading custom widget"), TEST_LOCATION);
  DALI_TEST_EQUALS(loading.GetFontSize(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(loading.GetProperty<Vector4>(Ui::Label::Property::TEXT_COLOR), Color::RED, TEST_LOCATION);
  DALI_TEST_CHECK(loading.GetFontWeight() == Ui::Text::FontWeight::BOLD);

  text.Clear();
  text.Insert("textPixelSize", 28.0f);
  view.SetProperty(Ui::Integration::WidgetView::Property::LOADING_TEXT, text);
  DALI_TEST_EQUALS(loading.GetFontSize(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(loading.GetText(), Dali::String("Loading custom widget"), TEST_LOCATION);

  text.Clear();
  text.Insert("stateText", "Retry custom widget");
  text.Insert("textPixelSize", 26.0f);
  text.Insert("textColor", Color::GREEN);
  text.Insert("textVisible", true);
  font.SetSize(26.0f);
  view.SetRetryText("Retry custom widget");
  view.SetRetryTextFont(font);
  view.SetRetryTextColor(Ui::UiColor(Color::GREEN));
  view.SetRetryTextVisible(true);
  DALI_TEST_EQUALS(retry.GetText(), Dali::String("Retry custom widget"), TEST_LOCATION);
  DALI_TEST_EQUALS(retry.GetFontSize(), 26.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(retry.GetProperty<Vector4>(Ui::Label::Property::TEXT_COLOR), Color::GREEN, TEST_LOCATION);
  DALI_TEST_CHECK(view.IsRetryTextVisible());

  // Empty text restores the existing localization binding through either API.
  text.Clear();
  text.Insert("stateText", "");
  view.SetLoadingText("");
  view.SetRetryText("");
  DALI_TEST_EQUALS(loading.GetText(), defaultLoading, TEST_LOCATION);
  DALI_TEST_EQUALS(retry.GetText(), defaultRetry, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewEffectApiAndPropertyP(void)
{
  UiTestApplication application;
  auto              view  = NewWidget();
  auto              image = TestNativeImage::New(64, 64);
  auto              url   = Ui::ImageUrlUtils::GenerateUrl(image, true);
  Property::Map     properties;
  properties.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  properties.Insert(Ui::Integration::ImageVisual::Property::URL, url.GetUrl());
  auto visual = Ui::Integration::VisualFactory::Get().CreateVisual(properties);
  Ui::Internal::ViewDataImpl::Get(Ui::Internal::GetImplementation(view)).RegisterVisual(WidgetView::Property::WIDGET_ID, visual);

  Property::Map shader;
  shader.Insert("fragmentShader", "void main() { gl_FragColor = texture2D(sTexture, vTexCoord); }");
  Property::Map effect;
  effect.Insert("shader", shader);
  view.SetCustomShader("", shader["fragmentShader"].Get<Dali::String>());
  DALI_TEST_CHECK(Ui::GetImplementation(visual).IsUsingCustomShader());
  view.SetProperty(Ui::Integration::WidgetView::Property::EFFECT, Property::Map());
  DALI_TEST_CHECK(!Ui::GetImplementation(visual).IsUsingCustomShader());
  view.SetProperty(Ui::Integration::WidgetView::Property::EFFECT, effect);
  DALI_TEST_CHECK(Ui::GetImplementation(visual).IsUsingCustomShader());
  Property::Map invalid;
  invalid.Insert("shader", true);
  view.SetProperty(Ui::Integration::WidgetView::Property::EFFECT, invalid);
  DALI_TEST_CHECK(Ui::GetImplementation(visual).IsUsingCustomShader());
  view.ClearCustomShader();
  DALI_TEST_CHECK(!Ui::GetImplementation(visual).IsUsingCustomShader());
  END_TEST;
}

int UtcDaliWidgetViewRejectsDifferentAppIdN(void)
{
  UiTestApplication application;
  auto              first = NewWidget();
  DALI_TEST_CHECK(first);
  DALI_TEST_CHECK(!NewWidget("test.widget", "different.viewer"));
  DALI_TEST_CHECK(!NewWidget("test.widget", ""));
  first.Reset();
  DALI_TEST_CHECK(!NewWidget("test.widget", "different.viewer"));
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
  DALI_TEST_CHECK(NewWidget());
  END_TEST;
}

int UtcDaliWidgetViewFailedCreationRetainsInitializedEngineP(void)
{
  UiTestApplication application;
  DALI_TEST_CHECK(!NewWidget("test.widget", "invalid.viewer"));
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 1, TEST_LOCATION);
  DALI_TEST_CHECK(!NewWidget(""));
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 1, TEST_LOCATION);
  auto view = NewWidget();
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewParentOwnsViewP(void)
{
  UiTestApplication application;
  auto              parent = Actor::New();
  auto              view   = NewWidget();
  DALI_TEST_CHECK(view);
  WeakHandle<WidgetView> weak(view);
  parent.Add(view);
  view.Reset();
  DALI_TEST_CHECK(weak.GetHandle());
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 0, TEST_LOCATION);
  parent.Remove(weak.GetHandle());
  DALI_TEST_CHECK(!weak.GetHandle());
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewReusesEngineAfterLastReleaseP(void)
{
  UiTestApplication application;
  for(int cycle = 1; cycle <= 3; ++cycle)
  {
    auto view = NewWidget();
    DALI_TEST_CHECK(view);
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
    view.Reset();
    DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), cycle, TEST_LOCATION);
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliWidgetViewReleasesEngineOnApplicationShutdownP(void)
{
  // The retained engine belongs to the application, not a process-static handle.
  {
    UiTestApplication application;
    auto              view = NewWidget();
    DALI_TEST_CHECK(view);
    view.Reset();
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
    DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewInitialCatalogRegistrationCanReenterP(void)
{
  UiTestApplication  application;
  LocalizationLookup lookup;
  auto               target       = Actor::New();
  auto               localization = Ui::UiLocalizationManager::Get();
  localization.SetBindingResource(target, "text", "test.resource", "widget_viewer_dali",
                                  Ui::LocalizedStringCallback::New(&lookup, &LocalizationLookup::Apply));
  lookup.enabled = true;
  auto view      = NewWidget();
  // The catalog is published before refreshing bindings; nested creation reuses it.
  DALI_TEST_CHECK(view && lookup.view);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_CREATED), 2, TEST_LOCATION);
  localization.ClearBindings(target);
  END_TEST;
}

int UtcDaliWidgetViewDestroyBeforeInitialCallbackP(void)
{
  UiTestApplication application;
  Signals           signals;
  auto              view = NewWidget();
  DALI_TEST_CHECK(view);
  view.WidgetAddedSignal().Connect(&signals, &Signals::Added);
  view.WidgetDeletedSignal().Connect(&signals, &Signals::Deleted);
  DALI_TEST_EQUALS(signals.added, 0, TEST_LOCATION);
  view.Reset();
  DALI_TEST_EQUALS(signals.added, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.deleted, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewFaultRetryWithoutManagerP(void)
{
  UiTestApplication application;
  Signals           signals;
  auto              view = NewWidget();
  DALI_TEST_CHECK(view);
  view.WidgetAddedSignal().Connect(&signals, &Signals::Added);
  view.WidgetDeletedSignal().Connect(&signals, &Signals::Deleted);
  view.WidgetFaultedSignal().Connect(&signals, &Signals::Faulted);
  DALI_TEST_EQUALS(signals.added, 0, TEST_LOCATION);
  application.RunIdles();
  DALI_TEST_EQUALS(signals.added, 1, TEST_LOCATION);
  emitEvent(0, Event::FAULTED);
  DALI_TEST_EQUALS(signals.deleted, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.faulted, 1, TEST_LOCATION);
  view.ActivateFaultedWidget();
  DALI_TEST_EQUALS(CountSinceStartup(RETRIES), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.added, 2, TEST_LOCATION);
  view.ActivateFaultedWidget();
  DALI_TEST_EQUALS(CountSinceStartup(RETRIES), 1, TEST_LOCATION);
  view.Reset();
  DALI_TEST_EQUALS(signals.deleted, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewRetryStateBlocksTouchSequenceP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.SetPivot(Pivot::TOP_LEFT);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  application.RunIdles();

  ProcessTouch(application, PointState::DOWN, 100u);
  ProcessTouch(application, PointState::UP, 110u);
  DALI_TEST_EQUALS(getTouchEventCount(0), 2, TEST_LOCATION);

  emitEvent(0, Event::FAULTED);
  const int touchCount = getTouchEventCount(0);
  ProcessTouch(application, PointState::DOWN, 200u);
  ProcessTouch(application, PointState::MOTION, 210u);
  ProcessTouch(application, PointState::UP, 220u);

  // Retry is activated on UP, but no part of that sequence reaches the provider.
  DALI_TEST_EQUALS(getTouchEventCount(0), touchCount, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(RETRIES), 1, TEST_LOCATION);

  // ActivateFaultedWidget hides the retry state, so later input is routed normally.
  ProcessTouch(application, PointState::DOWN, 300u);
  ProcessTouch(application, PointState::UP, 310u);
  DALI_TEST_EQUALS(getTouchEventCount(0), touchCount + 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewRetainsFrameUntilDestructionP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  DALI_TEST_CHECK(view);
  application.GetScene().Add(view);
  application.RunIdles();
  auto image = TestNativeImage::New(64, 64);
  {
    // Supply a native image without a platform graphics backend. Disconnection
    // retains it for reattachment; destruction releases it even if the image
    // itself is still held by a buffer queue.
    auto          url = Ui::ImageUrlUtils::GenerateUrl(image, true);
    Property::Map properties;
    properties.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
    properties.Insert(Ui::Integration::ImageVisual::Property::URL, url.GetUrl());
    properties.Insert(Ui::Integration::ImageVisual::Property::RELEASE_POLICY, Ui::Image::ReleasePolicy::DESTROYED);
    auto visual = Ui::Integration::VisualFactory::Get().CreateVisual(properties);
    Ui::Internal::ViewDataImpl::Get(Ui::Internal::GetImplementation(view)).RegisterVisual(WidgetView::Property::WIDGET_ID, visual);
  }
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  view.Unparent();
  for(int frame = 0; frame < 4; ++frame)
  {
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(image->mExtensionDestroyCalls, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 0, TEST_LOCATION);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  view.Unparent();
  view.Reset();
  for(int frame = 0; frame < 4; ++frame)
  {
    application.RunIdles();
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(image->mExtensionDestroyCalls, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewSceneConnectionControlsPauseP(void)
{
  UiTestApplication application;
  Signals           signals;
  auto              view = NewWidget();
  DALI_TEST_CHECK(view);
  auto parent = Ui::View::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(200, 200));
  parent.SetRequestedWidth(200);
  parent.SetRequestedHeight(200);
  view.WidgetDeletedSignal().Connect(&signals, &Signals::Deleted);
  DALI_TEST_CHECK(isPaused(0));
  parent.Add(view);
  // Adding to a parent that is still off scene must not resume the widget.
  DALI_TEST_CHECK(isPaused(0));
  const auto instanceId = view.GetProperty<Dali::String>(WidgetView::Property::INSTANCE_ID);
  application.GetScene().Add(parent);
  for(int frame = 0; frame < 3; ++frame)
  {
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(!isPaused(0));
  view.Unparent();
  DALI_TEST_CHECK(isPaused(0));
  DALI_TEST_EQUALS(signals.deleted, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 0, TEST_LOCATION);
  parent.Add(view);
  for(int frame = 0; frame < 3; ++frame)
  {
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(!isPaused(0));
  DALI_TEST_EQUALS(view.GetProperty<Dali::String>(WidgetView::Property::INSTANCE_ID), instanceId, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_CREATED), 1, TEST_LOCATION);

  // An explicit manual pause survives reattachment until the app resumes it.
  view.PauseWidget();
  view.Unparent();
  parent.Add(view);
  DALI_TEST_CHECK(isPaused(0));
  view.ResumeWidget();
  DALI_TEST_CHECK(!isPaused(0));
  application.GetScene().Remove(parent);
  DALI_TEST_CHECK(isPaused(0));
  view.Unparent();
  view.Reset();
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewParentRotationUpdatesPauseP(void)
{
  UiTestApplication application;
  auto              view = WidgetView::New(APP_ID, "test.widget", "", 300, 20, 0.0f);
  DALI_TEST_CHECK(view);
  // Rotate about the widget centre so position, size and scale stay unchanged.
  auto parent = Actor::New();
  parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  parent.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  parent.SetProperty(Actor::Property::SIZE, Vector2(300, 20));
  parent.SetProperty(Actor::Property::POSITION, Vector3(160, -50, 0));
  parent.Add(view);
  application.GetScene().Add(parent);

  const auto render = [&application]()
  {
    for(int frame = 0; frame < 10; ++frame)
    {
      application.RunIdles();
      application.SendNotification();
      application.Render();
    }
  };
  render();
  auto bounds = DevelActor::CalculateCurrentScreenExtents(view);
  DALI_TEST_CHECK(bounds.y + bounds.height < 0.0f);
  DALI_TEST_CHECK(isPaused(0));

  parent.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(90), Vector3::ZAXIS));
  render();
  bounds = DevelActor::CalculateCurrentScreenExtents(view);
  DALI_TEST_CHECK(bounds.y + bounds.height > 0.0f);
  DALI_TEST_CHECK(!isPaused(0));

  parent.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(0), Vector3::ZAXIS));
  render();
  DALI_TEST_CHECK(isPaused(0));

  // Rotation must not override an explicit pause request.
  view.PauseWidget();
  parent.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(90), Vector3::ZAXIS));
  render();
  DALI_TEST_CHECK(isPaused(0));
  DALI_TEST_CHECK(view.ResumeWidget());
  DALI_TEST_CHECK(!isPaused(0));
  END_TEST;
}

int UtcDaliWidgetViewNaturalSizeIsIndependentOfArrangeP(void)
{
  UiTestApplication application;
  auto              view = WidgetView::New(APP_ID, "test.widget", "", 100, 60, 0.0f);
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  auto measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 200.0f, 120.0f));
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 0), 200, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 1), 120, TEST_LOCATION);

  view.SetRequestedWidth(300.0f);
  view.SetRequestedHeight(180.0f);
  measured = view.Measure(500.0f, 400.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);

  // A temporary parent constraint must not overwrite intrinsic content size.
  measured = view.Measure(80.0f, 50.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewNaturalSizeScalesWithoutProviderFeedbackP(void)
{
  UiTestApplication application;
  auto              scaleManager  = Ui::UiScaleManager::Get();
  const float       originalScale = scaleManager.GetScale();
  scaleManager.SetScale(1.3f);
  auto view = WidgetView::New(APP_ID, "test.widget", "", 100, 60, 0.0f);
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  view.Measure(500.0f, 400.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 130.5f, 78.5f));
  const int resizeCount = getDimension(0, 2);
  for(int i = 0; i < 20; ++i)
  {
    auto measured = view.Measure(500.0f, 400.0f);
    DALI_TEST_EQUALS(measured.width, 130.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(measured.height, 78.0f, 0.001f, TEST_LOCATION);
    view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 130.5f, 78.5f));
  }
  DALI_TEST_EQUALS(getDimension(0, 0), 130, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 1), 78, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), resizeCount, TEST_LOCATION);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 130.75f, 78.75f));
  auto measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 130.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 78.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), resizeCount, TEST_LOCATION);
  application.GetScene().Add(view);
  scaleManager.SetScale(2.0f);
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 120.0f, 0.001f, TEST_LOCATION);
  scaleManager.SetScale(1.0f);
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, 0.001f, TEST_LOCATION);
  scaleManager.SetScale(originalScale);
  END_TEST;
}

int UtcDaliWidgetViewKeepSizeOnlyFreezesProviderP(void)
{
  UiTestApplication application;
  auto              view = WidgetView::New(APP_ID, "test.widget", "", 100, 60, 0.0f);
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  view.Measure(500.0f, 400.0f);
  view.SetKeepWidgetSizeEnabled(true);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 200.0f, 120.0f));
  auto measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), 0, TEST_LOCATION);

  // Disabling KEEP_WIDGET_SIZE alone must not synchronize the provider.
  view.SetKeepWidgetSizeEnabled(false);
  DALI_TEST_EQUALS(getDimension(0, 2), 0, TEST_LOCATION);
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 201.0f, 121.0f));
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 0), 201, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 1), 121, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), 1, TEST_LOCATION);

  // KEEP preserves logical natural size even when the display scale changes.
  auto        scaleManager  = Ui::UiScaleManager::Get();
  const float originalScale = scaleManager.GetScale();
  view.SetKeepWidgetSizeEnabled(true);
  application.GetScene().Add(view);
  scaleManager.SetScale(2.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 800.0f, 480.0f));
  measured = view.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(measured.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), 1, TEST_LOCATION);
  scaleManager.SetScale(originalScale);
  END_TEST;
}

int UtcDaliWidgetViewNaturalSizeSettlesOnSceneP(void)
{
  UiTestApplication application;
  auto              scaleManager  = Ui::UiScaleManager::Get();
  const float       originalScale = scaleManager.GetScale();
  scaleManager.SetScale(1.0f);
  auto view = WidgetView::New(APP_ID, "test.widget", "", 100, 60, 0.0f);
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  application.GetScene().Add(view);
  const auto settle = [&application]()
  {
    for(int i = 0; i < 10; ++i)
    {
      application.RunIdles();
      application.SendNotification();
      application.Render();
    }
  };
  settle();
  scaleManager.SetScale(1.3f);
  settle();
  auto size = view.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_EQUALS(size.x, 130.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.y, 78.0f, 0.001f, TEST_LOCATION);
  const int resizeCount = getDimension(0, 2);
  settle();
  DALI_TEST_EQUALS(getDimension(0, 2), resizeCount, TEST_LOCATION);
  scaleManager.SetScale(1.0f);
  settle();
  size = view.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_EQUALS(size.x, 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.y, 60.0f, 0.001f, TEST_LOCATION);
  scaleManager.SetScale(originalScale);
  END_TEST;
}

int UtcDaliWidgetViewOverlaysFollowParentLayoutP(void)
{
  UiTestApplication application;
  auto              scaleManager  = Ui::UiScaleManager::Get();
  const float       originalScale = scaleManager.GetScale();
  scaleManager.SetScale(1.3f);
  auto view = NewWidget();

  const auto checkOverlays = [&view](const Vector2& expected)
  {
    auto previewContainer = view.GetChildAt(0);
    auto preview          = previewContainer.GetChildAt(0);
    auto textContainer    = previewContainer.GetChildAt(1);
    for(auto child : {previewContainer, preview, textContainer, textContainer.GetChildAt(0), textContainer.GetChildAt(1)})
    {
      const auto size = child.GetProperty<Vector3>(Actor::Property::SIZE);
      DALI_TEST_EQUALS(size.x, expected.x, 0.001f, TEST_LOCATION);
      DALI_TEST_EQUALS(size.y, expected.y, 0.001f, TEST_LOCATION);
    }
  };

  view.Measure(500.0f, 400.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 260.0f, 156.0f));
  checkOverlays(Vector2(260.0f, 156.0f));
  view.Measure(500.0f, 400.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 195.0f, 104.0f));
  checkOverlays(Vector2(195.0f, 104.0f));
  DALI_TEST_EQUALS(getDimension(0, 0), 195, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 1), 104, TEST_LOCATION);

  scaleManager.SetScale(originalScale);
  END_TEST;
}

int UtcDaliWidgetEngineSharedGetOwnsApplicationLifetimeP(void)
{
  {
    UiTestApplication application;
    auto              first  = WidgetEngine::Get(APP_ID);
    auto              second = WidgetEngine::Get(APP_ID);
    DALI_TEST_CHECK(first && first == second);
    DALI_TEST_CHECK(!WidgetEngine::Get("another.viewer"));
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
    first.Reset();
    second.Reset();
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewIdleRegistrationFailureReturnsEmptyP(void)
{
  UiTestApplication application;
  UiTestApplication::ADD_IDLE_SUCCESS = false;
  auto view                           = NewWidget();
  UiTestApplication::ADD_IDLE_SUCCESS = true;
  DALI_TEST_CHECK(!view);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewCatalogIsNotRegisteredPerViewP(void)
{
  UiTestApplication application;
  auto first = NewWidget();
  DALI_TEST_CHECK(first);
  auto localization = Ui::UiLocalizationManager::Get();
  auto target = Actor::New();
  LocalizationLookup lookup;
  localization.SetBindingResource(target, "text", "test.resource", "widget_viewer_dali",
                                  Ui::LocalizedStringCallback::New(&lookup, &LocalizationLookup::Apply));
  lookup.enabled = true;
  auto second = NewWidget();
  DALI_TEST_CHECK(second && !lookup.view);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_CREATED), 2, TEST_LOCATION);
  localization.ClearBindings(target);
  END_TEST;
}
