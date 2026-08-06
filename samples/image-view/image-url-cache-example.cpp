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
#include <dali/integration-api/debug.h>

#include <chrono>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t RECREATE_INTERVAL_MS = 1800u;
constexpr float    TITLE_HEIGHT         = 54.0f;
constexpr float    PANEL_HEADER_HEIGHT  = 46.0f;
constexpr float    PANEL_STATUS_HEIGHT  = 70.0f;
constexpr float    SUMMARY_HEIGHT       = 42.0f;

using Clock = std::chrono::steady_clock;
} // unnamed namespace

/**
 * Repeatedly recreates two ImageViews that load the same 4K image through
 * different URL strings. The plain URL releases its cached texture when its
 * view is destroyed. The ImageUrl side keeps the cache alive between views.
 *
 * The cache-pinned URL intentionally contains "/./". It resolves to the same file
 * as the plain URL while providing a separate cache key, so cache pinning for the
 * right cannot make the plain side reuse its texture.
 *
 * Run with LOG_TEXTURE_MANAGER=2 to also inspect cache reference counts.
 */
class ImageUrlCacheController : public ConnectionTracker
{
public:
  explicit ImageUrlCacheController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ImageUrlCacheController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(UiColor(0x171717));
    mWindow.KeyEventSignal().Connect(this, &ImageUrlCacheController::OnKeyEvent);

    mPlainSourceUrl  = RESOURCES_DIR "image-url-cache-4k.png";
    mCachedSourceUrl = RESOURCES_DIR "./image-url-cache-4k.png";
    mCachedImageUrl  = ImageUrlUtils::GenerateUrl(mCachedSourceUrl);

    DALI_LOG_RELEASE_INFO("[ImageUrlCache] plain=%s cachedSource=%s cachedUrl=%s\n",
                          mPlainSourceUrl.CStr(),
                          mCachedSourceUrl.CStr(),
                          mCachedImageUrl.GetUrl().CStr());

    mWindow.Add(CreateRoot());
    CreateImageViews();

