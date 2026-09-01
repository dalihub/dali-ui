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
constexpr float PAGE_PADDING = 28.0f;
constexpr float CARD_WIDTH = 220.0f;
constexpr float CARD_HEIGHT = 180.0f;
constexpr float ROW_GAP = 32.0f;
constexpr float SECTION_GAP = 24.0f;
constexpr float TITLE_FONT = 22.0f;
constexpr float BODY_FONT = 15.0f;
constexpr float ANIMATION_DURATION = 1.8f;

Gradient::Linear CreateGradient()
{
  Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);

  Dali::Vector<Gradient::StopNode> stops;
  stops.PushBack(Gradient::StopNode(0.0f, UiColor(0x6D28D9)));
  stops.PushBack(Gradient::StopNode(0.5f, UiColor(0xEC4899)));
  stops.PushBack(Gradient::StopNode(1.0f, UiColor(0xFBBF24)));
  gradient.SetStopNodes(stops);
  return gradient;
}

Label MakeLabel(const Dali::String& text, float fontSize, const UiColor& color)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  return label;
}
} // namespace

class TcViewBackgroundGradientColors : public ManualTest::TestCase
{
public:
  Dali::String GetName() const override
  {
    return "View Background Gradient Colors";
  }

  Dali::String GetDescription() const override
  {
    return "Verify background gradient colors and start offset animation";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(PAGE_PADDING, PAGE_PADDING, PAGE_PADDING, PAGE_PADDING));
    root.SetSpacing(SECTION_GAP);

    root.Add(MakeLabel("View Background Gradient", TITLE_FONT, UiColor(0x1A202C)));
    root.Add(MakeLabel("Compare the fixed gradient colors with the animated start offset.", BODY_FONT, UiColor(0x4A5568)));

    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(CARD_HEIGHT);
    row.SetSpacing(ROW_GAP);
    row.Add(CreateCard("STATIC\ngradient colors", false));
    row.Add(CreateCard("ANIMATED\nstart offset", true));
    root.Add(row);

    contentArea.Add(root);
    StartAnimation();
  }

  void OnExit() override
  {
    if(mAnimation)
    {
      mAnimation.Stop();
      mAnimation.Reset();
    }
    mAnimatedCard.Reset();
  }

private:
  View CreateCard(const Dali::String& text, bool animated)
  {
    View card = View::New();
    card.SetRequestedWidth(CARD_WIDTH);
    card.SetRequestedHeight(CARD_HEIGHT);
    card.SetCornerRadius(20.0f);
    card.SetPadding(Insets(20.0f, 20.0f, 20.0f, 20.0f));
    card.SetBackgroundGradient(CreateGradient());

    Label label = MakeLabel(text, BODY_FONT, UiColor(0xFFFFFF));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetBackgroundColor(UiColor(0x000000, 0.2f));
    card.Add(label);

    if(animated)
    {
      mAnimatedCard = card;
    }
    return card;
  }

  void StartAnimation()
  {
    if(!mAnimatedCard)
    {
      return;
    }

    mAnimation = Animation::New(ANIMATION_DURATION);
    mAnimation.SetLoopCount(Animation::INFINITE_LOOP);
    mAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mAnimatedCard.Animate(mAnimation)
      .BackgroundGradientStartOffset(1.0f, Duration(ANIMATION_DURATION));
    mAnimation.Play();
  }

  View mAnimatedCard;
  Animation mAnimation;
};

REGISTER_MANUAL_TEST(TcViewBackgroundGradientColors)
