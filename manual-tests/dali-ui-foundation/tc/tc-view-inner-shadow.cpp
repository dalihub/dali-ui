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
constexpr float CARD_WIDTH = 180.0f;
constexpr float CARD_HEIGHT = 120.0f;
constexpr float ROW_GAP = 44.0f;
constexpr float SECTION_GAP = 38.0f;
constexpr float TITLE_FONT = 22.0f;
constexpr float LABEL_FONT = 15.0f;

Label MakeLabel(const Dali::String& text, float fontSize, const UiColor& color)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(MATCH_PARENT);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}
} // namespace

/**
 * @brief Manual check for typed View inner shadows and corner radius tracking.
 */
class TcViewInnerShadow : public ManualTest::TestCase
{
public:
  Dali::String GetName() const override
  {
    return "View Inner Shadow";
  }

  Dali::String GetDescription() const override
  {
    return "Verify symmetric and directional inner shadows with rounded corners";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(PAGE_PADDING, PAGE_PADDING, PAGE_PADDING, PAGE_PADDING));
    root.SetSpacing(SECTION_GAP);

    Label title = MakeLabel("View Inner Shadow", TITLE_FONT, UiColor(0x1A202C));
    title.SetRequestedHeight(WRAP_CONTENT);
    root.Add(title);

    Label description = MakeLabel("Insets control how far the shadow reaches inward from each edge.", LABEL_FONT, UiColor(0x4A5568));
    description.SetRequestedHeight(WRAP_CONTENT);
    root.Add(description);

    StackLayout firstRow = StackLayout::New(StackOrientation::HORIZONTAL);
    firstRow.SetRequestedWidth(MATCH_PARENT);
    firstRow.SetRequestedHeight(WRAP_CONTENT);
    firstRow.SetSpacing(ROW_GAP);
    firstRow.Add(CreateCard("EVEN\n18 each", Insets(18.0f, 18.0f, 18.0f, 18.0f), 10.0f, 18.0f, UiColor(0x000000, 0.55f)));
    firstRow.Add(CreateCard("START + TOP\n32 / 6 / 28 / 6", Insets(32.0f, 6.0f, 28.0f, 6.0f), 8.0f, 18.0f, UiColor(0x2B6CB0, 0.65f)));
    root.Add(firstRow);

    StackLayout secondRow = StackLayout::New(StackOrientation::HORIZONTAL);
    secondRow.SetRequestedWidth(MATCH_PARENT);
    secondRow.SetRequestedHeight(WRAP_CONTENT);
    secondRow.SetSpacing(ROW_GAP);
    secondRow.Add(CreateCard("END + BOTTOM\n4 / 30 / 4 / 26", Insets(4.0f, 30.0f, 4.0f, 26.0f), 8.0f, 28.0f, UiColor(0xC53030, 0.62f)));
    secondRow.Add(CreateAnimatedCornerCard());
    root.Add(secondRow);

    contentArea.Add(root);
    StartCornerRadiusAnimation();
  }

  void OnExit() override
  {
    if(mCornerRadiusAnimation)
    {
      mCornerRadiusAnimation.Stop();
      mCornerRadiusAnimation.Reset();
    }
    mAnimatedCard.Reset();
  }

private:
  View CreateCard(const Dali::String& text, const Insets& insets, float blurRadius, float cornerRadius, const UiColor& shadowColor)
  {
    View card = View::New();
    card.SetRequestedWidth(CARD_WIDTH);
    card.SetRequestedHeight(CARD_HEIGHT);
    card.SetBackgroundColor(UiColor(0xF7FAFC));
    card.SetCornerRadius(cornerRadius);
    card.SetInnerShadow(InnerShadow(insets, blurRadius, shadowColor));
    card.Add(MakeLabel(text, LABEL_FONT, UiColor(0x1A202C)));
    return card;
  }

  View CreateAnimatedCornerCard()
  {
    View card = CreateCard("ANIMATED\ncorner radius", Insets(16.0f, 16.0f, 16.0f, 16.0f), 10.0f, 0.0f, UiColor(0x2F855A, 0.62f));
    mAnimatedCard = card;
    return card;
  }

  void StartCornerRadiusAnimation()
  {
    if(!mAnimatedCard)
    {
      return;
    }

    mCornerRadiusAnimation = Animation::New(1.8f);
    mCornerRadiusAnimation.SetLooping(true);
    mCornerRadiusAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mAnimatedCard.Animate(mCornerRadiusAnimation)
      .CornerRadius(Vector4(42.0f, 42.0f, 42.0f, 42.0f), Duration(1.8f));
    mCornerRadiusAnimation.Play();
  }

  View mAnimatedCard;
  Animation mCornerRadiusAnimation;
};

REGISTER_MANUAL_TEST(TcViewInnerShadow)
