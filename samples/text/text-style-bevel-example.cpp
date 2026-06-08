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

Label CreateBevelLabel(const char* text, uint32_t backgroundColor, uint32_t textColor, Text::Bevel bevel, Extents padding = Extents(10, 10, 10, 10))
{
  Label label = Label::New(text);
  label.SetBackgroundColor(UiColor(backgroundColor));
  label.SetPadding(padding);
  label.SetFontFamily("SamsungOneUI_700");
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetFontSize(FONT_SIZE);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetTextColor(UiColor(textColor));
  label.SetBevel(bevel);
  return label;
}
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

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    mEmbossedLabel = CreateBevelLabel("Embossed", 0x212121, 0x333333, Text::Bevel()
                                                                             .SetDirection(Vector2(-1.0f, -1.0f))
                                                                             .SetIntensity(2.0f)
                                                                             .SetLightColor(UiColor(0x808080))
                                                                             .SetShadowColor(UiColor(0x0D0D0D)));
    root.Add(mEmbossedLabel);

    mEngravedLabel = CreateBevelLabel("Engraved", 0x333333, 0x212121, Text::Bevel()
                                                                             .SetDirection(Vector2(-1.0f, -1.0f))
                                                                             .SetIntensity(2.0f)
                                                                             .SetLightColor(UiColor(0x0D0D0D))
                                                                             .SetShadowColor(UiColor(0x808080)));
    root.Add(mEngravedLabel);

    mGlassLabel = CreateBevelLabel("Glass", 0xFFFFFF, 0xF3F3F3, Text::Bevel()
                                                                        .SetDirection(Vector2(-1.0f, -1.0f))
                                                                        .SetIntensity(5.0f)
                                                                        .SetLightColor(UiColor(0x555555))
                                                                        .SetShadowColor(UiColor(0x666666)));
    root.Add(mGlassLabel);

    mSameColorLabel = CreateBevelLabel("Same Color", 0xEEF3F9, 0xEEF3F9, Text::Bevel()
                                                                                 .SetDirection(Vector2(1.0f, -1.0f))
                                                                                 .SetIntensity(4.0f)
                                                                                 .SetLightColor(UiColor(0xFFFFFF))
                                                                                 .SetShadowColor(UiColor(0xD8E2E9)));
    root.Add(mSameColorLabel);

    mBronzeColorLabel = CreateBevelLabel("Bronze", 0x6B4A34, 0x6B4A34, Text::Bevel()
                                                                              .SetDirection(Vector2(-1.0f, -1.0f))
                                                                              .SetIntensity(4.0f)
                                                                              .SetLightColor(UiColor(0xB8845E))
                                                                              .SetShadowColor(UiColor(0x40281C)),
                                         Extents(20, 20, 20, 20));
    root.Add(mBronzeColorLabel);

    window.Add(root);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  void OnKeyEvent(Window window, KeyEvent event)
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
