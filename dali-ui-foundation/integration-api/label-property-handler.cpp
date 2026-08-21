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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/integration-api/label-property-handler.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>
#include <dali-ui-foundation/internal/text/text-font-style.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>

namespace Dali::Ui::Integration
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif
} // unnamed namespace

void LabelImpl::PropertyHandler::SetProperty(Ui::View view, Property::Index index, const Property::Value& value)
{
  LabelImpl& impl = static_cast<LabelImpl&>(GetImpl(view));
  DALI_ASSERT_ALWAYS(impl.mController && "No text controller");
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Set property index:%d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Ui::Text::LabelPropertyIndex::TEXT:
    {
      impl.SetText(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_FAMILY:
    {
      impl.SetFontFamily(value.Get<Dali::String>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_SIZE:
    {
      impl.SetFontSize(value.Get<float>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::MULTI_LINE:
    {
      impl.SetMultiLine(value.Get<bool>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::LINE_WRAP_MODE:
    {
      Ui::Text::LineWrapMode mode(static_cast<Ui::Text::LineWrapMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetLineWrapModeEnumeration(value, mode))
      {
        impl.SetLineWrapMode(mode);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      Ui::Text::Alignment alignment(static_cast<Ui::Text::Alignment>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetHorizontalAlignmentEnumeration(value, alignment))
      {
        impl.SetHorizontalTextAlignment(alignment);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::VERTICAL_ALIGNMENT:
    {
      Ui::Text::Alignment alignment(static_cast<Ui::Text::Alignment>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetVerticalAlignmentEnumeration(value, alignment))
      {
        impl.SetVerticalTextAlignment(alignment);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::OVERFLOW_MODE:
    {
      Ui::Text::OverflowMode mode(static_cast<Ui::Text::OverflowMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetOverflowModeEnumeration(value, mode))
      {
        impl.SetTextOverflowMode(mode);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::LINE_HEIGHT:
    {
      impl.SetLineHeight(value.Get<float>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::LINE_HEIGHT_MODE:
    {
      Ui::Text::LineHeightMode mode(static_cast<Ui::Text::LineHeightMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetLineHeightModeEnumeration(value, mode))
      {
        impl.SetLineHeightMode(mode);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      Ui::Text::LayoutDirectionMode mode(static_cast<Ui::Text::LayoutDirectionMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetLayoutDirectionModeEnumeration(value, mode))
      {
        impl.SetLayoutDirectionMode(mode);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::ANCHOR_COLOR:
    {
      impl.SetAnchorColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::LabelPropertyIndex::ANCHOR_CLICKED_COLOR:
    {
      impl.SetAnchorClickedColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_TRIGGER_POLICY:
    {
      Ui::Text::MarqueeTriggerPolicy policy(static_cast<Ui::Text::MarqueeTriggerPolicy>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetMarqueeTriggerPolicyEnumeration(value, policy))
      {
        impl.SetMarqueeTriggerPolicy(policy);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_SPEED:
    {
      impl.SetMarqueeSpeed(value.Get<int>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_LOOP_COUNT:
    {
      impl.SetMarqueeLoopCount(value.Get<int>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_LOOP_DELAY:
    {
      impl.SetMarqueeLoopDelay(value.Get<float>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_GAP:
    {
      impl.SetMarqueeGap(value.Get<int>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_ORIENTATION:
    {
      Ui::Text::MarqueeOrientation orientation(static_cast<Ui::Text::MarqueeOrientation>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetMarqueeOrientationEnumeration(value, orientation))
      {
        impl.SetMarqueeOrientation(orientation);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_STOP_MODE:
    {
      Ui::Text::MarqueeStopMode mode(static_cast<Ui::Text::MarqueeStopMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetMarqueeStopModeEnumeration(value, mode))
      {
        impl.SetMarqueeStopMode(mode);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_WEIGHT:
    {
      Ui::Text::FontWeight weight(static_cast<Ui::Text::FontWeight>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetFontWeightEnumeration(value, weight))
      {
        impl.SetFontWeight(weight);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_WIDTH:
    {
      Ui::Text::FontWidth width(static_cast<Ui::Text::FontWidth>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetFontWidthEnumeration(value, width))
      {
        impl.SetFontWidth(width);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_SLANT:
    {
      Ui::Text::FontSlant slant(static_cast<Ui::Text::FontSlant>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Ui::Text::GetFontSlantEnumeration(value, slant))
      {
        impl.SetFontSlant(slant);
      }
      break;
    }
    case Ui::Text::LabelPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      impl.SetTextBackgroundColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Ui::Text::LabelPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMinimumFontSizeScale(value.Get<float>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMaximumFontSizeScale(value.Get<float>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      impl.SetSystemFontSizeScaleEnabled(value.Get<bool>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::CUTOUT_ENABLED:
    {
      impl.SetCutoutEnabledInternal(value.Get<bool>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::ASYNC_RENDERING:
    {
      impl.SetAsyncRendering(value.Get<bool>());
      break;
    }
    case Ui::Text::LabelPropertyIndex::RENDER_SCALE:
    {
      impl.SetRenderScale(value.Get<float>());
      break;
    }
  }
}

bool LabelImpl::PropertyHandler::OnPropertySet(LabelImpl& impl, Dali::Property::Index index, const Dali::Property::Value& propertyValue)
{
  switch(index)
  {
    case Dali::Actor::Property::SIZE:
    {
      const Vector2& size = propertyValue.Get<Vector2>();
      if(size != impl.mSize)
      {
        impl.mSize                 = size;
        impl.mIsContentLayoutDirty = true;
      }
      return true;
    }
    case Dali::Actor::Property::SIZE_WIDTH:
    {
      const float width = propertyValue.Get<float>();
      if(width != impl.mSize.width)
      {
        impl.mSize.width           = width;
        impl.mIsContentLayoutDirty = true;
      }
      return true;
    }
    case Dali::Actor::Property::SIZE_HEIGHT:
    {
      const float height = propertyValue.Get<float>();
      if(height != impl.mSize.height)
      {
        impl.mSize.height          = height;
        impl.mIsContentLayoutDirty = true;
      }
      return true;
    }
    case Ui::View::Property::PADDING:
    {
      // Padding changes text content bounds even when the Label size is unchanged.
      impl.mIsContentLayoutDirty = true;
      impl.RequestTextRelayout();
      impl.RequestAsyncRender();
      return true;
    }
    case Ui::View::Property::BACKGROUND:
    {
      impl.OnBackgroundPropertyChanged();
      return true;
    }
    case Ui::Text::LabelPropertyIndex::TEXT_COLOR:
    {
      const Vector4& textColor = propertyValue.Get<Vector4>();
      if(impl.mController->GetDefaultColor() != textColor)
      {
        impl.mController->SetDefaultColor(textColor);
        impl.RequestRendererUpdate();

        // Trigger constraint always.
        if(DALI_LIKELY(impl.mVisual))
        {
          Internal::TextVisual::SetConstraintApplyAlways(impl.mVisual, impl.mTextColorAnimatedCount, true);
        }
      }
      return true;
    }
    case Ui::Text::LabelPropertyIndex::CUTOUT_ENABLED:
    {
      impl.UpdateCutoutState(propertyValue.Get<bool>());
      return true;
    }
    default:
    {
      return impl.HandleVariationPropertySet(index, propertyValue);
    }
  }
}

Property::Value LabelImpl::PropertyHandler::GetProperty(Ui::View view, Property::Index index)
{
  Property::Value value;
  LabelImpl&      impl = static_cast<LabelImpl&>(GetImpl(view));
  DALI_ASSERT_ALWAYS(impl.mController && "No text controller");
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Get property index:%d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Ui::Text::LabelPropertyIndex::TEXT:
    {
      value = impl.GetText();
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_FAMILY:
    {
      value = impl.GetFontFamily();
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_SIZE:
    {
      value = impl.GetFontSize();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MULTI_LINE:
    {
      value = impl.IsMultiLine();
      break;
    }
    case Ui::Text::LabelPropertyIndex::LINE_WRAP_MODE:
    {
      value = impl.GetLineWrapMode();
      break;
    }
    case Ui::Text::LabelPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      value = impl.GetHorizontalTextAlignment();
      break;
    }
    case Ui::Text::LabelPropertyIndex::VERTICAL_ALIGNMENT:
    {
      value = impl.GetVerticalTextAlignment();
      break;
    }
    case Ui::Text::LabelPropertyIndex::OVERFLOW_MODE:
    {
      value = impl.GetTextOverflowMode();
      break;
    }
    case Ui::Text::LabelPropertyIndex::LINE_HEIGHT:
    {
      value = impl.GetLineHeight();
      break;
    }
    case Ui::Text::LabelPropertyIndex::LINE_HEIGHT_MODE:
    {
      value = impl.GetLineHeightMode();
      break;
    }
    case Ui::Text::LabelPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      value = impl.GetLayoutDirectionMode();
      break;
    }
    case Ui::Text::LabelPropertyIndex::ANCHOR_COLOR:
    {
      value = impl.GetAnchorColor().GetRgba();
      break;
    }
    case Ui::Text::LabelPropertyIndex::ANCHOR_CLICKED_COLOR:
    {
      value = impl.GetAnchorClickedColor().GetRgba();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_TRIGGER_POLICY:
    {
      value = impl.GetMarqueeTriggerPolicy();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_SPEED:
    {
      value = impl.GetMarqueeSpeed();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_LOOP_COUNT:
    {
      value = impl.GetMarqueeLoopCount();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_LOOP_DELAY:
    {
      value = impl.GetMarqueeLoopDelay();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_GAP:
    {
      value = impl.GetMarqueeGap();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_ORIENTATION:
    {
      value = impl.GetMarqueeOrientation();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MARQUEE_STOP_MODE:
    {
      value = impl.GetMarqueeStopMode();
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_WEIGHT:
    {
      value = impl.GetFontWeight();
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_WIDTH:
    {
      value = impl.GetFontWidth();
      break;
    }
    case Ui::Text::LabelPropertyIndex::FONT_SLANT:
    {
      value = impl.GetFontSlant();
      break;
    }
    case Ui::Text::LabelPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      value = impl.GetTextBackgroundColor().GetRgba();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMinimumFontSizeScale();
      break;
    }
    case Ui::Text::LabelPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMaximumFontSizeScale();
      break;
    }
    case Ui::Text::LabelPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      value = impl.IsSystemFontSizeScaleEnabled();
      break;
    }
    case Ui::Text::LabelPropertyIndex::CUTOUT_ENABLED:
    {
      value = impl.IsTextCutoutEnabled();
      break;
    }
    case Ui::Text::LabelPropertyIndex::ASYNC_RENDERING:
    {
      value = impl.IsAsyncRendering();
      break;
    }
    case Ui::Text::LabelPropertyIndex::RENDER_SCALE:
    {
      value = impl.GetRenderScale();
      break;
    }
  }
  return value;
}

} // namespace Dali::Ui::Integration
