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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/trace.h>
#include <algorithm>
#include <cmath>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
#include <dali-ui-foundation/internal/text/bidirectional-support.h>
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/color-glyph-helper.h>
#include <dali-ui-foundation/internal/text/color-segmentation.h>
#include <dali-ui-foundation/internal/text/hyphenator.h>
#include <dali-ui-foundation/internal/text/marquee/marquee-start-geometry.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-glyph-helper.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-layout-data.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-placement.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-processing-source.h>
#include <dali-ui-foundation/internal/text/segmentation.h>
#include <dali-ui-foundation/internal/text/shaper.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/internal/text/text-alignment.h>
#include <dali-ui-foundation/internal/text/text-geometry.h>
#include <dali-ui-foundation/internal/text/text-gradient-bounds.h>
#include <dali-ui-foundation/internal/text/text-view.h>

namespace Dali
{
namespace Ui
{
namespace
{
constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

const float VERTICAL_ALIGNMENT_TABLE[static_cast<int>(Text::Alignment::END) + 1] = {
  0.0f, // Text::Alignment::START
  0.5f, // Text::Alignment::CENTER
  1.0f  // Text::Alignment::END
};

float ConvertToEven(float value)
{
  int intValue(static_cast<int>(value));
  return static_cast<float>(intValue + (intValue & 1));
}

float GetDpi(TextAbstraction::FontClient& fontClient)
{
  static uint32_t horizontalDpi = 0u;
  static uint32_t verticalDpi   = 0u;

  if(DALI_UNLIKELY(horizontalDpi == 0u))
  {
    fontClient.GetDpi(horizontalDpi, verticalDpi);
  }
  return static_cast<float>(horizontalDpi);
}

float ConvertPixelToPoint(float pixel, TextAbstraction::FontClient& fontClient)
{
  return pixel * 72.0f / GetDpi(fontClient);
}

float ConvertPointToPixel(float point, TextAbstraction::FontClient& fontClient)
{
  // Pixel size = Point size * DPI / 72.f
  return point * GetDpi(fontClient) / 72.0f;
}

std::string AnchorHrefToString(const Text::Anchor& anchor)
{
  return anchor.href == nullptr ? std::string() : std::string(anchor.href);
}

bool MatchesAnchorClickedState(const Text::AsyncAnchorClickedState& state, const Text::Anchor& anchor)
{
  return state.characterIndex == anchor.startIndex &&
         state.numberOfCharacters == anchor.endIndex - anchor.startIndex &&
         state.href == AnchorHrefToString(anchor);
}

void ApplyAsyncAnchorClickedStates(const Text::AsyncTextParameters& parameters, Text::LogicalModel& logicalModel)
{
  if(parameters.clickedAnchors.empty() || logicalModel.mAnchors.Empty())
  {
    return;
  }

  for(auto& anchor : logicalModel.mAnchors)
  {
    const auto clickedIt = std::find_if(parameters.clickedAnchors.begin(),
                                        parameters.clickedAnchors.end(),
                                        [&anchor](const Text::AsyncAnchorClickedState& state)
    {
      return MatchesAnchorClickedState(state, anchor);
    });

    if(clickedIt == parameters.clickedAnchors.end())
    {
      continue;
    }

    anchor.isClicked              = true;
    const Vector4  color          = anchor.isMarkupClickedColorSet ? anchor.markupClickedColor : parameters.anchorClickedColor;
    const uint32_t colorIndex     = anchor.colorRunIndex;
    const uint32_t underlineIndex = anchor.underlinedCharacterRunIndex;

    if(logicalModel.mColorRuns.Count() > colorIndex)
    {
      logicalModel.mColorRuns[colorIndex].color = color;
    }
    if(logicalModel.mUnderlinedCharacterRuns.Count() > underlineIndex)
    {
      logicalModel.mUnderlinedCharacterRuns[underlineIndex].properties.color = color;
    }
  }
}

std::vector<Text::AsyncAnchorHitRegion> BuildAsyncAnchorHitRegions(
  Text::ModelPtr                     semanticModel,
  Text::ModelPtr                     geometryModel,
  const Text::ReplacementProjection* projection,
  const Text::AsyncTextParameters&   parameters)
{
  std::vector<Text::AsyncAnchorHitRegion> regions;
  if(!semanticModel || !geometryModel || semanticModel->mLogicalModel->mAnchors.Empty())
  {
    return regions;
  }

  const float coordinateScale = parameters.renderScale > 1.0f ? 1.0f / parameters.renderScale : 1.0f;
  regions.reserve(semanticModel->mLogicalModel->mAnchors.Count());

  for(const auto& anchor : semanticModel->mLogicalModel->mAnchors)
  {
    if(anchor.endIndex <= anchor.startIndex)
    {
      continue;
    }

    Text::CharacterIndex geometryStart = anchor.startIndex;
    Text::CharacterIndex geometryEnd   = anchor.endIndex;
    if(projection)
    {
      geometryStart = projection->LogicalBoundaryToProjected(
        anchor.startIndex,
        Text::ReplacementProjection::BoundaryAffinity::LEADING);
      geometryEnd = projection->LogicalBoundaryToProjected(
        anchor.endIndex,
        Text::ReplacementProjection::BoundaryAffinity::TRAILING);
    }
    if(geometryEnd <= geometryStart)
    {
      continue;
    }

    Vector<Vector2> sizes;
    Vector<Vector2> positions;
    Text::GetTextGeometry(geometryModel, geometryStart, geometryEnd - 1u, sizes, positions);
    if(sizes.Empty() || sizes.Count() != positions.Count())
    {
      continue;
    }

    Text::AsyncAnchorHitRegion region;
    region.characterIndex     = anchor.startIndex;
    region.numberOfCharacters = anchor.endIndex - anchor.startIndex;
    region.href               = AnchorHrefToString(anchor);
    region.hasColor           = anchor.isMarkupColorSet;
    region.hasClickedColor    = anchor.isMarkupClickedColorSet;
    region.isClicked          = anchor.isClicked;
    region.clickedColor       = anchor.isMarkupClickedColorSet ? anchor.markupClickedColor : parameters.anchorClickedColor;

    if(semanticModel->mLogicalModel->mColorRuns.Count() > anchor.colorRunIndex)
    {
      region.color = semanticModel->mLogicalModel->mColorRuns[anchor.colorRunIndex].color;
    }
    else
    {
      region.color = anchor.isClicked ? region.clickedColor : parameters.anchorColor;
    }

    region.rectangles.reserve(sizes.Count());
    for(uint32_t index = 0u; index < sizes.Count(); ++index)
    {
      const Vector2& size     = sizes[index];
      const Vector2& position = positions[index];
      if(size.x <= Math::MACHINE_EPSILON_1000 || size.y <= Math::MACHINE_EPSILON_1000)
      {
        continue;
      }

      region.rectangles.emplace_back(position.x * coordinateScale,
                                     position.y * coordinateScale,
                                     size.x * coordinateScale,
                                     size.y * coordinateScale);
    }

    if(!region.rectangles.empty())
    {
      regions.push_back(std::move(region));
    }
  }

  return regions;
}

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_ASYNC, false);
} // namespace

namespace Text
{
namespace Internal
{
AsyncTextLoader::AsyncTextLoader()
: mModule(),
  mTextModel(),
  mMetrics(),
  mReplacementData(),
  mEndEllipsisResult(),
  mLocale(),
  mCustomFonts(),
  mNumberOfCharacters(0u),
  mFitActualEllipsis(true),
  mIsTextDirectionRTL(false),
  mIsTextMirrored(false),
  mModuleClearNeeded(false),
  mLocaleUpdateNeeded(false),
  mMutex()
{
  mModule = Dali::Ui::Text::AsyncTextModule::New();

  mTextModel = Model::New();

  mTypesetter = Text::Typesetter::New(mTextModel.Get());

  // Use this to access FontClient i.e. to get down-scaled Emoji metrics.
  mMetrics = Metrics::New(mModule.GetFontClient());
  mLayoutEngine.SetMetrics(mMetrics);

  mLocale = TextAbstraction::GetLocaleFull();
}

AsyncTextLoader::~AsyncTextLoader()
{
  if(mReplacementData && mReplacementData->renderState.attempted)
  {
    mReplacementData->renderState.Clear(mModule.GetBidirectionalSupport());
  }
}

void AsyncTextLoader::SetLocale(const std::string& locale)
{
  mLocale = locale;
  mModule.GetMultilanguageSupport().SetLocale(mLocale);
}

void AsyncTextLoader::SetLocaleUpdateNeeded(bool update)
{
  Dali::Mutex::ScopedLock lock(mMutex);
  mLocaleUpdateNeeded = update;
}

bool AsyncTextLoader::IsLocaleUpdateNeeded()
{
  return mLocaleUpdateNeeded;
}

void AsyncTextLoader::ClearModule()
{
  mModule.ClearCache();
}

void AsyncTextLoader::SetCustomFontDirectories(const TextAbstraction::FontPathList& customFontDirectories)
{
  for(auto& path : customFontDirectories)
  {
    mModule.GetFontClient().AddCustomFontDirectory(path);
  }
}

void AsyncTextLoader::RequestAddCustomFont(const std::string& path)
{
  Dali::Mutex::ScopedLock lock(mMutex);
  mCustomFonts.push_back(path);
}

void AsyncTextLoader::SetModuleClearNeeded(bool clear)
{
  Dali::Mutex::ScopedLock lock(mMutex);
  mModuleClearNeeded = clear;
}

bool AsyncTextLoader::IsModuleClearNeeded()
{
  return mModuleClearNeeded;
}

// Worker thread
void AsyncTextLoader::Initialize()
{
  mModule.GetFontClient().InitDefaultFontDescription();
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    if(!mCustomFonts.empty())
    {
      for(const auto& font : mCustomFonts)
      {
        mModule.GetFontClient().AddCustomFontDirectory(font);
      }
      mCustomFonts.clear();
    }
  }

  if(mReplacementData)
  {
    if(mReplacementData->renderState.attempted)
    {
      mReplacementData->renderState.Clear(mModule.GetBidirectionalSupport());
    }
    mReplacementData.reset();
  }
  if(mTypesetter)
  {
    mTypesetter->SetFinalElisionResult(nullptr);
  }
  mEndEllipsisResult.reset();
  ClearTextModelData();

  mNumberOfCharacters = 0u;
  mIsTextDirectionRTL = false;
  mIsTextMirrored     = false;

