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

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/public-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/timer.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float MAX_SCENE_WIDTH  = 1280.0f;
constexpr float MAX_SCENE_HEIGHT = 720.0f;

constexpr float SCENE_HORIZONTAL_PADDING = 32.0f;
constexpr float SCENE_VERTICAL_PADDING   = 16.0f;
constexpr float CARD_MINIMUM_WIDTH       = 280.0f;
constexpr float CARD_MINIMUM_HEIGHT      = 379.0f;
constexpr float CARD_HORIZONTAL_MARGIN   = 8.0f;
constexpr float CARD_VERTICAL_MARGIN     = 4.0f;

constexpr uint32_t BACKGROUND_COLOR       = 0x0A0F1C;
constexpr uint32_t BACKGROUND_DEEP_COLOR  = 0x0E1628;
constexpr uint32_t CARD_COLOR             = 0x141D30;
constexpr uint32_t CARD_SELECTED_COLOR    = 0x18243A;
constexpr uint32_t CARD_LINE_COLOR        = 0x273753;
constexpr uint32_t PRIMARY_TEXT_COLOR     = 0xF5F7FB;
constexpr uint32_t SECONDARY_TEXT_COLOR   = 0x9DA9BC;
constexpr uint32_t MUTED_TEXT_COLOR       = 0x6F7E94;
constexpr uint32_t BLUE_COLOR             = 0x3D7BFF;
constexpr uint32_t CYAN_COLOR             = 0x52E5FF;
constexpr uint32_t PURPLE_COLOR           = 0x8E6CFF;
constexpr uint32_t SUCCESS_COLOR          = 0x65E6A5;

constexpr float SCENE_FADE_OUT_SECONDS       = 0.22f;
constexpr float SCENE_FADE_IN_SECONDS        = 0.30f;
constexpr float CARD_ENTRANCE_SECONDS        = 0.36f;
constexpr float AFFORDANCE_ENTRANCE_SECONDS  = 0.32f;
constexpr float MARKDOWN_ENTRANCE_SECONDS    = 0.36f;
constexpr float INTRO_GRADIENT_SECONDS       = 2.80f;
constexpr float SHIMMER_DURATION_SECONDS    = 1.40f;
constexpr float HERO_GRADIENT_SECONDS       = 6.00f;
constexpr float HIGHLIGHT_SWEEP_SECONDS     = 0.85f;
constexpr float REVEAL_SEQUENCE_SECONDS     = 2.65f;
constexpr float COMPLETE_REVEAL_SECONDS     = 0.60f;
constexpr float STATUS_FADE_OUT_SECONDS      = 0.28f;
constexpr float STATUS_FADE_IN_SECONDS       = 0.52f;
constexpr float DETAIL_STATUS_REVEAL_SECONDS = 1.00f;
constexpr uint32_t GENERATING_INTERVAL_MS = 1300u;

constexpr std::array<const char*, 4u> GENERATING_STATUS{{
  "Understanding your travel style...",
  "Finding quiet places away from the crowd...",
  "Balancing travel time and experiences...",
  "Building your personalized itinerary...",
}};

Label NewLabel(const char* text, float fontSize, uint32_t color, const char* fontFamily = "SamsungOneUI_400")
{
  Label label = Label::New(text);
  label.SetAsyncRendering(false);
  label.SetFontFamily(fontFamily);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  return label;
}

Label NewCenteredLabel(const char* text, float fontSize, uint32_t color, const char* fontFamily = "SamsungOneUI_400")
{
  Label label = NewLabel(text, fontSize, color, fontFamily);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

Label NewActionButton(const char* text, float width, uint32_t backgroundColor = BLUE_COLOR)
{
  Label button = NewCenteredLabel(text, 17.0f, PRIMARY_TEXT_COLOR, "SamsungOneUI_700");
  button.SetRequestedWidth(width);
  button.SetRequestedHeight(54.0f);
  button.SetMultiLine(false);
  button.SetBackgroundColor(UiColor(backgroundColor));
  button.SetBorderlineWidth(1.0f);
  button.SetBorderlineOffset(-1.0f);
  button.SetBorderlineColor(UiColor(CYAN_COLOR));
  button.SetCornerRadius(12.0f);
  button.SetFocusable(true);
  button.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
  return button;
}

StackLayout NewVertical(float spacing = 0.0f)
{
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(spacing);
  return layout;
}

StackLayout NewHorizontal(float spacing = 0.0f)
{
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(spacing);
  return layout;
}

FlexLayout NewWrappingRow()
{
  FlexLayout layout = FlexLayout::New();
  layout.SetDirection(FlexDirection::ROW);
  layout.SetWrap(FlexWrap::WRAP);
  layout.SetJustifyContent(FlexJustify::FLEX_START);
  layout.SetAlignItems(FlexAlign::STRETCH);
  layout.SetAlignContent(FlexAlign::FLEX_START);
  layout.SetRequestedWidth(MATCH_PARENT);
  layout.SetRequestedHeight(WRAP_CONTENT);
  return layout;
}

View NewVerticalSpacer(float height)
{
  View spacer = View::New();
  spacer.SetRequestedHeight(height);
  return spacer;
}

View NewHorizontalSpacer(float width)
{
  View spacer = View::New();
  spacer.SetRequestedWidth(width);
  return spacer;
}

View NewWeightedSpacer()
{
  View spacer = View::New();
  spacer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  return spacer;
}

View NewDivider()
{
  View divider = View::New();
  divider.SetRequestedHeight(1.0f);
  divider.SetBackgroundColor(UiColor(CARD_LINE_COLOR));
  return divider;
}

Gradient::Linear NewJejuGradient(float startOffset = 0.0f)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  gradient.SetStartOffset(startOffset);
  gradient.SetStopNodes({
    Gradient::StopNode(0.00f, UiColor(BLUE_COLOR)),
    Gradient::StopNode(0.45f, UiColor(CYAN_COLOR)),
    Gradient::StopNode(1.00f, UiColor(PURPLE_COLOR)),
  });
  return gradient;
}

Gradient::Linear NewActionBlueGradient()
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::PAD);
  gradient.SetStopNodes({
    Gradient::StopNode(0.00f, UiColor(0x4388FF)),
    Gradient::StopNode(0.46f, UiColor(0x52E5FF)),
    Gradient::StopNode(0.72f, UiColor(0x8FC8FF)),
    Gradient::StopNode(1.00f, UiColor(0x5B9CFF)),
  });
  return gradient;
}

Gradient::Linear NewSeaSunsetGradient(float startOffset)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  gradient.SetStartOffset(startOffset);
  gradient.SetStopNodes({
    Gradient::StopNode(0.00f, UiColor(0x3278FF)),
    Gradient::StopNode(0.25f, UiColor(0x45D8FF)),
    Gradient::StopNode(0.52f, UiColor(0x8B72FF)),
    Gradient::StopNode(0.76f, UiColor(0xFF7B87)),
    Gradient::StopNode(1.00f, UiColor(0xFFAD58)),
  });
  return gradient;
}

Gradient::Linear NewShimmerOverlay(float startOffset, bool warmHighlight = false)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::PAD);
  gradient.SetStartOffset(startOffset);

  const UiColor transparent(1.0f, 1.0f, 1.0f, 0.0f);
  const UiColor cyanHighlight(0.75f, 0.97f, 1.0f, 0.86f);
  const UiColor whiteHighlight(1.0f, 1.0f, 1.0f, 1.0f);
  const UiColor warm(1.0f, 0.48f, 0.65f, 0.86f);
  gradient.SetStopNodes({
    Gradient::StopNode(0.00f, transparent),
    Gradient::StopNode(0.35f, transparent),
    Gradient::StopNode(0.48f, cyanHighlight),
    Gradient::StopNode(0.52f, whiteHighlight),
    Gradient::StopNode(0.56f, warmHighlight ? warm : cyanHighlight),
    Gradient::StopNode(0.70f, transparent),
    Gradient::StopNode(1.00f, transparent),
  });
  return gradient;
}

void ConfigureReveal(Label label, Text::Reveal::Unit unit, float fadeDurationRatio, float progress)
{
  Text::Reveal reveal;
  reveal.SetUnit(unit);
  reveal.SetFadeDurationRatio(fadeDurationRatio);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(progress);
}

