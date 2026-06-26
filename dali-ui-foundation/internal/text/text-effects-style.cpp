/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-ui-foundation/internal/text/text-effects-style.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/math/math-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/text-style-properties.h>
#include <dali-ui-foundation/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-ui-foundation/internal/text/property-string-parser.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>

using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
const char* COLOR_KEY        = "color";
const char* OFFSET_KEY       = "offset";
const char* BLUR_RADIUS_KEY  = "blurRadius";
const char* WIDTH_KEY        = "width";
const char* HEIGHT_KEY       = "height";
const char* ENABLE_KEY       = "enable";
const char* TYPE_KEY         = "type";
const char* DASH_WIDTH_KEY   = "dashWidth";
const char* DASH_GAP_KEY     = "dashGap";
const char* DIRECTION_KEY    = "direction";
const char* STRENGTH_KEY     = "strength";
const char* LIGHT_COLOR_KEY  = "lightColor";
const char* SHADOW_COLOR_KEY = "shadowColor";
const char* TRUE_TOKEN       = "true";

const char* EMPTY_STRING = "";
} // namespace

bool ParseShadowProperties(const Property::Map& shadowPropertiesMap, bool& colorDefined, Vector4& color,
                           bool& offsetDefined, Vector2& offset, bool& blurRadiusDefined, float& blurRadius)
{
  const unsigned int numberOfItems = shadowPropertiesMap.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = shadowPropertiesMap.GetKeyValue(index);

    if((Text::ShadowProperty::COLOR == valueGet.first.indexKey) || (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string colorStr = ToStdString(valueGet.second);
        Text::ColorStringToVector4(colorStr.c_str(), colorStr.size(), color);
      }
      else
      {
        color = valueGet.second.Get<Vector4>();
      }
    }
    else if((Text::ShadowProperty::OFFSET == valueGet.first.indexKey) ||
            (OFFSET_KEY == valueGet.first.stringKey))
    {
      /// Offset key.
      offsetDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string offsetStr = ToStdString(valueGet.second);
        StringToVector2(offsetStr.c_str(), offsetStr.size(), offset);
      }
      else
      {
        offset = valueGet.second.Get<Vector2>();
      }
    }
    else if((Text::ShadowProperty::BLUR_RADIUS == valueGet.first.indexKey) ||
            (BLUR_RADIUS_KEY == valueGet.first.stringKey))
    {
      /// Blur radius key.
      blurRadiusDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string blurRadiusStr = ToStdString(valueGet.second);
        blurRadius                      = StringToFloat(blurRadiusStr.c_str());
      }
      else
      {
        blurRadius = valueGet.second.Get<float>();
      }
    }
  }

  return 0u == numberOfItems;
}

