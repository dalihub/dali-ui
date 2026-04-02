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
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView Loading Policy sample — three independent sections, all verified via logs.
 *
 * [Section 1: SynchronousLoading]
 * - OFF (default): decoded on a background thread. IsResourceReady() returns false
 *   immediately after SetResourceUrl(); ResourceReady signal fires later.
 * - ON : decoded on the main thread. IsResourceReady() returns true immediately
 *   after SetResourceUrl(); no ResourceReady signal fires (already ready).
 * - How to verify: press [CHANGE IMAGE] and check the log.
 *   OFF → "IsResourceReady = false (async)"
 *   ON  → "IsResourceReady = true  (sync)"
 *
 * [Section 2: FastTrackUploading]
 * - OFF (default): GPU texture upload happens on the main thread after decoding.
 * - ON : GPU texture upload happens on a background thread, reducing main-thread stalls.
 * - No observable difference from sample code; effect is measurable only via
 *   frame-time profiling tools with large images.
 * - Log confirms the setting in use at ResourceReady time.
 *
 * [Section 3: ReleasePolicy]
 * - DETACHED (default): texture released when the view is removed from the scene.
 *   Re-adding the view triggers a reload → ResourceReady fires again.
 * - DESTROYED: texture cached while the ImageView handle lives.
 *   Re-adding after hide → no reload → ResourceReady does NOT fire.
 * - NEVER: texture never released. Same observable behaviour as DESTROYED here.
 * - How to verify: select a policy, press [HIDE 1s], watch the log.
 *   DETACHED  → "ResourceReady fired after re-add (texture was released)"
 *   DESTROYED → "ResourceReady did NOT fire after re-add (texture cached)"
 *   NEVER     → "ResourceReady did NOT fire after re-add (texture cached)"
 *
 * Press Escape or Back to quit.
 */
class ImageViewLoadingPolicyController : public ConnectionTracker
{
  static constexpr int POLICY_COUNT = 3;

  struct PolicyEntry
  {
    const char*             name;
    Ui::ReleasePolicy::Type policy;
  };