MarkdownViewStyle NewTravelMarkdownStyle()
{
  return MarkdownViewStyle::Builder()
    .SetTextFontFamily("SamsungOneUI_400")
    .SetHeadingFontFamily("SamsungOneUI_700")
    .SetCodeFontFamily("SamsungOneUI_400")
    .SetTextFontSize(16.0f)
    .SetHeading1FontSize(29.0f)
    .SetHeading2FontSize(23.0f)
    .SetHeading3FontSize(19.0f)
    .SetHeading4FontSize(17.0f)
    .SetHeading5FontSize(15.0f)
    .SetHeading6FontSize(14.0f)
    .SetCodeBlockFontSize(16.0f)
    .SetCodeBlockTitleFontSize(14.0f)
    .SetTextColor(UiColor(PRIMARY_TEXT_COLOR))
    .SetHeadingTextColor(UiColor(PRIMARY_TEXT_COLOR))
    .SetQuoteTextColor(UiColor(SECONDARY_TEXT_COLOR))
    .SetCodeTextColor(UiColor(PRIMARY_TEXT_COLOR))
    .SetCodeBlockTitleTextColor(UiColor(PRIMARY_TEXT_COLOR))
    .SetInlineCodeBackgroundColor(UiColor(BACKGROUND_DEEP_COLOR))
    .SetCodeBlockBackgroundColor(UiColor(BACKGROUND_DEEP_COLOR))
    .SetCodeBlockTitleBackgroundColor(UiColor(CARD_SELECTED_COLOR))
    .SetQuoteBarColor(UiColor(CYAN_COLOR))
    .SetThematicBreakColor(UiColor(CARD_LINE_COLOR))
    .SetTableRuleColor(UiColor(CARD_LINE_COLOR))
    .SetTaskCheckBoxIconColor(UiColor(SECONDARY_TEXT_COLOR))
    .SetTaskCheckBoxSelectedIconColor(UiColor(SUCCESS_COLOR))
    .Build();
}

LayoutTransition NewCardEntranceTransition()
{
  const LayoutTransitionTiming timing{Duration(CARD_ENTRANCE_SECONDS),
                                      AlphaFunction(AlphaFunction::EASE_OUT),
                                      Duration(0.06f)};

  ViewAnimationSpec enterSpec = ViewAnimationSpec::New();
  enterSpec.Opacity(1.0f,
                    Duration(CARD_ENTRANCE_SECONDS),
                    AlphaFunction(AlphaFunction::EASE_OUT),
                    Duration(0.06f));

  LayoutTransition transition = LayoutTransition::New();
  transition.SetEnterVisualSpec(enterSpec)
    .SetEnterBoundsEffect(LayoutBoundsEffects::SlideFrom(LayoutBoundsEdge::BOTTOM,
                                                         LayoutBoundsLength::Pixel(20.0f),
                                                         timing))
    .ClearChangeTiming()
    .SetEnterOnInitialMount(true);
  return transition;
}

LayoutTransition NewAffordanceTransition(LayoutBoundsEdge edge)
{
  const LayoutTransitionTiming timing{Duration(AFFORDANCE_ENTRANCE_SECONDS),
                                      AlphaFunction(AlphaFunction::EASE_OUT),
                                      Duration()};

  ViewAnimationSpec enterSpec = ViewAnimationSpec::New();
  enterSpec.Opacity(1.0f,
                    Duration(AFFORDANCE_ENTRANCE_SECONDS),
                    AlphaFunction(AlphaFunction::EASE_OUT));

  LayoutTransition transition = LayoutTransition::New();
  transition.SetEnterVisualSpec(enterSpec)
    .SetEnterBoundsEffect(LayoutBoundsEffects::SlideFrom(edge,
                                                         LayoutBoundsLength::Pixel(16.0f),
                                                         timing))
    .ClearChangeTiming()
    .SetChangeTiming(LayoutChangeCause::SIBLING_ADDED, timing);
  return transition;
}

LayoutTransition NewMarkdownEntranceTransition()
{
  const LayoutTransitionTiming timing{Duration(MARKDOWN_ENTRANCE_SECONDS),
                                      AlphaFunction(AlphaFunction::EASE_OUT),
                                      Duration()};

  ViewAnimationSpec enterSpec = ViewAnimationSpec::New();
  enterSpec.Opacity(1.0f,
                    Duration(MARKDOWN_ENTRANCE_SECONDS),
                    AlphaFunction(AlphaFunction::EASE_OUT));

  LayoutTransition transition = LayoutTransition::New();
  transition.SetEnterVisualSpec(enterSpec)
    .SetEnterBoundsEffect(LayoutBoundsEffects::SlideFrom(LayoutBoundsEdge::BOTTOM,
                                                         LayoutBoundsLength::Pixel(14.0f),
                                                         timing))
    .ClearChangeTiming();
  return transition;
}

} // namespace

class TextEffectDemo : public ConnectionTracker
{
public:
  explicit TextEffectDemo(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextEffectDemo::OnInit);
  }

  ~TextEffectDemo()
  {
    ++mLifecycleToken;
    StopSceneActivity();
    RemoveCurrentSceneImmediately();
    if(mSceneRoot)
    {
      mSceneRoot.SetLayoutTransition(LayoutTransition());
      mSceneRoot.Unparent();
    }
  }