bool ParseEmbossProperties(const Property::Map& embossPropertiesMap, bool& enabled, bool& directionDefined,
                           Vector2& direction, bool& strengthDefined, float& strength, bool& lightColorDefined,
                           Vector4& lightColor, bool& shadowColorDefined, Vector4& shadowColor)
{
  const unsigned int numberOfItems = embossPropertiesMap.Count();
  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = embossPropertiesMap.GetKeyValue(index);

    if((Text::Emboss::Property::ENABLE == valueGet.first.indexKey) || (ENABLE_KEY == valueGet.first.stringKey))
    {
      /// Enable key.
      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string enableStr = ToStdString(valueGet.second);
        enabled                     = Text::TokenComparison(TRUE_TOKEN, enableStr.c_str(), enableStr.size());
      }
      else
      {
        enabled = valueGet.second.Get<bool>();
      }
    }
    else if((Text::Emboss::Property::DIRECTION == valueGet.first.indexKey) ||
            (DIRECTION_KEY == valueGet.first.stringKey))
    {
      /// Direction key.
      directionDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string directionStr = ToStdString(valueGet.second);
        StringToVector2(directionStr.c_str(), directionStr.size(), direction);
      }
      else
      {
        direction = valueGet.second.Get<Vector2>();
      }
    }
    else if((Text::Emboss::Property::STRENGTH == valueGet.first.indexKey) ||
            (STRENGTH_KEY == valueGet.first.stringKey))
    {
      /// Strength key.
      strengthDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string strengthStr = ToStdString(valueGet.second);
        strength                      = StringToFloat(strengthStr.c_str());
      }
      else
      {
        strength = valueGet.second.Get<float>();
      }
    }
    else if((Text::Emboss::Property::LIGHT_COLOR == valueGet.first.indexKey) ||
            (LIGHT_COLOR_KEY == valueGet.first.stringKey))
    {
      /// Light Color key.
      lightColorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string lightColorStr = ToStdString(valueGet.second);
        Text::ColorStringToVector4(lightColorStr.c_str(), lightColorStr.size(), lightColor);
      }
      else
      {
        lightColor = valueGet.second.Get<Vector4>();
      }
    }
    else if((Text::Emboss::Property::SHADOW_COLOR == valueGet.first.indexKey) ||
            (SHADOW_COLOR_KEY == valueGet.first.stringKey))
    {
      /// Shadow Color key.
      shadowColorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string shadowColorStr = ToStdString(valueGet.second);
        Text::ColorStringToVector4(shadowColorStr.c_str(), shadowColorStr.size(), shadowColor);
      }
      else
      {
        shadowColor = valueGet.second.Get<Vector4>();
      }
    }
  }

  return 0u == numberOfItems;
}

bool ParseUnderlineProperties(const Property::Map& underlinePropertiesMap, bool& enabled, bool& colorDefined,
                              Vector4& color, bool& heightDefined, float& height, bool& typeDefined,
                              Text::Underline::Type& type, bool& dashWidthDefined, float& dashWidth,
                              bool& dashGapDefined, float& dashGap)
{
  const unsigned int numberOfItems = underlinePropertiesMap.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = underlinePropertiesMap.GetKeyValue(index);

    if((Text::UnderlineProperty::ENABLE == valueGet.first.indexKey) || (ENABLE_KEY == valueGet.first.stringKey))
    {
      /// Enable key.
      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string enableStr = ToStdString(valueGet.second);
        enabled                     = Text::TokenComparison(TRUE_TOKEN, enableStr.c_str(), enableStr.size());
      }
      else
      {
        enabled = valueGet.second.Get<bool>();
      }
    }
    else if((Text::UnderlineProperty::COLOR == valueGet.first.indexKey) ||
            (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string colorStr = ToStdString(valueGet.second);
        Text::ColorStringToVector4(colorStr.c_str(), colorStr.size(), color);
      }
      else
      {
        color = valueGet.second.Get<Vector4>();
      }
    }
    else if((Text::UnderlineProperty::HEIGHT == valueGet.first.indexKey) ||
            (HEIGHT_KEY == valueGet.first.stringKey))
    {
      /// Height key.
      heightDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string heightStr = ToStdString(valueGet.second);
        height                      = StringToFloat(heightStr.c_str());
      }
      else
      {
        height = valueGet.second.Get<float>();
      }
    }
    else if((Text::UnderlineProperty::TYPE == valueGet.first.indexKey) ||
            (TYPE_KEY == valueGet.first.stringKey))
    {
      /// Underline Type key.
      typeDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string typeStr = ToStdString(valueGet.second);
        Text::UnderlineTypeStringToTypeValue(typeStr.c_str(), typeStr.size(), type);
      }
      else
      {
        type = valueGet.second.Get<Text::Underline::Type>();
      }
    }
    else if((Text::UnderlineProperty::DASH_WIDTH == valueGet.first.indexKey) ||
            (DASH_WIDTH_KEY == valueGet.first.stringKey))
    {
      /// Dashed Underline Width key.
      dashWidthDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string dashWidthStr = ToStdString(valueGet.second);
        dashWidth                      = StringToFloat(dashWidthStr.c_str());
      }
      else
      {
        dashWidth = valueGet.second.Get<float>();
      }
    }
    else if((Text::UnderlineProperty::DASH_GAP == valueGet.first.indexKey) ||
            (DASH_GAP_KEY == valueGet.first.stringKey))
    {
      /// Dashed Underline Gap key.
      dashGapDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string dashGapStr = ToStdString(valueGet.second);
        dashGap                      = StringToFloat(dashGapStr.c_str());
      }
      else
      {
        dashGap = valueGet.second.Get<float>();
      }
    }
  }

  return 0u == numberOfItems;
}

