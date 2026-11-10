/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <algorithm>
#include <array>
#include <string>
#include <vector>

#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/internal/controls/text-controls/text-selection-popup-callback-interface.h>
#include <dali-ui-foundation/internal/text/decorator/text-decorator.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <cmath>
#include <dali-ui/ui-timer.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::Extension::LayoutScaleManager;

namespace
{
std::string GetRepositoryResourcePath(const char* relativePath)
{
#if defined(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR)
  std::string testResourceDirectory(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR);
#else
  std::string testResourceDirectory(__FILE__);
#endif
  std::replace(testResourceDirectory.begin(), testResourceDirectory.end(), '\\', '/');
  const std::string marker("/automated-tests/");
  const std::size_t markerOffset = testResourceDirectory.rfind(marker);
  return markerOffset == std::string::npos
           ? std::string(relativePath)
           : testResourceDirectory.substr(0u, markerOffset + 1u) + relativePath;
}

struct DecorationEventRecord
{
  Text::HandleType  type;
  Text::HandleState state;
};

class TestDecoratorController : public Text::Decorator::ControllerInterface,
                                public TextSelectionPopupCallbackInterface
{
public:
  explicit TestDecoratorController(Actor root)
  : mRoot(root)
  {
  }

  void GetTargetSize(Vector2& targetSize) override
  {
    targetSize = Vector2(240.0f, 160.0f);
  }

  void AddDecoration(Actor& actor, Ui::Integration::Text::DecorationType, bool) override
  {
    mRoot.Add(actor);
  }

  void DecorationEvent(Text::HandleType type, Text::HandleState state, float, float) override
  {
    mEvents.push_back({type, state});
  }

  void TextPopupButtonTouched(Text::InputCommandType) override
  {
  }

  Actor                              mRoot;
  std::vector<DecorationEventRecord> mEvents;
};

struct NoopRelayoutContainer : public RelayoutContainer
{
  void Add(const Actor&, const Vector2&) override
  {
  }
};

void CollectImageViews(Actor actor, std::vector<ImageView>& imageViews)
{
  ImageView imageView = ImageView::DownCast(actor);
  if(imageView)
  {
    imageViews.push_back(imageView);
  }

  for(uint32_t index = 0u; index < actor.GetChildCount(); ++index)
  {
    CollectImageViews(actor.GetChildAt(index), imageViews);
  }
}

void CollectViews(Actor actor, std::vector<Ui::View>& views)
{
  Ui::View view = Ui::View::DownCast(actor);
  if(view)
  {
    views.push_back(view);
  }

  for(uint32_t index = 0u; index < actor.GetChildCount(); ++index)
  {
    CollectViews(actor.GetChildAt(index), views);
  }
}
} // namespace

