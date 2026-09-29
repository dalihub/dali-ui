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

#include <dali-ui-foundation/integration-api/text/input-style.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/multi-language-support.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-processing-source.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Text;

void utc_dali_text_logical_model_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_logical_model_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextVariationsDefaultAndValidationP(void)
{
  UiTestApplication application;
  auto              controller = Controller::New();
  auto&             logical    = *Controller::Impl::GetImplementation(*controller.Get()).mModel->mLogicalModel;
  DALI_TEST_CHECK(!logical.mVariationsMap);
  DALI_TEST_CHECK(controller->GetVariations().Empty());
  Property::Map output;
  output.Insert("wght", 500.0f);
  controller->GetVariationsMap(output);
  DALI_TEST_CHECK(output.Empty());
  controller->ClearVariationsMap();
  controller->SetText("Hello");
  controller->Relayout(Size(200.0f, 60.0f));
  DALI_TEST_CHECK(!logical.mVariationsMap);

  Vector<FontVariation::Axis> invalid;
  invalid.PushBack(FontVariation::Axis("bad", 400.0f));
  invalid.PushBack(FontVariation::Axis("wght", std::numeric_limits<float>::quiet_NaN()));
  invalid.PushBack(FontVariation::Axis("wdth", std::numeric_limits<float>::infinity()));
  controller->SetVariations(invalid);
  Property::Map invalidMap;
  invalidMap.Insert("bad", 400.0f);
  invalidMap.Insert("wght", "not a number");
  invalidMap.Insert(123, 500.0f);
  controller->SetVariationsMap(invalidMap);
  DALI_TEST_CHECK(!logical.mVariationsMap);

  Vector<FontVariation::Axis> axes;
  axes.PushBack(FontVariation::Axis("wght", 500.0f));
  axes.PushBack(FontVariation::Axis("wdth", 90.0f));
  controller->SetVariations(axes);
  DALI_TEST_EQUALS(controller->GetVariations().Count(), 2u, TEST_LOCATION);
  auto* storage = logical.GetVariationsMap();
  DALI_TEST_CHECK(storage);
  auto* impl = storage->Read();
  controller->GetVariationsMap(output);
  DALI_TEST_CHECK(output == *storage);
  controller->SetVariations(invalid); // Invalid setter still clears previous content.
  DALI_TEST_CHECK(logical.mVariationsMap.has_value());
  DALI_TEST_CHECK(!logical.GetVariationsMap());
  DALI_TEST_CHECK(!static_cast<const LogicalModel&>(logical).GetVariationsMap());
  controller->GetVariationsMap(output);
  DALI_TEST_CHECK(output.Empty());
  controller->SetVariations(axes);
  DALI_TEST_CHECK(logical.GetVariationsMap() == storage);
  DALI_TEST_CHECK(storage->Read() == impl);

  // The Map API accepts non-finite float values; the axes API above rejects them.
  Property::Map nonFinite;
  nonFinite.Insert("wght", std::numeric_limits<float>::quiet_NaN());
  nonFinite.Insert("wdth", std::numeric_limits<float>::infinity());
  controller->SetVariationsMap(nonFinite);
  controller->GetVariationsMap(output);
  float value = 0.0f;
  DALI_TEST_CHECK(output.Find("wght")->Get(value) && std::isnan(value));
  DALI_TEST_CHECK(output.Find("wdth")->Get(value) && std::isinf(value));
  controller->SetVariationsMap(invalidMap);
  DALI_TEST_CHECK(!logical.GetVariationsMap());
  DALI_TEST_CHECK(logical.mVariationsMap->Read() == impl);
  END_TEST;
}

