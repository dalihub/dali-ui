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
 *
 */

#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
} // namespace

void utc_dali_text_value_types_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_value_types_cleanup(void)
{
  test_return_value = TET_PASS;
}

// FontAttributes Tests

int UtcDaliTextFontAttributesDefaultP(void)
{
  UiTestApplication application;

  Text::FontAttributes attributes;

  DALI_TEST_CHECK(!attributes.HasAttributes());
  DALI_TEST_CHECK(!attributes.Has(Text::FontAttributes::Attribute::FAMILY));
  DALI_TEST_CHECK(!attributes.Has(Text::FontAttributes::Attribute::SIZE));
  DALI_TEST_CHECK(!attributes.Has(Text::FontAttributes::Attribute::WEIGHT));
  DALI_TEST_CHECK(!attributes.Has(Text::FontAttributes::Attribute::WIDTH));
  DALI_TEST_CHECK(!attributes.Has(Text::FontAttributes::Attribute::SLANT));

  END_TEST;
}

int UtcDaliTextRevealValueTypeP(void)
{
  UiTestApplication application;

  Text::Reveal reveal;
  DALI_TEST_EQUALS(reveal.GetUnit(), Text::Reveal::Unit::CHARACTER, TEST_LOCATION);
  DALI_TEST_EQUALS(reveal.GetSequence(), Text::Reveal::Sequence::WHOLE_TEXT, TEST_LOCATION);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), Text::Reveal::AUTO_FADE_DURATION_RATIO, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_CHECK(reveal != Text::Reveal::None());
  DALI_TEST_CHECK(Text::Reveal::None() == Text::Reveal::None());

  reveal.SetUnit(Text::Reveal::Unit::PIXEL);
  DALI_TEST_EQUALS(reveal.GetUnit(), Text::Reveal::Unit::PIXEL, TEST_LOCATION);
  Text::Reveal pixelCopied(reveal);
  DALI_TEST_CHECK(pixelCopied == reveal);
  Text::Reveal pixelAssigned;
  pixelAssigned = pixelCopied;
  DALI_TEST_CHECK(pixelAssigned == reveal);
  Text::Reveal pixelMoved(std::move(pixelCopied));
  DALI_TEST_CHECK(pixelMoved == reveal);
  Text::Reveal pixelMoveAssigned;
  pixelMoveAssigned = std::move(pixelAssigned);
  DALI_TEST_CHECK(pixelMoveAssigned == reveal);

  reveal.SetUnit(Text::Reveal::Unit::LINE);
  DALI_TEST_EQUALS(reveal.GetUnit(), Text::Reveal::Unit::LINE, TEST_LOCATION);
  Text::Reveal lineCopied(reveal);
  DALI_TEST_CHECK(lineCopied == reveal);
  Text::Reveal lineAssigned;
  lineAssigned = lineCopied;
  DALI_TEST_CHECK(lineAssigned == reveal);
  Text::Reveal lineMoved(std::move(lineCopied));
  DALI_TEST_CHECK(lineMoved == reveal);
  Text::Reveal lineMoveAssigned;
  lineMoveAssigned = std::move(lineAssigned);
  DALI_TEST_CHECK(lineMoveAssigned == reveal);

  reveal.SetUnit(Text::Reveal::Unit::WORD);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  reveal.SetFadeDurationRatio(0.2f);
  DALI_TEST_EQUALS(reveal.GetSequence(), Text::Reveal::Sequence::PER_LINE, TEST_LOCATION);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 0.2f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Text::Reveal nearlyEqual(reveal);
  nearlyEqual.SetFadeDurationRatio(std::nextafter(0.2f, 1.0f));
  DALI_TEST_CHECK(nearlyEqual == reveal);
  DALI_TEST_CHECK(nearlyEqual.GetFadeDurationRatio() != reveal.GetFadeDurationRatio());

  Text::Reveal copied(reveal);
  DALI_TEST_CHECK(copied == reveal);
  Text::Reveal assigned;
  assigned = copied;
  DALI_TEST_CHECK(assigned == copied);
  Text::Reveal moved(std::move(copied));
  DALI_TEST_CHECK(moved == reveal);
  Text::Reveal moveAssigned;
  moveAssigned = std::move(assigned);
  DALI_TEST_CHECK(moveAssigned == reveal);

  Text::Reveal differentSequence(reveal);
  differentSequence.SetSequence(Text::Reveal::Sequence::WHOLE_TEXT);
  DALI_TEST_CHECK(differentSequence != reveal);
  Text::Reveal differentSequenceStagger(reveal);
  differentSequenceStagger.SetSequenceStaggerRatio(0.5f);
  DALI_TEST_CHECK(differentSequenceStagger != reveal);

  Text::Reveal noneCopy(Text::Reveal::None());
  DALI_TEST_CHECK(noneCopy == Text::Reveal::None());
  DALI_TEST_ASSERTION(noneCopy.GetUnit(), "Cannot access Text::Reveal::None() properties.");
  DALI_TEST_ASSERTION(noneCopy.SetFadeDurationRatio(0.0f), "Cannot modify Text::Reveal::None().");
  DALI_TEST_ASSERTION(noneCopy.GetSequence(), "Cannot access Text::Reveal::None() properties.");
  DALI_TEST_ASSERTION(noneCopy.SetSequence(Text::Reveal::Sequence::PER_LINE), "Cannot modify Text::Reveal::None().");
  DALI_TEST_ASSERTION(noneCopy.GetSequenceStaggerRatio(), "Cannot access Text::Reveal::None() properties.");
  DALI_TEST_ASSERTION(noneCopy.SetSequenceStaggerRatio(0.5f), "Cannot modify Text::Reveal::None().");

  reveal.SetSequence(Text::Reveal::Sequence::WHOLE_TEXT);
  DALI_TEST_EQUALS(reveal.GetSequence(), Text::Reveal::Sequence::WHOLE_TEXT, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(-1.0f);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(0.0f);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(0.25f);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(0.5f);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(0.75f);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.75f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(1.0f);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(2.0f);
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(std::numeric_limits<float>::quiet_NaN());
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(std::numeric_limits<float>::infinity());
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetSequenceStaggerRatio(-std::numeric_limits<float>::infinity());
  DALI_TEST_EQUALS(reveal.GetSequenceStaggerRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  reveal.SetFadeDurationRatio(Text::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), Text::Reveal::AUTO_FADE_DURATION_RATIO, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetFadeDurationRatio(-0.5f);
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetFadeDurationRatio(-2.0f);
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetFadeDurationRatio(0.0f);
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetFadeDurationRatio(0.5f);
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetFadeDurationRatio(1.0f);
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetFadeDurationRatio(2.0f);
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetFadeDurationRatio(std::numeric_limits<float>::quiet_NaN());
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetFadeDurationRatio(std::numeric_limits<float>::infinity());
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  reveal.SetFadeDurationRatio(-std::numeric_limits<float>::infinity());
  DALI_TEST_EQUALS(reveal.GetFadeDurationRatio(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextFontAttributesSetAndUnsetP(void)
{
  UiTestApplication application;

  Text::FontAttributes attributes;

  attributes.SetWeight(Text::FontWeight::BOLD);
  DALI_TEST_CHECK(attributes.HasAttributes());
  DALI_TEST_CHECK(attributes.Has(Text::FontAttributes::Attribute::WEIGHT));
  DALI_TEST_EQUALS(attributes.GetWeight(), Text::FontWeight::BOLD, TEST_LOCATION);

  attributes.SetWeight(Text::FontWeight::NORMAL);
  DALI_TEST_CHECK(attributes.Has(Text::FontAttributes::Attribute::WEIGHT));
  DALI_TEST_EQUALS(attributes.GetWeight(), Text::FontWeight::NORMAL, TEST_LOCATION);

  Text::FontAttributes inherited;
  DALI_TEST_CHECK(inherited != attributes);

  attributes.Unset(Text::FontAttributes::Attribute::WEIGHT);
  DALI_TEST_CHECK(!attributes.Has(Text::FontAttributes::Attribute::WEIGHT));
  DALI_TEST_CHECK(!attributes.HasAttributes());
  DALI_TEST_CHECK(inherited == attributes);

  END_TEST;
}

int UtcDaliTextFontAttributesFamilyEmptyStringP(void)
{
  UiTestApplication application;

  Text::FontAttributes attributes;

  attributes.SetFamily("");
  DALI_TEST_CHECK(attributes.HasAttributes());
  DALI_TEST_CHECK(attributes.Has(Text::FontAttributes::Attribute::FAMILY));
  DALI_TEST_EQUALS(attributes.GetFamily(), Dali::String(""), TEST_LOCATION);

  attributes.Unset(Text::FontAttributes::Attribute::FAMILY);
  DALI_TEST_CHECK(!attributes.Has(Text::FontAttributes::Attribute::FAMILY));
  DALI_TEST_CHECK(!attributes.HasAttributes());

  END_TEST;
}

int UtcDaliTextFontAttributesAllFieldsP(void)
{
  UiTestApplication application;

  Text::FontAttributes attributes;
  attributes.SetFamily("Ubuntu Mono");
  attributes.SetSize(28.0f);
  attributes.SetWeight(Text::FontWeight::BOLD);
  attributes.SetWidth(Text::FontWidth::CONDENSED);
  attributes.SetSlant(Text::FontSlant::ITALIC);

  DALI_TEST_CHECK(attributes.Has(Text::FontAttributes::Attribute::FAMILY));
  DALI_TEST_CHECK(attributes.Has(Text::FontAttributes::Attribute::SIZE));
  DALI_TEST_CHECK(attributes.Has(Text::FontAttributes::Attribute::WEIGHT));
  DALI_TEST_CHECK(attributes.Has(Text::FontAttributes::Attribute::WIDTH));
  DALI_TEST_CHECK(attributes.Has(Text::FontAttributes::Attribute::SLANT));
  DALI_TEST_EQUALS(attributes.GetFamily(), Dali::String("Ubuntu Mono"), TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetSize(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetWeight(), Text::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetWidth(), Text::FontWidth::CONDENSED, TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetSlant(), Text::FontSlant::ITALIC, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFontAttributesEqualityUsesDefinedMaskP(void)
{
  UiTestApplication application;

  Text::FontAttributes a;
  Text::FontAttributes b;
  b.SetWeight(Text::FontWeight::BOLD);
  b.Unset(Text::FontAttributes::Attribute::WEIGHT);

  DALI_TEST_CHECK(a == b);

  a.SetWeight(Text::FontWeight::NORMAL);
  b.SetWeight(Text::FontWeight::BOLD);
  b.Unset(Text::FontAttributes::Attribute::WEIGHT);
  DALI_TEST_CHECK(a != b);

  Text::FontAttributes c;
  c.SetWeight(Text::FontWeight::NORMAL);
  DALI_TEST_CHECK(a == c);

  c.SetSlant(Text::FontSlant::NORMAL);
  DALI_TEST_CHECK(a != c);

  END_TEST;
}

int UtcDaliTextFontAttributesCopyAndMoveP(void)
{
  UiTestApplication application;

  Text::FontAttributes original;
  original.SetFamily("Ubuntu Mono");
  original.SetSize(30.0f);
  original.SetWeight(Text::FontWeight::BOLD);

  Text::FontAttributes copy(original);
  DALI_TEST_CHECK(copy == original);

  Text::FontAttributes assigned;
  assigned = original;
  DALI_TEST_CHECK(assigned == original);

  Text::FontAttributes moved(std::move(copy));
  DALI_TEST_CHECK(moved == original);

  Text::FontAttributes moveAssigned;
  moveAssigned = std::move(assigned);
  DALI_TEST_CHECK(moveAssigned == original);

  END_TEST;
}

int UtcDaliTextFontAttributesInvalidAttributeP(void)
{
  UiTestApplication application;

  Text::FontAttributes attributes;
  const auto           invalidAttribute = static_cast<Text::FontAttributes::Attribute>(999u);

  DALI_TEST_CHECK(!attributes.Has(invalidAttribute));
  attributes.Unset(invalidAttribute);
  DALI_TEST_CHECK(!attributes.HasAttributes());

  END_TEST;
}

// AnchorAttributes Tests

int UtcDaliTextAnchorAttributesDefaultP(void)
{
  UiTestApplication application;

  Text::AnchorAttributes attributes;

  DALI_TEST_CHECK(!attributes.HasAttributes());
  DALI_TEST_CHECK(!attributes.Has(Text::AnchorAttributes::Attribute::HREF));
  DALI_TEST_CHECK(!attributes.Has(Text::AnchorAttributes::Attribute::COLOR));
  DALI_TEST_CHECK(!attributes.Has(Text::AnchorAttributes::Attribute::CLICKED_COLOR));

  END_TEST;
}

int UtcDaliTextAnchorAttributesSetAndUnsetP(void)
{
  UiTestApplication application;

  Text::AnchorAttributes attributes;

  attributes.SetHref("");
  DALI_TEST_CHECK(attributes.HasAttributes());
  DALI_TEST_CHECK(attributes.Has(Text::AnchorAttributes::Attribute::HREF));
  DALI_TEST_EQUALS(attributes.GetHref(), Dali::String(""), TEST_LOCATION);

  attributes.SetColor(UiColor(Color::GREEN));
  DALI_TEST_CHECK(attributes.Has(Text::AnchorAttributes::Attribute::COLOR));
  DALI_TEST_EQUALS(attributes.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  attributes.SetClickedColor(UiColor(Color::RED));
  DALI_TEST_CHECK(attributes.Has(Text::AnchorAttributes::Attribute::CLICKED_COLOR));
  DALI_TEST_EQUALS(attributes.GetClickedColor().GetRgba(), Color::RED, TEST_LOCATION);

  attributes.Unset(Text::AnchorAttributes::Attribute::COLOR);
  DALI_TEST_CHECK(!attributes.Has(Text::AnchorAttributes::Attribute::COLOR));
  DALI_TEST_CHECK(attributes.HasAttributes());

  attributes.Unset(Text::AnchorAttributes::Attribute::HREF);
  attributes.Unset(Text::AnchorAttributes::Attribute::CLICKED_COLOR);
  DALI_TEST_CHECK(!attributes.HasAttributes());

  END_TEST;
}

int UtcDaliTextAnchorAttributesEqualityUsesDefinedMaskP(void)
{
  UiTestApplication application;

  Text::AnchorAttributes unset;
  Text::AnchorAttributes explicitEmpty;
  explicitEmpty.SetHref("");
  DALI_TEST_CHECK(unset != explicitEmpty);

  Text::AnchorAttributes a;
  Text::AnchorAttributes b;
  a.SetHref("https://www.tizen.org");
  b.SetHref("https://www.tizen.org");
  DALI_TEST_CHECK(a == b);

  a.SetColor(UiColor());
  DALI_TEST_CHECK(a != b);

  b.SetColor(UiColor());
  DALI_TEST_CHECK(a == b);

  b.SetClickedColor(UiColor(Color::BLUE));
  DALI_TEST_CHECK(a != b);

  b.Unset(Text::AnchorAttributes::Attribute::CLICKED_COLOR);
  DALI_TEST_CHECK(a == b);

  END_TEST;
}

int UtcDaliTextAnchorAttributesCopyAndMoveP(void)
{
  UiTestApplication application;

  Text::AnchorAttributes original;
  original.SetHref("href");
  original.SetColor(UiColor(Color::CYAN));

  Text::AnchorAttributes copy(original);
  DALI_TEST_CHECK(copy == original);

  Text::AnchorAttributes assigned;
  assigned = original;
  DALI_TEST_CHECK(assigned == original);

  Text::AnchorAttributes moved(std::move(copy));
  DALI_TEST_CHECK(moved == original);

  Text::AnchorAttributes moveAssigned;
  moveAssigned = std::move(assigned);
  DALI_TEST_CHECK(moveAssigned == original);

  END_TEST;
}

int UtcDaliTextAnchorAttributesInvalidAttributeP(void)
{
  UiTestApplication application;

  Text::AnchorAttributes attributes;
  const auto             invalidAttribute = static_cast<Text::AnchorAttributes::Attribute>(999u);

  DALI_TEST_CHECK(!attributes.Has(invalidAttribute));
  attributes.Unset(invalidAttribute);
  DALI_TEST_CHECK(!attributes.HasAttributes());

  END_TEST;
}

// Underline Tests

int UtcDaliTextUnderlineColorP(void)
{
  UiTestApplication application;

  Text::Underline underline;

  UiColor color(Color::RED);
  underline.SetColor(color);
  DALI_TEST_EQUALS(underline.GetColor().GetRgba(), Color::RED, TEST_LOCATION);

  UiColor color2(Color::BLUE);
  underline.SetColor(color2);
  DALI_TEST_EQUALS(underline.GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUnderlineThicknessP(void)
{
  UiTestApplication application;

  Text::Underline underline;

  underline.SetThickness(2.5f);
  DALI_TEST_EQUALS(underline.GetThickness(), 2.5f, TEST_LOCATION);

  underline.SetThickness(5.0f);
  DALI_TEST_EQUALS(underline.GetThickness(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUnderlineTypeP(void)
{
  UiTestApplication application;

  Text::Underline underline;

  underline.SetType(Text::Underline::Type::DASHED);
  DALI_TEST_EQUALS(underline.GetType(), Text::Underline::Type::DASHED, TEST_LOCATION);

  underline.SetType(Text::Underline::Type::DOUBLE);
  DALI_TEST_EQUALS(underline.GetType(), Text::Underline::Type::DOUBLE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUnderlineDashLengthP(void)
{
  UiTestApplication application;

  Text::Underline underline;

  underline.SetDashLength(4.0f);
  DALI_TEST_EQUALS(underline.GetDashLength(), 4.0f, TEST_LOCATION);

  underline.SetDashLength(8.0f);
  DALI_TEST_EQUALS(underline.GetDashLength(), 8.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUnderlineDashGapP(void)
{
  UiTestApplication application;

  Text::Underline underline;

  underline.SetDashGap(3.0f);
  DALI_TEST_EQUALS(underline.GetDashGap(), 3.0f, TEST_LOCATION);

  underline.SetDashGap(6.0f);
  DALI_TEST_EQUALS(underline.GetDashGap(), 6.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUnderlineCopyCtorP(void)
{
  UiTestApplication application;

  Text::Underline original;
  original.SetColor(UiColor(Color::RED));
  original.SetThickness(3.0f);
  original.SetType(Text::Underline::Type::DASHED);
  original.SetDashLength(5.0f);
  original.SetDashGap(2.0f);

  Text::Underline copy(original);

  DALI_TEST_EQUALS(copy.GetColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetThickness(), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetType(), Text::Underline::Type::DASHED, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDashLength(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDashGap(), 2.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetThickness(), 3.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUnderlineCopyAssignP(void)
{
  UiTestApplication application;

  Text::Underline original;
  original.SetColor(UiColor(Color::GREEN));
  original.SetThickness(4.0f);
  original.SetType(Text::Underline::Type::DOUBLE);
  original.SetDashLength(6.0f);
  original.SetDashGap(3.0f);

  Text::Underline copy;
  copy = original;

  DALI_TEST_EQUALS(copy.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetThickness(), 4.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetType(), Text::Underline::Type::DOUBLE, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDashLength(), 6.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDashGap(), 3.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetThickness(), 4.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUnderlineMoveCtorP(void)
{
  UiTestApplication application;

  Text::Underline original;
  original.SetColor(UiColor(Color::BLUE));
  original.SetThickness(2.5f);
  original.SetType(Text::Underline::Type::DASHED);
  original.SetDashLength(4.0f);
  original.SetDashGap(1.5f);

  Text::Underline moved(std::move(original));

  DALI_TEST_EQUALS(moved.GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetThickness(), 2.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetType(), Text::Underline::Type::DASHED, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetDashLength(), 4.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetDashGap(), 1.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUnderlineMoveAssignP(void)
{
  UiTestApplication application;

  Text::Underline original;
  original.SetColor(UiColor(Color::YELLOW));
  original.SetThickness(3.5f);
  original.SetType(Text::Underline::Type::DOUBLE);
  original.SetDashLength(7.0f);
  original.SetDashGap(2.5f);

  Text::Underline moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.GetColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetThickness(), 3.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetType(), Text::Underline::Type::DOUBLE, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetDashLength(), 7.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetDashGap(), 2.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUnderlineMovedFromGetN(void)
{
  UiTestApplication application;

  Text::Underline original;
  Text::Underline moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.GetColor(), "Cannot use a moved-from Underline object");

  END_TEST;
}

int UtcDaliTextUnderlineMovedFromSetN(void)
{
  UiTestApplication application;

  Text::Underline original;
  Text::Underline moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.SetColor(UiColor(Color::RED)), "Cannot use a moved-from Underline object");

  END_TEST;
}

int UtcDaliTextUnderlineCopyFromMovedN(void)
{
  UiTestApplication application;

  Text::Underline original;
  Text::Underline moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(Text::Underline copy(original), "Cannot use a moved-from Underline object");

  END_TEST;
}

int UtcDaliTextUnderlineCopyAssignFromMovedN(void)
{
  UiTestApplication application;

  Text::Underline original;
  Text::Underline moved(std::move(original));
  (void)moved;

  Text::Underline copy;

  DALI_TEST_ASSERTION(copy = original, "Cannot use a moved-from Underline object");

  END_TEST;
}

// Shadow Tests

int UtcDaliTextShadowColorP(void)
{
  UiTestApplication application;

  Text::Shadow shadow;

  shadow.SetColor(UiColor(Color::RED));
  DALI_TEST_EQUALS(shadow.GetColor().GetRgba(), Color::RED, TEST_LOCATION);

  shadow.SetColor(UiColor(Color::BLUE));
  DALI_TEST_EQUALS(shadow.GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextShadowOffsetP(void)
{
  UiTestApplication application;

  Text::Shadow shadow;

  shadow.SetOffset(Vector2(1.0f, 2.0f));
  DALI_TEST_EQUALS(shadow.GetOffset(), Vector2(1.0f, 2.0f), TEST_LOCATION);

  shadow.SetOffset(Vector2(3.0f, 4.0f));
  DALI_TEST_EQUALS(shadow.GetOffset(), Vector2(3.0f, 4.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextShadowBlurRadiusP(void)
{
  UiTestApplication application;

  Text::Shadow shadow;

  shadow.SetBlurRadius(2.5f);
  DALI_TEST_EQUALS(shadow.GetBlurRadius(), 2.5f, TEST_LOCATION);

  shadow.SetBlurRadius(5.0f);
  DALI_TEST_EQUALS(shadow.GetBlurRadius(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextShadowCopyCtorP(void)
{
  UiTestApplication application;

  Text::Shadow original;
  original.SetColor(UiColor(Color::RED));
  original.SetOffset(Vector2(1.0f, 2.0f));
  original.SetBlurRadius(3.0f);

  Text::Shadow copy(original);

  DALI_TEST_EQUALS(copy.GetColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetOffset(), Vector2(1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetBlurRadius(), 3.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextShadowMoveCtorP(void)
{
  UiTestApplication application;

  Text::Shadow original;
  original.SetColor(UiColor(Color::BLUE));
  original.SetOffset(Vector2(2.0f, 3.0f));
  original.SetBlurRadius(4.0f);

  Text::Shadow moved(std::move(original));

  DALI_TEST_EQUALS(moved.GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetOffset(), Vector2(2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetBlurRadius(), 4.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextShadowMovedFromGetN(void)
{
  UiTestApplication application;

  Text::Shadow original;
  Text::Shadow moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.GetColor(), "Cannot use a moved-from Shadow object");

  END_TEST;
}

int UtcDaliTextShadowMovedFromSetN(void)
{
  UiTestApplication application;

  Text::Shadow original;
  Text::Shadow moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.SetColor(UiColor(Color::RED)), "Cannot use a moved-from Shadow object");

  END_TEST;
}

int UtcDaliTextShadowCopyFromMovedN(void)
{
  UiTestApplication application;

  Text::Shadow original;
  Text::Shadow moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(Text::Shadow copy(original), "Cannot use a moved-from Shadow object");

  END_TEST;
}

int UtcDaliTextShadowCopyAssignP(void)
{
  UiTestApplication application;

  Text::Shadow original;
  original.SetColor(UiColor(Color::GREEN));
  original.SetOffset(Vector2(3.0f, 4.0f));
  original.SetBlurRadius(5.0f);

  Text::Shadow copy;
  copy = original;

  DALI_TEST_EQUALS(copy.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetOffset(), Vector2(3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetBlurRadius(), 5.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetOffset(), Vector2(3.0f, 4.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextShadowMoveAssignP(void)
{
  UiTestApplication application;

  Text::Shadow original;
  original.SetColor(UiColor(Color::YELLOW));
  original.SetOffset(Vector2(4.0f, 5.0f));
  original.SetBlurRadius(6.0f);

  Text::Shadow moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.GetColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetOffset(), Vector2(4.0f, 5.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetBlurRadius(), 6.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextShadowCopyAssignFromMovedN(void)
{
  UiTestApplication application;

  Text::Shadow original;
  Text::Shadow moved(std::move(original));
  (void)moved;

  Text::Shadow copy;

  DALI_TEST_ASSERTION(copy = original, "Cannot use a moved-from Shadow object");

  END_TEST;
}

// Outline Tests

int UtcDaliTextOutlineColorP(void)
{
  UiTestApplication application;

  Text::Outline outline;

  outline.SetColor(UiColor(Color::RED));
  DALI_TEST_EQUALS(outline.GetColor().GetRgba(), Color::RED, TEST_LOCATION);

  outline.SetColor(UiColor(Color::BLUE));
  DALI_TEST_EQUALS(outline.GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextOutlineWidthP(void)
{
  UiTestApplication application;

  Text::Outline outline;

  outline.SetWidth(2.5f);
  DALI_TEST_EQUALS(outline.GetWidth(), 2.5f, TEST_LOCATION);

  outline.SetWidth(5.0f);
  DALI_TEST_EQUALS(outline.GetWidth(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextOutlineCopyCtorP(void)
{
  UiTestApplication application;

  Text::Outline original;
  original.SetColor(UiColor(Color::RED));
  original.SetOffset(Vector2(1.0f, 2.0f));
  original.SetWidth(3.0f);

  Text::Outline copy(original);

  DALI_TEST_EQUALS(copy.GetColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetOffset(), Vector2(1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetWidth(), 3.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextOutlineMoveCtorP(void)
{
  UiTestApplication application;

  Text::Outline original;
  original.SetColor(UiColor(Color::BLUE));
  original.SetOffset(Vector2(2.0f, 3.0f));
  original.SetWidth(4.0f);

  Text::Outline moved(std::move(original));

  DALI_TEST_EQUALS(moved.GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetOffset(), Vector2(2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetWidth(), 4.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextOutlineMovedFromGetN(void)
{
  UiTestApplication application;

  Text::Outline original;
  Text::Outline moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.GetColor(), "Cannot use a moved-from Outline object");

  END_TEST;
}

int UtcDaliTextOutlineCopyFromMovedN(void)
{
  UiTestApplication application;

  Text::Outline original;
  Text::Outline moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(Text::Outline copy(original), "Cannot use a moved-from Outline object");

  END_TEST;
}

int UtcDaliTextOutlineOffsetP(void)
{
  UiTestApplication application;

  Text::Outline outline;

  outline.SetOffset(Vector2(1.0f, 2.0f));
  DALI_TEST_EQUALS(outline.GetOffset(), Vector2(1.0f, 2.0f), TEST_LOCATION);

  outline.SetOffset(Vector2(3.0f, 4.0f));
  DALI_TEST_EQUALS(outline.GetOffset(), Vector2(3.0f, 4.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextOutlineBlurRadiusP(void)
{
  UiTestApplication application;

  Text::Outline outline;

  outline.SetBlurRadius(2.5f);
  DALI_TEST_EQUALS(outline.GetBlurRadius(), 2.5f, TEST_LOCATION);

  outline.SetBlurRadius(5.0f);
  DALI_TEST_EQUALS(outline.GetBlurRadius(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextOutlineCopyAssignP(void)
{
  UiTestApplication application;

  Text::Outline original;
  original.SetColor(UiColor(Color::GREEN));
  original.SetOffset(Vector2(3.0f, 4.0f));
  original.SetWidth(5.0f);

  Text::Outline copy;
  copy = original;

  DALI_TEST_EQUALS(copy.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetOffset(), Vector2(3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetWidth(), 5.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetOffset(), Vector2(3.0f, 4.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextOutlineMoveAssignP(void)
{
  UiTestApplication application;

  Text::Outline original;
  original.SetColor(UiColor(Color::YELLOW));
  original.SetOffset(Vector2(4.0f, 5.0f));
  original.SetWidth(6.0f);

  Text::Outline moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.GetColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetOffset(), Vector2(4.0f, 5.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetWidth(), 6.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextOutlineMovedFromSetN(void)
{
  UiTestApplication application;

  Text::Outline original;
  Text::Outline moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.SetColor(UiColor(Color::RED)), "Cannot use a moved-from Outline object");

  END_TEST;
}

int UtcDaliTextOutlineCopyAssignFromMovedN(void)
{
  UiTestApplication application;

  Text::Outline original;
  Text::Outline moved(std::move(original));
  (void)moved;

  Text::Outline copy;

  DALI_TEST_ASSERTION(copy = original, "Cannot use a moved-from Outline object");

  END_TEST;
}

// LineThrough Tests

int UtcDaliTextLineThroughColorP(void)
{
  UiTestApplication application;

  Text::LineThrough lineThrough;

  lineThrough.SetColor(UiColor(Color::RED));
  DALI_TEST_EQUALS(lineThrough.GetColor().GetRgba(), Color::RED, TEST_LOCATION);

  lineThrough.SetColor(UiColor(Color::BLUE));
  DALI_TEST_EQUALS(lineThrough.GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLineThroughThicknessP(void)
{
  UiTestApplication application;

  Text::LineThrough lineThrough;

  lineThrough.SetThickness(2.5f);
  DALI_TEST_EQUALS(lineThrough.GetThickness(), 2.5f, TEST_LOCATION);

  lineThrough.SetThickness(5.0f);
  DALI_TEST_EQUALS(lineThrough.GetThickness(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLineThroughCopyCtorP(void)
{
  UiTestApplication application;

  Text::LineThrough original;
  original.SetColor(UiColor(Color::RED));
  original.SetThickness(3.0f);

  Text::LineThrough copy(original);

  DALI_TEST_EQUALS(copy.GetColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetThickness(), 3.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLineThroughMoveCtorP(void)
{
  UiTestApplication application;

  Text::LineThrough original;
  original.SetColor(UiColor(Color::BLUE));
  original.SetThickness(4.0f);

  Text::LineThrough moved(std::move(original));

  DALI_TEST_EQUALS(moved.GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetThickness(), 4.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLineThroughMovedFromGetN(void)
{
  UiTestApplication application;

  Text::LineThrough original;
  Text::LineThrough moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.GetColor(), "Cannot use a moved-from LineThrough object");

  END_TEST;
}

int UtcDaliTextLineThroughCopyFromMovedN(void)
{
  UiTestApplication application;

  Text::LineThrough original;
  Text::LineThrough moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(Text::LineThrough copy(original), "Cannot use a moved-from LineThrough object");

  END_TEST;
}

int UtcDaliTextLineThroughCopyAssignP(void)
{
  UiTestApplication application;

  Text::LineThrough original;
  original.SetColor(UiColor(Color::GREEN));
  original.SetThickness(4.0f);

  Text::LineThrough copy;
  copy = original;

  DALI_TEST_EQUALS(copy.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetThickness(), 4.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetThickness(), 4.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLineThroughMoveAssignP(void)
{
  UiTestApplication application;

  Text::LineThrough original;
  original.SetColor(UiColor(Color::YELLOW));
  original.SetThickness(5.0f);

  Text::LineThrough moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.GetColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetThickness(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLineThroughMovedFromSetN(void)
{
  UiTestApplication application;

  Text::LineThrough original;
  Text::LineThrough moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.SetColor(UiColor(Color::RED)), "Cannot use a moved-from LineThrough object");

  END_TEST;
}

int UtcDaliTextLineThroughCopyAssignFromMovedN(void)
{
  UiTestApplication application;

  Text::LineThrough original;
  Text::LineThrough moved(std::move(original));
  (void)moved;

  Text::LineThrough copy;

  DALI_TEST_ASSERTION(copy = original, "Cannot use a moved-from LineThrough object");

  END_TEST;
}

// Bevel Tests

int UtcDaliTextBevelDirectionP(void)
{
  UiTestApplication application;

  Text::Bevel bevel;

  bevel.SetDirection(Vector2(1.0f, 1.0f));
  DALI_TEST_EQUALS(bevel.GetDirection(), Vector2(1.0f, 1.0f), TEST_LOCATION);

  bevel.SetDirection(Vector2(-1.0f, -1.0f));
  DALI_TEST_EQUALS(bevel.GetDirection(), Vector2(-1.0f, -1.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextBevelIntensityP(void)
{
  UiTestApplication application;

  Text::Bevel bevel;

  bevel.SetIntensity(2.5f);
  DALI_TEST_EQUALS(bevel.GetIntensity(), 2.5f, TEST_LOCATION);

  bevel.SetIntensity(5.0f);
  DALI_TEST_EQUALS(bevel.GetIntensity(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextBevelCopyCtorP(void)
{
  UiTestApplication application;

  Text::Bevel original;
  original.SetDirection(Vector2(1.0f, 1.0f));
  original.SetIntensity(3.0f);
  original.SetLightColor(UiColor(Color::RED));
  original.SetShadowColor(UiColor(Color::BLUE));

  Text::Bevel copy(original);

  DALI_TEST_EQUALS(copy.GetDirection(), Vector2(1.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetIntensity(), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetLightColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetShadowColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextBevelMoveCtorP(void)
{
  UiTestApplication application;

  Text::Bevel original;
  original.SetDirection(Vector2(2.0f, 2.0f));
  original.SetIntensity(4.0f);
  original.SetLightColor(UiColor(Color::GREEN));
  original.SetShadowColor(UiColor(Color::YELLOW));

  Text::Bevel moved(std::move(original));

  DALI_TEST_EQUALS(moved.GetDirection(), Vector2(2.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetIntensity(), 4.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetLightColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetShadowColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextBevelMovedFromGetN(void)
{
  UiTestApplication application;

  Text::Bevel original;
  Text::Bevel moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.GetDirection(), "Cannot use a moved-from Bevel object");

  END_TEST;
}

int UtcDaliTextBevelCopyFromMovedN(void)
{
  UiTestApplication application;

  Text::Bevel original;
  Text::Bevel moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(Text::Bevel copy(original), "Cannot use a moved-from Bevel object");

  END_TEST;
}

int UtcDaliTextBevelLightColorP(void)
{
  UiTestApplication application;

  Text::Bevel bevel;

  bevel.SetLightColor(UiColor(Color::RED));
  DALI_TEST_EQUALS(bevel.GetLightColor().GetRgba(), Color::RED, TEST_LOCATION);

  bevel.SetLightColor(UiColor(Color::BLUE));
  DALI_TEST_EQUALS(bevel.GetLightColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextBevelShadowColorP(void)
{
  UiTestApplication application;

  Text::Bevel bevel;

  bevel.SetShadowColor(UiColor(Color::RED));
  DALI_TEST_EQUALS(bevel.GetShadowColor().GetRgba(), Color::RED, TEST_LOCATION);

  bevel.SetShadowColor(UiColor(Color::BLUE));
  DALI_TEST_EQUALS(bevel.GetShadowColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextBevelCopyAssignP(void)
{
  UiTestApplication application;

  Text::Bevel original;
  original.SetDirection(Vector2(2.0f, 2.0f));
  original.SetIntensity(4.0f);
  original.SetLightColor(UiColor(Color::GREEN));
  original.SetShadowColor(UiColor(Color::YELLOW));

  Text::Bevel copy;
  copy = original;

  DALI_TEST_EQUALS(copy.GetDirection(), Vector2(2.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetIntensity(), 4.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetLightColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetShadowColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetDirection(), Vector2(2.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetIntensity(), 4.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextBevelMoveAssignP(void)
{
  UiTestApplication application;

  Text::Bevel original;
  original.SetDirection(Vector2(3.0f, 3.0f));
  original.SetIntensity(5.0f);
  original.SetLightColor(UiColor(Color::CYAN));
  original.SetShadowColor(UiColor(Color::MAGENTA));

  Text::Bevel moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.GetDirection(), Vector2(3.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetIntensity(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetLightColor().GetRgba(), Color::CYAN, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetShadowColor().GetRgba(), Color::MAGENTA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextBevelMovedFromSetN(void)
{
  UiTestApplication application;

  Text::Bevel original;
  Text::Bevel moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.SetDirection(Vector2(1.0f, 1.0f)), "Cannot use a moved-from Bevel object");

  END_TEST;
}

int UtcDaliTextBevelCopyAssignFromMovedN(void)
{
  UiTestApplication application;

  Text::Bevel original;
  Text::Bevel moved(std::move(original));
  (void)moved;

  Text::Bevel copy;

  DALI_TEST_ASSERTION(copy = original, "Cannot use a moved-from Bevel object");

  END_TEST;
}

int UtcDaliTextStyleNoneEqualityP(void)
{
  UiTestApplication application;

  DALI_TEST_CHECK(Text::Underline::None() == Text::Underline::None());
  DALI_TEST_CHECK(Text::Shadow::None() == Text::Shadow::None());
  DALI_TEST_CHECK(Text::Outline::None() == Text::Outline::None());
  DALI_TEST_CHECK(Text::LineThrough::None() == Text::LineThrough::None());
  DALI_TEST_CHECK(Text::Bevel::None() == Text::Bevel::None());

  DALI_TEST_CHECK(Text::Underline() != Text::Underline::None());
  DALI_TEST_CHECK(Text::LineThrough() != Text::LineThrough::None());
  DALI_TEST_CHECK(Text::Bevel() != Text::Bevel::None());

  Text::Shadow zeroOffsetShadow;
  zeroOffsetShadow.SetOffset(Vector2::ZERO);
  DALI_TEST_CHECK(zeroOffsetShadow != Text::Shadow::None());

  Text::Outline zeroWidthOutline;
  zeroWidthOutline.SetWidth(0.0f);
  DALI_TEST_CHECK(zeroWidthOutline != Text::Outline::None());

  Text::Underline copiedNone(Text::Underline::None());
  DALI_TEST_CHECK(copiedNone == Text::Underline::None());

  Text::Shadow assignedNone;
  assignedNone = Text::Shadow::None();
  DALI_TEST_CHECK(assignedNone == Text::Shadow::None());

  END_TEST;
}

int UtcDaliTextStyleNoneAccessorN(void)
{
  UiTestApplication application;

  Text::Underline underline = Text::Underline::None();
  DALI_TEST_ASSERTION(Text::Underline::None().GetColor(), "Cannot access Text::Underline::None() properties.");
  DALI_TEST_ASSERTION(underline.SetColor(UiColor(Color::RED)), "Cannot modify Text::Underline::None().");

  Text::Shadow shadow = Text::Shadow::None();
  DALI_TEST_ASSERTION(Text::Shadow::None().GetOffset(), "Cannot access Text::Shadow::None() properties.");
  DALI_TEST_ASSERTION(shadow.SetOffset(Vector2(1.0f, 1.0f)), "Cannot modify Text::Shadow::None().");

  Text::Outline outline = Text::Outline::None();
  DALI_TEST_ASSERTION(Text::Outline::None().GetWidth(), "Cannot access Text::Outline::None() properties.");
  DALI_TEST_ASSERTION(outline.SetWidth(1.0f), "Cannot modify Text::Outline::None().");

  Text::LineThrough lineThrough = Text::LineThrough::None();
  DALI_TEST_ASSERTION(Text::LineThrough::None().GetColor(), "Cannot access Text::LineThrough::None() properties.");
  DALI_TEST_ASSERTION(lineThrough.SetColor(UiColor(Color::GREEN)), "Cannot modify Text::LineThrough::None().");

  Text::Bevel bevel = Text::Bevel::None();
  DALI_TEST_ASSERTION(Text::Bevel::None().GetDirection(), "Cannot access Text::Bevel::None() properties.");
  DALI_TEST_ASSERTION(bevel.SetDirection(Vector2(1.0f, 1.0f)), "Cannot modify Text::Bevel::None().");

  END_TEST;
}

int UtcDaliTextStyleEqualityAuthoredColorP(void)
{
  UiTestApplication application;

  Text::Underline tokenUnderlineA;
  tokenUnderlineA.SetColor(UiColor("Primary"));
  Text::Underline tokenUnderlineB;
  tokenUnderlineB.SetColor(UiColor("Primary"));
  Text::Underline rgbaUnderline;
  rgbaUnderline.SetColor(UiColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
  DALI_TEST_CHECK(tokenUnderlineA == tokenUnderlineB);
  DALI_TEST_CHECK(tokenUnderlineA != rgbaUnderline);

  Text::Shadow tokenShadowA;
  tokenShadowA.SetColor(UiColor("Primary").WithAlpha(0.5f));
  Text::Shadow tokenShadowB;
  tokenShadowB.SetColor(UiColor("Primary").WithAlpha(0.5f));
  Text::Shadow tokenShadowC;
  tokenShadowC.SetColor(UiColor("Primary").WithAlpha(0.7f));
  DALI_TEST_CHECK(tokenShadowA == tokenShadowB);
  DALI_TEST_CHECK(tokenShadowA != tokenShadowC);

  Text::Outline tokenOutlineA;
  tokenOutlineA.SetColor(UiColor("Outline"));
  Text::Outline tokenOutlineB;
  tokenOutlineB.SetColor(UiColor("Outline"));
  Text::Outline tokenOutlineC;
  tokenOutlineC.SetColor(UiColor("Primary"));
  DALI_TEST_CHECK(tokenOutlineA == tokenOutlineB);
  DALI_TEST_CHECK(tokenOutlineA != tokenOutlineC);

  Text::LineThrough tokenLineThroughA;
  tokenLineThroughA.SetColor(UiColor("Primary"));
  Text::LineThrough tokenLineThroughB;
  tokenLineThroughB.SetColor(UiColor("Primary"));
  DALI_TEST_CHECK(tokenLineThroughA == tokenLineThroughB);

  Text::Bevel tokenBevelA;
  tokenBevelA.SetLightColor(UiColor("Primary"));
  tokenBevelA.SetShadowColor(UiColor("Shadow").WithAlpha(0.5f));
  Text::Bevel tokenBevelB;
  tokenBevelB.SetLightColor(UiColor("Primary"));
  tokenBevelB.SetShadowColor(UiColor("Shadow").WithAlpha(0.5f));
  Text::Bevel tokenBevelC;
  tokenBevelC.SetLightColor(UiColor("Primary"));
  tokenBevelC.SetShadowColor(UiColor("Shadow").WithAlpha(0.7f));
  DALI_TEST_CHECK(tokenBevelA == tokenBevelB);
  DALI_TEST_CHECK(tokenBevelA != tokenBevelC);

  END_TEST;
}

// FontVariation::Axis Tests

int UtcDaliTextFontVariationNestedAxisTagP(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis axis;

  axis.SetTag("wght");
  DALI_TEST_EQUALS(axis.GetTag(), Dali::String("wght"), TEST_LOCATION);

  axis.SetTag("wdth");
  DALI_TEST_EQUALS(axis.GetTag(), Dali::String("wdth"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFontVariationNestedAxisValueP(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis axis;

  axis.SetValue(700.0f);
  DALI_TEST_EQUALS(axis.GetValue(), 700.0f, TEST_LOCATION);

  axis.SetValue(400.0f);
  DALI_TEST_EQUALS(axis.GetValue(), 400.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFontVariationNestedAxisCopyCtorP(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis original;
  original.SetTag("wght");
  original.SetValue(700.0f);

  Text::FontVariation::Axis copy(original);

  DALI_TEST_EQUALS(copy.GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetValue(), 700.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFontVariationNestedAxisMoveCtorP(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis original;
  original.SetTag("wdth");
  original.SetValue(90.0f);

  Text::FontVariation::Axis moved(std::move(original));

  DALI_TEST_EQUALS(moved.GetTag(), Dali::String("wdth"), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetValue(), 90.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFontVariationNestedAxisMovedFromGetN(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis original;
  Text::FontVariation::Axis moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.GetTag(), "Cannot use a moved-from FontVariation::Axis object");

  END_TEST;
}

int UtcDaliTextFontVariationNestedAxisCopyFromMovedN(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis original;
  Text::FontVariation::Axis moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(Text::FontVariation::Axis copy(original), "Cannot use a moved-from FontVariation::Axis object");

  END_TEST;
}

int UtcDaliTextFontVariationNestedAxisCopyAssignP(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis original;
  original.SetTag("wght");
  original.SetValue(600.0f);

  Text::FontVariation::Axis copy;
  copy = original;

  DALI_TEST_EQUALS(copy.GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetValue(), 600.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetTag(), Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetValue(), 600.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFontVariationNestedAxisMoveAssignP(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis original;
  original.SetTag("slnt");
  original.SetValue(100.0f);

  Text::FontVariation::Axis moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.GetTag(), Dali::String("slnt"), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetValue(), 100.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFontVariationNestedAxisMovedFromSetN(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis original;
  Text::FontVariation::Axis moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.SetTag("wght"), "Cannot use a moved-from FontVariation::Axis object");

  END_TEST;
}

int UtcDaliTextFontVariationNestedAxisCopyAssignFromMovedN(void)
{
  UiTestApplication application;

  Text::FontVariation::Axis original;
  Text::FontVariation::Axis moved(std::move(original));
  (void)moved;

  Text::FontVariation::Axis copy;

  DALI_TEST_ASSERTION(copy = original, "Cannot use a moved-from FontVariation::Axis object");

  END_TEST;
}

// Text::Fit::Range Tests

int UtcDaliTextFitRangeFontSizeP(void)
{
  UiTestApplication application;

  Text::Fit::Range fitRange;

  fitRange.SetMinimumFontSize(10.0f);
  DALI_TEST_EQUALS(fitRange.GetMinimumFontSize(), 10.0f, TEST_LOCATION);

  fitRange.SetMaximumFontSize(30.0f);
  DALI_TEST_EQUALS(fitRange.GetMaximumFontSize(), 30.0f, TEST_LOCATION);

  fitRange.SetFontSizeStep(5.0f);
  DALI_TEST_EQUALS(fitRange.GetFontSizeStep(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitRangeCopyCtorP(void)
{
  UiTestApplication application;

  Text::Fit::Range original;
  original.SetMinimumFontSize(12.0f);
  original.SetMaximumFontSize(24.0f);
  original.SetFontSizeStep(4.0f);

  Text::Fit::Range copy(original);

  DALI_TEST_EQUALS(copy.GetMinimumFontSize(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetMaximumFontSize(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetFontSizeStep(), 4.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitRangeMoveCtorP(void)
{
  UiTestApplication application;

  Text::Fit::Range original;
  original.SetMinimumFontSize(14.0f);
  original.SetMaximumFontSize(28.0f);
  original.SetFontSizeStep(2.0f);

  Text::Fit::Range moved(std::move(original));

  DALI_TEST_EQUALS(moved.GetMinimumFontSize(), 14.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetMaximumFontSize(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetFontSizeStep(), 2.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitRangeMovedFromGetN(void)
{
  UiTestApplication application;

  Text::Fit::Range original;
  Text::Fit::Range moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.GetMinimumFontSize(), "Cannot use a moved-from Text::Fit::Range object");

  END_TEST;
}

int UtcDaliTextFitRangeCopyFromMovedN(void)
{
  UiTestApplication application;

  Text::Fit::Range original;
  Text::Fit::Range moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(Text::Fit::Range copy(original), "Cannot use a moved-from Text::Fit::Range object");

  END_TEST;
}

int UtcDaliTextFitRangeCopyAssignP(void)
{
  UiTestApplication application;

  Text::Fit::Range original;
  original.SetMinimumFontSize(10.0f);
  original.SetMaximumFontSize(20.0f);
  original.SetFontSizeStep(2.0f);

  Text::Fit::Range copy;
  copy = original;

  DALI_TEST_EQUALS(copy.GetMinimumFontSize(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetMaximumFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetFontSizeStep(), 2.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetMinimumFontSize(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetMaximumFontSize(), 20.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitRangeMoveAssignP(void)
{
  UiTestApplication application;

  Text::Fit::Range original;
  original.SetMinimumFontSize(12.0f);
  original.SetMaximumFontSize(24.0f);
  original.SetFontSizeStep(3.0f);

  Text::Fit::Range moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.GetMinimumFontSize(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetMaximumFontSize(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetFontSizeStep(), 3.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitRangeMovedFromSetN(void)
{
  UiTestApplication application;

  Text::Fit::Range original;
  Text::Fit::Range moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.SetMinimumFontSize(10.0f), "Cannot use a moved-from Text::Fit::Range object");

  END_TEST;
}

int UtcDaliTextFitRangeCopyAssignFromMovedN(void)
{
  UiTestApplication application;

  Text::Fit::Range original;
  Text::Fit::Range moved(std::move(original));
  (void)moved;

  Text::Fit::Range copy;

  DALI_TEST_ASSERTION(copy = original, "Cannot use a moved-from Text::Fit::Range object");

  END_TEST;
}

// Text::Fit::Candidate Tests

int UtcDaliTextFitCandidateFontSizeP(void)
{
  UiTestApplication application;

  Text::Fit::Candidate candidate;

  candidate.SetFontSize(18.0f);
  DALI_TEST_EQUALS(candidate.GetFontSize(), 18.0f, TEST_LOCATION);

  candidate.SetFontSize(24.0f);
  DALI_TEST_EQUALS(candidate.GetFontSize(), 24.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitCandidateLineHeightP(void)
{
  UiTestApplication application;

  Text::Fit::Candidate candidate;

  candidate.SetLineHeight(1.5f);
  DALI_TEST_EQUALS(candidate.GetLineHeight(), 1.5f, TEST_LOCATION);

  candidate.SetLineHeight(2.0f);
  DALI_TEST_EQUALS(candidate.GetLineHeight(), 2.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitCandidateCopyCtorP(void)
{
  UiTestApplication application;

  Text::Fit::Candidate original;
  original.SetFontSize(20.0f);
  original.SetLineHeight(1.5f);

  Text::Fit::Candidate copy(original);

  DALI_TEST_EQUALS(copy.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetLineHeight(), 1.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitCandidateMoveCtorP(void)
{
  UiTestApplication application;

  Text::Fit::Candidate original;
  original.SetFontSize(22.0f);
  original.SetLineHeight(1.8f);

  Text::Fit::Candidate moved(std::move(original));

  DALI_TEST_EQUALS(moved.GetFontSize(), 22.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetLineHeight(), 1.8f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitCandidateMovedFromGetN(void)
{
  UiTestApplication application;

  Text::Fit::Candidate original;
  Text::Fit::Candidate moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.GetFontSize(), "Cannot use a moved-from Text::Fit::Candidate object");

  END_TEST;
}

int UtcDaliTextFitCandidateCopyFromMovedN(void)
{
  UiTestApplication application;

  Text::Fit::Candidate original;
  Text::Fit::Candidate moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(Text::Fit::Candidate copy(original), "Cannot use a moved-from Text::Fit::Candidate object");

  END_TEST;
}

int UtcDaliTextFitCandidateCopyAssignP(void)
{
  UiTestApplication application;

  Text::Fit::Candidate original;
  original.SetFontSize(18.0f);
  original.SetLineHeight(1.5f);

  Text::Fit::Candidate copy;
  copy = original;

  DALI_TEST_EQUALS(copy.GetFontSize(), 18.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetLineHeight(), 1.5f, TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetFontSize(), 18.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetLineHeight(), 1.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitCandidateMoveAssignP(void)
{
  UiTestApplication application;

  Text::Fit::Candidate original;
  original.SetFontSize(20.0f);
  original.SetLineHeight(2.0f);

  Text::Fit::Candidate moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetLineHeight(), 2.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitCandidateMovedFromSetN(void)
{
  UiTestApplication application;

  Text::Fit::Candidate original;
  Text::Fit::Candidate moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.SetFontSize(16.0f), "Cannot use a moved-from Text::Fit::Candidate object");

  END_TEST;
}

int UtcDaliTextFitCandidateCopyAssignFromMovedN(void)
{
  UiTestApplication application;

  Text::Fit::Candidate original;
  Text::Fit::Candidate moved(std::move(original));
  (void)moved;

  Text::Fit::Candidate copy;

  DALI_TEST_ASSERTION(copy = original, "Cannot use a moved-from Text::Fit::Candidate object");

  END_TEST;
}

// Text::Fit Tests

int UtcDaliTextFitNoneP(void)
{
  UiTestApplication application;

  Text::Fit fit;
  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::NONE);
  DALI_TEST_CHECK(Text::Fit::None().GetType() == Text::Fit::Type::NONE);

  END_TEST;
}

int UtcDaliTextFitFromRangeP(void)
{
  UiTestApplication application;

  Text::Fit::Range   range(10.0f, 40.0f, 2.0f);
  Text::Fit fit = Text::Fit::FromRange(range);

  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::RANGE);

  const Text::Fit::Range& result = fit.GetRange();
  DALI_TEST_EQUALS(result.GetMinimumFontSize(), 10.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result.GetMaximumFontSize(), 40.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result.GetFontSizeStep(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitFromCandidatesP(void)
{
  UiTestApplication application;

  Dali::Vector<Text::Fit::Candidate> candidates;
  candidates.PushBack(Text::Fit::Candidate(16.0f, 32.0f));
  candidates.PushBack(Text::Fit::Candidate(24.0f, 48.0f));

  Text::Fit fit = Text::Fit::FromCandidates(candidates);

  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::CANDIDATES);

  const Dali::Vector<Text::Fit::Candidate>& result = fit.GetCandidates();
  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetFontSize(), 16.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0].GetLineHeight(), 32.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetFontSize(), 24.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(result[1].GetLineHeight(), 48.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitFromEmptyCandidatesP(void)
{
  UiTestApplication application;

  Dali::Vector<Text::Fit::Candidate> candidates;
  Text::Fit                            fit = Text::Fit::FromCandidates(candidates);

  DALI_TEST_CHECK(fit.GetType() == Text::Fit::Type::NONE);

  END_TEST;
}

int UtcDaliTextFitCopyMoveP(void)
{
  UiTestApplication application;

  Text::Fit::Range   range(12.0f, 36.0f, 3.0f);
  Text::Fit original = Text::Fit::FromRange(range);
  Text::Fit copy(original);

  DALI_TEST_CHECK(copy.GetType() == Text::Fit::Type::RANGE);
  DALI_TEST_EQUALS(copy.GetRange().GetMaximumFontSize(), 36.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Text::Fit moved(std::move(copy));

  DALI_TEST_CHECK(moved.GetType() == Text::Fit::Type::RANGE);
  DALI_TEST_EQUALS(moved.GetRange().GetFontSizeStep(), 3.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFitWrongGetterN(void)
{
  UiTestApplication application;

  DALI_TEST_ASSERTION(Text::Fit::None().GetRange(), "Text::Fit does not contain a range.");

  Text::Fit::Range   range;
  Text::Fit          fit = Text::Fit::FromRange(range);
  DALI_TEST_ASSERTION(fit.GetCandidates(), "Text::Fit does not contain candidates.");

  END_TEST;
}

int UtcDaliTextFitMovedFromGetN(void)
{
  UiTestApplication application;

  Text::Fit original;
  Text::Fit moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.GetType(), "Cannot use a moved-from Text::Fit object");

  END_TEST;
}

// InputFilter Tests

int UtcDaliTextInputFilterP(void)
{
  UiTestApplication application;

  // Default construction: allow and deny patterns should be empty
  Text::InputFilter filter;
  DALI_TEST_EQUALS(filter.GetAllowPattern(), Dali::String(""), TEST_LOCATION);
  DALI_TEST_EQUALS(filter.GetDenyPattern(), Dali::String(""), TEST_LOCATION);
  DALI_TEST_CHECK(filter == Text::InputFilter::None());

  Text::InputFilter none = Text::InputFilter::None();
  DALI_TEST_EQUALS(none.GetAllowPattern(), Dali::String(""), TEST_LOCATION);
  DALI_TEST_EQUALS(none.GetDenyPattern(), Dali::String(""), TEST_LOCATION);
  DALI_TEST_CHECK(none == filter);

  // SetAllowPattern
  filter.SetAllowPattern("[\\d]");
  DALI_TEST_EQUALS(filter.GetAllowPattern(), Dali::String("[\\d]"), TEST_LOCATION);
  DALI_TEST_CHECK(filter != Text::InputFilter::None());

  // SetDenyPattern
  filter.SetDenyPattern("[0-5]");
  DALI_TEST_EQUALS(filter.GetDenyPattern(), Dali::String("[0-5]"), TEST_LOCATION);
  DALI_TEST_CHECK(filter != Text::InputFilter::None());

  END_TEST;
}

int UtcDaliTextInputFilterCopyCtorP(void)
{
  UiTestApplication application;

  Text::InputFilter original;
  original.SetAllowPattern("[\\d]");
  original.SetDenyPattern("[0-5]");

  Text::InputFilter copy(original);

  DALI_TEST_EQUALS(copy.GetAllowPattern(), Dali::String("[\\d]"), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDenyPattern(), Dali::String("[0-5]"), TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetAllowPattern(), Dali::String("[\\d]"), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetDenyPattern(), Dali::String("[0-5]"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextInputFilterCopyAssignP(void)
{
  UiTestApplication application;

  Text::InputFilter original;
  original.SetAllowPattern("[a-z]");
  original.SetDenyPattern("[aeiou]");

  Text::InputFilter copy;
  copy = original;

  DALI_TEST_EQUALS(copy.GetAllowPattern(), Dali::String("[a-z]"), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDenyPattern(), Dali::String("[aeiou]"), TEST_LOCATION);

  DALI_TEST_EQUALS(original.GetAllowPattern(), Dali::String("[a-z]"), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetDenyPattern(), Dali::String("[aeiou]"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextInputFilterMoveCtorP(void)
{
  UiTestApplication application;

  Text::InputFilter original;
  original.SetAllowPattern("[\\d]");
  original.SetDenyPattern("[0-5]");

  Text::InputFilter moved(std::move(original));

  DALI_TEST_EQUALS(moved.GetAllowPattern(), Dali::String("[\\d]"), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetDenyPattern(), Dali::String("[0-5]"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextInputFilterMoveAssignP(void)
{
  UiTestApplication application;

  Text::InputFilter original;
  original.SetAllowPattern("[A-Z]");
  original.SetDenyPattern("[AEIOU]");

  Text::InputFilter moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.GetAllowPattern(), Dali::String("[A-Z]"), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetDenyPattern(), Dali::String("[AEIOU]"), TEST_LOCATION);

  END_TEST;
}