void utc_dali_text_decorator_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_decorator_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextDecoratorHandlesReceivePanWithHitTestModesP(void)
{
  UiTestApplication application;

  struct HandleCase
  {
    Text::HandleType type;
    std::string      image;
    std::string      pressedImage;
    float            x;
  };
  const std::array<HandleCase, 3u> handles{{
    {Text::GRAB_HANDLE,
     GetRepositoryResourcePath("samples/text/res/cursor_handle.png"),
     GetRepositoryResourcePath("samples/text/res/cursor_handle_pressed.png"),
     60.0f},
    {Text::LEFT_SELECTION_HANDLE,
     GetRepositoryResourcePath("samples/text/res/selection_handle_left.png"),
     GetRepositoryResourcePath("samples/text/res/selection_handle_left_pressed.png"),
     120.0f},
    {Text::RIGHT_SELECTION_HANDLE,
     GetRepositoryResourcePath("samples/text/res/selection_handle_right.png"),
     GetRepositoryResourcePath("samples/text/res/selection_handle_right_pressed.png"),
     180.0f},
  }};

  const std::array<bool, 2u> geometryHitTestModes{{false, true}};
  for(bool geometryHitTest : geometryHitTestModes)
  {
    // Gesture detectors select their processing path when their actors enter the scene.
    // Set the mode before creating the handles so both paths are tested independently.
    application.GetScene().SetGeometryHittestEnabled(geometryHitTest);

    Actor root = Actor::New();
    root.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    root.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    root.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 160.0f));
    application.GetScene().Add(root);

    TestDecoratorController controller(root);
    Text::DecoratorPtr      decorator = Text::Decorator::New(controller, controller);
    decorator->SetBoundingBox(BoundsInteger(0, 0, 240, 160));

    for(const HandleCase& handle : handles)
    {
      decorator->SetHandleImage(handle.type, Text::HANDLE_IMAGE_RELEASED, handle.image);
      decorator->SetHandleImage(handle.type, Text::HANDLE_IMAGE_PRESSED, handle.pressedImage);
      decorator->SetPosition(handle.type, handle.x, 40.0f, 20.0f);
      decorator->SetHandleActive(handle.type, true);
    }

    NoopRelayoutContainer relayoutContainer;
    decorator->Relayout(Vector2(240.0f, 160.0f), relayoutContainer);
    application.SendNotification();
    application.Render();

    std::vector<ImageView> handleViews;
    CollectImageViews(root, handleViews);
    DALI_TEST_EQUALS(handleViews.size(), handles.size(), TEST_LOCATION);
    std::sort(handleViews.begin(), handleViews.end(), [](const ImageView& lhs, const ImageView& rhs)
    {
      return lhs.CalculateScreenBounds().x < rhs.CalculateScreenBounds().x;
    });
    for(size_t index = 0u; index < handles.size(); ++index)
    {
      const HandleCase& handleCase = handles[index];
      const ImageView&  handle     = handleViews[index];

      const Bounds screenExtents = handle.CalculateScreenBounds();
      Vector2      panStart(screenExtents.x + 0.5f * screenExtents.width,
                            screenExtents.y + 0.5f * screenExtents.height);
      const Insets touchHitAreaMargin =
        handle.GetProperty<Insets>(Actor::Property::TOUCH_HIT_AREA_MARGIN);
      DALI_TEST_CHECK(touchHitAreaMargin.start > 0.0f);
      DALI_TEST_CHECK(touchHitAreaMargin.bottom > 0.0f);
      panStart.x = screenExtents.x - 0.5f * touchHitAreaMargin.start;

      const Vector2      panEnd(panStart.x + 30.0f, panStart.y);
      const Dali::String releasedImage = handle.GetResourceUrl();

      controller.mEvents.clear();
      uint32_t time = 100u;
      TestStartPan(application, panStart, panEnd, time);
      DALI_TEST_CHECK(handle.GetResourceUrl() != releasedImage);
      const Dali::String pressedImage = handle.GetResourceUrl();
      TestEndPan(application, panEnd, time);

      DALI_TEST_CHECK(!controller.mEvents.empty());
      DALI_TEST_EQUALS(controller.mEvents.size(), 2u, TEST_LOCATION);
      DALI_TEST_EQUALS(controller.mEvents.front().type, handleCase.type, TEST_LOCATION);
      DALI_TEST_EQUALS(controller.mEvents.front().state, Text::HANDLE_PRESSED, TEST_LOCATION);
      DALI_TEST_EQUALS(controller.mEvents.back().type, handleCase.type, TEST_LOCATION);
      DALI_TEST_EQUALS(controller.mEvents.back().state, Text::HANDLE_RELEASED, TEST_LOCATION);
      DALI_TEST_CHECK(handle.GetResourceUrl() != pressedImage);
    }
    application.GetScene().Remove(root);
  }

  END_TEST;
}

