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

// CLASS HEADER
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/integration-api/trace.h>
#include <string.h>

// INTERNAL HEADER
#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/text/color-glyph-helper.h>
#include <dali-ui-foundation/internal/text/script-run.h>
#include <dali-ui-foundation/internal/text/text-effects-style.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>
#include <dali-ui-foundation/internal/text/text-enumerations.h>
#include <dali-ui-foundation/internal/text/text-font-style.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/ui-constraint-tag-ranges.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);
DALI_INIT_TRACE_FILTER(gTraceFilter2, DALI_TRACE_TEXT_ASYNC, false);

const int CUSTOM_PROPERTY_COUNT(3); // uTextColorAnimatable, uHasMultipleTextColors, requireRender

static constexpr uint32_t TEXT_VISUAL_COLOR_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 21);
static constexpr uint32_t TEXT_VISUAL_OPACITY_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START +
                                                             22);

const float VERTICAL_ALIGNMENT_TABLE[static_cast<int>(Text::Alignment::END) + 1] = {
  0.0f, // Text::Alignment::START
  0.5f, // Text::Alignment::CENTER
  1.0f  // Text::Alignment::END
};

#ifdef TRACE_ENABLED
const char* GetRequestTypeName(Text::Async::RequestType type)
{
  if(type < Text::Async::RENDER_FIXED_SIZE || type > Text::Async::COMPUTE_HEIGHT_FOR_WIDTH)
  {
    return "INVALID_REQUEST_TYPE";
  }
  return Text::Async::RequestTypeName[type];
}
#endif

struct NameIndexMatch
{
  const char* const name;
  Property::Index   index;
};

const NameIndexMatch NAME_INDEX_MATCH_TABLE[] = {
  {TEXT_PROPERTY, Ui::TextVisualPropertyIndex::TEXT},
  {FONT_FAMILY_PROPERTY, Ui::TextVisualPropertyIndex::FONT_FAMILY},
  {FONT_SIZE_PROPERTY, Ui::TextVisualPropertyIndex::FONT_SIZE},
  {FONT_WEIGHT_PROPERTY, Ui::TextVisualPropertyIndex::FONT_WEIGHT},
  {FONT_WIDTH_PROPERTY, Ui::TextVisualPropertyIndex::FONT_WIDTH},
  {FONT_SLANT_PROPERTY, Ui::TextVisualPropertyIndex::FONT_SLANT},
  {MULTI_LINE_PROPERTY, Ui::TextVisualPropertyIndex::MULTI_LINE},
  {LINE_WRAP_MODE_PROPERTY, Ui::TextVisualPropertyIndex::LINE_WRAP_MODE},
  {HORIZONTAL_ALIGNMENT_PROPERTY, Ui::TextVisualPropertyIndex::HORIZONTAL_ALIGNMENT},
  {VERTICAL_ALIGNMENT_PROPERTY, Ui::TextVisualPropertyIndex::VERTICAL_ALIGNMENT},
  {OVERFLOW_MODE_PROPERTY, Ui::TextVisualPropertyIndex::OVERFLOW_MODE},
  {LINE_HEIGHT_PROPERTY, Ui::TextVisualPropertyIndex::LINE_HEIGHT},
  {LINE_HEIGHT_MODE_PROPERTY, Ui::TextVisualPropertyIndex::LINE_HEIGHT_MODE},
  {TEXT_COLOR_PROPERTY, Ui::TextVisualPropertyIndex::TEXT_COLOR},
  {MARKUP_ENABLED_PROPERTY, Ui::TextVisualPropertyIndex::MARKUP_ENABLED},
};
const int NAME_INDEX_MATCH_TABLE_SIZE = sizeof(NAME_INDEX_MATCH_TABLE) / sizeof(NAME_INDEX_MATCH_TABLE[0]);

void TextColorConstraint(Vector4& current, const PropertyInputContainer& inputs)
{
  Vector4 color = inputs[0]->GetVector4();
  current.r     = color.r * color.a;
  current.g     = color.g * color.a;
  current.b     = color.b * color.a;
  current.a     = color.a;
}

void OpacityConstraint(float& current, const PropertyInputContainer& inputs)
{
  // Make zero if the alpha value of text color is zero to skip rendering text
  if(EqualsZero(inputs[0]->GetVector4().a) && !inputs[1]->GetBoolean())
  {
    current = 0.0f;
  }
  else
  {
    current = 1.0f;
  }
}

} // unnamed namespace

TextVisualPtr TextVisual::New(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory,
                              const Property::Map& properties)
{
  TextVisualPtr textVisualPtr(new TextVisual(factoryCache, shaderFactory));
  textVisualPtr->SetProperties(properties);
  textVisualPtr->Initialize();
  return textVisualPtr;
}

float TextVisual::GetHeightForWidth(float width)
{
  return mController->GetHeightForWidth(width);
}

void TextVisual::GetNaturalSize(Vector2& naturalSize)
{
  naturalSize = mController->GetNaturalSize().GetVectorXY();
}

void TextVisual::DoCreatePropertyMap(Property::Map& map) const
{
  Property::Value value;

  map.Clear();
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::InternalVisualType::TEXT);

  std::string text;
  mController->GetText(text);
  map.Insert(Ui::TextVisualPropertyIndex::TEXT, ToPropertyValue(text));

  map.Insert(Ui::TextVisualPropertyIndex::FONT_FAMILY, ToPropertyValue(mController->GetDefaultFontFamily()));
  map.Insert(Ui::TextVisualPropertyIndex::FONT_SIZE, mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE));
  map.Insert(Ui::TextVisualPropertyIndex::FONT_WEIGHT, Text::ToFontWeight(mController->GetDefaultFontWeight()));
  map.Insert(Ui::TextVisualPropertyIndex::FONT_WIDTH, Text::ToFontWidth(mController->GetDefaultFontWidth()));
  map.Insert(Ui::TextVisualPropertyIndex::FONT_SLANT, Text::ToFontSlant(mController->GetDefaultFontSlant()));

  map.Insert(Ui::TextVisualPropertyIndex::MULTI_LINE, mController->IsMultiLineEnabled());
  map.Insert(Ui::TextVisualPropertyIndex::LINE_WRAP_MODE, mController->GetLineWrapMode());

  map.Insert(Ui::TextVisualPropertyIndex::HORIZONTAL_ALIGNMENT, mController->GetHorizontalAlignment());
  map.Insert(Ui::TextVisualPropertyIndex::VERTICAL_ALIGNMENT, mController->GetVerticalAlignment());

  map.Insert(Ui::TextVisualPropertyIndex::OVERFLOW_MODE, mOverflowMode);
  map.Insert(Ui::TextVisualPropertyIndex::LINE_HEIGHT, mLineHeight);
  map.Insert(Ui::TextVisualPropertyIndex::LINE_HEIGHT_MODE, mLineHeightMode);

  map.Insert(Ui::TextVisualPropertyIndex::TEXT_COLOR, mController->GetDefaultColor());

  map.Insert(Ui::TextVisualPropertyIndex::MARKUP_ENABLED, mController->IsMarkupProcessorEnabled());
}

void TextVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::InternalVisualType::TEXT);
  std::string text;
  mController->GetText(text);
  map.Insert(Ui::TextVisualPropertyIndex::TEXT, ToPropertyValue(text));
}

