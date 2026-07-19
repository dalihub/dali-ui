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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/replacement/replacement-edit-normalizer.h>
#include <dali-ui-foundation/internal/text/styled-text/editable-styled-text-data.h>
#include <dali-ui-foundation/public-api/text/style/image-attributes.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Text::ReplacementRunSnapshot Run(Text::CharacterIndex start, Text::Length length, uint64_t identity)
{
  Text::ReplacementRunSnapshot run;
  run.logicalCharacterRange = {start, length};
  run.occurrenceIdentity    = identity;
  return run;
}

Vector<Text::ReplacementRunSnapshot> AdjacentRuns()
{
  Vector<Text::ReplacementRunSnapshot> runs;
  runs.PushBack(Run(2u, 3u, 1u));
  runs.PushBack(Run(5u, 2u, 2u));
  return runs;
}
} // unnamed namespace

void utc_dali_replacement_edit_normalizer_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_replacement_edit_normalizer_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliReplacementEditNormalizerBoundaryP(void)
{
  const auto runs = AdjacentRuns();
  DALI_TEST_EQUALS(Text::ReplacementEditNormalizer::NormalizeBoundary(
                     runs, 3u, Text::ReplacementEditNormalizer::BoundaryAffinity::LEADING, 9u),
                   2u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Text::ReplacementEditNormalizer::NormalizeBoundary(
                     runs, 3u, Text::ReplacementEditNormalizer::BoundaryAffinity::TRAILING, 9u),
                   5u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Text::ReplacementEditNormalizer::NormalizeBoundary(
                     runs, 5u, Text::ReplacementEditNormalizer::BoundaryAffinity::LEADING, 9u),
                   5u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Text::ReplacementEditNormalizer::NormalizeBoundary(
                     runs, 99u, Text::ReplacementEditNormalizer::BoundaryAffinity::TRAILING, 9u),
                   9u,
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliReplacementEditNormalizerSelectionP(void)
{
  const auto           runs  = AdjacentRuns();
  Text::CharacterIndex start = 3u;
  Text::CharacterIndex end   = 6u;
  Text::ReplacementEditNormalizer::NormalizeSelection(runs, start, end, 9u);
  DALI_TEST_EQUALS(start, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(end, 7u, TEST_LOCATION);

  start = 6u;
  end   = 3u;
  Text::ReplacementEditNormalizer::NormalizeSelection(runs, start, end, 9u);
  DALI_TEST_EQUALS(start, 7u, TEST_LOCATION);
  DALI_TEST_EQUALS(end, 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliReplacementEditNormalizerDeletionP(void)
{
  const auto         runs     = AdjacentRuns();
  Text::CharacterRun deletion = Text::ReplacementEditNormalizer::GetDeletionRange(runs, 5u, true, 9u);
  DALI_TEST_EQUALS(deletion.characterIndex, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(deletion.numberOfCharacters, 3u, TEST_LOCATION);

  deletion = Text::ReplacementEditNormalizer::GetDeletionRange(runs, 5u, false, 9u);
  DALI_TEST_EQUALS(deletion.characterIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(deletion.numberOfCharacters, 2u, TEST_LOCATION);

  deletion = Text::ReplacementEditNormalizer::NormalizeDeletion(runs, {4u, 2u}, 9u);
  DALI_TEST_EQUALS(deletion.characterIndex, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(deletion.numberOfCharacters, 5u, TEST_LOCATION);

  deletion = Text::ReplacementEditNormalizer::NormalizeDeletion(runs, {99u, 2u}, 9u);
  DALI_TEST_EQUALS(deletion.numberOfCharacters, 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliReplacementEditNormalizerRangeUpdateP(void)
{
  auto runs = AdjacentRuns();
  Text::ReplacementEditNormalizer::ApplyEdit(runs, 2u, 3u, 0u);
  DALI_TEST_EQUALS(runs.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(runs[0u].logicalCharacterRange.characterIndex, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(runs[0u].logicalCharacterRange.numberOfCharacters, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(runs[0u].occurrenceIdentity, 2u, TEST_LOCATION);

  Text::ReplacementEditNormalizer::ApplyEdit(runs, 2u, 0u, 4u);
  DALI_TEST_EQUALS(runs[0u].logicalCharacterRange.characterIndex, 6u, TEST_LOCATION);

  Text::ReplacementEditNormalizer::ApplyEdit(runs, 9u, 0u, 2u);
  DALI_TEST_EQUALS(runs[0u].logicalCharacterRange.characterIndex, 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(runs[0u].occurrenceIdentity, 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliReplacementEditNormalizerStyledAttachmentUpdateP(void)
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("0123456789abcde");
  builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(0xFF0000)), 5u, 10u);
  builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes("unused.png", Vector2(20.0f, 20.0f))), 12u, 14u);

  Text::EditableStyledTextData data;
  data.Set(builder.Build());
  data.ApplyEdit(2u, 5u, 0u);
  data.ApplyEdit(7u, 0u, 3u);

  Text::StyledText snapshot = data.Build("01789abXYZcde");
  DALI_TEST_EQUALS(snapshot.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.GetSpanStartIndexAt(0u), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.GetSpanEndIndexAt(0u), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.GetSpanStartIndexAt(1u), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.GetSpanEndIndexAt(1u), 12u, TEST_LOCATION);

  data.ApplyEdit(10u, 2u, 0u);
  snapshot = data.Build("01789abXYZe");
  DALI_TEST_EQUALS(snapshot.GetSpanCount(), 1u, TEST_LOCATION);

  Text::StyledTextBuilder canonicalBuilder = Text::StyledTextBuilder::New("A\uFFFCB");
  canonicalBuilder.SetSpan(
    Text::ImageSpan::New(Text::ImageAttributes("unused.png", Vector2(12.0f, 12.0f))), 1u, 2u);
  data.Set(canonicalBuilder.Build());
  snapshot = data.Build("A\uFFFCB");
  DALI_TEST_EQUALS(snapshot.GetSpanStartIndexAt(0u), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.GetSpanEndIndexAt(0u), 2u, TEST_LOCATION);
  END_TEST;
}
