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
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/clipboard-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <memory.h>
#include <cmath>
#include <limits>
#include <regex>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/text-editable-control-interface.h>
#include <dali-ui-foundation/internal/controls/text-controls/common-text-utils.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-background-actor.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-event-handler.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-input-font-handler.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-input-properties.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-placeholder-handler.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-relayouter.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-text-updater.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/text-geometry.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const char* EMPTY_STRING         = "";
const char* MIME_TYPE_TEXT_PLAIN = "text/plain;charset=utf-8";
const char* MIME_TYPE_HTML       = "application/xhtml+xml";

template<typename Type>
void EnsureCreated(Type*& object)
{
  if(!object)
  {
    object = new Type();
  }
}

template<typename Type>
void EnsureCreated(std::unique_ptr<Type>& object)
{
  if(!object)
  {
    object = std::unique_ptr<Type>(new Type());
  }
}

template<typename Type, typename Arg1>
void EnsureCreated(Type*& object, Arg1 arg1)
{
  if(!object)
  {
    object = new Type(arg1);
  }
}

template<typename Type, typename Arg1, typename Arg2>
void EnsureCreated(Type*& object, Arg1 arg1, Arg2 arg2)
{
  if(!object)
  {
    object = new Type(arg1, arg2);
  }
}

float GetDpi()
{
  static uint32_t horizontalDpi = 0u;
  static uint32_t verticalDpi   = 0u;

  // TODO : How can we know when fontClient DPI changed case?
  if(DALI_UNLIKELY(horizontalDpi == 0u))
  {
    Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
    fontClient.GetDpi(horizontalDpi, verticalDpi);
  }
  return static_cast<float>(horizontalDpi);
}

float ConvertPixelToPoint(float pixel)
{
  return pixel * 72.0f / GetDpi();
}

float ConvertPointToPixel(float point)
{
  // Pixel size = Point size * DPI / 72.f
  return point * GetDpi() / 72.0f;
}

void UpdateCursorPosition(Dali::Ui::Text::EventData* eventData)
{
  if(eventData && Dali::Ui::Text::EventData::IsEditingState(eventData->mState))
  {
    // Update the cursor position if it's in editing mode
    eventData->mDecoratorUpdated     = true;
    eventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font size is updated.
  }
}

} // namespace

namespace Dali::Ui::Text
{
void Controller::EnableTextInput(DecoratorPtr decorator, InputMethodContext& inputMethodContext)
{
  if(!decorator)
  {
    delete mImpl->mEventData;
    mImpl->mEventData = NULL;

    // Nothing else to do.
    return;
  }

  EnsureCreated(mImpl->mEventData, decorator, inputMethodContext);
}

void Controller::SetGlyphType(TextAbstraction::GlyphType glyphType)
{
  // Metrics for bitmap & vector based glyphs are different
  mImpl->mMetrics->SetGlyphType(glyphType);

  mImpl->ClearFontData();

  RequestRelayout();
}

bool Controller::HasAnchors() const
{
  return (mImpl->mModel->mLogicalModel->mAnchors.Count() && mImpl->IsShowingRealText());
}

void Controller::SetMarqueeEnabled(bool enable, bool requestRelayout, Text::MarqueeOrientation orientation)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetMarqueeEnabled[%s] SingleBox[%s]-> [%p]\n",
                (enable) ? "true" : "false",
                (mImpl->mLayoutEngine.GetLayout() == Layout::Engine::SINGLE_LINE_BOX) ? "true" : "false", this);
  mImpl->SetMarqueeEnabled(enable, requestRelayout, orientation);
}

void Controller::SetMarqueeMaxTextureExceeded(bool exceed)
{
  mImpl->mIsMarqueeMaxTextureExceeded = exceed;
}

bool Controller::IsMarqueeEnabled() const
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::IsMarqueeEnabled[%s]\n",
                mImpl->mIsMarqueeEnabled ? "true" : "false");
  return mImpl->mIsMarqueeEnabled;
}

CharacterDirection Controller::GetMarqueeTextDirection() const
{
  return mImpl->mIsTextDirectionRTL;
}

float Controller::GetMarqueeLineAlignment() const
{
  float offset = 0.f;
  if(mImpl->mModel->mVisualModel && (0u != mImpl->mModel->mVisualModel->mLines.Count()))
  {
    offset = (*mImpl->mModel->mVisualModel->mLines.Begin()).alignmentOffset;
  }
  return offset;
}

void Controller::SetHorizontalScrollEnabled(bool enable)
{
  if(mImpl->mEventData && mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetHorizontalScrollEnabled(enable);
  }
}

bool Controller::IsHorizontalScrollEnabled() const
{
  return mImpl->mEventData && mImpl->mEventData->mDecorator &&
         mImpl->mEventData->mDecorator->IsHorizontalScrollEnabled();
}

void Controller::SetVerticalScrollEnabled(bool enable)
{
  if(mImpl->mEventData && mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetVerticalScrollEnabled(enable);
  }
}

bool Controller::IsVerticalScrollEnabled() const
{
  return mImpl->mEventData && mImpl->mEventData->mDecorator && mImpl->mEventData->mDecorator->IsVerticalScrollEnabled();
}

void Controller::SetSmoothHandlePanEnabled(bool enable)
{
  if(mImpl->mEventData && mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetSmoothHandlePanEnabled(enable);
  }
}

bool Controller::IsSmoothHandlePanEnabled() const
{
  return mImpl->mEventData && mImpl->mEventData->mDecorator &&
         mImpl->mEventData->mDecorator->IsSmoothHandlePanEnabled();
}

void Controller::SetMaximumNumberOfCharacters(Length maxCharacters)
{
  mImpl->mMaximumNumberOfCharacters = maxCharacters;
}

int Controller::GetMaximumNumberOfCharacters()
{
  return mImpl->mMaximumNumberOfCharacters;
}

void Controller::SetEnableCursorBlink(bool enable)
{
  mImpl->SetEnableCursorBlink(enable);
}

bool Controller::GetEnableCursorBlink() const
{
  return mImpl->mEventData && mImpl->mEventData->mCursorBlinkEnabled;
}

void Controller::SetMultiLineEnabled(bool enable)
{
  mImpl->SetMultiLineEnabled(enable);
}

bool Controller::IsMultiLineEnabled() const
{
  return Layout::Engine::MULTI_LINE_BOX == mImpl->mLayoutEngine.GetLayout();
}

void Controller::SetHorizontalAlignment(Alignment alignment)
{
  mImpl->SetHorizontalAlignment(alignment);
}

Alignment Controller::GetHorizontalAlignment() const
{
  return mImpl->mModel->mHorizontalAlignment;
}

void Controller::SetVerticalAlignment(Alignment alignment)
{
  mImpl->SetVerticalAlignment(alignment);
}

Alignment Controller::GetVerticalAlignment() const
{
  return mImpl->mModel->mVerticalAlignment;
}

bool Controller::IsRemoveFrontInset() const
{
  return mImpl->mModel->mRemoveFrontInset;
}

void Controller::SetRemoveFrontInset(bool remove)
{
  mImpl->mModel->mRemoveFrontInset = remove;
}

bool Controller::IsRemoveBackInset() const
{
  return mImpl->mModel->mRemoveBackInset;
}

void Controller::SetRemoveBackInset(bool remove)
{
  mImpl->mModel->mRemoveBackInset = remove;
}

bool Controller::IsCursorInsetEnabled() const
{
  return mImpl->mIsCursorInsetEnabled;
}

void Controller::SetCursorInsetEnabled(bool enable)
{
  mImpl->mIsCursorInsetEnabled = enable;
}

bool Controller::IsTextCutout() const
{
  return mImpl->mTextCutout;
}

void Controller::SetTextCutout(bool cutout)
{
  if(cutout != mImpl->mTextCutout)
  {
    mImpl->mModel->mVisualModel->SetCutoutEnabled(cutout);
    mImpl->mTextCutout = cutout;
    RequestRelayout();
    RequestAsyncRender();
  }
}

void Controller::GetVariationsMap(Property::Map& map)
{
  map = mImpl->mModel->mLogicalModel->mVariationsMap;
}

void Controller::SetVariationsMap(const Property::Map& map)
{
  auto& variationsMap = mImpl->mModel->mLogicalModel->mVariationsMap;
  variationsMap.Clear();

  Property::Map::SizeType numberOfItems = map.Count();

  for(Property::Map::SizeType index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& keyvalue = map.GetKeyValue(index);

    if(keyvalue.first.type == Property::Key::STRING)
    {
      float value = 0.f;
      if(keyvalue.first.stringKey.Size() == 4 && keyvalue.second.Get(value)) // Variable tag must be 4-length string.
      {
        variationsMap[keyvalue.first.stringKey] = value;
      }
    }
  }

  mImpl->ClearFontData();
  RequestRelayout();
  RequestAsyncRender();
  InvalidateMeasure();
}

Dali::Vector<Text::FontVariation::Axis> Controller::GetVariations() const
{
  Dali::Vector<Text::FontVariation::Axis> axes;
  const Property::Map&                    map   = mImpl->mModel->mLogicalModel->mVariationsMap;
  const Property::Map::SizeType           count = map.Count();
  for(Property::Map::SizeType i = 0u; i < count; ++i)
  {
    const auto& keyValue = map.GetKeyValue(i);
    if(keyValue.first.type == Property::Key::STRING)
    {
      float value = 0.0f;
      if(keyValue.second.Get(value))
      {
        axes.PushBack(Text::FontVariation::Axis(keyValue.first.stringKey, value));
      }
    }
  }

  return axes;
}

void Controller::SetVariations(const Dali::Vector<Text::FontVariation::Axis>& axes)
{
  auto& variationsMap = mImpl->mModel->mLogicalModel->mVariationsMap;
  variationsMap.Clear();

  const std::size_t count = axes.Count();
  for(std::size_t i = 0u; i < count; i++)
  {
    const auto& tag   = axes[i].GetTag();
    const float value = axes[i].GetValue();
    if(tag.Size() == 4u && std::isfinite(value))
    {
      variationsMap[tag] = value;
    }
  }

  mImpl->ClearFontData();
  RequestRelayout();
  RequestAsyncRender();
  InvalidateMeasure();
}

void Controller::ClearVariationsMap()
{
  auto& variationsMap = mImpl->mModel->mLogicalModel->mVariationsMap;
  if(!variationsMap.Empty())
  {
    variationsMap.Clear();
    mImpl->ClearFontData();
    RequestRelayout();
    RequestAsyncRender();
    InvalidateMeasure();
  }
}

void Controller::ChangedLayoutDirection()
{
  mImpl->mIsLayoutDirectionChanged = true;
  mImpl->InvalidateLayoutDirectionData();
}