private:
  enum class DemoState
  {
    INTRO,
    GENERATING,
    REVEAL_RESULTS,
    RESULTS_READY,
    DETAIL_STREAMING,
    DETAIL_READY
  };

  struct ItineraryCard
  {
    StackLayout root;
    StackLayout eyebrowRow;
    Label       day;
    Label       badge;
    Label       title;
    Label       places;
    Label       subtitle;
    Label       action;
  };

  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(UiColor(BACKGROUND_COLOR));
    mWindow.KeyEventSignal().Connect(this, &TextEffectDemo::OnKeyEvent);
    mWindow.ResizedSignal().Connect(this, &TextEffectDemo::OnWindowResized);

    mSceneRoot = NewSceneViewport();
    ConfigureSceneTransition();
    mWindow.Add(mSceneRoot);
    ResetToState(DemoState::INTRO);
  }

  StackLayout NewSceneRoot(float spacing = 0.0f) const
  {
    StackLayout root = NewVertical(spacing);
    root.SetBackgroundColor(UiColor(BACKGROUND_COLOR));
    return root;
  }

  FlexLayout NewSceneViewport() const
  {
    FlexLayout viewport = FlexLayout::New();
    viewport.SetDirection(FlexDirection::ROW);
    viewport.SetJustifyContent(FlexJustify::CENTER);
    viewport.SetAlignItems(FlexAlign::CENTER);
    viewport.SetRequestedWidth(MATCH_PARENT);
    viewport.SetRequestedHeight(MATCH_PARENT);
    return viewport;
  }

  void ConfigureSceneTransition()
  {
    ViewAnimationSpec enterSpec = ViewAnimationSpec::New();
    enterSpec.Opacity(1.0f, Duration(SCENE_FADE_IN_SECONDS), AlphaFunction(AlphaFunction::EASE_OUT));

    ViewAnimationSpec exitSpec = ViewAnimationSpec::New();
    exitSpec.Opacity(0.0f, Duration(SCENE_FADE_OUT_SECONDS), AlphaFunction(AlphaFunction::EASE_IN));

    LayoutTransition transition = LayoutTransition::New();
    // Scene changes only animate visual lifetime. Bounds continue to follow the
    // layout immediately, so the default CHANGE transition is disabled.
    transition.SetEnterVisualSpec(enterSpec)
      .SetExitVisualSpec(exitSpec)
      .ClearChangeTiming()
      .SetEnterOnInitialMount(true)
      .SetOnFinished(LayoutLifecycleCallback::New(this, &TextEffectDemo::OnSceneTransitionFinished));
    mSceneRoot.SetLayoutTransition(transition);
  }

  View BuildIntroScene()
  {
    StackLayout root = NewSceneRoot();
    root.SetPadding(Extents(74, 74, 34, 34));

    Label brand = NewLabel("DALI UI AI TRAVEL CONCIERGE", 15.0f, SECONDARY_TEXT_COLOR, "SamsungOneUI_700");
    brand.SetRequestedHeight(30.0f);
    root.Add(brand);
    root.Add(NewWeightedSpacer());

    Label hero = NewCenteredLabel("Plan your next\nescape", 53.0f, PRIMARY_TEXT_COLOR, "SamsungOneUI_700");
    hero.SetTextFit(Text::Fit::Range(40.0f, 52.0f, 1.0f));
    hero.SetRequestedHeight(126.0f);
    hero.SetMultiLine(true);
    hero.SetLineHeight(1.04f);
    root.Add(hero);

    mIntroJeju = NewCenteredLabel("JEJU", 72.0f, PRIMARY_TEXT_COLOR, "SamsungOneUI_700");
    mIntroJeju.SetRequestedHeight(94.0f);
    mIntroJeju.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
    mIntroJeju.SetTextGradient(NewJejuGradient(-0.24f));
    root.Add(mIntroJeju);

    Label conditions = NewCenteredLabel("3 DAYS  ·  NATURE  ·  RELAX", 17.0f, SECONDARY_TEXT_COLOR, "SamsungOneUI_500");
    conditions.SetRequestedHeight(44.0f);
    root.Add(conditions);
    root.Add(NewVerticalSpacer(20.0f));

    Label primaryButton = NewActionButton("Create My Trip  →", 272.0f);
    primaryButton.SetFocusable(false);
    primaryButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      StartGenerating();
    });
    root.Add(primaryButton);

    Label footnote = NewCenteredLabel("Personalized itinerary powered by AI", 14.0f, MUTED_TEXT_COLOR);
    footnote.SetRequestedHeight(56.0f);
    root.Add(footnote);
    root.Add(NewWeightedSpacer());

    return root;
  }

  StackLayout NewSkeletonCard(const char* dayText)
  {
    StackLayout card = NewVertical(18.0f);
    card.SetRequestedWidth(MATCH_PARENT);
    card.SetMinimumWidth(CARD_MINIMUM_WIDTH);
    card.SetMinimumHeight(CARD_MINIMUM_HEIGHT);
    card.SetMargin(Extents(CARD_HORIZONTAL_MARGIN,
                           CARD_HORIZONTAL_MARGIN,
                           CARD_VERTICAL_MARGIN,
                           CARD_VERTICAL_MARGIN));
    card.SetPadding(Extents(26, 26, 25, 25));
    card.SetBackgroundColor(UiColor(CARD_COLOR));
    card.SetCornerRadius(18.0f);
    card.SetBorderlineWidth(1.0f);
    card.SetBorderlineOffset(-1.0f);
    card.SetBorderlineColor(UiColor(CARD_LINE_COLOR));
    card.SetLayoutParams(FlexLayoutParams::New()
                           .SetFlexBasis(CARD_MINIMUM_WIDTH)
                           .SetFlexGrow(1.0f)
                           .SetFlexShrink(0.0f));

    Label day = NewLabel(dayText, 14.0f, MUTED_TEXT_COLOR, "SamsungOneUI_700");
    day.SetRequestedHeight(24.0f);
    card.Add(day);
    card.Add(NewVerticalSpacer(8.0f));

    View line1 = View::New();
    line1.SetRequestedWidth(150.0f);
    line1.SetRequestedHeight(12.0f);
    line1.SetBackgroundColor(UiColor(CARD_LINE_COLOR));
    line1.SetCornerRadius(6.0f);
    line1.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    card.Add(line1);

    View line2 = View::New();
    line2.SetRequestedWidth(MATCH_PARENT);
    line2.SetRequestedHeight(11.0f);
    line2.SetBackgroundColor(UiColor(CARD_LINE_COLOR));
    line2.SetOpacity(0.72f);
    line2.SetCornerRadius(6.0f);
    line2.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    card.Add(line2);

    View line3 = View::New();
    line3.SetRequestedWidth(205.0f);
    line3.SetRequestedHeight(11.0f);
    line3.SetBackgroundColor(UiColor(CARD_LINE_COLOR));
    line3.SetOpacity(0.52f);
    line3.SetCornerRadius(6.0f);
    line3.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    card.Add(line3);
    card.Add(NewWeightedSpacer());
    return card;
  }

  View BuildGeneratingCards()
  {
    FlexLayout cards = NewWrappingRow();
    cards.Add(NewSkeletonCard("DAY 1"));
    cards.Add(NewSkeletonCard("DAY 2"));
    cards.Add(NewSkeletonCard("DAY 3"));

    ScrollView scroll = ScrollView::New();
    scroll.SetScrollDirection(ScrollDirection::Vertical);
    scroll.SetRequestedWidth(MATCH_PARENT);
    scroll.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    scroll.SetContent(cards);
    return scroll;
  }

  View BuildGeneratingScene()
  {
    StackLayout root = NewSceneRoot(0.0f);
    root.SetPadding(Extents(SCENE_HORIZONTAL_PADDING,
                            SCENE_HORIZONTAL_PADDING,
                            SCENE_VERTICAL_PADDING,
                            SCENE_VERTICAL_PADDING));

    StackLayout header = NewHorizontal();
    header.SetRequestedWidth(MATCH_PARENT);
    header.SetRequestedHeight(32.0f);
    Label brand = NewLabel("DALI UI AI TRAVEL CONCIERGE", 14.0f, SECONDARY_TEXT_COLOR, "SamsungOneUI_700");
    brand.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    Label trip = NewLabel("JEJU  ·  3 DAYS", 14.0f, MUTED_TEXT_COLOR, "SamsungOneUI_500");
    trip.SetHorizontalTextAlignment(Text::Alignment::END);
    header.Add(brand);
    header.Add(trip);
    root.Add(header);
    root.Add(NewVerticalSpacer(16.0f));

    mGeneratingTitle = NewCenteredLabel("Creating your perfect Jeju escape", 38.0f, SECONDARY_TEXT_COLOR, "SamsungOneUI_700");
    mGeneratingTitle.SetRequestedHeight(58.0f);
    root.Add(mGeneratingTitle);

    mGeneratingStatus = NewCenteredLabel(GENERATING_STATUS[0], 18.0f, SECONDARY_TEXT_COLOR);
    mGeneratingStatus.SetRequestedHeight(46.0f);
    root.Add(mGeneratingStatus);
    root.Add(NewVerticalSpacer(16.0f));
    root.Add(BuildGeneratingCards());

    return root;
  }

  ItineraryCard NewItineraryCard(std::size_t index, bool revealImmediately)
  {
    static constexpr std::array<const char*, 3u> DAY{{"DAY 1", "DAY 2", "DAY 3"}};
    static constexpr std::array<const char*, 3u> TITLE{{"Forest & Oreum", "Sea & Sunset", "Market & Old Town"}};
    static constexpr std::array<const char*, 3u> PLACES{{
      "Bijarim Forest · Abu Oreum · Local Cafe",
      "Woljeongri · Sehwa · Hamdeok",
      "Dongmun Market · Old Jeju · Local Dessert",
    }};
    static constexpr std::array<const char*, 3u> SUBTITLE{{
      "Walk beneath ancient cedars, climb a quiet oreum, and end with coffee among Jeju's green landscapes.",
      "Follow the eastern coastline slowly, leaving time for ocean views, village walks, and an unhurried sunset.",
      "Browse the morning market, discover old alleyways, and finish the trip with Jeju's local flavors.",
    }};

    ItineraryCard card;
    card.root = NewVertical(12.0f);
    card.root.SetRequestedWidth(MATCH_PARENT);
    card.root.SetMinimumWidth(CARD_MINIMUM_WIDTH);
    card.root.SetMinimumHeight(CARD_MINIMUM_HEIGHT);
    card.root.SetMargin(Extents(CARD_HORIZONTAL_MARGIN,
                                CARD_HORIZONTAL_MARGIN,
                                CARD_VERTICAL_MARGIN,
                                CARD_VERTICAL_MARGIN));
    card.root.SetPadding(Extents(25, 25, 22, 22));
    card.root.SetBackgroundColor(UiColor(index == 1u ? CARD_SELECTED_COLOR : CARD_COLOR));
    card.root.SetCornerRadius(18.0f);
    card.root.SetBorderlineWidth(index == 1u ? 1.5f : 1.0f);
    card.root.SetBorderlineOffset(-1.0f);
    card.root.SetBorderlineColor(UiColor(index == 1u ? BLUE_COLOR : CARD_LINE_COLOR));
    card.root.SetLayoutParams(FlexLayoutParams::New()
                                .SetFlexBasis(CARD_MINIMUM_WIDTH)
                                .SetFlexGrow(1.0f)
                                .SetFlexShrink(0.0f));

    card.eyebrowRow = NewHorizontal(8.0f);
    card.eyebrowRow.SetRequestedWidth(MATCH_PARENT);
    card.eyebrowRow.SetRequestedHeight(30.0f);
    card.day = NewLabel(DAY[index], 14.0f, index == 1u ? CYAN_COLOR : SECONDARY_TEXT_COLOR, "SamsungOneUI_700");
    card.day.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    card.eyebrowRow.Add(card.day);
    card.root.Add(card.eyebrowRow);

    if(index == 1u)
    {
      card.badge = NewCenteredLabel("BEST MATCH", 12.0f, PRIMARY_TEXT_COLOR, "SamsungOneUI_700");
      card.badge.SetRequestedWidth(102.0f);
      card.badge.SetBackgroundColor(UiColor(PURPLE_COLOR));
      card.badge.SetCornerRadius(10.0f);
      card.badge.SetOpacity(0.0f);
      card.eyebrowRow.SetLayoutTransition(NewAffordanceTransition(LayoutBoundsEdge::RIGHT));
    }

    card.title = NewLabel(TITLE[index], 31.0f, PRIMARY_TEXT_COLOR, "SamsungOneUI_700");
    card.title.SetRequestedHeight(54.0f);
    card.title.SetMultiLine(true);
    card.title.SetLineHeight(1.02f);
    card.title.SetTextFit(Text::Fit::Range(24.0f, 30.0f, 1.0f));
    if(index == 1u)
    {
      card.title.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
      card.title.SetTextGradient(NewSeaSunsetGradient(-0.12f));
    }
    card.root.Add(card.title);
    card.root.Add(NewDivider());

    card.places = NewLabel(PLACES[index], 18.0f, PRIMARY_TEXT_COLOR, "SamsungOneUI_500");
    card.places.SetRequestedHeight(64.0f);
    card.places.SetMultiLine(true);
    card.places.SetLineHeight(1.22f);
    card.root.Add(card.places);

    card.subtitle = NewLabel(SUBTITLE[index], 16.0f, SECONDARY_TEXT_COLOR);
    card.subtitle.SetRequestedHeight(82.0f);
    card.subtitle.SetMultiLine(true);
    card.subtitle.SetLineHeight(1.25f);
    card.root.Add(card.subtitle);
    card.root.Add(NewWeightedSpacer());

    if(index == 1u)
    {
      card.action = NewLabel("View day plan  →", 16.0f, CYAN_COLOR, "SamsungOneUI_700");
      card.action.SetRequestedHeight(32.0f);
      card.action.SetOpacity(0.0f);
      card.action.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
      card.action.SetTextGradient(NewActionBlueGradient());
      card.root.SetLayoutTransition(NewAffordanceTransition(LayoutBoundsEdge::BOTTOM));
    }

    const float progress = revealImmediately ? 1.0f : 0.0f;
    ConfigureReveal(card.day, Text::Reveal::Unit::WORD, Text::Reveal::AUTO_FADE_DURATION_RATIO, progress);
    ConfigureReveal(card.title, Text::Reveal::Unit::CHARACTER, Text::Reveal::AUTO_FADE_DURATION_RATIO, progress);
    ConfigureReveal(card.places, Text::Reveal::Unit::CHARACTER, Text::Reveal::AUTO_FADE_DURATION_RATIO, progress);
    ConfigureReveal(card.subtitle, Text::Reveal::Unit::WORD, Text::Reveal::AUTO_FADE_DURATION_RATIO, progress);
    return card;
  }

  View BuildResultCards(bool revealImmediately)
  {
    FlexLayout cards = NewWrappingRow();
    cards.SetLayoutTransition(NewCardEntranceTransition());
    for(std::size_t index = 0u; index < mCards.size(); ++index)
    {
      mCards[index] = NewItineraryCard(index, revealImmediately);
      mCards[index].root.SetOpacity(0.0f);
      cards.Add(mCards[index].root);
    }

    ScrollView scroll = ScrollView::New();
    scroll.SetScrollDirection(ScrollDirection::Vertical);
    scroll.SetRequestedWidth(MATCH_PARENT);
    scroll.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    scroll.SetContent(cards);
    return scroll;
  }

  View BuildResultsScene(bool revealImmediately)
  {
    StackLayout root = NewSceneRoot();
    root.SetPadding(Extents(SCENE_HORIZONTAL_PADDING,
                            SCENE_HORIZONTAL_PADDING,
                            SCENE_VERTICAL_PADDING,
                            SCENE_VERTICAL_PADDING));

    StackLayout header = NewHorizontal();
    header.SetRequestedWidth(MATCH_PARENT);
    header.SetRequestedHeight(30.0f);
    Label brand = NewLabel("DALI UI AI TRAVEL CONCIERGE", 14.0f, SECONDARY_TEXT_COLOR, "SamsungOneUI_700");
    brand.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    Label trip = NewLabel("JEJU  ·  3 DAYS  ·  RELAXED", 14.0f, MUTED_TEXT_COLOR, "SamsungOneUI_500");
    trip.SetHorizontalTextAlignment(Text::Alignment::END);
    header.Add(brand);
    header.Add(trip);
    root.Add(header);
    root.Add(NewVerticalSpacer(8.0f));

    Label heading = NewLabel("Your personalized escape", 39.0f, PRIMARY_TEXT_COLOR, "SamsungOneUI_700");
    heading.SetRequestedHeight(58.0f);
    root.Add(heading);
    Label subheading = NewLabel("A slower route through forests, coastlines and local neighborhoods.", 17.0f, SECONDARY_TEXT_COLOR);
    subheading.SetRequestedHeight(38.0f);
    root.Add(subheading);
    root.Add(NewVerticalSpacer(8.0f));
    root.Add(BuildResultCards(revealImmediately));

    Label hint = NewCenteredLabel("Select the best match to open your detailed AI itinerary", 14.0f, MUTED_TEXT_COLOR);
    hint.SetRequestedHeight(36.0f);
    root.Add(hint);

    mCards[1].root.SetFocusable(true);
    mCards[1].root.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      OpenDay2Detail();
    });
    return root;
  }

  View BuildDetailScene()
  {
    StackLayout root = NewSceneRoot();
    root.SetPadding(Extents(42, 42, 22, 24));

    StackLayout navigation = NewHorizontal(12.0f);
    navigation.SetRequestedWidth(MATCH_PARENT);
    navigation.SetRequestedHeight(44.0f);
    Label backButton = NewLabel("←  Back", 16.0f, SECONDARY_TEXT_COLOR, "SamsungOneUI_700");
    backButton.SetRequestedWidth(120.0f);
    backButton.SetFocusable(true);
    Label brand = NewCenteredLabel("DALI UI AI TRAVEL CONCIERGE", 13.0f, MUTED_TEXT_COLOR, "SamsungOneUI_700");
    brand.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    navigation.Add(backButton);
    navigation.Add(brand);
    navigation.Add(NewHorizontalSpacer(120.0f));
    root.Add(navigation);
    root.Add(NewVerticalSpacer(12.0f));

    Label day = NewLabel("DAY 2  ·  BEST MATCH", 14.0f, CYAN_COLOR, "SamsungOneUI_700");
    day.SetRequestedHeight(24.0f);
    root.Add(day);

    mDetailHero = NewLabel("Sea & Sunset", 46.0f, PRIMARY_TEXT_COLOR, "SamsungOneUI_700");
    mDetailHero.SetRequestedHeight(62.0f);
    mDetailHero.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
    mDetailHero.SetTextGradient(NewSeaSunsetGradient(-0.12f));
    root.Add(mDetailHero);

    Label route = NewLabel("Woljeongri  ·  Sehwa  ·  Hamdeok", 17.0f, SECONDARY_TEXT_COLOR);
    route.SetRequestedHeight(30.0f);
    root.Add(route);

    mDetailStatus = NewLabel("Planning your day...✦", 17.0f, SECONDARY_TEXT_COLOR, "SamsungOneUI_700");
    mDetailStatus.SetRequestedHeight(36.0f);
    root.Add(mDetailStatus);
    root.Add(NewDivider());
    root.Add(NewVerticalSpacer(12.0f));

    mMarkdownHost = NewVertical();
    mMarkdownHost.SetRequestedWidth(MATCH_PARENT);
    mMarkdownHost.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mMarkdownHost.SetLayoutTransition(NewMarkdownEntranceTransition());

    mMarkdownPanel = NewVertical();
    mMarkdownPanel.SetRequestedWidth(MATCH_PARENT);
    mMarkdownPanel.SetPadding(Extents(22, 22, 18, 24));
    mMarkdownPanel.SetBackgroundColor(UiColor(CARD_COLOR));
    mMarkdownPanel.SetCornerRadius(16.0f);
    mMarkdownPanel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    mMarkdownScroll = ScrollView::New();
    mMarkdownScroll.SetScrollDirection(ScrollDirection::Vertical);
    mMarkdownScroll.SetRequestedWidth(MATCH_PARENT);
    mMarkdownScroll.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    mMarkdownView = MarkdownView::New(NewTravelMarkdownStyle());
    mMarkdownView.SetRequestedWidth(MATCH_PARENT);
    mMarkdownView.SetRequestedHeight(WRAP_CONTENT);
    mMarkdownScroll.SetContent(mMarkdownView);
    mMarkdownPanel.Add(mMarkdownScroll);
    root.Add(mMarkdownHost);

    Label scrollHint = NewCenteredLabel("Scroll to explore the complete day plan", 12.0f, MUTED_TEXT_COLOR);
    scrollHint.SetRequestedHeight(24.0f);
    root.Add(scrollHint);

    backButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ShowResultsReady();
    });
    return root;
  }

  View BuildScene(DemoState state)
  {
    switch(state)
    {
      case DemoState::INTRO:
        return BuildIntroScene();
      case DemoState::GENERATING:
        return BuildGeneratingScene();
      case DemoState::REVEAL_RESULTS:
        return BuildResultsScene(false);
      case DemoState::RESULTS_READY:
        return BuildResultsScene(true);
      case DemoState::DETAIL_STREAMING:
      case DemoState::DETAIL_READY:
        return BuildDetailScene();
    }
    return View();
  }

  void StartSceneActivity(DemoState state, uint64_t token)
  {
    switch(state)
    {
      case DemoState::GENERATING:
        StartGeneratingActivity(token);
        break;
      case DemoState::REVEAL_RESULTS:
        StartResultReveal(token);
        break;
      case DemoState::RESULTS_READY:
        StartDay2Highlight(token, false);
        break;
      case DemoState::DETAIL_STREAMING:
        StartDetailActivity(token, true);
        break;
      case DemoState::DETAIL_READY:
        StartDetailActivity(token, false);
        SetDetailReadyImmediately();
        break;
      case DemoState::INTRO:
        StartIntroGradient();
        break;
    }
  }

  void InstallState(DemoState state, uint64_t token)
  {
    mState = state;
    ResetSceneHandles();
    mSceneContent = BuildScene(state);
    if(!mSceneContent)
    {
      return;
    }

    const PositionSize windowBounds = mWindow.GetPositionSize();
    SizeSceneContent(windowBounds.width, windowBounds.height);

    mSceneContent.SetOpacity(0.0f);
    mSceneRoot.Add(mSceneContent);
    StartSceneActivity(state, token);
  }

  void SizeSceneContent(int32_t windowWidth, int32_t windowHeight)
  {
    if(!mSceneContent)
    {
      return;
    }

    const float frameWidth  = std::min(static_cast<float>(windowWidth), MAX_SCENE_WIDTH);
    const float frameHeight = std::min(static_cast<float>(windowHeight), MAX_SCENE_HEIGHT);
    mSceneContent.SetRequestedWidth(frameWidth);
    mSceneContent.SetRequestedHeight(frameHeight);
  }

  void OnWindowResized(Window, Window::WindowSize windowSize)
  {
    SizeSceneContent(windowSize.GetWidth(), windowSize.GetHeight());
  }

  void OnSceneTransitionFinished(View view, LayoutTransitionSlot slot)
  {
    if(view != mSceneContent)
    {
      return;
    }

    if(slot == LayoutTransitionSlot::ENTER)
    {
      mTransitioning = false;
      return;
    }

    if(slot != LayoutTransitionSlot::EXIT ||
       !mTransitioning ||
       mTransitionToken != mLifecycleToken)
    {
      return;
    }

    const DemoState nextState = mPendingState;
    const uint64_t  token     = mTransitionToken;
    mSceneContent.Reset();
    StopSceneActivity();
    // Install only after EXIT completes to preserve fade-out then fade-in.
    // Adding the next scene during EXIT would turn this into a cross-fade.
    InstallState(nextState, token);
  }

  void TransitionTo(DemoState state)
  {
    if(mTransitioning || state == mState || !mSceneContent)
    {
      return;
    }

    mTransitionToken = ++mLifecycleToken;
    mPendingState    = state;
    mTransitioning   = true;
    StopSceneActivity(true);
    mSceneContent.SetOpacity(1.0f);
    mSceneRoot.Remove(mSceneContent, RemovePolicy::ANIMATE_EXIT);
  }

  void ResetToState(DemoState state)
  {
    const uint64_t token = ++mLifecycleToken;
    mTransitioning       = false;
    StopSceneActivity();
    RemoveCurrentSceneImmediately();
    InstallState(state, token);
  }

  void RemoveCurrentSceneImmediately()
  {
    if(mSceneRoot)
    {
      // The inherited immediate RemoveAll also cancels a scene that is currently
      // retained as an EXIT ghost by LayoutTransition.
      mSceneRoot.RemoveAll();
    }
    mSceneContent.Reset();
  }

  void StartGenerating()
  {
    if(mState == DemoState::INTRO)
    {
      TransitionTo(DemoState::GENERATING);
    }
  }

  void StartIntroGradient()
  {
    if(!mIntroJeju)
    {
      return;
    }

    mIntroJeju.SetTextGradient(NewJejuGradient(-0.24f));
    mIntroGradientAnimation = Animation::New(INTRO_GRADIENT_SECONDS);
    mIntroJeju.Animate(mIntroGradientAnimation)
      .TextGradientStartOffset(0.24f, Duration(INTRO_GRADIENT_SECONDS), AlphaFunction::EASE_IN_OUT);
    mIntroGradientAnimation.SetLooping(true);
    mIntroGradientAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mIntroGradientAnimation.SetEndAction(Animation::DISCARD);
    mIntroGradientAnimation.Play();
  }

  void RevealGeneratingStatus(std::size_t index, uint64_t token)
  {
    if(token != mLifecycleToken || mState != DemoState::GENERATING ||
       !mGeneratingStatus || index >= GENERATING_STATUS.size())
    {
      return;
    }

    StopAnimation(mGeneratingStatusRevealAnimation);
    mGeneratingStatus.SetText(GENERATING_STATUS[index]);
    ConfigureReveal(mGeneratingStatus, Text::Reveal::Unit::CHARACTER, 1.0f, 0.0f);
    mGeneratingStatusRevealAnimation = Animation::New(STATUS_FADE_IN_SECONDS);
    mGeneratingStatus.Animate(mGeneratingStatusRevealAnimation)
      .TextRevealProgress(1.0f, Duration(STATUS_FADE_IN_SECONDS), AlphaFunction::EASE_OUT);
    mGeneratingStatusRevealAnimation.Play();
  }

  void TransitionGeneratingStatus(std::size_t index, uint64_t token)
  {
    if(token != mLifecycleToken || mState != DemoState::GENERATING ||
       !mGeneratingStatus || index >= GENERATING_STATUS.size())
    {
      return;
    }

    StopAnimation(mGeneratingStatusRevealAnimation);
    ConfigureReveal(mGeneratingStatus, Text::Reveal::Unit::CHARACTER, 1.0f, 1.0f);
    mGeneratingStatusRevealAnimation = Animation::New(STATUS_FADE_OUT_SECONDS);
    mGeneratingStatus.Animate(mGeneratingStatusRevealAnimation)
      .TextRevealProgress(0.0f, Duration(STATUS_FADE_OUT_SECONDS), AlphaFunction::EASE_IN);
    mGeneratingStatusRevealAnimation.FinishedSignal().Connect(this, [this, index, token](Animation animation)
    {
      if(token != mLifecycleToken || mState != DemoState::GENERATING ||
         animation != mGeneratingStatusRevealAnimation)
      {
        return;
      }

      mGeneratingStatusRevealAnimation.Reset();
      RevealGeneratingStatus(index, token);
    });
    mGeneratingStatusRevealAnimation.Play();
  }

  void StartGeneratingActivity(uint64_t token)
  {
    mGeneratingStatusIndex = 0u;
    ConfigureShimmer(mGeneratingTitle, false);
    StartShimmer(mGeneratingTitle);
    RevealGeneratingStatus(mGeneratingStatusIndex, token);

    mGeneratingTimer = Timer::New(GENERATING_INTERVAL_MS);
    mGeneratingTimer.TickSignal().Connect(this, [this, token]()
    {
      if(token != mLifecycleToken || mState != DemoState::GENERATING)
      {
        return false;
      }

      ++mGeneratingStatusIndex;
      if(mGeneratingStatusIndex < GENERATING_STATUS.size())
      {
        TransitionGeneratingStatus(mGeneratingStatusIndex, token);
        return true;
      }

      TransitionTo(DemoState::REVEAL_RESULTS);
      return false;
    });
    mGeneratingTimer.Start();
  }

  void StartResultReveal(uint64_t token)
  {
    mRevealSequenceAnimation = Animation::New(REVEAL_SEQUENCE_SECONDS);
    static constexpr std::array<float, 3u> CARD_DELAY{{0.00f, 0.48f, 0.96f}};
    for(std::size_t index = 0u; index < mCards.size(); ++index)
    {
      const float delay = CARD_DELAY[index];
      mCards[index].day.Animate(mRevealSequenceAnimation)
        .TextRevealProgress(1.0f, Duration(0.42f), AlphaFunction::LINEAR, Duration(delay));
      mCards[index].title.Animate(mRevealSequenceAnimation)
        .TextRevealProgress(1.0f, Duration(0.88f), AlphaFunction::LINEAR, Duration(delay + 0.10f));
      mCards[index].places.Animate(mRevealSequenceAnimation)
        .TextRevealProgress(1.0f, Duration(1.18f), AlphaFunction::LINEAR, Duration(delay + 0.30f));
      mCards[index].subtitle.Animate(mRevealSequenceAnimation)
        .TextRevealProgress(1.0f, Duration(1.05f), AlphaFunction::LINEAR, Duration(delay + 0.42f));
    }
    mRevealSequenceAnimation.FinishedSignal().Connect(this, [this, token](Animation animation)
    {
      if(token != mLifecycleToken || animation != mRevealSequenceAnimation || mState != DemoState::REVEAL_RESULTS)
      {
        return;
      }
      mRevealSequenceAnimation.Reset();
      mState = DemoState::RESULTS_READY;
      StartDay2Highlight(token, true);
    });
    mRevealSequenceAnimation.Play();
  }

  void StartDay2Highlight(uint64_t token, bool runEntranceSweep)
  {
    if(!mCards[1].title)
    {
      return;
    }

    mCards[1].title.SetTextGradient(NewSeaSunsetGradient(-0.12f));
    mHeroGradientAnimation = Animation::New(HERO_GRADIENT_SECONDS);
    mCards[1].title.Animate(mHeroGradientAnimation)
      .TextGradientStartOffset(0.34f, Duration(HERO_GRADIENT_SECONDS), AlphaFunction::LINEAR);
    mHeroGradientAnimation.SetLooping(true);
    mHeroGradientAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mHeroGradientAnimation.SetEndAction(Animation::DISCARD);
    mHeroGradientAnimation.Play();

    if(mCards[1].eyebrowRow && mCards[1].badge && !mCards[1].badge.GetParent())
    {
      mCards[1].eyebrowRow.Add(mCards[1].badge);
    }
    if(mCards[1].root && mCards[1].action && !mCards[1].action.GetParent())
    {
      mCards[1].root.Add(mCards[1].action);
    }

    if(!runEntranceSweep)
    {
      return;
    }

    ConfigureShimmer(mCards[1].title, true);
    mOverlaySweepAnimation = Animation::New(HIGHLIGHT_SWEEP_SECONDS);
    mCards[1].title.Animate(mOverlaySweepAnimation)
      .TextGradientOverlayStartOffset(-1.15f, Duration(HIGHLIGHT_SWEEP_SECONDS), AlphaFunction::LINEAR);
    mOverlaySweepAnimation.FinishedSignal().Connect(this, [this, token](Animation animation)
    {
      if(token != mLifecycleToken || animation != mOverlaySweepAnimation)
      {
        return;
      }
      mOverlaySweepAnimation.Reset();
      if(mCards[1].title)
      {
        mCards[1].title.SetTextGradientOverlay(Gradient::Base::None());
      }
    });
    mOverlaySweepAnimation.Play();
  }

  void OpenDay2Detail()
  {
    if(mState == DemoState::RESULTS_READY)
    {
      TransitionTo(DemoState::DETAIL_STREAMING);
    }
  }

  void StartDetailActivity(uint64_t token, bool streamMarkdown)
  {
    mDetailHero.SetTextGradient(NewSeaSunsetGradient(-0.12f));
    mHeroGradientAnimation = Animation::New(HERO_GRADIENT_SECONDS);
    mDetailHero.Animate(mHeroGradientAnimation)
      .TextGradientStartOffset(0.34f, Duration(HERO_GRADIENT_SECONDS), AlphaFunction::LINEAR);
    mHeroGradientAnimation.SetLooping(true);
    mHeroGradientAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mHeroGradientAnimation.SetEndAction(Animation::DISCARD);
    mHeroGradientAnimation.Play();

    if(!streamMarkdown)
    {
      return;
    }

    PrepareMarkdownSimulation();

    ConfigureReveal(mDetailStatus, Text::Reveal::Unit::CHARACTER, 0.0f, 0.0f);
    mDetailStatusRevealAnimation = Animation::New(DETAIL_STATUS_REVEAL_SECONDS);
    mDetailStatus.Animate(mDetailStatusRevealAnimation)
      .TextRevealProgress(1.0f, Duration(DETAIL_STATUS_REVEAL_SECONDS), AlphaFunction::LINEAR);
    mDetailStatusRevealAnimation.FinishedSignal().Connect(this, [this, token](Animation animation)
    {
      if(token != mLifecycleToken || mState != DemoState::DETAIL_STREAMING ||
         animation != mDetailStatusRevealAnimation)
      {
        return;
      }

      mDetailStatusRevealAnimation.Reset();
      ConfigureShimmer(mDetailStatus, false);
      StartShimmer(mDetailStatus);
      StartMarkdownDelay(token);
    });
    mDetailStatusRevealAnimation.Play();
  }

  void StartWritingStatus(uint64_t token)
  {
    if(token != mLifecycleToken || mState != DemoState::DETAIL_STREAMING || !mDetailStatus)
    {
      return;
    }

    StopShimmer();
    StopAnimation(mDetailStatusRevealAnimation);
    mDetailStatus.SetText("Writing your plan...✦");
    ConfigureReveal(mDetailStatus, Text::Reveal::Unit::CHARACTER, 0.0f, 0.0f);

    mDetailStatusRevealAnimation = Animation::New(DETAIL_STATUS_REVEAL_SECONDS);
    mDetailStatus.Animate(mDetailStatusRevealAnimation)
      .TextRevealProgress(1.0f, Duration(DETAIL_STATUS_REVEAL_SECONDS), AlphaFunction::LINEAR);
    mDetailStatusRevealAnimation.FinishedSignal().Connect(this, [this, token](Animation animation)
    {
      if(token != mLifecycleToken || mState != DemoState::DETAIL_STREAMING ||
         animation != mDetailStatusRevealAnimation)
      {
        return;
      }

      mDetailStatusRevealAnimation.Reset();
      ConfigureShimmer(mDetailStatus, false);
      StartShimmer(mDetailStatus);
    });
    mDetailStatusRevealAnimation.Play();
  }

  void ConfigureShimmer(Label label, bool warmHighlight)
  {
    if(!label)
    {
      return;
    }
    label.SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
    label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SCREEN);
    label.SetTextGradientOverlay(NewShimmerOverlay(1.15f, warmHighlight));
  }

  void StartShimmer(Label label)
  {
    StopShimmer();
    if(!label)
    {
      return;
    }
    mShimmerLabel     = label;
    mShimmerAnimation = Animation::New(SHIMMER_DURATION_SECONDS);
    label.Animate(mShimmerAnimation)
      .TextGradientOverlayStartOffset(-1.15f, Duration(SHIMMER_DURATION_SECONDS), AlphaFunction::LINEAR);
    mShimmerAnimation.SetLooping(true);
    mShimmerAnimation.SetEndAction(Animation::DISCARD);
    mShimmerAnimation.Play();
  }

  void StopShimmer()
  {
    StopAnimation(mShimmerAnimation);
    if(mShimmerLabel)
    {
      mShimmerLabel.SetTextGradientOverlay(Gradient::Base::None());
    }
    mShimmerLabel.Reset();
  }

  void ShowResultsReady()
  {
    if(mState == DemoState::DETAIL_STREAMING || mState == DemoState::DETAIL_READY)
    {
      TransitionTo(DemoState::RESULTS_READY);
    }
  }

  void RestartDemo()
  {
    ResetToState(DemoState::INTRO);
  }

  void OnPrimaryAction()
  {
    switch(mState)
    {
      case DemoState::INTRO:
        StartGenerating();
        break;
      case DemoState::RESULTS_READY:
        OpenDay2Detail();
        break;
      case DemoState::DETAIL_READY:
        RestartDemo();
        break;
      case DemoState::GENERATING:
      case DemoState::REVEAL_RESULTS:
      case DemoState::DETAIL_STREAMING:
        break;
    }
  }

  void OnBackAction()
  {
    switch(mState)
    {
      case DemoState::DETAIL_STREAMING:
      case DemoState::DETAIL_READY:
        ShowResultsReady();
        break;
      case DemoState::GENERATING:
      case DemoState::REVEAL_RESULTS:
      case DemoState::RESULTS_READY:
        TransitionTo(DemoState::INTRO);
        break;
      case DemoState::INTRO:
        mApplication.Quit();
        break;
    }
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    const Dali::String& key = event.GetKeyName();
    if(key == "0" || key == "KP_0")
    {
      RestartDemo();
    }
    else if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      OnBackAction();
    }
    else if(IsKey(event, Dali::DALI_KEY_RETURN) || key == "Return" || key == "Enter" ||
            key == "space" || key == "Space" || key == " ")
    {
      OnPrimaryAction();
    }
  }

  static void StopAnimation(Animation& animation)
  {
    if(animation)
    {
      animation.Stop();
      animation.Clear();
      animation.Reset();
    }
  }

  static void StopTimer(Timer& timer)
  {
    if(timer)
    {
      timer.Stop();
      timer.Reset();
    }
  }

  void StopSceneActivity(bool preserveSceneVisuals = false)
  {
    StopTimer(mGeneratingTimer);
    StopTimer(mMarkdownStartTimer);
    StopTimer(mMarkdownStreamStartTimer);
    StopTimer(mMarkdownTimer);
    StopTimer(mFinalScrollTimer);

    if(preserveSceneVisuals)
    {
      return;
    }

    StopShimmer();
    StopAnimation(mIntroGradientAnimation);
    StopAnimation(mGeneratingStatusRevealAnimation);
    StopAnimation(mRevealSequenceAnimation);
    StopAnimation(mHeroGradientAnimation);
    StopAnimation(mOverlaySweepAnimation);
    StopAnimation(mDetailStatusRevealAnimation);
    StopAnimation(mCompletionAnimation);

    if(mCards[1].title)
    {
      mCards[1].title.SetTextGradientOverlay(Gradient::Base::None());
    }
    if(mDetailStatus)
    {
      mDetailStatus.SetTextGradientOverlay(Gradient::Base::None());
    }
  }

  void ResetSceneHandles()
  {
    mIntroJeju.Reset();
    mGeneratingTitle.Reset();
    mGeneratingStatus.Reset();
    mDetailHero.Reset();
    mDetailStatus.Reset();
    mMarkdownScroll.Reset();
    mMarkdownHost.Reset();
    mMarkdownPanel.Reset();
    mMarkdownView.Reset();
    for(ItineraryCard& card : mCards)
    {
      card = ItineraryCard{};
    }
  }

  // Markdown streaming simulation is implemented after main().
  void PrepareMarkdownSimulation();
  void StartMarkdownDelay(uint64_t token);
  void ShowMarkdownPanel(uint64_t token);
  void StartMarkdownStreaming(uint64_t token);
  void TrackMarkdownBottom();
  void AppendNextMarkdownCharacters(uint64_t token);
  void FinishMarkdownStreaming(uint64_t token);
  void SetDetailReadyImmediately();