TextVisual::TextVisual(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory)
: Visual::Base(factoryCache, Ui::InternalVisualType::TEXT),
  mController(Text::Controller::New()),
  mTypesetter(Text::Typesetter::New(mController->GetTextModel())),
  mAsyncTextInterface(nullptr),
  mTextVisualShaderFactory(shaderFactory),
  mTextShaderFeatureCache(),
  mHasMultipleTextColorsIndex(Property::INVALID_INDEX),
  mAnimatableTextColorPropertyIndex(Property::INVALID_INDEX),
  mTextColorAnimatableIndex(Property::INVALID_INDEX),
  mTextRequireRenderPropertyIndex(Property::INVALID_INDEX),
  mLineHeight(Text::LINE_HEIGHT_AUTO),
  mLineHeightMode(Text::LineHeightMode::RELATIVE),
  mOverflowMode(Text::OverflowMode::ELLIPSIS),
  mTextLoadingTaskId(0u),
  mNaturalSizeTaskId(0u),
  mHeightForWidthTaskId(0u),
  mRendererUpdateNeeded(false),
  mTextRequireRender(false),
  mIsConstraintAppliedAlways(false),
  mIsTextLoadingTaskRunning(false),
  mIsNaturalSizeTaskRunning(false),
  mIsHeightForWidthTaskRunning(false)
{
  // Enable the pre-multiplied alpha to improve the text quality
  mImpl->mFlags |= Impl::IS_PRE_MULTIPLIED_ALPHA;

  // Enable fitting mode, to acquire effectiveScale value.
  mImpl->mFittingModeRequired = true;
}

TextVisual::~TextVisual()
{
}

void TextVisual::OnInitialize()
{
  Geometry geometry       = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  auto     featureBuilder = TextVisualShaderFeature::FeatureBuilder();
  Shader   shader         = GetTextShader(mFactoryCache, featureBuilder);

  mImpl->mRenderer = VisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);
  mTextRequireRenderPropertyIndex = mImpl->mRenderer.RegisterUniqueProperty("requireRender", mTextRequireRender);
  mHasMultipleTextColorsIndex =
    mImpl->mRenderer.RegisterUniqueProperty("uHasMultipleTextColors", static_cast<float>(false));

  // Apply overflow mode now. (Because contorller's text elide disabled as default)
  switch(mOverflowMode)
  {
    case Text::OverflowMode::CLIP:
    {
      mController->SetTextElideEnabled(false);
      break;
    }
    case Text::OverflowMode::ELLIPSIS:
    {
      mController->SetTextElideEnabled(true);
      break;
    }
  }

  // Retrieve the layout engine to set the cursor's width.
  Text::Layout::Engine& engine = mController->GetLayoutEngine();

  // Sets 0 as cursor's width.
  engine.SetCursorWidth(0u); // Do not layout space for the cursor.

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, static_cast<Ui::Direction::Type>(Text::Direction::LEFT_TO_RIGHT));
}

void TextVisual::DoSetProperties(const Property::Map& propertyMap)
{
  for(Property::Map::SizeType index = 0u, count = propertyMap.Count(); index < count; ++index)
  {
    const KeyValuePair& keyValue = propertyMap.GetKeyValue(index);
    if(keyValue.first.type == Property::Key::INDEX)
    {
      DoSetProperty(keyValue.first.indexKey, keyValue.second);
    }
    else
    {
      for(int i = 0; i < NAME_INDEX_MATCH_TABLE_SIZE; ++i)
      {
        if(keyValue.first == NAME_INDEX_MATCH_TABLE[i].name)
        {
          DoSetProperty(NAME_INDEX_MATCH_TABLE[i].index, keyValue.second);
          break;
        }
      }
    }
  }

  if(IsOnScene())
  {
    mRendererUpdateNeeded = true;

    // TODO : Need to trigger the owner to call OnSetTransform()
    // TODO : RelayoutRequest might not be works for dali-ui.
    // if(mImpl->mEventObserver)
    // {
    //   mImpl->mEventObserver->RelayoutRequest(*this);
    // }
    UpdateRenderer();
  }
}

void TextVisual::DoSetOnScene(Actor& actor)
{
  mControl = actor;

  const Vector4& defaultColor = mController->GetTextModel()->GetDefaultColor();
  if(mTextColorAnimatableIndex == Property::INVALID_INDEX)
  {
    mTextColorAnimatableIndex = mImpl->mRenderer.RegisterUniqueProperty("uTextColorAnimatable", defaultColor);
  }
  else
  {
    mImpl->mRenderer.SetProperty(mTextColorAnimatableIndex, defaultColor);
  }

  if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX)
  {
    // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
    if(mTextColorAnimatableIndex != Property::INVALID_INDEX)
    {
      if(!mColorConstraint)
      {
        mColorConstraint = Constraint::New<Vector4>(mImpl->mRenderer, mTextColorAnimatableIndex, TextColorConstraint);
        mColorConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
        Dali::Integration::ConstraintSetInternalTag(mColorConstraint, TEXT_VISUAL_COLOR_CONSTRAINT_TAG);
        mColorConstraint.Apply();
      }
      mColorConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                               : Dali::Constraint::APPLY_ONCE);

      mColorConstraintList.push_back(mColorConstraint);
    }

    // Make zero if the alpha value of text color is zero to skip rendering text
    if(!mOpacityConstraint)
    {
      // VisualRenderer::Property::OPACITY uses same animatable property internally.
      mOpacityConstraint =
        Constraint::New<float>(mImpl->mRenderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint);
      mOpacityConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
      mOpacityConstraint.AddSource(Source(mImpl->mRenderer, mTextRequireRenderPropertyIndex));
      Dali::Integration::ConstraintSetInternalTag(mOpacityConstraint, TEXT_VISUAL_OPACITY_CONSTRAINT_TAG);
      mOpacityConstraint.Apply();
    }
    mOpacityConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                               : Dali::Constraint::APPLY_ONCE);

    mOpacityConstraintList.push_back(mOpacityConstraint);
  }

  // Renderer needs textures and to be added to control
  mRendererUpdateNeeded = true;

  UpdateRenderer();
}

void TextVisual::RemoveRenderer(Actor& actor, bool removeDefaultRenderer)
{
  for(RendererContainer::iterator iter = mRendererList.begin(); iter != mRendererList.end(); ++iter)
  {
    Renderer renderer = (*iter);
    if(renderer && (removeDefaultRenderer || (renderer != mImpl->mRenderer)))
    {
      // Removes the renderer from the actor.
      actor.RemoveRenderer(renderer);
    }
  }
  // Clear the renderer list
  mRendererList.clear();

  if(removeDefaultRenderer)
  {
    // Remove default renderer's textureset
    mImpl->mRenderer.RemoveTextures();
  }

  // Clear constraint, and keep default renderer's constraint only.
  if(mColorConstraint)
  {
    for(auto& constraint : mColorConstraintList)
    {
      if(constraint && (constraint != mColorConstraint))
      {
        constraint.Remove();
      }
    }
    mColorConstraintList.clear();
    mColorConstraintList.push_back(mColorConstraint);
  }
  if(mOpacityConstraint)
  {
    for(auto& constraint : mOpacityConstraintList)
    {
      if(constraint && (constraint != mOpacityConstraint))
      {
        constraint.Remove();
      }
    }
    mOpacityConstraintList.clear();
    mOpacityConstraintList.push_back(mOpacityConstraint);
  }
}

