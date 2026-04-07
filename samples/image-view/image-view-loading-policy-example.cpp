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
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView Loading Policy sample — four independent sections, all verified via logs.
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
 * [Section 4: LoadPolicy]
 * - ATTACHED (default): loading starts only when the ImageView is added to the scene.
 *   ResourceReady fires after some delay following scene add (loading begins then).
 * - IMMEDIATE: loading starts as soon as the URL is set, before scene add.
 *   ResourceReady fires quickly after scene add because the texture was pre-loaded.
 *   Note: ResourceReady still requires the view to be on-scene; it cannot fire before add.
 * - How to verify: select a policy, press [TEST], watch the log.
 *   ATTACHED  → "ResourceReady fired [slow] AFTER scene add (loading started on add)"
 *   IMMEDIATE → "ResourceReady fired [fast] AFTER scene add (texture was pre-loaded)"
 *
 * Press Escape or Back to quit.
 */
class ImageViewLoadingPolicyController : public ConnectionTracker
{
  static constexpr int POLICY_COUNT      = 3;
  static constexpr int LOAD_POLICY_COUNT = 2;

  struct PolicyEntry
  {
    const char*             name;
    Ui::ReleasePolicy::Type policy;
  };

  struct LoadPolicyEntry
  {
    const char*           name;
    Ui::LoadPolicy::Type  policy;
  };

  static const PolicyEntry     POLICIES[POLICY_COUNT];
  static const LoadPolicyEntry LOAD_POLICIES[LOAD_POLICY_COUNT];

public:
  explicit ImageViewLoadingPolicyController(Application& application)
  : mApplication(application),
    mSyncLoading(false),
    mFastTrack(false),
    mPolicyIndex(0),
    mLoadPolicyIndex(0),
    mSyncImageIndex(0),
    mReleasePolicyReadyAfterReAdd(false),
    mLoadPolicyReadyFired(false),
    mLoadPolicyAddedToScene(false)
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
        CreateSectionLabel("4. LoadPolicy  (log: ResourceReady fires before/after scene add)"),
        CreateLoadPolicyArea(),
        CreateLoadPolicyInfoLabel(),
        CreateLoadPolicyRow(),
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

  // ── Section 4: LoadPolicy ───────────────────────────────────────────────

  View CreateLoadPolicyArea()
  {
    // Placeholder shown while waiting for the test ImageView to be added.
    // The actual mLoadPolicyImage is created fresh on each [TEST] press.
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .SetBackgroundColor(UiColor(0x1A2A1A))
      .Children({
        Label::New("Press [TEST] to create an ImageView (not yet added to scene)")
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(12.0f)
          .SetTextColor(UiColor(0x888888))
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
      })
      .As(mLoadPolicyContainer);
  }

  View CreateLoadPolicyInfoLabel()
  {
    return Label::New(MakeLoadPolicyInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mLoadPolicyInfoLabel);
  }

  View CreateLoadPolicyRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(56.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < LOAD_POLICY_COUNT; ++i)
    {
      row.Add(CreateLoadPolicyButton(i));
    }

    StackLayout testButton = StackLayout::New(StackOrientation::VERTICAL)
                               .SetRequestedWidth(WRAP_CONTENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                               .SetBackgroundColor(UiColor(0x2E7D32))
                               .Children({
                                 Label::New("TEST")
                                   .SetRequestedWidth(MATCH_PARENT)
                                   .SetRequestedHeight(MATCH_PARENT)
                                   .SetFontSize(13.0f)
                                   .SetTextColor(UiColor(0xFFFFFF))
                                   .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                   .SetVerticalTextAlignment(Text::Alignment::CENTER),
                               });
    testButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewLoadingPolicyController::OnLoadPolicyTestClicked);