    mTimer = Timer::New(RECREATE_INTERVAL_MS);
    mTimer.TickSignal().Connect(this, &ImageUrlCacheController::OnTick);
    mTimer.Start();
  }

  View CreateRoot()
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(4.0f);
    root.SetPadding(Extents(6, 6, 6, 6));

    Label title = CreateLabel("4K IMAGE RECREATION: PLAIN URL vs ImageUrl", 20.0f, UiColor(0xFFFFFF));
    title.SetRequestedHeight(TITLE_HEIGHT);
    root.Add(title);

    StackLayout comparison = StackLayout::New(StackOrientation::HORIZONTAL);
    comparison.SetRequestedWidth(MATCH_PARENT);
    comparison.SetRequestedHeight(MATCH_PARENT);
    comparison.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    comparison.SetSpacing(6.0f);
    comparison.Add(CreatePanel(false));
    comparison.Add(CreatePanel(true));
    root.Add(comparison);

    mSummaryLabel = CreateLabel("Initial 4K load", 14.0f, UiColor(0xBDBDBD));
    mSummaryLabel.SetRequestedHeight(SUMMARY_HEIGHT);
    root.Add(mSummaryLabel);

    return root;
  }

  View CreatePanel(bool cached)
  {
    StackLayout panel = StackLayout::New(StackOrientation::VERTICAL);
    panel.SetRequestedWidth(MATCH_PARENT);
    panel.SetRequestedHeight(MATCH_PARENT);
    panel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    panel.SetSpacing(4.0f);
    panel.SetPadding(Extents(4, 4, 4, 4));
    panel.SetBackgroundColor(cached ? UiColor(0x12321D) : UiColor(0x321818));

    Label header = CreateLabel(cached ? "ImageUrl (CACHE PINNED)" : "PLAIN URL (NOT PINNED)",
                               16.0f,
                               cached ? UiColor(0x7CFF9A) : UiColor(0xFF8A80));
    header.SetRequestedHeight(PANEL_HEADER_HEIGHT);
    panel.Add(header);

    StackLayout imageContainer = StackLayout::New(StackOrientation::VERTICAL);
    imageContainer.SetRequestedWidth(MATCH_PARENT);
    imageContainer.SetRequestedHeight(MATCH_PARENT);
    imageContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    imageContainer.SetBackgroundColor(UiColor(0x080808));
    panel.Add(imageContainer);

    Label status = CreateLabel("Waiting...", 15.0f, UiColor(0xFFB74D));
    status.SetRequestedHeight(PANEL_STATUS_HEIGHT);
    panel.Add(status);

    if(cached)
    {
      mCachedContainer = imageContainer;
      mCachedStatus    = status;
    }
    else
    {
      mPlainContainer = imageContainer;
      mPlainStatus    = status;
    }

    return panel;
  }

  Label CreateLabel(const char* text, float fontSize, const UiColor& color)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetFontSize(fontSize);
    label.SetTextColor(color);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  void CreateImageViews()
  {
    ++mCycle;
    mPlainReady  = false;
    mCachedReady = false;

    mPlainStatus.SetText("Loading 4K image...");
    mPlainStatus.SetTextColor(UiColor(0xFFB74D));
    mCachedStatus.SetText("Loading 4K image...");
    mCachedStatus.SetTextColor(UiColor(0xFFB74D));

    std::ostringstream summary;
    summary << "Cycle " << mCycle << ": creating both ImageViews";
    mSummaryLabel.SetText(summary.str().c_str());

    mPlainImageView = CreateImageView();
    mPlainImageView.ResourceReadySignal().Connect(this, &ImageUrlCacheController::OnPlainResourceReady);
    mPlainContainer.Add(mPlainImageView);

    mCachedImageView = CreateImageView();
    mCachedImageView.ResourceReadySignal().Connect(this, &ImageUrlCacheController::OnCachedResourceReady);
    mCachedContainer.Add(mCachedImageView);

    // Connect ResourceReadySignal before assigning the URLs. A cached texture
    // can become ready immediately, while ImageView::New(url) is still running.
    mPlainLoadStarted = Clock::now();
    mPlainImageView.SetResourceUrl(mPlainSourceUrl);
    mCachedLoadStarted = Clock::now();
    mCachedImageView.SetResourceUrl(mCachedImageUrl.GetUrl());

    // A cache hit can already be READY when SetResourceUrl() returns without
    // emitting a later ResourceReadySignal. Reflect that immediate result too.
    if(!mPlainReady && mPlainImageView.GetLoadingStatus() != Ui::Visual::ResourceStatus::PREPARING)
    {
      OnPlainResourceReady(mPlainImageView);
    }
    if(!mCachedReady && mCachedImageView.GetLoadingStatus() != Ui::Visual::ResourceStatus::PREPARING)
    {
      OnCachedResourceReady(mCachedImageView);
    }

    mViewsVisible = true;
    DALI_LOG_RELEASE_INFO("[ImageUrlCache] cycle=%u views created\n", mCycle);
  }

  ImageView CreateImageView()
  {
    ImageView imageView = ImageView::New();
    imageView.SetRequestedWidth(MATCH_PARENT);
    imageView.SetRequestedHeight(MATCH_PARENT);
    imageView.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    imageView.SetProperty(ImageView::Property::LOAD_POLICY, Ui::Image::LoadPolicy::IMMEDIATE);
    return imageView;
  }

  void DestroyImageViews()
  {
    if(mPlainImageView)
    {
      mPlainContainer.Remove(mPlainImageView);
      mPlainImageView.Reset();
    }
    if(mCachedImageView)
    {
      mCachedContainer.Remove(mCachedImageView);
      mCachedImageView.Reset();
    }

    mPlainStatus.SetText("View deleted - texture released");
    mPlainStatus.SetTextColor(UiColor(0xBDBDBD));
    mCachedStatus.SetText("View deleted - texture still cached");
    mCachedStatus.SetTextColor(UiColor(0x7CFF9A));

    std::ostringstream summary;
    summary << "Cycle " << mCycle << ": views deleted; recreating in " << RECREATE_INTERVAL_MS << " ms";
    mSummaryLabel.SetText(summary.str().c_str());

    mViewsVisible = false;
    DALI_LOG_RELEASE_INFO("[ImageUrlCache] cycle=%u views destroyed; cached handle remains alive\n", mCycle);
  }

  bool OnTick()
  {
    if(mViewsVisible)
    {
      DestroyImageViews();
    }
    else
    {
      CreateImageViews();
    }
    return true;
  }

  void OnPlainResourceReady(View view)
  {
    if(!mPlainImageView || view != mPlainImageView)
    {
      return;
    }

    mPlainLoadTimeMs = ElapsedMilliseconds(mPlainLoadStarted);
    mPlainReady      = true;
    UpdateReadyStatus(mPlainStatus, mPlainImageView, mPlainLoadTimeMs);
    UpdateSummaryIfReady();
    DALI_LOG_RELEASE_INFO("[ImageUrlCache] cycle=%u plain ready in %lld ms status=%d\n",
                          mCycle,
                          static_cast<long long>(mPlainLoadTimeMs),
                          static_cast<int>(mPlainImageView.GetLoadingStatus()));
  }

  void OnCachedResourceReady(View view)
  {
    if(!mCachedImageView || view != mCachedImageView)
    {
      return;
    }

    mCachedLoadTimeMs = ElapsedMilliseconds(mCachedLoadStarted);
    mCachedReady      = true;
    UpdateReadyStatus(mCachedStatus, mCachedImageView, mCachedLoadTimeMs);
    UpdateSummaryIfReady();
    DALI_LOG_RELEASE_INFO("[ImageUrlCache] cycle=%u cached ready in %lld ms status=%d\n",
                          mCycle,
                          static_cast<long long>(mCachedLoadTimeMs),
                          static_cast<int>(mCachedImageView.GetLoadingStatus()));
  }

  int64_t ElapsedMilliseconds(const Clock::time_point& started) const
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - started).count();
  }

  void UpdateReadyStatus(Label& label, const ImageView& imageView, int64_t elapsedMilliseconds)
  {
    std::ostringstream text;
    if(imageView.GetLoadingStatus() == Ui::Visual::ResourceStatus::READY)
    {
      text << "READY in " << elapsedMilliseconds << " ms";
      label.SetTextColor(UiColor(0x7CFF9A));
    }
    else
    {
      text << "LOAD FAILED after " << elapsedMilliseconds << " ms";
      label.SetTextColor(UiColor(0xFF5252));
    }
    label.SetText(text.str().c_str());
  }

  void UpdateSummaryIfReady()
  {
    if(!mPlainReady || !mCachedReady)
    {
      return;
    }

    std::ostringstream summary;
    summary << "Cycle " << mCycle << " result: plain " << mPlainLoadTimeMs
            << " ms / cached " << mCachedLoadTimeMs << " ms";
    mSummaryLabel.SetText(summary.str().c_str());
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(IsKey(event, DALI_KEY_RETURN))
    {
      OnTick();
    }
  }

private:
  Application& mApplication;
  Window       mWindow;

  Dali::String mPlainSourceUrl;
  Dali::String mCachedSourceUrl;
  ImageUrl     mCachedImageUrl;

  StackLayout mPlainContainer;
  StackLayout mCachedContainer;
  ImageView   mPlainImageView;
  ImageView   mCachedImageView;
  Label       mPlainStatus;
  Label       mCachedStatus;
  Label       mSummaryLabel;
  Timer       mTimer;

  Clock::time_point mPlainLoadStarted;
  Clock::time_point mCachedLoadStarted;
  int64_t           mPlainLoadTimeMs{0};
  int64_t           mCachedLoadTimeMs{0};
  uint32_t          mCycle{0u};
  bool              mViewsVisible{false};
  bool              mPlainReady{false};
  bool              mCachedReady{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageUrlCacheController controller(application);
  application.MainLoop();
  return 0;
}
