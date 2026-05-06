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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float FONT_SIZE = 140.0f;
} // namespace

class TextController : public ConnectionTracker
{
public:
  explicit TextController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));
    window.SetSize(Dali::Window::WindowSize(1200, 1080));

    window.Add(StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        // Embossed Style - Light from top-left creates raised effect
        Label::New("Embossed")
          .SetBackgroundColor(UiColor(0x212121))
          .SetPadding(Extents(10, 10, 10, 10))
          .SetFontFamily("SamsungOneUI_700")
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetFontSize(FONT_SIZE)
          .SetRequestedWidth(MATCH_PARENT)
          .SetTextColor(UiColor(0x333333))
          .SetBevel(Text::Bevel()
            .SetDirection(Vector2(-1.0f, -1.0f))
            .SetIntensity(2.0f)
            .SetLightColor(UiColor(0x808080))
            .SetShadowColor(UiColor(0x0D0D0D)))
          .As(mEmbossedLabel),

        // Engraved Style - Light from top-left creates sunken effect
        Label::New("Engraved")
          .SetBackgroundColor(UiColor(0x333333))
          .SetPadding(Extents(10, 10, 10, 10))
          .SetFontFamily("SamsungOneUI_700")
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetFontSize(FONT_SIZE)
          .SetRequestedWidth(MATCH_PARENT)
          .SetTextColor(UiColor(0x212121))
          .SetBevel(Text::Bevel()
            .SetDirection(Vector2(-1.0f, -1.0f))
            .SetIntensity(2.0f)
            .SetLightColor(UiColor(0x0D0D0D))
            .SetShadowColor(UiColor(0x808080)))
          .As(mEngravedLabel),

        // Glass Style - Subtle bevel effect
        Label::New("Glass")
          .SetBackgroundColor(UiColor(0xFFFFFF))
          .SetPadding(Extents(10, 10, 10, 10))
          .SetFontFamily("SamsungOneUI_700")
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetFontSize(FONT_SIZE)
          .SetRequestedWidth(MATCH_PARENT)
          .SetTextColor(UiColor(0xF3F3F3))
          .SetBevel(Text::Bevel()
            .SetDirection(Vector2(-1.0f, -1.0f))
            .SetIntensity(5.0f)
            .SetLightColor(UiColor(0x555555))
            .SetShadowColor(UiColor(0x666666)))
          .As(mGlassLabel),

        // Same Color Style - Text matches background
        Label::New("Same Color")
          .SetBackgroundColor(UiColor(0xEEF3F9))
          .SetPadding(Extents(10, 10, 10, 10))
          .SetFontFamily("SamsungOneUI_700")
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetFontSize(FONT_SIZE)
          .SetRequestedWidth(MATCH_PARENT)
          .SetTextColor(UiColor(0xEEF3F9))
          .SetBevel(Text::Bevel()
            .SetDirection(Vector2(1.0f, -1.0f))
            .SetIntensity(4.0f)
            .SetLightColor(UiColor(0xFFFFFF))
            .SetShadowColor(UiColor(0xD8E2E9)))
          .As(mSameColorLabel),

        // Bronze Style - Decorative warm-toned metallic bevel
        Label::New("Bronze")
          .SetBackgroundColor(UiColor(0x6B4A34))
          .SetPadding(Extents(20, 20, 20, 20))
          .SetFontFamily("SamsungOneUI_700")
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetFontSize(FONT_SIZE)
          .SetRequestedWidth(MATCH_PARENT)
          .SetTextColor(UiColor(0x6B4A34))
          .SetBevel(Text::Bevel()
            .SetDirection(Vector2(-1.0f, -1.0f))
            .SetIntensity(4.0f)
            .SetLightColor(UiColor(0xB8845E))
            .SetShadowColor(UiColor(0x40281C)))
          .As(mBronzeColorLabel),
        }));

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  void OnKeyEvent(KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    // Clears bevel styles with number keys
    if(event.GetKeyName() == "1")
    {
      mEmbossedLabel.ClearBevel();
    }
    else if(event.GetKeyName() == "2")
    {
      mEngravedLabel.ClearBevel();
    }
    else if(event.GetKeyName() == "3")
    {
      mGlassLabel.ClearBevel();
    }
    else if(event.GetKeyName() == "4")
    {
      mSameColorLabel.ClearBevel();
    }
    else if(event.GetKeyName() == "5")
    {
      mBronzeColorLabel.ClearBevel();
    }
  }

private:
  Application& mApplication;
  Label        mEmbossedLabel;
  Label        mEngravedLabel;
  Label        mGlassLabel;
  Label        mSameColorLabel;
  Label        mBronzeColorLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
