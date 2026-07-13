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

#include <string>

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const IMAGE_URLS[] = {
  RESOURCES_DIR "gallery-large-3.jpg",
  RESOURCES_DIR "gallery-large-1.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "people-small-10.jpg",
};
const char* const GIF_URLS[] = {
  RESOURCES_DIR "dali-logo-anim.gif",
  RESOURCES_DIR "animatedLoading.gif",
};
const char* const LOTTIE_URLS[] = {
  RESOURCES_DIR "jolly_walker.json",
  RESOURCES_DIR "insta_camera.json",
  RESOURCES_DIR "confetti.json",
};
constexpr int IMAGE_COUNT  = 4;
constexpr int GIF_COUNT    = 2;
constexpr int LOTTIE_COUNT = 3;
} // namespace

/**
 * Image Loading Policy sample:
 * - One image at a time; CHANGE cycles through images for the current type
 * - PLAY / STOP for animated types
 * - SYNC LOADING toggle
 * - FAST TRACK UPLOADING toggle (not applicable to Lottie)
 * - RELEASE POLICY: DETACHED / DESTROYED / NEVER with HIDE 1s test
 * - LOAD POLICY: ATTACHED / IMMEDIATE with TEST (loads before scene add)
 * - Press Escape or Back to quit
 */
class ImageLoadingPolicyController : public ConnectionTracker
{
  enum class ViewType { IMAGE = 0, ANIMATED, LOTTIE, COUNT };
  static constexpr int TYPE_COUNT = static_cast<int>(ViewType::COUNT);

