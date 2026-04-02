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
 * ImageView PixelArea sample:
 * - Displays a sub-region of the image using SetPixelArea()
 * - PixelArea is specified as normalized (x, y, width, height) in [0, 1]
 * - Buttons select preset regions: FULL, four quadrants, and a center zoom
 * - A label shows the currently active region name
 * - Press Escape or Back to quit
 */
class ImageViewPixelAreaController : public ConnectionTracker
{
  static constexpr int AREA_COUNT = 6;

  struct AreaEntry
  {
    const char* name;
    Vector4     area; // (x, y, width, height) normalized
  };

  static const AreaEntry AREAS[AREA_COUNT];

public:
  explicit ImageViewPixelAreaController(Application& application)
  : mApplication(application),
    mActiveIndex(0)
  {
    mApplication.InitSignal().Connect(this, &ImageViewPixelAreaController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewPixelAreaController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateImageArea(),
        CreateInfoLabel(),
        CreateButtonRow(),
      });
  }

  View CreateImageArea()
  {
    return ImageView::New(RESOURCES_DIR "gallery-large-3.jpg")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetPixelArea(AREAS[mActiveIndex].area)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .As(mImage);
  }

  View CreateInfoLabel()
  {
    return Label::New(MakeInfoText(mActiveIndex))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(40.0f)
      .SetFontSize(14.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mInfoLabel);
  }

  View CreateButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(100.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < AREA_COUNT; ++i)
    {
      row.Add(CreateAreaButton(i));
    }

    return row;
  }

  View CreateAreaButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mActiveIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(AREAS[index].name)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(13.0f)
                               .SetMultiLine(true)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewPixelAreaController::OnAreaButtonClicked);

    mButtons[index] = button;
    return button;
  }

  void OnAreaButtonClicked(View clickedView, const InputEvent& /*event*/)
  {
    for(int i = 0; i < AREA_COUNT; ++i)
    {
      if(mButtons[i] == clickedView)
      {
        SelectArea(i);
        return;
      }
    }
  }

  void SelectArea(int index)
  {
    mButtons[mActiveIndex].SetBackgroundColor(UiColor(0x333333));
    mActiveIndex = index;
    mButtons[mActiveIndex].SetBackgroundColor(UiColor(0x4A90E2));

    const Vector4& area = AREAS[mActiveIndex].area;
    mImage.SetPixelArea(area);
    mInfoLabel.SetText(MakeInfoText(mActiveIndex));

    DALI_LOG_RELEASE_INFO("[PixelArea] %s -> (x=%.2f y=%.2f w=%.2f h=%.2f)\n",
                          AREAS[mActiveIndex].name,
                          area.x, area.y, area.z, area.w);
  }

  Dali::String MakeInfoText(int index) const
  {
    const Vector4& a = AREAS[index].area;
    char buf[128];
    snprintf(buf, sizeof(buf),
             "PixelArea: x=%.2f  y=%.2f  w=%.2f  h=%.2f",
             a.x, a.y, a.z, a.w);
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
  Application&  mApplication;
  Ui::ImageView mImage;
  Label         mInfoLabel;
  View          mButtons[AREA_COUNT];
  int           mActiveIndex;
};

const ImageViewPixelAreaController::AreaEntry ImageViewPixelAreaController::AREAS[ImageViewPixelAreaController::AREA_COUNT] = {
  {"FULL",         Vector4(0.0f,  0.0f,  1.0f, 1.0f)},
  {"TOP-L",        Vector4(0.0f,  0.0f,  0.5f, 0.5f)},
  {"TOP-R",        Vector4(0.5f,  0.0f,  0.5f, 0.5f)},
  {"BOT-L",        Vector4(0.0f,  0.5f,  0.5f, 0.5f)},
  {"BOT-R",        Vector4(0.5f,  0.5f,  0.5f, 0.5f)},
  {"CENTER\nZOOM", Vector4(0.25f, 0.25f, 0.5f, 0.5f)},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewPixelAreaController controller(application);
  application.MainLoop();
  return 0;
}