bool ParseOutlineProperties(const Property::Map& underlinePropertiesMap, bool& colorDefined, Vector4& color,
                            bool& widthDefined, uint16_t& width, bool& offsetDefined, Vector2& offset,
                            bool& blurRadiusDefined, float& blurRadius)
{
  const unsigned int numberOfItems = underlinePropertiesMap.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = underlinePropertiesMap.GetKeyValue(index);

    if((Text::OutlineProperty::COLOR == valueGet.first.indexKey) || (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;
      color        = valueGet.second.Get<Vector4>();
    }
    else if((Text::OutlineProperty::WIDTH == valueGet.first.indexKey) ||
            (WIDTH_KEY == valueGet.first.stringKey))
    {
      /// Width key.
      widthDefined = true;
      width        = static_cast<uint16_t>(valueGet.second.Get<float>());
    }
    else if((Text::OutlineProperty::OFFSET == valueGet.first.indexKey) ||
            (OFFSET_KEY == valueGet.first.stringKey))
    {
      /// Offset key.
      offsetDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string offsetStr = ToStdString(valueGet.second);
        StringToVector2(offsetStr.c_str(), offsetStr.size(), offset);
      }
      else
      {
        offset = valueGet.second.Get<Vector2>();
      }
    }
    else if((Text::OutlineProperty::BLUR_RADIUS == valueGet.first.indexKey) ||
            (BLUR_RADIUS_KEY == valueGet.first.stringKey))
    {
      /// Blur radius key.
      blurRadiusDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string blurRadiusStr = ToStdString(valueGet.second);
        blurRadius                      = StringToFloat(blurRadiusStr.c_str());
      }
      else
      {
        blurRadius = valueGet.second.Get<float>();
      }
    }
  }

  return 0u == numberOfItems;
}

bool ParseBackgroundProperties(const Property::Map& backgroundProperties, bool& enabled, bool& colorDefined,
                               Vector4& color)
{
  const unsigned int numberOfItems = backgroundProperties.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = backgroundProperties.GetKeyValue(index);

    if((Text::Background::Property::ENABLE == valueGet.first.indexKey) ||
       (ENABLE_KEY == valueGet.first.stringKey))
    {
      /// Enable key.
      enabled = valueGet.second.Get<bool>();
    }
    else if((Text::Background::Property::COLOR == valueGet.first.indexKey) ||
            (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;
      color        = valueGet.second.Get<Vector4>();
    }
  }

  return 0u == numberOfItems;
}

bool ParseStrikethroughProperties(const Property::Map& strikethroughPropertiesMap, bool& enabled, bool& colorDefined,
                                  Vector4& color, bool& heightDefined, float& height)
{
  const unsigned int numberOfItems = strikethroughPropertiesMap.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = strikethroughPropertiesMap.GetKeyValue(index);

    if((Text::StrikethroughProperty::ENABLE == valueGet.first.indexKey) ||
       (ENABLE_KEY == valueGet.first.stringKey))
    {
      /// Enable key.
      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string enableStr = ToStdString(valueGet.second);
        enabled                     = Text::TokenComparison(TRUE_TOKEN, enableStr.c_str(), enableStr.size());
      }
      else
      {
        enabled = valueGet.second.Get<bool>();
      }
    }
    else if((Text::StrikethroughProperty::COLOR == valueGet.first.indexKey) ||
            (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string colorStr = ToStdString(valueGet.second);
        Text::ColorStringToVector4(colorStr.c_str(), colorStr.size(), color);
      }
      else
      {
        color = valueGet.second.Get<Vector4>();
      }
    }
    else if((Text::StrikethroughProperty::HEIGHT == valueGet.first.indexKey) ||
            (HEIGHT_KEY == valueGet.first.stringKey))
    {
      /// Height key.
      heightDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string heightStr = ToStdString(valueGet.second);
        height                      = StringToFloat(heightStr.c_str());
      }
      else
      {
        height = valueGet.second.Get<float>();
      }
    }
  }
  return 0u == numberOfItems;
}