int UtcDaliTextDecoratorCursorHighlightAndPopupStateP(void)
{
  UiTestApplication application;
  Actor root = Actor::New();
  root.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 160.0f));
  application.GetScene().Add(root);

  TestDecoratorController controller(root);
  Text::DecoratorPtr decorator = Text::Decorator::New(controller, controller);
  NoopRelayoutContainer container;

  decorator->SetBoundingBox(BoundsInteger(0, 0, 240, 160));
  BoundsInteger bounds;
  decorator->GetBoundingBox(bounds);
  DALI_TEST_EQUALS(bounds.width, 240, TEST_LOCATION);
  DALI_TEST_EQUALS(bounds.height, 160, TEST_LOCATION);

  decorator->SetUiScale(1.5f);
  DALI_TEST_EQUALS(decorator->GetUiScale(), 1.5f, TEST_LOCATION);
  decorator->SetCursorWidth(2);
  DALI_TEST_EQUALS(decorator->GetCursorWidth(), 2, TEST_LOCATION);
  DALI_TEST_CHECK(decorator->GetEffectiveCursorWidth() > 0.0f);
  decorator->SetCursorBlinkInterval(0.4f);
  DALI_TEST_EQUALS(decorator->GetCursorBlinkInterval(), 0.4f, TEST_LOCATION);

  for(Text::Cursor cursor : {Text::PRIMARY_CURSOR, Text::SECONDARY_CURSOR})
  {
    decorator->SetPosition(cursor, 30.0f, 40.0f, 18.0f, 20.0f);
    decorator->SetVisualCursorGeometry(cursor, 32.0f, 40.0f, 18.0f);
    decorator->SetGlyphOffset(cursor, 3.0f);
    DALI_TEST_EQUALS(decorator->GetGlyphOffset(cursor), 3.0f, TEST_LOCATION);
    decorator->SetCursorColor(cursor, Vector4(0.3f, 0.4f, 0.5f, 1.0f));
    DALI_TEST_EQUALS(decorator->GetColor(cursor), Vector4(0.3f, 0.4f, 0.5f, 1.0f), TEST_LOCATION);
    float x;
    float y;
    float height;
    float lineHeight;
    decorator->GetPosition(cursor, x, y, height, lineHeight);
    DALI_TEST_EQUALS(x, 30.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(lineHeight, 20.0f, TEST_LOCATION);
  }

  decorator->SetActiveCursor(Text::ACTIVE_CURSOR_BOTH);
  decorator->StartCursorBlink();
  decorator->DelayCursorBlink();
  decorator->StopCursorBlink();

  decorator->SetHandleColor(Vector4(1.0f, 0.5f, 0.0f, 1.0f));
  DALI_TEST_EQUALS(decorator->GetHandleColor(), Vector4(1.0f, 0.5f, 0.0f, 1.0f), TEST_LOCATION);
  for(Text::HandleType handle : {Text::GRAB_HANDLE, Text::LEFT_SELECTION_HANDLE, Text::RIGHT_SELECTION_HANDLE})
  {
    decorator->SetPosition(handle, 40.0f, 50.0f, 18.0f);
    decorator->SetHandleActive(handle, true);
    DALI_TEST_CHECK(decorator->IsHandleActive(handle));
    decorator->FlipHandleVertically(handle, true);
    DALI_TEST_CHECK(decorator->IsHandleVerticallyFlipped(handle));
    decorator->FlipHandleVertically(handle, false);
    float x;
    float y;
    float lineHeight;
    decorator->GetPosition(handle, x, y, lineHeight);
    DALI_TEST_EQUALS(x, 40.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(y, 50.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(lineHeight, 18.0f, TEST_LOCATION);
  }
  decorator->FlipSelectionHandlesOnCrossEnabled(true);
  decorator->SetSelectionHandleFlipState(true, true, false);

  decorator->ResizeHighlightQuads(2u);
  decorator->AddHighlight(0u, Vector4(10.0f, 10.0f, 35.0f, 25.0f));
  decorator->AddHighlight(1u, Vector4(40.0f, 10.0f, 65.0f, 25.0f));
  decorator->SetHighLightBox(Vector2(10.0f, 10.0f), Size(55.0f, 15.0f), 0.0f);
  decorator->SetHighlightColor(Vector4(0.0f, 0.0f, 1.0f, 0.5f));
  DALI_TEST_EQUALS(decorator->GetHighlightColor(), Vector4(0.0f, 0.0f, 1.0f, 0.5f), TEST_LOCATION);
  decorator->SetHighlightActive(true);
  DALI_TEST_CHECK(decorator->IsHighlightActive());
  decorator->SetPopupActive(true);
  DALI_TEST_CHECK(decorator->IsPopupActive());

  decorator->SetScrollThreshold(12.0f);
  DALI_TEST_EQUALS(decorator->GetScrollThreshold(), 12.0f, TEST_LOCATION);
  decorator->SetScrollSpeed(45.0f);
  DALI_TEST_EQUALS(decorator->GetScrollSpeed(), 45.0f, TEST_LOCATION);
  decorator->SetHorizontalScrollEnabled(true);
  decorator->SetVerticalScrollEnabled(true);
  decorator->SetSmoothHandlePanEnabled(true);
  DALI_TEST_CHECK(decorator->IsHorizontalScrollEnabled());
  DALI_TEST_CHECK(decorator->IsVerticalScrollEnabled());
  DALI_TEST_CHECK(decorator->IsSmoothHandlePanEnabled());

  decorator->Relayout(Vector2(240.0f, 160.0f), container);
  decorator->UpdatePositions(Vector2(4.0f, 5.0f));
  application.SendNotification();
  application.Render();

  decorator->ClearHighlights();
  decorator->SetHighlightActive(false);
  decorator->SetPopupActive(false);
  decorator->SetActiveCursor(Text::ACTIVE_CURSOR_NONE);
  decorator->NotifyEndOfScroll();
  decorator->Relayout(Vector2(240.0f, 160.0f), container);
  DALI_TEST_CHECK(!decorator->IsHighlightActive());
  DALI_TEST_CHECK(!decorator->IsPopupActive());
  END_TEST;
}

int UtcDaliTextDecoratorHandleEdgeAutoScrollP(void)
{
  UiTestApplication application;
  struct EdgeCase
  {
    Vector2 delta;
    bool horizontal;
    bool vertical;
  };
  const std::array<EdgeCase, 4u> cases{{
    {Vector2(-110.0f, 0.0f), true, false},
    {Vector2(110.0f, 0.0f), true, false},
    {Vector2(0.0f, -70.0f), false, true},
    {Vector2(0.0f, 70.0f), false, true},
  }};

  for(const EdgeCase& edge : cases)
  {
    Actor root = Actor::New();
    root.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    root.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    root.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 160.0f));
    application.GetScene().Add(root);

    TestDecoratorController controller(root);
    Text::DecoratorPtr decorator = Text::Decorator::New(controller, controller);
    decorator->SetBoundingBox(BoundsInteger(0, 0, 240, 160));
    decorator->SetHandleImage(Text::GRAB_HANDLE,
                              Text::HANDLE_IMAGE_RELEASED,
                              GetRepositoryResourcePath("samples/text/res/cursor_handle.png"));
    decorator->SetHandleImage(Text::GRAB_HANDLE,
                              Text::HANDLE_IMAGE_PRESSED,
                              GetRepositoryResourcePath("samples/text/res/cursor_handle_pressed.png"));
    decorator->SetPosition(Text::GRAB_HANDLE, 120.0f, 65.0f, 20.0f);
    decorator->SetHandleActive(Text::GRAB_HANDLE, true);
    decorator->SetScrollThreshold(25.0f);
    decorator->SetScrollSpeed(60.0f);
    decorator->SetHorizontalScrollEnabled(edge.horizontal);
    decorator->SetVerticalScrollEnabled(edge.vertical);

    NoopRelayoutContainer container;
    decorator->Relayout(Vector2(240.0f, 160.0f), container);
    application.SendNotification();
    application.Render();

    std::vector<ImageView> views;
    CollectImageViews(root, views);
    DALI_TEST_EQUALS(views.size(), 1u, TEST_LOCATION);
    const Bounds extents = views.front().CalculateScreenBounds();
    const Vector2 start(extents.x + 0.5f * extents.width, extents.y + 0.5f * extents.height);
    const Vector2 end = start + edge.delta;
    uint32_t time = 100u;
    TestStartPan(application, start, end, time);
    Test::EmitGlobalTimerSignal();
    float anchorX = 0.0f;
    float anchorY = 0.0f;
    decorator->GetScrollingAnchor(anchorX, anchorY);
    DALI_TEST_CHECK(std::isfinite(anchorX));
    DALI_TEST_CHECK(std::isfinite(anchorY));
    TestEndPan(application, end, time);
    DALI_TEST_CHECK(std::any_of(controller.mEvents.begin(),
                                controller.mEvents.end(),
                                [](const DecorationEventRecord& event)
                                {
                                  return event.state == Text::HANDLE_SCROLLING;
                                }));
    DALI_TEST_CHECK(std::any_of(controller.mEvents.begin(),
                                controller.mEvents.end(),
                                [](const DecorationEventRecord& event)
                                {
                                  return event.state == Text::HANDLE_STOP_SCROLLING;
                                }));
    application.GetScene().Remove(root);
  }
  END_TEST;
}