void Controller::InvalidateFontData()
{
  mImpl->InvalidateFontData();
}

void Controller::SetLayoutDirectionMode(LayoutDirectionMode type)
{
  mImpl->mModel->mLayoutDirectionMode = type;
  mImpl->InvalidateFontData();
}

LayoutDirectionMode Controller::GetLayoutDirectionMode() const
{
  return mImpl->mModel->mLayoutDirectionMode;
}

void Controller::SetLayoutDirection(Dali::LayoutDirection::Type layoutDirection)
{
  mImpl->mLayoutDirection = layoutDirection;
}

Dali::LayoutDirection::Type Controller::GetLayoutDirection(Dali::Actor& actor) const
{
  return mImpl->GetLayoutDirection(actor);
}

bool Controller::IsShowingPlaceholderText() const
{
  return mImpl->IsShowingPlaceholderText();
}

bool Controller::IsShowingRealText() const
{
  return mImpl->IsShowingRealText();
}

void Controller::SetAsyncRendering(bool asyncRendering)
{
  if(mImpl->mIsAsyncRendering != asyncRendering)
  {
    mImpl->mIsAsyncRendering = asyncRendering;
    RequestRelayout();
    RequestAsyncRender();
  }
}

bool Controller::IsAsyncRendering() const
{
  return mImpl->mIsAsyncRendering;
}

void Controller::SetLineWrapMode(LineWrapMode lineWrapMode)
{
  mImpl->SetLineWrapMode(lineWrapMode);
}

LineWrapMode Controller::GetLineWrapMode() const
{
  return mImpl->mModel->mLineWrapMode;
}

void Controller::SetTextElideEnabled(bool enabled)
{
  mImpl->mModel->mElideEnabled = enabled;
  mImpl->mModel->mVisualModel->SetTextElideEnabled(enabled);
}

bool Controller::IsTextElideEnabled() const
{
  return mImpl->mModel->mElideEnabled;
}

void Controller::SetTextFitEnabled(bool enabled)
{
  mImpl->mTextFitEnabled = enabled;
  mImpl->ClearFontData();
  RequestRelayout();
  RequestAsyncRender();
}

bool Controller::IsTextFitEnabled() const
{
  return mImpl->mTextFitEnabled;
}

void Controller::SetTextFitChanged(bool changed)
{
  mImpl->mTextFitChanged = changed;
}

bool Controller::IsTextFitChanged() const
{
  return mImpl->mTextFitChanged;
}

void Controller::SetCurrentLineSize(float lineSize)
{
  mImpl->mCurrentLineSize = lineSize;
}

float Controller::GetCurrentLineSize() const
{
  return mImpl->mCurrentLineSize;
}

void Controller::SetTextFitMinSize(float minSize, FontSizeType type)
{
  mImpl->mTextFitMinSize = (type == POINT_SIZE) ? minSize : ConvertPixelToPoint(minSize);
}

float Controller::GetTextFitMinSize(FontSizeType type) const
{
  return type == POINT_SIZE ? mImpl->mTextFitMinSize : ConvertPointToPixel(mImpl->mTextFitMinSize);
}

void Controller::SetTextFitMaxSize(float maxSize, FontSizeType type)
{
  mImpl->mTextFitMaxSize = (type == POINT_SIZE) ? maxSize : ConvertPixelToPoint(maxSize);
}

float Controller::GetTextFitMaxSize(FontSizeType type) const
{
  return type == POINT_SIZE ? mImpl->mTextFitMaxSize : ConvertPointToPixel(mImpl->mTextFitMaxSize);
}

void Controller::SetTextFitStepSize(float step, FontSizeType type)
{
  mImpl->mTextFitStepSize = (type == POINT_SIZE) ? step : ConvertPixelToPoint(step);
}

float Controller::GetTextFitStepSize(FontSizeType type) const
{
  return type == POINT_SIZE ? mImpl->mTextFitStepSize : ConvertPointToPixel(mImpl->mTextFitStepSize);
}

void Controller::SetTextFitContentSize(Vector2 size)
{
  mImpl->mTextFitContentSize = size;
}

Vector2 Controller::GetTextFitContentSize() const
{
  return mImpl->mTextFitContentSize;
}

float Controller::GetTextFitFontSize(FontSizeType type) const
{
  if(mImpl->mFontDefaults)
  {
    return type == POINT_SIZE ? mImpl->mFontDefaults->mFitPointSize : ConvertPointToPixel(mImpl->mFontDefaults->mFitPointSize);
  }
  return 0.0f;
}

void Controller::SetTextFitPointSize(float pointSize)
{
  EnsureCreated(mImpl->mFontDefaults);
  mImpl->mFontDefaults->mFitPointSize = pointSize;
  mImpl->mFontDefaults->sizeDefined   = true;
  mImpl->ClearFontData();
}

void Controller::SetTextFitCandidatesEnabled(bool enabled)
{
  mImpl->mTextFitCandidatesEnabled = enabled;
  mImpl->ClearFontData();
  RequestRelayout();
  RequestAsyncRender();
}

bool Controller::IsTextFitCandidatesEnabled() const
{
  return mImpl->mTextFitCandidatesEnabled;
}

const Text::Fit::Candidate* Controller::GetMaxFitCandidate() const
{
  const int index = mImpl->mMaxFitCandidateIndex;
  if(index < 0 || static_cast<uint32_t>(index) >= mImpl->mTextFitCandidates.Count())
  {
    return nullptr;
  }

  return &mImpl->mTextFitCandidates[static_cast<uint32_t>(index)];
}

void Controller::SetTextFitCandidates(const Dali::Vector<Text::Fit::Candidate>& candidates)
{
  mImpl->mTextFitCandidates    = candidates;
  mImpl->mMaxFitCandidateIndex = -1;

  const uint32_t count = static_cast<uint32_t>(mImpl->mTextFitCandidates.Count());
  if(count == 0u)
  {
    return;
  }

  uint32_t bestIndex = 0u;
  for(uint32_t i = 1u; i < count; ++i)
  {
    const Text::Fit::Candidate& best = mImpl->mTextFitCandidates[bestIndex];
    const Text::Fit::Candidate& cur  = mImpl->mTextFitCandidates[i];

    const float bestFontSize = best.GetFontSize();
    const float curFontSize  = cur.GetFontSize();

    if((curFontSize > bestFontSize) ||
       (Equals(curFontSize, bestFontSize, Math::MACHINE_EPSILON_1000) &&
        cur.GetLineHeight() > best.GetLineHeight()))
    {
      bestIndex = i;
    }
  }

  mImpl->mMaxFitCandidateIndex = static_cast<int>(bestIndex);
}

const Dali::Vector<Text::Fit::Candidate>& Controller::GetTextFitCandidates() const
{
  return mImpl->mTextFitCandidates;
}

void Controller::ClearTextFitCandidates()
{
  if(!mImpl->mTextFitCandidates.Empty())
  {
    mImpl->mTextFitCandidates.Clear();
  }
  mImpl->mMaxFitCandidateIndex = -1;
}

void Controller::SetPlaceholderTextElideEnabled(bool enabled)
{
  PlaceholderHandler::SetPlaceholderTextElideEnabled(*this, enabled);
}

bool Controller::IsPlaceholderTextElideEnabled() const
{
  return PlaceholderHandler::IsPlaceholderTextElideEnabled(*this);
}

void Controller::SetSelectionEnabled(bool enabled)
{
  mImpl->mEventData->mSelectionEnabled = enabled;
}

bool Controller::IsSelectionEnabled() const
{
  return mImpl->mEventData->mSelectionEnabled;
}

void Controller::SetShiftSelectionEnabled(bool enabled)
{
  mImpl->mEventData->mShiftSelectionFlag = enabled;
}

bool Controller::IsShiftSelectionEnabled() const
{
  return mImpl->mEventData->mShiftSelectionFlag;
}

void Controller::SetGrabHandleEnabled(bool enabled)
{
  mImpl->mEventData->mGrabHandleEnabled = enabled;
}

bool Controller::IsGrabHandleEnabled() const
{
  return mImpl->mEventData->mGrabHandleEnabled;
}

void Controller::SetGrabHandlePopupEnabled(bool enabled)
{
  mImpl->mEventData->mGrabHandlePopupEnabled = enabled;
}

bool Controller::IsGrabHandlePopupEnabled() const
{
  return mImpl->mEventData->mGrabHandlePopupEnabled;
}

void Controller::SetText(const std::string& text)
{
  TextUpdater::SetText(*this, text);
}

void Controller::SetStyledText(const StyledText& styledText)
{
  TextUpdater::SetStyledText(*this, styledText);
}

StyledText Controller::GetStyledText() const
{
  if(!mImpl->mEventData || !mImpl->mEventData->mEditableStyledText)
  {
    return {};
  }

  std::string text;
  mImpl->GetText(text);
  return mImpl->mEventData->mEditableStyledText->Build(text);
}

void Controller::GetText(std::string& text) const
{
  mImpl->GetText(text);
}

Length Controller::GetNumberOfCharacters() const
{
  return mImpl->GetNumberOfCharacters();
}

void Controller::SetPlaceholderText(PlaceholderType type, const std::string& text)
{
  PlaceholderHandler::SetPlaceholderText(*this, type, text);
}

void Controller::GetPlaceholderText(PlaceholderType type, std::string& text) const
{
  PlaceholderHandler::GetPlaceholderText(*this, type, text);
}

void Controller::SetShowPlaceholderOnFocus(bool enabled)
{
  PlaceholderHandler::SetShowPlaceholderOnFocus(*this, enabled);
}

bool Controller::IsPlaceholderShownOnFocus() const
{
  return PlaceholderHandler::IsPlaceholderShownOnFocus(*this);
}

void Controller::UpdateAfterFontChange(const std::string& newDefaultFont)
{
  mImpl->UpdateAfterFontChange(newDefaultFont);
}

void Controller::RetrieveSelection(std::string& selectedText) const
{
  mImpl->RetrieveSelection(selectedText, false);
}

void Controller::SetSelection(int start, int end)
{
  mImpl->SetSelection(start, end);
}

std::pair<int, int> Controller::GetSelectionIndexes() const
{
  return mImpl->GetSelectionIndexes();
}

void Controller::CopyStringToClipboard(const std::string& source)
{
  mImpl->CopyStringToClipboard(source);
}

void Controller::SendSelectionToClipboard(bool deleteAfterSending)
{
  mImpl->SendSelectionToClipboard(deleteAfterSending);
}

float Controller::GetDefaultFontLineHeight()
{
  return mImpl->GetDefaultFontLineHeight();
}

