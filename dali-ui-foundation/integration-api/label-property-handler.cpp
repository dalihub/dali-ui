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

namespace Dali::Ui::Integration
{

void LabelImpl::PropertyHandler::SetProperty(Ui::View view, Property::Index index, const Property::Value& value)
{
  LabelImpl& impl = static_cast<LabelImpl&>(GetImpl(view));
  DALI_ASSERT_ALWAYS(impl.mController && "No text controller");
  DALI_LOG_RELEASE_INFO("[%p] index : %d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Text::LabelPropertyIndex::TEXT:
    {
      impl.SetText(value.Get<Dali::String>());
      break;
    }
    case Text::LabelPropertyIndex::FONT_FAMILY:
    {
      impl.SetFontFamily(value.Get<Dali::String>());
      break;
    }
    case Text::LabelPropertyIndex::FONT_SIZE:
    {
      impl.SetFontSize(value.Get<float>());
      break;
    }
    case Text::LabelPropertyIndex::MULTI_LINE:
    {
      impl.SetMultiLine(value.Get<bool>());
      break;
    }
    case Text::LabelPropertyIndex::LINE_WRAP_MODE:
    {
      Text::LineWrapMode mode;
      if(Text::GetLineWrapModeEnumeration(value, mode))
      {
        impl.SetLineWrapMode(mode);
      }
      break;
    }
    case Text::LabelPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      Text::Alignment alignment;
      if(Text::GetHorizontalAlignmentEnumeration(value, alignment))
      {
        impl.SetHorizontalTextAlignment(alignment);
      }
      break;
    }
    case Text::LabelPropertyIndex::VERTICAL_ALIGNMENT:
    {
      Text::Alignment alignment;
      if(Text::GetVerticalAlignmentEnumeration(value, alignment))
      {
        impl.SetVerticalTextAlignment(alignment);
      }
      break;
    }
    case Text::LabelPropertyIndex::OVERFLOW_MODE:
    {
      Text::OverflowMode mode;
      if(Text::GetOverflowModeEnumeration(value, mode))
      {
        impl.SetOverflowMode(mode);
      }
      break;
    }
    case Text::LabelPropertyIndex::LINE_HEIGHT:
    {
      impl.SetLineHeight(value.Get<float>());
      break;
    }
    case Text::LabelPropertyIndex::LINE_HEIGHT_MODE:
    {
      Text::LineHeightMode mode;
      if(Text::GetLineHeightModeEnumeration(value, mode))
      {
        impl.SetLineHeightMode(mode);
      }
      break;
    }
    case Text::LabelPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      Text::LayoutDirectionMode mode;
      if(Text::GetLayoutDirectionModeEnumeration(value, mode))
      {
        impl.SetLayoutDirectionMode(mode);
      }
      break;
    }
    case Text::LabelPropertyIndex::MARKUP_ENABLED:
    {
      impl.SetMarkupEnabled(value.Get<bool>());
      break;
    }
    case Text::LabelPropertyIndex::ANCHOR_COLOR:
    {
      impl.SetAnchorColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::LabelPropertyIndex::ANCHOR_CLICKED_COLOR:
    {
      impl.SetAnchorClickedColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_SPEED:
    {
      impl.SetMarqueeSpeed(value.Get<int>());
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_LOOP_COUNT:
    {
      impl.SetMarqueeLoopCount(value.Get<int>());
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_LOOP_DELAY:
    {
      impl.SetMarqueeLoopDelay(value.Get<float>());
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_GAP:
    {
      impl.SetMarqueeGap(value.Get<int>());
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_ORIENTATION:
    {
      Text::MarqueeOrientation orientation;
      if(Text::GetMarqueeOrientationEnumeration(value, orientation))
      {
        impl.SetMarqueeOrientation(orientation);
      }
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_STOP_MODE:
    {
      Text::MarqueeStopMode mode;
      if(Text::GetMarqueeStopModeEnumeration(value, mode))
      {
        impl.SetMarqueeStopMode(mode);
      }
      break;
    }
    case Text::LabelPropertyIndex::FONT_WEIGHT:
    {
      Text::FontWeight weight;
      if(Text::GetFontWeightEnumeration(value, weight))
      {
        impl.SetFontWeight(weight);
      }
      break;
    }
    case Text::LabelPropertyIndex::FONT_WIDTH:
    {
      Text::FontWidth width;
      if(Text::GetFontWidthEnumeration(value, width))
      {
        impl.SetFontWidth(width);
      }
      break;
    }
    case Text::LabelPropertyIndex::FONT_SLANT:
    {
      Text::FontSlant slant;
      if(Text::GetFontSlantEnumeration(value, slant))
      {
        impl.SetFontSlant(slant);
      }
      break;
    }
    case Text::LabelPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      impl.SetTextBackgroundColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::LabelPropertyIndex::FONT_SIZE_SCALE:
    {
      impl.SetFontSizeScale(value.Get<float>());
      break;
    }
    case Text::LabelPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMinimumFontSizeScale(value.Get<float>());
      break;
    }
    case Text::LabelPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      impl.SetMaximumFontSizeScale(value.Get<float>());
      break;
    }
    case Text::LabelPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      impl.SetSystemFontSizeScaleEnabled(value.Get<bool>());
      break;
    }
    case Text::LabelPropertyIndex::CUTOUT_ENABLED:
    {
      impl.SetCutoutEnabledInternal(value.Get<bool>());
      break;
    }
    case Text::LabelPropertyIndex::ASYNC_RENDERING:
    {
      impl.SetAsyncRendering(value.Get<bool>());
      break;
    }
    case Text::LabelPropertyIndex::RENDER_SCALE:
    {
      impl.SetRenderScale(value.Get<float>());
      break;
    }
  }
}

Property::Value LabelImpl::PropertyHandler::GetProperty(Ui::View view, Property::Index index)
{
  Property::Value value;
  LabelImpl&      impl = static_cast<LabelImpl&>(GetImpl(view));
  DALI_ASSERT_ALWAYS(impl.mController && "No text controller");
  DALI_LOG_RELEASE_INFO("[%p] index : %d\n", impl.mController.Get(), index);

  switch(index)
  {
    case Text::LabelPropertyIndex::TEXT:
    {
      value = impl.GetText();
      break;
    }
    case Text::LabelPropertyIndex::FONT_FAMILY:
    {
      value = impl.GetFontFamily();
      break;
    }
    case Text::LabelPropertyIndex::FONT_SIZE:
    {
      value = impl.GetFontSize();
      break;
    }
    case Text::LabelPropertyIndex::MULTI_LINE:
    {
      value = impl.IsMultiLine();
      break;
    }
    case Text::LabelPropertyIndex::LINE_WRAP_MODE:
    {
      value = impl.GetLineWrapMode();
      break;
    }
    case Text::LabelPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      value = impl.GetHorizontalTextAlignment();
      break;
    }
    case Text::LabelPropertyIndex::VERTICAL_ALIGNMENT:
    {
      value = impl.GetVerticalTextAlignment();
      break;
    }
    case Text::LabelPropertyIndex::OVERFLOW_MODE:
    {
      value = impl.GetOverflowMode();
      break;
    }
    case Text::LabelPropertyIndex::LINE_HEIGHT:
    {
      value = impl.GetLineHeight();
      break;
    }
    case Text::LabelPropertyIndex::LINE_HEIGHT_MODE:
    {
      value = impl.GetLineHeightMode();
      break;
    }
    case Text::LabelPropertyIndex::LAYOUT_DIRECTION_MODE:
    {
      value = impl.GetLayoutDirectionMode();
      break;
    }
    case Text::LabelPropertyIndex::MARKUP_ENABLED:
    {
      value = impl.IsMarkupEnabled();
      break;
    }
    case Text::LabelPropertyIndex::ANCHOR_COLOR:
    {
      value = impl.GetAnchorColor().Resolve();
      break;
    }
    case Text::LabelPropertyIndex::ANCHOR_CLICKED_COLOR:
    {
      value = impl.GetAnchorClickedColor().Resolve();
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_SPEED:
    {
      value = impl.GetMarqueeSpeed();
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_LOOP_COUNT:
    {
      value = impl.GetMarqueeLoopCount();
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_LOOP_DELAY:
    {
      value = impl.GetMarqueeLoopDelay();
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_GAP:
    {
      value = impl.GetMarqueeGap();
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_ORIENTATION:
    {
      value = impl.GetMarqueeOrientation();
      break;
    }
    case Text::LabelPropertyIndex::MARQUEE_STOP_MODE:
    {
      value = impl.GetMarqueeStopMode();
      break;
    }
    case Text::LabelPropertyIndex::FONT_WEIGHT:
    {
      value = impl.GetFontWeight();
      break;
    }
    case Text::LabelPropertyIndex::FONT_WIDTH:
    {
      value = impl.GetFontWidth();
      break;
    }
    case Text::LabelPropertyIndex::FONT_SLANT:
    {
      value = impl.GetFontSlant();
      break;
    }
    case Text::LabelPropertyIndex::TEXT_BACKGROUND_COLOR:
    {
      value = impl.GetTextBackgroundColor().Resolve();
      break;
    }
    case Text::LabelPropertyIndex::FONT_SIZE_SCALE:
    {
      value = impl.GetFontSizeScale();
      break;
    }
    case Text::LabelPropertyIndex::MINIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMinimumFontSizeScale();
      break;
    }
    case Text::LabelPropertyIndex::MAXIMUM_FONT_SIZE_SCALE:
    {
      value = impl.GetMaximumFontSizeScale();
      break;
    }
    case Text::LabelPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED:
    {
      value = impl.IsSystemFontSizeScaleEnabled();
      break;
    }
    case Text::LabelPropertyIndex::CUTOUT_ENABLED:
    {
      value = impl.IsCutoutEnabled();
      break;
    }
    case Text::LabelPropertyIndex::ASYNC_RENDERING:
    {
      value = impl.IsAsyncRendering();
      break;
    }
    case Text::LabelPropertyIndex::RENDER_SCALE:
    {
      value = impl.GetRenderScale();
      break;
    }
  }
  return value;
}

} // namespace Dali::Ui::Integration
