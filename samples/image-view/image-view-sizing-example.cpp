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
 * ImageView Sizing sample — two independent sections:
 *
 * [Section 1: ImageLoadWithViewSize]
 * - Direction: view size → image load size.
 * - Fixed-height container (200px).
 * - OFF: image decoded at full resolution once → ResourceReady fires 1 time.
 * - ON : image decoded at full resolution first, then OnSetTransform detects the view
 *        size and reloads at display dimensions → ResourceReady fires 2 times.
 *        The second load is memory-efficient: texture size == view size.
 * - After the view is fully laid out, explicit Reload() fires ResourceReady 1 time only
 *   because mLastRequiredSize already holds the correct view size.
 * - The info label shows the ResourceReady call count to make this difference observable.
 *
 * NOTE: Do NOT use ImageLoadWithViewSize together with a WRAP_CONTENT axis expecting
 * aspect-ratio adjustment. ImageLoadWithViewSize overrides GetNaturalSize with the view
 * size, making aspect-ratio layout ineffective.
 *
 * [Section 2: OrientationCorrection]
 * - Corrects EXIF orientation metadata embedded in JPEG files.
 * - Effect is visible only with EXIF-rotated images (EXIF orientation != 1).
 *   Replace the image URL with such a file to verify.
 *
 * Press Escape or Back to quit.
 */