  // Set the text properties to default
  mTextModel->mVisualModel->SetUnderlineEnabled(false);
  mTextModel->mVisualModel->SetUnderlineHeight(0.0f);
  mTextModel->mVisualModel->SetOutlineEnabled(false);
  mTextModel->mVisualModel->SetOutlineWidth(static_cast<uint16_t>(0.0f));
  mTextModel->mVisualModel->SetShadowEnabled(false);
  mTextModel->mVisualModel->SetShadowOffset(Vector2(0.0f, 0.0f));
  mTextModel->mVisualModel->SetStrikethroughEnabled(false);
  mTextModel->mVisualModel->SetStrikethroughHeight(0.0f);
  mTextModel->mVisualModel->SetBackgroundEnabled(false);
}

void AsyncTextLoader::ClearTextModelData()
{
  mTextModel->mLogicalModel->mText.Clear();
  mTextModel->mLogicalModel->mScriptRuns.Clear();
  mTextModel->mLogicalModel->mFontRuns.Clear();
  mTextModel->mLogicalModel->mColorRuns.Clear();
  mTextModel->mLogicalModel->mBackgroundColorRuns.Clear();
  mTextModel->mLogicalModel->mLineBreakInfo.Clear();
  mTextModel->mLogicalModel->mParagraphInfo.Clear();
  mTextModel->mLogicalModel->ClearBidirectionalParagraphInfo(mModule.GetBidirectionalSupport());
  mTextModel->mLogicalModel->mBidirectionalParagraphInfo.Clear();
  mTextModel->mLogicalModel->mCharacterDirections.Clear();
  mTextModel->mLogicalModel->mCharacterSpacingCharacterRuns.Clear();

  mTextModel->mLogicalModel->ClearFontDescriptionRuns();
  mTextModel->mLogicalModel->ClearStrikethroughRuns();
  mTextModel->mLogicalModel->ClearUnderlineRuns();
  mTextModel->mLogicalModel->ClearAnchors();
  mTextModel->mLogicalModel->mVariationsMap.Clear();

  // Free the allocated memory used to store the conversion table in the bidirectional line info run.
  for(Vector<BidirectionalLineInfoRun>::Iterator it    = mTextModel->mLogicalModel->mBidirectionalLineInfo.Begin(),
                                                 endIt = mTextModel->mLogicalModel->mBidirectionalLineInfo.End();
      it != endIt; ++it)
  {
    BidirectionalLineInfoRun& bidiLineInfo = *it;

    free(bidiLineInfo.visualToLogicalMap);
    bidiLineInfo.visualToLogicalMap = NULL;

    free(bidiLineInfo.visualToLogicalMapSecondHalf);
    bidiLineInfo.visualToLogicalMapSecondHalf = NULL;
  }
  mTextModel->mLogicalModel->mBidirectionalLineInfo.Clear();

  mTextModel->mVisualModel->ClearCaches();
  mTextModel->mVisualModel->mGlyphs.Clear();
  mTextModel->mVisualModel->mGlyphsToCharacters.Clear();
  mTextModel->mVisualModel->mCharactersToGlyph.Clear();
  mTextModel->mVisualModel->mCharactersPerGlyph.Clear();
  mTextModel->mVisualModel->mGlyphsPerCharacter.Clear();
  mTextModel->mVisualModel->mGlyphPositions.Clear();
  mTextModel->mVisualModel->mLines.Clear();
  mTextModel->mVisualModel->mColors.Clear();
  mTextModel->mVisualModel->mColorIndices.Clear();
  mTextModel->mVisualModel->mBackgroundColors.Clear();
  mTextModel->mVisualModel->mBackgroundColorIndices.Clear();
  mTextModel->mVisualModel->mUnderlineRuns.Clear();
  mTextModel->mVisualModel->mStrikethroughRuns.Clear();
  mTextModel->mVisualModel->mCharacterSpacingRuns.Clear();
}

void AsyncTextLoader::Update(AsyncTextParameters& parameters)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_UPDATE");

  if(parameters.text.empty())
  {
    DALI_LOG_ERROR("Text is empty\n");
    return;
  }

  const uint8_t* utf8     = nullptr; // pointer to the first character of the text (encoded in utf8)
  Length         textSize = 0u;      // The length of the utf8 string.

  Length&                       numberOfCharacters = mNumberOfCharacters;
  Vector<Character>             mirroredUtf32Characters;
  ProjectedTextProcessingSource projectedSourceStorage;
  TextProcessingSource          processingSource;

  Vector<Character>&          utf32Characters = mTextModel->mLogicalModel->mText;          // Characters encoded in utf32.
  Vector<LineBreakInfo>&      lineBreakInfo   = mTextModel->mLogicalModel->mLineBreakInfo; // The line break info.
  Vector<ScriptRun>&          scripts         = mTextModel->mLogicalModel->mScriptRuns;    // Charactes's script.
  Vector<FontDescriptionRun>& fontDescriptionRuns =
    mTextModel->mLogicalModel->mFontDescriptionRuns;                                        // Desired font descriptions.
  Vector<FontRun>&                       validFonts = mTextModel->mLogicalModel->mFontRuns; // Validated fonts.
  Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo =
    mTextModel->mLogicalModel->mBidirectionalParagraphInfo; // The bidirectional info per paragraph.
  // Set the default font's description with the given text parameters.
  TextAbstraction::FontDescription defaultFontDescription;
  defaultFontDescription.family = parameters.fontFamily;
  defaultFontDescription.weight = parameters.fontWeight;
  defaultFontDescription.width  = parameters.fontWidth;
  defaultFontDescription.slant  = parameters.fontSlant;

  mTextModel->mHorizontalAlignment   = parameters.horizontalAlignment;
  mTextModel->mVerticalAlignment     = parameters.verticalAlignment;
  mTextModel->mVerticalLineAlignment = parameters.verticalLineAlignment;
  mTextModel->mVisualModel->SetVerticalLineAlignment(parameters.verticalLineAlignment);

  mTextModel->mLogicalModel->mVariationsMap = parameters.variationsMap;

  ////////////////////////////////////////////////////////////////////////////////
  // Update visual model.
  ////////////////////////////////////////////////////////////////////////////////

  // Store the size used to layout the text.
  // control size is used in ElideGlyphs in ViewModel.
  mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);

  // Update style properties.
  mTextModel->mVisualModel->SetTextColor(parameters.textColor);

  if(parameters.isUnderlineEnabled)
  {
    mTextModel->mVisualModel->SetUnderlineEnabled(parameters.isUnderlineEnabled);
    mTextModel->mVisualModel->SetUnderlineType(parameters.underlineType);
    mTextModel->mVisualModel->SetUnderlineColor(parameters.underlineColor);
    mTextModel->mVisualModel->SetUnderlineHeight(parameters.underlineHeight);
    mTextModel->mVisualModel->SetDashedUnderlineWidth(parameters.dashedUnderlineWidth);
    mTextModel->mVisualModel->SetDashedUnderlineGap(parameters.dashedUnderlineGap);
  }

  if(parameters.isStrikethroughEnabled)
  {
    mTextModel->mVisualModel->SetStrikethroughEnabled(parameters.isStrikethroughEnabled);
    mTextModel->mVisualModel->SetStrikethroughColor(parameters.strikethroughColor);
    mTextModel->mVisualModel->SetStrikethroughHeight(parameters.strikethroughHeight);
  }

  if(parameters.isTextBackgroundEnabled)
  {
    mTextModel->mVisualModel->SetBackgroundEnabled(parameters.isTextBackgroundEnabled);
    mTextModel->mVisualModel->SetBackgroundColor(parameters.textBackgroundColor);
  }

  if(parameters.isShadowEnabled)
  {
    mTextModel->mVisualModel->SetShadowEnabled(true);
    mTextModel->mVisualModel->SetShadowBlurRadius(parameters.shadowBlurRadius);
    mTextModel->mVisualModel->SetShadowColor(parameters.shadowColor);
    mTextModel->mVisualModel->SetShadowOffset(parameters.shadowOffset);
  }
  else
  {
    mTextModel->mVisualModel->SetShadowEnabled(false);
  }

  if(parameters.isOutlineEnabled)
  {
    mTextModel->mVisualModel->SetOutlineEnabled(true);
    mTextModel->mVisualModel->SetOutlineColor(parameters.outlineColor);
    mTextModel->mVisualModel->SetOutlineWidth(parameters.outlineWidth);
    mTextModel->mVisualModel->SetOutlineBlurRadius(parameters.outlineBlurRadius);
    mTextModel->mVisualModel->SetOutlineOffset(parameters.outlineOffset);
  }
  else
  {
    mTextModel->mVisualModel->SetOutlineEnabled(false);
  }

  mTextModel->mVisualModel->SetCutoutEnabled(parameters.isCutoutEnabled);
  mTextModel->mVisualModel->SetBackgroundWithCutoutEnabled(parameters.isBackgroundWithCutoutEnabled);
  mTextModel->mVisualModel->SetBackgroundColorWithCutout(parameters.backgroundColorWithCutout);

  mTextModel->mRemoveFrontInset = false;
  mTextModel->mRemoveBackInset  = false;

  if(parameters.hasStyledTextStyleSnapshot)
  {
    Dali::Ui::Internal::Text::StyledTextApplier::ApplySnapshotToLogicalModel(parameters.styledTextStyleSnapshot,
                                                                             parameters.text,
                                                                             *mTextModel->mLogicalModel);
    // ApplySnapshotToLogicalModel updates logicalModel.mText; utf32Characters is a reference to it.
    numberOfCharacters = static_cast<Dali::Ui::Text::Length>(utf32Characters.Count());
  }
  else
  {
    textSize = static_cast<Dali::Ui::Text::Length>(parameters.text.size());

    // This is a bit horrible but std::string returns a (signed) char*
    utf8 = reinterpret_cast<const uint8_t*>(parameters.text.c_str());

    ////////////////////////////////////////////////////////////////////////////////
    // Convert from utf8 to utf32
    ////////////////////////////////////////////////////////////////////////////////

    utf32Characters.Resize(textSize);

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    numberOfCharacters = Utf8ToUtf32(utf8, textSize, utf32Characters.Begin());
    utf32Characters.Resize(numberOfCharacters);
  }

  processingSource = MakeTextProcessingSource(*mTextModel);
  if(!parameters.replacementSourceSnapshot.runs.Empty())
  {
    mReplacementData                         = std::make_unique<ReplacementData>();
    ReplacementRenderState& replacementState = mReplacementData->renderState;
    mReplacementData->originalLogicalText    = utf32Characters;
    replacementState.attempted               = true;
    replacementState.sourceRevision          = parameters.replacementSourceSnapshot.sourceRevision;
    replacementState.layoutGeneration        = parameters.replacementLayoutGeneration;
    replacementState.projection              = ReplacementProjection::Build(
      mReplacementData->originalLogicalText,
      parameters.replacementSourceSnapshot.runs,
      parameters.effectiveTextScale * parameters.renderScale);
    if(replacementState.projection.HasReplacements() &&
       PrepareProjectedTextProcessingSource(*mTextModel,
                                            replacementState.projection,
                                            projectedSourceStorage))
    {
      processingSource = projectedSourceStorage.source;
      ApplyTextProcessingSource(processingSource, *mTextModel->mLogicalModel);
      numberOfCharacters               = static_cast<Dali::Ui::Text::Length>(utf32Characters.Count());
      replacementState.processingModel = mTextModel;
    }
  }

  ApplyAsyncAnchorClickedStates(parameters, *mTextModel->mLogicalModel);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the Line and Word Break Info.
  ////////////////////////////////////////////////////////////////////////////////

  lineBreakInfo.Resize(numberOfCharacters, TextAbstraction::LINE_NO_BREAK);
  SetLineBreakInfo(mModule.GetSegmentation(), utf32Characters, 0u, numberOfCharacters, lineBreakInfo);

  // Check if an ICU-based line break update is required.
  if(mModule.GetMultilanguageSupport().IsICULineBreakNeeded())
  {
    std::string currentText;
    Utf32ToUtf8(mTextModel->mLogicalModel->mText.Begin(), numberOfCharacters, currentText);
    mModule.GetMultilanguageSupport().UpdateICULineBreak(currentText, numberOfCharacters, lineBreakInfo.Begin());
  }

  // Hyphenation
  if(parameters.lineWrapMode == LineWrapMode::HYPHENATION ||
     parameters.lineWrapMode == LineWrapMode::MIXED)
  {
    CharacterIndex startIndex          = 0u;
    CharacterIndex end                 = numberOfCharacters;
    LineBreakInfo* lineBreakInfoBuffer = lineBreakInfo.Begin();

    for(CharacterIndex index = startIndex; index < end; index++)
    {
      CharacterIndex wordEnd = index;
      while((*(lineBreakInfoBuffer + wordEnd) != TextAbstraction::LINE_ALLOW_BREAK) &&
            (*(lineBreakInfoBuffer + wordEnd) != TextAbstraction::LINE_MUST_BREAK))
      {
        wordEnd++;
      }

      if((wordEnd + 1) == end) // add last char
      {
        wordEnd++;
      }

      Vector<bool> hyphens =
        GetWordHyphens(mModule.GetHyphenation(), utf32Characters.Begin() + index, wordEnd - index, nullptr);

      for(CharacterIndex i = 0; i < (wordEnd - index) && i < hyphens.Size(); i++)
      {
        if(hyphens[i])
        {
          *(lineBreakInfoBuffer + index + i) = TextAbstraction::LINE_HYPHENATION_BREAK;
        }
      }

      index = wordEnd;
    }
  }