void TextVisual::DoSetOffScene(Actor& actor)
{
  if(mController->IsAsyncRendering() && mIsTextLoadingTaskRunning)
  {
    Text::AsyncTextManager::Get().RequestCancel(mTextLoadingTaskId);
    mIsTextLoadingTaskRunning = false;
  }

  RemoveRenderer(actor, true);

  // Change the constraint as APPLY_ONCE if apply rate was always.
  if(mIsConstraintAppliedAlways)
  {
    mColorConstraint.SetApplyRate(Dali::Constraint::APPLY_ONCE);
    mOpacityConstraint.SetApplyRate(Dali::Constraint::APPLY_ONCE);
  }

  // Resets the control handle.
  mControl.Reset();
}

void TextVisual::SetFittingMode(Ui::Image::FittingMode fittingMode)
{
  // Do nothing
}

void TextVisual::OnApplyFittingMode(const Vector2& controlSize, const Extents& padding, float effectiveScale)
{
  // Apply UiScale to controller
  if(mController->SetUiScale(effectiveScale))
  {
    mController->InvalidateFontData();

    // Renderer needs textures and to be added to control
    mRendererUpdateNeeded = true;
  }

  // Update control size without change transform.
  Visual::Base::OnApplyFittingMode(controlSize, padding, effectiveScale);
}

void TextVisual::OnSetTransform()
{
  UpdateRenderer();
}

