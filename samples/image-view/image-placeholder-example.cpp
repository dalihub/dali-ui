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
#include <dali-ui-foundation/public-api/animated-image-view.h>
#include <dali-ui-foundation/public-api/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * Image Placeholder sample — supports ImageView, AnimatedImageView, and LottieAnimationView.
 *
 * Flow:
 *   1. View is created with a placeholder image but NO main URL → placeholder shown immediately.
 *   2. After 2 seconds a timer fires, sets the main URL and starts playback.
 *   3. ResourceReadySignal fires when loading completes → placeholder removed automatically.
 *
 * Buttons:
 *   [TYPE]   — toggles between IMAGE / ANIMATED / LOTTIE and restarts the cycle.
 *   [RELOAD] — restarts the placeholder → main-image cycle for the current type.
 */
class ImagePlaceholderController : public ConnectionTracker
{
  enum class ViewType { IMAGE = 0, ANIMATED, LOTTIE, COUNT };
  static const char* TYPE_NAMES[(int)ViewType::COUNT];
  static const char* TYPE_URLS[(int)ViewType::COUNT];

public:
  explicit ImagePlaceholderController(Application& application)
  : mApplication(application), mViewType(ViewType::IMAGE)
  {
    mApplication.InitSignal().Connect(this, &ImagePlaceholderController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1B1B1B));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT)
        .Spacing(8.0f).SetViewPadding(Extents(10, 10, 10, 10))
        .Children({
          Label::New("Image Placeholder Sample").SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(40.0f).SetFontSize(16.0f).SetTextColor(UiColor(0xFFFFFF)).SetHorizontalTextAlignment(Text::Alignment::CENTER).SetVerticalTextAlignment(Text::Alignment::CENTER),
          mContainer = StackLayout::New(StackOrientation::VERTICAL).SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)).SetBackgroundColor(UiColor(0x2A2A2A)),
          mStatusLabel = Label::New("Status: —").SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(28.0f).SetFontSize(12.0f).SetTextColor(UiColor(0xCCCCCC)).SetHorizontalTextAlignment(Text::Alignment::CENTER).SetVerticalTextAlignment(Text::Alignment::CENTER),
          CreateControlRow(),
        }));

    window.KeyEventSignal().Connect(this, &ImagePlaceholderController::OnKeyEvent);
    ResetImage();
  }

  View CreateControlRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL).Spacing(8.0f).SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(50.0f);

    mTypeButton = Label::New(TYPE_NAMES[(int)mViewType])
                    .SetRequestedWidth(WRAP_CONTENT).SetRequestedHeight(MATCH_PARENT)
                    .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                    .SetBackgroundColor(UiColor(0x1565C0)).SetFontSize(13.0f).SetTextColor(UiColor(0xFFFFFF))
                    .SetHorizontalTextAlignment(Text::Alignment::CENTER).SetVerticalTextAlignment(Text::Alignment::CENTER);
    mTypeButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImagePlaceholderController::OnTypeToggleClicked);

    View reloadButton = Label::New("RELOAD")
                          .SetRequestedWidth(WRAP_CONTENT).SetRequestedHeight(MATCH_PARENT)
                          .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                          .SetBackgroundColor(UiColor(0x2E7D32)).SetFontSize(13.0f).SetTextColor(UiColor(0xFFFFFF))
                          .SetHorizontalTextAlignment(Text::Alignment::CENTER).SetVerticalTextAlignment(Text::Alignment::CENTER);
    reloadButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImagePlaceholderController::OnReloadClicked);

    row.Add(mTypeButton);
    row.Add(reloadButton);
    return row;
  }

  void ResetImage()
  {
    // Stop any pending load timer from a previous cycle.
    if(mLoadTimer && mLoadTimer.IsRunning()) { mLoadTimer.Stop(); }

    if(mImage) { mContainer.Remove(mImage); mImage.Reset(); }

    // Step 1: create view with placeholder only — no main URL yet.
    // MATCH_PARENT height ensures the view fills the weighted container so the placeholder is visible.
    switch(mViewType)
    {
      case ViewType::ANIMATED:
      {
        AnimatedImageView view = AnimatedImageView::New();
        view.SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT);
        view.SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO);
        view.SetPlaceholderUrl(RESOURCES_DIR "placeholder_image.png");
        view.SetLoopCount(-1);
        view.ResourceReadySignal().Connect(this, &ImagePlaceholderController::OnResourceReady);
        mImage = view;
        break;
      }
      case ViewType::LOTTIE:
      {
        LottieAnimationView view = LottieAnimationView::New();
        view.SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT);
        view.SetPlaceholderUrl(RESOURCES_DIR "placeholder_image.png");
        view.SetLoopCount(-1);
        view.ResourceReadySignal().Connect(this, &ImagePlaceholderController::OnResourceReady);
        mImage = view;
        break;
      }
      default:
      {
        ImageView view = ImageView::New();
        view.SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT);
        view.SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO);
        view.SetPlaceholderUrl(RESOURCES_DIR "placeholder_image.png");
        view.ResourceReadySignal().Connect(this, &ImagePlaceholderController::OnResourceReady);
        mImage = view;
        break;
      }
    }

    mContainer.Add(mImage);
    mStatusLabel.SetText("Placeholder shown — loading in 2s...");
    DALI_LOG_ERROR("[Placeholder] Type=%s — placeholder shown, main URL pending\n", TYPE_NAMES[(int)mViewType]);

    // Step 2: fire the main URL after 2 seconds so the placeholder is visible long enough.
    mLoadTimer = Timer::New(2000);
    mLoadTimer.TickSignal().Connect(this, &ImagePlaceholderController::OnLoadTimerTick);
    mLoadTimer.Start();
  }

  bool OnLoadTimerTick()
  {
    const char* mainUrl = TYPE_URLS[(int)mViewType];

    switch(mViewType)
    {
      case ViewType::ANIMATED:
        AnimatedImageView::DownCast(mImage).SetResourceUrl(mainUrl);
        AnimatedImageView::DownCast(mImage).Play();
        break;
      case ViewType::LOTTIE:
        LottieAnimationView::DownCast(mImage).SetResourceUrl(mainUrl);
        LottieAnimationView::DownCast(mImage).Play();
        break;
      default:
        ImageView::DownCast(mImage).SetResourceUrl(mainUrl);
        break;
    }

    mStatusLabel.SetText("Loading main image...");
    DALI_LOG_ERROR("[Placeholder] Timer fired — url=%s\n", mainUrl);
    return false; // one-shot
  }

  void OnTypeToggleClicked(View, const InputEvent&)
  {
    mViewType = (ViewType)(((int)mViewType + 1) % (int)ViewType::COUNT);
    mTypeButton.SetText(TYPE_NAMES[(int)mViewType]);
    ResetImage();
  }
  void OnReloadClicked(View, const InputEvent&) { ResetImage(); }
  void OnResourceReady(View view)
  {
    mStatusLabel.SetText("ResourceReady — placeholder removed");

    Ui::Visual::ResourceStatus status = Ui::Visual::ResourceStatus::FAILED;
    if(auto v = ImageView::DownCast(view)) status = v.GetLoadingStatus();
    else if(auto v = AnimatedImageView::DownCast(view)) status = v.GetLoadingStatus();
    else if(auto v = LottieAnimationView::DownCast(view)) status = v.GetLoadingStatus();

    DALI_LOG_ERROR("[Placeholder] ResourceReady fired! GetLoadingStatus=%d\n", (int)status);
  }
  void OnKeyEvent(const KeyEvent& event) { if(event.GetState() == KeyEvent::DOWN && (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))) mApplication.Quit(); }

  Application& mApplication;
  View         mImage;
  View         mContainer;
  Label        mTypeButton;
  Label        mStatusLabel;
  Timer        mLoadTimer;
  ViewType     mViewType;
};

const char* ImagePlaceholderController::TYPE_NAMES[(int)ImagePlaceholderController::ViewType::COUNT] = { "TYPE: IMAGE", "TYPE: ANIMATED", "TYPE: LOTTIE" };
const char* ImagePlaceholderController::TYPE_URLS[(int)ImagePlaceholderController::ViewType::COUNT] = { RESOURCES_DIR "gallery-large-3.jpg", RESOURCES_DIR "dog-anim.webp", RESOURCES_DIR "jolly_walker.json" };

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImagePlaceholderController controller(application);
  application.MainLoop();
  return 0;
}