int UtcDaliTextVariationsReplacementCopyP(void)
{
  UiTestApplication application;
  auto              source = Model::New();
  auto              target = Model::New();
  CopyTextProcessingProperties(*source, *target);
  DALI_TEST_CHECK(!source->mLogicalModel->mVariationsMap);
  DALI_TEST_CHECK(!target->mLogicalModel->mVariationsMap);

  Property::Map values;
  values.Insert("wght", 450.0f);
  values.Insert("wdth", 90.0f);
  source->mLogicalModel->SetVariationsMap(&values);
  CopyTextProcessingProperties(*source, *target);
  auto* targetMap  = target->mLogicalModel->GetVariationsMap();
  auto* targetImpl = targetMap->Read();
  DALI_TEST_CHECK(*targetMap == values);
  DALI_TEST_CHECK(targetMap->Read() != source->mLogicalModel->GetVariationsMap()->Read());
  (*source->mLogicalModel->GetVariationsMap())["wght"] = 700.0f;
  DALI_TEST_CHECK(*targetMap == values);

  source->mLogicalModel->SetVariationsMap(nullptr);
  CopyTextProcessingProperties(*source, *target);
  DALI_TEST_CHECK(!target->mLogicalModel->GetVariationsMap());
  DALI_TEST_CHECK(target->mLogicalModel->mVariationsMap->Read() == targetImpl);
  auto fresh = Model::New();
  CopyTextProcessingProperties(*source, *fresh); // Engaged-empty also copies as empty.
  DALI_TEST_CHECK(!fresh->mLogicalModel->mVariationsMap);
  source->mLogicalModel->SetVariationsMap(&values);
  CopyTextProcessingProperties(*source, *target);
  DALI_TEST_CHECK(target->mLogicalModel->GetVariationsMap() == targetMap);
  DALI_TEST_CHECK(targetMap->Read() == targetImpl);
  DALI_TEST_CHECK(*targetMap == values);
  END_TEST;
}

