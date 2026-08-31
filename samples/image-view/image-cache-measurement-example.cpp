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

#include <cstdint>
#include <sstream>
#include <vector>

#if defined(_WIN32)
#include <process.h>
#else
#include <unistd.h>
#endif

#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/image/image-cache-utils.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali/dali.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t IMAGE_COUNT      = 96u;
constexpr int      MAX_DESIRED_SIZE = 512;
constexpr float    PREVIEW_SIZE     = 32.0f;
constexpr uint32_t PREVIEW_COLUMNS  = 12u;

enum class ResourceType : uint8_t
{
  BITMAP,
  NPATCH,
  SVG,
  SINGLE_FRAME,
  COUNT
};

struct ResourceTypeInfo
{
  const char* label;
  uint32_t    expectedTextureCount;
  const char* memoryHint;
};

constexpr uint32_t     RESOURCE_TYPE_COUNT                     = static_cast<uint32_t>(ResourceType::COUNT);
const ResourceTypeInfo RESOURCE_TYPE_INFO[RESOURCE_TYPE_COUNT] = {
  {"BITMAP", 96u, "about 59 MiB (RGB888)"},
  {"N-PATCH", 2u, "small; 96 Views share two textures"},
  {"SVG", 96u, "about 79 MiB (RGBA8888)"},
  {"SINGLE FRAME", 96u, "about 59 MiB (RGB888)"},
};

const char* const NPATCH_URLS[] = {
  RESOURCES_DIR "button-up-1.9.png",
  RESOURCES_DIR "tooltip.9.png",
};

int GetProcessId()
{
#if defined(_WIN32)
  return _getpid();
#else
  return getpid();
#endif
}
} // namespace

/**
 * NEVER texture cleanup measurement sample:
 * - Loads 96 separately cached textures with ReleasePolicy::NEVER
 * - Removes all ImageViews while retaining their unused textures
 * - Explicitly clears the retained textures through ImageCache
 * - Shows the process ID and smaps_rollup measurement command on screen
 */