  // Create the paragraph info.
  mTextModel->mLogicalModel->CreateParagraphInfo(0u, numberOfCharacters);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the script runs.
  ////////////////////////////////////////////////////////////////////////////////

  mModule.GetMultilanguageSupport().SetScripts(utf32Characters, 0u, numberOfCharacters, scripts);

  ////////////////////////////////////////////////////////////////////////////////
  // Validate Fonts.
  ////////////////////////////////////////////////////////////////////////////////

  // Text fit and fit candidates are already converted to effective scaled font sizes
  // before CheckForTextFit(). Avoid applying effectiveTextScale again here.
  float effectiveTextScale = (parameters.isTextFitEnabled || parameters.isTextFitCandidatesEnabled) ? 1.0f : parameters.effectiveTextScale;
  float scale              = effectiveTextScale * parameters.renderScale;

  TextAbstraction::PointSize26Dot6 defaultPointSize = static_cast<TextAbstraction::PointSize26Dot6>(TextAbstraction::FontClient::DEFAULT_POINT_SIZE * scale);

  // Get the number of points per one unit of point-size
  uint32_t numberOfPointsPerOneUnitOfPointSize = mModule.GetFontClient().GetNumberOfPointsPerOneUnitOfPointSize();

  defaultPointSize = static_cast<TextAbstraction::PointSize26Dot6>(parameters.fontSize * scale * numberOfPointsPerOneUnitOfPointSize);

  Property::Map* variationsMapPtr = nullptr;
  if(!mTextModel->mLogicalModel->mVariationsMap.Empty())
  {
    variationsMapPtr = &mTextModel->mLogicalModel->mVariationsMap;
  }