  struct ReleasePolicyEntry
  {
    const char*              label;
    Ui::Image::ReleasePolicy policy;
  };
  static const ReleasePolicyEntry RELEASE_POLICIES[3];

public:
  explicit ImageLoadingPolicyController(Application& application)
  : mApplication(application),
    mViewType(ViewType::IMAGE),
    mImageIndex(0),
    mSyncLoading(false),
    mFastTrack(false),
    mReleasePolicyIndex(0),
    mLoadPolicyIndex(0),
    mReleasePolicyReadyAfterReAdd(false),
    mLoadPolicyReadyFired(false),
    mLoadPolicyAddedToScene(false)
  {
    mApplication.InitSignal().Connect(this, &ImageLoadingPolicyController::OnInit);
  }

private:
  // ── Init ──────────────────────────────────────────────────────────────────

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1B1B1B));
    window.Add(CreateRoot());
    window.KeyEventSignal().Connect(this, &ImageLoadingPolicyController::OnKeyEvent);
  }

  View CreateRoot()
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.Add(CreateTypeRow());
    root.Add(CreateImageArea());
    root.Add(CreateStatusBar());
    root.Add(CreateControls());
    return root;
  }

  // ── ViewType selector ─────────────────────────────────────────────────────

  View CreateTypeRow()
  {
    static const char* NAMES[TYPE_COUNT] = {"IMAGE", "ANIMATED", "LOTTIE"};
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    row.SetPadding(Extents(4, 4, 4, 4));
    row.SetBackgroundColor(UiColor(0x111111));
    for(int i = 0; i < TYPE_COUNT; ++i)
    {
      mTypeButtons[i] = MakeBtn(NAMES[i], i == static_cast<int>(mViewType) ? UiColor(0xD32F2F) : UiColor(0x444444));
      mTypeButtons[i].AsInteractive().ClickedSignal().Connect(this, &ImageLoadingPolicyController::OnTypeClicked);
      row.Add(mTypeButtons[i]);
    }
    return row;
  }

  // ── Single image area ─────────────────────────────────────────────────────

  View CreateImageArea()
  {
    mImageContainer = StackLayout::New(StackOrientation::VERTICAL);
    mImageContainer.SetRequestedWidth(MATCH_PARENT);
    mImageContainer.SetRequestedHeight(WRAP_CONTENT);
    mImageContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    mImageContainer.SetBackgroundColor(UiColor(0x222222));
    RebuildView();
    return mImageContainer;
  }

  // ── Status bar ────────────────────────────────────────────────────────────

  View CreateStatusBar()
  {
    mStatusLabel = Label::New("");
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(20.0f);
    mStatusLabel.SetFontSize(10.0f);
    mStatusLabel.SetTextColor(UiColor(0x888888));
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    RefreshStatus();
    return mStatusLabel;
  }

  void RefreshStatus()
  {
    std::string s;
    s += mSyncLoading ? "Sync:ON" : "Sync:OFF";
    s += "  FT:";
    s += (mViewType == ViewType::LOTTIE) ? "N/A" : (mFastTrack ? "ON" : "OFF");
    s += "  Rel:";
    s += RELEASE_POLICIES[mReleasePolicyIndex].label;
    s += "  Load:";
    s += (mLoadPolicyIndex == 0) ? "ATTACHED" : "IMMEDIATE";
    mStatusLabel.SetText(s.c_str());
  }

  // ── Control rows ──────────────────────────────────────────────────────────

  View CreateControls()
  {
    StackLayout controls = StackLayout::New(StackOrientation::VERTICAL);
    controls.SetSpacing(2.0f);
    controls.SetRequestedWidth(MATCH_PARENT);
    controls.SetRequestedHeight(WRAP_CONTENT);
    controls.SetPadding(Extents(2, 2, 2, 2));
    controls.Add(CreateImageControlRow());
    controls.Add(CreateLoadingToggleRow());
    controls.Add(CreateReleasePolicyRow());
    controls.Add(CreateLoadPolicyRow());
    return controls;
  }

  View CreateImageControlRow()
  {
    return MakeRow({"CHANGE", "PLAY", "STOP", "RELOAD"},
                   {&ImageLoadingPolicyController::OnChangeClicked,
                    &ImageLoadingPolicyController::OnPlayClicked,
                    &ImageLoadingPolicyController::OnStopClicked,
                    &ImageLoadingPolicyController::OnReloadClicked});
  }

  View CreateLoadingToggleRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(2.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(40.0f);

    mSyncBtn = MakeToggleBtn("SYNC: OFF", false, mSyncBtnLabel);
    mSyncBtn.AsInteractive().ClickedSignal().Connect(this, &ImageLoadingPolicyController::OnSyncClicked);

    mFastTrackBtn = MakeToggleBtn("FASTTRACK: OFF", false, mFastTrackBtnLabel);
    mFastTrackBtn.AsInteractive().ClickedSignal().Connect(this, &ImageLoadingPolicyController::OnFastTrackClicked);

    row.Add(mSyncBtn);
    row.Add(mFastTrackBtn);
    return row;
  }

  View CreateReleasePolicyRow()
  {
    static const char* LABELS[3] = {"REL:DETACHED", "REL:DESTROYED", "REL:NEVER"};
    typedef void (ImageLoadingPolicyController::*Cb)(View, InputEvent);
    static const Cb CBS[3] = {
      &ImageLoadingPolicyController::OnRelDetachedClicked,
      &ImageLoadingPolicyController::OnRelDestroyedClicked,
      &ImageLoadingPolicyController::OnRelNeverClicked,
    };
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(2.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(40.0f);
    for(int i = 0; i < 3; ++i)
    {
      mReleaseBtns[i] = MakeBtn(LABELS[i], i == mReleasePolicyIndex ? UiColor(0x1565C0) : UiColor(0x444444));
      mReleaseBtns[i].AsInteractive().ClickedSignal().Connect(this, CBS[i]);
      row.Add(mReleaseBtns[i]);
    }
    auto hideBtn = MakeBtn("HIDE 1s", UiColor(0x444444));
    hideBtn.AsInteractive().ClickedSignal().Connect(this, &ImageLoadingPolicyController::OnHideClicked);
    row.Add(hideBtn);
    return row;
  }

  View CreateLoadPolicyRow()
  {
    static const char* LABELS[2] = {"LOAD:ATTACHED", "LOAD:IMMEDIATE"};
    typedef void (ImageLoadingPolicyController::*Cb)(View, InputEvent);
    static const Cb CBS[2] = {
      &ImageLoadingPolicyController::OnLoadAttachedClicked,
      &ImageLoadingPolicyController::OnLoadImmediateClicked,
    };
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(2.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(40.0f);
    for(int i = 0; i < 2; ++i)
    {
      mLoadBtns[i] = MakeBtn(LABELS[i], i == mLoadPolicyIndex ? UiColor(0x1565C0) : UiColor(0x444444));
      mLoadBtns[i].AsInteractive().ClickedSignal().Connect(this, CBS[i]);
      row.Add(mLoadBtns[i]);
    }
    auto testBtn = MakeBtn("TEST LOAD", UiColor(0x2E7D32));
    testBtn.AsInteractive().ClickedSignal().Connect(this, &ImageLoadingPolicyController::OnLoadTestClicked);
    row.Add(testBtn);
    return row;
  }

  // ── Button factories ──────────────────────────────────────────────────────

  StackLayout MakeBtn(const char* labelText, UiColor bg)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(bg);
    Label label = Label::New(labelText);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(10.0f);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(label);
    return button;
  }

  StackLayout MakeToggleBtn(const char* labelText, bool active, Label& labelRef)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(active ? UiColor(0x1565C0) : UiColor(0x444444));
    labelRef = Label::New(labelText);
    labelRef.SetRequestedWidth(MATCH_PARENT);
    labelRef.SetRequestedHeight(MATCH_PARENT);
    labelRef.SetFontSize(10.0f);
    labelRef.SetTextColor(UiColor(0xFFFFFF));
    labelRef.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    labelRef.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(labelRef);
    return button;
  }

  typedef void (ImageLoadingPolicyController::*Cb)(View, InputEvent);

  View MakeRow(std::vector<const char*> labels, std::vector<Cb> callbacks)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(2.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(40.0f);
    for(size_t i = 0; i < labels.size(); ++i)
    {
      auto btn = MakeBtn(labels[i], UiColor(0x444444));
      btn.AsInteractive().ClickedSignal().Connect(this, callbacks[i]);
      row.Add(btn);
    }
    return row;
  }

  // ── View management ───────────────────────────────────────────────────────

  const char* GetCurrentUrl() const
  {
    switch(mViewType)
    {
      case ViewType::ANIMATED: return GIF_URLS[mImageIndex % GIF_COUNT];
      case ViewType::LOTTIE:   return LOTTIE_URLS[mImageIndex % LOTTIE_COUNT];
      default:                  return IMAGE_URLS[mImageIndex % IMAGE_COUNT];
    }
  }

  int GetUrlCount() const
  {
    switch(mViewType)
    {
      case ViewType::ANIMATED: return GIF_COUNT;
      case ViewType::LOTTIE:   return LOTTIE_COUNT;
      default:                  return IMAGE_COUNT;
    }
  }

  View CreateView()
  {
    const char* url = GetCurrentUrl();
    View view;
    if(mViewType == ViewType::ANIMATED)
      view = AnimatedImageView::New(url);
    else if(mViewType == ViewType::LOTTIE)
      view = LottieAnimationView::New(url);
    else
      view = ImageView::New(url);

    view.SetRequestedWidth(MATCH_PARENT);
    view.SetRequestedHeight(MATCH_PARENT);
    view.SetProperty(ImageView::Property::FITTING_MODE, static_cast<int>(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO));
    view.SetProperty(ImageView::Property::SYNCHRONOUS_LOADING, mSyncLoading);
    if(mViewType != ViewType::LOTTIE)
      view.SetProperty(ImageView::Property::FAST_TRACK_UPLOADING, mFastTrack);
    view.SetProperty(ImageView::Property::RELEASE_POLICY, RELEASE_POLICIES[mReleasePolicyIndex].policy);
    return view;
  }

  void RebuildView()
  {
    if(mCurrentView) mImageContainer.Remove(mCurrentView);
    mCurrentView = CreateView();
    ConnectResourceReady(mCurrentView, &ImageLoadingPolicyController::OnMainResourceReady);
    mImageContainer.Add(mCurrentView);
  }

  void SetViewUrl(View view, const char* url)
  {
    if(auto v = AnimatedImageView::DownCast(view))    v.SetResourceUrl(url);
    else if(auto v = LottieAnimationView::DownCast(view)) v.SetResourceUrl(url);
    else view.SetProperty(ImageView::Property::IMAGE, url);
  }

  void ConnectResourceReady(View view, void (ImageLoadingPolicyController::*callback)(View))
  {
    if(auto v = ImageView::DownCast(view))              v.ResourceReadySignal().Connect(this, callback);
    else if(auto v = AnimatedImageView::DownCast(view)) v.ResourceReadySignal().Connect(this, callback);
    else if(auto v = LottieAnimationView::DownCast(view)) v.ResourceReadySignal().Connect(this, callback);
  }

  void PlayView(View view)
  {
    if(auto v = AnimatedImageView::DownCast(view))    v.Play();
    else if(auto v = LottieAnimationView::DownCast(view)) v.Play();
  }

  void StopAnimView(View view)
  {
    if(auto v = AnimatedImageView::DownCast(view))    v.Stop();
    else if(auto v = LottieAnimationView::DownCast(view)) v.Stop();
  }

  // ── Timers ────────────────────────────────────────────────────────────────

  void StopAllTimers()
  {
    if(mHideTimer)    { mHideTimer.Stop();    mHideTimer.Reset(); }
    if(mCheckTimer)   { mCheckTimer.Stop();   mCheckTimer.Reset(); }
    if(mLoadAddTimer) { mLoadAddTimer.Stop(); mLoadAddTimer.Reset(); }
  }

  // ── Callbacks ─────────────────────────────────────────────────────────────

  void OnTypeClicked(View clicked, InputEvent)
  {
    for(int i = 0; i < TYPE_COUNT; ++i)
    {
      if(mTypeButtons[i] == clicked && mViewType != static_cast<ViewType>(i))
      {
        mTypeButtons[static_cast<int>(mViewType)].SetBackgroundColor(UiColor(0x444444));
        mViewType   = static_cast<ViewType>(i);
        mImageIndex = 0;
        mTypeButtons[i].SetBackgroundColor(UiColor(0xD32F2F));
        StopAllTimers();
        RebuildView();
        RefreshStatus();
        return;
      }
    }
  }

  void OnChangeClicked(View, InputEvent)
  {
    mImageIndex = (mImageIndex + 1) % GetUrlCount();
    SetViewUrl(mCurrentView, GetCurrentUrl());
  }

  void OnPlayClicked(View, InputEvent)  { PlayView(mCurrentView); }
  void OnStopClicked(View, InputEvent)  { StopAnimView(mCurrentView); }
  void OnReloadClicked(View, InputEvent)
  {
    if(auto v = ImageView::DownCast(mCurrentView)) v.Reload();
  }

  void OnSyncClicked(View, InputEvent)
  {
    mSyncLoading = !mSyncLoading;
    mSyncBtnLabel.SetText(mSyncLoading ? "SYNC: ON" : "SYNC: OFF");
    mSyncBtn.SetBackgroundColor(mSyncLoading ? UiColor(0x1565C0) : UiColor(0x444444));
    StopAllTimers();
    RebuildView();
    RefreshStatus();
  }

  void OnFastTrackClicked(View, InputEvent)
  {
    if(mViewType == ViewType::LOTTIE) return;
    mFastTrack = !mFastTrack;
    mFastTrackBtnLabel.SetText(mFastTrack ? "FASTTRACK: ON" : "FASTTRACK: OFF");
    mFastTrackBtn.SetBackgroundColor(mFastTrack ? UiColor(0x1565C0) : UiColor(0x444444));
    StopAllTimers();
    RebuildView();
    RefreshStatus();
  }

  void OnRelDetachedClicked(View, InputEvent)  { SetReleasePolicy(0); }
  void OnRelDestroyedClicked(View, InputEvent) { SetReleasePolicy(1); }
  void OnRelNeverClicked(View, InputEvent)     { SetReleasePolicy(2); }

  void SetReleasePolicy(int index)
  {
    mReleaseBtns[mReleasePolicyIndex].SetBackgroundColor(UiColor(0x444444));
    mReleasePolicyIndex = index;
    mReleaseBtns[mReleasePolicyIndex].SetBackgroundColor(UiColor(0x1565C0));
    if(mCurrentView) mCurrentView.SetProperty(ImageView::Property::RELEASE_POLICY, RELEASE_POLICIES[mReleasePolicyIndex].policy);
    RefreshStatus();
  }

  void OnHideClicked(View, InputEvent)
  {
    if(mHideTimer)  { mHideTimer.Stop();  mHideTimer.Reset(); }
    if(mCheckTimer) { mCheckTimer.Stop(); mCheckTimer.Reset(); }
    mReleasePolicyReadyAfterReAdd = false;
    if(mCurrentView) mImageContainer.Remove(mCurrentView);
    mHideTimer = Timer::New(1000);
    mHideTimer.TickSignal().Connect(this, &ImageLoadingPolicyController::OnHideTimerTick);
    mHideTimer.Start();
  }

  bool OnHideTimerTick()
  {
    if(mCurrentView)
    {
      ConnectResourceReady(mCurrentView, &ImageLoadingPolicyController::OnHideReady);
      mImageContainer.Add(mCurrentView);
    }
    mCheckTimer = Timer::New(500);
    mCheckTimer.TickSignal().Connect(this, &ImageLoadingPolicyController::OnCheckTimerTick);
    mCheckTimer.Start();
    return false;
  }

  bool OnCheckTimerTick()
  {
    if(!mReleasePolicyReadyAfterReAdd)
      DALI_LOG_RELEASE_INFO("[Release] Served from cache — image was not released.\n");
    return false;
  }

  void OnHideReady(View)
  {
    if(!mReleasePolicyReadyAfterReAdd)
    {
      mReleasePolicyReadyAfterReAdd = true;
      DALI_LOG_RELEASE_INFO("[Release] Image reloaded after re-add to scene.\n");
    }
  }

  void OnLoadAttachedClicked(View, InputEvent)  { SetLoadPolicy(0); }
  void OnLoadImmediateClicked(View, InputEvent) { SetLoadPolicy(1); }

  void SetLoadPolicy(int index)
  {
    mLoadBtns[mLoadPolicyIndex].SetBackgroundColor(UiColor(0x444444));
    mLoadPolicyIndex = index;
    mLoadBtns[mLoadPolicyIndex].SetBackgroundColor(UiColor(0x1565C0));
    RefreshStatus();
  }

  void OnLoadTestClicked(View, InputEvent)
  {
    StopAllTimers();
    mPendingLoadView = CreateView();
    Ui::Image::LoadPolicy lp = (mLoadPolicyIndex == 0) ? Ui::Image::LoadPolicy::ATTACHED : Ui::Image::LoadPolicy::IMMEDIATE;
    mPendingLoadView.SetProperty(ImageView::Property::LOAD_POLICY, lp);
    ConnectResourceReady(mPendingLoadView, &ImageLoadingPolicyController::OnLoadTestReady);
    mLoadPolicyReadyFired   = false;
    mLoadPolicyAddedToScene = false;
    DALI_LOG_RELEASE_INFO("[LoadPolicy] View created. Adding to scene in 800ms...\n");
    mLoadAddTimer = Timer::New(800);
    mLoadAddTimer.TickSignal().Connect(this, &ImageLoadingPolicyController::OnLoadAddTimerTick);
    mLoadAddTimer.Start();
  }

  bool OnLoadAddTimerTick()
  {
    mLoadPolicyAddedToScene = true;
    if(mCurrentView) mImageContainer.Remove(mCurrentView);
    mCurrentView = mPendingLoadView;
    mPendingLoadView.Reset();
    mImageContainer.Add(mCurrentView);
    DALI_LOG_RELEASE_INFO("[LoadPolicy] Added to scene. ResourceReady had fired %s.\n",
                          mLoadPolicyReadyFired ? "BEFORE (IMMEDIATE worked)" : "not yet (ATTACHED)");
    return false;
  }

  void OnLoadTestReady(View)
  {
    mLoadPolicyReadyFired = true;
    DALI_LOG_RELEASE_INFO("[LoadPolicy] ResourceReady fired %s scene add.\n",
                          mLoadPolicyAddedToScene ? "AFTER" : "BEFORE");
  }

  void OnMainResourceReady(View view)
  {
    int status = 0;
    if(auto v = ImageView::DownCast(view))              status = static_cast<int>(v.GetLoadingStatus());
    else if(auto v = AnimatedImageView::DownCast(view)) status = static_cast<int>(v.GetLoadingStatus());
    else if(auto v = LottieAnimationView::DownCast(view)) status = static_cast<int>(v.GetLoadingStatus());
    DALI_LOG_RELEASE_INFO("[Main] ResourceReady. Status=%d\n", status);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN && (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK)))
      mApplication.Quit();
  }

  // ── Members ───────────────────────────────────────────────────────────────

  Application& mApplication;
  View         mImageContainer;
  View         mCurrentView;
  View         mPendingLoadView;
  Label        mStatusLabel;
  View         mTypeButtons[TYPE_COUNT];
  View         mSyncBtn;
  Label        mSyncBtnLabel;
  View         mFastTrackBtn;
  Label        mFastTrackBtnLabel;
  View         mReleaseBtns[3];
  View         mLoadBtns[2];
  Timer        mHideTimer;
  Timer        mCheckTimer;
  Timer        mLoadAddTimer;
  ViewType     mViewType;
  int          mImageIndex;
  bool         mSyncLoading;
  bool         mFastTrack;
  int          mReleasePolicyIndex;
  int          mLoadPolicyIndex;
  bool         mReleasePolicyReadyAfterReAdd;
  bool         mLoadPolicyReadyFired;
  bool         mLoadPolicyAddedToScene;
};

const ImageLoadingPolicyController::ReleasePolicyEntry ImageLoadingPolicyController::RELEASE_POLICIES[3] = {
  {"DETACHED", Ui::Image::ReleasePolicy::DETACHED},
  {"DESTROYED", Ui::Image::ReleasePolicy::DESTROYED},
  {"NEVER", Ui::Image::ReleasePolicy::NEVER},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  ImageLoadingPolicyController controller(application);
  application.MainLoop();
  return 0;
}