private:
  Application& mApplication;
  Window       mWindow;
  DemoState    mState{DemoState::INTRO};
  DemoState    mPendingState{DemoState::INTRO};
  View         mSceneRoot;
  View         mSceneContent;

  Label mIntroJeju;
  Label mGeneratingTitle;
  Label mGeneratingStatus;
  Label mDetailHero;
  Label mDetailStatus;
  Label mShimmerLabel;

  std::array<ItineraryCard, 3u> mCards;
  ScrollView                    mMarkdownScroll;
  StackLayout                   mMarkdownHost;
  StackLayout                   mMarkdownPanel;
  MarkdownView                  mMarkdownView;

  Timer mGeneratingTimer;

  Animation mIntroGradientAnimation;
  Animation mShimmerAnimation;
  Animation mGeneratingStatusRevealAnimation;
  Animation mRevealSequenceAnimation;
  Animation mHeroGradientAnimation;
  Animation mOverlaySweepAnimation;
  Animation mDetailStatusRevealAnimation;
  Animation mCompletionAnimation;

  std::size_t mGeneratingStatusIndex{0u};
  uint64_t    mLifecycleToken{0u};
  uint64_t    mTransitionToken{0u};
  bool        mTransitioning{false};

  // Markdown streaming simulation state.
  Timer       mMarkdownStartTimer;
  Timer       mMarkdownStreamStartTimer;
  Timer       mMarkdownTimer;
  Timer       mFinalScrollTimer;
  std::size_t mMarkdownByteOffset{0u};
  std::size_t mMarkdownStreamStep{0u};
  std::string mMarkdownFullSource;
  std::string mMarkdownSource;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.Apply();

  TextEffectDemo controller(application);
  application.MainLoop();
  return 0;
}