    row.Add(testButton);
    return row;
  }

  View CreateLoadPolicyButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mLoadPolicyIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(LOAD_POLICIES[index].name)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(11.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });
    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewLoadingPolicyController::OnLoadPolicyButtonClicked);
    mLoadPolicyButtons[index] = button;
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
    mSyncLoadingLabel.SetText(mSyncLoading ? "SYNC_LOAD: ON" : "SYNC_LOAD: OFF");
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
    mSyncLoadingContainer.Remove(mSyncLoadingImage);

    mSyncChangeTimer = Timer::New(100);
    mSyncChangeTimer.TickSignal().Connect(this, &ImageViewLoadingPolicyController::OnSyncChangeTimerTick);
    mSyncChangeTimer.Start();

    mSyncLoadingInfoLabel.SetText(MakeSyncLoadingInfoText());
  }

  bool OnSyncChangeTimerTick()
  {
    // Texture should be released by now (DETACHED). Set new URL and re-add.
    mSyncLoadingImage.SetResourceUrl(mSyncPendingUrl);
    mSyncLoadingContainer.Add(mSyncLoadingImage);

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
    mFastTrackLabel.SetText(mFastTrack ? "FAST_TRACK: ON" : "FAST_TRACK: OFF");
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
    mReleasePolicyContainer.Remove(mReleasePolicyImage);

    mHideTimer = Timer::New(1000);
    mHideTimer.TickSignal().Connect(this, &ImageViewLoadingPolicyController::OnHideTimerTick);
    mHideTimer.Start();
  }

  bool OnHideTimerTick()
  {
    DALI_LOG_ERROR("[ReleasePolicy] Re-adding view to scene (policy=%s) — watch for ResourceReady\n",
                   POLICIES[mPolicyIndex].name);
    mReleasePolicyContainer.Add(mReleasePolicyImage);
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

  void OnLoadPolicyButtonClicked(View clickedView, const InputEvent& /*event*/)
  {
    for(int i = 0; i < LOAD_POLICY_COUNT; ++i)
    {
      if(mLoadPolicyButtons[i] == clickedView)
      {
        mLoadPolicyButtons[mLoadPolicyIndex].SetBackgroundColor(UiColor(0x333333));
        mLoadPolicyIndex = i;
        mLoadPolicyButtons[mLoadPolicyIndex].SetBackgroundColor(UiColor(0x4A90E2));
        mLoadPolicyInfoLabel.SetText(MakeLoadPolicyInfoText());
        DALI_LOG_ERROR("[LoadPolicy] Policy changed → %s\n", LOAD_POLICIES[mLoadPolicyIndex].name);
        return;
      }
    }
  }

  void OnLoadPolicyTestClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    // Reset previous test state.
    mLoadPolicyReadyFired   = false;
    mLoadPolicyAddedToScene = false;
    mLoadPolicyImage.Reset();

    // Clear the container so the placeholder is shown while loading.
    while(mLoadPolicyContainer.GetChildCount() > 0)
    {
      mLoadPolicyContainer.Remove(mLoadPolicyContainer.GetChildAt(0));
    }

    // Create a new ImageView with the selected LoadPolicy but do NOT add it to the scene yet.
    // To properly catch synchronous ResourceReady events (like when using IMMEDIATE),
    // we MUST connect the signal BEFORE setting the URL and Policy.
    mLoadPolicyImage = ImageView::New();
    mLoadPolicyImage.SetRequestedWidth(MATCH_PARENT);
    mLoadPolicyImage.SetRequestedHeight(WRAP_CONTENT);
    mLoadPolicyImage.SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO);

    mLoadPolicyImage.ResourceReadySignal().Connect(this, &ImageViewLoadingPolicyController::OnLoadPolicyResourceReady);

    mLoadPolicyImage.SetLoadPolicy(LOAD_POLICIES[mLoadPolicyIndex].policy);
    mLoadPolicyImage.SetResourceUrl(RESOURCES_DIR "gallery-large-3.jpg");

    DALI_LOG_ERROR("[LoadPolicy] ImageView created with policy=%s — NOT yet added to scene. Waiting 800ms...\n",
                   LOAD_POLICIES[mLoadPolicyIndex].name);
    mLoadPolicyInfoLabel.SetText(MakeLoadPolicyInfoText());

    // Add to scene after 800ms — enough time for IMMEDIATE to pre-load the texture.
    mLoadPolicyReadyFired = false;
    mLoadPolicyAddTimer   = Timer::New(800);
    mLoadPolicyAddTimer.TickSignal().Connect(this, &ImageViewLoadingPolicyController::OnLoadPolicyAddTimerTick);
    mLoadPolicyAddTimer.Start();
  }

  bool OnLoadPolicyAddTimerTick()
  {
    DALI_LOG_ERROR("[LoadPolicy] 800ms elapsed — adding ImageView to scene (policy=%s). "
                   "Watch for ResourceReady timing...\n",
                   LOAD_POLICIES[mLoadPolicyIndex].name);

    mLoadPolicyAddedToScene = true;
    mLoadPolicyContainer.Add(mLoadPolicyImage);
    mLoadPolicyInfoLabel.SetText(MakeLoadPolicyInfoText());

    // Post a short check: if ResourceReady fires quickly it means the texture was pre-loaded (IMMEDIATE).
    // If it fires slowly or only after this check, loading started on scene add (ATTACHED).
    mLoadPolicyCheckTimer = Timer::New(200);
    mLoadPolicyCheckTimer.TickSignal().Connect(this, &ImageViewLoadingPolicyController::OnLoadPolicyCheckTimerTick);
    mLoadPolicyCheckTimer.Start();

    return false; // one-shot
  }

  bool OnLoadPolicyCheckTimerTick()
  {
    if(mLoadPolicyReadyFired)
    {
      DALI_LOG_ERROR("[LoadPolicy] ✓ ResourceReady fired within 200ms of scene add (policy=%s) "
                     "— texture was pre-loaded (IMMEDIATE behaviour confirmed)\n",
                     LOAD_POLICIES[mLoadPolicyIndex].name);
    }
    else
    {
      DALI_LOG_ERROR("[LoadPolicy] ResourceReady NOT yet fired 200ms after scene add (policy=%s) "
                     "— loading started on scene add (ATTACHED behaviour)\n",
                     LOAD_POLICIES[mLoadPolicyIndex].name);
    }
    return false; // one-shot
  }

  void OnLoadPolicyResourceReady(View /*view*/)
  {
    mLoadPolicyReadyFired = true;

    bool isImmediate = (LOAD_POLICIES[mLoadPolicyIndex].policy == Ui::LoadPolicy::IMMEDIATE);
    DALI_LOG_ERROR("[LoadPolicy] ResourceReady fired (policy=%s) — %s scene add\n",
                   LOAD_POLICIES[mLoadPolicyIndex].name,
                   mLoadPolicyAddedToScene ? "AFTER" : (isImmediate ? "BEFORE (pre-loaded)" : "BEFORE (unexpected)"));

    if(mLoadPolicyInfoLabel)
    {
      mLoadPolicyInfoLabel.SetText(MakeLoadPolicyInfoText());
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

  Dali::String MakeLoadPolicyInfoText() const
  {
    char buf[128];
    const char* desc = "";
    switch(LOAD_POLICIES[mLoadPolicyIndex].policy)
    {
      case Ui::LoadPolicy::ATTACHED:  desc = "loading starts on scene add";          break;
      case Ui::LoadPolicy::IMMEDIATE: desc = "loading starts on view creation";      break;
    }
    snprintf(buf, sizeof(buf), "LoadPolicy: %s  — %s", LOAD_POLICIES[mLoadPolicyIndex].name, desc);
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
  StackLayout     mSyncLoadingContainer;
  Ui::ImageView   mReleasePolicyImage;
  Ui::ImageView   mLoadPolicyImage;
  Label           mSyncLoadingInfoLabel;
  Label           mFastTrackInfoLabel;
  Label           mReleasePolicyInfoLabel;
  Label           mLoadPolicyInfoLabel;
  Label           mSyncLoadingLabel;
  Label           mFastTrackLabel;
  StackLayout     mReleasePolicyContainer;
  StackLayout            mLoadPolicyContainer;
  View            mPolicyButtons[POLICY_COUNT];
  View            mLoadPolicyButtons[LOAD_POLICY_COUNT];
  Timer           mSyncChangeTimer;
  Timer           mHideTimer;
  Timer           mCheckTimer;
  Timer           mLoadPolicyAddTimer;
  Timer           mLoadPolicyCheckTimer;
  Dali::String    mSyncPendingUrl;
  bool            mSyncLoading;
  bool            mFastTrack;
  int             mPolicyIndex;
  int             mLoadPolicyIndex;
  int             mSyncImageIndex;
  bool            mReleasePolicyReadyAfterReAdd;
  bool            mLoadPolicyReadyFired;
  bool            mLoadPolicyAddedToScene;
};

const ImageViewLoadingPolicyController::PolicyEntry ImageViewLoadingPolicyController::POLICIES[ImageViewLoadingPolicyController::POLICY_COUNT] = {
  {"DETACHED",  Ui::ReleasePolicy::DETACHED},
  {"DESTROYED", Ui::ReleasePolicy::DESTROYED},
  {"NEVER",     Ui::ReleasePolicy::NEVER},
};

const ImageViewLoadingPolicyController::LoadPolicyEntry ImageViewLoadingPolicyController::LOAD_POLICIES[ImageViewLoadingPolicyController::LOAD_POLICY_COUNT] = {
  {"ATTACHED",  Ui::LoadPolicy::ATTACHED},
  {"IMMEDIATE", Ui::LoadPolicy::IMMEDIATE},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewLoadingPolicyController controller(application);
  application.MainLoop();
  return 0;
}