bool SetUnderlineProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool                  enabled      = false;
        bool                  colorDefined = false;
        Vector4               color;
        bool                  heightDefined = false;
        float                 height        = 0.f;
        bool                  typeDefined   = false;
        Text::Underline::Type type;
        bool                  dashWidthDefined = false;
        float                 dashWidth        = 2.0f;
        bool                  dashGapDefined   = false;
        float                 dashGap          = 1.0f;

        bool empty = true;

        if(propertiesMap.Empty())
        {
          // Map empty so check if a string provided
          const std::string propertyString = ToStdString(value);

          if(!propertyString.empty())
          {
            Property::Map parsedStringMap;
            Text::ParsePropertyString(propertyString, parsedStringMap);

            empty = ParseUnderlineProperties(parsedStringMap, enabled, colorDefined, color, heightDefined, height,
                                             typeDefined, type, dashWidthDefined, dashWidth, dashGapDefined, dashGap);

            controller->UnderlineSetByString(!empty);
          }
        }
        else
        {
          empty = ParseUnderlineProperties(propertiesMap, enabled, colorDefined, color, heightDefined, height,
                                           typeDefined, type, dashWidthDefined, dashWidth, dashGapDefined, dashGap);

          controller->UnderlineSetByString(false);
        }

        if(!empty)
        {
          if(enabled != controller->IsUnderlineEnabled())
          {
            controller->SetUnderlineEnabled(enabled);
            update = true;
          }

          // Sets the default underline values.
          if(colorDefined && (controller->GetUnderlineColor() != color))
          {
            controller->SetUnderlineColor(color);
            update = true;
          }

          if(heightDefined && (fabsf(controller->GetUnderlineHeight() - height) > Math::MACHINE_EPSILON_1000))
          {
            controller->SetUnderlineHeight(height);
            update = true;
          }

          if(typeDefined && (controller->GetUnderlineType() != type))
          {
            controller->SetUnderlineType(type);
            update = true;
          }

          if(dashWidthDefined &&
             (fabsf(controller->GetDashedUnderlineWidth() - dashWidth) > Math::MACHINE_EPSILON_1000))
          {
            controller->SetDashedUnderlineWidth(dashWidth);
            update = true;
          }

          if(dashGapDefined && (fabsf(controller->GetDashedUnderlineGap() - dashGap) > Math::MACHINE_EPSILON_1000))
          {
            controller->SetDashedUnderlineGap(dashGap);
            update = true;
          }
        }
        else
        {
          // Disable underline.
          if(controller->IsUnderlineEnabled())
          {
            controller->SetUnderlineEnabled(false);
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& underlineProperties = ToStdString(value);

        controller->SetInputUnderlineProperties(underlineProperties);
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetUnderlineProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const bool                  enabled   = controller->IsUnderlineEnabled();
        const Vector4&              color     = controller->GetUnderlineColor();
        const float                 height    = controller->GetUnderlineHeight();
        const Text::Underline::Type type      = controller->GetUnderlineType();
        const float                 dashWidth = controller->GetDashedUnderlineWidth();
        const float                 dashGap   = controller->GetDashedUnderlineGap();

        if(controller->IsUnderlineSetByString())
        {
          std::string       underlineProperties = "{\"enable\":";
          const std::string enabledStr          = enabled ? "true" : "false";
          underlineProperties += "\"" + enabledStr + "\",";

          std::string colorStr;
          Vector4ToColorString(color, colorStr);
          underlineProperties += "\"color\":\"" + colorStr + "\",";

          std::string heightStr;
          FloatToString(height, heightStr);
          underlineProperties += "\"height\":\"" + heightStr + "\",";

          std::string typeStr;
          typeStr = GetUnderlineTypeToString(type);
          underlineProperties += "\"type\":\"" + typeStr + "\",";

          std::string dashWidthStr;
          FloatToString(dashWidth, dashWidthStr);
          underlineProperties += "\"dashWidth\":\"" + dashWidthStr + "\",";

          std::string dashGapStr;
          FloatToString(dashGap, dashGapStr);
          underlineProperties += "\"dashGap\":\"" + dashGapStr + "\"}";

          value = ToPropertyValue(underlineProperties);
        }
        else
        {
          Property::Map map;

          map.Insert(ENABLE_KEY, enabled);
          map.Insert(COLOR_KEY, color);
          map.Insert(HEIGHT_KEY, height);
          map.Insert(TYPE_KEY, type);
          map.Insert(DASH_WIDTH_KEY, dashWidth);
          map.Insert(DASH_GAP_KEY, dashGap);

          value = map;
        }

        break;
      }
      case EffectStyle::INPUT:
      {
        value = ToPropertyValue(controller->GetInputUnderlineProperties());
        break;
      }
    }
  }
}

