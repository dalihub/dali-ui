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
#include <dali-ui-foundation/public-api/views/image/image-view.h>
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
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewSizingController::OnKeyEvent);
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.Add(CreateSectionLabel("1. ImageLoadWithViewSize  (view size → image load size)"));
    contents.Add(CreateSyncSizeContainer());
    contents.Add(CreateSyncSizeInfoLabel());
    contents.Add(CreateSyncSizeRow());
    contents.Add(CreateSectionLabel("2. OrientationCorrection  (exif-rotated.jpg: EXIF orientation=6, Rotate 90 CW)"));
    contents.Add(CreateOrientationArea());
    contents.Add(CreateOrientationInfoLabel());
    contents.Add(CreateOrientationToggle());
    return contents;
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
    mSyncImage = ImageView::New(RESOURCES_DIR "gallery-medium-49.jpg");
    mSyncImage.SetRequestedWidth(MATCH_PARENT);
    mSyncImage.SetRequestedHeight(MATCH_PARENT);
    mSyncImage.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    mSyncImage.SetImageLoadWithViewSize(mImageLoadWithViewSize);

    mSyncImage.ResourceReadySignal().Connect(this, &ImageViewSizingController::OnSyncImageResourceReady);

    StackLayout container = StackLayout::New(StackOrientation::VERTICAL);
    container.SetRequestedWidth(MATCH_PARENT);
    container.SetRequestedHeight(200.0f);
    container.SetBackgroundColor(UiColor(0x2A2A2A));
    container.Add(mSyncImage);
    return container;
  }

  View CreateSyncSizeInfoLabel()
  {
    mSyncSizeInfoLabel = Label::New(MakeSyncSizeInfoText());
    mSyncSizeInfoLabel.SetRequestedWidth(MATCH_PARENT);
    mSyncSizeInfoLabel.SetRequestedHeight(32.0f);
    mSyncSizeInfoLabel.SetFontSize(13.0f);
    mSyncSizeInfoLabel.SetTextColor(UiColor(0xCCCCCC));
    mSyncSizeInfoLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mSyncSizeInfoLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return mSyncSizeInfoLabel;
  }

  View CreateSyncSizeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(56.0f);
    row.SetPadding(Extents(4, 4, 4, 4));

    StackLayout toggleButton = StackLayout::New(StackOrientation::VERTICAL);
    toggleButton.SetRequestedWidth(WRAP_CONTENT);
    toggleButton.SetRequestedHeight(MATCH_PARENT);
    toggleButton.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    toggleButton.SetBackgroundColor(UiColor(0x444444));
    mSyncSizeLabel = Label::New("SYNC_SIZE: OFF");
    mSyncSizeLabel.SetRequestedWidth(MATCH_PARENT);
    mSyncSizeLabel.SetRequestedHeight(MATCH_PARENT);
    mSyncSizeLabel.SetFontSize(13.0f);
    mSyncSizeLabel.SetTextColor(UiColor(0xFFFFFF));
    mSyncSizeLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mSyncSizeLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    toggleButton.Add(mSyncSizeLabel);
    toggleButton.AsInteractive().ClickedSignal().Connect(this, &ImageViewSizingController::OnSyncSizeToggleClicked);

    StackLayout reloadButton = StackLayout::New(StackOrientation::VERTICAL);
    reloadButton.SetRequestedWidth(WRAP_CONTENT);
    reloadButton.SetRequestedHeight(MATCH_PARENT);
    reloadButton.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    reloadButton.SetBackgroundColor(UiColor(0x2E7D32));
    Label reloadLabel = Label::New("RELOAD");
    reloadLabel.SetRequestedWidth(MATCH_PARENT);
    reloadLabel.SetRequestedHeight(MATCH_PARENT);
    reloadLabel.SetFontSize(13.0f);
    reloadLabel.SetTextColor(UiColor(0xFFFFFF));
    reloadLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    reloadLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    reloadButton.Add(reloadLabel);
    reloadButton.AsInteractive().ClickedSignal().Connect(this, &ImageViewSizingController::OnSyncReloadClicked);

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
    mOrientationImage = ImageView::New(RESOURCES_DIR "exif-rotated.jpg");
    mOrientationImage.SetRequestedWidth(MATCH_PARENT);
    mOrientationImage.SetRequestedHeight(MATCH_PARENT);
    mOrientationImage.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    mOrientationImage.SetOrientationCorrection(mOrientationCorrection);

    StackLayout container = StackLayout::New(StackOrientation::VERTICAL);
    container.SetRequestedWidth(MATCH_PARENT);
    container.SetRequestedHeight(200.0f);
    container.SetBackgroundColor(UiColor(0x222222));
    container.Add(mOrientationImage);
    return container;
  }

  View CreateOrientationInfoLabel()
  {
    mOrientationInfoLabel = Label::New(MakeOrientationInfoText());
    mOrientationInfoLabel.SetRequestedWidth(MATCH_PARENT);
    mOrientationInfoLabel.SetRequestedHeight(32.0f);
    mOrientationInfoLabel.SetFontSize(13.0f);
    mOrientationInfoLabel.SetTextColor(UiColor(0xCCCCCC));
    mOrientationInfoLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mOrientationInfoLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return mOrientationInfoLabel;
  }

  View CreateOrientationToggle()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(MATCH_PARENT);
    button.SetRequestedHeight(56.0f);
    button.SetPadding(Extents(4, 4, 4, 4));
    button.SetBackgroundColor(UiColor(0x555555));
    mOrientationLabel = Label::New("ORIENTATION CORRECTION: ON");
    mOrientationLabel.SetRequestedWidth(MATCH_PARENT);
    mOrientationLabel.SetRequestedHeight(MATCH_PARENT);
    mOrientationLabel.SetFontSize(13.0f);
    mOrientationLabel.SetTextColor(UiColor(0xFFFFFF));
    mOrientationLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mOrientationLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(mOrientationLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageViewSizingController::OnOrientationToggleClicked);
    return button;
  }

  View CreateSectionLabel(const char* text)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(28.0f);
    label.SetFontSize(11.0f);
    label.SetTextColor(UiColor(0x888888));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  // ── Callbacks ───────────────────────────────────────────────────────────

  void OnSyncSizeToggleClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mImageLoadWithViewSize      = !mImageLoadWithViewSize;
    mSyncResourceReadyCount = 0;
    mSyncImage.SetImageLoadWithViewSize(mImageLoadWithViewSize);
    mSyncSizeLabel.SetText(mImageLoadWithViewSize ? "SYNC_SIZE: ON" : "SYNC_SIZE: OFF");
    mSyncSizeInfoLabel.SetText(MakeSyncSizeInfoText());
    DALI_LOG_RELEASE_INFO("[SyncSizing] ImageLoadWithViewSize toggled → %s\n", mImageLoadWithViewSize ? "ON" : "OFF");
  }

  void OnSyncReloadClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mSyncResourceReadyCount = 0;
    DALI_LOG_RELEASE_INFO("[SyncSizing] Reload() (ImageLoadWithViewSize=%s) — expecting 1x ResourceReady (loads at current view size)\n",
                          mImageLoadWithViewSize ? "ON" : "OFF");
    mSyncImage.Reload();
  }

  void OnSyncImageResourceReady(View /*view*/)
  {
    ++mSyncResourceReadyCount;
    Vector3 size = mSyncImage.GetSize();
    DALI_LOG_RELEASE_INFO("[SyncSizing] ResourceReady #%d (ImageLoadWithViewSize=%s) — viewSize=(%.0f,%.0f)\n",
                          mSyncResourceReadyCount,
                          mImageLoadWithViewSize ? "ON" : "OFF",
                          size.width,
                          size.height);

    if(mSyncSizeInfoLabel)
    {
      mSyncSizeInfoLabel.SetText(MakeSyncSizeInfoText());
    }
  }

  void OnOrientationToggleClicked(View /*clickedView*/, InputEvent /*event*/)
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

  void OnKeyEvent(Window window, KeyEvent event)
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
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  ImageViewSizingController controller(application);
  application.MainLoop();
  return 0;
}