  // Validates the fonts. If there is a character with no assigned font it sets a default one.
  // After this call, fonts are validated.
  if(processingSource.HasReplacements())
  {
    ValidateFontsForProcessingSource(mModule.GetMultilanguageSupport(),
                                     mModule.GetFontClient(),
                                     processingSource,
                                     scripts,
                                     defaultFontDescription,
                                     defaultPointSize,
                                     scale,
                                     0u,
                                     numberOfCharacters,
                                     validFonts,
                                     variationsMapPtr);
  }
  else
  {
    mModule.GetMultilanguageSupport().ValidateFonts(mModule.GetFontClient(), utf32Characters, scripts,
                                                    fontDescriptionRuns, defaultFontDescription, defaultPointSize,
                                                    scale, 0u, numberOfCharacters, validFonts, variationsMapPtr);
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the Bidirectional info.
  ////////////////////////////////////////////////////////////////////////////////

  // Update the layout direction policy to text model.
  mTextModel->mLayoutDirectionMode = parameters.layoutDirectionPolicy;

  mIsTextMirrored                 = false;
  const Length numberOfParagraphs = static_cast<Dali::Ui::Text::Length>(mTextModel->mLogicalModel->mParagraphInfo.Count());

  bidirectionalInfo.Reserve(numberOfParagraphs);

  // Calculates the bidirectional info for the whole paragraph if it contains right to left scripts.
  SetBidirectionalInfo(mModule.GetBidirectionalSupport(), utf32Characters, scripts, lineBreakInfo, 0u,
                       numberOfCharacters, bidirectionalInfo, mTextModel->mLogicalModel->mBidirectionalLineInfo,
                       (mTextModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS),
                       parameters.layoutDirection);

  if(0u != bidirectionalInfo.Count())
  {
    // Only set the character directions if there is right to left characters.
    Vector<CharacterDirection>& directions = mTextModel->mLogicalModel->mCharacterDirections;
    GetCharactersDirection(mModule.GetBidirectionalSupport(), bidirectionalInfo, numberOfCharacters, 0u,
                           numberOfCharacters, directions);

    // This paragraph has right to left text. Some characters may need to be mirrored.
    // TODO: consider if the mirrored string can be stored as well.

    mIsTextMirrored = GetMirroredText(mModule.GetBidirectionalSupport(), utf32Characters, directions, bidirectionalInfo,
                                      0u, numberOfCharacters, mirroredUtf32Characters);
  }
  else
  {
    // There is no right to left characters. Clear the directions vector.
    mTextModel->mLogicalModel->mCharacterDirections.Clear();
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the glyphs. Text shaping
  ////////////////////////////////////////////////////////////////////////////////

  Vector<GlyphInfo>& glyphs = mTextModel->mVisualModel->mGlyphs;
  Vector<GlyphIndex> newParagraphGlyphs;
  newParagraphGlyphs.Reserve(numberOfParagraphs);

  const Length currentNumberOfGlyphs = static_cast<Dali::Ui::Text::Length>(glyphs.Count());

  const Vector<Character>& textToShape = mIsTextMirrored ? mirroredUtf32Characters : utf32Characters;

  // Shapes the text.
  ShapeTextForProcessingSource(mModule.GetShaping(),
                               mModule.GetFontClient(),
                               processingSource,
                               textToShape,
                               lineBreakInfo,
                               scripts,
                               validFonts,
                               0u,
                               0u,
                               numberOfCharacters,
                               *mTextModel->mVisualModel,
                               newParagraphGlyphs);

  // Create the 'number of glyphs' per character and the glyph to character conversion tables.
  mTextModel->mVisualModel->CreateGlyphsPerCharacterTable(0u, 0u, numberOfCharacters);
  mTextModel->mVisualModel->CreateCharacterToGlyphTable(0u, 0u, numberOfCharacters);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the glyph's metrics.
  ////////////////////////////////////////////////////////////////////////////////

  const Length numberOfGlyphs = static_cast<Length>(glyphs.Count()) - currentNumberOfGlyphs;

  GetGlyphMetricsForProcessingSource(*mMetrics,
                                     processingSource,
                                     glyphs,
                                     0u,
                                     numberOfGlyphs,
                                     newParagraphGlyphs);

  ////////////////////////////////////////////////////////////////////////////////
  // Set the color runs in glyphs.
  ////////////////////////////////////////////////////////////////////////////////

  // Set the color runs in glyphs.
  SetColorSegmentationInfo(mTextModel->mLogicalModel->mColorRuns, mTextModel->mVisualModel->mCharactersToGlyph,
                           mTextModel->mVisualModel->mGlyphsPerCharacter, 0u, 0u, numberOfCharacters,
                           mTextModel->mVisualModel->mColors, mTextModel->mVisualModel->mColorIndices);

  // Set the background color runs in glyphs.
  SetColorSegmentationInfo(mTextModel->mLogicalModel->mBackgroundColorRuns,
                           mTextModel->mVisualModel->mCharactersToGlyph, mTextModel->mVisualModel->mGlyphsPerCharacter,
                           0u, 0u, numberOfCharacters, mTextModel->mVisualModel->mBackgroundColors,
                           mTextModel->mVisualModel->mBackgroundColorIndices);

  ////////////////////////////////////////////////////////////////////////////////
  // Update visual model for StyledText range style.
  ////////////////////////////////////////////////////////////////////////////////

  if(parameters.hasStyledTextStyleSnapshot)
  {
    const Vector<UnderlinedCharacterRun>&    underlinedCharacterRuns = mTextModel->mLogicalModel->mUnderlinedCharacterRuns;
    const Vector<StrikethroughCharacterRun>& strikethroughCharacterRuns =
      mTextModel->mLogicalModel->mStrikethroughCharacterRuns;
    const Vector<CharacterSpacingCharacterRun>& characterSpacingCharacterRuns =
      mTextModel->mLogicalModel->mCharacterSpacingCharacterRuns;
    const Vector<GlyphIndex>& charactersToGlyph  = mTextModel->mVisualModel->mCharactersToGlyph;
    const Vector<Length>&     glyphsPerCharacter = mTextModel->mVisualModel->mGlyphsPerCharacter;

    ////////////////////////////////////////////////////////////////////////////////
    // Markup underline
    ////////////////////////////////////////////////////////////////////////////////

    // Should clear previous underline runs.
    mTextModel->mVisualModel->mUnderlineRuns.Clear();

    for(Vector<UnderlinedCharacterRun>::ConstIterator it    = underlinedCharacterRuns.Begin(),
                                                      endIt = underlinedCharacterRuns.End();
        it != endIt; ++it)
    {
      CharacterIndex characterIndex     = it->characterRun.characterIndex;
      Length         numberOfCharacters = it->characterRun.numberOfCharacters;

      if(numberOfCharacters == 0)
      {
        continue;
      }

      // Create one run for all glyphs of all run's characters that has same properties
      // This enhance performance and reduce the needed memory to store glyphs-runs
      UnderlinedGlyphRun underlineGlyphRun;
      underlineGlyphRun.properties              = it->properties;
      underlineGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
      underlineGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

      for(Length index = 1u; index < numberOfCharacters; index++)
      {
        underlineGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
      }

      mTextModel->mVisualModel->mUnderlineRuns.PushBack(underlineGlyphRun);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Markup strikethrough
    ////////////////////////////////////////////////////////////////////////////////

    // Should clear previous strikethrough runs.
    mTextModel->mVisualModel->mStrikethroughRuns.Clear();

    for(Vector<StrikethroughCharacterRun>::ConstIterator it    = strikethroughCharacterRuns.Begin(),
                                                         endIt = strikethroughCharacterRuns.End();
        it != endIt; ++it)
    {
      CharacterIndex characterIndex     = it->characterRun.characterIndex;
      Length         numberOfCharacters = it->characterRun.numberOfCharacters;

      if(numberOfCharacters == 0)
      {
        continue;
      }

      StrikethroughGlyphRun strikethroughGlyphRun;
      strikethroughGlyphRun.properties              = it->properties;
      strikethroughGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
      strikethroughGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

      for(Length index = 1u; index < numberOfCharacters; index++)
      {
        strikethroughGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
      }

      mTextModel->mVisualModel->mStrikethroughRuns.PushBack(strikethroughGlyphRun);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Markup character spacing
    ////////////////////////////////////////////////////////////////////////////////

    // Should clear previous character spacing runs.
    mTextModel->mVisualModel->mCharacterSpacingRuns.Clear();

    for(Vector<CharacterSpacingCharacterRun>::ConstIterator it    = characterSpacingCharacterRuns.Begin(),
                                                            endIt = characterSpacingCharacterRuns.End();
        it != endIt; ++it)
    {
      const CharacterIndex& characterIndex     = it->characterRun.characterIndex;
      const Length&         numberOfCharacters = it->characterRun.numberOfCharacters;

      if(numberOfCharacters == 0)
      {
        continue;
      }

      CharacterSpacingGlyphRun characterSpacingGlyphRun;
      characterSpacingGlyphRun.value                   = it->value;
      characterSpacingGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
      characterSpacingGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

      for(Length index = 1u; index < numberOfCharacters; index++)
      {
        characterSpacingGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
      }

      mTextModel->mVisualModel->mCharacterSpacingRuns.PushBack(characterSpacingGlyphRun);
    }
  }
}

void AsyncTextLoader::UpdateReplacementProcessing(AsyncTextParameters& parameters, FontId defaultFontId)
{
  if(!mReplacementData || !mReplacementData->renderState.projection.HasReplacements())
  {
    return;
  }

  ReplacementRenderState& replacementState = mReplacementData->renderState;

  View finalView;
  finalView.SetVisualModel(mTextModel->mVisualModel);
  finalView.SetLogicalModel(mTextModel->mLogicalModel);
  // One async request may perform natural-size, height-for-width, text-fit and
  // final fixed-size layouts. Each pass mutates the worker model and therefore
  // receives its own final-elision generation; the request generation remains
  // separately available for UI-thread stale-result rejection.
  const uint64_t finalGeneration = ++mReplacementData->finalElisionGeneration;
  finalView.ResolveFinalElision(mModule.GetFontClient(),
                                replacementState.finalElision,
                                finalGeneration);

  ReplacementRenderState& result = replacementState;
  result.processingModel         = mTextModel;
  result.layoutSize              = mTextModel->mVisualModel->GetLayoutSize();
  result.sourceRevision          = parameters.replacementSourceSnapshot.sourceRevision;
  result.layoutGeneration        = parameters.replacementLayoutGeneration;

  ExtractReplacementPlacements(*mTextModel,
                               result.projection,
                               result.finalElision,
                               mModule.GetFontClient(),
                               defaultFontId,
                               result.placements);
}

void AsyncTextLoader::CopyReplacementResult(AsyncTextRenderInfo& renderInfo, float renderScale) const
{
  if(!mReplacementData || !mReplacementData->renderState.projection.HasReplacements())
  {
    return;
  }

  const ReplacementRenderState& replacementState = mReplacementData->renderState;
  renderInfo.replacementPlacements               = replacementState.placements;
  renderInfo.replacementSourceRevision           = replacementState.sourceRevision;
  renderInfo.replacementLayoutGeneration         = replacementState.layoutGeneration;
  if(renderScale > 1.0f)
  {
    const float logicalScale = 1.0f / renderScale;
    for(ReplacementPlacement& placement : renderInfo.replacementPlacements)
    {
      placement.position *= logicalScale;
      placement.size *= logicalScale;
    }
  }
}

const Model* AsyncTextLoader::GetRenderTextModel() const
{
  if(mReplacementData &&
     mReplacementData->renderState.processingModel &&
     mReplacementData->renderState.projection.HasReplacements())
  {
    return mReplacementData->renderState.processingModel.Get();
  }
  return mTextModel.Get();
}

void AsyncTextLoader::CopyRenderModelSummary(AsyncTextRenderInfo& renderInfo) const
{
  const Model* const renderModel = GetRenderTextModel();
  renderInfo.lineCount           = renderModel ? renderModel->GetNumberOfLines() : 0u;
  renderInfo.isTextDirectionRTL  = false;
  if(renderModel && !renderModel->mVisualModel->mLines.Empty())
  {
    renderInfo.isTextDirectionRTL = renderModel->mVisualModel->mLines[0u].direction;
  }
}

Size AsyncTextLoader::Layout(AsyncTextParameters& parameters, bool& updated)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_LAYOUT");

  ////////////////////////////////////////////////////////////////////////////////
  // Layout the text.
  ////////////////////////////////////////////////////////////////////////////////

  Length& numberOfCharacters = mNumberOfCharacters;

  const Size textLayoutArea(parameters.textWidth, parameters.textHeight);

  // Calculate the number of glyphs to layout.
  const Vector<GlyphIndex>& charactersToGlyph        = mTextModel->mVisualModel->mCharactersToGlyph;
  const Vector<Length>&     glyphsPerCharacter       = mTextModel->mVisualModel->mGlyphsPerCharacter;
  const GlyphIndex* const   charactersToGlyphBuffer  = charactersToGlyph.Begin();
  const Length* const       glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();

  const CharacterIndex startIndex      = 0u;
  const CharacterIndex lastIndex       = numberOfCharacters > 0u ? numberOfCharacters - 1u : 0u;
  const GlyphIndex     startGlyphIndex = 0u;

  // Make sure the index is not out of bound
  if(charactersToGlyph.Count() != glyphsPerCharacter.Count() || numberOfCharacters > charactersToGlyph.Count() ||
     (lastIndex > charactersToGlyph.Count() && charactersToGlyph.Count() > 0u))
  {
    DALI_LOG_ERROR("Attempting to access invalid buffer\n");
    DALI_LOG_ERROR("Current text is: %s\n", parameters.text.c_str());
    DALI_LOG_ERROR(
      "startIndex: %u, lastIndex: %u, requestedNumberOfCharacters: %u, charactersToGlyph.Count = %lu, "
      "glyphsPerCharacter.Count = %lu\n",
      startIndex, lastIndex, numberOfCharacters, charactersToGlyph.Count(), glyphsPerCharacter.Count());
    return Size::ZERO;
  }

  const Length numberOfGlyphs =
    (numberOfCharacters > 0u)
      ? *(charactersToGlyphBuffer + lastIndex) + *(glyphsPerCharacterBuffer + lastIndex) - startGlyphIndex
      : 0u;
  const Length totalNumberOfGlyphs = static_cast<Dali::Ui::Text::Length>(mTextModel->mVisualModel->mGlyphs.Count());

  if(0u == totalNumberOfGlyphs)
  {
    mTextModel->mVisualModel->SetLayoutSize(Size::ZERO);

    // Nothing else to do if there is no glyphs.
    UpdateReplacementProcessing(parameters, 0u);
    DALI_LOG_RELEASE_INFO("no glyphs\n");
    return Size::ZERO;
  }

  const Text::Layout::Engine::Type layoutType =
    parameters.isMultiLine ? Text::Layout::Engine::MULTI_LINE_BOX : Text::Layout::Engine::SINGLE_LINE_BOX;
  mLayoutEngine.SetLayout(layoutType);

  // Set minimun line size, line spacing, relative line size.
  mLayoutEngine.SetFontSizeScale(parameters.effectiveTextScale);
  mLayoutEngine.SetDefaultLineSize(parameters.minLineSize);
  mLayoutEngine.SetDefaultLineSpacing(0.0f);
  mLayoutEngine.SetRelativeLineSize(parameters.relativeLineSize);

  // Text fit and fit candidates are already converted to effective scaled font sizes
  // before CheckForTextFit(). Avoid applying effectiveTextScale again here.
  float fontPointSize = (parameters.isTextFitEnabled || parameters.isTextFitCandidatesEnabled)
                          ? parameters.fontSize
                          : parameters.fontSize * parameters.effectiveTextScale;
  mLayoutEngine.SetFontPixelSize(ConvertPointToPixel(fontPointSize, mModule.GetFontClient()));

  // Set vertical line alignment.
  mTextModel->mVerticalLineAlignment = parameters.verticalLineAlignment;
  mTextModel->mVisualModel->SetVerticalLineAlignment(parameters.verticalLineAlignment);

  // Set character spacing.
  mTextModel->mVisualModel->SetCharacterSpacing(parameters.characterSpacing);

  // Set the layout parameters.
  mTextModel->mLineWrapMode = parameters.lineWrapMode;

  // Set the layout parameters.
  Layout::Parameters            layoutParameters(textLayoutArea, mTextModel, mModule.GetFontClient(),
                                                 mModule.GetBidirectionalSupport());
  const ReplacementRenderState* replacementState     = mReplacementData ? &mReplacementData->renderState : nullptr;
  const bool                    hasActiveReplacement = replacementState && replacementState->projection.HasReplacements();

  // Resize the vector of positions to have the same size than the vector of glyphs.
  Vector<Vector2>& glyphPositions = mTextModel->mVisualModel->mGlyphPositions;
  glyphPositions.Resize(totalNumberOfGlyphs);

  // The initial glyph and the number of glyphs to layout.
  layoutParameters.startGlyphIndex        = startGlyphIndex;
  layoutParameters.numberOfGlyphs         = numberOfGlyphs;
  layoutParameters.startLineIndex         = 0u;
  layoutParameters.estimatedNumberOfLines = 1u;
  layoutParameters.interGlyphExtraAdvance = 0.f;

  // Whether the last character is a new paragraph character.
  const Character* const textBuffer = mTextModel->mLogicalModel->mText.Begin();
  layoutParameters.isLastNewParagraph =
    TextAbstraction::IsNewParagraph(*(textBuffer + (mTextModel->mLogicalModel->mText.Count() - 1u)));

  // Update the ellipsis
  // START/MIDDLE are not released for replacement content. Keep the
  // replacement projection active and fall back to CLIP so the underlying
  // authored range is never exposed as ordinary text.
  const bool useReplacementClipFallback =
    hasActiveReplacement &&
    parameters.ellipsis && parameters.ellipsisPosition != EllipsisPosition::END;
  bool ellipsisEnabled      = parameters.ellipsis && !useReplacementClipFallback;
  mTextModel->mElideEnabled = ellipsisEnabled;
  mTextModel->mVisualModel->SetTextElideEnabled(ellipsisEnabled);

  auto ellipsisPosition         = parameters.ellipsisPosition;
  mTextModel->mEllipsisPosition = ellipsisPosition;
  mTextModel->mVisualModel->SetEllipsisPosition(ellipsisPosition);

  // Update the visual model.
  Size       newLayoutSize; // The size of the text after it has been laid-out.
  bool       isMarqueeEnabled            = parameters.isMarqueeEnabled;
  bool       isMarqueeMaxTextureExceeded = parameters.isMarqueeMaxTextureExceeded;
  bool       isHiddenInputEnabled        = false;
  const auto layoutText                  = [&](ReplacementLayoutData* replacementLayoutData)
  {
    layoutParameters.replacementLayoutData = replacementLayoutData;
    updated                                = mLayoutEngine.LayoutText(layoutParameters, newLayoutSize, ellipsisEnabled, isMarqueeEnabled,
                                                                      isMarqueeMaxTextureExceeded, isHiddenInputEnabled, ellipsisPosition);
  };

  FontId replacementDefaultFontId = 0u;
  if(hasActiveReplacement)
  {
    const bool                             textFit            = parameters.isTextFitEnabled || parameters.isTextFitCandidatesEnabled;
    const float                            effectiveTextScale = textFit ? 1.0f : parameters.effectiveTextScale;
    const float                            fontScale          = effectiveTextScale * parameters.renderScale;
    const uint32_t                         pointsPerUnit      = mModule.GetFontClient().GetNumberOfPointsPerOneUnitOfPointSize();
    const TextAbstraction::PointSize26Dot6 pointSize =
      static_cast<TextAbstraction::PointSize26Dot6>(parameters.fontSize * fontScale * pointsPerUnit);
    TextAbstraction::FontDescription defaultFontDescription;
    defaultFontDescription.family = parameters.fontFamily;
    defaultFontDescription.weight = parameters.fontWeight;
    defaultFontDescription.width  = parameters.fontWidth;
    defaultFontDescription.slant  = parameters.fontSlant;

    ReplacementLayoutData replacementLayoutData;
    replacementLayoutData.runs                = &replacementState->projection.GetReplacementRuns();
    replacementDefaultFontId                  = mModule.GetFontClient().GetFontId(defaultFontDescription, pointSize);
    replacementLayoutData.defaultFontId       = replacementDefaultFontId;
    replacementLayoutData.horizontalAlignment = parameters.horizontalAlignment;
    replacementLayoutData.layoutDirection     = parameters.layoutDirection;
    replacementLayoutData.matchLayoutDirection =
      mTextModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS;
    layoutText(&replacementLayoutData);
  }
  else
  {
    layoutText(nullptr);
  }

  mTextModel->mVisualModel->SetLayoutSize(newLayoutSize);
  if(mTypesetter)
  {
    mTypesetter->SetFinalElisionResult(nullptr);
  }
  mEndEllipsisResult.reset();
  bool hasEndEllipsisCandidate = false;
  for(const LineRun& line : mTextModel->mVisualModel->mLines)
  {
    hasEndEllipsisCandidate |= line.ellipsis;
  }
  const bool hasNoVisibleEndLine = !updated && mTextModel->mVisualModel->mLines.Empty() &&
                                   !mTextModel->mLogicalModel->mText.Empty();
  if(!hasActiveReplacement && ellipsisEnabled && ellipsisPosition == EllipsisPosition::END &&
     (hasEndEllipsisCandidate || hasNoVisibleEndLine))
  {
    mEndEllipsisResult  = std::make_unique<FinalElisionResult>();
    const bool resolved = ResolveEndEllipsis(*mTextModel,
                                             textLayoutArea,
                                             mModule.GetFontClient(),
                                             *mEndEllipsisResult);
    DALI_ASSERT_DEBUG(resolved && mEndEllipsisResult->resolved &&
                      "Supported async END layout must publish an authoritative final result");
    if(resolved)
    {
      newLayoutSize = mEndEllipsisResult->layoutSize;
    }
    else
    {
      mEndEllipsisResult.reset();
    }
  }
  mIsTextDirectionRTL = false;

  if(!mTextModel->mVisualModel->mLines.Empty())
  {
    mIsTextDirectionRTL = mTextModel->mVisualModel->mLines[0u].direction;
  }

  // Store the actual size of the text after it has been laid-out.
  // The source VisualModel keeps the pre-elision layout size set above.
  // newLayoutSize may now be the authoritative final-domain output size.

  ////////////////////////////////////////////////////////////////////////////////
  // Align the text.
  ////////////////////////////////////////////////////////////////////////////////

  mTextModel->mHorizontalAlignment = parameters.horizontalAlignment;

  Vector<LineRun>& lines = mTextModel->mVisualModel->mLines; // The laid out lines.

  // Calculate the horizontal offset according with the given alignment.
  float alignmentOffset = 0.f;

  // Need to align with the control's size as the text may contain lines
  // starting either with left to right text or right to left.
  AlignTextLines(mLayoutEngine,
                 textLayoutArea,
                 0u,
                 numberOfCharacters,
                 *mTextModel,
                 lines,
                 alignmentOffset,
                 parameters.layoutDirection,
                 mTextModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS);
  if(mEndEllipsisResult)
  {
    FinalizeEndEllipsisGeometry(*mTextModel,
                                textLayoutArea,
                                parameters.layoutDirection,
                                mTextModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS,
                                mLayoutEngine,
                                *mEndEllipsisResult);
  }

  // Calculate vertical offset.
  Size layoutSize = mEndEllipsisResult && mEndEllipsisResult->HasAuthoritativeLayout()
                      ? mEndEllipsisResult->layoutSize
                      : mTextModel->mVisualModel->GetLayoutSize();

  switch(parameters.verticalAlignment)
  {
    case Text::Alignment::START:
    {
      mTextModel->mScrollPosition.y = 0.f;
      break;
    }
    case Text::Alignment::CENTER:
    {
      mTextModel->mScrollPosition.y = floorf(0.5f * (textLayoutArea.height - layoutSize.height));
      break;
    }
    case Text::Alignment::END:
    {
      mTextModel->mScrollPosition.y = textLayoutArea.height - layoutSize.height;
      break;
    }
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ControlSize : %f, %f, LayoutSize : %f, %f\n", textLayoutArea.x, textLayoutArea.y,
                          newLayoutSize.x, newLayoutSize.y);
  }
#endif

  // Every secondary layout/text-fit/max-size path calls Layout(), so this is the final aligned placement source.
  UpdateReplacementProcessing(parameters, replacementDefaultFontId);

  if(replacementState && replacementState->processingModel &&
     replacementState->projection.HasReplacements())
  {
    if(!replacementState->processingModel->mVisualModel->mLines.Empty())
    {
      mIsTextDirectionRTL = replacementState->processingModel->mVisualModel->mLines[0u].direction;
    }
    return replacementState->layoutSize;
  }

  return newLayoutSize;
}

AsyncTextRenderInfo AsyncTextLoader::Render(AsyncTextParameters& parameters)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_RENDER");

  ReplacementRenderState* replacementState = mReplacementData ? &mReplacementData->renderState : nullptr;
  ModelPtr                renderModel      = (replacementState && replacementState->processingModel &&
                          replacementState->projection.HasReplacements())
                                               ? replacementState->processingModel
                                               : mTextModel;

  // render test
  mTypesetter->SetFontClient(mModule.GetFontClient());
  mTypesetter->SetModel(renderModel.Get());
  if(replacementState && replacementState->projection.HasReplacements())
  {
    mTypesetter->SetFinalElisionResult(&replacementState->finalElision);
  }
  else if(mEndEllipsisResult && mEndEllipsisResult->resolved)
  {
    mTypesetter->SetFinalElisionResult(mEndEllipsisResult.get());
  }

  // Check whether it is a markup text with multiple text colors
  const Vector4* const    colorsBuffer       = renderModel->GetColors();
  const ColorIndex* const colorIndicesBuffer = renderModel->GetColorIndices();

  const Text::GlyphInfo* const glyphsBuffer          = renderModel->GetGlyphs();
  const Text::Length           numberOfGlyphs        = renderModel->GetNumberOfGlyphs();
  const bool                   hasColorIndexBuffer   = nullptr != colorsBuffer && nullptr != colorIndicesBuffer;
  TextAbstraction::FontClient& fontClient            = mModule.GetFontClient();
  bool                         hasMultipleTextColors = false;
  bool                         containsColorGlyph    = false;
  for(Text::Length glyphIndex = 0; glyphIndex < numberOfGlyphs; glyphIndex++)
  {
    if(hasColorIndexBuffer && *(colorIndicesBuffer + glyphIndex) > 0u)
    {
      hasMultipleTextColors = true;
    }

    if(!containsColorGlyph)
    {
      const Text::GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;
      if(IsRenderableColorGlyph(fontClient, glyphInfo->fontId, glyphInfo->index))
      {
        containsColorGlyph = true;
      }
    }

    if(hasMultipleTextColors && containsColorGlyph)
    {
      break;
    }
  }

  // Check whether the text contains any style colors (e.g. underline color, shadow color, etc.)
  const bool     shadowEnabled = renderModel->IsShadowEnabled();
  const Vector2& shadowOffset  = renderModel->GetShadowOffset();

  const bool outlineEnabled    = renderModel->IsOutlineEnabled();
  const bool backgroundEnabled = renderModel->IsBackgroundEnabled();
  // Legacy "Markup" accessors also report range decoration runs produced by StyledText spans.
  const bool underlineRunEnabled         = renderModel->IsMarkupUnderlineSet();
  const bool strikethroughRunEnabled     = renderModel->IsMarkupStrikethroughSet();
  const bool underlineEnabled            = renderModel->IsUnderlineEnabled() || underlineRunEnabled;
  const bool strikethroughEnabled        = renderModel->IsStrikethroughEnabled() || strikethroughRunEnabled;
  const bool backgroundMarkupSet         = renderModel->IsMarkupBackgroundColorSet();
  const bool cutoutEnabled               = renderModel->IsCutoutEnabled();
  const bool backgroundWithCutoutEnabled = renderModel->IsBackgroundWithCutoutEnabled();
  const bool styleTextureEnabled         = shadowEnabled || outlineEnabled || backgroundEnabled || backgroundMarkupSet;
  const bool styleBlocksTextGradient     = cutoutEnabled || backgroundWithCutoutEnabled;
  const bool styleEnabled                = styleTextureEnabled || styleBlocksTextGradient;
  const bool isOverlayStyle              = underlineEnabled || strikethroughEnabled;
  const bool embossEnabled               = parameters.isEmbossEnabled;

  // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
  Pixel::Format textPixelFormat =
    (containsColorGlyph || hasMultipleTextColors || cutoutEnabled) ? Pixel::RGBA8888 : Pixel::L8;

  // The width is the control's width, height is the minimum height of the text.
  // This calculated layout size determines the size of the pixel data buffer.
  Size layoutSize = renderModel->mVisualModel->GetLayoutSize();
  layoutSize.x    = parameters.textWidth;

  if(parameters.isMarqueeEnabled && parameters.marqueeOrientation == Text::MarqueeOrientation::VERTICAL)
  {
    layoutSize.y = parameters.textHeight;
  }

  if(shadowEnabled && shadowOffset.y > Math::MACHINE_EPSILON_1)
  {
    layoutSize.y += shadowOffset.y;
  }

  float outlineWidth = outlineEnabled ? renderModel->GetOutlineWidth() : 0.0f;
  layoutSize.y += outlineWidth * 2.0f;
  layoutSize.y = std::min(layoutSize.y, parameters.textHeight);

  if(cutoutEnabled)
  {
    // We need to store the offset including padding and vertical alignment.
    float xOffset = parameters.padding.start;
    float yOffset = parameters.padding.top + std::round((parameters.textHeight - layoutSize.y) *
                                                        VERTICAL_ALIGNMENT_TABLE[static_cast<int>(parameters.verticalAlignment)]);
    renderModel->mVisualModel->SetOffsetWithCutout(Vector2(xOffset, yOffset));

    // The layout size is set to the text control size including padding.
    layoutSize.x = parameters.textWidth + (parameters.padding.start + parameters.padding.end);
    layoutSize.y = parameters.textHeight + (parameters.padding.top + parameters.padding.bottom);
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ControlSize : %f, %f, LayoutSize : %f, %f\n", parameters.textWidth, parameters.textHeight,
                          layoutSize.x, layoutSize.y);
  }
#endif