bool SetShadowProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool    colorDefined = false;
        Vector4 color;
        bool    offsetDefined = false;
        Vector2 offset;
        bool    blurRadiusDefined = false;
        float   blurRadius;

        bool empty = true;

        if(propertiesMap.Empty())
        {
          // Map empty so check if a string provided
          const std::string propertyString = ToStdString(value);

          Property::Map parsedStringMap;
          Text::ParsePropertyString(propertyString, parsedStringMap);

          empty = ParseShadowProperties(parsedStringMap, colorDefined, color, offsetDefined, offset, blurRadiusDefined,
                                        blurRadius);

          controller->ShadowSetByString(!empty);
        }
        else
        {
          empty = ParseShadowProperties(propertiesMap, colorDefined, color, offsetDefined, offset, blurRadiusDefined,
                                        blurRadius);

          controller->ShadowSetByString(false);
        }

        if(!empty)
        {
          if(!controller->IsShadowEnabled())
          {
            controller->SetShadowEnabled(true);
            update = true;
          }

          // Sets the default shadow values.
          if(colorDefined && (controller->GetShadowColor() != color))
          {
            controller->SetShadowColor(color);
            update = true;
          }

          if(offsetDefined && (controller->GetShadowOffset() != offset))
          {
            controller->SetShadowOffset(offset);
            update = true;
          }

          if(blurRadiusDefined && (!Dali::Equals(controller->GetShadowBlurRadius(), blurRadius)))
          {
            controller->SetShadowBlurRadius(blurRadius);
            update = true;
          }
        }
        else
        {
          if(controller->IsShadowEnabled())
          {
            controller->SetShadowEnabled(false);
            update = true;
          }
          if(Vector2::ZERO != controller->GetShadowOffset())
          {
            controller->SetShadowOffset(Vector2::ZERO);
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& shadowString = ToStdString(value);

        controller->SetInputShadowProperties(shadowString);
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetShadowProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Vector4& color      = controller->GetShadowColor();
        const Vector2& offset     = controller->GetShadowOffset();
        const float&   blurRadius = controller->GetShadowBlurRadius();

        if(controller->IsShadowSetByString())
        {
          std::string shadowProperties = "{";

          std::string colorStr;
          Vector4ToColorString(color, colorStr);
          shadowProperties += "\"color\":\"" + colorStr + "\",";

          std::string offsetStr;
          Vector2ToString(offset, offsetStr);
          shadowProperties += "\"offset\":\"" + offsetStr + "\",";

          std::string blurRadiusStr;
          FloatToString(blurRadius, blurRadiusStr);
          shadowProperties += "\"blurRadius\":\"" + blurRadiusStr + "\"}";

          value = ToPropertyValue(shadowProperties);
        }
        else
        {
          Property::Map map;

          map.Insert(COLOR_KEY, color);
          map.Insert(OFFSET_KEY, offset);
          map.Insert(BLUR_RADIUS_KEY, blurRadius);

          value = map;
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        value = ToPropertyValue(controller->GetInputShadowProperties());
        break;
      }
    }
  }
}