// Markdown streaming simulator
namespace
{
constexpr uint32_t DETAIL_MARKDOWN_DELAY_MS        = 3000u;
constexpr uint32_t MARKDOWN_STREAM_START_DELAY_MS  = 180u;
constexpr uint32_t MARKDOWN_STREAM_INTERVAL        = 6u;
constexpr uint32_t MARKDOWN_SCROLL_INTERVAL        = 10u;
constexpr uint32_t FINAL_SCROLL_DELAY_MS           = 32u;

constexpr std::array<const char*, 10u> MARKDOWN_CHUNKS{{
  R"MD(# Day 2 · Sea & Sunset 🌊

Today is about slowing down and following Jeju's eastern coastline.

)MD",
  R"MD(Rather than checking off as many places as possible, this route keeps travel time short and leaves room to enjoy each stop.

)MD",
  R"MD(## Morning · Woljeongri Beach

Start the day by the sea before the busiest hours.

**Recommended time:** 9:30 AM – 11:30 AM

)MD",
  R"MD(- Walk along Woljeongri Beach
- Stop at a small ocean-view cafe
- Explore the quiet streets behind the coast
- Take the coastal road toward Sehwa

)MD",
  R"MD(> Travel tip: The water often looks clearest before noon, and the beach is noticeably quieter in the morning.

)MD",
  R"MD(## Lunch · Sehwa 🍜

