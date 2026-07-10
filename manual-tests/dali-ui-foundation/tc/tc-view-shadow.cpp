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

#include <dali-ui-foundation/public-api/views/effects/shadow-stack.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float PAGE_PADDING = 28.0f;
constexpr float CARD_W       = 160.0f;
constexpr float CARD_H       = 108.0f;
constexpr float ROW_GAP      = 56.0f;
constexpr float SECTION_GAP  = 46.0f;
constexpr float TITLE_FONT   = 22.0f;
constexpr float LABEL_FONT   = 15.0f;

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

/**
 * @brief Manual check for View shadow animation and corner radius tracking.
 */
class TcViewShadow : public ManualTest::TestCase
{
public:
  Dali::String GetName() const override
  {
    return "View Shadow";
  }

  Dali::String GetDescription() const override
  {
    return "Verify first shadow animation and shadow corner radius tracking";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(PAGE_PADDING, PAGE_PADDING, PAGE_PADDING, PAGE_PADDING));
    root.SetSpacing(SECTION_GAP);

    root.Add(MakeLabel("View Shadow", TITLE_FONT, UiColor(0x1A202C)));
    root.Add(MakeLabel("Static shadows, first shadow animation, and multi-shadow corner radius tracking.", LABEL_FONT, UiColor(0x4A5568)));

    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(WRAP_CONTENT);
    row.SetSpacing(ROW_GAP);
    row.Add(CreateCard("ANIMATED\nblur + opacity", true));
    row.Add(CreateCard("STATIC\nshadow", false));
    root.Add(row);

    StackLayout cornerRow = StackLayout::New(StackOrientation::HORIZONTAL);
    cornerRow.SetRequestedWidth(MATCH_PARENT);
    cornerRow.SetRequestedHeight(WRAP_CONTENT);
    cornerRow.SetSpacing(ROW_GAP);
    cornerRow.Add(CreateMultiShadowCard("RADIUS\n0", 0.0f));
    cornerRow.Add(CreateMultiShadowCard("RADIUS\n42", 42.0f));
    root.Add(cornerRow);

    StackLayout animatedCornerRow = StackLayout::New(StackOrientation::HORIZONTAL);
    animatedCornerRow.SetRequestedWidth(MATCH_PARENT);
    animatedCornerRow.SetRequestedHeight(WRAP_CONTENT);
    animatedCornerRow.SetSpacing(ROW_GAP);
    animatedCornerRow.Add(CreateCornerAnimatedCard());
    root.Add(animatedCornerRow);

    contentArea.Add(root);
    StartAnimation();
    StartCornerRadiusAnimation();
  }

  void OnExit() override
  {
    if(mAnimation)
    {
      mAnimation.Stop();
      mAnimation.Reset();
    }
    mAnimatedCard.Reset();
    if(mCornerRadiusAnimation)
    {
      mCornerRadiusAnimation.Stop();
      mCornerRadiusAnimation.Reset();
    }
    mCornerAnimatedCard.Reset();
  }

private:
  View CreateCard(const Dali::String& text, bool animated)
  {
    View card = View::New();
    card.SetRequestedWidth(CARD_W);
    card.SetRequestedHeight(CARD_H);
    card.SetBackgroundColor(UiColor(animated ? 0xEDF2F7 : 0xFFFFFF));
    card.SetCornerRadius(18.0f);
    card.SetPadding(Extents(20.0f, 20.0f, 20.0f, 20.0f));
    card.SetShadow(Shadow(animated ? 0.0f : 18.0f,
                          Vector2(0.0f, 14.0f),
                          UiColor(0.0f, 0.0f, 0.0f, animated ? 0.12f : 0.35f),
                          Vector2(18.0f, 18.0f)));

    Label label = MakeLabel(text, LABEL_FONT, UiColor(0x1A202C));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetRequestedHeight(MATCH_PARENT);
    card.Add(label);

    if(animated)
    {
      mAnimatedCard = card;
    }
    return card;
  }

  View CreateMultiShadowCard(const Dali::String& text, float cornerRadius)
  {
    View card = View::New();
    card.SetRequestedWidth(CARD_W);
    card.SetRequestedHeight(CARD_H);
    card.SetBackgroundColor(UiColor(0xFFFFFF));
    card.SetCornerRadius(cornerRadius);
    card.SetPadding(Extents(20.0f, 20.0f, 20.0f, 20.0f));
    card.SetShadow(ShadowStack{
      Shadow(0.0f, Vector2(-8.0f, 8.0f), UiColor(0x3182CE, 0.45f), Vector2(8.0f, 8.0f)),
      Shadow(2.0f, Vector2(10.0f, 16.0f), UiColor(0xDD6B20, 0.45f), Vector2(10.0f, 10.0f)),
    });

    Label label = MakeLabel(text, LABEL_FONT, UiColor(0x1A202C));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetRequestedHeight(MATCH_PARENT);
    card.Add(label);

    return card;
  }

  View CreateCornerAnimatedCard()
  {
    View card = CreateMultiShadowCard("ANIMATED\nradius", 0.0f);
    card.SetBackgroundColor(UiColor(0xEBF8FF));
    mCornerAnimatedCard = card;
    return card;
  }

  void StartAnimation()
  {
    if(!mAnimatedCard)
    {
      return;
    }

    mAnimation = Animation::New(1.4f);
    mAnimation.SetLooping(true);
    mAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mAnimatedCard.Animate(mAnimation)
      .ShadowBlurRadius(28.0f, Duration(1.4f))
      .ShadowOpacity(0.75f, Duration(1.4f));
    mAnimation.Play();
  }

  void StartCornerRadiusAnimation()
  {
    if(!mCornerAnimatedCard)
    {
      return;
    }

    mCornerRadiusAnimation = Animation::New(1.6f);
    mCornerRadiusAnimation.SetLooping(true);
    mCornerRadiusAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mCornerAnimatedCard.Animate(mCornerRadiusAnimation)
      .CornerRadius(Vector4(42.0f, 42.0f, 42.0f, 42.0f), Duration(1.6f));
    mCornerRadiusAnimation.Play();
  }

  View      mAnimatedCard;
  View      mCornerAnimatedCard;
  Animation mAnimation;
  Animation mCornerRadiusAnimation;
};

REGISTER_MANUAL_TEST(TcViewShadow)