  // Check the text direction
  Direction textDirection = mIsTextDirectionRTL ? Direction::RIGHT_TO_LEFT : Direction::LEFT_TO_RIGHT;

  // Set information for creating pixel datas.
  AsyncTextRenderInfo renderInfo;

  if(!parameters.isMarqueeEnabled)
  {
    renderInfo.isMarqueeStartAnchorResolved          = true;
    renderInfo.isMarqueeFittingStartGeometryResolved = true;
    renderInfo.marqueeStartAnchor =
      ResolveMarqueeStartAnchor(mEndEllipsisResult.get(), renderModel->mVisualModel.Get());
    renderInfo.marqueeFittingStartGeometry =
      ResolveMarqueeFittingStartGeometry(renderModel.Get());
    const float geometryScale = std::max(parameters.renderScale, 1.0f);
    if(renderInfo.marqueeStartAnchor.valid)
    {
      renderInfo.marqueeStartAnchor.staticControlX /= geometryScale;
    }
    if(renderInfo.marqueeFittingStartGeometry.valid)
    {
      renderInfo.marqueeFittingStartGeometry.staticTranslation /= geometryScale;
    }
  }

  bool isRenderScale = parameters.renderScale > 1.0f ? true : false;
  if(isRenderScale)
  {
    float width     = layoutSize.width / parameters.renderScale;
    float height    = layoutSize.height / parameters.renderScale;
    renderInfo.size = Size(width, height);
  }
  else
  {
    renderInfo.size = layoutSize;
  }
  renderInfo.textLogicalBounds = CalculateGradientContentBounds(layoutSize,
                                                                renderModel->mVisualModel->GetLayoutSize(),
                                                                renderModel->mVisualModel->mLines.Begin(),
                                                                static_cast<Dali::Ui::Text::Length>(renderModel->mVisualModel->mLines.Count()),
                                                                parameters.verticalAlignment);
  const Text::ReplacementProjection* activeProjection =
    (replacementState && replacementState->processingModel && replacementState->projection.HasReplacements())
      ? &replacementState->projection
      : nullptr;
  renderInfo.anchorHitRegions = BuildAsyncAnchorHitRegions(mTextModel, renderModel, activeProjection, parameters);

