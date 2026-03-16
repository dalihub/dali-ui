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
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * PlaceholderImage sample
 *
 * Sequence:
 *   1. ImageView created with PlaceholderImage set → placeholder shown immediately
 *   2. After 2 seconds, timer callback calls SetResourceUrl() → loading starts, placeholder re-shown
 *   3. Loading complete → ResourceReadySignal fired, placeholder removed automatically
 *
 * Button:
 *   [Change Image] — cycles through images to repeat the placeholder → main image cycle
 *
 * Press Escape or Back to quit.
 */
class PlaceholderSampleController : public ConnectionTracker
{
public:
  explicit PlaceholderSampleController(Application& application)
  : mApplication(application),
    mImageIndex(0)
  {
    mApplication.InitSignal().Connect(this, &PlaceholderSampleController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    mImage = ImageView::New()
               .SetRequestedWidth(MATCH_PARENT)
               .SetRequestedHeight(WRAP_CONTENT)
               .SetPlaceholderUrl(RESOURCES_DIR "placeholder_image.png")
               .SetFittingMode(Dali::Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
               .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    mImage.ResourceReadySignal().Connect(this, &PlaceholderSampleController::OnResourceReady);

    mButton = StackLayout::New(StackOrientation::VERTICAL)
                .SetRequestedWidth(MATCH_PARENT)
                .SetRequestedHeight(80.0f)
                .SetBackgroundColor(UiColor(0x4A90E2))
                .Children({
                  Label::New("Change Image")
                    .SetRequestedWidth(MATCH_PARENT)
                    .SetRequestedHeight(MATCH_PARENT)
                    .SetTextColor(UiColor(0xFFFFFF))
                    .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                    .SetVerticalTextAlignment(Text::Alignment::CENTER),
                });
    mButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &PlaceholderSampleController::OnButtonClicked);

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .Children({mImage, mButton}));

    // 2초 후 첫 이미지 로드 (placeholder → main image 사이클 확인)
    mTimer = Timer::New(2000);
    mTimer.TickSignal().Connect(this, &PlaceholderSampleController::OnTimerTick);
    mTimer.Start();

    DALI_LOG_RELEASE_INFO("[Placeholder] Timer started. Main image will load in 2 seconds.\n");

    window.KeyEventSignal().Connect(this, &PlaceholderSampleController::OnKeyEvent);
  }

  bool OnTimerTick()
  {
    LoadNextImage();
    return false; // one-shot
  }

  void OnButtonClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    LoadNextImage();
  }

  void LoadNextImage()
  {
    static const char* images[] = {
      RESOURCES_DIR "gallery-large-3.jpg",
      RESOURCES_DIR "gallery-medium-49.jpg",
      RESOURCES_DIR "gallery-medium-3.jpg",
    };
    constexpr int imageCount = sizeof(images) / sizeof(images[0]);

    const char* url = images[mImageIndex % imageCount];
    ++mImageIndex;

    DALI_LOG_RELEASE_INFO("[Placeholder] SetResourceUrl(%s) — placeholder should appear.\n", url);
    mImage.SetResourceUrl(url);
  }

  void OnResourceReady(View view)
  {
    ImageView imageView = ImageView::DownCast(view);
    DALI_LOG_RELEASE_INFO("[Placeholder] ResourceReadySignal fired — placeholder should be gone. status=%d\n",
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
  Application&     mApplication;
  Ui::ImageView    mImage;
  Ui::StackLayout  mButton;
  Timer            mTimer;
  int              mImageIndex;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  PlaceholderSampleController sample(application);
  application.MainLoop();
  return 0;
}