float Controller::GetDefaultLineBoxHeight()
{
  return mImpl->GetDefaultLineBoxHeight();
}

void Controller::SetDefaultFontFamily(const std::string& defaultFontFamily)
{
  EnsureCreated(mImpl->mFontDefaults);

  if(mImpl->mFontDefaults->GetFontDescription().family != defaultFontFamily)
  {
    mImpl->mFontDefaults->GetFontDescription().family = defaultFontFamily;
    DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetDefaultFontFamily %s\n", defaultFontFamily.c_str());
    mImpl->mFontDefaults->familyDefined = !defaultFontFamily.empty();

    // Update the cursor position if it's in editing mode
    UpdateCursorPosition(mImpl->mEventData);

    mImpl->ClearFontData();
    RequestRelayout();
    RequestAsyncRender();
    InvalidateMeasure();
  }
}

std::string Controller::GetDefaultFontFamily() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->GetFontDescription().family : EMPTY_STRING;
}

void Controller::SetPlaceholderFontFamily(const std::string& placeholderTextFontFamily)
{
  PlaceholderHandler::SetPlaceholderFontFamily(*this, placeholderTextFontFamily);
}

std::string Controller::GetPlaceholderFontFamily() const
{
  return PlaceholderHandler::GetPlaceholderFontFamily(*this);
}

void Controller::SetDefaultFontWeight(FontWeightType weight)
{
  EnsureCreated(mImpl->mFontDefaults);

  mImpl->mFontDefaults->GetFontDescription().weight = weight;
  mImpl->mFontDefaults->weightDefined               = true;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  mImpl->ClearFontData();
  RequestRelayout();
  RequestAsyncRender();
  InvalidateMeasure();
}

bool Controller::IsDefaultFontWeightDefined() const
{
  return mImpl->mFontDefaults && mImpl->mFontDefaults->weightDefined;
}

FontWeightType Controller::GetDefaultFontWeight() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->GetFontDescription().weight : TextAbstraction::FontWeight::NORMAL;
}

void Controller::SetPlaceholderTextFontWeight(FontWeightType weight)
{
  PlaceholderHandler::SetPlaceholderTextFontWeight(*this, weight);
}

bool Controller::IsPlaceholderTextFontWeightDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontWeightDefined(*this);
}

FontWeightType Controller::GetPlaceholderTextFontWeight() const
{
  return PlaceholderHandler::GetPlaceholderTextFontWeight(*this);
}

void Controller::SetDefaultFontWidth(FontWidthType width)
{
  EnsureCreated(mImpl->mFontDefaults);

  mImpl->mFontDefaults->GetFontDescription().width = width;
  mImpl->mFontDefaults->widthDefined               = true;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  mImpl->ClearFontData();
  RequestRelayout();
  RequestAsyncRender();
  InvalidateMeasure();
}

bool Controller::IsDefaultFontWidthDefined() const
{
  return mImpl->mFontDefaults && mImpl->mFontDefaults->widthDefined;
}

FontWidthType Controller::GetDefaultFontWidth() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->GetFontDescription().width : TextAbstraction::FontWidth::NORMAL;
}

void Controller::SetPlaceholderTextFontWidth(FontWidthType width)
{
  PlaceholderHandler::SetPlaceholderTextFontWidth(*this, width);
}

bool Controller::IsPlaceholderTextFontWidthDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontWidthDefined(*this);
}

FontWidthType Controller::GetPlaceholderTextFontWidth() const
{
  return PlaceholderHandler::GetPlaceholderTextFontWidth(*this);
}

void Controller::SetDefaultFontSlant(FontSlantType slant)
{
  EnsureCreated(mImpl->mFontDefaults);

  mImpl->mFontDefaults->GetFontDescription().slant = slant;
  mImpl->mFontDefaults->slantDefined               = true;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  mImpl->ClearFontData();
  RequestRelayout();
  RequestAsyncRender();
  InvalidateMeasure();
}

bool Controller::IsDefaultFontSlantDefined() const
{
  return mImpl->mFontDefaults && mImpl->mFontDefaults->slantDefined;
}

FontSlantType Controller::GetDefaultFontSlant() const
{
  return mImpl->mFontDefaults ? mImpl->mFontDefaults->GetFontDescription().slant : TextAbstraction::FontSlant::NORMAL;
}

void Controller::SetPlaceholderTextFontSlant(FontSlantType slant)
{
  PlaceholderHandler::SetPlaceholderTextFontSlant(*this, slant);
}

bool Controller::IsPlaceholderTextFontSlantDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontSlantDefined(*this);
}

FontSlantType Controller::GetPlaceholderTextFontSlant() const
{
  return PlaceholderHandler::GetPlaceholderTextFontSlant(*this);
}

float Controller::GetEffectiveTextScale() const
{
  return mImpl->GetEffectiveTextScale();
}

bool Controller::SetUiScale(float scale)
{
  return mImpl->SetUiScale(scale);
}

float Controller::GetUiScale() const
{
  return mImpl->GetUiScale();
}

void Controller::SetFontSizeScale(float scale)
{
  mImpl->SetFontSizeScale(scale);
}

float Controller::GetFontSizeScale() const
{
  return mImpl->GetFontSizeScale();
}

void Controller::SetMinimumFontSizeScale(float scale)
{
  mImpl->SetMinimumFontSizeScale(scale);
}

float Controller::GetMinimumFontSizeScale() const
{
  return mImpl->GetMinimumFontSizeScale();
}

void Controller::SetMaximumFontSizeScale(float scale)
{
  mImpl->SetMaximumFontSizeScale(scale);
}

float Controller::GetMaximumFontSizeScale() const
{
  return mImpl->GetMaximumFontSizeScale();
}

void Controller::SetSystemFontSizeScaleEnabled(bool enabled)
{
  mImpl->SetSystemFontSizeScaleEnabled(enabled);
}

bool Controller::IsSystemFontSizeScaleEnabled() const
{
  return mImpl->IsSystemFontSizeScaleEnabled();
}

void Controller::SetSystemFontSizeScale(float scale)
{
  mImpl->SetSystemFontSizeScale(scale);
}

float Controller::GetAdjustedFontSizeScale() const
{
  return mImpl->GetAdjustedFontSizeScale();
}

void Controller::SetDefaultFontSize(float fontSize, FontSizeType type)
{
  EnsureCreated(mImpl->mFontDefaults);

  mImpl->mFontDefaults->mDefaultPointSize = (type == POINT_SIZE) ? fontSize : ConvertPixelToPoint(fontSize);
  mImpl->mFontDefaults->sizeDefined       = true;

  // Update the cursor position if it's in editing mode
  UpdateCursorPosition(mImpl->mEventData);

  mImpl->ClearFontData();
  RequestRelayout();
  RequestAsyncRender();

  if(mImpl->mEventData && EventData::INACTIVE != mImpl->mEventData->mState)
  {
    SetInputFontSize(mImpl->mFontDefaults->mDefaultPointSize, POINT_SIZE, true);
  }
}

float Controller::GetDefaultFontSize(FontSizeType type) const
{
  if(mImpl->mFontDefaults)
  {
    return (type == POINT_SIZE) ? mImpl->mFontDefaults->mDefaultPointSize
                                : ConvertPointToPixel(mImpl->mFontDefaults->mDefaultPointSize);
  }
  return 0.0f;
}

void Controller::SetPlaceholderTextFontSize(float fontSize, FontSizeType type)
{
  PlaceholderHandler::SetPlaceholderTextFontSize(*this, fontSize, type);
}

float Controller::GetPlaceholderTextFontSize(FontSizeType type) const
{
  return PlaceholderHandler::GetPlaceholderTextFontSize(*this, type);
}

void Controller::SetDefaultColor(const Vector4& color)
{
  mImpl->SetDefaultColor(color);
}

const Vector4& Controller::GetDefaultColor() const
{
  return mImpl->mTextColor;
}

void Controller::SetAnchorColor(const Vector4& color)
{
  mImpl->SetAnchorColor(color);
}

const Vector4& Controller::GetAnchorColor() const
{
  return mImpl->GetAnchorColor();
}

void Controller::SetAnchorClickedColor(const Vector4& color)
{
  mImpl->SetAnchorClickedColor(color);
}

const Vector4& Controller::GetAnchorClickedColor() const
{
  return mImpl->GetAnchorClickedColor();
}

void Controller::SetDisabledColorOpacity(float opacity)
{
  mImpl->mDisabledColorOpacity = opacity;
}

float Controller::GetDisabledColorOpacity() const
{
  return mImpl->mDisabledColorOpacity;
}

void Controller::SetUserInteractionEnabled(bool enabled)
{
  mImpl->SetUserInteractionEnabled(enabled);
}

bool Controller::IsUserInteractionEnabled() const
{
  return mImpl->mIsUserInteractionEnabled;
}

void Controller::SetPlaceholderTextColor(const Vector4& textColor)
{
  PlaceholderHandler::SetPlaceholderTextColor(*this, textColor);
}

const Vector4& Controller::GetPlaceholderTextColor() const
{
  return PlaceholderHandler::GetPlaceholderTextColor(*this);
}

void Controller::SetShadowOffset(const Vector2& shadowOffset)
{
  mImpl->mModel->mVisualModel->SetShadowOffset(shadowOffset);
  RequestRelayout();
  RequestAsyncRender();
}

const Vector2& Controller::GetShadowOffset() const
{
  return mImpl->mModel->mVisualModel->GetShadowOffset();
}

void Controller::SetShadowEnabled(bool enabled)
{
  if(mImpl->mModel->mVisualModel->IsShadowEnabled() != enabled)
  {
    mImpl->mModel->mVisualModel->SetShadowEnabled(enabled);
    RequestRelayout();
    RequestAsyncRender();
  }
}

bool Controller::IsShadowEnabled() const
{
  return mImpl->mModel->mVisualModel->IsShadowEnabled();
}

void Controller::SetShadowColor(const Vector4& shadowColor)
{
  mImpl->mModel->mVisualModel->SetShadowColor(shadowColor);
  RequestRelayout();
  RequestAsyncRender();
}

const Vector4& Controller::GetShadowColor() const
{
  return mImpl->mModel->mVisualModel->GetShadowColor();
}

void Controller::SetShadowBlurRadius(const float& shadowBlurRadius)
{
  if(fabsf(GetShadowBlurRadius() - shadowBlurRadius) > Math::MACHINE_EPSILON_1)
  {
    mImpl->mModel->mVisualModel->SetShadowBlurRadius(shadowBlurRadius);
  }
}

bool Controller::IsEmbossEnabled() const
{
  return mImpl->mModel->mVisualModel->IsEmbossEnabled();
}