bool SetEmbossProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        if(!propertiesMap.Empty())
        {
          bool    enabled          = false;
          bool    directionDefined = false;
          Vector2 direction;
          bool    strengthDefined   = false;
          float   strength          = 0.f;
          bool    lightColorDefined = false;
          Vector4 lightColor;
          bool    shadowColorDefined = false;
          Vector4 shadowColor;

          bool empty = ParseEmbossProperties(propertiesMap, enabled, directionDefined, direction, strengthDefined,
                                             strength, lightColorDefined, lightColor, shadowColorDefined, shadowColor);

          if(!empty)
          {
            if(enabled != controller->IsEmbossEnabled())
            {
              controller->SetEmbossEnabled(enabled);
              update = true;
            }

            // Sets the default emboss values.
            if(directionDefined && (controller->GetEmbossDirection() != direction))
            {
              controller->SetEmbossDirection(direction);
              update = true;
            }

            if(strengthDefined && (fabsf(controller->GetEmbossStrength() - strength) > Math::MACHINE_EPSILON_1000))
            {
              controller->SetEmbossStrength(strength);
              update = true;
            }

            if(lightColorDefined && (controller->GetEmbossLightColor() != lightColor))
            {
              controller->SetEmbossLightColor(lightColor);
              update = true;
            }

            if(shadowColorDefined && (controller->GetEmbossShadowColor() != shadowColor))
            {
              controller->SetEmbossShadowColor(shadowColor);
              update = true;
            }
          }
          else
          {
            // Disable emboss.
            if(controller->IsEmbossEnabled())
            {
              controller->SetEmbossEnabled(false);
              update = true;
            }
          }

          // Note : This code is for keep legacy logics, which is not using Property::Map.
          controller->SetDefaultEmbossProperties(EMPTY_STRING);
        }
        else
        {
          // Disable emboss.
          if(controller->IsEmbossEnabled())
          {
            controller->SetEmbossEnabled(false);
            update = true;
          }

          // Note : This code is for keep legacy logics, which is not using Property::Map.
          const std::string embossString = ToStdString(value);

          controller->SetDefaultEmbossProperties(embossString);
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& embossString = ToStdString(value);

        controller->SetInputEmbossProperties(embossString);
        break;
      }
    }
  }

  return update;
}

void GetEmbossProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        // Note : This code is for keep legacy logics, which is not using Property::Map.
        const std::string embossString = controller->GetDefaultEmbossProperties();
        if(embossString.empty())
        {
          const bool     enabled     = controller->IsEmbossEnabled();
          const Vector2& direction   = controller->GetEmbossDirection();
          const float    strength    = controller->GetEmbossStrength();
          const Vector4& lightColor  = controller->GetEmbossLightColor();
          const Vector4& shadowColor = controller->GetEmbossShadowColor();

          Property::Map map;

          map.Insert(ENABLE_KEY, enabled);
          map.Insert(DIRECTION_KEY, direction);
          map.Insert(STRENGTH_KEY, strength);
          map.Insert(LIGHT_COLOR_KEY, lightColor);
          map.Insert(SHADOW_COLOR_KEY, shadowColor);

          value = map;
        }
        else
        {
          value = ToPropertyValue(embossString);
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        value = ToPropertyValue(controller->GetInputEmbossProperties());
        break;
      }
    }
  }
}