// Every decoration is handed geometry the text controller has ALREADY scaled, so every
// View the decorator creates -- the cursor, the grab handle, the two selection handles
// and their two markers -- opts out of the per-window scale as well as the system
// scale. The cursor and the handles are layout roots here (their parents are the
// decorator's plain-Actor layers), so a window scale they kept would multiply their
// requested size and position a second time.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no build
// was performed for this change): dropping the WindowScalePolicy line at the cursor,
// grab-handle, primary-handle or secondary-handle creation site makes that view read
// 2.0 below. The marker line is mutation-equivalent -- a marker is parented to its
// handle and inherits the handle's opt-out -- so the code, not this check, carries it.
int UtcDaliTextDecoratorDecorationsIgnoreWindowScaleP(void)
{
  UiTestApplication application;
  tet_infoline("Every View the decorator creates resolves to scale 1.0 under a window scale");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  // Anti-vacuity: an application view in the same window does take the window scale,
  // so every 1.0 below is an opt-out and not a missing window scale.
  Ui::View plain = Ui::View::New();
  plain.SetRequestedWidth(10.0f);
  plain.SetRequestedHeight(10.0f);
  application.GetScene().Add(plain);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(plain)), 2.0f, 0.001f, TEST_LOCATION);

  Actor root = Actor::New();
  root.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  root.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  root.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 160.0f));
  application.GetScene().Add(root);

  TestDecoratorController controller(root);
  Text::DecoratorPtr      decorator = Text::Decorator::New(controller, controller);
  decorator->SetBoundingBox(BoundsInteger(0, 0, 240, 160));

  // The three handles as UtcDaliTextDecoratorHandlesReceivePanWithHitTestModesP builds
  // them, plus the two selection-handle markers and the primary cursor.
  struct HandleCase
  {
    Text::HandleType type;
    std::string      image;
    std::string      pressedImage;
    float            x;
  };
  const std::array<HandleCase, 3u> handles{{
    {Text::GRAB_HANDLE,
     GetRepositoryResourcePath("samples/text/res/cursor_handle.png"),
     GetRepositoryResourcePath("samples/text/res/cursor_handle_pressed.png"),
     60.0f},
    {Text::LEFT_SELECTION_HANDLE,
     GetRepositoryResourcePath("samples/text/res/selection_handle_left.png"),
     GetRepositoryResourcePath("samples/text/res/selection_handle_left_pressed.png"),
     120.0f},
    {Text::RIGHT_SELECTION_HANDLE,
     GetRepositoryResourcePath("samples/text/res/selection_handle_right.png"),
     GetRepositoryResourcePath("samples/text/res/selection_handle_right_pressed.png"),
     180.0f},
  }};
  for(const HandleCase& handle : handles)
  {
    decorator->SetHandleImage(handle.type, Text::HANDLE_IMAGE_RELEASED, handle.image);
    decorator->SetHandleImage(handle.type, Text::HANDLE_IMAGE_PRESSED, handle.pressedImage);
    decorator->SetPosition(handle.type, handle.x, 40.0f, 20.0f);
    decorator->SetHandleActive(handle.type, true);
  }
  decorator->SetHandleImage(Text::LEFT_SELECTION_HANDLE_MARKER,
                            Text::HANDLE_IMAGE_RELEASED,
                            GetRepositoryResourcePath("samples/text/res/selection_handle_left.png"));
  decorator->SetHandleImage(Text::RIGHT_SELECTION_HANDLE_MARKER,
                            Text::HANDLE_IMAGE_RELEASED,
                            GetRepositoryResourcePath("samples/text/res/selection_handle_right.png"));

  decorator->SetPosition(Text::PRIMARY_CURSOR, 30.0f, 40.0f, 18.0f, 20.0f);
  decorator->SetVisualCursorGeometry(Text::PRIMARY_CURSOR, 32.0f, 40.0f, 18.0f);
  decorator->SetActiveCursor(Text::ACTIVE_CURSOR_PRIMARY);

  NoopRelayoutContainer relayoutContainer;
  decorator->Relayout(Vector2(240.0f, 160.0f), relayoutContainer);
  application.SendNotification();
  application.Render();

  // The decorator creates Views at five sites -- the cursor, the grab handle, the two
  // selection handles, and the marker, once per selection handle -- so the tree holds
  // exactly six.
  std::vector<Ui::View> views;
  CollectViews(root, views);
  DALI_TEST_EQUALS(views.size(), 6u, TEST_LOCATION);
  for(Ui::View& view : views)
  {
    DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(GetImpl(view)), 1.0f, 0.001f, TEST_LOCATION);
    // Pins each creation site's own policy line, the markers included: a marker inherits
    // its handle's opt-out, so the 1.0 above alone cannot tell a dropped marker line.
    DALI_TEST_CHECK(Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(view)).GetWindowScalePolicy() == Dali::Ui::Internal::WindowScalePolicy::DISABLED);
  }

  application.GetScene().Remove(root);
  application.GetScene().Remove(plain);
  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}