void Controller::SetEmbossEnabled(const bool enable)
{
  mImpl->mModel->mVisualModel->SetEmbossEnabled(enable);
  RequestRelayout();
  RequestAsyncRender();
}

const Vector2& Controller::GetEmbossDirection() const
{
  return mImpl->mModel->mVisualModel->GetEmbossDirection();
}

void Controller::SetEmbossDirection(const Vector2& direction)
{
  mImpl->mModel->mVisualModel->SetEmbossDirection(direction);
}

float Controller::GetEmbossStrength() const
{
  return mImpl->mModel->mVisualModel->GetEmbossStrength();
}

void Controller::SetEmbossStrength(const float strength)
{
  mImpl->mModel->mVisualModel->SetEmbossStrength(strength);
}

const Vector4& Controller::GetEmbossLightColor() const
{
  return mImpl->mModel->mVisualModel->GetEmbossLightColor();
}

void Controller::SetEmbossLightColor(const Vector4& lightColor)
{
  mImpl->mModel->mVisualModel->SetEmbossLightColor(lightColor);
  RequestRelayout();
  RequestAsyncRender();
}

const Vector4& Controller::GetEmbossShadowColor() const
{
  return mImpl->mModel->mVisualModel->GetEmbossShadowColor();
}

void Controller::SetEmbossShadowColor(const Vector4& shadowColor)
{
  mImpl->mModel->mVisualModel->SetEmbossShadowColor(shadowColor);
  RequestRelayout();
  RequestAsyncRender();
}

const float& Controller::GetShadowBlurRadius() const
{
  return mImpl->mModel->mVisualModel->GetShadowBlurRadius();
}

void Controller::SetUnderlineColor(const Vector4& color)
{
  mImpl->mModel->mVisualModel->SetUnderlineColor(color);
  RequestRelayout();
  RequestAsyncRender();
}

const Vector4& Controller::GetUnderlineColor() const
{
  return mImpl->mModel->mVisualModel->GetUnderlineColor();
}

void Controller::SetUnderlineEnabled(bool enabled)
{
  mImpl->mModel->mVisualModel->SetUnderlineEnabled(enabled);
  RequestRelayout();
  RequestAsyncRender();
}

bool Controller::IsUnderlineEnabled() const
{
  return mImpl->mModel->mVisualModel->IsUnderlineEnabled();
}

void Controller::SetUnderlineHeight(float height)
{
  mImpl->mModel->mVisualModel->SetUnderlineHeight(height);
}

float Controller::GetUnderlineHeight() const
{
  return mImpl->mModel->mVisualModel->GetUnderlineHeight();
}

void Controller::SetUnderlineType(Text::Underline::Type type)
{
  mImpl->mModel->mVisualModel->SetUnderlineType(type);
}

Text::Underline::Type Controller::GetUnderlineType() const
{
  return mImpl->mModel->mVisualModel->GetUnderlineType();
}

void Controller::SetDashedUnderlineWidth(float width)
{
  mImpl->mModel->mVisualModel->SetDashedUnderlineWidth(width);
}

float Controller::GetDashedUnderlineWidth() const
{
  return mImpl->mModel->mVisualModel->GetDashedUnderlineWidth();
}

void Controller::SetDashedUnderlineGap(float gap)
{
  mImpl->mModel->mVisualModel->SetDashedUnderlineGap(gap);
}

float Controller::GetDashedUnderlineGap() const
{
  return mImpl->mModel->mVisualModel->GetDashedUnderlineGap();
}

void Controller::SetOutlineOffset(const Vector2& outlineOffset)
{
  mImpl->mModel->mVisualModel->SetOutlineOffset(outlineOffset);
}

const Vector2& Controller::GetOutlineOffset() const
{
  return mImpl->mModel->mVisualModel->GetOutlineOffset();
}

void Controller::SetOutlineColor(const Vector4& color)
{
  mImpl->mModel->mVisualModel->SetOutlineColor(color);
  RequestRelayout();
  RequestAsyncRender();
}

const Vector4& Controller::GetOutlineColor() const
{
  return mImpl->mModel->mVisualModel->GetOutlineColor();
}

void Controller::SetOutlineWidth(uint16_t width)
{
  mImpl->mModel->mVisualModel->SetOutlineWidth(width);
  RequestRelayout();
  RequestAsyncRender();
}

uint16_t Controller::GetOutlineWidth() const
{
  return mImpl->mModel->mVisualModel->GetOutlineWidth();
}

void Controller::SetOutlineEnabled(bool enabled)
{
  if(mImpl->mModel->mVisualModel->IsOutlineEnabled() != enabled)
  {
    mImpl->mModel->mVisualModel->SetOutlineEnabled(enabled);
    RequestRelayout();
    RequestAsyncRender();
  }
}

bool Controller::IsOutlineEnabled() const
{
  return mImpl->mModel->mVisualModel->IsOutlineEnabled();
}

void Controller::SetOutlineBlurRadius(const float& outlineBlurRadius)
{
  if(fabsf(GetOutlineBlurRadius() - outlineBlurRadius) > Math::MACHINE_EPSILON_1)
  {
    mImpl->mModel->mVisualModel->SetOutlineBlurRadius(outlineBlurRadius);
  }
}

const float& Controller::GetOutlineBlurRadius() const
{
  return mImpl->mModel->mVisualModel->GetOutlineBlurRadius();
}

void Controller::SetBackgroundColor(const Vector4& color)
{
  mImpl->mModel->mVisualModel->SetBackgroundColor(color);
  RequestRelayout();
  RequestAsyncRender();
}

const Vector4& Controller::GetBackgroundColor() const
{
  return mImpl->mModel->mVisualModel->GetBackgroundColor();
}

void Controller::SetBackgroundEnabled(bool enabled)
{
  mImpl->mModel->mVisualModel->SetBackgroundEnabled(enabled);
  RequestRelayout();
  RequestAsyncRender();
}

bool Controller::IsBackgroundEnabled() const
{
  return mImpl->mModel->mVisualModel->IsBackgroundEnabled();
}

void Controller::SetDefaultEmbossProperties(const std::string& embossProperties)
{
  EnsureCreated(mImpl->mEmbossDefaults);
  mImpl->mEmbossDefaults->properties = embossProperties;
}

std::string Controller::GetDefaultEmbossProperties() const
{
  return mImpl->mEmbossDefaults ? mImpl->mEmbossDefaults->properties : EMPTY_STRING;
}

void Controller::SetDefaultOutlineProperties(const std::string& outlineProperties)
{
  EnsureCreated(mImpl->mOutlineDefaults);
  mImpl->mOutlineDefaults->properties = outlineProperties;
}

std::string Controller::GetDefaultOutlineProperties() const
{
  return mImpl->mOutlineDefaults ? mImpl->mOutlineDefaults->properties : EMPTY_STRING;
}

bool Controller::SetDefaultLineSpacing(float lineSpacing)
{
  return mImpl->SetDefaultLineSpacing(lineSpacing);
}

float Controller::GetDefaultLineSpacing() const
{
  return mImpl->mLayoutEngine.GetDefaultLineSpacing();
}

bool Controller::SetDefaultLineSize(float lineSize)
{
  return mImpl->SetDefaultLineSize(lineSize);
}

float Controller::GetDefaultLineSize() const
{
  return mImpl->mLayoutEngine.GetDefaultLineSize();
}

bool Controller::SetRelativeLineSize(float relativeLineSize)
{
  return mImpl->SetRelativeLineSize(relativeLineSize);
}

float Controller::GetRelativeLineSize() const
{
  return mImpl->GetRelativeLineSize();
}

void Controller::SetInputColor(const Vector4& color)
{
  InputProperties::SetInputColor(*this, color);
}

const Vector4& Controller::GetInputColor() const
{
  return InputProperties::GetInputColor(*this);
}

void Controller::SetInputFontFamily(const std::string& fontFamily)
{
  InputFontHandler::SetInputFontFamily(*this, fontFamily);
}

std::string Controller::GetInputFontFamily() const
{
  return InputFontHandler::GetInputFontFamily(*this);
}

void Controller::SetInputFontWeight(FontWeightType weight)
{
  InputFontHandler::SetInputFontWeight(*this, weight);
}

bool Controller::IsInputFontWeightDefined() const
{
  return InputFontHandler::IsInputFontWeightDefined(*this);
}

FontWeightType Controller::GetInputFontWeight() const
{
  return InputFontHandler::GetInputFontWeight(*this);
}

void Controller::SetInputFontWidth(FontWidthType width)
{
  InputFontHandler::SetInputFontWidth(*this, width);
}

bool Controller::IsInputFontWidthDefined() const
{
  return InputFontHandler::IsInputFontWidthDefined(*this);
}

FontWidthType Controller::GetInputFontWidth() const
{
  return InputFontHandler::GetInputFontWidth(*this);
}

void Controller::SetInputFontSlant(FontSlantType slant)
{
  InputFontHandler::SetInputFontSlant(*this, slant);
}

bool Controller::IsInputFontSlantDefined() const
{
  return InputFontHandler::IsInputFontSlantDefined(*this);
}

FontSlantType Controller::GetInputFontSlant() const
{
  return InputFontHandler::GetInputFontSlant(*this);
}

void Controller::SetInputFontSize(float fontSize, FontSizeType type, bool defaultFontSizeUpdated)
{
  const float pointSize = (type == POINT_SIZE) ? fontSize : ConvertPixelToPoint(fontSize);
  InputFontHandler::SetInputFontPointSize(*this, pointSize, defaultFontSizeUpdated);
}

float Controller::GetInputFontSize(FontSizeType type) const
{
  const float pointSize = InputFontHandler::GetInputFontPointSize(*this);
  return (type == POINT_SIZE) ? pointSize : ConvertPointToPixel(pointSize);
}

void Controller::SetInputLineSpacing(float lineSpacing)
{
  InputProperties::SetInputLineSpacing(*this, lineSpacing);
}

float Controller::GetInputLineSpacing() const
{
  return InputProperties::GetInputLineSpacing(*this);
}

void Controller::SetInputShadowProperties(const std::string& shadowProperties)
{
  InputProperties::SetInputShadowProperties(*this, shadowProperties);
}

std::string Controller::GetInputShadowProperties() const
{
  return InputProperties::GetInputShadowProperties(*this);
}

void Controller::SetInputUnderlineProperties(const std::string& underlineProperties)
{
  InputProperties::SetInputUnderlineProperties(*this, underlineProperties);
}

std::string Controller::GetInputUnderlineProperties() const
{
  return InputProperties::GetInputUnderlineProperties(*this);
}

void Controller::SetInputEmbossProperties(const std::string& embossProperties)
{
  InputProperties::SetInputEmbossProperties(*this, embossProperties);
}