Head east to Sehwa for a relaxed local lunch.

### Try something local

- Jeju-style seafood noodles
- Grilled cutlassfish
- Abalone porridge

Keep about **90 minutes** free for lunch and a short walk around the village.

)MD",
  R"MD(## Afternoon · Follow the Coast

Take the slower coastal road toward Hamdeok.

You'll pass small beaches, stone walls, wind turbines, and quiet villages along the way.

### Optional stop

If you have extra time, stop for coffee near Gimnyeong and enjoy the view before continuing west.

)MD",
  R"MD(## Sunset · Hamdeok 🌅

Arrive at Hamdeok before sunset.

Find a place near the western side of the beach and stay until the sky begins to change.

**Best arrival time:** about 60 minutes before sunset.

> Don't rush to the next destination. This is the final activity of the day.

)MD",
  R"MD(## Today's Route

1. Woljeongri Beach
2. Sehwa
3. Coastal Road
4. Hamdeok Beach

---

)MD",
  R"MD(### AI Travel Note ✨️

Your route was optimized to reduce unnecessary driving while keeping the ocean visible for most of the day.

**Total driving:** approximately 1 hour 20 minutes

**Recommended pace:** Relaxed

**Theme:** Sea · Cafe · Sunset

Enjoy the slow side of Jeju.
)MD",
}};

