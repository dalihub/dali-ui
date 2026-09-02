/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/extension-api/view.h>

#include <array>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float CARD_WIDTH  = 250.0f;
constexpr float CARD_HEIGHT = 125.0f;

enum DemoIndex : std::size_t
{
  POSITION_2D,
  SIZE_2D,
  ORIENTATION_2D,
  POSITION_3D,
  SIZE_3D,
  ORIENTATION_3D,
  DEMO_COUNT
};
} // unnamed namespace

class TransformExample : public ConnectionTracker
{
public:
  explicit TransformExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TransformExample::Create);
  }

private:
  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x101826));
    window.KeyEventSignal().Connect(this, &TransformExample::OnKeyEvent);

    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    Label title = MakeLabel("2D / 3D Position, Size, Orientation", 24.0f, UiColor(0xFFFFFF));
    title.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 15.0f, 820.0f, 45.0f)));
    root.Add(title);

    AddCard(root, "2D Position\nSetPositionX / Y", POSITION_2D, 30.0f, 75.0f, UiColor(0x1565C0));
    AddCard(root, "2D Size\nSetSizeWidth / Height", SIZE_2D, 300.0f, 75.0f, UiColor(0x00897B));
    AddCard(root, "2D Orientation\nSetOrientationAngle, Z axis", ORIENTATION_2D, 570.0f, 75.0f, UiColor(0x6A1B9A));

    AddCard(root, "3D Position\nSetPositionX / Y / Z", POSITION_3D, 30.0f, 230.0f, UiColor(0xEF6C00));
    AddCard(root, "3D Size\nSetSizeWidth / Height / Depth", SIZE_3D, 300.0f, 230.0f, UiColor(0x2E7D32));
    AddCard(root, "3D Orientation\nQuaternion axis + angle", ORIENTATION_3D, 570.0f, 230.0f, UiColor(0xC62828));

    mStatus = MakeLabel("Click a card to toggle its transform. Escape/Back quits.", 17.0f, UiColor(0xCFD8DC));
    mStatus.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 390.0f, 790.0f, 70.0f)));
    root.Add(mStatus);

    window.Add(root);
  }

  Label MakeLabel(const char* text, float fontSize, UiColor color)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(fontSize);
    label.SetTextColor(color);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  void AddCard(AbsoluteLayout root, const char* text, DemoIndex index, float x, float y, UiColor color)
  {
    AbsoluteLayout card = AbsoluteLayout::New();
    card.SetBackgroundColor(color);
    card.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, CARD_WIDTH, CARD_HEIGHT)));

    Label label = MakeLabel(text, 16.0f, UiColor(0xFFFFFF));
    label.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, 0.0f, CARD_WIDTH, CARD_HEIGHT)));
    card.Add(label);

    card.AsInteractive().ClickedSignal().Connect(this, [this, card, index](View, InputEvent) -> bool {
      Toggle(card, index);
      return true;
    });

    root.Add(card);
  }

  void Toggle(View card, DemoIndex index)
  {
    const bool enabled = !mEnabled[index];
    mEnabled[index]    = enabled;

    switch(index)
    {
      case POSITION_2D:
      {
        Extension::View::SetPositionX(card, enabled ? 55.0f : 30.0f);
        Extension::View::SetPositionY(card, enabled ? 95.0f : 75.0f);
        mStatus.SetText(enabled ? "2D Position = (55, 95)" : "2D Position = (30, 75)");
        break;
      }
      case SIZE_2D:
      {
        Extension::View::SetSizeWidth(card, enabled ? 285.0f : CARD_WIDTH);
        Extension::View::SetSizeHeight(card, enabled ? 145.0f : CARD_HEIGHT);
        mStatus.SetText(enabled ? "2D Size = (285, 145)" : "2D Size = (250, 125)");
        break;
      }
      case ORIENTATION_2D:
      {
        card.SetOrientationAngle(Degree(enabled ? 25.0f : 0.0f));
        mStatus.SetText(enabled ? "2D Orientation = 25 degrees around default Z axis" : "2D Orientation = 0 degrees");
        break;
      }
      case POSITION_3D:
      {
        Extension::View::SetPositionX(card, enabled ? 55.0f : 30.0f);
        Extension::View::SetPositionY(card, enabled ? 245.0f : 230.0f);
        Extension::View::SetPositionZ(card, enabled ? 80.0f : 0.0f);
        mStatus.SetText(enabled ? "3D Position = (55, 245, 80)" : "3D Position = (30, 230, 0)");
        break;
      }
      case SIZE_3D:
      {
        Extension::View::SetSizeWidth(card, enabled ? 280.0f : CARD_WIDTH);
        Extension::View::SetSizeHeight(card, enabled ? 145.0f : CARD_HEIGHT);
        Extension::View::SetSizeDepth(card, enabled ? 100.0f : 0.0f);
        mStatus.SetText(enabled ? "3D Size = (280, 145, 100)" : "3D Size = (250, 125, 0)");
        break;
      }
      case ORIENTATION_3D:
      {
        if(!mThreeDimensionalAxisInitialized)
        {
          Vector3 axis(1.0f, 1.0f, 0.35f);
          axis.Normalize();
          card.SetOrientation(Quaternion(Degree(5.0f), axis));
          mThreeDimensionalAxisInitialized = true;
        }
        card.SetOrientationAngle(Degree(enabled ? 55.0f : 5.0f));
        mStatus.SetText(enabled ? "3D Orientation = 55 degrees; existing XYZ axis preserved"
                                : "3D Orientation = 5 degrees; existing XYZ axis preserved");
        break;
      }
      default:
        break;
    }
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN &&
       (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK)))
    {
      mApplication.Quit();
    }
  }

private:
  Application&                 mApplication;
  Label                        mStatus;
  std::array<bool, DEMO_COUNT> mEnabled{};
  bool                         mThreeDimensionalAxisInitialized{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TransformExample example(application);
  application.MainLoop();
  return 0;
}