std::string Controller::GetInputEmbossProperties() const
{
  return InputProperties::GetInputEmbossProperties(*this);
}

void Controller::SetInputOutlineProperties(const std::string& outlineProperties)
{
  InputProperties::SetInputOutlineProperties(*this, outlineProperties);
}

std::string Controller::GetInputOutlineProperties() const
{
  return InputProperties::GetInputOutlineProperties(*this);
}

void Controller::SetInputModePassword(bool passwordInput)
{
  InputProperties::SetInputModePassword(*this, passwordInput);
}

bool Controller::IsInputModePassword()
{
  return InputProperties::IsInputModePassword(*this);
}

void Controller::SetNoTextDoubleTapAction(NoTextTap::Action action)
{
  if(mImpl->mEventData)
  {
    mImpl->mEventData->mDoubleTapAction = action;
  }
}

Controller::NoTextTap::Action Controller::GetNoTextDoubleTapAction() const
{
  return mImpl->mEventData ? mImpl->mEventData->mDoubleTapAction : NoTextTap::NO_ACTION;
}

void Controller::SetNoTextLongPressAction(NoTextTap::Action action)
{
  if(mImpl->mEventData)
  {
    mImpl->mEventData->mLongPressAction = action;
  }
}

Controller::NoTextTap::Action Controller::GetNoTextLongPressAction() const
{
  return mImpl->mEventData ? mImpl->mEventData->mLongPressAction : NoTextTap::NO_ACTION;
}

bool Controller::IsUnderlineSetByString()
{
  return mImpl->mUnderlineSetByString;
}

void Controller::UnderlineSetByString(bool setByString)
{
  mImpl->mUnderlineSetByString = setByString;
}

bool Controller::IsShadowSetByString()
{
  return mImpl->mShadowSetByString;
}

void Controller::ShadowSetByString(bool setByString)
{
  mImpl->mShadowSetByString = setByString;
}

bool Controller::IsOutlineSetByString()
{
  return mImpl->mOutlineSetByString;
}

void Controller::OutlineSetByString(bool setByString)
{
  mImpl->mOutlineSetByString = setByString;
}

bool Controller::IsFontStyleSetByString()
{
  return mImpl->mFontStyleSetByString;
}

void Controller::FontStyleSetByString(bool setByString)
{
  mImpl->mFontStyleSetByString = setByString;
}

void Controller::SetStrikethroughHeight(float height)
{
  mImpl->mModel->mVisualModel->SetStrikethroughHeight(height);
}

float Controller::GetStrikethroughHeight() const
{
  return mImpl->mModel->mVisualModel->GetStrikethroughHeight();
}

void Controller::SetStrikethroughColor(const Vector4& color)
{
  mImpl->mModel->mVisualModel->SetStrikethroughColor(color);
  RequestRelayout();
  RequestAsyncRender();
}

const Vector4& Controller::GetStrikethroughColor() const
{
  return mImpl->mModel->mVisualModel->GetStrikethroughColor();
}

void Controller::SetStrikethroughEnabled(bool enabled)
{
  mImpl->mModel->mVisualModel->SetStrikethroughEnabled(enabled);
  RequestRelayout();
  RequestAsyncRender();
}

bool Controller::IsStrikethroughEnabled() const
{
  return mImpl->mModel->mVisualModel->IsStrikethroughEnabled();
}

void Controller::SetInputStrikethroughProperties(const std::string& strikethroughProperties)
{
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mInputStyle.strikethroughProperties = strikethroughProperties;
  }
}

std::string Controller::GetInputStrikethroughProperties() const
{
  return (NULL != mImpl->mEventData) ? mImpl->mEventData->mInputStyle.strikethroughProperties : EMPTY_STRING;
}

bool Controller::IsStrikethroughSetByString()
{
  return mImpl->mStrikethroughSetByString;
}

void Controller::StrikethroughSetByString(bool setByString)
{
  mImpl->mStrikethroughSetByString = setByString;
}

Layout::Engine& Controller::GetLayoutEngine()
{
  return mImpl->mLayoutEngine;
}

View& Controller::GetView()
{
  return mImpl->mView;
}

Vector3 Controller::GetNaturalSize(bool convertToEven)
{
  return Relayouter::GetNaturalSize(*this, convertToEven);
}

bool Controller::CheckForTextFit(float pointSize, Size& layoutSize)
{
  return Relayouter::CheckForTextFit(*this, pointSize, layoutSize);
}

void Controller::FitPointSizeforLayout(Size layoutSize)
{
  Relayouter::FitPointSizeforLayout(*this, layoutSize);
}

void Controller::FitCandidatesPointSizeForLayout(Size layoutSize)
{
  Relayouter::FitCandidatesPointSizeForLayout(*this, layoutSize);
}

float Controller::GetHeightForWidth(float width)
{
  return Relayouter::GetHeightForWidth(*this, width);
}

Vector2 Controller::CalculateLayoutSize(float width, float height, bool forceUpdate)
{
  return Relayouter::CalculateLayoutSize(*this, width, height, forceUpdate);
}

int Controller::GetLineCount(float width)
{
  GetHeightForWidth(width);
  return GetRenderTextModel()->GetNumberOfLines();
}

void Controller::SetTextChangedSignalEmission(bool emitting)
{
  mImpl->mIsEmittingTextChangedSignal = emitting;
}

bool Controller::IsTextChangedSignalEmission() const
{
  return mImpl->mIsEmittingTextChangedSignal;
}

const ModelInterface* Controller::GetLogicalTextModel() const
{
  return mImpl->mModel.Get();
}

const ModelInterface* Controller::GetRenderTextModel() const
{
  const ReplacementRenderState* replacement = mImpl->GetReplacementRenderStatePtr();
  if(replacement && replacement->processingModel && replacement->projection.HasReplacements())
  {
    return replacement->processingModel.Get();
  }
  return mImpl->mModel.Get();
}

bool Controller::HasValidReplacementSource() const
{
  return mImpl->HasValidReplacementSource();
}

const FinalElisionResult* Controller::GetFinalElisionResult() const
{
  const ReplacementRenderState* replacement = mImpl->GetReplacementRenderStatePtr();
  if(replacement && replacement->processingModel && replacement->projection.HasReplacements() &&
     replacement->finalElision.resolved)
  {
    return &replacement->finalElision;
  }
  return nullptr;
}

const ReplacementSourceSnapshot& Controller::GetReplacementSourceSnapshot() const
{
  return mImpl->GetReplacementSourceSnapshot();
}

const ReplacementRenderState& Controller::GetReplacementRenderState() const
{
  return mImpl->GetReplacementRenderState();
}

float Controller::GetScrollAmountByUserInput()
{
  float scrollAmount = 0.0f;

  if(NULL != mImpl->mEventData && mImpl->mEventData->mCheckScrollAmount)
  {
    scrollAmount                          = mImpl->mModel->mScrollPosition.y - mImpl->mModel->mScrollPositionLast.y;
    mImpl->mEventData->mCheckScrollAmount = false;
  }
  return scrollAmount;
}

bool Controller::GetTextScrollInfo(float& scrollPosition, float& controlHeight, float& layoutHeight)
{
  const Vector2& layout = mImpl->mModel->mVisualModel->GetLayoutSize();
  bool           isScrolled;

  controlHeight  = mImpl->mModel->mVisualModel->mControlSize.height;
  layoutHeight   = layout.height;
  scrollPosition = mImpl->mModel->mScrollPosition.y;
  isScrolled     = !Equals(mImpl->mModel->mScrollPosition.y, mImpl->mModel->mScrollPositionLast.y, Math::MACHINE_EPSILON_1);
  return isScrolled;
}

void Controller::SetPasswordMode(PasswordMode mode)
{
  EnsureCreated<HiddenText, Controller*>(mImpl->mHiddenInput, this);
  if(mImpl->mHiddenInput->GetPasswordMode() != mode)
  {
    mImpl->mHiddenInput->SetPasswordMode(mode);
    if(mImpl->mEventData)
    {
      mImpl->mEventData->mDecoratorUpdated     = true;
      mImpl->mEventData->mUpdateCursorPosition = true;
    }
    InvalidateFontData();

    // TODO: Update the accessibility role when password mode changes.
  }
}

PasswordMode Controller::GetPasswordMode() const
{
  return mImpl->mHiddenInput ? mImpl->mHiddenInput->GetPasswordMode() : PasswordMode::NONE;
}

void Controller::SetPasswordMaskCharacter(uint32_t character)
{
  EnsureCreated<HiddenText, Controller*>(mImpl->mHiddenInput, this);

  if(mImpl->mHiddenInput->GetPasswordMaskCharacter() != character)
  {
    mImpl->mHiddenInput->SetPasswordMaskCharacter(character);
    if(mImpl->mEventData)
    {
      mImpl->mEventData->mDecoratorUpdated     = true;
      mImpl->mEventData->mUpdateCursorPosition = true;
    }
    if(mImpl->mHiddenInput->GetMode() != HiddenText::Mode::NONE)
    {
      InvalidateFontData();
    }
  }
}

uint32_t Controller::GetPasswordMaskCharacter() const
{
  return mImpl->mHiddenInput ? mImpl->mHiddenInput->GetPasswordMaskCharacter() : DEFAULT_PASSWORD_MASK_CHARACTER;
}

void Controller::SetPasswordRevealDuration(int duration)
{
  EnsureCreated<HiddenText, Controller*>(mImpl->mHiddenInput, this);
  if(mImpl->mHiddenInput->GetPasswordRevealDuration() != duration)
  {
    mImpl->mHiddenInput->SetPasswordRevealDuration(duration);
  }
}

int Controller::GetPasswordRevealDuration() const
{
  return mImpl->mHiddenInput ? mImpl->mHiddenInput->GetPasswordRevealDuration() : DEFAULT_PASSWORD_REVEAL_DURATION;
}

void Controller::ClearHiddenText()
{
  EnsureCreated<HiddenText, Controller*>(mImpl->mHiddenInput, this);
  mImpl->mHiddenInput->ClearHiddenText();
}

void Controller::HideAllText()
{
  EnsureCreated<HiddenText, Controller*>(mImpl->mHiddenInput, this);
  mImpl->mHiddenInput->HideAll();
}

void Controller::HideFirstCharacters(uint32_t count)
{
  EnsureCreated<HiddenText, Controller*>(mImpl->mHiddenInput, this);
  mImpl->mHiddenInput->HideFirstCharacters(count);
}

void Controller::ShowFirstCharacters(uint32_t count)
{
  EnsureCreated<HiddenText, Controller*>(mImpl->mHiddenInput, this);
  mImpl->mHiddenInput->ShowFirstCharacters(count);
}