  // Set the direction of text.
  renderInfo.isTextDirectionRTL = mIsTextDirectionRTL;

  PixelBuffer cutoutData;
  if(cutoutEnabled)
  {
    cutoutData = mTypesetter->RenderWithPixelBuffer(layoutSize, textDirection, Text::Typesetter::RENDER_NO_STYLES,
                                                    false, textPixelFormat);

    // Make transparent buffer.
    // If the cutout is enabled, a separate texture is not used for the text.
    PixelBuffer buffer       = mTypesetter->CreateFullBackgroundBuffer(1, 1, Color::TRANSPARENT);
    renderInfo.textPixelData = PixelBuffer::Convert(buffer);

    // Set the flag of cutout.
    renderInfo.isCutoutEnabled = cutoutEnabled && (cutoutData != nullptr);
  }
  else
  {
    // Create a pixel data for the text without any styles
    renderInfo.textPixelData =
      mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);
  }

  renderInfo.isTextRevealEnabled = parameters.isTextRevealEnabled && !cutoutEnabled && !parameters.isMarqueeEnabled;
  if(renderInfo.isTextRevealEnabled && renderInfo.textPixelData)
  {
    const auto     sourceRevealPlan = parameters.textRevealUnit == Internal::Reveal::Unit::WORD
                                        ? Internal::Reveal::BuildPlan(*renderModel,
                                                                      parameters.textRevealUnit,
                                                                      parameters.textRevealFadeDurationRatio,
                                                                      mModule.GetSegmentation())
                                        : Internal::Reveal::BuildCharacterPlan(*renderModel,
                                                                               parameters.textRevealFadeDurationRatio);
    const auto     revealPlan       = mTypesetter->CreateFinalRevealPlan(sourceRevealPlan, parameters.textRevealUnit);
    const uint32_t metadataWidth    = renderInfo.textPixelData.GetWidth();
    const uint32_t metadataHeight   = renderInfo.textPixelData.GetHeight();
    const uint32_t tileLimit        = parameters.maxTextureSize > 0
                                        ? static_cast<uint32_t>(parameters.maxTextureSize)
                                        : metadataHeight;
    const Vector2  fullMetadataSize(static_cast<float>(metadataWidth), static_cast<float>(metadataHeight));
    const bool     isRendererTiled = parameters.maxTextureSize > 0 &&
                                 renderInfo.size.height > static_cast<float>(parameters.maxTextureSize);
    // With render scale, the worker raster is larger than the renderer's
    // logical projection. Height tiling uploads only the renderer-sized region
    // from that raster, so Reveal metadata must use those exact tile extents.
    const uint32_t metadataTileWidth   = isRendererTiled
                                           ? static_cast<uint32_t>(renderInfo.size.width)
                                           : metadataWidth;
    const uint32_t tiledMetadataHeight = isRendererTiled
                                           ? static_cast<uint32_t>(renderInfo.size.height)
                                           : metadataHeight;
    for(uint32_t offsetY = 0u; offsetY < tiledMetadataHeight; offsetY += tileLimit)
    {
      const uint32_t tileHeight = std::min(tileLimit, tiledMetadataHeight - offsetY);
      renderInfo.revealMetadataTiles.push_back(
        mTypesetter->RenderTextRevealMetadata(
          Vector2(static_cast<float>(metadataTileWidth), static_cast<float>(tileHeight)), textDirection,
          revealPlan, renderInfo.textRevealFadeDuration, offsetY, fullMetadataSize));
    }
  }

  if(styleEnabled)
  {
    if(renderInfo.isCutoutEnabled)
    {
      float cutoutAlpha         = renderModel->GetDefaultColor().a;
      renderInfo.stylePixelData = mTypesetter->RenderWithCutout(
        layoutSize, textDirection, cutoutData, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888, cutoutAlpha);
    }
    else
    {
      // Create RGBA pixel data for all the text styles (without the text itself)
      renderInfo.stylePixelData =
        mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);
    }
  }
  if(isOverlayStyle)
  {
    // Create RGBA pixel data for all the overlay styles
    renderInfo.overlayStylePixelData =
      mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_OVERLAY_STYLE, false, Pixel::RGBA8888);
  }
  if(containsColorGlyph && !hasMultipleTextColors)
  {
    // Create a L8 pixel data as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
    renderInfo.maskPixelData =
      mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);
  }
  const bool textGradientMixedPayloadBaseAllowed =
    parameters.isTextGradientRequested &&
    !styleBlocksTextGradient &&
    !embossEnabled &&
    !cutoutEnabled;
  const bool hasMixedColorTarget = hasMultipleTextColors || containsColorGlyph;
  const bool nonMarqueeTextGradientMixedPayload =
    textGradientMixedPayloadBaseAllowed &&
    !parameters.isMarqueeEnabled &&
    hasMixedColorTarget;
  const bool marqueeTextGradientMixedPayload =
    textGradientMixedPayloadBaseAllowed &&
    parameters.isMarqueeEnabled &&
    hasMixedColorTarget;
  // Mixed preserved/mask payload is driven by the base TextGradient. Overlay-only
  // mixed targets keep the simple overlay path instead of allocating mixed payloads.
  if(nonMarqueeTextGradientMixedPayload || marqueeTextGradientMixedPayload)
  {
    const bool ignoreHorizontalAlignment = parameters.isMarqueeEnabled &&
                                           parameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL;
    const Vector2 originSize = parameters.isMarqueeEnabled ? Size(parameters.originWidth, parameters.originHeight)
                                                           : Size::ZERO;
    renderInfo.textGradientPreservedPixelData =
      mTypesetter->RenderTextGradientPreserved(layoutSize, textDirection, ignoreHorizontalAlignment, Pixel::RGBA8888, originSize);
    renderInfo.textGradientMaskPixelData =
      mTypesetter->RenderTextGradientMask(layoutSize, textDirection, ignoreHorizontalAlignment, Pixel::L8, originSize);
  }

  const bool marqueeStylePayload =
    parameters.isMarqueeEnabled &&
    styleTextureEnabled &&
    !styleBlocksTextGradient &&
    !embossEnabled &&
    !cutoutEnabled &&
    !backgroundWithCutoutEnabled &&
    (hasMixedColorTarget ? parameters.isTextGradientRequested
                         : (parameters.isTextGradientRequested || parameters.isTextGradientOverlayRequested));
  if(marqueeStylePayload)
  {
    const bool    ignoreHorizontalAlignment = parameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL;
    const Vector2 originSize(parameters.originWidth, parameters.originHeight);
    if(!hasMixedColorTarget)
    {
      renderInfo.marqueeFillPixelData =
        mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_NO_STYLES,
                            ignoreHorizontalAlignment, Pixel::RGBA8888, originSize);
    }
    renderInfo.marqueeStylePixelData =
      mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_NO_TEXT,
                          ignoreHorizontalAlignment, Pixel::RGBA8888, originSize);
  }

  if(parameters.isMarqueeEnabled && isOverlayStyle)
  {
    const bool    ignoreHorizontalAlignment = parameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL;
    const Vector2 originSize(parameters.originWidth, parameters.originHeight);
    renderInfo.marqueeOverlayStylePixelData =
      mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_OVERLAY_STYLE,
                          ignoreHorizontalAlignment, Pixel::RGBA8888, originSize);
  }

  if(parameters.isMarqueeEnabled)
  {
    // This will be uploaded in async text interface's setup marquee.
    // Resolve immediately after the primary text render. Later supplemental
    // render passes must not replace the texture geometry used by the delta.
    renderInfo.marqueePixelData =
      mTypesetter->Render(layoutSize, textDirection, Text::Typesetter::RENDER_TEXT_AND_STYLES,
                          parameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL, Pixel::RGBA8888,
                          Size(parameters.originWidth, parameters.originHeight));
    if(parameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL)
    {
      renderInfo.marqueeStartAnchor = parameters.marqueeStartAnchor;
      renderInfo.marqueeTextureAnchor =
        mTypesetter->ResolveMarqueeTextureAnchor(parameters.marqueeStartAnchor);
      if(renderInfo.marqueeTextureAnchor.valid)
      {
        const float geometryScale = std::max(parameters.renderScale, 1.0f);
        renderInfo.marqueeTextureAnchor.textureX /= geometryScale;
      }
    }
  }

  renderInfo.hasMultipleTextColors   = hasMultipleTextColors;
  renderInfo.containsColorGlyph      = containsColorGlyph;
  renderInfo.styleEnabled            = styleEnabled;
  renderInfo.styleTextureEnabled     = styleTextureEnabled;
  renderInfo.styleBlocksTextGradient = styleBlocksTextGradient;
  renderInfo.isOverlayStyle          = isOverlayStyle;
  renderInfo.lineCount               = renderModel->GetNumberOfLines();
  renderInfo.isEmbossEnabled         = embossEnabled;

  if(cutoutEnabled)
  {
    renderInfo.renderedSize = renderInfo.size;
  }
  else
  {
    float renderedWidth     = isRenderScale ? parameters.renderScaleWidth : parameters.textWidth;
    float renderedHeight    = isRenderScale ? parameters.renderScaleHeight : parameters.textHeight;
    renderInfo.renderedSize = Size(renderedWidth, renderedHeight);
  }

  CopyReplacementResult(renderInfo, parameters.renderScale);

  return renderInfo;
}

AsyncTextRenderInfo AsyncTextLoader::RenderText(AsyncTextParameters& parameters, bool useCachedNaturalSize,
                                                const Size& naturalSize)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_RENDER_TEXT");

  Size textNaturalSize   = naturalSize;
  bool cachedNaturalSize = useCachedNaturalSize;

  if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_HEIGHT || parameters.requestType == Ui::Integration::Text::Async::RENDER_CONSTRAINT)
  {
    if(!cachedNaturalSize)
    {
      textNaturalSize   = ComputeNaturalSize(parameters);
      cachedNaturalSize = true;
    }
    // textWidth is widthConstraint
    if(parameters.textWidth > textNaturalSize.width)
    {
      parameters.textWidth = textNaturalSize.width;
    }
  }

  if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_WIDTH || parameters.requestType == Ui::Integration::Text::Async::RENDER_CONSTRAINT)
  {
    // In case of CONSTRAINT, the natural size has already been calculated.
    // So we can skip Initialize and Update at this stage.
    // Only the layout is newly calculated to obtain the height.
    bool  layoutOnly = cachedNaturalSize;
    float height     = ComputeHeightForWidth(parameters, parameters.textWidth, layoutOnly);

    // textHeight is heightConstraint.
    if(parameters.textHeight < height)
    {
      bool layoutUpdated = false;
      // Re-layout is required to apply new height.
      Layout(parameters, layoutUpdated);
    }
    else
    {
      parameters.textHeight = height;
    }

    mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);
  }
  else
  {
    if(!cachedNaturalSize)
    {
      Initialize();
      Update(parameters);
    }
    bool layoutUpdated = false;
    Layout(parameters, layoutUpdated);
  }

  return Render(parameters);
}

float AsyncTextLoader::ComputeHeightForWidth(AsyncTextParameters& parameters, float width, bool layoutOnly)
{
#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ComputeHeightForWidth, width:%f, layoutOnly:%d\n", width, layoutOnly);
  }
#endif
  return ComputeLayoutSize(parameters, width, MAX_FLOAT, layoutOnly).height;
}

Size AsyncTextLoader::ComputeLayoutSize(AsyncTextParameters& parameters, float width, float height, bool layoutOnly)
{
#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ComputeLayoutSize, width:%f, height:%f, layoutOnly:%d\n", width, height, layoutOnly);
  }