std::size_t NextUtf8CharacterEnd(const std::string& text, std::size_t offset)
{
  if(offset >= text.size())
  {
    return text.size();
  }

  std::size_t next = offset + 1u;
  while(next < text.size() && (static_cast<unsigned char>(text[next]) & 0xC0u) == 0x80u)
  {
    ++next;
  }
  return next;
}

std::string BuildMarkdownSource()
{
  std::string source;
  source.reserve(2600u);
  for(const char* chunk : MARKDOWN_CHUNKS)
  {
    source += chunk;
  }
  return source;
}
} // namespace

void TextEffectDemo::PrepareMarkdownSimulation()
{
  mMarkdownFullSource = BuildMarkdownSource();
  mMarkdownSource.clear();
  mMarkdownSource.reserve(mMarkdownFullSource.size());
  mMarkdownByteOffset = 0u;
  mMarkdownStreamStep = 0u;
  mMarkdownPanel.SetOpacity(0.0f);
}

void TextEffectDemo::StartMarkdownDelay(uint64_t token)
{
  if(token != mLifecycleToken || mState != DemoState::DETAIL_STREAMING)
  {
    return;
  }

  mMarkdownStartTimer = Timer::New(DETAIL_MARKDOWN_DELAY_MS);
  mMarkdownStartTimer.TickSignal().Connect(this, [this, token]()
  {
    if(token == mLifecycleToken && mState == DemoState::DETAIL_STREAMING)
    {
      ShowMarkdownPanel(token);
    }
    return false;
  });
  mMarkdownStartTimer.Start();
}