HiddenText::Mode Controller::GetHiddenTextMode() const
{
  return mImpl->mHiddenInput ? mImpl->mHiddenInput->GetMode() : HiddenText::Mode::NONE;
}

uint32_t Controller::GetHiddenTextSubstituteCount() const
{
  return mImpl->mHiddenInput ? mImpl->mHiddenInput->GetSubstituteCount() : 0u;
}

void Controller::SetInputFilter(const InputFilter& inputFilter)
{
  std::string allowPattern;
  std::string denyPattern;

  Dali::Integration::GetStdString(inputFilter.GetAllowPattern(), allowPattern);
  Dali::Integration::GetStdString(inputFilter.GetDenyPattern(), denyPattern);

  if(allowPattern.empty() && denyPattern.empty())
  {
    mImpl->mInputFilterProcessor.reset();
    return;
  }

  EnsureCreated(mImpl->mInputFilterProcessor);

  mImpl->mInputFilterProcessor->SetAllowPattern(allowPattern);
  mImpl->mInputFilterProcessor->SetDenyPattern(denyPattern);
}

InputFilter Controller::GetInputFilter() const
{
  if(!mImpl->mInputFilterProcessor)
  {
    return InputFilter::None();
  }

  InputFilter inputFilter;
  inputFilter.SetAllowPattern(Dali::Integration::ToDaliString(mImpl->mInputFilterProcessor->GetAllowPattern()));
  inputFilter.SetDenyPattern(Dali::Integration::ToDaliString(mImpl->mInputFilterProcessor->GetDenyPattern()));
  return inputFilter;
}

void Controller::SetPlaceholderProperty(const Property::Map& map)
{
  PlaceholderHandler::SetPlaceholderProperty(*this, map);
}

void Controller::GetPlaceholderProperty(Property::Map& map)
{
  PlaceholderHandler::GetPlaceholderProperty(*this, map);
}

Direction Controller::GetTextDirection()
{
  // Make sure the model is up-to-date before layouting
  EventHandler::ProcessModifyEvents(*this);

  return mImpl->GetTextDirection();
}

Alignment Controller::GetVerticalLineAlignment() const
{
  return mImpl->mModel->GetVerticalLineAlignment();
}

void Controller::SetVerticalLineAlignment(Alignment alignment)
{
  mImpl->mModel->mVerticalLineAlignment = alignment;
  if(mImpl->mModel->mVisualModel)
  {
    mImpl->mModel->mVisualModel->SetVerticalLineAlignment(alignment);
  }
}

Text::EllipsisPosition::Type Controller::GetEllipsisPosition() const
{
  return mImpl->mModel->GetEllipsisPosition();
}

void Controller::SetEllipsisPosition(Text::EllipsisPosition::Type ellipsisPosition)
{
  mImpl->mModel->mEllipsisPosition = ellipsisPosition;
  mImpl->mModel->mVisualModel->SetEllipsisPosition(ellipsisPosition);
}

void Controller::SetRenderScale(const float renderScale)
{
  float scale = renderScale;
  if(scale < 1.0f)
  {
    DALI_LOG_DEBUG_INFO("RenderScale must be greater than or equal to 1.0f. It will change as follows:%f -> 1.0\n", scale);
    scale = 1.0f;
  }

  if(!Equals(scale, mImpl->mRenderScale, Math::MACHINE_EPSILON_1000))
  {
    mImpl->mRenderScale = scale;
    RequestRelayout();
    RequestAsyncRender();
  }
}

float Controller::GetRenderScale() const
{
  return mImpl->mRenderScale;
}

void Controller::SetCharacterSpacing(float characterSpacing)
{
  mImpl->mModel->mVisualModel->SetCharacterSpacing(characterSpacing);
  mImpl->RelayoutAllCharacters();
  RequestRelayout();
  RequestAsyncRender();
}

const float Controller::GetCharacterSpacing() const
{
  return mImpl->mModel->mVisualModel->GetCharacterSpacing();
}

void Controller::SetLayoutAlignmentOffset(Vector2 offset)
{
  mImpl->mModel->mLayoutAlignmentOffset = offset;
}

void Controller::SetLayoutOffsetWithPadding(Vector2 offset)
{
  mImpl->mModel->mLayoutOffsetWithPadding = offset;
}

Vector2 Controller::GetLayoutOffsetWithPadding() const
{
  return mImpl->mModel->mLayoutOffsetWithPadding;
}

void Controller::SetBackgroundWithCutoutEnabled(bool cutout)
{
  mImpl->mModel->mVisualModel->SetBackgroundWithCutoutEnabled(cutout);
  RequestRelayout();
  RequestAsyncRender();
}

bool Controller::IsBackgroundWithCutoutEnabled() const
{
  return mImpl->mModel->mVisualModel->IsBackgroundWithCutoutEnabled();
}

void Controller::SetBackgroundColorWithCutout(const Vector4& color)
{
  mImpl->mModel->mVisualModel->SetBackgroundColorWithCutout(color);
  RequestRelayout();
}

const Vector4 Controller::GetBackgroundColorWithCutout() const
{
  return mImpl->mModel->mVisualModel->GetBackgroundColorWithCutout();
}

void Controller::SetOffsetWithCutout(const Vector2& offset)
{
  mImpl->mModel->mVisualModel->SetOffsetWithCutout(offset);
}

Controller::UpdateTextType Controller::Relayout(const Size& size, Dali::LayoutDirection::Type layoutDirection)
{
  return Relayouter::Relayout(*this, size, layoutDirection);
}

void Controller::RequestRelayout()
{
  mImpl->RequestRelayout();
}

void Controller::InvalidateMeasure()
{
  mImpl->InvalidateMeasure();
}

void Controller::RequestAsyncRender()
{
  mImpl->RequestAsyncRender();
}

namespace
{
struct InclusiveCharacterRange
{
  CharacterIndex start{0u};
  CharacterIndex end{0u};
  CharacterIndex endExclusive{0u};
};

bool NormalizeInclusiveCharacterRange(CharacterIndex           start,
                                      CharacterIndex           end,
                                      Length                   logicalCount,
                                      InclusiveCharacterRange& normalized)
{
  if(logicalCount == 0u || (start >= logicalCount && end >= logicalCount))
  {
    return false;
  }

  start = std::min(start, static_cast<CharacterIndex>(logicalCount - 1u));
  end   = std::min(end, static_cast<CharacterIndex>(logicalCount - 1u));
  if(start > end)
  {
    std::swap(start, end);
  }

  normalized.start        = start;
  normalized.end          = end;
  normalized.endExclusive = end + 1u; // Safe after clamping to a valid character index.
  return true;
}

bool LogicalRangeIsWithinReplacement(const ProjectedReplacementRun& replacement,
                                     CharacterIndex                 start,
                                     CharacterIndex                 end)
{
  const CharacterIndex replacementStart = replacement.logicalCharacterRange.characterIndex;
  const Length         replacementCount = replacement.logicalCharacterRange.numberOfCharacters;
  return replacementCount > 0u && start >= replacementStart &&
         start - replacementStart < replacementCount && end >= replacementStart &&
         end - replacementStart < replacementCount;
}

bool PlacementMatchesReplacement(const ReplacementPlacement&    placement,
                                 const ProjectedReplacementRun& replacement)
{
  return placement.logicalCharacterRange.characterIndex == replacement.logicalCharacterRange.characterIndex &&
         placement.logicalCharacterRange.numberOfCharacters == replacement.logicalCharacterRange.numberOfCharacters;
}

bool PlacementIntersectsRange(const ReplacementPlacement& placement, CharacterIndex start, CharacterIndex endExclusive)
{
  const CharacterIndex placementStart = placement.logicalCharacterRange.characterIndex;
  const Length         placementCount = placement.logicalCharacterRange.numberOfCharacters;
  const uint64_t       placementEnd   = static_cast<uint64_t>(placementStart) + placementCount;
  return placement.visible && !placement.elided && placementCount > 0u &&
         placementStart < endExclusive && static_cast<uint64_t>(start) < placementEnd;
}
} // unnamed namespace

Vector<Vector2> Controller::GetTextSize(CharacterIndex startIndex, CharacterIndex endIndex)
{
  Vector<Vector2> sizesList;
  Vector<Vector2> positionsList;

  InclusiveCharacterRange logicalRange;
  if(!NormalizeInclusiveCharacterRange(startIndex, endIndex, mImpl->mModel->GetNumberOfCharacters(), logicalRange))
  {
    return sizesList;
  }
  startIndex = logicalRange.start;
  endIndex   = logicalRange.end;

  const ReplacementRenderState& replacementState = mImpl->GetReplacementRenderState();
  ModelPtr                      geometryModel    = mImpl->mModel;
  if(replacementState.processingModel && replacementState.projection.HasReplacements())
  {
    const ProjectedReplacementRun* replacement =
      replacementState.projection.FindByLogicalCharacter(logicalRange.start);
    if(replacement && LogicalRangeIsWithinReplacement(*replacement, logicalRange.start, logicalRange.end))
    {
      for(const ReplacementPlacement& placement : replacementState.placements)
      {
        if(PlacementMatchesReplacement(placement, *replacement) && placement.visible && !placement.elided)
        {
          sizesList.PushBack(placement.size);
          return sizesList;
        }
      }
      return sizesList;
    }

    geometryModel                       = replacementState.processingModel;
    const CharacterIndex projectedStart = replacementState.projection.LogicalBoundaryToProjected(
      logicalRange.start,
      ReplacementProjection::BoundaryAffinity::LEADING);
    const CharacterIndex projectedEnd = replacementState.projection.LogicalBoundaryToProjected(
      logicalRange.endExclusive,
      ReplacementProjection::BoundaryAffinity::TRAILING);
    startIndex = projectedStart;
    endIndex   = projectedEnd > projectedStart ? projectedEnd - 1u : projectedStart;
  }

  GetTextGeometry(geometryModel, startIndex, endIndex, sizesList, positionsList,
                  replacementState.processingModel ? &replacementState.finalElision : nullptr);
  return sizesList;
}

