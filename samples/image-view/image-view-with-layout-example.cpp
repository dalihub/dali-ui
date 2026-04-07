/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/image-view.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int     IMAGE_COUNT   = 4;
const char* const IMAGE_URLS[IMAGE_COUNT] = {
  RESOURCES_DIR "gallery-large-3.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "people-small-10.jpg",
};
} // namespace

/**
 * ImageView with Layout sample: demonstrates 4 ImageViews arranged in a 2x2
 * grid using nested StackLayouts (vertical root -> 2 horizontal rows).
 *
 * Layout structure:
 *   root (vertical StackLayout, MATCH_PARENT x MATCH_PARENT)
 *   ├── topRow    (horizontal StackLayout, MATCH_PARENT x weight 1)
 *   │   ├── images[0]  (weight 1 x MATCH_PARENT)
 *   │   └── images[1]  (weight 1 x MATCH_PARENT)
 *   └── bottomRow (horizontal StackLayout, MATCH_PARENT x weight 1)
 *       ├── images[2]  (weight 1 x MATCH_PARENT)
 *       └── images[3]  (weight 1 x MATCH_PARENT)
 *
 * Press Escape or Back to quit.
 */
class ImageViewWithLayoutController : public ConnectionTracker
{
public:
  explicit ImageViewWithLayoutController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ImageViewWithLayoutController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::DARK_GRAY);

    window.Add(CreateContents());

    for(auto& image : mImages)
    {
      image.ResourceReadySignal().Connect(this, &ImageViewWithLayoutController::OnResourceReady);
    }
    window.KeyEventSignal().Connect(this, &ImageViewWithLayoutController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .Spacing(8.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateRow(0),
        CreateRow(2),
      });
  }

  StackLayout CreateRow(int startIndex)
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .Spacing(8.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .Children({
        ImageView::New(IMAGE_URLS[startIndex])
          .SetRequestedWidth(WRAP_CONTENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
          .As(mImages[startIndex]),
        ImageView::New(IMAGE_URLS[startIndex + 1])
          .SetRequestedWidth(WRAP_CONTENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
          .As(mImages[startIndex + 1]),
      });
  }

  void OnResourceReady(View view)
  {
    ImageView imageView = ImageView::DownCast(view);
    DALI_LOG_RELEASE_INFO("[ImageView] ResourceReadySignal fired. url=%s status=%d\n",
                          imageView.GetResourceUrl().CStr(),
                          static_cast<int>(imageView.GetLoadingStatus()));
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
  Ui::ImageView mImages[IMAGE_COUNT];
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewWithLayoutController controller(application);
  application.MainLoop();
  return 0;
}
