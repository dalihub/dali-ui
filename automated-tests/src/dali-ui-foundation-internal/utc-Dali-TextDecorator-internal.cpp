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

#include <dali-ui-foundation/internal/controls/text-controls/text-selection-popup-callback-interface.h>
#include <dali-ui-foundation/internal/text/decorator/text-decorator.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

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
      return lhs.CalculateScreenExtents().x < rhs.CalculateScreenExtents().x;
    });
    for(size_t index = 0u; index < handles.size(); ++index)
    {
      const HandleCase& handleCase = handles[index];
      const ImageView&  handle     = handleViews[index];

      const Bounds screenExtents = handle.CalculateScreenExtents();
      Vector2      panStart(screenExtents.x + 0.5f * screenExtents.width,
                            screenExtents.y + 0.5f * screenExtents.height);
      const Extents touchHitAreaMargin =
        handle.GetProperty<Extents>(Actor::Property::TOUCH_HIT_AREA_MARGIN);
      DALI_TEST_CHECK(touchHitAreaMargin.start > 0);
      DALI_TEST_CHECK(touchHitAreaMargin.bottom > 0);
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