Vector<Vector2> Controller::GetTextPosition(CharacterIndex startIndex, CharacterIndex endIndex)
{
  Vector<Vector2> sizesList;
  Vector<Vector2> positionsList;

  InclusiveCharacterRange logicalRange;
  if(!NormalizeInclusiveCharacterRange(startIndex, endIndex, mImpl->mModel->GetNumberOfCharacters(), logicalRange))
  {
    return positionsList;
  }
  startIndex = logicalRange.start;
  endIndex   = logicalRange.end;

  const ReplacementRenderState& replacementState = mImpl->GetReplacementRenderState();
  ModelPtr                      geometryModel    = mImpl->mModel;
  if(replacementState.processingModel && replacementState.projection.HasReplacements())
  {
    const ProjectedReplacementRun* replacement =
      replacementState.projection.FindByLogicalCharacter(logicalRange.start);
    if(replacement && LogicalRangeIsWithinReplacement(*replacement, logicalRange.start, logicalRange.end))
    {
      for(const ReplacementPlacement& placement : replacementState.placements)
      {
        if(PlacementMatchesReplacement(placement, *replacement) && placement.visible && !placement.elided)
        {
          positionsList.PushBack(placement.position);
          return positionsList;
        }
      }
      return positionsList;
    }

    geometryModel                       = replacementState.processingModel;
    const CharacterIndex projectedStart = replacementState.projection.LogicalBoundaryToProjected(
      logicalRange.start,
      ReplacementProjection::BoundaryAffinity::LEADING);
    const CharacterIndex projectedEnd = replacementState.projection.LogicalBoundaryToProjected(
      logicalRange.endExclusive,
      ReplacementProjection::BoundaryAffinity::TRAILING);
    startIndex = projectedStart;
    endIndex   = projectedEnd > projectedStart ? projectedEnd - 1u : projectedStart;
  }

  GetTextGeometry(geometryModel, startIndex, endIndex, sizesList, positionsList,
                  replacementState.processingModel ? &replacementState.finalElision : nullptr);
  return positionsList;
}

Bounds Controller::GetLineBoundingRectangle(const uint32_t lineIndex)
{
  const ReplacementRenderState& replacement   = mImpl->GetReplacementRenderState();
  ModelPtr                      geometryModel = (replacement.processingModel && replacement.projection.HasReplacements())
                                                  ? replacement.processingModel
                                                  : mImpl->mModel;
  return GetLineBoundingRect(geometryModel, lineIndex);
}

Bounds Controller::GetCharacterBoundingRectangle(const uint32_t charIndex)
{
  if(charIndex >= mImpl->mModel->GetNumberOfCharacters())
  {
    return {};
  }

  const ReplacementRenderState& replacementState = mImpl->GetReplacementRenderState();
  ModelPtr                      geometryModel    = mImpl->mModel;
  CharacterIndex                geometryIndex    = charIndex;
  if(replacementState.processingModel && replacementState.projection.HasReplacements())
  {
    const ProjectedReplacementRun* replacement =
      replacementState.projection.FindByLogicalCharacter(charIndex);
    if(replacement)
    {
      for(const ReplacementPlacement& placement : replacementState.placements)
      {
        if(PlacementMatchesReplacement(placement, *replacement) && placement.visible && !placement.elided)
        {
          return {placement.position.x, placement.position.y, placement.size.x, placement.size.y};
        }
      }
      return {};
    }

    geometryModel = replacementState.processingModel;
    geometryIndex = replacementState.projection.LogicalCharacterToProjected(charIndex);
  }
  return GetCharacterBoundingRect(geometryModel, geometryIndex,
                                  replacementState.processingModel ? &replacementState.finalElision : nullptr);
}

int Controller::GetCharacterIndexAtPosition(float visualX, float visualY)
{
  const ReplacementRenderState& replacementState = mImpl->GetReplacementRenderState();
  ModelPtr                      geometryModel    = mImpl->mModel;
  const bool                    hasReplacementProjection =
    replacementState.processingModel && replacementState.projection.HasReplacements();
  if(hasReplacementProjection)
  {
    geometryModel                           = replacementState.processingModel;
    const ReplacementProjection& projection = replacementState.projection;
    for(const ReplacementPlacement& placement : replacementState.placements)
    {
      if(!placement.visible || placement.elided)
      {
        continue;
      }
      const float placementX = placement.position.x + geometryModel->mScrollPosition.x;
      const float placementY = placement.position.y + geometryModel->mScrollPosition.y;
      if(visualX >= placementX && visualX < placementX + placement.size.x &&
         visualY >= placementY && visualY < placementY + placement.size.y)
      {
        const ProjectedReplacementRun* run = projection.FindByLogicalCharacter(
          placement.logicalCharacterRange.characterIndex);
        if(run)
        {
          const CharacterIndex logicalBoundary = projection.HitTestLogicalBoundary(
            run->projectedCharacterIndex,
            visualX - placementX,
            placement.size.x,
            placement.lineDirection);
          return logicalBoundary <= static_cast<CharacterIndex>(std::numeric_limits<int>::max())
                   ? static_cast<int>(logicalBoundary)
                   : -1;
        }
      }
    }
  }

  const int projectedIndex = GetCharIndexAtPosition(geometryModel,
                                                    visualX - geometryModel->mScrollPosition.x,
                                                    visualY - geometryModel->mScrollPosition.y);
  if(!hasReplacementProjection || projectedIndex < 0)
  {
    return projectedIndex;
  }

  const ReplacementProjection& projection = replacementState.projection;
  if(const ProjectedReplacementRun* run = projection.FindByProjectedCharacter(projectedIndex))
  {
    for(const ReplacementPlacement& placement : replacementState.placements)
    {
      if(placement.visible && !placement.elided &&
         placement.logicalCharacterRange.characterIndex == run->logicalCharacterRange.characterIndex &&
         placement.logicalCharacterRange.numberOfCharacters == run->logicalCharacterRange.numberOfCharacters)
      {
        const float          placementX      = placement.position.x + geometryModel->mScrollPosition.x;
        const CharacterIndex logicalBoundary = projection.HitTestLogicalBoundary(
          projectedIndex,
          visualX - placementX,
          placement.size.x,
          placement.lineDirection);
        return logicalBoundary <= static_cast<CharacterIndex>(std::numeric_limits<int>::max())
                 ? static_cast<int>(logicalBoundary)
                 : -1;
      }
    }
  }

  const CharacterIndex logicalIndex = projection.ProjectedCharacterToLogical(projectedIndex);
  return logicalIndex <= static_cast<CharacterIndex>(std::numeric_limits<int>::max())
           ? static_cast<int>(logicalIndex)
           : -1;
}

Bounds Controller::GetTextBoundingRectangle(CharacterIndex startIndex, CharacterIndex endIndex)
{
  InclusiveCharacterRange logicalRange;
  if(!NormalizeInclusiveCharacterRange(startIndex, endIndex, mImpl->mModel->GetNumberOfCharacters(), logicalRange))
  {
    return {};
  }
  startIndex = logicalRange.start;
  endIndex   = logicalRange.end;

  const ReplacementRenderState& replacementState         = mImpl->GetReplacementRenderState();
  ModelPtr                      geometryModel            = mImpl->mModel;
  const bool                    hasReplacementProjection = replacementState.processingModel &&
                                        replacementState.projection.HasReplacements();
  if(hasReplacementProjection)
  {
    const ProjectedReplacementRun* replacement =
      replacementState.projection.FindByLogicalCharacter(logicalRange.start);
    if(replacement && LogicalRangeIsWithinReplacement(*replacement, logicalRange.start, logicalRange.end))
    {
      const auto placement = std::find_if(replacementState.placements.Begin(),
                                          replacementState.placements.End(),
                                          [replacement](const ReplacementPlacement& candidate)
      {
        return PlacementMatchesReplacement(candidate, *replacement) && candidate.visible && !candidate.elided;
      });
      if(placement == replacementState.placements.End())
      {
        return {};
      }
    }

    geometryModel                       = replacementState.processingModel;
    const CharacterIndex projectedStart = replacementState.projection.LogicalBoundaryToProjected(
      logicalRange.start,
      ReplacementProjection::BoundaryAffinity::LEADING);
    const CharacterIndex projectedEnd = replacementState.projection.LogicalBoundaryToProjected(
      logicalRange.endExclusive,
      ReplacementProjection::BoundaryAffinity::TRAILING);
    if(projectedEnd <= projectedStart)
    {
      return {};
    }
    startIndex = projectedStart;
    endIndex   = projectedEnd - 1u;
  }

  Bounds bounds = Ui::Internal::CommonTextUtils::GetTextBoundingRectangle(geometryModel, startIndex, endIndex);
  if(!hasReplacementProjection)
  {
    return bounds;
  }

  bool hasBounds = bounds.width > 0.0f || bounds.height > 0.0f;
  for(const ReplacementPlacement& placement : replacementState.placements)
  {
    if(!PlacementIntersectsRange(placement, logicalRange.start, logicalRange.endExclusive))
    {
      continue;
    }

    if(!hasBounds)
    {
      bounds    = Bounds(placement.position.x, placement.position.y, placement.size.x, placement.size.y);
      hasBounds = true;
      continue;
    }

    const float left   = std::min(bounds.x, placement.position.x);
    const float top    = std::min(bounds.y, placement.position.y);
    const float right  = std::max(bounds.x + bounds.width, placement.position.x + placement.size.x);
    const float bottom = std::max(bounds.y + bounds.height, placement.position.y + placement.size.y);
    bounds             = Bounds(left, top, right - left, bottom - top);
  }

  if(hasBounds)
  {
    const float controlWidth = geometryModel->mVisualModel->mControlSize.width;
    const float left         = std::max(0.0f, bounds.x);
    const float right        = std::min(controlWidth, bounds.x + bounds.width);
    bounds.x                 = left;
    bounds.width             = std::max(0.0f, right - left);
  }
  return bounds;
}

bool Controller::IsInputStyleChangedSignalsQueueEmpty()
{
  return mImpl->IsInputStyleChangedSignalsQueueEmpty();
}

void Controller::RequestProcessInputStyleChangedSignals()
{
  if(Dali::Adaptor::IsAvailable() && !mImpl->mProcessorRegistered)
  {
    mImpl->mProcessorRegistered = true;
    Dali::Adaptor::Get().RegisterProcessorOnce(*this, true);
  }
}

void Controller::OnIdleSignal()
{
  if(mImpl->mIdleCallback)
  {
    mImpl->mIdleCallback = NULL;

    mImpl->ProcessInputStyleChangedSignals();
  }
}

void Controller::KeyboardFocusGainEvent(bool scrollToCursor)
{
  EventHandler::KeyboardFocusGainEvent(*this, scrollToCursor);
}

void Controller::KeyboardFocusLostEvent()
{
  EventHandler::KeyboardFocusLostEvent(*this);
}

bool Controller::KeyEvent(const Dali::KeyEvent& keyEvent)
{
  return EventHandler::KeyEvent(*this, keyEvent);
}

bool Controller::AnchorClickEvent(uint32_t cursorPosition, std::string& href)
{
  return EventHandler::AnchorClickEvent(*this, cursorPosition, href);
}