void TextVisual::DoSetProperty(Dali::Property::Index index, const Dali::Property::Value& propertyValue)
{
  switch(index)
  {
    case Ui::TextVisualPropertyIndex::TEXT:
    {
      mController->SetText(ToStdString(propertyValue));
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_FAMILY:
    {
      SetFontFamilyProperty(mController, propertyValue);
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_SIZE:
    {
      const float fontSize = propertyValue.Get<float>();
      if(!Equals(mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), fontSize))
      {
        mController->SetDefaultFontSize(fontSize, Text::Controller::PIXEL_SIZE);
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_WEIGHT:
    {
      Text::FontWeight weight(static_cast<Text::FontWeight>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontWeightEnumeration(propertyValue, weight))
      {
        mController->SetDefaultFontWeight(Text::ToTextAbstractionFontWeight(weight));
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_WIDTH:
    {
      Text::FontWidth width(static_cast<Text::FontWidth>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontWidthEnumeration(propertyValue, width))
      {
        mController->SetDefaultFontWidth(Text::ToTextAbstractionFontWidth(width));
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_SLANT:
    {
      Text::FontSlant slant(static_cast<Text::FontSlant>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontSlantEnumeration(propertyValue, slant))
      {
        mController->SetDefaultFontSlant(Text::ToTextAbstractionFontSlant(slant));
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::MULTI_LINE:
    {
      mController->SetMultiLineEnabled(propertyValue.Get<bool>());
      break;
    }
    case Ui::TextVisualPropertyIndex::LINE_WRAP_MODE:
    {
      Text::LineWrapMode mode(static_cast<Text::LineWrapMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetLineWrapModeEnumeration(propertyValue, mode))
      {
        mController->SetLineWrapMode(mode);
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      if(mController)
      {
        Text::Alignment alignment(static_cast<Text::Alignment>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(Ui::Text::GetHorizontalAlignmentEnumeration(propertyValue, alignment))
        {
          mController->SetHorizontalAlignment(alignment);
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::VERTICAL_ALIGNMENT:
    {
      if(mController)
      {
        Text::Alignment alignment(static_cast<Text::Alignment>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(Ui::Text::GetVerticalAlignmentEnumeration(propertyValue, alignment))
        {
          mController->SetVerticalAlignment(alignment);
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::OVERFLOW_MODE:
    {
      Text::OverflowMode mode(static_cast<Text::OverflowMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetOverflowModeEnumeration(propertyValue, mode))
      {
        if(mOverflowMode != mode)
        {
          mOverflowMode = mode;
          switch(mode)
          {
            case Text::OverflowMode::CLIP:
            {
              mController->SetTextElideEnabled(false);
              break;
            }
            case Text::OverflowMode::ELLIPSIS:
            {
              mController->SetTextElideEnabled(true);
              break;
            }
          }
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::LINE_HEIGHT:
    {
      float lineHeight = 0.0f;
      if(propertyValue.Get(lineHeight))
      {
        if(!Dali::Equals(mLineHeight, lineHeight))
        {
          mLineHeight = lineHeight;
          UpdateLineHeight();
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::LINE_HEIGHT_MODE:
    {
      Text::LineHeightMode mode(static_cast<Text::LineHeightMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetLineHeightModeEnumeration(propertyValue, mode))
      {
        if(mLineHeightMode != mode)
        {
          mLineHeightMode = mode;
          UpdateLineHeight();
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::TEXT_COLOR:
    {
      const Vector4& textColor = propertyValue.Get<Vector4>();
      if(mController->GetDefaultColor() != textColor)
      {
        mController->SetDefaultColor(textColor);
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::MARKUP_ENABLED:
    {
      mController->SetMarkupProcessorEnabled(propertyValue.Get<bool>());
      break;
    }
  }
}

void TextVisual::UpdateLineHeight()
{
  if(Equals(mLineHeight, Text::LINE_HEIGHT_AUTO, Math::MACHINE_EPSILON_1000))
  {
    // clear explicit line height and use the natural line height.
    mController->SetRelativeLineSize(-1.0f);
    mController->SetDefaultLineSize(0.0f);
  }
  else if(mLineHeightMode == Text::LineHeightMode::RELATIVE)
  {
    mController->SetDefaultLineSize(0.0f);
    mController->SetRelativeLineSize(mLineHeight);
  }
  else // LineHeightMode::ABSOLUTE
  {
    mController->SetRelativeLineSize(-1.0f);
    mController->SetDefaultLineSize(mLineHeight);
  }
}

void TextVisual::UpdateRenderer()
{
  if(mController->IsAsyncRendering())
  {
    return;
  }

  Actor control = mControl.GetHandle();
  if(!control)
  {
    // Nothing to do.
    return;
  }

  // Calculates the size to be used to relayout.
  Vector2 relayoutSize = mImpl->GetTransformVisualSize(mImpl->mControlSize);
  // TODO : Round the size and offset to avoid pixel alignment issues.

  auto textLengthUtf32 = mController->GetNumberOfCharacters();

  if((fabsf(relayoutSize.width) < Math::MACHINE_EPSILON_1000) ||
     (fabsf(relayoutSize.height) < Math::MACHINE_EPSILON_1000) || textLengthUtf32 == 0u)
  {
    // Remove the texture set and any renderer previously set.
    RemoveRenderer(control, true);

    // Nothing else to do if the relayout size is zero.
    ResourceReady(Ui::Visual::ResourceStatus::READY);
    return;
  }

  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(control);

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout(relayoutSize, layoutDirection);

  if(Text::Controller::NONE_UPDATED != (Text::Controller::MODEL_UPDATED & updateTextType) || mRendererUpdateNeeded)
  {
    mRendererUpdateNeeded = false;

    // Remove the texture set and any renderer previously set.
    // Note, we don't need to remove the mImpl->Renderer, since it will be added again after AddRenderer call.
    RemoveRenderer(control, false);

    if((relayoutSize.width > Math::MACHINE_EPSILON_1000) && (relayoutSize.height > Math::MACHINE_EPSILON_1000))
    {
      // Check whether it is a markup text with multiple text colors
      const Vector4* const colorsBuffer          = mController->GetTextModel()->GetColors();
      bool                 hasMultipleTextColors = (NULL != colorsBuffer);

      // Check whether the text contains any color glyph
      bool containsColorGlyph = false;

      TextAbstraction::FontClient  fontClient     = TextAbstraction::FontClient::Get();
      const Text::GlyphInfo* const glyphsBuffer   = mController->GetTextModel()->GetGlyphs();
      const Text::Length           numberOfGlyphs = mController->GetTextModel()->GetNumberOfGlyphs();
      for(Text::Length glyphIndex = 0; glyphIndex < numberOfGlyphs; glyphIndex++)
      {
        // Retrieve the glyph's info.
        const Text::GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;

        // Whether the current glyph is a color one.
        if(Text::Internal::IsRenderableColorGlyph(fontClient, glyphInfo->fontId, glyphInfo->index))
        {
          containsColorGlyph = true;
          break;
        }
      }

      // Check whether the text contains any style colors (e.g. underline color, shadow color, etc.)

      const bool shadowEnabled = mController->GetTextModel()->IsShadowEnabled();

      const bool outlineEnabled         = (mController->GetTextModel()->GetOutlineWidth() > Math::MACHINE_EPSILON_1);
      const bool backgroundEnabled      = mController->GetTextModel()->IsBackgroundEnabled();
      const bool markupOrEnabled        = mController->IsMarkupProcessorEnabled();
      const bool markupUnderlineEnabled = markupOrEnabled && mController->GetTextModel()->IsMarkupUnderlineSet();
      const bool markupStrikethroughEnabled =
        markupOrEnabled && mController->GetTextModel()->IsMarkupStrikethroughSet();
      const bool underlineEnabled = mController->GetTextModel()->IsUnderlineEnabled() || markupUnderlineEnabled;
      const bool strikethroughEnabled =
        mController->GetTextModel()->IsStrikethroughEnabled() || markupStrikethroughEnabled;
      const bool backgroundMarkupSet         = mController->GetTextModel()->IsMarkupBackgroundColorSet();
      const bool cutoutEnabled               = mController->IsTextCutout();
      const bool backgroundWithCutoutEnabled = mController->GetTextModel()->IsBackgroundWithCutoutEnabled();
      const bool styleEnabled                = (shadowEnabled || outlineEnabled || backgroundEnabled || markupOrEnabled ||
                                 backgroundMarkupSet || cutoutEnabled || backgroundWithCutoutEnabled);
      const bool isOverlayStyle              = underlineEnabled || strikethroughEnabled;
      const bool embossEnabled               = mController->IsEmbossEnabled();

      // if background with cutout is enabled, This text visual must render the entire control size.

      if(cutoutEnabled)
      {
        if(mImpl->mTransformMapUsingDefault)
        {
          relayoutSize = mImpl->mControlSize;
          mController->SetOffsetWithCutout(Vector2::ZERO);
        }
        else
        {
          // mTransform stores the size and offset of the current visual.
          // padding and alignment information is stored in mOffset.
          // When Cutout Enabled, the current visual must draw the entire control.
          // so set the size to controlSize and offset to 0.

          relayoutSize = mImpl->mControlSize;

          // Note : TextVisual which is not created by Label should not support cutout feature!
          // We can assume that current transform is absolute offset / size which is effectScale applied already
          // and the visualTransform will be change for next layout (mean, we can reset it as initial value freely).

          auto& visualTransform = mImpl->GetOrCreateTransform();

          // Relayout to the original size has been completed, so save only the offset information and use it in
          // typesetter.
          // Note : We reset OffsetWithCutout whenever visual trasnform updated. So we need to "append" the offset
          // if UpdateRenderer() called without visual transform changed.
          const Vector2& previousCutoutOffset = mController->GetTextModel()->GetOffsetWithCutout();
          mController->SetOffsetWithCutout(visualTransform.mOffset + previousCutoutOffset);

          visualTransform.SetPropertyMap(Property::Map());
        }
      }

      AddRenderer(control, relayoutSize, hasMultipleTextColors, containsColorGlyph, styleEnabled, isOverlayStyle,
                  embossEnabled);

      // Text rendered and ready to display
      ResourceReady(Ui::Visual::ResourceStatus::READY);
    }
  }
}

void TextVisual::AddTexture(TextureSet& textureSet, PixelData& data, Sampler& sampler, unsigned int textureSetIndex)
{
  Texture texture =
    Texture::New(Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(), data.GetWidth(), data.GetHeight());
#if defined(GPU_MEMORY_PROFILE_ENABLED)
  {
    std::string text;
    mController->GetText(text);
    Dali::Integration::TextureUploadWithContent(texture, data, ToDaliString(std::move(text)), Dali::Integration::TextureContextTypeHint::TEXT_SIMPLE_LABEL);
  }
#else
  texture.Upload(data);
#endif

  textureSet.SetTexture(textureSetIndex, texture);
  textureSet.SetSampler(textureSetIndex, sampler);
}

void TextVisual::AddTilingTexture(TextureSet& textureSet, TilingInfo& tilingInfo, PixelData& data, Sampler& sampler,
                                  unsigned int textureSetIndex)
{
  Texture texture =
    Texture::New(Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(), tilingInfo.width, tilingInfo.height);
  DevelTexture::UploadSubPixelData(texture, data, 0u, tilingInfo.offsetHeight, tilingInfo.width, tilingInfo.height);

  textureSet.SetTexture(textureSetIndex, texture);
  textureSet.SetSampler(textureSetIndex, sampler);
}

void TextVisual::CreateTextureSet(TilingInfo& info, VisualRenderer& renderer, Sampler& sampler)
{
  TextureSet textureSet      = TextureSet::New();
  uint32_t   textureSetIndex = 0u;

  // Convert the buffer to pixel data to make it a texture.

  if(info.textPixelData)
  {
    AddTilingTexture(textureSet, info, info.textPixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledStyle() && info.stylePixelData)
  {
    AddTilingTexture(textureSet, info, info.stylePixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledOverlay() && info.overlayStylePixelData)
  {
    AddTilingTexture(textureSet, info, info.overlayStylePixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor() && info.maskPixelData)
  {
    AddTilingTexture(textureSet, info, info.maskPixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  renderer.SetTextures(textureSet);

  // Enable the pre-multiplied alpha to improve the text quality
  renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);

  // Set size and offset for the tiling.
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE,
                       Vector2(static_cast<float>(info.width), static_cast<float>(info.height)));
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET, info.transformOffset);
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  renderer.RegisterProperty("uHasMultipleTextColors",
                            static_cast<float>(mTextShaderFeatureCache.IsEnabledMultiColor()));

  mRendererList.push_back(renderer);
}

// From async text manager
void TextVisual::LoadComplete(bool loadingSuccess, const TextInformation& textInformation)
{
  Text::AsyncTextParameters parameters = textInformation.parameters;

#ifdef TRACE_ENABLED
  if(gTraceFilter2 && gTraceFilter2->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("LoadComplete, success:%d, type:%s\n", loadingSuccess,
                          GetRequestTypeName(parameters.requestType));
  }
#endif

  switch(parameters.requestType)
  {
    case Text::Async::RENDER_FIXED_SIZE:
    case Text::Async::RENDER_FIXED_WIDTH:
    case Text::Async::RENDER_FIXED_HEIGHT:
    case Text::Async::RENDER_CONSTRAINT:
    {
      mIsTextLoadingTaskRunning = false;
      break;
    }
    case Text::Async::COMPUTE_NATURAL_SIZE:
    {
      mIsNaturalSizeTaskRunning = false;
      break;
    }
    case Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
      mIsHeightForWidthTaskRunning = false;
      break;
    }
    default:
    {
      DALI_LOG_ERROR("Unexpected request type : %d\n", parameters.requestType);
      break;
    }
  }

  Ui::Visual::ResourceStatus resourceStatus;

  if(loadingSuccess)
  {
    resourceStatus = Ui::Visual::ResourceStatus::READY;

    Text::AsyncTextRenderInfo renderInfo = textInformation.renderInfo;

    if(parameters.requestType == Text::Async::COMPUTE_NATURAL_SIZE ||
       parameters.requestType == Text::Async::COMPUTE_HEIGHT_FOR_WIDTH)
    {
      if(mAsyncTextInterface)
      {
        mAsyncTextInterface->AsyncSizeComputed(renderInfo);
        return;
      }
    }

    Actor control = mControl.GetHandle();
    if(!control)
    {
      // Nothing to do.
      ResourceReady(Ui::Visual::ResourceStatus::READY);
      return;
    }

    // Calculate the size of the visual that can fit the text.
    // The size of the text after it has been laid-out, size of pixel data buffer.
    Size layoutSize = renderInfo.size;

    // Set textWidth, textHeight to the original size requested for rendering.
    bool isRenderScale = parameters.renderScale > 1.0f ? true : false;
    if(isRenderScale)
    {
      parameters.textWidth  = parameters.renderScaleWidth;
      parameters.textHeight = parameters.renderScaleHeight;
    }

    // Size of the text control including padding.
    Vector2 textControlSize(parameters.textWidth + (parameters.padding.start + parameters.padding.end),
                            parameters.textHeight + (parameters.padding.top + parameters.padding.bottom));

    bool isVerticalScroll = false;
    if(parameters.isMarqueeEnabled)
    {
      // In case of marquee, the layout width (renderInfo's width) is the natural size of the text.
      // Since the layout size is the size of the visual transform, it should be reset to the text area excluding
      // padding.
      if(parameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL)
      {
        layoutSize.width = parameters.textWidth;
      }
      else
      {
        layoutSize.height = parameters.textHeight;
        isVerticalScroll  = true;
      }
    }

    // Calculate the offset for vertical alignment only, as the layout engine will do the horizontal alignment.
    Vector2 alignmentOffset;
    alignmentOffset.x = 0.0f;
    alignmentOffset.y = isVerticalScroll ? 0.0f
                                         : (parameters.textHeight - layoutSize.y) *
                                             VERTICAL_ALIGNMENT_TABLE[static_cast<int>(parameters.verticalAlignment)];

    Vector2 visualTransformOffset;
    if(renderInfo.isCutoutEnabled)
    {
      // When Cutout Enabled, the current visual must draw the entire control.
      // so set the size to controlSize and offset to 0.
      visualTransformOffset.x = 0.0f;
      visualTransformOffset.y = 0.0f;

      // The layout size is set to the text control size including padding.
      layoutSize = textControlSize;
    }
    else
    {
      // This affects font rendering quality.
      // It need to be integerized.
      visualTransformOffset.x = roundf(parameters.padding.start + alignmentOffset.x);
      visualTransformOffset.y =
        isRenderScale
          ? roundf((layoutSize.y + parameters.padding.top + alignmentOffset.y) * 2.0f) * 0.5f - layoutSize.y
          : roundf(parameters.padding.top + alignmentOffset.y);
    }

    SetRequireRender(renderInfo.isCutoutEnabled);

    // Mark that we don't use viewEffectiveScale at transform's size & offset for this visual.
    // (Because visual transform size and polic already apply viewEffectiveScale).
    SetTransformMapUsageForFittingMode(true);

    // Reset cutout offset before call SetTransformAndSize.
    // (cutout offset will be set at OnSetTransform)
    if(mController->IsTextCutout())
    {
      mController->SetOffsetWithCutout(Vector2::ZERO);
    }

    // Transform offset is used for subpixel data upload in text tiling.
    // We should set the transform before creating a tiling texture.
    Property::Map visualTransform;
    visualTransform.Add(Ui::Visual::Transform::Property::SIZE, layoutSize)
      .Add(Ui::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::OFFSET, visualTransformOffset)
      .Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::ORIGIN, Ui::Align::TOP_BEGIN)
      .Add(Ui::Visual::Transform::Property::PIVOT, Ui::Align::TOP_BEGIN);
    SetTransformAndSize(visualTransform, textControlSize, parameters.effectiveTextScale);

    Shader shader = GetTextShader(mFactoryCache, TextVisualShaderFeature::FeatureBuilder()
                                                   .EnableMultiColor(renderInfo.hasMultipleTextColors)
                                                   .EnableEmoji(renderInfo.containsColorGlyph)
                                                   .EnableStyle(renderInfo.styleEnabled)
                                                   .EnableOverlay(renderInfo.isOverlayStyle)
                                                   .EnableEmboss(renderInfo.isEmbossEnabled));
    mImpl->mRenderer.SetShader(shader);

    // Remove the texture set and any renderer previously set.
    RemoveRenderer(control, false);

    // Get the maximum texture size.
    const int maxTextureSize = Dali::GetMaxTextureSize();

    // No tiling required. Use the default renderer.
    if(renderInfo.size.height <= static_cast<float>(maxTextureSize))
    {
      // Filter mode needs to be set to linear to produce better quality while scaling.
      Sampler sampler = Sampler::New();
      sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

      TextureSet textureSet = TextureSet::New();

      uint32_t textureSetIndex = 0u;
      AddTexture(textureSet, renderInfo.textPixelData, sampler, textureSetIndex);
      ++textureSetIndex;

      if(mTextShaderFeatureCache.IsEnabledStyle())
      {
        // Create RGBA texture for all the text styles that render in the background (without the text itself)
        AddTexture(textureSet, renderInfo.stylePixelData, sampler, textureSetIndex);
        ++textureSetIndex;
      }
      if(mTextShaderFeatureCache.IsEnabledOverlay())
      {
        // Create RGBA texture for overlay styles such as underline and strikethrough (without the text itself)
        AddTexture(textureSet, renderInfo.overlayStylePixelData, sampler, textureSetIndex);
        ++textureSetIndex;
      }

      if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor())
      {
        // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
        AddTexture(textureSet, renderInfo.maskPixelData, sampler, textureSetIndex);
      }

      mImpl->mRenderer.SetTextures(textureSet);
      mImpl->mRenderer.SetProperty(mHasMultipleTextColorsIndex,
                                   static_cast<float>(mTextShaderFeatureCache.IsEnabledMultiColor()));
      mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

      mRendererList.push_back(mImpl->mRenderer);
    }
    else
    {
      // Filter mode needs to be set to linear to produce better quality while scaling.
      Sampler sampler = Sampler::New();
      sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

      int verifiedWidth  = static_cast<int>(renderInfo.size.width);
      int verifiedHeight = static_cast<int>(renderInfo.size.height);

      // Set information for creating textures.
      TilingInfo info(verifiedWidth, maxTextureSize);

      // Get the pixel data of text.
      info.textPixelData = renderInfo.textPixelData;

      if(mTextShaderFeatureCache.IsEnabledStyle())
      {
        info.stylePixelData = renderInfo.stylePixelData;
      }

      if(mTextShaderFeatureCache.IsEnabledOverlay())
      {
        info.overlayStylePixelData = renderInfo.overlayStylePixelData;
      }

      if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor())
      {
        info.maskPixelData = renderInfo.maskPixelData;
      }

      // Get the current offset for recalculate the offset when tiling.
      Property::Map retMap;
      if(mImpl->mTransform)
      {
        mImpl->mTransform->GetPropertyMap(retMap);
        Property::Value* offsetValue = retMap.Find(Dali::Ui::Visual::Transform::Property::OFFSET);
        if(offsetValue)
        {
          offsetValue->Get(info.transformOffset);
        }
      }

      // Create a textureset in the default renderer.
      CreateTextureSet(info, mImpl->mRenderer, sampler);

      verifiedHeight -= maxTextureSize;

      Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

      // Create a renderer by cutting maxTextureSize.
      while(verifiedHeight > 0)
      {
        VisualRenderer tilingRenderer = VisualRenderer::New(geometry, shader);
        tilingRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Ui::DepthIndex::CONTENT);
        // New offset position of buffer for tiling.
        info.offsetHeight += static_cast<uint32_t>(maxTextureSize);
        // New height for tiling.
        info.height = (verifiedHeight - maxTextureSize) > 0 ? maxTextureSize : verifiedHeight;
        // New offset for tiling.
        info.transformOffset.y += static_cast<float>(maxTextureSize);

        // Create a textureset int the new tiling renderer.
        CreateTextureSet(info, tilingRenderer, sampler);

        verifiedHeight -= maxTextureSize;
      }
    }

    const Vector4& defaultColor = parameters.textColor;

    for(RendererContainer::iterator iter = mRendererList.begin(); iter != mRendererList.end(); ++iter)
    {
      VisualRenderer renderer = (*iter);
      if(renderer)
      {
        // Register transform properties
        mImpl->SetTransformUniforms(renderer, static_cast<Ui::Direction::Type>(Text::Direction::LEFT_TO_RIGHT));

        control.AddRenderer(renderer);

        if(renderInfo.isEmbossEnabled)
        {
          float          sizeX             = std::max(layoutSize.x, Math::MACHINE_EPSILON_100);
          float          sizeY             = std::max(std::min((float)maxTextureSize, layoutSize.y), Math::MACHINE_EPSILON_100);
          const Vector2& embossSize        = Vector2(1.0f / sizeX, 1.0f / sizeY);
          const Vector2& embossDirection   = parameters.embossDirection;
          const float    embossStrength    = parameters.embossStrength;
          const Vector4& embossLightColor  = parameters.embossLightColor;
          const Vector4& embossShadowColor = parameters.embossShadowColor;

          renderer.RegisterProperty("uEmbossSize", embossSize);
          renderer.RegisterProperty("uEmbossDirection", embossDirection);
          renderer.RegisterProperty("uEmbossStrength", embossStrength);
          renderer.RegisterProperty("uEmbossLightColor", embossLightColor);
          renderer.RegisterProperty("uEmbossShadowColor", embossShadowColor);
        }

        if(renderer != mImpl->mRenderer)
        {
          // Set constraint for text label's color for non-default renderers.
          if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX)
          {
            // Register unique property, or get property for default renderer.
            Property::Index index = renderer.RegisterUniqueProperty("uTextColorAnimatable", defaultColor);

            // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
            if(index != Property::INVALID_INDEX)
            {
              Constraint colorConstraint = Constraint::New<Vector4>(renderer, index, TextColorConstraint);
              colorConstraint.AddSource(Source(control, mAnimatableTextColorPropertyIndex));
              colorConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                      : Dali::Constraint::APPLY_ONCE);
              Dali::Integration::ConstraintSetInternalTag(colorConstraint, TEXT_VISUAL_COLOR_CONSTRAINT_TAG);
              colorConstraint.Apply();

              mColorConstraintList.push_back(colorConstraint);
            }

            // Make zero if the alpha value of text color is zero to skip rendering text
            // VisualRenderer::Property::OPACITY uses same animatable property internally.
            Constraint opacityConstraint =
              Constraint::New<float>(renderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint);
            opacityConstraint.AddSource(Source(control, mAnimatableTextColorPropertyIndex));
            opacityConstraint.AddSource(Source(mImpl->mRenderer, mTextRequireRenderPropertyIndex));
            opacityConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                      : Dali::Constraint::APPLY_ONCE);
            Dali::Integration::ConstraintSetInternalTag(opacityConstraint, TEXT_VISUAL_OPACITY_CONSTRAINT_TAG);
            opacityConstraint.Apply();

            mOpacityConstraintList.push_back(opacityConstraint);
          }
        }
      }
    }

    if(mAsyncTextInterface && parameters.isMarqueeEnabled)
    {
      mAsyncTextInterface->AsyncInitializeMarquee(renderInfo);
    }

    if(mAsyncTextInterface && parameters.isTextFitEnabled)
    {
      mAsyncTextInterface->AsyncTextFitChanged(parameters.fontSize);
    }

    if(mAsyncTextInterface)
    {
      mAsyncTextInterface->AsyncRenderFinished(renderInfo);
    }

    // Ignore current result when user re-request async load during load complete callback.
    if(mIsTextLoadingTaskRunning)
    {
      // Remove the texture set and any renderer previously set.
      RemoveRenderer(control, true);
      return;
    }
    else
    {
      // Apply constraint once after async text completed.
      SetConstraintApplyAlways(mIsConstraintAppliedAlways, true);
    }
  }
  else
  {
    resourceStatus = Ui::Visual::ResourceStatus::FAILED;
  }

  // Signal to observers ( control ) that resources are ready. Must be all resources.
  ResourceReady(resourceStatus);
}

void TextVisual::SetAsyncTextInterface(Text::AsyncTextInterface* asyncTextInterface)
{
  mAsyncTextInterface = asyncTextInterface;
}

void TextVisual::SetConstraintApplyAlways(bool applyAlways, bool notifyToConstraint)
{
  if(mIsConstraintAppliedAlways != applyAlways || notifyToConstraint)
  {
    mIsConstraintAppliedAlways = applyAlways;

    // Change apply rate only if it is scene on.
    if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX && mControl.GetHandle())
    {
      for(auto& constraint : mColorConstraintList)
      {
        if(constraint)
        {
          constraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                             : Dali::Constraint::APPLY_ONCE);
        }
      }
      for(auto& constraint : mOpacityConstraintList)
      {
        if(constraint)
        {
          constraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                             : Dali::Constraint::APPLY_ONCE);
        }
      }
    }
  }
}

void TextVisual::RequestAsyncSizeComputation(Text::AsyncTextParameters& parameters)
{
#ifdef TRACE_ENABLED
  if(gTraceFilter2 && gTraceFilter2->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("Request size computation, type:%s\n", GetRequestTypeName(parameters.requestType));
  }
#endif

  switch(parameters.requestType)
  {
    case Text::Async::COMPUTE_NATURAL_SIZE:
    {
      if(mIsNaturalSizeTaskRunning)
      {
        Text::AsyncTextManager::Get().RequestCancel(mNaturalSizeTaskId);
      }
      mIsNaturalSizeTaskRunning = true;

      TextLoadObserver* textLoadObserver = this;
      mNaturalSizeTaskId                 = Text::AsyncTextManager::Get().RequestLoad(parameters, textLoadObserver);
      break;
    }
    case Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
      if(mIsHeightForWidthTaskRunning)
      {
        Text::AsyncTextManager::Get().RequestCancel(mHeightForWidthTaskId);
      }
      mIsHeightForWidthTaskRunning = true;

      TextLoadObserver* textLoadObserver = this;
      mHeightForWidthTaskId              = Text::AsyncTextManager::Get().RequestLoad(parameters, textLoadObserver);
      break;
    }
    default:
    {
      DALI_LOG_ERROR("Unexpected request type : %d\n", parameters.requestType);
      break;
    }
  }
}

bool TextVisual::UpdateAsyncRenderer(Text::AsyncTextParameters& parameters)
{
  Actor control = mControl.GetHandle();
  if(!control)
  {
    // Nothing to do.
    ResourceReady(Ui::Visual::ResourceStatus::READY);
    return false;
  }

  if((fabsf(parameters.textWidth) < Math::MACHINE_EPSILON_1000) ||
     (fabsf(parameters.textHeight) < Math::MACHINE_EPSILON_1000) || parameters.text.empty())
  {
    if(mIsTextLoadingTaskRunning)
    {
      Text::AsyncTextManager::Get().RequestCancel(mTextLoadingTaskId);
      mIsTextLoadingTaskRunning = false;
    }

    // Remove the texture set and any renderer previously set.
    RemoveRenderer(control, true);

    // Nothing else to do if the relayout size is zero.
    ResourceReady(Ui::Visual::ResourceStatus::READY);

    if(mAsyncTextInterface)
    {
      Text::AsyncTextRenderInfo renderInfo;
      if(parameters.requestType == Text::Async::RENDER_FIXED_SIZE)
      {
        renderInfo.renderedSize = Size(parameters.textWidth, parameters.textHeight);
      }
      else if(parameters.requestType == Text::Async::RENDER_FIXED_WIDTH)
      {
        renderInfo.renderedSize = Size(parameters.textWidth, 0.0f);
      }
      else if(parameters.requestType == Text::Async::RENDER_FIXED_HEIGHT)
      {
        renderInfo.renderedSize = Size(0.0f, parameters.textHeight);
      }
      else
      {
        renderInfo.renderedSize = Size::ZERO;
      }

      mAsyncTextInterface->AsyncRenderFinished(renderInfo);
    }

    return true;
  }

  // Get the maximum texture size.
  const int maxTextureSize = Dali::GetMaxTextureSize();

  if(parameters.textWidth > maxTextureSize)
  {
    DALI_LOG_DEBUG_INFO(
      "layoutSize(%f) > maxTextureSize(%d): To guarantee the behavior of Texture::New, layoutSize must not be bigger "
      "than maxTextureSize\n",
      parameters.textWidth, maxTextureSize);
    parameters.textWidth = maxTextureSize;
  }

  // This does not mean whether task is actually running or waiting.
  // It is whether text visual received a completion callback after requesting a task.
  if(mIsTextLoadingTaskRunning)
  {
    Text::AsyncTextManager::Get().RequestCancel(mTextLoadingTaskId);
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter2 && gTraceFilter2->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("Request render, type:%s\n", GetRequestTypeName(parameters.requestType));
  }
#endif

  mIsTextLoadingTaskRunning          = true;
  TextLoadObserver* textLoadObserver = this;
  mTextLoadingTaskId                 = Text::AsyncTextManager::Get().RequestLoad(parameters, textLoadObserver);

  return true;
}

void TextVisual::AddRenderer(Actor& actor, const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph,
                             bool styleEnabled, bool isOverlayStyle, bool embossEnabled)
{
  Shader shader = GetTextShader(mFactoryCache, TextVisualShaderFeature::FeatureBuilder()
                                                 .EnableMultiColor(hasMultipleTextColors)
                                                 .EnableEmoji(containsColorGlyph)
                                                 .EnableStyle(styleEnabled)
                                                 .EnableOverlay(isOverlayStyle)
                                                 .EnableEmboss(embossEnabled));
  mImpl->mRenderer.SetShader(shader);

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_VISUAL_UPDATE_RENDERER");

  // Get the maximum size.
  const int maxTextureSize = Dali::GetMaxTextureSize();

  // No tiling required. Use the default renderer.
  if(size.height < maxTextureSize)
  {
    TextureSet textureSet = GetTextTexture(size);

    mImpl->mRenderer.SetTextures(textureSet);
    mImpl->mRenderer.SetProperty(mHasMultipleTextColorsIndex, static_cast<float>(hasMultipleTextColors));
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

    mRendererList.push_back(mImpl->mRenderer);
  }
  // If the pixel data exceeds the maximum size, tiling is required.
  else
  {
    // Filter mode needs to be set to linear to produce better quality while scaling.
    Sampler sampler = Sampler::New();
    sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

    // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
    Pixel::Format textPixelFormat = (containsColorGlyph || hasMultipleTextColors) ? Pixel::RGBA8888 : Pixel::L8;

    // Check the text direction
    Text::Direction textDirection = mController->GetTextDirection();

    // Create a texture for the text without any styles
    PixelData data =
      mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);

    int verifiedWidth  = data.GetWidth();
    int verifiedHeight = data.GetHeight();

    // Set information for creating textures.
    TilingInfo info(verifiedWidth, maxTextureSize);

    // Get the pixel data of text.
    info.textPixelData = data;

    if(mTextShaderFeatureCache.IsEnabledStyle())
    {
      // Create RGBA texture for all the text styles (without the text itself)
      info.stylePixelData =
        mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);
    }

    if(mTextShaderFeatureCache.IsEnabledOverlay())
    {
      // Create RGBA texture for all the overlay styles
      info.overlayStylePixelData =
        mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_OVERLAY_STYLE, false, Pixel::RGBA8888);
    }

    if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor())
    {
      // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
      info.maskPixelData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);
    }

    // Get the current offset for recalculate the offset when tiling.
    Property::Map retMap;
    if(mImpl->mTransform)
    {
      mImpl->mTransform->GetPropertyMap(retMap);
      Property::Value* offsetValue = retMap.Find(Dali::Ui::Visual::Transform::Property::OFFSET);
      if(offsetValue)
      {
        offsetValue->Get(info.transformOffset);
      }
    }

    // Create a textureset in the default renderer.
    CreateTextureSet(info, mImpl->mRenderer, sampler);

    verifiedHeight -= maxTextureSize;

    Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

    // Create a renderer by cutting maxTextureSize.
    while(verifiedHeight > 0)
    {
      VisualRenderer tilingRenderer = VisualRenderer::New(geometry, shader);
      tilingRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Ui::DepthIndex::CONTENT);
      // New offset position of buffer for tiling.
      info.offsetHeight += maxTextureSize;
      // New height for tiling.
      info.height = (verifiedHeight - maxTextureSize) > 0 ? maxTextureSize : verifiedHeight;
      // New offset for tiling.
      info.transformOffset.y += maxTextureSize;
      // Create a textureset int the new tiling renderer.
      CreateTextureSet(info, tilingRenderer, sampler);

      verifiedHeight -= maxTextureSize;
    }
  }

  const Vector4& defaultColor = mController->GetTextModel()->GetDefaultColor();

  for(RendererContainer::iterator iter = mRendererList.begin(); iter != mRendererList.end(); ++iter)
  {
    VisualRenderer renderer = (*iter);
    if(renderer)
    {
      // Register transform properties
      mImpl->SetTransformUniforms(renderer, static_cast<Ui::Direction::Type>(Text::Direction::LEFT_TO_RIGHT));

      // Note, AddRenderer will ignore renderer if it is already added.
      actor.AddRenderer(renderer);

      if(embossEnabled)
      {
        float          sizeX             = std::max(size.x, Math::MACHINE_EPSILON_100);
        float          sizeY             = std::max(std::min((float)maxTextureSize, size.y), Math::MACHINE_EPSILON_100);
        const Vector2& embossSize        = Vector2(1.0f / sizeX, 1.0f / sizeY);
        const Vector2& embossDirection   = mController->GetEmbossDirection();
        const float    embossStrength    = mController->GetEmbossStrength();
        const Vector4& embossLightColor  = mController->GetEmbossLightColor();
        const Vector4& embossShadowColor = mController->GetEmbossShadowColor();

        renderer.RegisterProperty("uEmbossSize", embossSize);
        renderer.RegisterProperty("uEmbossDirection", embossDirection);
        renderer.RegisterProperty("uEmbossStrength", embossStrength);
        renderer.RegisterProperty("uEmbossLightColor", embossLightColor);
        renderer.RegisterProperty("uEmbossShadowColor", embossShadowColor);
      }

      if(renderer != mImpl->mRenderer)
      {
        // Set constraint for text label's color for non-default renderers.
        if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX)
        {
          // Register unique property, or get property for default renderer.
          Property::Index index = renderer.RegisterUniqueProperty("uTextColorAnimatable", defaultColor);

          // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
          if(index != Property::INVALID_INDEX)
          {
            Constraint colorConstraint = Constraint::New<Vector4>(renderer, index, TextColorConstraint);
            colorConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
            colorConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                    : Dali::Constraint::APPLY_ONCE);
            Dali::Integration::ConstraintSetInternalTag(colorConstraint, TEXT_VISUAL_COLOR_CONSTRAINT_TAG);
            colorConstraint.Apply();

            mColorConstraintList.push_back(colorConstraint);
          }

          // Make zero if the alpha value of text color is zero to skip rendering text
          // VisualRenderer::Property::OPACITY uses same animatable property internally.
          Constraint opacityConstraint =
            Constraint::New<float>(renderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint);
          opacityConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
          opacityConstraint.AddSource(Source(mImpl->mRenderer, mTextRequireRenderPropertyIndex));
          opacityConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                    : Dali::Constraint::APPLY_ONCE);
          Dali::Integration::ConstraintSetInternalTag(opacityConstraint, TEXT_VISUAL_OPACITY_CONSTRAINT_TAG);
          opacityConstraint.Apply();

          mOpacityConstraintList.push_back(opacityConstraint);
        }
      }
    }
  }
}

TextureSet TextVisual::GetTextTexture(const Vector2& size)
{
  const bool cutoutEnabled = mController->IsTextCutout();

  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

  TextureSet textureSet = TextureSet::New();

  // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
  Pixel::Format textPixelFormat =
    (mTextShaderFeatureCache.IsEnabledEmoji() || mTextShaderFeatureCache.IsEnabledMultiColor() || cutoutEnabled)
      ? Pixel::RGBA8888
      : Pixel::L8;

  // Check the text direction
  Text::Direction textDirection   = mController->GetTextDirection();
  uint32_t        textureSetIndex = 0u;
  // Create a texture for the text without any styles

  Devel::PixelBuffer cutoutData;
  float              cutoutAlpha = mController->GetTextModel()->GetDefaultColor().a;
  if(cutoutEnabled)
  {
    cutoutData = mTypesetter->RenderWithPixelBuffer(size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false,
                                                    textPixelFormat);

    // Make transparent buffer.
    // If the cutout is enabled, a separate texture is not used for the text.
    Devel::PixelBuffer buffer = mTypesetter->CreateFullBackgroundBuffer(1, 1, Vector4(0.f, 0.f, 0.f, 0.f));
    PixelData          data   = Devel::PixelBuffer::Convert(buffer);
    AddTexture(textureSet, data, sampler, textureSetIndex);
    ++textureSetIndex;
  }
  else
  {
    PixelData data =
      mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);
    AddTexture(textureSet, data, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledStyle())
  {
    // Create RGBA texture for all the text styles that render in the background (without the text itself)
    PixelData styleData;

    if(cutoutEnabled && cutoutData)
    {
      styleData = mTypesetter->RenderWithCutout(size, textDirection, cutoutData, Text::Typesetter::RENDER_NO_TEXT,
                                                false, Pixel::RGBA8888, cutoutAlpha);
    }
    else
    {
      styleData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);
    }

    AddTexture(textureSet, styleData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledOverlay())
  {
    // Create RGBA texture for overlay styles such as underline and strikethrough (without the text itself)
    PixelData overlayStyleData =
      mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_OVERLAY_STYLE, false, Pixel::RGBA8888);
    AddTexture(textureSet, overlayStyleData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor())
  {
    // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
    PixelData maskData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);

    AddTexture(textureSet, maskData, sampler, textureSetIndex);
  }

  return textureSet;
}

Shader TextVisual::GetTextShader(VisualFactoryCache&                      factoryCache,
                                 TextVisualShaderFeature::FeatureBuilder& featureBuilder)
{
  // Cache feature builder informations.
  mTextShaderFeatureCache = featureBuilder;

  Shader shader = mTextVisualShaderFactory.GetShader(factoryCache, mTextShaderFeatureCache);
  return shader;
}

void TextVisual::SetRequireRender(bool requireRender)
{
  // Avoid function calls if there is no change.
  if(mTextRequireRender != requireRender)
  {
    mTextRequireRender = requireRender;
    if(mImpl->mRenderer)
    {
      mImpl->mRenderer.SetProperty(mTextRequireRenderPropertyIndex, mTextRequireRender);
    }

    // Notify once to opacity constraints
    if(!mIsConstraintAppliedAlways && mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX &&
       mControl.GetHandle())
    {
      for(auto& constraint : mOpacityConstraintList)
      {
        if(constraint)
        {
          constraint.SetApplyRate(Dali::Constraint::APPLY_ONCE);
        }
      }
    }
  }
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