#endif

  float actualWidth  = parameters.textWidth;
  float actualHeight = parameters.textHeight;

  parameters.textWidth  = width;
  parameters.textHeight = height;

  if(!layoutOnly)
  {
    Initialize();
    Update(parameters);
  }

  bool layoutUpdated = false;
  Size layoutSize    = Layout(parameters, layoutUpdated);

  // Restore actual size.
  parameters.textWidth                   = actualWidth;
  parameters.textHeight                  = actualHeight;
  mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);

  return layoutSize;
}

Size AsyncTextLoader::SetupRenderScale(AsyncTextParameters& parameters, bool& cachedNaturalSize)
{
  if(parameters.isTextFitEnabled || parameters.isTextFitCandidatesEnabled)
  {
    // If text fit, only update the scaled size.
    parameters.renderScaleWidth  = parameters.textWidth;
    parameters.renderScaleHeight = parameters.textHeight;
    parameters.textWidth         = ConvertToEven(ceil(parameters.textWidth * parameters.renderScale));
    parameters.textHeight        = ConvertToEven(ceil(parameters.textHeight * parameters.renderScale));
    parameters.minLineSize       = parameters.minLineSize * parameters.renderScale;
    parameters.marqueeGap        = static_cast<int>(parameters.marqueeGap * parameters.renderScale);
    cachedNaturalSize            = false;
    return Size::ZERO;
  }

  float renderScale = parameters.renderScale;
  // Set render scale to 1.0 to compute the original scale natural size.
  parameters.renderScale   = 1.0f;
  Size originalNaturalSize = ComputeNaturalSize(parameters);

  // Restore render scale.
  parameters.renderScale = renderScale;

  // Check if the original text is ellipsized or not.
  bool widthEllipsized  = parameters.textWidth < originalNaturalSize.width ? true : false;
  bool heightEllipsized = parameters.textHeight < originalNaturalSize.height ? true : false;

  Size naturalSize = Size::ZERO;
  if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_HEIGHT || parameters.requestType == Ui::Integration::Text::Async::RENDER_CONSTRAINT)
  {
    naturalSize       = ComputeNaturalSize(parameters);
    cachedNaturalSize = true;
    if(parameters.textWidth > naturalSize.width)
    {
      parameters.textWidth = naturalSize.width;
    }
  }

  if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_WIDTH || parameters.requestType == Ui::Integration::Text::Async::RENDER_CONSTRAINT)
  {
    bool  layoutOnly = cachedNaturalSize;
    float height     = ComputeHeightForWidth(parameters, parameters.textWidth, layoutOnly);
    if(parameters.textHeight > height)
    {
      parameters.textHeight = height;
    }
  }

  if(!cachedNaturalSize)
  {
    naturalSize       = ComputeNaturalSize(parameters);
    cachedNaturalSize = true;
  }

  // Update the scaled size.
  parameters.renderScaleWidth  = parameters.textWidth;
  parameters.renderScaleHeight = parameters.textHeight;
  parameters.textWidth         = ConvertToEven(ceil(parameters.textWidth * parameters.renderScale));
  parameters.textHeight        = ConvertToEven(ceil(parameters.textHeight * parameters.renderScale));
  parameters.minLineSize       = parameters.minLineSize * parameters.renderScale;
  parameters.marqueeGap        = static_cast<int>(parameters.marqueeGap * parameters.renderScale);

  // The texture in RenderScale needs to be resized because it exceeds the control size.
  if(!widthEllipsized && naturalSize.width > parameters.textWidth)
  {
    float renderScaleGap = ceil(naturalSize.width - parameters.textWidth);
    if(renderScaleGap > 0.0f)
    {
      Vector<GlyphInfo>& glyphs         = mTextModel->mVisualModel->mGlyphs;
      const Length       numberOfGlyphs = static_cast<Length>(glyphs.Count());
      if(numberOfGlyphs > 1u)
      {
        naturalSize.width -= (renderScaleGap - 1.0f);
        parameters.textWidth = naturalSize.width;

        uint32_t numberOfAdvance = 0u;
        float    sumOfGap        = 0.0f;
        float    gap             = renderScaleGap / static_cast<float>(numberOfGlyphs - 1u);

        // Reduce the advance of all glyphs slightly to fit the width to the control size.
        // Reducing the advance of the last glyph is pointless.
        for(Length index = 0u; index < numberOfGlyphs - 1u; index++)
        {
          if(glyphs[index].advance > 0.0f)
          {
            glyphs[index].advance -= gap;
            numberOfAdvance++;
            sumOfGap += gap;
          }
        }

        // Remove all remaining gaps.
        if(numberOfAdvance > 0u && fabsf(renderScaleGap - sumOfGap) > Math::MACHINE_EPSILON_1000)
        {
          float remainedGap = (renderScaleGap - sumOfGap) / static_cast<float>(numberOfAdvance);
          for(Length index = 0u; index < numberOfGlyphs - 1u; index++)
          {
            if(glyphs[index].advance > 0.0f)
            {
              glyphs[index].advance -= remainedGap;
            }
          }
        }
      }
    }
  }

  // Adjust the size to ensure same ellipsis behavior as the original text.
  parameters.textWidth  = widthEllipsized ? std::min(parameters.textWidth, naturalSize.width - 1)
                                          : std::max(parameters.textWidth, naturalSize.width);
  parameters.textHeight = heightEllipsized ? std::min(parameters.textHeight, naturalSize.height - 1)
                                           : std::max(parameters.textHeight, naturalSize.height);

  // Update the control size because textWidth and textHeight have been adjusted. (Skip Initialize and Update)
  mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);

  return naturalSize;
}

Size AsyncTextLoader::ComputeNaturalSize(AsyncTextParameters& parameters)
{
#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ComputeNaturalSize\n");
  }
#endif

  float actualWidth  = parameters.textWidth;
  float actualHeight = parameters.textHeight;

  // To measure natural size, set the size of the control to the maximum.
  parameters.textWidth  = MAX_FLOAT;
  parameters.textHeight = MAX_FLOAT;

  Initialize();
  Update(parameters);
  bool layoutUpdated = false;

  Size naturalSize = Layout(parameters, layoutUpdated);

  // Restore actual size.
  parameters.textWidth                   = actualWidth;
  parameters.textHeight                  = actualHeight;
  mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);

  return naturalSize;
}

AsyncTextRenderInfo AsyncTextLoader::GetHeightForWidth(AsyncTextParameters& parameters)
{
  float height = ComputeHeightForWidth(parameters, parameters.textWidth, false);

  AsyncTextRenderInfo renderInfo;
  renderInfo.renderedSize.width  = parameters.textWidth;
  renderInfo.renderedSize.height = height;
  renderInfo.requestType         = Ui::Integration::Text::Async::COMPUTE_HEIGHT_FOR_WIDTH;
  CopyRenderModelSummary(renderInfo);
  CopyReplacementResult(renderInfo, parameters.renderScale);

  return renderInfo;
}

AsyncTextRenderInfo AsyncTextLoader::GetNaturalSize(AsyncTextParameters& parameters)
{
  Size textNaturalSize   = ComputeNaturalSize(parameters);
  textNaturalSize.width  = ConvertToEven(textNaturalSize.width);
  textNaturalSize.height = ConvertToEven(textNaturalSize.height);

  AsyncTextRenderInfo renderInfo;
  renderInfo.renderedSize = textNaturalSize;
  renderInfo.requestType  = Ui::Integration::Text::Async::COMPUTE_NATURAL_SIZE;
  CopyRenderModelSummary(renderInfo);
  CopyReplacementResult(renderInfo, parameters.renderScale);

  return renderInfo;
}

AsyncTextRenderInfo AsyncTextLoader::RenderMarquee(AsyncTextParameters& parameters, bool useCachedNaturalSize,
                                                   const Size& naturalSize)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_RENDER_MARQUEE");

  Size      controlSize(parameters.textWidth, parameters.textHeight);
  Size      verifiedSize;
  float     wrapGap               = 0.0f;
  bool      isHorizontal          = parameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL;
  const int maxTextureSize        = parameters.maxTextureSize;
  bool      isTextContentOverflow = false;

  if(isHorizontal)
  {
    // As relayout of text may not be done at this point natural size is used to get size. Single line scrolling only.
    Size textNaturalSize = useCachedNaturalSize ? naturalSize : ComputeNaturalSize(parameters);

    isTextContentOverflow = textNaturalSize.width > controlSize.width;

    if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_WIDTH || parameters.requestType == Ui::Integration::Text::Async::RENDER_CONSTRAINT)
    {
      // The real height calculated during layout should be set.
      parameters.textHeight                  = textNaturalSize.height;
      controlSize.height                     = parameters.textHeight;
      mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);
    }

#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      DALI_LOG_RELEASE_INFO("natural size : %f, %f, control size : %f, %f\n", textNaturalSize.x, textNaturalSize.y,
                            controlSize.x, controlSize.y);
    }
#endif

    // Calculate the actual gap before scrolling wraps.
    int textPadding     = static_cast<int>(std::max(controlSize.x - textNaturalSize.x, 0.0f));
    wrapGap             = static_cast<float>(std::max(parameters.marqueeGap, textPadding));
    Vector2 textureSize = textNaturalSize + Vector2(wrapGap, 0.0f); // Add the gap as a part of the texture.

    // Calculate a size of texture for text scrolling
    verifiedSize = textureSize;

    // If the texture size width exceed maxTextureSize, modify the visual model size and enabled the ellipsis.
    if(verifiedSize.width > maxTextureSize)
    {
      verifiedSize.width = static_cast<float>(maxTextureSize);
      if(textNaturalSize.width > maxTextureSize)
      {
        float actualWidth  = parameters.textWidth;
        float actualHeight = parameters.textHeight;

        parameters.textWidth                   = verifiedSize.width - static_cast<float>(parameters.marqueeGap);
        parameters.textHeight                  = textNaturalSize.height;
        parameters.isMarqueeMaxTextureExceeded = true;

        bool layoutUpdated = false;

        // Re-layout is required to apply ellipsis.
        Layout(parameters, layoutUpdated);

        parameters.textWidth  = actualWidth;
        parameters.textHeight = actualHeight;
      }
      wrapGap = std::max(maxTextureSize - textNaturalSize.width, static_cast<float>(parameters.marqueeGap));
    }
  }
  else // MarqueeOrientation::VERTICAL
  {
    bool  layoutOnly      = useCachedNaturalSize;
    bool  useCachedHeight = false;
    float textHeight      = 0.0f;
    if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_WIDTH || parameters.requestType == Ui::Integration::Text::Async::RENDER_CONSTRAINT)
    {
      // The real height calculated during layout should be set.
      textHeight      = ComputeHeightForWidth(parameters, parameters.textWidth, layoutOnly);
      useCachedHeight = true;
      layoutOnly      = true;

      if(parameters.textHeight > textHeight)
      {
        parameters.textHeight                  = textHeight;
        controlSize.height                     = parameters.textHeight;
        mTextModel->mVisualModel->mControlSize = Size(parameters.textWidth, parameters.textHeight);
      }
    }

    Size originSize                = Size::ZERO;
    bool needLayoutSizeCalculation = parameters.verticalAlignment != Text::Alignment::START ? true : false;
    if(needLayoutSizeCalculation)
    {
      parameters.isMarqueeEnabled = false;
      originSize                  = ComputeLayoutSize(parameters, parameters.textWidth, parameters.textHeight, layoutOnly);
      parameters.isMarqueeEnabled = true;
      parameters.originWidth      = originSize.width;
      parameters.originHeight     = originSize.height;
      layoutOnly                  = true;
    }

    textHeight = useCachedHeight ? textHeight : ComputeHeightForWidth(parameters, parameters.textWidth, layoutOnly);

    // Calculate the actual gap before scrolling wraps.
    int textPadding = static_cast<int>(std::max(controlSize.y - textHeight, 0.0f));
    wrapGap         = static_cast<float>(std::max(parameters.marqueeGap, textPadding));
    Vector2 textureSize(controlSize.width, textHeight + wrapGap); // Add the gap as a part of the texture

    // Calculate a size of texture for text scrolling
    verifiedSize = textureSize;

    // If the texture size height exceed maxTextureSize, modify the visual model size and enabled the ellipsis.
    if(verifiedSize.height > maxTextureSize)
    {
      verifiedSize.height = static_cast<float>(maxTextureSize);
      if(textHeight > maxTextureSize)
      {
        float actualWidth  = parameters.textWidth;
        float actualHeight = parameters.textHeight;

        parameters.textWidth        = verifiedSize.width;
        parameters.textHeight       = verifiedSize.height;
        parameters.isMarqueeEnabled = false;

        bool layoutUpdated = false;

        // Re-layout is required to apply ellipsis.
        Layout(parameters, layoutUpdated);

        parameters.textWidth        = actualWidth;
        parameters.textHeight       = actualHeight;
        parameters.isMarqueeEnabled = true;
      }
      wrapGap = std::max(maxTextureSize - textHeight, 0.0f);
    }
  }

  uint32_t actualWidth  = static_cast<uint32_t>(parameters.textWidth);
  uint32_t actualHeight = static_cast<uint32_t>(parameters.textHeight);
  parameters.textWidth  = verifiedSize.width;
  parameters.textHeight = verifiedSize.height;

  AsyncTextRenderInfo renderInfo = Render(parameters);
  renderInfo.textGradientMarqueeViewportBounds =
    CalculateMarqueeGradientViewportBounds(controlSize,
                                           mTextModel->mVisualModel->GetLayoutSize(),
                                           mTextModel->mVisualModel->mLines.Begin(),
                                           static_cast<Dali::Ui::Text::Length>(mTextModel->mVisualModel->mLines.Count()),
                                           parameters.horizontalAlignment,
                                           parameters.verticalAlignment);

  parameters.textWidth  = static_cast<float>(actualWidth);
  parameters.textHeight = static_cast<float>(actualHeight);

  // Store the control size and calculated wrap gap in render info.
  bool  isRenderScale                 = parameters.renderScale > 1.0f ? true : false;
  float renderedWidth                 = isRenderScale ? parameters.renderScaleWidth : controlSize.width;
  float renderedHeight                = isRenderScale ? parameters.renderScaleHeight : controlSize.height;
  renderInfo.controlSize              = Size(renderedWidth, renderedHeight);
  renderInfo.renderedSize             = Size(renderedWidth, renderedHeight);
  renderInfo.marqueeWrapGap           = wrapGap;
  renderInfo.isMarqueeContentOverflow = isTextContentOverflow;
  return renderInfo;
}

