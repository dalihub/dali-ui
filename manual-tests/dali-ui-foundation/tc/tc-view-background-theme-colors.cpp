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

#include "manual-test-case.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const COLOR_TOKEN = "ManualBackgroundColor";
const char* const GRADIENT_START_TOKEN = "ManualBackgroundGradientStart";
const char* const GRADIENT_END_TOKEN = "ManualBackgroundGradientEnd";

const Vector4 COLOR_A(0.13f, 0.45f, 0.86f, 1.0f);
const Vector4 COLOR_B(0.86f, 0.32f, 0.18f, 1.0f);
const Vector4 GRADIENT_START_A(0.95f, 0.24f, 0.36f, 1.0f);
const Vector4 GRADIENT_START_B(0.16f, 0.75f, 0.48f, 1.0f);
const Vector4 GRADIENT_END_A(0.16f, 0.42f, 0.88f, 1.0f);
const Vector4 GRADIENT_END_B(0.98f, 0.78f, 0.22f, 1.0f);

constexpr uint32_t PAGE_BG = 0xF4F5F7;
constexpr uint32_t TEXT_DARK = 0x20242A;
constexpr uint32_t TEXT_LIGHT = 0xFFFFFF;
constexpr uint32_t BUTTON_BG = 0x1F7A8C;
constexpr float CARD_HEIGHT = 180.0f;
constexpr float BUTTON_H = 56.0f;
constexpr float GAP = 12.0f;
constexpr float FONT_TITLE = 20.0f;
constexpr float FONT_BODY = 14.0f;

bool gUseAlternateColors = false;

bool OverrideManualBackgroundColors(StringView colorId, Vector4& outColor)
{
  if(colorId == COLOR_TOKEN)
  {
    outColor = gUseAlternateColors ? COLOR_B : COLOR_A;
    return true;
  }

  if(colorId == GRADIENT_START_TOKEN)
  {
    outColor = gUseAlternateColors ? GRADIENT_START_B : GRADIENT_START_A;
    return true;
  }

  if(colorId == GRADIENT_END_TOKEN)
  {
    outColor = gUseAlternateColors ? GRADIENT_END_B : GRADIENT_END_A;
    return true;
  }

  return false;
}

Gradient::Linear CreateTokenGradient()
{
  Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.0f, UiColor(GRADIENT_START_TOKEN)));
  stopNodes.PushBack(Gradient::StopNode(1.0f, UiColor(GRADIENT_END_TOKEN)));
  gradient.SetStopNodes(stopNodes);
  return gradient;
}
} // namespace

class TcViewBackgroundThemeColors : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "View Background Theme Colors";
  }

  Dali::String GetDescription() const override
  {
    return "Background color and gradient update from color table changes";
  }

  void OnEnter(View contentArea) override
  {
    gUseAlternateColors = false;
    UiColorManager::Get().SetColorOverride(OverrideManualBackgroundColors);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(PAGE_BG));
    root.SetPadding(Insets(GAP, GAP, GAP, GAP));

    mStatusLabel = MakeLabel("Table A", TEXT_DARK, FONT_TITLE, 48.0f);

    StackLayout cardRow = StackLayout::New(StackOrientation::HORIZONTAL);
    cardRow.SetRequestedWidth(MATCH_PARENT);
    cardRow.SetRequestedHeight(CARD_HEIGHT);
    cardRow.SetPadding(Insets(0.0f, 0.0f, 0.0f, GAP));
    cardRow.Add(MakeColorCard());
    cardRow.Add(MakeGradientCard());

    root.Add(mStatusLabel);
    root.Add(cardRow);
    root.Add(MakeToggleButton());
    contentArea.Add(root);
  }

  void OnExit() override
  {
    UiColorManager::Get().ClearColorOverride();
    gUseAlternateColors = false;
    mStatusLabel.Reset();
  }

private:
  Label MakeLabel(const Dali::String& text, uint32_t color, float fontSize, float height)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(height);
    label.SetFontSize(fontSize);
    label.SetTextColor(UiColor(color));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetMultiLine(true);
    return label;
  }

  View MakeColorCard()
  {
    StackLayout card = MakeCard("SetBackgroundColor(UiColor token)");
    card.SetBackgroundColor(UiColor(COLOR_TOKEN));
    return card;
  }

  View MakeGradientCard()
  {
    StackLayout card = MakeCard("SetBackgroundGradient(token stops)");
    card.SetBackgroundGradient(CreateTokenGradient());
    return card;
  }

  StackLayout MakeCard(const Dali::String& text)
  {
    StackLayout card = StackLayout::New(StackOrientation::VERTICAL);
    card.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    card.SetRequestedHeight(MATCH_PARENT);
    card.SetPadding(Insets(GAP, GAP, GAP, GAP));
    card.SetMargin(Insets(GAP / 2.0f, GAP / 2.0f, 0.0f, 0.0f));

    Label label = MakeLabel(text, TEXT_LIGHT, FONT_BODY, MATCH_PARENT);
    label.SetBackgroundColor(UiColor(0x000000, 0.25f));
    card.Add(label);
    return card;
  }

  View MakeToggleButton()
  {
    Label button = MakeLabel("Toggle Color Table", TEXT_LIGHT, FONT_TITLE, BUTTON_H);
    button.SetBackgroundColor(UiColor(BUTTON_BG));
    button.SetFocusable(true);
    button.SetStateEffect(OverlayEffect::ListItem());
    InteractiveTrait interactive = button.AsInteractive();
    interactive.ClickedSignal().Connect(this, [this](View, InputEvent) -> bool
    {
      ToggleColors();
      return true;
    });
    return button;
  }

  void ToggleColors()
  {
    gUseAlternateColors = !gUseAlternateColors;
    UiColorManager::Get().SetColorOverride(OverrideManualBackgroundColors);
    mStatusLabel.SetText(gUseAlternateColors ? "Table B" : "Table A");
  }

  Label mStatusLabel;
};

REGISTER_MANUAL_TEST(TcViewBackgroundThemeColors)
