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

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float CARD_W        = 128.0f;
constexpr float CARD_H        = 128.0f;
constexpr float GAP           = 18.0f;
constexpr float PAGE_PADDING  = 24.0f;
constexpr float TEXT_INSET_H  = 24.0f;
constexpr float TEXT_INSET_V  = 18.0f;
constexpr float TARGET_INSET  = 18.0f;
constexpr float LABEL_FONT    = 13.0f;
constexpr float SUBLABEL_FONT = 12.0f;
constexpr uint32_t COLOR_DISABLED_ENABLED  = 0xB83280;
constexpr uint32_t COLOR_DISABLED_DISABLED = 0x4A5568;

struct CardOptions
{
  bool touchFocusable{true};
};
} // namespace

/**
 * @brief Manual check for pressed/focus-indicated OverlayEffect and primary-target routing.
 */
class TcOverlayEffect : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "38. OverlayEffect";
  }

  Dali::String GetDescription() const override
  {
    return "Compare focus-indicated, press-only, target, round, and disabled overlay behavior";
  }

  void OnEnter(View contentArea) override
  {
    mDisabled = false;

    StackLayout layout = StackLayout::New();
    layout.SetSpacing(GAP);
    layout.SetPadding(Insets(PAGE_PADDING, PAGE_PADDING, PAGE_PADDING, PAGE_PADDING));
    layout.SetRequestedWidth(MATCH_PARENT);
    layout.Add(CreateRow({
      CreateCard("FOCUS INDICATED", UiColor(0x2B6CB0), OverlayEffect::Plain().Configure().SetOverlayColor(UiColor(0x000000, 0.4f)).Build()),
      CreateCard("PRESS ONLY", UiColor(0x2F855A), OverlayEffect::Plain().Configure().SetOverlayColor(UiColor(0x000000, 0.4f)).Build(), {false}),
      CreateTargetCard(),
    }));
    layout.Add(CreateRow({
      CreateCard("ROUND", UiColor(0x805AD5), OverlayEffect::Round()),
      CreateDisabledTargetCard(),
      CreateDisabledToggleCard(),
    }));
    contentArea.Add(layout);
  }

  void OnExit() override
  {
    mDisabledCard.Reset();
    mDisabledStatusLabel.Reset();
    mDisabled = false;
  }

private:
  StackLayout CreateRow(std::initializer_list<View> children)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(GAP);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(WRAP_CONTENT);
    for(auto& child : children)
    {
      row.Add(child);
    }
    return row;
  }

  View CreateTextContent(const Dali::String& text, float fontSize = LABEL_FONT)
  {
    View content = View::New();
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(MATCH_PARENT);
    content.SetLayoutMode(LayoutMode::STANDALONE);
    content.SetPadding(Insets(TEXT_INSET_H, TEXT_INSET_H, TEXT_INSET_V, TEXT_INSET_V));

    Label label = Label::New(text);
    label.SetFontSize(fontSize);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetMultiLine(true);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    content.Add(label);

    return content;
  }

  InteractiveView CreateCard(const Dali::String& text, const UiColor& color, StateEffect effect, CardOptions options = {})
  {
    InteractiveView card = InteractiveView::New();
    card.SetBackgroundColor(color);
    card.SetRequestedWidth(CARD_W);
    card.SetRequestedHeight(CARD_H);
    card.SetFocusOnTouchEnabled(options.touchFocusable);
    card.SetStateEffect(effect);
    card.Add(CreateTextContent(text));
    return card;
  }

  InteractiveView CreateTargetCard()
  {
    InteractiveView card = InteractiveView::New();
    card.SetRequestedWidth(CARD_W);
    card.SetRequestedHeight(CARD_H);
    card.SetBackgroundColor(UiColor(0xAA5568));
    card.SetFocusOnTouchEnabled(false);
    card.SetStateEffect(OverlayEffect::Plain().Configure().SetOverlayColor(UiColor(0x000000, 0.4f)).Build());
    card.SetPadding(Insets(TARGET_INSET, TARGET_INSET, TARGET_INSET, TARGET_INSET));

    Label target = Label::New("STATE EFFECT TARGET");
    target.SetBackgroundColor(UiColor(0xDD6B20));
    target.SetRequestedWidth(MATCH_PARENT);
    target.SetRequestedHeight(MATCH_PARENT);
    target.SetFontSize(SUBLABEL_FONT);
    target.SetTextColor(UiColor(0xFFFFFF));
    target.SetMultiLine(true);

    card.Add(target);
    card.SetStateEffectTarget(target);
    return card;
  }

  InteractiveView CreateDisabledTargetCard()
  {
    mDisabledCard = CreateCard("DISABLED TARGET", UiColor(COLOR_DISABLED_ENABLED), OverlayEffect::Plain().Configure().SetOverlayColor(UiColor(0x000000, 0.4f)).Build());
    return mDisabledCard;
  }

  InteractiveView CreateDisabledToggleCard()
  {
    InteractiveView toggle = InteractiveView::New();
    toggle.SetBackgroundColor(UiColor(0x1A202C));
    toggle.SetRequestedWidth(CARD_W);
    toggle.SetRequestedHeight(CARD_H);
    toggle.SetFocusOnTouchEnabled(false);
    toggle.SetStateEffect(OverlayEffect::Round());

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(MATCH_PARENT);
    content.SetPadding(Insets(TEXT_INSET_H, TEXT_INSET_H, TEXT_INSET_V, TEXT_INSET_V));

    Label titleLabel = Label::New("TOGGLE DISABLED");
    titleLabel.SetFontSize(SUBLABEL_FONT);
    titleLabel.SetTextColor(UiColor(0xFFFFFF));
    titleLabel.SetMultiLine(true);
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(WRAP_CONTENT);

    mDisabledStatusLabel = Label::New("enabled");
    mDisabledStatusLabel.SetFontSize(SUBLABEL_FONT);
    mDisabledStatusLabel.SetTextColor(UiColor(0xFFFFFF));
    mDisabledStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mDisabledStatusLabel.SetRequestedHeight(WRAP_CONTENT);

    content.Add(titleLabel);
    content.Add(ManualTest::MakeWeightedSpacer());
    content.Add(mDisabledStatusLabel);
    toggle.Add(content);

    toggle.ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleDisabledTarget();
    });
    return toggle;
  }

  void ToggleDisabledTarget()
  {
    if(!mDisabledCard)
    {
      return;
    }

    mDisabled = !mDisabled;
    mDisabledCard.SetEnabled(!mDisabled);
    mDisabledCard.SetBackgroundColor(UiColor(mDisabled ? COLOR_DISABLED_DISABLED : COLOR_DISABLED_ENABLED));

    if(mDisabledStatusLabel)
    {
      mDisabledStatusLabel.SetText(mDisabled ? "disabled" : "enabled");
    }
  }

  InteractiveView mDisabledCard;
  Label           mDisabledStatusLabel;
  bool            mDisabled{false};
};

REGISTER_MANUAL_TEST(TcOverlayEffect)