bool AsyncTextLoader::CheckForTextFit(AsyncTextParameters& parameters, float pointSize, const Size& allowedSize)
{
  parameters.fontSize = pointSize;

  Initialize();
  Update(parameters);
  bool layoutUpdated = false;
  Size layoutSize    = Layout(parameters, layoutUpdated);

  if(!layoutUpdated || layoutSize.width > allowedSize.width || layoutSize.height > allowedSize.height)
  {
    return false;
  }
  return true;
}

AsyncTextRenderInfo AsyncTextLoader::RenderTextFit(AsyncTextParameters& parameters, bool useCachedNaturalSize,
                                                   const Size& naturalSize)
{
  Size textNaturalSize   = naturalSize;
  bool cachedNaturalSize = useCachedNaturalSize;

  if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_HEIGHT || parameters.requestType == Ui::Integration::Text::Async::RENDER_CONSTRAINT)
  {
    if(!cachedNaturalSize)
    {
      textNaturalSize   = ComputeNaturalSize(parameters);
      cachedNaturalSize = true;
    }
    // textWidth is widthConstraint
    if(parameters.textWidth > textNaturalSize.width)
    {
      parameters.textWidth = textNaturalSize.width;
    }
  }

  if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_WIDTH || parameters.requestType == Ui::Integration::Text::Async::RENDER_CONSTRAINT)
  {
    // In case of CONSTRAINT, the natural size has already been calculated.
    // So we can skip Initialize and Update at this stage.
    // Only the layout is newly calculated to obtain the height.
    bool  layoutOnly = cachedNaturalSize;
    float height     = ComputeHeightForWidth(parameters, parameters.textWidth, layoutOnly);

    // textHeight is heightConstraint
    if(parameters.textHeight > height)
    {
      parameters.textHeight = height;
    }
    DALI_LOG_DEBUG_INFO("TextFit requires a fixed size. Render with natural size : %f, %f\n", parameters.textWidth,
                        parameters.textHeight);
  }

  if(parameters.isTextFitCandidatesEnabled)
  {
#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      DALI_LOG_RELEASE_INFO("AsyncTextLoader::RenderTextFit -> TextFit candidate mode\n");
    }
#endif

    Dali::Vector<Ui::Text::Fit::Candidate> fitCandidates  = parameters.textFitCandidates;
    int                                    candidateCount = static_cast<int>(fitCandidates.Count());

    if(candidateCount == 0)
    {
      DALI_LOG_ERROR("fitCandidates is empty, render with default value, point size:%f, line height:%f\n",
                     parameters.fontSize, parameters.minLineSize);
      fitCandidates.PushBack(Ui::Text::Fit::Candidate(ConvertPointToPixel(parameters.fontSize, mModule.GetFontClient()), parameters.minLineSize));
      candidateCount = 1;
    }

    mFitActualEllipsis  = parameters.ellipsis;
    parameters.ellipsis = false;

    Size allowedSize(parameters.textWidth, parameters.textHeight);

    // Sort in ascending order by font size.
    std::sort(fitCandidates.Begin(), fitCandidates.End(), compareByPointSize);

    // Decide whether to use binary search.
    // If lineHeight is not sorted in ascending order,
    // binary search cannot guarantee that it will always find the best value.
    bool  binarySearch   = true;
    float prevLineHeight = 0.0f;
    for(auto it = fitCandidates.Begin(); it != fitCandidates.End(); ++it)
    {
      const float candidateLineHeight = it->GetLineHeight();
      if(prevLineHeight > candidateLineHeight)
      {
        binarySearch = false;
        break;
      }
      prevLineHeight = candidateLineHeight;
    }

    // Set the first candidate (minimum font size) as the default best value.
    // If the search does not find an optimal value, the minimum point size will be used to text fit.
    const Ui::Text::Fit::Candidate& firstCandidate        = fitCandidates[0];
    bool                            bestSizeUpdatedLatest = false;
    float                           bestPointSize         = ConvertPixelToPoint(firstCandidate.GetFontSize(), mModule.GetFontClient()) * parameters.effectiveTextScale;
    float                           bestLineHeight        = firstCandidate.GetLineHeight();

    if(binarySearch)
    {
      int left  = 0;
      int right = candidateCount - 1;

      while(left <= right)
      {
        const int                       mid            = left + (right - left) / 2;
        const Ui::Text::Fit::Candidate& candidate      = fitCandidates[mid];
        const float                     testPointSize  = ConvertPixelToPoint(candidate.GetFontSize(), mModule.GetFontClient()) * parameters.effectiveTextScale;
        const float                     testLineHeight = candidate.GetLineHeight();
        parameters.minLineSize                         = testLineHeight;

        if(CheckForTextFit(parameters, testPointSize, allowedSize))
        {
          bestSizeUpdatedLatest = true;
          bestPointSize         = testPointSize;
          bestLineHeight        = testLineHeight;
          left                  = mid + 1;
        }
        else
        {
          bestSizeUpdatedLatest = false;
          right                 = mid - 1;
        }
      }
    }
    else
    {
      // If binary search is not possible, search sequentially from the largest font size.
      for(auto it = fitCandidates.End(); it != fitCandidates.Begin();)
      {
        --it;

        const float testPointSize  = ConvertPixelToPoint(it->GetFontSize(), mModule.GetFontClient()) * parameters.effectiveTextScale;
        const float testLineHeight = it->GetLineHeight();
        parameters.minLineSize     = testLineHeight;

        if(CheckForTextFit(parameters, testPointSize, allowedSize))
        {
          bestSizeUpdatedLatest = true;
          bestPointSize         = testPointSize;
          bestLineHeight        = testLineHeight;
          break;
        }
        else
        {
          bestSizeUpdatedLatest = false;
        }
      }
    }

    // Best point size was not updated. Re-run so the text fit is really applied.
    if(!bestSizeUpdatedLatest)
    {
      parameters.ellipsis    = mFitActualEllipsis;
      parameters.minLineSize = bestLineHeight;
      CheckForTextFit(parameters, bestPointSize, allowedSize);
    }

    return Render(parameters);
  }
  else if(parameters.isTextFitEnabled)
  {
#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      DALI_LOG_RELEASE_INFO("AsyncTextLoader::RenderTextFit -> TextFit\n");
    }
#endif

    float minPointSize  = parameters.textFitMinSize * parameters.effectiveTextScale;
    float maxPointSize  = parameters.textFitMaxSize * parameters.effectiveTextScale;
    float pointInterval = parameters.textFitStepSize * parameters.effectiveTextScale;

    mFitActualEllipsis  = parameters.ellipsis;
    parameters.ellipsis = false;
    float bestPointSize = minPointSize;

    Size allowedSize(parameters.textWidth, parameters.textHeight);

    // check zero value
    if(pointInterval < 1.f)
    {
      pointInterval = 1.0f;
    }

    uint32_t pointSizeRange = static_cast<uint32_t>(ceil((maxPointSize - minPointSize) / pointInterval));

    // Ensure minPointSize + pointSizeRange * pointInterval >= maxPointSize
    while(minPointSize + static_cast<float>(pointSizeRange) * pointInterval < maxPointSize)
    {
      ++pointSizeRange;
    }

    uint32_t bestSizeIndex = 0;
    uint32_t minIndex      = bestSizeIndex + 1u;
    uint32_t maxIndex      = pointSizeRange + 1u;

    bool bestSizeUpdatedLatest = false;
    // Find best size as binary search.
    // Range format as [l r). (left closed, right opened)
    // It means, we already checked all i < l are valid, and r <= i are invalid.
    // Below binary search will check m = (l+r)/2 point.
    // Search area separates as [l m) or [m+1 r)
    //
    // Basically, we can assume that 0 (minPointSize) is always valid.
    // Now, we will check [1 pointSizeRange] range where pointSizeRange means the maxPointSize.
    while(minIndex < maxIndex)
    {
      const uint32_t testIndex     = minIndex + ((maxIndex - minIndex) >> 1u);
      const float    testPointSize = std::min(maxPointSize, minPointSize + static_cast<float>(testIndex) * pointInterval);

      if(CheckForTextFit(parameters, testPointSize, allowedSize))
      {
        bestSizeUpdatedLatest = true;

        bestSizeIndex = testIndex;
        minIndex      = testIndex + 1u;
      }
      else
      {
        bestSizeUpdatedLatest = false;
        maxIndex              = testIndex;
      }
    }
    bestPointSize = std::min(maxPointSize, minPointSize + static_cast<float>(bestSizeIndex) * pointInterval);

    // Best point size was not updated. Re-run so the text fit is really applied.
    if(!bestSizeUpdatedLatest)
    {
      parameters.ellipsis = mFitActualEllipsis;
      CheckForTextFit(parameters, bestPointSize, allowedSize);
    }

    return Render(parameters);
  }
  else
  {
    DALI_LOG_ERROR("There is no TextFit information in AsyncTextParameters. It returns empty AsyncTextRenderInfo.\n");
    AsyncTextRenderInfo renderInfo;
    return renderInfo;
  }
}

} // namespace Internal

} // namespace Text

} // namespace Ui

} // namespace Dali
