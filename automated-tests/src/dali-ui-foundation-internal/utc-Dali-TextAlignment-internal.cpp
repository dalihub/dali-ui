/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali-ui-test-suite-utils.h>

#include <dali-ui-foundation/internal/text/text-alignment.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float UNALIGNED = -123.0f;

Text::LineRun MakeLine(Text::CharacterIndex start,
                       Text::Length         length,
                       float                width,
                       bool                 rightToLeft,
                       float                extraLength = 0.0f)
{
  Text::LineRun line;
  line.characterRun.characterIndex     = start;
  line.characterRun.numberOfCharacters = length;
  line.width                            = width;
  line.extraLength                      = extraLength;
  line.alignmentOffset                  = UNALIGNED;
  line.direction                        = rightToLeft;
  return line;
}

void ResetOffsets(Vector<Text::LineRun>& lines)
{
  for(Text::LineRun& line : lines)
  {
    line.alignmentOffset = UNALIGNED;
  }
}
} // unnamed namespace

int UtcDaliTextAlignmentBoundedParagraphRangesP(void)
{
  Text::ModelPtr model = Text::Model::New();
  model->mLogicalModel->mText.Resize(12u);

  Text::Layout::Engine engine;
  engine.SetLayout(Text::Layout::Engine::MULTI_LINE_BOX);

  Vector<Text::LineRun> lines;
  lines.PushBack(MakeLine(0u, 4u, 40.0f, false));
  lines.PushBack(MakeLine(4u, 4u, 50.0f, true, 4.0f));
  lines.PushBack(MakeLine(8u, 4u, 60.0f, false));

  float minimumOffset = 0.0f;
  model->mHorizontalAlignment = Text::Alignment::CENTER;
  Text::AlignTextLines(engine,
                       Size(100.0f, 40.0f),
                       0u,
                       12u,
                       *model,
                       lines,
                       minimumOffset,
                       LayoutDirection::LEFT_TO_RIGHT,
                       false);
  DALI_TEST_EQUALS(lines[0u].alignmentOffset, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[1u].alignmentOffset, 21.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[2u].alignmentOffset, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(minimumOffset, 20.0f, TEST_LOCATION);

  model->mHorizontalAlignment = Text::Alignment::START;
  ResetOffsets(lines);
  Text::AlignTextLines(engine,
                       Size(100.0f, 40.0f),
                       0u,
                       12u,
                       *model,
                       lines,
                       minimumOffset,
                       LayoutDirection::RIGHT_TO_LEFT,
                       true);
  DALI_TEST_EQUALS(lines[0u].alignmentOffset, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[1u].alignmentOffset, 46.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[2u].alignmentOffset, 40.0f, TEST_LOCATION);

  model->mHorizontalAlignment = Text::Alignment::END;
  ResetOffsets(lines);
  Text::AlignTextLines(engine,
                       Size(100.0f, 40.0f),
                       0u,
                       12u,
                       *model,
                       lines,
                       minimumOffset,
                       LayoutDirection::RIGHT_TO_LEFT,
                       true);
  DALI_TEST_EQUALS(lines[0u].alignmentOffset, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[1u].alignmentOffset, -4.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[2u].alignmentOffset, 0.0f, TEST_LOCATION);

  Text::BoundedParagraphRun center;
  center.characterRun               = {4u, 4u};
  center.horizontalAlignment        = Text::Alignment::CENTER;
  center.horizontalAlignmentDefined = true;
  model->mLogicalModel->mBoundedParagraphRuns.PushBack(center);
  Text::BoundedParagraphRun end;
  end.characterRun               = {8u, 4u};
  end.horizontalAlignment        = Text::Alignment::END;
  end.horizontalAlignmentDefined = true;
  model->mLogicalModel->mBoundedParagraphRuns.PushBack(end);
  model->mHorizontalAlignment = Text::Alignment::START;

  ResetOffsets(lines);
  Text::AlignTextLines(engine,
                       Size(100.0f, 40.0f),
                       0u,
                       12u,
                       *model,
                       lines,
                       minimumOffset,
                       LayoutDirection::LEFT_TO_RIGHT,
                       false);
  DALI_TEST_EQUALS(lines[0u].alignmentOffset, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[1u].alignmentOffset, 21.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[2u].alignmentOffset, 40.0f, TEST_LOCATION);
  // Bounded-paragraph traversal leaves the offset from its last range.
  DALI_TEST_EQUALS(minimumOffset, 40.0f, TEST_LOCATION);

  ResetOffsets(lines);
  Text::AlignTextLines(engine,
                       Size(100.0f, 40.0f),
                       4u,
                       4u,
                       *model,
                       lines,
                       minimumOffset,
                       LayoutDirection::RIGHT_TO_LEFT,
                       false);
  DALI_TEST_EQUALS(lines[0u].alignmentOffset, UNALIGNED, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[1u].alignmentOffset, 21.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[2u].alignmentOffset, UNALIGNED, TEST_LOCATION);

  engine.SetLayout(Text::Layout::Engine::SINGLE_LINE_BOX);
  Vector<Text::LineRun> singleLine;
  singleLine.PushBack(MakeLine(0u, 12u, 40.0f, false));
  model->mLogicalModel->mBoundedParagraphRuns.Resize(1u);
  model->mLogicalModel->mBoundedParagraphRuns[0u] = center;
  Text::AlignTextLines(engine,
                       Size(100.0f, 40.0f),
                       0u,
                       12u,
                       *model,
                       singleLine,
                       minimumOffset,
                       LayoutDirection::LEFT_TO_RIGHT,
                       false);
  DALI_TEST_EQUALS(singleLine[0u].alignmentOffset, 0.0f, TEST_LOCATION);

  model->mLogicalModel->mBoundedParagraphRuns[0u].characterRun        = {0u, 12u};
  model->mLogicalModel->mBoundedParagraphRuns[0u].horizontalAlignment = Text::Alignment::END;
  ResetOffsets(singleLine);
  Text::AlignTextLines(engine,
                       Size(100.0f, 40.0f),
                       0u,
                       12u,
                       *model,
                       singleLine,
                       minimumOffset,
                       LayoutDirection::LEFT_TO_RIGHT,
                       false);
  DALI_TEST_EQUALS(singleLine[0u].alignmentOffset, 60.0f, TEST_LOCATION);

  END_TEST;
}