class ImageCacheMeasurementController : public ConnectionTracker
{
public:
  explicit ImageCacheMeasurementController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ImageCacheMeasurementController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x181818));
    window.Add(CreateRoot());
    window.KeyEventSignal().Connect(this, &ImageCacheMeasurementController::OnKeyEvent);
  }

  View CreateRoot()
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(4.0f);
    root.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    Label title = Label::New("NEVER texture cleanup measurement");
    title.SetRequestedWidth(MATCH_PARENT);
    title.SetRequestedHeight(42.0f);
    title.SetFontSize(20.0f);
    title.SetTextColor(UiColor(0xFFFFFF));
    title.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    title.SetVerticalTextAlignment(Text::Alignment::CENTER);
    root.Add(title);

    root.Add(CreateTypeRow());

    mGuideLabel = Label::New(CreateGuideText().c_str());
    mGuideLabel.SetRequestedWidth(MATCH_PARENT);
    mGuideLabel.SetRequestedHeight(130.0f);
    mGuideLabel.SetFontSize(12.0f);
    mGuideLabel.SetTextColor(UiColor(0xDDDDDD));
    mGuideLabel.SetMultiLine(true);
    mGuideLabel.SetLineWrapMode(Text::LineWrapMode::WORD);
    mGuideLabel.SetHorizontalTextAlignment(Text::Alignment::START);
    mGuideLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    root.Add(mGuideLabel);

    mStatusLabel = Label::New("IDLE | Measure the baseline, then tap 1 LOAD NEVER.");
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(40.0f);
    mStatusLabel.SetFontSize(13.0f);
    mStatusLabel.SetTextColor(UiColor(0x80CBC4));
    mStatusLabel.SetMultiLine(true);
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    root.Add(mStatusLabel);

    mImageContainer = View::New();
    mImageContainer.SetRequestedWidth(MATCH_PARENT);
    mImageContainer.SetRequestedHeight(WRAP_CONTENT);
    mImageContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    mImageContainer.SetBackgroundColor(UiColor(0x242424));
    root.Add(mImageContainer);

    StackLayout controls = StackLayout::New(StackOrientation::HORIZONTAL);
    controls.SetRequestedWidth(MATCH_PARENT);
    controls.SetRequestedHeight(56.0f);
    controls.SetSpacing(4.0f);

    auto loadButton = CreateButton("1 LOAD NEVER", UiColor(0x1565C0));
    loadButton.AsInteractive().ClickedSignal().Connect(this, &ImageCacheMeasurementController::OnLoadClicked);
    controls.Add(loadButton);

    auto removeButton = CreateButton("2 REMOVE VIEWS", UiColor(0xEF6C00));
    removeButton.AsInteractive().ClickedSignal().Connect(this, &ImageCacheMeasurementController::OnRemoveClicked);
    controls.Add(removeButton);

    auto clearButton = CreateButton("3 CLEAR UNUSED", UiColor(0xC62828));
    clearButton.AsInteractive().ClickedSignal().Connect(this, &ImageCacheMeasurementController::OnClearClicked);
    controls.Add(clearButton);

    root.Add(controls);
    return root;
  }

  std::string CreateGuideText() const
  {
    const auto&        typeInfo = GetResourceTypeInfo();
    std::ostringstream guide;
    guide << "PID: " << GetProcessId() << " | Type: " << typeInfo.label
          << " | Expected Image textures: +" << typeInfo.expectedTextureCount << "\n";
#if defined(_WIN32)
    guide << "Use Task Manager or a graphics profiler to record memory after each step.\n";
#else
    guide << "Terminal: watch -n 1 'grep -E \"^(Rss|Pss|Private_Dirty):\" /proc/" << GetProcessId()
          << "/smaps_rollup'\n";
#endif
    guide << "Measure: baseline -> LOAD (wait 96/96) -> REMOVE (wait and measure) -> CLEAR (wait and measure).\n"
          << "Expected allocation: " << typeInfo.memoryHint
          << ". CLEAR may be delayed or hidden by the GPU driver allocator.";
    return guide.str();
  }

  View CreateTypeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    row.SetSpacing(4.0f);

    for(uint32_t index = 0u; index < RESOURCE_TYPE_COUNT; ++index)
    {
      const auto type     = static_cast<ResourceType>(index);
      mTypeButtons[index] = CreateButton(RESOURCE_TYPE_INFO[index].label,
                                         type == mSelectedType ? UiColor(0x1565C0) : UiColor(0x444444));
      mTypeButtons[index].AsInteractive().ClickedSignal().Connect(this, &ImageCacheMeasurementController::OnTypeClicked);
      row.Add(mTypeButtons[index]);
    }
    return row;
  }

  const ResourceTypeInfo& GetResourceTypeInfo() const
  {
    return RESOURCE_TYPE_INFO[static_cast<uint32_t>(mSelectedType)];
  }

  StackLayout CreateButton(const char* text, UiColor color)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(color);

    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(label);
    return button;
  }

  const char* GetResourceUrl(uint32_t index) const
  {
    switch(mSelectedType)
    {
      case ResourceType::NPATCH:
        return NPATCH_URLS[index % 2u];
      case ResourceType::SVG:
        return RESOURCES_DIR "svg-blocks.svg";
      case ResourceType::SINGLE_FRAME:
      case ResourceType::BITMAP:
      default:
        return RESOURCES_DIR "gallery-large-1.jpg";
    }
  }

  View CreateResourceView(uint32_t index)
  {
    const int desiredSize = MAX_DESIRED_SIZE - static_cast<int>(index);
    View      view;

    if(mSelectedType == ResourceType::SINGLE_FRAME)
    {
      AnimatedImageView image = AnimatedImageView::New(GetResourceUrl(index));
      image.SetReleasePolicy(Ui::Image::ReleasePolicy::NEVER);
      image.SetDesiredWidth(desiredSize);
      image.SetDesiredHeight(desiredSize);
      image.ResourceReadySignal().Connect(this, &ImageCacheMeasurementController::OnResourceReady);
      view = image;
    }
    else
    {
      ImageView image = ImageView::New(GetResourceUrl(index));
      image.SetReleasePolicy(Ui::Image::ReleasePolicy::NEVER);
      if(mSelectedType != ResourceType::NPATCH)
      {
        image.SetDesiredWidth(desiredSize);
        image.SetDesiredHeight(desiredSize);
      }
      image.ResourceReadySignal().Connect(this, &ImageCacheMeasurementController::OnResourceReady);
      view = image;
    }

    view.SetRequestedWidth(PREVIEW_SIZE);
    view.SetRequestedHeight(PREVIEW_SIZE);
    view.SetRequestedX(static_cast<float>(index % PREVIEW_COLUMNS) * PREVIEW_SIZE);
    view.SetRequestedY(static_cast<float>(index / PREVIEW_COLUMNS) * PREVIEW_SIZE);
    return view;
  }

  void OnTypeClicked(View clicked, InputEvent)
  {
    if(!mResourceViews.empty() || mCanClear)
    {
      mStatusLabel.SetText("Complete REMOVE and CLEAR before changing the resource type.");
      return;
    }

    for(uint32_t index = 0u; index < RESOURCE_TYPE_COUNT; ++index)
    {
      if(mTypeButtons[index] == clicked)
      {
        mTypeButtons[static_cast<uint32_t>(mSelectedType)].SetBackgroundColor(UiColor(0x444444));
        mSelectedType = static_cast<ResourceType>(index);
        mTypeButtons[index].SetBackgroundColor(UiColor(0x1565C0));
        mGuideLabel.SetText(CreateGuideText().c_str());
        mStatusLabel.SetText("IDLE | Measure the baseline, then tap 1 LOAD NEVER.");
        return;
      }
    }
  }

  void OnLoadClicked(View, InputEvent)
  {
    if(!mResourceViews.empty())
    {
      mStatusLabel.SetText("Images already exist. Complete REMOVE and CLEAR before loading again.");
      return;
    }

    mReadyCount  = 0u;
    mFailedCount = 0u;
    mCanClear    = false;
    mResourceViews.reserve(IMAGE_COUNT);

    for(uint32_t index = 0u; index < IMAGE_COUNT; ++index)
    {
      View resourceView = CreateResourceView(index);
      mImageContainer.Add(resourceView);
      mResourceViews.push_back(resourceView);
    }

    std::ostringstream status;
    status << GetResourceTypeInfo().label << " LOADING 0/" << IMAGE_COUNT
           << " | Wait until every resource is ready before measuring.";
    mStatusLabel.SetText(status.str().c_str());
  }

  void OnResourceReady(View view)
  {
    Ui::Visual::ResourceStatus resourceStatus = Ui::Visual::ResourceStatus::FAILED;
    if(auto animatedImage = AnimatedImageView::DownCast(view))
    {
      resourceStatus = animatedImage.GetLoadingStatus();
    }
    else if(auto image = ImageView::DownCast(view))
    {
      resourceStatus = image.GetLoadingStatus();
    }

    if(resourceStatus == Ui::Visual::ResourceStatus::READY)
    {
      ++mReadyCount;
    }
    else
    {
      ++mFailedCount;
    }

    std::ostringstream status;
    status << GetResourceTypeInfo().label << " LOADING " << mReadyCount << "/" << IMAGE_COUNT;
    if(mFailedCount > 0u)
    {
      status << " | failed: " << mFailedCount;
    }
    else if(mReadyCount == IMAGE_COUNT)
    {
      status << " | Measure Pss/Rss, then tap 2 REMOVE VIEWS.";
    }
    else
    {
      status << " | Wait for all resources.";
    }
    mStatusLabel.SetText(status.str().c_str());
  }

  void OnRemoveClicked(View, InputEvent)
  {
    if(mResourceViews.empty())
    {
      mStatusLabel.SetText("Nothing to remove. Tap 1 LOAD NEVER first.");
      return;
    }
    if(mReadyCount + mFailedCount < IMAGE_COUNT)
    {
      mStatusLabel.SetText("Resources are still loading. Wait for 96 completion signals.");
      return;
    }

    for(auto& resourceView : mResourceViews)
    {
      mImageContainer.Remove(resourceView);
    }
    mResourceViews.clear();
    mCanClear = true;
    std::ostringstream status;
    status << GetResourceTypeInfo().label
           << " REMOVED | Wait several frames and measure. NEVER textures should remain cached.";
    mStatusLabel.SetText(status.str().c_str());
  }

  void OnClearClicked(View, InputEvent)
  {
    if(!mCanClear)
    {
      mStatusLabel.SetText("Complete LOAD and REMOVE first so there are unused NEVER textures to clear.");
      return;
    }

    Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
    mCanClear = false;
    mStatusLabel.SetText("CLEAR REQUESTED | Wait several frames, then compare Pss/Rss with REMOVED.");
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN &&
       (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK)))
    {
      mApplication.Quit();
    }
  }

private:
  Application&      mApplication;
  View              mImageContainer;
  Label             mGuideLabel;
  Label             mStatusLabel;
  View              mTypeButtons[RESOURCE_TYPE_COUNT];
  std::vector<View> mResourceViews;
  ResourceType      mSelectedType{ResourceType::BITMAP};
  uint32_t          mReadyCount{0u};
  uint32_t          mFailedCount{0u};
  bool              mCanClear{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  ImageCacheMeasurementController controller(application);
  application.MainLoop();
  return 0;
}