void Controller::AnchorEvent(float x, float y)
{
  EventHandler::AnchorEvent(*this, x, y);
}

void Controller::TapEvent(unsigned int tapCount, float x, float y)
{
  EventHandler::TapEvent(*this, tapCount, x, y);
}

void Controller::PanEvent(GestureState state, const Vector2& displacement)
{
  EventHandler::PanEvent(*this, state, displacement);
}

void Controller::LongPressEvent(GestureState state, float x, float y)
{
  EventHandler::LongPressEvent(*this, state, x, y);
}

void Controller::SelectEvent(float x, float y, SelectionType selectType)
{
  EventHandler::SelectEvent(*this, x, y, selectType);
}

void Controller::SetTextSelectionRange(const uint32_t* start, const uint32_t* end)
{
  if(mImpl->mEventData)
  {
    mImpl->mEventData->mCheckScrollAmount     = true;
    mImpl->mEventData->mIsLeftHandleSelected  = true;
    mImpl->mEventData->mIsRightHandleSelected = true;
    mImpl->SetTextSelectionRange(start, end);
    RequestRelayout();
    EventHandler::KeyboardFocusGainEvent(*this, true);
  }
}

Ui::Integration::Text::Uint32Pair Controller::GetTextSelectionRange() const
{
  return mImpl->GetTextSelectionRange();
}

CharacterIndex Controller::GetPrimaryCursorPosition() const
{
  return mImpl->GetPrimaryCursorPosition();
}

bool Controller::SetPrimaryCursorPosition(CharacterIndex index, bool focused)
{
  if(mImpl->mEventData)
  {
    mImpl->mEventData->mCheckScrollAmount     = true;
    mImpl->mEventData->mIsLeftHandleSelected  = true;
    mImpl->mEventData->mIsRightHandleSelected = true;
    mImpl->mEventData->mCheckScrollAmount     = true;
    if(mImpl->SetPrimaryCursorPosition(index, focused) && focused)
    {
      EventHandler::KeyboardFocusGainEvent(*this, true);
      return true;
    }
  }
  return false;
}

void Controller::SelectWholeText()
{
  EventHandler::SelectEvent(*this, 0.f, 0.f, SelectionType::ALL);
}

void Controller::SelectNone()
{
  EventHandler::SelectEvent(*this, 0.f, 0.f, SelectionType::NONE);
}

void Controller::SelectText(const uint32_t start, const uint32_t end)
{
  EventHandler::SelectEvent(*this, start, end, SelectionType::RANGE);
}

std::string Controller::GetSelectedText() const
{
  return mImpl->GetSelectedText();
}

std::string Controller::CopyText()
{
  return mImpl->CopyText();
}

std::string Controller::CutText()
{
  return mImpl->CutText();
}

void Controller::PasteClipboardItemEvent(uint32_t id, const char* mimeType, const char* data)
{
  // Upon receiving the data, it is important to disconnect the signal
  // to avoid potential unintended pasting caused by subsequent requests.
  Dali::Integration::Clipboard::DataReceivedSignal(mImpl->mClipboard).Disconnect(this, &Controller::PasteClipboardItemEvent);

  // If the id is 0u, it is an invalid response.
  if(id == 0u)
  {
    return;
  }

  // text-controller allows only plain text type.
  if(!strncmp(mimeType, MIME_TYPE_TEXT_PLAIN,
              strlen(MIME_TYPE_TEXT_PLAIN) + 1 /* Compare include null-terminated char */))
  {
    EventHandler::PasteClipboardItemEvent(*this, data);
  }
  else if(!strncmp(mimeType, MIME_TYPE_HTML, strlen(MIME_TYPE_HTML) + 1 /* Compare include null-terminated char */))
  {
    // This does not mean that text controls can parse html.
    // This is temporary code, as text controls do not support html type data.
    // Simply remove the tags inside the angle brackets.
    // Once multiple types and data can be stored in the clipboard, this code should be removed.
    std::regex  reg("<[^>]*>");
    std::string result = regex_replace(data, reg, "");

    EventHandler::PasteClipboardItemEvent(*this, result.c_str());
  }
}

void Controller::PasteText()
{
  if(mImpl->EnsureClipboardCreated())
  {
    // Connect the signal before calling GetData() of the clipboard.
    Dali::Integration::Clipboard::DataReceivedSignal(mImpl->mClipboard).Connect(this, &Controller::PasteClipboardItemEvent);

    // If there is no plain text type data on the clipboard, request html type data.
    Dali::String mimeType = mImpl->mClipboard.HasType(MIME_TYPE_TEXT_PLAIN) ? MIME_TYPE_TEXT_PLAIN : MIME_TYPE_HTML;

    // Request clipboard service to retrieve an item.
    uint32_t id = Dali::Integration::Clipboard::GetData(mImpl->mClipboard, mimeType);
    if(id == 0u)
    {
      // If the return id is 0u, the signal is not emitted, we must disconnect signal here.
      Dali::Integration::Clipboard::DataReceivedSignal(mImpl->mClipboard).Disconnect(this, &Controller::PasteClipboardItemEvent);
    }
  }
}

Dali::Integration::InputMethodContext::CallbackData Controller::OnInputMethodContextEvent(
  InputMethodContext&                                     inputMethodContext,
  const Dali::Integration::InputMethodContext::EventData& inputMethodContextEvent)
{
  return EventHandler::OnInputMethodContextEvent(*this, inputMethodContext, inputMethodContextEvent);
}

void Controller::GetTargetSize(Vector2& targetSize)
{
  targetSize = mImpl->mModel->mVisualModel->mControlSize;
}

void Controller::AddDecoration(Actor& actor, Ui::Integration::Text::DecorationType type, bool needsClipping)
{
  if(mImpl->mEditableControlInterface)
  {
    mImpl->mEditableControlInterface->AddDecoration(actor, type, needsClipping);
  }
}

bool Controller::IsEditable() const
{
  return mImpl->IsEditable();
}

void Controller::SetEditable(bool editable)
{
  mImpl->SetEditable(editable);
}

void Controller::ScrollBy(Vector2 scroll)
{
  mImpl->ScrollBy(scroll);
}

bool Controller::IsScrollable(const Vector2& displacement)
{
  return mImpl->IsScrollable(displacement);
}

float Controller::GetHorizontalScrollPosition()
{
  return mImpl->GetHorizontalScrollPosition();
}

float Controller::GetVerticalScrollPosition()
{
  return mImpl->GetVerticalScrollPosition();
}

void Controller::DecorationEvent(HandleType handleType, HandleState state, float x, float y)
{
  EventHandler::DecorationEvent(*this, handleType, state, x, y);
}

void Controller::TextPopupButtonTouched(Dali::Ui::Text::InputCommandType button)
{
  EventHandler::TextPopupButtonTouched(*this, button);
}

void Controller::DisplayTimeExpired()
{
  mImpl->mEventData->mUpdateCursorPosition = true;
  // Apply modifications to the model
  mImpl->mOperationsPending = ALL_OPERATIONS;

  RequestRelayout();
}

void Controller::ResetCursorPosition(CharacterIndex cursorIndex)
{
  // Reset the cursor position
  if(NULL != mImpl->mEventData)
  {
    mImpl->mEventData->mPrimaryCursorPosition = mImpl->NormalizeReplacementBoundary(
      cursorIndex,
      ReplacementEditNormalizer::BoundaryAffinity::TRAILING);

    // Update the cursor if it's in editing mode.
    if(EventData::IsEditingState(mImpl->mEventData->mState))
    {
      mImpl->mEventData->mUpdateCursorPosition = true;
    }
  }
}

CharacterIndex Controller::GetCursorPosition()
{
  return mImpl->mEventData ? mImpl->mEventData->mPrimaryCursorPosition : 0;
}

void Controller::SetControlInterface(Ui::Integration::Text::ControlInterface* controlInterface)
{
  mImpl->mControlInterface = controlInterface;
}

void Controller::SetAnchorControlInterface(Ui::Integration::Text::AnchorControlInterface* anchorControlInterface)
{
  mImpl->mAnchorControlInterface = anchorControlInterface;
}

bool Controller::ShouldClearFocusOnEscape() const
{
  return mImpl->ShouldClearFocusOnEscape();
}

Actor Controller::CreateBackgroundActor()
{
  ModelPtr geometryModel = mImpl->GetEditableGeometryModel();
  return CreateControllerBackgroundActor(mImpl->mView,
                                         geometryModel->mVisualModel,
                                         geometryModel->mLogicalModel,
                                         mImpl->mShaderBackground);
}

void Controller::GetAnchorActors(std::vector<Ui::TextAnchor>& anchorActors)
{
  mImpl->GetAnchorActors(anchorActors);
}

int Controller::GetAnchorIndex(size_t characterOffset)
{
  return mImpl->GetAnchorIndex(characterOffset);
}

void Controller::Process(bool postProcess)
{
  if(Dali::Adaptor::IsAvailable() && mImpl->mProcessorRegistered)
  {
    Dali::Adaptor& adaptor = Dali::Adaptor::Get();

    mImpl->mProcessorRegistered = false;

    if(NULL == mImpl->mIdleCallback)
    {
      // @note: The callback manager takes the ownership of the callback object.
      mImpl->mIdleCallback = MakeCallback(this, &Controller::OnIdleSignal);
      if(DALI_UNLIKELY(!adaptor.AddIdle(mImpl->mIdleCallback, false)))
      {
        DALI_LOG_ERROR(
          "Fail to add idle callback for text controller style changed signals queue. Skip these callbacks\n");

        // Clear queue forcely.
        if(mImpl->mEventData)
        {
          mImpl->mEventData->mInputStyleChangedQueue.Clear();
        }

        // Set the pointer to null as the callback manager deletes the callback even AddIdle failed.
        mImpl->mIdleCallback = NULL;
      }
    }
  }
}

Controller::Controller(Ui::Integration::Text::ControlInterface* controlInterface, Ui::Integration::Text::EditableControlInterface* editableControlInterface,
                       Ui::Integration::Text::SelectableControlInterface* selectableControlInterface,
                       Ui::Integration::Text::AnchorControlInterface*     anchorControlInterface)
: mImpl(new Controller::Impl(controlInterface, editableControlInterface, selectableControlInterface,
                             anchorControlInterface))
{
}

Controller::~Controller()
{
  if(Dali::Adaptor::IsAvailable())
  {
    if(mImpl->mProcessorRegistered)
    {
      Dali::Adaptor::Get().UnregisterProcessorOnce(*this, true);
    }
    if(mImpl->mIdleCallback)
    {
      Dali::Adaptor::Get().RemoveIdle(mImpl->mIdleCallback);
    }
  }
}

} // namespace Dali::Ui::Text
