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
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t STEP_INTERVAL_MS = 3000u;

using Clock = std::chrono::steady_clock;
} // unnamed namespace

/**
 * Verifies ImageUrl reference counting by repeatedly adding and removing two
 * independent ImageUrl handles for one 4K image.
 *
 * Scenario:
 *  1. Add two ImageUrl handles and create an ImageView (initial load).
 *  2. Remove the ImageView and one ImageUrl handle.
 *  3. Recreate the ImageView (the remaining handle should keep the cache pinned).
 *  4. Remove the ImageView and the final ImageUrl handle.
 *  5. Recreate the ImageView (the image should be loaded again).
 *
 * The scenario advances automatically. Press Enter to advance immediately,
 * Space to pause or resume, and Escape or Back to quit.
 * Run with LOG_TEXTURE_MANAGER=2 to inspect cache reference counts.
 */
class ImageUrlAddRemoveController : public ConnectionTracker
{
public:
  explicit ImageUrlAddRemoveController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ImageUrlAddRemoveController::OnInit);
  }

private:
  enum class Step
  {
    REMOVE_FIRST,
    RECREATE_PINNED,
    REMOVE_LAST,
    RECREATE_UNPINNED,
    RESET,
    START,
  };

  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(UiColor(0x171717));
    mWindow.KeyEventSignal().Connect(this, &ImageUrlAddRemoveController::OnKeyEvent);

    mSourceUrl = RESOURCES_DIR "image-url-cache-4k.png";
    mWindow.Add(CreateRoot());
    StartCycle();

    mTimer = Timer::New(STEP_INTERVAL_MS);
    mTimer.TickSignal().Connect(this, &ImageUrlAddRemoveController::OnTick);
    mTimer.Start();
  }

  View CreateRoot()
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(6.0f);
    root.SetPadding(Extents(8, 8, 8, 8));

    Label title = CreateLabel("ImageUrl ADD / REMOVE REFERENCE TEST", 20.0f, UiColor(0xFFFFFF));
    title.SetRequestedHeight(54.0f);
    root.Add(title);

    mStateLabel = CreateLabel("", 17.0f, UiColor(0x7CFF9A));
    mStateLabel.SetRequestedHeight(58.0f);
    root.Add(mStateLabel);

    mImageContainer = StackLayout::New(StackOrientation::VERTICAL);
    mImageContainer.SetRequestedWidth(MATCH_PARENT);
    mImageContainer.SetRequestedHeight(MATCH_PARENT);
    mImageContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    mImageContainer.SetBackgroundColor(UiColor(0x080808));
    root.Add(mImageContainer);

    mResultLabel = CreateLabel("Waiting for the first load...", 16.0f, UiColor(0xFFB74D));
    mResultLabel.SetRequestedHeight(70.0f);
    root.Add(mResultLabel);

    Label help = CreateLabel("Auto: 3 sec  |  Enter: next  |  Space: pause/resume", 13.0f, UiColor(0xBDBDBD));
    help.SetRequestedHeight(40.0f);
    root.Add(help);

    return root;
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

  void StartCycle()
  {
    ++mCycle;

    AddImageUrl();
    AddImageUrl();
    CreateImageView("Initial load", "COLD LOAD expected");
    mNextStep = Step::REMOVE_FIRST;
  }

  void AddImageUrl()
  {
    ImageUrl imageUrl = ImageUrlUtils::GenerateUrl(mSourceUrl);
    if(mVisualUrl.Size() == 0u)
    {
      mVisualUrl = imageUrl.GetUrl();
    }
    mImageUrls.emplace_back(std::move(imageUrl));

    DALI_LOG_RELEASE_INFO("[ImageUrlAddRemove] ADD ImageUrl: handles=%zu url=%s\n",
                          mImageUrls.size(),
                          mVisualUrl.CStr());
  }

  void RemoveImageUrl()
  {
    if(!mImageUrls.empty())
    {
      mImageUrls.pop_back();
    }

    DALI_LOG_RELEASE_INFO("[ImageUrlAddRemove] REMOVE ImageUrl: handles=%zu\n", mImageUrls.size());
  }

  void CreateImageView(const char* operation, const char* expectation)
  {
    mExpectation = expectation;
    mImageReady  = false;

    mImageView = ImageView::New();
    mImageView.SetRequestedWidth(MATCH_PARENT);
    mImageView.SetRequestedHeight(MATCH_PARENT);
    mImageView.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    mImageView.SetProperty(ImageView::Property::LOAD_POLICY, Ui::Image::LoadPolicy::IMMEDIATE);
    mImageView.ResourceReadySignal().Connect(this, &ImageUrlAddRemoveController::OnResourceReady);
    mImageContainer.Add(mImageView);

    mLoadStarted = Clock::now();
    mImageView.SetResourceUrl(mVisualUrl);

    UpdateState(operation);
    mResultLabel.SetText(expectation);
    mResultLabel.SetTextColor(UiColor(0xFFB74D));

    if(mImageView.GetLoadingStatus() != Ui::Visual::ResourceStatus::PREPARING)
    {
      OnResourceReady(mImageView);
    }
  }

  void RemoveImageView()
  {
    if(mImageView)
    {
      mImageContainer.Remove(mImageView);
      mImageView.Reset();
    }
  }

  void AdvanceScenario()
  {
    switch(mNextStep)
    {
      case Step::REMOVE_FIRST:
      {
        RemoveImageView();
        RemoveImageUrl();
        UpdateState("REMOVE view + first ImageUrl");
        mResultLabel.SetText("One ImageUrl remains: texture must stay cached");
        mResultLabel.SetTextColor(UiColor(0x7CFF9A));
        mNextStep = Step::RECREATE_PINNED;
        break;
      }
      case Step::RECREATE_PINNED:
      {
        CreateImageView("ADD view with one ImageUrl", "CACHE HIT expected");
        mNextStep = Step::REMOVE_LAST;
        break;
      }
      case Step::REMOVE_LAST:
      {
        RemoveImageView();
        RemoveImageUrl();
        UpdateState("REMOVE view + final ImageUrl");
        mResultLabel.SetText("No owners remain: texture should no longer be pinned");
        mResultLabel.SetTextColor(UiColor(0xFF8A80));
        mNextStep = Step::RECREATE_UNPINNED;
        break;
      }
      case Step::RECREATE_UNPINNED:
      {
        CreateImageView("ADD view with no ImageUrl", "RELOAD expected");
        mNextStep = Step::RESET;
        break;
      }
      case Step::RESET:
      {
        RemoveImageView();
        UpdateState("REMOVE view; cycle complete");
        mResultLabel.SetText("All ImageUrl handles removed");
        mResultLabel.SetTextColor(UiColor(0xBDBDBD));
        mNextStep = Step::START;
        break;
      }
      case Step::START:
      {
        StartCycle();
        break;
      }
    }
  }

  void UpdateState(const char* operation)
  {
    std::ostringstream state;
    state << "Cycle " << mCycle << " | ImageUrl handles: " << mImageUrls.size()
          << " | View: " << (mImageView ? "ADDED" : "REMOVED") << "\n"
          << operation;
    mStateLabel.SetText(state.str().c_str());
  }

  void OnResourceReady(View view)
  {
    if(mImageReady || !mImageView || view != mImageView)
    {
      return;
    }

    mImageReady                    = true;
    const auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - mLoadStarted).count();

    std::ostringstream result;
    if(mImageView.GetLoadingStatus() == Ui::Visual::ResourceStatus::READY)
    {
      result << mExpectation << ": READY in " << elapsedMilliseconds << " ms";
      mResultLabel.SetTextColor(UiColor(0x7CFF9A));
    }
    else
    {
      result << "LOAD FAILED after " << elapsedMilliseconds << " ms";
      mResultLabel.SetTextColor(UiColor(0xFF5252));
    }
    mResultLabel.SetText(result.str().c_str());

    DALI_LOG_RELEASE_INFO("[ImageUrlAddRemove] handles=%zu expectation=%s ready=%d elapsed=%lld ms\n",
                          mImageUrls.size(),
                          mExpectation.c_str(),
                          mImageView.GetLoadingStatus() == Ui::Visual::ResourceStatus::READY,
                          static_cast<long long>(elapsedMilliseconds));
  }

  bool OnTick()
  {
    AdvanceScenario();
    return true;
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
      AdvanceScenario();
    }
    else if(event.GetKeyName() == "space" || event.GetKeyName() == "Space")
    {
      mAutoPlay = !mAutoPlay;
      if(mAutoPlay)
      {
        mTimer.Start();
      }
      else
      {
        mTimer.Stop();
      }
    }
  }

private:
  Application&          mApplication;
  Window                mWindow;
  StackLayout           mImageContainer;
  ImageView             mImageView;
  Label                 mStateLabel;
  Label                 mResultLabel;
  Timer                 mTimer;
  Dali::String          mSourceUrl;
  Dali::String          mVisualUrl;
  std::vector<ImageUrl> mImageUrls;
  std::string           mExpectation;
  Clock::time_point     mLoadStarted;
  Step                  mNextStep{Step::START};
  uint32_t              mCycle{0u};
  bool                  mImageReady{false};
  bool                  mAutoPlay{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageUrlAddRemoveController controller(application);
  application.MainLoop();
  return 0;
}