bool SetOutlineProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool     colorDefined = false;
        Vector4  color;
        bool     widthDefined  = false;
        uint16_t width         = 0u;
        bool     offsetDefined = false;
        Vector2  offset;
        bool     blurRadiusDefined = false;
        float    blurRadius;

        bool empty = true;

        if(propertiesMap.Empty())
        {
          // Map empty so check if a string provided
          // This is purely to maintain backward compatibility, but we don't parse the string to be a property map.
          const std::string propertyString = ToStdString(value);

          // Stores the default outline's properties string to be recovered by the GetOutlineProperties() function.
          controller->SetDefaultOutlineProperties(propertyString);

          controller->OutlineSetByString(true);
        }
        else
        {
          empty = ParseOutlineProperties(propertiesMap, colorDefined, color, widthDefined, width, offsetDefined, offset,
                                         blurRadiusDefined, blurRadius);

          controller->OutlineSetByString(false);
        }

        if(!empty)
        {
          if(!controller->IsOutlineEnabled())
          {
            controller->SetOutlineEnabled(true);
            update = true;
          }

          // Sets the default outline values.
          if(colorDefined && (controller->GetOutlineColor() != color))
          {
            controller->SetOutlineColor(color);
            update = true;
          }

          if(widthDefined && (controller->GetOutlineWidth() != width))
          {
            controller->SetOutlineWidth(width);
            update = true;
          }

          if(offsetDefined && (controller->GetOutlineOffset() != offset))
          {
            controller->SetOutlineOffset(offset);
            update = true;
          }

          if(blurRadiusDefined && (!Dali::Equals(controller->GetOutlineBlurRadius(), blurRadius)))
          {
            controller->SetOutlineBlurRadius(blurRadius);
            update = true;
          }
        }
        else
        {
          if(controller->IsOutlineEnabled())
          {
            controller->SetOutlineEnabled(false);
            update = true;
          }
          if(0u != controller->GetOutlineWidth())
          {
            controller->SetOutlineWidth(0u);
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& outlineProperties = ToStdString(value);

        controller->SetInputOutlineProperties(outlineProperties);
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetOutlineProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        if(controller->IsOutlineSetByString())
        {
          value = ToPropertyValue(controller->GetDefaultOutlineProperties());
          break;
        }
        else
        {
          const Vector4& color      = controller->GetOutlineColor();
          const uint16_t width      = controller->GetOutlineWidth();
          const Vector2& offset     = controller->GetOutlineOffset();
          const float&   blurRadius = controller->GetOutlineBlurRadius();

          Property::Map map;
          map.Insert(COLOR_KEY, color);
          map.Insert(WIDTH_KEY, static_cast<int>(width));
          map.Insert(OFFSET_KEY, offset);
          map.Insert(BLUR_RADIUS_KEY, blurRadius);

          value = map;

          break;
        }
      }
      case EffectStyle::INPUT:
      {
        value = ToPropertyValue(controller->GetInputOutlineProperties());
        break;
      }
    }
  }
}

bool SetBackgroundProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool    enabled      = false;
        bool    colorDefined = false;
        Vector4 color;

        bool empty = true;

        if(!propertiesMap.Empty())
        {
          empty = ParseBackgroundProperties(propertiesMap, enabled, colorDefined, color);
        }

        if(!empty)
        {
          if(enabled != controller->IsBackgroundEnabled())
          {
            controller->SetBackgroundEnabled(enabled);
            update = true;
          }

          if(colorDefined && (controller->GetBackgroundColor() != color))
          {
            controller->SetBackgroundColor(color);
            update = true;
          }
        }
        else
        {
          // Disable background.
          if(controller->IsBackgroundEnabled())
          {
            controller->SetBackgroundEnabled(false);
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        // Text background is not supported while inputting yet
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetBackgroundProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const bool     enabled = controller->IsBackgroundEnabled();
        const Vector4& color   = controller->GetBackgroundColor();

        Property::Map map;
        map.Insert(ENABLE_KEY, enabled);
        map.Insert(COLOR_KEY, color);

        value = map;

        break;
      }
      case EffectStyle::INPUT:
      {
        // Text background is not supported while inputting yet
        break;
      }
    }
  }
}