void TextEffectDemo::ShowMarkdownPanel(uint64_t token)
{
  if(token != mLifecycleToken || mState != DemoState::DETAIL_STREAMING)
  {
    return;
  }

  mMarkdownHost.Add(mMarkdownPanel);

  mMarkdownStreamStartTimer = Timer::New(MARKDOWN_STREAM_START_DELAY_MS);
  mMarkdownStreamStartTimer.TickSignal().Connect(this, [this, token]()
  {
    if(token == mLifecycleToken && mState == DemoState::DETAIL_STREAMING)
    {
      StartMarkdownStreaming(token);
    }
    return false;
  });
  mMarkdownStreamStartTimer.Start();
}

void TextEffectDemo::StartMarkdownStreaming(uint64_t token)
{
  if(token != mLifecycleToken || mState != DemoState::DETAIL_STREAMING)
  {
    return;
  }

  StartWritingStatus(token);
  AppendNextMarkdownCharacters(token);

  mMarkdownTimer = Timer::New(MARKDOWN_STREAM_INTERVAL);
  mMarkdownTimer.TickSignal().Connect(this, [this, token]()
  {
    if(token != mLifecycleToken || mState != DemoState::DETAIL_STREAMING)
    {
      return false;
    }
    AppendNextMarkdownCharacters(token);
    return mState == DemoState::DETAIL_STREAMING;
  });
  mMarkdownTimer.Start();
}

void TextEffectDemo::TrackMarkdownBottom()
{
  if(!mMarkdownScroll || !mMarkdownView)
  {
    return;
  }

  const float scrollRange = std::max(0.0f,
                                     mMarkdownView.GetCurrentSize().height -
                                       mMarkdownScroll.GetCurrentSize().height);
  if(scrollRange > 0.0f)
  {
    mMarkdownScroll.ScrollTo(Vector2(0.0f, scrollRange), false);
  }
}

void TextEffectDemo::AppendNextMarkdownCharacters(uint64_t token)
{
  if(token != mLifecycleToken || mState != DemoState::DETAIL_STREAMING)
  {
    return;
  }

  const std::size_t characterCount = (mMarkdownStreamStep % 2u == 0u) ? 1u : 2u;
  for(std::size_t count = 0u; count < characterCount && mMarkdownByteOffset < mMarkdownFullSource.size(); ++count)
  {
    mMarkdownByteOffset = NextUtf8CharacterEnd(mMarkdownFullSource, mMarkdownByteOffset);
  }

  mMarkdownSource.assign(mMarkdownFullSource, 0u, mMarkdownByteOffset);
  mMarkdownView.SetMarkdown(Dali::String(mMarkdownSource.c_str()));
  ++mMarkdownStreamStep;
  if(mMarkdownStreamStep % MARKDOWN_SCROLL_INTERVAL == 0u)
  {
    TrackMarkdownBottom();
  }

  if(mMarkdownByteOffset >= mMarkdownFullSource.size())
  {
    FinishMarkdownStreaming(token);
  }
}

void TextEffectDemo::FinishMarkdownStreaming(uint64_t token)
{
  if(token != mLifecycleToken || mState != DemoState::DETAIL_STREAMING)
  {
    return;
  }

  StopTimer(mMarkdownTimer);
  TrackMarkdownBottom();
  StopAnimation(mDetailStatusRevealAnimation);
  StopShimmer();
  mDetailStatus.SetTextGradientOverlay(Gradient::Base::None());
  StopAnimation(mCompletionAnimation);
  mState = DemoState::DETAIL_READY;
  mDetailStatus.SetText("Your day is ready  ✓");
  ConfigureReveal(mDetailStatus, Text::Reveal::Unit::WORD, Text::Reveal::AUTO_FADE_DURATION_RATIO, 0.0f);

  mCompletionAnimation = Animation::New(COMPLETE_REVEAL_SECONDS);
  mDetailStatus.Animate(mCompletionAnimation)
    .TextRevealProgress(1.0f, Duration(COMPLETE_REVEAL_SECONDS), AlphaFunction::LINEAR);
  mCompletionAnimation.FinishedSignal().Connect(this, [this, token](Animation animation)
  {
    if(token != mLifecycleToken || animation != mCompletionAnimation)
    {
      return;
    }
    mCompletionAnimation.Reset();
    if(mDetailStatus)
    {
      mDetailStatus.SetTextColor(UiColor(SUCCESS_COLOR));
    }
  });
  mCompletionAnimation.Play();

  mFinalScrollTimer = Timer::New(FINAL_SCROLL_DELAY_MS);
  mFinalScrollTimer.TickSignal().Connect(this, [this, token]()
  {
    if(token == mLifecycleToken && mState == DemoState::DETAIL_READY)
    {
      TrackMarkdownBottom();
    }
    return false;
  });
  mFinalScrollTimer.Start();
}

void TextEffectDemo::SetDetailReadyImmediately()
{
  mMarkdownFullSource = BuildMarkdownSource();
  mMarkdownSource     = mMarkdownFullSource;
  mMarkdownView.SetMarkdown(Dali::String(mMarkdownSource.c_str()));
  mMarkdownHost.Add(mMarkdownPanel);
  mDetailStatus.SetText("Your day is ready  ✓");
  mDetailStatus.SetTextColor(UiColor(SUCCESS_COLOR));
}
