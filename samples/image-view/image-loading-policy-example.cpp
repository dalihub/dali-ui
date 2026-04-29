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
 * Image Loading Policy sample — supports ImageView, AnimatedImageView, and LottieAnimationView.
 */
class ImageLoadingPolicyController : public ConnectionTracker
{
  static constexpr int POLICY_COUNT      = 3;
  static constexpr int LOAD_POLICY_COUNT = 2;

  enum class ViewType
  {
    IMAGE_VIEW = 0,
    ANIMATED_IMAGE_VIEW,
    LOTTIE_ANIMATION_VIEW,
    COUNT
  };

  struct PolicyEntry { const char* name; Ui::Image::ReleasePolicy policy; };
  struct LoadPolicyEntry { const char* name; Ui::Image::LoadPolicy policy; };

  static const PolicyEntry     POLICIES[POLICY_COUNT];
  static const LoadPolicyEntry LOAD_POLICIES[LOAD_POLICY_COUNT];

public:
  explicit ImageLoadingPolicyController(Application& application)
  : mApplication(application),
    mSyncLoading(false),
    mFastTrack(false),
    mPolicyIndex(0),
    mLoadPolicyIndex(0),
    mSyncImageIndex(0),
    mReleasePolicyReadyAfterReAdd(false),
    mLoadPolicyReadyFired(false),
    mLoadPolicyAddedToScene(false),
    mViewType(ViewType::IMAGE_VIEW)
  {
    mApplication.InitSignal().Connect(this, &ImageLoadingPolicyController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1B1B1B));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .Children({
          CreateTypeSelector(),
          CreateContents(),
        }));

    window.KeyEventSignal().Connect(this, &ImageLoadingPolicyController::OnKeyEvent);
  }

  View CreateTypeSelector()
  {
    static const char* TYPE_NAMES[] = {"IMAGE", "ANIMATED", "LOTTIE"};
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetSpacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(48.0f)
                        .SetPadding(Extents(4, 4, 4, 4))
                        .SetBackgroundColor(UiColor(0x111111));

    for(int i = 0; i < (int)ViewType::COUNT; ++i)
    {
      StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                             .SetRequestedWidth(WRAP_CONTENT)
                             .SetRequestedHeight(MATCH_PARENT)
                             .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                             .SetBackgroundColor(i == (int)mViewType ? UiColor(0xD32F2F) : UiColor(0x333333))
                             .Children({
                               Label::New(TYPE_NAMES[i])
                                 .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT)
                                 .SetFontSize(12.0f).SetTextColor(UiColor(0xFFFFFF))
                                 .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                 .SetVerticalTextAlignment(Text::Alignment::CENTER),
                             });
      button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageLoadingPolicyController::OnViewTypeButtonClicked);
      mTypeButtons[i] = button;
      row.Add(button);
    }
    return row;
  }

  void OnViewTypeButtonClicked(View clickedView, const InputEvent& /*event*/)
  {
    for(int i = 0; i < (int)ViewType::COUNT; ++i)
    {
      if(mTypeButtons[i] == clickedView)
      {
        if(mViewType != (ViewType)i)
        {
          mTypeButtons[(int)mViewType].SetBackgroundColor(UiColor(0x333333));
          mViewType = (ViewType)i;
          mTypeButtons[(int)mViewType].SetBackgroundColor(UiColor(0xD32F2F));
          DALI_LOG_ERROR("[ImageLoadingPolicy] ViewType changed to %d\n", (int)mViewType);
          ResetSections();
        }
        return;
      }
    }
  }

  void ConnectResourceReady(View view, void (ImageLoadingPolicyController::*callback)(View))
  {
    if(auto v = ImageView::DownCast(view)) v.ResourceReadySignal().Connect(this, callback);
    else if(auto v = AnimatedImageView::DownCast(view)) v.ResourceReadySignal().Connect(this, callback);
    else if(auto v = LottieAnimationView::DownCast(view)) v.ResourceReadySignal().Connect(this, callback);
  }

  void ResetSections()
  {
    mSyncLoadingContainer.Remove(mSyncLoadingImage);
    mSyncLoadingImage = CreateTargetView(false);
    ApplyProperty(mSyncLoadingImage, ImageView::Property::SYNCHRONOUS_LOADING, mSyncLoading);
    ConnectResourceReady(mSyncLoadingImage, &ImageLoadingPolicyController::OnSyncLoadingResourceReady);
    mSyncLoadingContainer.Add(mSyncLoadingImage);

    mFastTrackContainer.Remove(mFastTrackImage);
    mFastTrackImage = CreateTargetView(true);
    if(mViewType != ViewType::LOTTIE_ANIMATION_VIEW)
    {
       ApplyProperty(mFastTrackImage, ImageView::Property::FAST_TRACK_UPLOADING, mFastTrack);
    }
    ConnectResourceReady(mFastTrackImage, &ImageLoadingPolicyController::OnFastTrackResourceReady);
    mFastTrackContainer.Add(mFastTrackImage);

    mReleasePolicyContainer.Remove(mReleasePolicyImage);
    mReleasePolicyImage = CreateTargetView(false);
    ApplyProperty(mReleasePolicyImage, ImageView::Property::RELEASE_POLICY, POLICIES[mPolicyIndex].policy);
    ConnectResourceReady(mReleasePolicyImage, &ImageLoadingPolicyController::OnReleasePolicyResourceReady);
    mReleasePolicyContainer.Add(mReleasePolicyImage);

    mLoadPolicyContainer.Remove(mLoadPolicyImage);
    mLoadPolicyImage.Reset();
    while(mLoadPolicyContainer.GetChildCount() > 0) mLoadPolicyContainer.Remove(mLoadPolicyContainer.GetChildAt(0));
    mLoadPolicyContainer.Add(Label::New("Press [TEST] to create view").SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT).SetHorizontalTextAlignment(Text::Alignment::CENTER).SetVerticalTextAlignment(Text::Alignment::CENTER));

    UpdateInfoLabels();
  }

  View CreateTargetView(bool large)
  {
    const char* imageUrl = large ? (mViewType == ViewType::LOTTIE_ANIMATION_VIEW ? RESOURCES_DIR "jolly_walker.json" : RESOURCES_DIR "gallery-large-3.jpg")
                                 : (mViewType == ViewType::LOTTIE_ANIMATION_VIEW ? RESOURCES_DIR "done.json" : RESOURCES_DIR "gallery-medium-49.jpg");
    View view;
    if(mViewType == ViewType::ANIMATED_IMAGE_VIEW) view = AnimatedImageView::New(imageUrl);
    else if(mViewType == ViewType::LOTTIE_ANIMATION_VIEW) view = LottieAnimationView::New(imageUrl);
    else view = ImageView::New(imageUrl);
    view.SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(WRAP_CONTENT);
    ApplyProperty(view, ImageView::Property::FITTING_MODE, (int)Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    return view;
  }

  void ApplyProperty(View view, Property::Index index, Property::Value value)
  {
     if(view) view.SetProperty(index, value);
  }

  void UpdateInfoLabels()
  {
    mSyncLoadingInfoLabel.SetText(mSyncLoading ? "Sync: ON" : "Sync: OFF");
    mFastTrackInfoLabel.SetText(mViewType == ViewType::LOTTIE_ANIMATION_VIEW ? "N/A (Lottie)" : (mFastTrack ? "FastTrack: ON" : "FastTrack: OFF"));
    mReleasePolicyInfoLabel.SetText(POLICIES[mPolicyIndex].name);
    mLoadPolicyInfoLabel.SetText(LOAD_POLICIES[mLoadPolicyIndex].name);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateSection("1. SynchronousLoading", mSyncLoadingContainer, mSyncLoadingImage = CreateTargetView(false), mSyncLoadingInfoLabel, CreateSyncLoadingRow()),
        CreateSection("2. FastTrackUploading", mFastTrackContainer, mFastTrackImage = CreateTargetView(true), mFastTrackInfoLabel, CreateFastTrackRow()),
        CreateSection("3. ReleasePolicy", mReleasePolicyContainer, mReleasePolicyImage = CreateTargetView(false), mReleasePolicyInfoLabel, CreateReleasePolicyRow()),
        CreateSection("4. LoadPolicy", mLoadPolicyContainer, View(), mLoadPolicyInfoLabel, CreateLoadPolicyRow()),
      });
  }

  View CreateSection(const char* title, View& container, View initialView, Label& infoLabel, View controlRow)
  {
    container = StackLayout::New(StackOrientation::VERTICAL).SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)).SetBackgroundColor(UiColor(0x222222));
    if(initialView) {
       container.Add(initialView);
       if(&container == &mSyncLoadingContainer) ConnectResourceReady(initialView, &ImageLoadingPolicyController::OnSyncLoadingResourceReady);
       if(&container == &mFastTrackContainer) ConnectResourceReady(initialView, &ImageLoadingPolicyController::OnFastTrackResourceReady);
       if(&container == &mReleasePolicyContainer) ConnectResourceReady(initialView, &ImageLoadingPolicyController::OnReleasePolicyResourceReady);
    }
    else container.Add(Label::New("Press [TEST]").SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT).SetHorizontalTextAlignment(Text::Alignment::CENTER).SetVerticalTextAlignment(Text::Alignment::CENTER));

    return StackLayout::New(StackOrientation::VERTICAL).SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .Children({
        Label::New(title).SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(20.0f).SetFontSize(10.0f).SetHorizontalTextAlignment(Text::Alignment::CENTER),
        container,
        Label::New("").SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(20.0f).SetFontSize(12.0f).SetHorizontalTextAlignment(Text::Alignment::CENTER).As(infoLabel),
        controlRow
      });
  }

  View CreateSyncLoadingRow() { return CreateControlRow({"SYNC TOGGLE", "CHANGE"}, {&ImageLoadingPolicyController::OnSyncLoadingToggleClicked, &ImageLoadingPolicyController::OnSyncLoadingChangeClicked}); }
  View CreateFastTrackRow() { return CreateControlRow({"FAST TOGGLE", "RELOAD"}, {&ImageLoadingPolicyController::OnFastTrackToggleClicked, &ImageLoadingPolicyController::OnFastTrackReloadClicked}); }
  View CreateReleasePolicyRow() { return CreateControlRow({"DETACHED", "DESTROYED", "HIDE 1s"}, {&ImageLoadingPolicyController::OnReleasePolicyDetachedClicked, &ImageLoadingPolicyController::OnReleasePolicyDestroyedClicked, &ImageLoadingPolicyController::OnHideClicked}); }
  View CreateLoadPolicyRow() { return CreateControlRow({"ATTACHED", "IMMEDIATE", "TEST"}, {&ImageLoadingPolicyController::OnLoadPolicyAttachedClicked, &ImageLoadingPolicyController::OnLoadPolicyImmediateClicked, &ImageLoadingPolicyController::OnLoadPolicyTestClicked}); }

  typedef void (ImageLoadingPolicyController::*ClickCallback)(View, const InputEvent&);
  View CreateControlRow(std::vector<const char*> labels, std::vector<ClickCallback> callbacks)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL).SetSpacing(4.0f).SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(40.0f);
    for(size_t i=0; i<labels.size(); ++i) {
      View btn = Label::New(labels[i]).SetRequestedWidth(WRAP_CONTENT).SetRequestedHeight(MATCH_PARENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)).SetBackgroundColor(UiColor(0x444444)).SetFontSize(10.0f).SetHorizontalTextAlignment(Text::Alignment::CENTER).SetVerticalTextAlignment(Text::Alignment::CENTER);
      btn.EnsureInteractiveTrait().ClickedSignal().Connect(this, callbacks[i]);
      row.Add(btn);
    }
    return row;
  }

  void OnSyncLoadingToggleClicked(View, const InputEvent&) { mSyncLoading = !mSyncLoading; ResetSections(); }
  void OnSyncLoadingChangeClicked(View, const InputEvent&) {
    mSyncPendingUrl = (mViewType == ViewType::LOTTIE_ANIMATION_VIEW) ? RESOURCES_DIR "done.json" : RESOURCES_DIR "gallery-medium-3.jpg";
    mSyncLoadingContainer.Remove(mSyncLoadingImage);
    mSyncChangeTimer = Timer::New(100);
    mSyncChangeTimer.TickSignal().Connect(this, &ImageLoadingPolicyController::OnSyncChangeTimerTick);
    mSyncChangeTimer.Start();
  }
  bool OnSyncChangeTimerTick() {
    mSyncLoadingImage.SetProperty(ImageView::Property::IMAGE, mSyncPendingUrl);
    mSyncLoadingContainer.Add(mSyncLoadingImage);
    return false;
  }
  void OnSyncLoadingResourceReady(View view) { LogStatus("Sync", view); }

  void OnFastTrackToggleClicked(View, const InputEvent&) { if(mViewType != ViewType::LOTTIE_ANIMATION_VIEW) { mFastTrack = !mFastTrack; ResetSections(); } }
  void OnFastTrackReloadClicked(View view, const InputEvent&) { if(auto v = ImageView::DownCast(mFastTrackImage)) v.Reload(); }
  void OnFastTrackResourceReady(View view) { LogStatus("FastTrack", view); }

  void OnReleasePolicyDetachedClicked(View, const InputEvent&) { mPolicyIndex = 0; ResetSections(); }
  void OnReleasePolicyDestroyedClicked(View, const InputEvent&) { mPolicyIndex = 1; ResetSections(); }
  void OnHideClicked(View, const InputEvent&) {
     mReleasePolicyReadyAfterReAdd = false;
     mReleasePolicyContainer.Remove(mReleasePolicyImage);
     mHideTimer = Timer::New(1000);
     mHideTimer.TickSignal().Connect(this, &ImageLoadingPolicyController::OnHideTimerTick);
     mHideTimer.Start();
  }
  bool OnHideTimerTick() {
    mReleasePolicyContainer.Add(mReleasePolicyImage);
    mCheckTimer = Timer::New(500);
    mCheckTimer.TickSignal().Connect(this, &ImageLoadingPolicyController::OnCheckTimerTick);
    mCheckTimer.Start();
    return false;
  }
  bool OnCheckTimerTick() { if(!mReleasePolicyReadyAfterReAdd) DALI_LOG_ERROR("[Release] Cached\n"); return false; }
  void OnReleasePolicyResourceReady(View view) { mReleasePolicyReadyAfterReAdd = true; LogStatus("Release", view); }

  void OnLoadPolicyAttachedClicked(View, const InputEvent&) { mLoadPolicyIndex = 0; UpdateInfoLabels(); }
  void OnLoadPolicyImmediateClicked(View, const InputEvent&) { mLoadPolicyIndex = 1; UpdateInfoLabels(); }
  void OnLoadPolicyTestClicked(View, const InputEvent&) {
    mLoadPolicyImage = CreateTargetView(true);
    ApplyProperty(mLoadPolicyImage, ImageView::Property::LOAD_POLICY, LOAD_POLICIES[mLoadPolicyIndex].policy);
    ConnectResourceReady(mLoadPolicyImage, &ImageLoadingPolicyController::OnLoadPolicyResourceReady);
    ApplyProperty(mLoadPolicyImage, ImageView::Property::IMAGE, (mViewType == ViewType::LOTTIE_ANIMATION_VIEW) ? RESOURCES_DIR "jolly_walker.json" : RESOURCES_DIR "gallery-large-3.jpg");
    mLoadPolicyReadyFired = false; mLoadPolicyAddedToScene = false;
    mLoadPolicyAddTimer = Timer::New(800);
    mLoadPolicyAddTimer.TickSignal().Connect(this, &ImageLoadingPolicyController::OnLoadPolicyAddTimerTick);
    mLoadPolicyAddTimer.Start();
  }
  bool OnLoadPolicyAddTimerTick() {
    mLoadPolicyAddedToScene = true;
    while(mLoadPolicyContainer.GetChildCount() > 0) mLoadPolicyContainer.Remove(mLoadPolicyContainer.GetChildAt(0));
    mLoadPolicyContainer.Add(mLoadPolicyImage);
    return false;
  }
  void OnLoadPolicyResourceReady(View view) { mLoadPolicyReadyFired = true; DALI_LOG_ERROR("[LoadPolicy] Ready %s scene add. ", mLoadPolicyAddedToScene ? "AFTER" : "BEFORE"); LogStatus("LoadPolicy", view); }

  void LogStatus(const char* section, View view)
  {
     int status = 0;
     if(auto v = ImageView::DownCast(view)) status = (int)v.GetLoadingStatus();
     else if(auto v = AnimatedImageView::DownCast(view)) status = (int)v.GetLoadingStatus();
     else if(auto v = LottieAnimationView::DownCast(view)) status = (int)v.GetLoadingStatus();
     DALI_LOG_ERROR("[%s] ResourceReady. Status=%d\n", section, status);
  }

  void OnKeyEvent(const KeyEvent& event) { if(event.GetState() == KeyEvent::DOWN && (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))) mApplication.Quit(); }

  Application&  mApplication;
  View          mSyncLoadingImage, mFastTrackImage, mReleasePolicyImage, mLoadPolicyImage;
  View          mSyncLoadingContainer, mFastTrackContainer, mReleasePolicyContainer, mLoadPolicyContainer;
  Label         mSyncLoadingInfoLabel, mFastTrackInfoLabel, mReleasePolicyInfoLabel, mLoadPolicyInfoLabel;
  View          mTypeButtons[(int)ViewType::COUNT];
  Timer         mSyncChangeTimer, mHideTimer, mCheckTimer, mLoadPolicyAddTimer;
  Dali::String  mSyncPendingUrl;
  bool          mSyncLoading, mFastTrack, mReleasePolicyReadyAfterReAdd, mLoadPolicyReadyFired, mLoadPolicyAddedToScene;
  int           mPolicyIndex, mLoadPolicyIndex, mSyncImageIndex;
  ViewType      mViewType;
};

const ImageLoadingPolicyController::PolicyEntry ImageLoadingPolicyController::POLICIES[POLICY_COUNT] = { {"DETACHED", Ui::Image::ReleasePolicy::DETACHED}, {"DESTROYED", Ui::Image::ReleasePolicy::DESTROYED}, {"NEVER", Ui::Image::ReleasePolicy::NEVER} };
const ImageLoadingPolicyController::LoadPolicyEntry ImageLoadingPolicyController::LOAD_POLICIES[LOAD_POLICY_COUNT] = { {"ATTACHED", Ui::Image::LoadPolicy::ATTACHED}, {"IMMEDIATE", Ui::Image::LoadPolicy::IMMEDIATE} };

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageLoadingPolicyController controller(application);
  application.MainLoop();
  return 0;
}