bool SetStrikethroughProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool    enabled      = false;
        bool    colorDefined = false;
        Vector4 color;
        bool    heightDefined = false;
        float   height        = 0.f;

        bool empty = true;

        if(propertiesMap.Empty())
        {
          // Map empty so check if a string provided
          const std::string propertyString = ToStdString(value);

          if(!propertyString.empty())
          {
            Property::Map parsedStringMap;
            Text::ParsePropertyString(propertyString, parsedStringMap);

            empty = ParseStrikethroughProperties(parsedStringMap, enabled, colorDefined, color, heightDefined, height);

            controller->StrikethroughSetByString(!empty);
          }
        }
        else
        {
          empty = ParseStrikethroughProperties(propertiesMap, enabled, colorDefined, color, heightDefined, height);

          controller->StrikethroughSetByString(false);
        }

        if(!empty)
        {
          if(enabled != controller->IsStrikethroughEnabled())
          {
            controller->SetStrikethroughEnabled(enabled);
            update = true;
          }

          // Sets the default strikethrough values.
          if(colorDefined && (controller->GetStrikethroughColor() != color))
          {
            controller->SetStrikethroughColor(color);
            update = true;
          }
          if(heightDefined && (fabsf(controller->GetStrikethroughHeight() - height) > Math::MACHINE_EPSILON_1000))
          {
            controller->SetStrikethroughHeight(height);
            update = true;
          }
        }
        else
        {
          // Disable strikethrough.
          if(controller->IsStrikethroughEnabled())
          {
            controller->SetStrikethroughEnabled(false);
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& strikethroughProperties = ToStdString(value);

        controller->SetInputStrikethroughProperties(strikethroughProperties);
        update = true;
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetStrikethroughProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const bool     enabled = controller->IsStrikethroughEnabled();
        const Vector4& color   = controller->GetStrikethroughColor();
        const float    height  = controller->GetStrikethroughHeight();

        if(controller->IsStrikethroughSetByString())
        {
          std::string       strikethroughProperties = "{\"enable\":";
          const std::string enabledStr              = enabled ? "true" : "false";
          strikethroughProperties += "\"" + enabledStr + "\",";

          std::string colorStr;
          Vector4ToColorString(color, colorStr);
          strikethroughProperties += "\"color\":\"" + colorStr + "\",";

          std::string heightStr;
          FloatToString(height, heightStr);
          strikethroughProperties += "\"height\":\"" + heightStr + "\"}";

          value = ToPropertyValue(strikethroughProperties);
        }
        else
        {
          Property::Map map;

          map.Insert(ENABLE_KEY, enabled);
          map.Insert(COLOR_KEY, color);
          map.Insert(HEIGHT_KEY, height);

          value = map;
        }

        break;
      }
      case EffectStyle::INPUT:
      {
        value = ToPropertyValue(controller->GetInputStrikethroughProperties());
        break;
      }
    }
  }
}

Underline::Type StringToUnderlineType(const char* const underlineTypeStr)
{
  Underline::Type underlineType = Text::Underline::Type::SOLID;
  Scripting::GetEnumeration<Underline::Type>(underlineTypeStr, UNDERLINE_TYPE_STRING_TABLE,
                                             UNDERLINE_TYPE_STRING_TABLE_COUNT, underlineType);

  return underlineType;
}

} // namespace Text

} // namespace Ui

} // namespace Dali