  static const PolicyEntry POLICIES[POLICY_COUNT];

public:
  explicit ImageViewLoadingPolicyController(Application& application)
  : mApplication(application),
    mSyncLoading(false),
    mFastTrack(false),
    mPolicyIndex(0),
    mSyncImageIndex(0),
    mReleasePolicyReadyAfterReAdd(false)
  {
    mApplication.InitSignal().Connect(this, &ImageViewLoadingPolicyController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewLoadingPolicyController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateSectionLabel("1. SynchronousLoading  (log: IsResourceReady immediately after SetResourceUrl)"),
        CreateSyncLoadingArea(),
        CreateSyncLoadingInfoLabel(),
        CreateSyncLoadingRow(),
        CreateSectionLabel("2. FastTrackUploading  (log: setting at ResourceReady; profiling tool for effect)"),
        CreateFastTrackArea(),
        CreateFastTrackInfoLabel(),
        CreateFastTrackRow(),
        CreateSectionLabel("3. ReleasePolicy  (log: ResourceReady fires on re-add only for DETACHED)"),
        CreateReleasePolicyArea(),
        CreateReleasePolicyInfoLabel(),
        CreateReleasePolicyRow(),
      });
  }

  // ── Section 1: SynchronousLoading ──────────────────────────────────────

  View CreateSyncLoadingArea()
  {
    // ReleasePolicy=DETACHED: texture released when the view leaves the scene.
    // [CHANGE IMAGE] removes the view, changes URL, then re-adds it — guaranteeing
    // a cache miss so IsResourceReady() truly reflects the sync/async path.
    ImageView::New(RESOURCES_DIR "gallery-medium-49.jpg")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetFitSizeToImage(true)
      .SetReleasePolicy(Ui::ReleasePolicy::DETACHED)
      .SetSynchronousLoading(mSyncLoading)
      .As(mSyncLoadingImage);

    mSyncLoadingImage.ResourceReadySignal().Connect(this, &ImageViewLoadingPolicyController::OnSyncLoadingResourceReady);

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .SetBackgroundColor(UiColor(0x2A2A2A))
      .Children({mSyncLoadingImage})
      .As(mSyncLoadingContainer);
  }

  View CreateSyncLoadingInfoLabel()
  {
    return Label::New(MakeSyncLoadingInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mSyncLoadingInfoLabel);
  }

  View CreateSyncLoadingRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(56.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    StackLayout toggleButton = StackLayout::New(StackOrientation::VERTICAL)
                                 .SetRequestedWidth(WRAP_CONTENT)
                                 .SetRequestedHeight(MATCH_PARENT)
                                 .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                                 .SetBackgroundColor(UiColor(0x444444))
                                 .Children({
                                   Label::New("SYNC_LOAD: OFF")
                                     .SetRequestedWidth(MATCH_PARENT)
                                     .SetRequestedHeight(MATCH_PARENT)
                                     .SetFontSize(13.0f)
                                     .SetTextColor(UiColor(0xFFFFFF))
                                     .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                     .SetVerticalTextAlignment(Text::Alignment::CENTER)
                                     .As(mSyncLoadingLabel),
                                 });
    toggleButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewLoadingPolicyController::OnSyncLoadingToggleClicked);

    StackLayout changeButton = StackLayout::New(StackOrientation::VERTICAL)
                                 .SetRequestedWidth(WRAP_CONTENT)
                                 .SetRequestedHeight(MATCH_PARENT)
                                 .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                                 .SetBackgroundColor(UiColor(0x2E7D32))
                                 .Children({
                                   Label::New("CHANGE IMAGE")
                                     .SetRequestedWidth(MATCH_PARENT)
                                     .SetRequestedHeight(MATCH_PARENT)
                                     .SetFontSize(13.0f)
                                     .SetTextColor(UiColor(0xFFFFFF))
                                     .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                     .SetVerticalTextAlignment(Text::Alignment::CENTER),
                                 });
    changeButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewLoadingPolicyController::OnSyncLoadingChangeClicked);

    row.Add(toggleButton);
    row.Add(changeButton);
    return row;
  }

  // ── Section 2: FastTrackUploading ──────────────────────────────────────

  View CreateFastTrackArea()
  {
    ImageView::New(RESOURCES_DIR "gallery-large-3.jpg")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetFitSizeToImage(true)
      .SetFastTrackUploading(mFastTrack)
      .As(mFastTrackImage);

    mFastTrackImage.ResourceReadySignal().Connect(this, &ImageViewLoadingPolicyController::OnFastTrackResourceReady);

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .SetBackgroundColor(UiColor(0x222222))
      .Children({mFastTrackImage});
  }

  View CreateFastTrackInfoLabel()
  {
    return Label::New(MakeFastTrackInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mFastTrackInfoLabel);
  }

  View CreateFastTrackRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(56.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    StackLayout toggleButton = StackLayout::New(StackOrientation::VERTICAL)
                                 .SetRequestedWidth(WRAP_CONTENT)
                                 .SetRequestedHeight(MATCH_PARENT)
                                 .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                                 .SetBackgroundColor(UiColor(0x444444))
                                 .Children({
                                   Label::New("FAST_TRACK: OFF")
                                     .SetRequestedWidth(MATCH_PARENT)
                                     .SetRequestedHeight(MATCH_PARENT)
                                     .SetFontSize(13.0f)
                                     .SetTextColor(UiColor(0xFFFFFF))
                                     .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                     .SetVerticalTextAlignment(Text::Alignment::CENTER)
                                     .As(mFastTrackLabel),
                                 });
    toggleButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewLoadingPolicyController::OnFastTrackToggleClicked);

    StackLayout reloadButton = StackLayout::New(StackOrientation::VERTICAL)
                                 .SetRequestedWidth(WRAP_CONTENT)
                                 .SetRequestedHeight(MATCH_PARENT)
                                 .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                                 .SetBackgroundColor(UiColor(0x2E7D32))
                                 .Children({
                                   Label::New("RELOAD")
                                     .SetRequestedWidth(MATCH_PARENT)
                                     .SetRequestedHeight(MATCH_PARENT)
                                     .SetFontSize(13.0f)
                                     .SetTextColor(UiColor(0xFFFFFF))
                                     .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                     .SetVerticalTextAlignment(Text::Alignment::CENTER),
                                 });
    reloadButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewLoadingPolicyController::OnFastTrackReloadClicked);

    row.Add(toggleButton);
    row.Add(reloadButton);
    return row;
  }

  // ── Section 3: ReleasePolicy ────────────────────────────────────────────

  View CreateReleasePolicyArea()
  {
    ImageView::New(RESOURCES_DIR "gallery-medium-3.jpg")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetFitSizeToImage(true)
      .SetReleasePolicy(POLICIES[mPolicyIndex].policy)
      .As(mReleasePolicyImage);

    mReleasePolicyImage.ResourceReadySignal().Connect(this, &ImageViewLoadingPolicyController::OnReleasePolicyResourceReady);

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .SetBackgroundColor(UiColor(0x1E1E2E))
      .Children({mReleasePolicyImage})
      .As(mReleasePolicyContainer);
  }

  View CreateReleasePolicyInfoLabel()
  {
    return Label::New(MakeReleasePolicyInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mReleasePolicyInfoLabel);
  }

  View CreateReleasePolicyRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(56.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < POLICY_COUNT; ++i)
    {
      row.Add(CreateReleasePolicyButton(i));
    }

    StackLayout hideButton = StackLayout::New(StackOrientation::VERTICAL)
                               .SetRequestedWidth(WRAP_CONTENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                               .SetBackgroundColor(UiColor(0x7B3F00))
                               .Children({
                                 Label::New("HIDE 1s")
                                   .SetRequestedWidth(MATCH_PARENT)
                                   .SetRequestedHeight(MATCH_PARENT)
                                   .SetFontSize(13.0f)
                                   .SetTextColor(UiColor(0xFFFFFF))
                                   .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                   .SetVerticalTextAlignment(Text::Alignment::CENTER),
                               });
    hideButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewLoadingPolicyController::OnHideClicked);

    row.Add(hideButton);
    return row;
  }

  View CreateReleasePolicyButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mPolicyIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(POLICIES[index].name)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(11.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });
    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewLoadingPolicyController::OnReleasePolicyButtonClicked);
    mPolicyButtons[index] = button;
    return button;
  }

  View CreateSectionLabel(const char* text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(28.0f)
      .SetFontSize(11.0f)
      .SetTextColor(UiColor(0x888888))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
  }

  // ── Callbacks ───────────────────────────────────────────────────────────

  void OnSyncLoadingToggleClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mSyncLoading = !mSyncLoading;
    mSyncLoadingImage.SetSynchronousLoading(mSyncLoading);
    Label::DownCast(mSyncLoadingLabel).SetText(mSyncLoading ? "SYNC_LOAD: ON" : "SYNC_LOAD: OFF");
    mSyncLoadingInfoLabel.SetText(MakeSyncLoadingInfoText());
    DALI_LOG_ERROR("[SyncLoading] SynchronousLoading toggled → %s\n", mSyncLoading ? "ON" : "OFF");
  }

  void OnSyncLoadingChangeClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    static const char* images[] = {
      RESOURCES_DIR "gallery-medium-49.jpg",
      RESOURCES_DIR "gallery-medium-3.jpg",
      RESOURCES_DIR "gallery-large-3.jpg",
    };
    constexpr int imageCount = static_cast<int>(sizeof(images) / sizeof(images[0]));

    mSyncPendingUrl = images[mSyncImageIndex % imageCount];
    ++mSyncImageIndex;

    // Remove from scene first → DETACHED policy releases the texture from cache.
    // A short timer is needed before SetResourceUrl + Add so the texture release
    // actually completes before the next load starts (release is not immediate).
    StackLayout::DownCast(mSyncLoadingContainer).Remove(mSyncLoadingImage);

    mSyncChangeTimer = Timer::New(100);
    mSyncChangeTimer.TickSignal().Connect(this, &ImageViewLoadingPolicyController::OnSyncChangeTimerTick);
    mSyncChangeTimer.Start();

    mSyncLoadingInfoLabel.SetText(MakeSyncLoadingInfoText());
  }

  bool OnSyncChangeTimerTick()
  {
    // Texture should be released by now (DETACHED). Set new URL and re-add.
    mSyncLoadingImage.SetResourceUrl(mSyncPendingUrl);
    StackLayout::DownCast(mSyncLoadingContainer).Add(mSyncLoadingImage);

    // SYNC ON : decoded on main thread → IsResourceReady() = true immediately
    // SYNC OFF: decoded on background thread → IsResourceReady() = false here,
    //           ResourceReady signal fires later on the main thread
    DALI_LOG_ERROR("[SyncLoading] SetResourceUrl (SynchronousLoading=%s) → IsResourceReady=%s\n",
                   mSyncLoading ? "ON " : "OFF",
                   mSyncLoadingImage.IsResourceReady() ? "true  (sync: already ready)" : "false (async: fires later)");
    return false; // one-shot
  }

  void OnSyncLoadingResourceReady(View /*view*/)
  {
    // SYNC OFF: fires after background decode completes — expected async notification.
    // SYNC ON : fires on scene re-attach even though the image was already ready
    //           (framework always emits ResourceReady on attach if resource is ready).
    //           The meaningful check is IsResourceReady()=true immediately after SetResourceUrl().
    DALI_LOG_ERROR("[SyncLoading] ResourceReady fired (SynchronousLoading=%s)%s\n",
                   mSyncLoading ? "ON " : "OFF",
                   mSyncLoading ? " — scene re-attach notification (image was already ready)"
                                : " — async decode complete");
  }

  void OnFastTrackToggleClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mFastTrack = !mFastTrack;
    mFastTrackImage.SetFastTrackUploading(mFastTrack);
    Label::DownCast(mFastTrackLabel).SetText(mFastTrack ? "FAST_TRACK: ON" : "FAST_TRACK: OFF");
    mFastTrackInfoLabel.SetText(MakeFastTrackInfoText());
    DALI_LOG_ERROR("[FastTrack] FastTrackUploading toggled → %s\n", mFastTrack ? "ON" : "OFF");
  }

  void OnFastTrackReloadClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    // FastTrackUploading difference (main-thread vs background GPU upload) is not
    // observable from sample code. Use a frame-time profiling tool with large images.
    DALI_LOG_ERROR("[FastTrack] Reload() — FastTrackUploading=%s (GPU upload on %s)\n",
                   mFastTrack ? "ON " : "OFF",
                   mFastTrack ? "background thread" : "main thread    ");
    mFastTrackImage.Reload();
  }

  void OnFastTrackResourceReady(View /*view*/)
  {
    DALI_LOG_ERROR("[FastTrack] ResourceReady — FastTrackUploading=%s\n",
                   mFastTrack ? "ON " : "OFF");
    if(mFastTrackInfoLabel)
    {
      mFastTrackInfoLabel.SetText(MakeFastTrackInfoText());
    }
  }

  void OnReleasePolicyButtonClicked(View clickedView, const InputEvent& /*event*/)
  {
    for(int i = 0; i < POLICY_COUNT; ++i)
    {
      if(mPolicyButtons[i] == clickedView)
      {
        mPolicyButtons[mPolicyIndex].SetBackgroundColor(UiColor(0x333333));
        mPolicyIndex = i;
        mPolicyButtons[mPolicyIndex].SetBackgroundColor(UiColor(0x4A90E2));
        mReleasePolicyImage.SetReleasePolicy(POLICIES[mPolicyIndex].policy);
        mReleasePolicyInfoLabel.SetText(MakeReleasePolicyInfoText());
        DALI_LOG_ERROR("[ReleasePolicy] Policy changed → %s\n", POLICIES[mPolicyIndex].name);
        return;
      }
    }
  }

  void OnHideClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mReleasePolicyReadyAfterReAdd = false;
    DALI_LOG_ERROR("[ReleasePolicy] Removing view from scene (policy=%s) — re-add in 1s\n",
                   POLICIES[mPolicyIndex].name);
    StackLayout::DownCast(mReleasePolicyContainer).Remove(mReleasePolicyImage);

    mHideTimer = Timer::New(1000);
    mHideTimer.TickSignal().Connect(this, &ImageViewLoadingPolicyController::OnHideTimerTick);
    mHideTimer.Start();
  }

  bool OnHideTimerTick()
  {
    DALI_LOG_ERROR("[ReleasePolicy] Re-adding view to scene (policy=%s) — watch for ResourceReady\n",
                   POLICIES[mPolicyIndex].name);
    StackLayout::DownCast(mReleasePolicyContainer).Add(mReleasePolicyImage);
    mReleasePolicyInfoLabel.SetText(MakeReleasePolicyInfoText());

    // Post a one-shot check: if ResourceReady hasn't fired by next frame, texture was cached.
    mCheckTimer = Timer::New(500);
    mCheckTimer.TickSignal().Connect(this, &ImageViewLoadingPolicyController::OnCheckTimerTick);
    mCheckTimer.Start();
    return false; // one-shot
  }

  bool OnCheckTimerTick()
  {
    if(!mReleasePolicyReadyAfterReAdd)
    {
      DALI_LOG_ERROR("[ReleasePolicy] ResourceReady did NOT fire after re-add (policy=%s) → texture was cached\n",
                     POLICIES[mPolicyIndex].name);
    }
    return false; // one-shot
  }

  void OnReleasePolicyResourceReady(View /*view*/)
  {
    mReleasePolicyReadyAfterReAdd = true;
    DALI_LOG_ERROR("[ReleasePolicy] ResourceReady fired after re-add (policy=%s) → texture was released and reloaded\n",
                   POLICIES[mPolicyIndex].name);
    if(mReleasePolicyInfoLabel)
    {
      mReleasePolicyInfoLabel.SetText(MakeReleasePolicyInfoText());
    }
  }

  Dali::String MakeSyncLoadingInfoText() const
  {
    return Dali::String(mSyncLoading
                          ? "SynchronousLoading: ON  — IsResourceReady=true immediately"
                          : "SynchronousLoading: OFF — IsResourceReady=false, fires async");
  }

  Dali::String MakeFastTrackInfoText() const
  {
    return Dali::String(mFastTrack
                          ? "FastTrackUploading: ON  — GPU upload on background thread"
                          : "FastTrackUploading: OFF — GPU upload on main thread");
  }

  Dali::String MakeReleasePolicyInfoText() const
  {
    char buf[128];
    const char* desc = "";
    switch(POLICIES[mPolicyIndex].policy)
    {
      case Ui::ReleasePolicy::DETACHED:  desc = "ResourceReady fires on re-add";        break;
      case Ui::ReleasePolicy::DESTROYED: desc = "ResourceReady silent on re-add";       break;
      case Ui::ReleasePolicy::NEVER:     desc = "ResourceReady silent on re-add";       break;
    }
    snprintf(buf, sizeof(buf), "ReleasePolicy: %s  — %s", POLICIES[mPolicyIndex].name, desc);
    return Dali::String(buf);
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
  Application&    mApplication;
  Ui::ImageView   mSyncLoadingImage;
  Ui::ImageView   mFastTrackImage;
  View            mSyncLoadingContainer;
  Ui::ImageView   mReleasePolicyImage;
  Label           mSyncLoadingInfoLabel;
  Label           mFastTrackInfoLabel;
  Label           mReleasePolicyInfoLabel;
  View            mSyncLoadingLabel;
  View            mFastTrackLabel;
  View            mReleasePolicyContainer;
  View            mPolicyButtons[POLICY_COUNT];
  Timer           mSyncChangeTimer;
  Timer           mHideTimer;
  Timer           mCheckTimer;
  Dali::String    mSyncPendingUrl;
  bool            mSyncLoading;
  bool            mFastTrack;
  int             mPolicyIndex;
  int             mSyncImageIndex;
  bool            mReleasePolicyReadyAfterReAdd;
};

const ImageViewLoadingPolicyController::PolicyEntry ImageViewLoadingPolicyController::POLICIES[ImageViewLoadingPolicyController::POLICY_COUNT] = {
  {"DETACHED",  Ui::ReleasePolicy::DETACHED},
  {"DESTROYED", Ui::ReleasePolicy::DESTROYED},
  {"NEVER",     Ui::ReleasePolicy::NEVER},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewLoadingPolicyController controller(application);
  application.MainLoop();
  return 0;
}