int UtcDaliTextVariationsFontCacheP(void)
{
  UiTestApplication application;
  auto              logical = LogicalModel::New();
  for(char c : std::string("Hello")) logical->mText.PushBack(c);
  auto support = MultilanguageSupport::New(false);
  auto client  = TextAbstraction::FontClient::Get();
  support.SetScripts(logical->mText, 0u, logical->mText.Count(), logical->mScriptRuns);
  TextAbstraction::FontDescription description;
  description.family  = "DejaVu Sans";
  const auto size     = 12u * client.GetNumberOfPointsPerOneUnitOfPointSize();
  const auto validate = [&](Property::Map* variations)
  {
    Vector<FontRun> fonts;
    support.ValidateFonts(client, logical->mText, logical->mScriptRuns, logical->mFontDescriptionRuns,
                          description, size, 1.0f, 0u, logical->mText.Count(), fonts, variations);
    return fonts.Empty() ? 0u : fonts[0].fontId;
  };
  const auto defaultFont = validate(logical->GetVariationsMap());
  DALI_TEST_CHECK(defaultFont != 0u);
  DALI_TEST_EQUALS(defaultFont, client.GetFontId(description, size, 0u, nullptr), TEST_LOCATION);
  DALI_TEST_CHECK(!logical->mVariationsMap);

  Property::Map reference;
  reference.Insert("wght", 450.0f);
  logical->SetVariationsMap(&reference);
  auto*      map       = logical->GetVariationsMap();
  const auto firstHash = map->GetHash();
  DALI_TEST_EQUALS(firstHash, reference.GetHash(), TEST_LOCATION);
  const auto variedFont = validate(map);
  DALI_TEST_EQUALS(variedFont, client.GetFontId(description, size, 0u, &reference), TEST_LOCATION);
  DALI_TEST_EQUALS(validate(map), variedFont, TEST_LOCATION);
  logical->SetVariationsMap(nullptr);
  DALI_TEST_CHECK(!logical->GetVariationsMap());
  DALI_TEST_EQUALS(validate(logical->GetVariationsMap()), defaultFont, TEST_LOCATION);

  reference["wght"] = 700.0f;
  logical->SetVariationsMap(&reference);
  DALI_TEST_CHECK(logical->GetVariationsMap() == map);
  DALI_TEST_CHECK(map->GetHash() != firstHash);
  const auto otherFont = validate(map);
  DALI_TEST_EQUALS(otherFont, client.GetFontId(description, size, 0u, &reference), TEST_LOCATION);
  DALI_TEST_EQUALS(validate(map), otherFont, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextVariationsAsyncWorkerReuseP(void)
{
  UiTestApplication application;
  auto              controller = Controller::New();
  Property::Map     values;
  values.Insert("wght", 450.0f);
  values.Insert("wdth", 90.0f);
  controller->SetVariationsMap(values);
  AsyncTextParameters request;
  request.text       = "Hello";
  request.fontFamily = "DejaVu Sans";
  request.fontSize   = 16.0f;
  request.textWidth  = 200.0f;
  request.textHeight = 60.0f;
  controller->GetVariationsMap(request.variationsMap);
  controller->ClearVariationsMap();
  DALI_TEST_CHECK(request.variationsMap == values);
  auto loader = AsyncTextLoader::New();
  for(int pass = 0; pass < 4; ++pass)
  {
    if(pass == 1) request.variationsMap.Clear();
    if(pass == 2) request.variationsMap.Insert("wght", 700.0f);
    if(pass == 3)
    {
      ReplacementRunSnapshot replacement;
      replacement.logicalCharacterRange = {1u, 1u};
      replacement.metrics.width         = 20.0f;
      replacement.metrics.height        = 18.0f;
      replacement.occurrenceIdentity    = 1u;
      request.replacementSourceSnapshot.runs.PushBack(replacement);
      request.replacementSourceSnapshot.hasValidReplacementSource = true;
    }
    auto freshLoader  = AsyncTextLoader::New();
    auto freshRequest = request;
    auto expected     = freshLoader.GetNaturalSize(freshRequest);
    auto actual       = loader.GetNaturalSize(request);
    DALI_TEST_EQUALS(actual.renderedSize, expected.renderedSize, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(actual.lineCount, expected.lineCount, TEST_LOCATION);
    if(pass == 3)
    {
      auto* state = Text::GetImplementation(loader).GetReplacementRenderState();
      DALI_TEST_CHECK(state && state->processingModel);
      auto* copied = state->processingModel->mLogicalModel->GetVariationsMap();
      DALI_TEST_CHECK(copied && *copied == request.variationsMap);
      DALI_TEST_CHECK(copied->Read() != request.variationsMap.Read());
    }
  }
  END_TEST;
}

int UtcDaliTextVariationsAsyncPendingP(void)
{
  UiTestApplication application;
  TextAbstraction::FontClient::Get();
  auto label = Label::New();
  label.SetRequestedWidth(200.0f);
  label.SetRequestedHeight(60.0f);
  label.SetStyledText(StyledText::FromMarkup("<b>Hello</b>"));
  label.SetAsyncRendering(true);
  application.GetScene().Add(label);
  ConnectionTracker tracker;
  unsigned int      completed = 0u;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Dali::Ui::View, float, float)
  { ++completed; });
  const auto drain = [&]()
  { application.SendNotification(); application.Render(); };
  label.SetFontVariation("wght=450,wdth=90");
  drain(); // Submit before changing the owner while its value snapshot is pending.
  label.SetFontVariation(FontVariation::None());
  label.SetFontVariation("wght=700");
  drain();
  for(int attempt = 0; attempt < 4 && !completed; ++attempt)
  {
    Test::WaitForEventThreadTrigger(1, 5);
    drain();
  }
  DALI_TEST_CHECK(completed > 0u);
  DALI_TEST_EQUALS(label.GetFontVariation().Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetFontVariation()[0].GetValue(), 700.0f, TEST_LOCATION);
  label.SetFontVariation("malformed");
  label.SetFontVariation("");
  DALI_TEST_EQUALS(label.GetFontVariation()[0].GetValue(), 700.0f, TEST_LOCATION);
  completed = 0u;
  label.SetFontVariation(FontVariation::None());
  drain();
  for(int attempt = 0; attempt < 4 && !completed; ++attempt)
  {
    Test::WaitForEventThreadTrigger(1, 5);
    drain();
  }
  DALI_TEST_CHECK(completed > 0u);
  DALI_TEST_CHECK(label.GetFontVariation().Empty());
  label.SetFontVariation("wght=550");
  drain();
  label.Unparent();
  label.Reset(); // Pending work owns values, not this Label's optional map.
  drain();
  END_TEST;
}

int UtcDaliTextLogicalModelRunsAndStyleP(void)
{
  UiTestApplication application;
  LogicalModelPtr model = LogicalModel::New();
  model->mText.PushBack('a');
  model->mText.PushBack('b');
  model->mText.PushBack('c');
  model->mText.PushBack('d');

  ScriptRun script{{1u, 2u}, TextAbstraction::LATIN, false};
  model->mScriptRuns.PushBack(script);
  DALI_TEST_EQUALS(static_cast<int>(model->GetScript(1u)), static_cast<int>(TextAbstraction::LATIN), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(model->GetScript(3u)), static_cast<int>(TextAbstraction::UNKNOWN), TEST_LOCATION);
  DALI_TEST_CHECK(!model->GetCharacterDirection(10u));
  model->mCharacterDirections.PushBack(false);
  model->mCharacterDirections.PushBack(true);
  DALI_TEST_CHECK(model->GetCharacterDirection(1u));

  ColorRun color;
  color.characterRun = {0u, 3u};
  color.color = Color::RED;
  model->mColorRuns.PushBack(color);

  char* family = new char[5];
  std::memcpy(family, "Sans", 5u);
  FontDescriptionRun font({0u, 3u}, family, 4u,
                          TextAbstraction::FontWeight::BOLD,
                          TextAbstraction::FontWidth::CONDENSED,
                          TextAbstraction::FontSlant::ITALIC,
                          12 * 64, true, true, true, true, true);
  model->mFontDescriptionRuns.PushBack(font);
  Ui::Integration::Text::InputStyle style;
  model->RetrieveStyle(1u, style);
  DALI_TEST_CHECK(!style.isDefaultColor);
  DALI_TEST_EQUALS(style.textColor, Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(style.familyName, std::string("Sans"), TEST_LOCATION);
  DALI_TEST_CHECK(style.isWeightDefined && style.isWidthDefined && style.isSlantDefined && style.isSizeDefined);
  DALI_TEST_EQUALS(style.size, 12.0f, 0.001f, TEST_LOCATION);

  model->UpdateTextStyleRuns(1u, 1);
  model->UpdateTextStyleRuns(1u, -1);
  model->ClearStrikethroughRuns();
  model->ClearUnderlineRuns();
  model->ClearFontDescriptionRuns();
  END_TEST;
}

int UtcDaliTextLogicalModelParagraphAndBidiP(void)
{
  UiTestApplication application;
  LogicalModelPtr model = LogicalModel::New();
  for(uint32_t i = 0u; i < 8u; ++i)
  {
    model->mText.PushBack('a' + i);
    model->mCharacterDirections.PushBack(i % 2u != 0u);
    model->mLineBreakInfo.PushBack((i == 2u || i == 5u || i == 7u)
                                    ? TextAbstraction::LINE_MUST_BREAK
                                    : TextAbstraction::LINE_NO_BREAK);
  }
  model->CreateParagraphInfo(0u, 8u);
  DALI_TEST_EQUALS(model->mParagraphInfo.Count(), 3u, TEST_LOCATION);
  Vector<ParagraphRunIndex> paragraphs;
  model->FindParagraphs(1u, 5u, paragraphs);
  DALI_TEST_EQUALS(paragraphs.Count(), 2u, TEST_LOCATION);
  model->CreateParagraphInfo(3u, 3u);

  BidirectionalLineInfoRun first{};
  first.characterRun = {0u, 4u};
  first.visualToLogicalMap = static_cast<CharacterIndex*>(std::calloc(4u, sizeof(CharacterIndex)));
  first.visualToLogicalMap[0] = 3u;
  first.visualToLogicalMap[1] = 2u;
  first.visualToLogicalMap[2] = 1u;
  first.visualToLogicalMap[3] = 0u;
  first.direction = true;
  first.isIdentity = false;
  model->mBidirectionalLineInfo.PushBack(first);

  BidirectionalLineInfoRun second{};
  second.characterRun = {6u, 2u};
  second.visualToLogicalMap = static_cast<CharacterIndex*>(std::calloc(2u, sizeof(CharacterIndex)));
  second.visualToLogicalMap[0] = 0u;
  second.visualToLogicalMap[1] = 1u;
  second.direction = false;
  second.isIdentity = true;
  model->mBidirectionalLineInfo.PushBack(second);

  DALI_TEST_CHECK(model->FetchBidirectionalLineInfo(1u));
  DALI_TEST_EQUALS(model->GetLogicalCharacterIndex(0u), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetLogicalCursorIndex(0u), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetLogicalCursorIndex(4u), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!model->FetchBidirectionalLineInfo(5u));
  DALI_TEST_CHECK(model->FetchBidirectionalLineInfo(6u));
  DALI_TEST_EQUALS(model->GetBidirectionalLineInfo(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!model->FetchBidirectionalLineInfo(20u));

  BoundedParagraphRun bounded;
  bounded.characterRun = {0u, 2u};
  model->mBoundedParagraphRuns.PushBack(bounded);
  CharacterSpacingCharacterRun spacing;
  spacing.characterRun = {0u, 2u};
  model->mCharacterSpacingCharacterRuns.PushBack(spacing);
  DALI_TEST_EQUALS(model->GetNumberOfBoundedParagraphRuns(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetBoundedParagraphRuns().Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetNumberOfCharacterSpacingCharacterRuns(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetCharacterSpacingCharacterRuns().Count(), 1u, TEST_LOCATION);
  model->ClearAnchors();
  END_TEST;
}
