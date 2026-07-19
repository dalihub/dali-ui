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

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/replacement/replacement-processing-source.h>
namespace Dali::Ui::Text
{
namespace
{
template<typename CharacterStyleRun>
void ProjectGlyphStyleRuns(const Vector<CharacterStyleRun>& source,
                           const ReplacementProjection&     projection,
                           Vector<CharacterStyleRun>&       destination)
{
  if(source.Empty())
  {
    return;
  }

  Vector<CharacterRun> logicalRuns;
  logicalRuns.Reserve(source.Count());
  for(const CharacterStyleRun& run : source)
  {
    logicalRuns.PushBack(run.characterRun);
  }

  Vector<ProjectedStyleSegment> segments;
  projection.ProjectGlyphStyleRuns(logicalRuns, segments);
  destination.Reserve(segments.Count());
  for(const ProjectedStyleSegment& segment : segments)
  {
    CharacterStyleRun run = source[segment.sourceRunIndex];
    run.characterRun      = segment.projectedCharacterRange;
    destination.PushBack(run);
  }
}

template<typename CharacterStyleRun>
void ProjectBoxStyleRuns(const Vector<CharacterStyleRun>& source,
                         const ReplacementProjection&     projection,
                         Vector<CharacterStyleRun>&       destination)
{
  const Length logicalCount = projection.GetLogicalCharacterCount();
  destination.Reserve(source.Count());
  for(const CharacterStyleRun& sourceRun : source)
  {
    const CharacterIndex logicalStart = sourceRun.characterRun.characterIndex;
    if(sourceRun.characterRun.numberOfCharacters == 0u || logicalStart >= logicalCount)
    {
      continue;
    }

    const Length         boundedLength  = std::min<Length>(sourceRun.characterRun.numberOfCharacters,
                                                           logicalCount - logicalStart);
    const CharacterIndex projectedStart = projection.LogicalBoundaryToProjected(
      logicalStart,
      ReplacementProjection::BoundaryAffinity::LEADING);
    const CharacterIndex projectedEnd = projection.LogicalBoundaryToProjected(
      logicalStart + boundedLength,
      ReplacementProjection::BoundaryAffinity::TRAILING);
    if(projectedEnd <= projectedStart)
    {
      continue;
    }

    CharacterStyleRun projectedRun = sourceRun;
    projectedRun.characterRun      = CharacterRun{projectedStart, projectedEnd - projectedStart};
    destination.PushBack(projectedRun);
  }
}

} // unnamed namespace

TextProcessingSource MakeTextProcessingSource(const Model& model)
{
  const LogicalModel&  logical = *model.mLogicalModel;
  TextProcessingSource source;
  source.text                 = &logical.mText;
  source.fontDescriptionRuns  = &logical.mFontDescriptionRuns;
  source.colorRuns            = &logical.mColorRuns;
  source.backgroundRuns       = &logical.mBackgroundColorRuns;
  source.underlineRuns        = &logical.mUnderlinedCharacterRuns;
  source.strikethroughRuns    = &logical.mStrikethroughCharacterRuns;
  source.characterSpacingRuns = &logical.mCharacterSpacingCharacterRuns;
  return source;
}

bool PrepareProjectedTextProcessingSource(const Model&                   originalModel,
                                          const ReplacementProjection&   projection,
                                          ProjectedTextProcessingSource& storage)
{
  if(!projection.HasReplacements())
  {
    return false;
  }

  storage                     = ProjectedTextProcessingSource{};
  const LogicalModel& logical = *originalModel.mLogicalModel;
  ProjectGlyphStyleRuns(logical.mFontDescriptionRuns, projection, storage.fontDescriptionRuns);
  ProjectGlyphStyleRuns(logical.mColorRuns, projection, storage.colorRuns);
  ProjectBoxStyleRuns(logical.mBackgroundColorRuns, projection, storage.backgroundRuns);
  ProjectGlyphStyleRuns(logical.mUnderlinedCharacterRuns, projection, storage.underlineRuns);
  ProjectGlyphStyleRuns(logical.mStrikethroughCharacterRuns, projection, storage.strikethroughRuns);
  ProjectGlyphStyleRuns(logical.mCharacterSpacingCharacterRuns, projection, storage.characterSpacingRuns);

  storage.source.text                  = &projection.GetProcessingText();
  storage.source.fontDescriptionRuns   = &storage.fontDescriptionRuns;
  storage.source.colorRuns             = &storage.colorRuns;
  storage.source.backgroundRuns        = &storage.backgroundRuns;
  storage.source.underlineRuns         = &storage.underlineRuns;
  storage.source.strikethroughRuns     = &storage.strikethroughRuns;
  storage.source.characterSpacingRuns  = &storage.characterSpacingRuns;
  storage.source.replacementProjection = &projection;
  storage.source.replacementRuns       = &projection.GetReplacementRuns();
  return true;
}

void ApplyTextProcessingSource(const TextProcessingSource& source, LogicalModel& target)
{
  target.mText                          = *source.text;
  target.mFontDescriptionRuns           = *source.fontDescriptionRuns;
  target.mColorRuns                     = *source.colorRuns;
  target.mBackgroundColorRuns           = *source.backgroundRuns;
  target.mUnderlinedCharacterRuns       = *source.underlineRuns;
  target.mStrikethroughCharacterRuns    = *source.strikethroughRuns;
  target.mCharacterSpacingCharacterRuns = *source.characterSpacingRuns;
}

void CopyTextProcessingProperties(const Model& source, Model& target)
{
  const VisualModel& sourceVisual = *source.mVisualModel;
  VisualModel&       targetVisual = *target.mVisualModel;

  target.mLineWrapMode          = source.mLineWrapMode;
  target.mElideEnabled          = source.mElideEnabled;
  target.mEllipsisPosition      = source.mEllipsisPosition;
  target.mHorizontalAlignment   = source.mHorizontalAlignment;
  target.mVerticalAlignment     = source.mVerticalAlignment;
  target.mVerticalLineAlignment = source.mVerticalLineAlignment;
  target.mLayoutDirectionMode   = source.mLayoutDirectionMode;
  target.mRemoveFrontInset      = source.mRemoveFrontInset;
  target.mRemoveBackInset       = source.mRemoveBackInset;
  target.mScrollPosition        = source.mScrollPosition;

  target.mLogicalModel->mVariationsMap = source.mLogicalModel->mVariationsMap;

  targetVisual.mControlSize = sourceVisual.mControlSize;
  targetVisual.SetTextColor(source.GetDefaultColor());
  targetVisual.SetCharacterSpacing(source.GetCharacterSpacing());
  targetVisual.SetShadowEnabled(source.IsShadowEnabled());
  targetVisual.SetShadowOffset(source.GetShadowOffset());
  targetVisual.SetShadowColor(source.GetShadowColor());
  targetVisual.SetShadowBlurRadius(source.GetShadowBlurRadius());
  targetVisual.SetUnderlineEnabled(source.IsUnderlineEnabled());
  targetVisual.SetUnderlineType(source.GetUnderlineType());
  targetVisual.SetUnderlineColor(source.GetUnderlineColor());
  targetVisual.SetUnderlineHeight(source.GetUnderlineHeight());
  targetVisual.SetDashedUnderlineWidth(source.GetDashedUnderlineWidth());
  targetVisual.SetDashedUnderlineGap(source.GetDashedUnderlineGap());
  targetVisual.SetOutlineEnabled(source.IsOutlineEnabled());
  targetVisual.SetOutlineOffset(source.GetOutlineOffset());
  targetVisual.SetOutlineColor(source.GetOutlineColor());
  targetVisual.SetOutlineWidth(source.GetOutlineWidth());
  targetVisual.SetOutlineBlurRadius(source.GetOutlineBlurRadius());
  targetVisual.SetBackgroundEnabled(source.IsBackgroundEnabled());
  targetVisual.SetBackgroundColor(source.GetBackgroundColor());
  targetVisual.SetStrikethroughEnabled(source.IsStrikethroughEnabled());
  targetVisual.SetStrikethroughColor(source.GetStrikethroughColor());
  targetVisual.SetStrikethroughHeight(source.GetStrikethroughHeight());

  // Separately registered image visuals cannot contribute to the text cutout mask.
  targetVisual.SetCutoutEnabled(false);
  targetVisual.SetBackgroundWithCutoutEnabled(false);
  targetVisual.SetBackgroundColorWithCutout(source.GetBackgroundColorWithCutout());
  targetVisual.SetVerticalLineAlignment(source.GetVerticalLineAlignment());
}

} // namespace Dali::Ui::Text