class ImageViewSizingController : public ConnectionTracker
{
public:
  explicit ImageViewSizingController(Application& application)
  : mApplication(application),
    mImageLoadWithViewSize(false),
    mOrientationCorrection(true),
    mSyncResourceReadyCount(0)
  {
    mApplication.InitSignal().Connect(this, &ImageViewSizingController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewSizingController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateSectionLabel("1. ImageLoadWithViewSize  (view size → image load size)"),
        CreateSyncSizeContainer(),
        CreateSyncSizeInfoLabel(),
        CreateSyncSizeRow(),
        CreateSectionLabel("2. OrientationCorrection  (exif-rotated.jpg: EXIF orientation=6, Rotate 90 CW)"),
        CreateOrientationArea(),
        CreateOrientationInfoLabel(),
        CreateOrientationToggle(),
      });
  }

  // ── Section 1: ImageLoadWithViewSize ───────────────────────────────────────

  View CreateSyncSizeContainer()
  {
    // Fixed-height container so the view size is stable and OnSetTransform gets a real size.
    // SYNC OFF: image decoded at original resolution once → ResourceReady fires 1 time.
    // SYNC ON : initial load fires ResourceReady, then OnSetTransform reloads at view
    //           dimensions → ResourceReady fires a 2nd time (texture size == view size).
    //           For explicit Reload() after view is laid out, mLastRequiredSize already
    //           holds the view size → ResourceReady fires 1 time only.
    ImageView::New(RESOURCES_DIR "gallery-medium-49.jpg")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetImageLoadWithViewSize(mImageLoadWithViewSize)
      .As(mSyncImage);

    mSyncImage.ResourceReadySignal().Connect(this, &ImageViewSizingController::OnSyncImageResourceReady);

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(200.0f)
      .SetBackgroundColor(UiColor(0x2A2A2A))
      .Children({mSyncImage});
  }

  View CreateSyncSizeInfoLabel()
  {
    return Label::New(MakeSyncSizeInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mSyncSizeInfoLabel);
  }

  View CreateSyncSizeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(56.0f)
                        .SetPadding(Extents(4, 4, 4, 4));

    StackLayout toggleButton = StackLayout::New(StackOrientation::VERTICAL)
                                 .SetRequestedWidth(WRAP_CONTENT)
                                 .SetRequestedHeight(MATCH_PARENT)
                                 .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                                 .SetBackgroundColor(UiColor(0x444444))
                                 .Children({
                                   Label::New("SYNC_SIZE: OFF")
                                     .SetRequestedWidth(MATCH_PARENT)
                                     .SetRequestedHeight(MATCH_PARENT)
                                     .SetFontSize(13.0f)
                                     .SetTextColor(UiColor(0xFFFFFF))
                                     .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                     .SetVerticalTextAlignment(Text::Alignment::CENTER)
                                     .As(mSyncSizeLabel),
                                 });
    toggleButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewSizingController::OnSyncSizeToggleClicked);

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
    reloadButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewSizingController::OnSyncReloadClicked);

    row.Add(toggleButton);
    row.Add(reloadButton);
    return row;
  }

  // ── Section 2: OrientationCorrection ───────────────────────────────────

  View CreateOrientationArea()
  {
    // exif-rotated.jpg: actual pixels are 360x640 (portrait) but EXIF orientation=6
    // (Rotate 90 CW), meaning the image was shot in landscape and stored rotated.
    //
    // ON : EXIF is applied → displayed as landscape (640x360 aspect ratio)
    // OFF: EXIF ignored    → displayed as portrait  (360x640 aspect ratio)
    ImageView::New(RESOURCES_DIR "exif-rotated.jpg")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetOrientationCorrection(mOrientationCorrection)
      .As(mOrientationImage);

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(200.0f)
      .SetBackgroundColor(UiColor(0x222222))
      .Children({mOrientationImage});
  }

  View CreateOrientationInfoLabel()
  {
    return Label::New(MakeOrientationInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mOrientationInfoLabel);
  }

  View CreateOrientationToggle()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(MATCH_PARENT)
                           .SetRequestedHeight(56.0f)
                           .SetPadding(Extents(4, 4, 4, 4))
                           .SetBackgroundColor(UiColor(0x555555))
                           .Children({
                             Label::New("ORIENTATION CORRECTION: ON")
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(13.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER)
                               .As(mOrientationLabel),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewSizingController::OnOrientationToggleClicked);
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

  void OnSyncSizeToggleClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mImageLoadWithViewSize      = !mImageLoadWithViewSize;
    mSyncResourceReadyCount = 0;
    mSyncImage.SetImageLoadWithViewSize(mImageLoadWithViewSize);
    mSyncSizeLabel.SetText(mImageLoadWithViewSize ? "SYNC_SIZE: ON" : "SYNC_SIZE: OFF");
    mSyncSizeInfoLabel.SetText(MakeSyncSizeInfoText());
    DALI_LOG_RELEASE_INFO("[SyncSizing] ImageLoadWithViewSize toggled → %s\n", mImageLoadWithViewSize ? "ON" : "OFF");
  }

  void OnSyncReloadClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mSyncResourceReadyCount = 0;
    DALI_LOG_RELEASE_INFO("[SyncSizing] Reload() (ImageLoadWithViewSize=%s) — expecting 1x ResourceReady (loads at current view size)\n",
                          mImageLoadWithViewSize ? "ON" : "OFF");
    mSyncImage.Reload();
  }

  void OnSyncImageResourceReady(View /*view*/)
  {
    ++mSyncResourceReadyCount;
    MeasuredSize size = mSyncImage.GetSize();
    DALI_LOG_RELEASE_INFO("[SyncSizing] ResourceReady #%d (ImageLoadWithViewSize=%s) — viewSize=(%.0f,%.0f)\n",
                          mSyncResourceReadyCount,
                          mImageLoadWithViewSize ? "ON" : "OFF",
                          size.GetWidth(),
                          size.GetHeight());

    if(mSyncSizeInfoLabel)
    {
      mSyncSizeInfoLabel.SetText(MakeSyncSizeInfoText());
    }
  }

  void OnOrientationToggleClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mOrientationCorrection = !mOrientationCorrection;
    mOrientationImage.SetOrientationCorrection(mOrientationCorrection);
    mOrientationImage.Reload();
    mOrientationLabel.SetText(mOrientationCorrection ? "ORIENTATION CORRECTION: ON" : "ORIENTATION CORRECTION: OFF");
    mOrientationInfoLabel.SetText(MakeOrientationInfoText());
    DALI_LOG_RELEASE_INFO("[Sizing] OrientationCorrection=%d\n", mOrientationCorrection);
  }

  Dali::String MakeSyncSizeInfoText() const
  {
    char buf[128];
    snprintf(buf, sizeof(buf),
             "ImageLoadWithViewSize: %s  — ResourceReady count: %d",
             mImageLoadWithViewSize ? "ON " : "OFF",
             mSyncResourceReadyCount);
    return Dali::String(buf);
  }

  Dali::String MakeOrientationInfoText() const
  {
    return Dali::String(mOrientationCorrection ? "OrientationCorrection: ON" : "OrientationCorrection: OFF");
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
  Ui::ImageView mSyncImage;
  Ui::ImageView mOrientationImage;
  Label         mSyncSizeInfoLabel;
  Label         mOrientationInfoLabel;
  Label         mSyncSizeLabel;
  Label         mOrientationLabel;
  bool          mImageLoadWithViewSize;
  bool          mOrientationCorrection;
  int           mSyncResourceReadyCount;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewSizingController controller(application);
  application.MainLoop();
  return 0;
}
