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

// FILE HEADER
#include <dali-ui-foundation/internal/text/text-font-style.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-ui-foundation/internal/text/property-string-parser.h>

using Dali::Integration::ToDaliString;
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
const char* WEIGHT_KEY = "weight";
const char* WIDTH_KEY  = "width";
const char* SLANT_KEY  = "slant";
const char* FAMILY_KEY = "family";
const char* TYPE_KEY   = "type";

const char* SYSTEM_TOKEN = "system";

} // namespace

const char* GetFontWeightName(FontWeight weight)
{
  return GetEnumerationName(static_cast<int>(weight), FONT_WEIGHT_STRING_TABLE, FONT_WEIGHT_STRING_TABLE_COUNT);
}

const char* GetFontWidthName(FontWidth width)
{
  return GetEnumerationName(static_cast<int>(width), FONT_WIDTH_STRING_TABLE, FONT_WIDTH_STRING_TABLE_COUNT);
}

const char* GetFontSlantName(FontSlant slant)
{
  return GetEnumerationName(static_cast<int>(slant), FONT_SLANT_STRING_TABLE, FONT_SLANT_STRING_TABLE_COUNT);
}

TextAbstraction::FontWidth::Type ToTextAbstractionFontWidth(FontWidth width)
{
  switch(width)
  {
    case FontWidth::ULTRA_CONDENSED:
    {
      return TextAbstraction::FontWidth::ULTRA_CONDENSED;
    }
    case FontWidth::EXTRA_CONDENSED:
    {
      return TextAbstraction::FontWidth::EXTRA_CONDENSED;
    }
    case FontWidth::CONDENSED:
    {
      return TextAbstraction::FontWidth::CONDENSED;
    }
    case FontWidth::SEMI_CONDENSED:
    {
      return TextAbstraction::FontWidth::SEMI_CONDENSED;
    }
    case FontWidth::NORMAL:
    {
      return TextAbstraction::FontWidth::NORMAL;
    }
    case FontWidth::SEMI_EXPANDED:
    {
      return TextAbstraction::FontWidth::SEMI_EXPANDED;
    }
    case FontWidth::EXPANDED:
    {
      return TextAbstraction::FontWidth::EXPANDED;
    }
    case FontWidth::EXTRA_EXPANDED:
    {
      return TextAbstraction::FontWidth::EXTRA_EXPANDED;
    }
    case FontWidth::ULTRA_EXPANDED:
    {
      return TextAbstraction::FontWidth::ULTRA_EXPANDED;
    }
  }

  return TextAbstraction::FontWidth::NORMAL;
}

FontWidth ToFontWidth(TextAbstraction::FontWidth::Type width)
{
  switch(width)
  {
    case TextAbstraction::FontWidth::ULTRA_CONDENSED:
    {
      return FontWidth::ULTRA_CONDENSED;
    }
    case TextAbstraction::FontWidth::EXTRA_CONDENSED:
    {
      return FontWidth::EXTRA_CONDENSED;
    }
    case TextAbstraction::FontWidth::CONDENSED:
    {
      return FontWidth::CONDENSED;
    }
    case TextAbstraction::FontWidth::SEMI_CONDENSED:
    {
      return FontWidth::SEMI_CONDENSED;
    }
    case TextAbstraction::FontWidth::NORMAL:
    case TextAbstraction::FontWidth::NONE:
    {
      return FontWidth::NORMAL;
    }
    case TextAbstraction::FontWidth::SEMI_EXPANDED:
    {
      return FontWidth::SEMI_EXPANDED;
    }
    case TextAbstraction::FontWidth::EXPANDED:
    {
      return FontWidth::EXPANDED;
    }
    case TextAbstraction::FontWidth::EXTRA_EXPANDED:
    {
      return FontWidth::EXTRA_EXPANDED;
    }
    case TextAbstraction::FontWidth::ULTRA_EXPANDED:
    {
      return FontWidth::ULTRA_EXPANDED;
    }
  }

  return FontWidth::NORMAL;
}

TextAbstraction::FontWeight::Type ToTextAbstractionFontWeight(FontWeight weight)
{
  switch(weight)
  {
    case FontWeight::THIN:
    {
      return TextAbstraction::FontWeight::THIN;
    }
    case FontWeight::EXTRA_LIGHT:
    {
      return TextAbstraction::FontWeight::ULTRA_LIGHT;
    }
    case FontWeight::LIGHT:
    {
      return TextAbstraction::FontWeight::LIGHT;
    }
    case FontWeight::DEMI_LIGHT:
    {
      return TextAbstraction::FontWeight::DEMI_LIGHT;
    }
    case FontWeight::BOOK:
    {
      return TextAbstraction::FontWeight::BOOK;
    }
    case FontWeight::NORMAL:
    {
      return TextAbstraction::FontWeight::NORMAL;
    }
    case FontWeight::MEDIUM:
    {
      return TextAbstraction::FontWeight::MEDIUM;
    }
    case FontWeight::SEMI_BOLD:
    {
      return TextAbstraction::FontWeight::DEMI_BOLD;
    }
    case FontWeight::BOLD:
    {
      return TextAbstraction::FontWeight::BOLD;
    }
    case FontWeight::EXTRA_BOLD:
    {
      return TextAbstraction::FontWeight::ULTRA_BOLD;
    }
    case FontWeight::BLACK:
    {
      return TextAbstraction::FontWeight::BLACK;
    }
  }

  return TextAbstraction::FontWeight::NORMAL;
}

FontWeight ToFontWeight(TextAbstraction::FontWeight::Type weight)
{
  switch(weight)
  {
    case TextAbstraction::FontWeight::THIN:
    {
      return FontWeight::THIN;
    }
    case TextAbstraction::FontWeight::ULTRA_LIGHT:
    {
      return FontWeight::EXTRA_LIGHT;
    }
    case TextAbstraction::FontWeight::LIGHT:
    {
      return FontWeight::LIGHT;
    }
    case TextAbstraction::FontWeight::DEMI_LIGHT:
    {
      return FontWeight::DEMI_LIGHT;
    }
    case TextAbstraction::FontWeight::BOOK:
    {
      return FontWeight::BOOK;
    }
    case TextAbstraction::FontWeight::NORMAL:
    case TextAbstraction::FontWeight::NONE:
    {
      return FontWeight::NORMAL;
    }
    case TextAbstraction::FontWeight::MEDIUM:
    {
      return FontWeight::MEDIUM;
    }
    case TextAbstraction::FontWeight::DEMI_BOLD:
    {
      return FontWeight::SEMI_BOLD;
    }
    case TextAbstraction::FontWeight::BOLD:
    {
      return FontWeight::BOLD;
    }
    case TextAbstraction::FontWeight::ULTRA_BOLD:
    {
      return FontWeight::EXTRA_BOLD;
    }
    case TextAbstraction::FontWeight::BLACK:
    {
      return FontWeight::BLACK;
    }
  }

  return FontWeight::NORMAL;
}

TextAbstraction::FontSlant::Type ToTextAbstractionFontSlant(FontSlant slant)
{
  switch(slant)
  {
    case FontSlant::NORMAL:
    {
      return TextAbstraction::FontSlant::NORMAL;
    }
    case FontSlant::ITALIC:
    {
      return TextAbstraction::FontSlant::ITALIC;
    }
    case FontSlant::OBLIQUE:
    {
      return TextAbstraction::FontSlant::OBLIQUE;
    }
  }

  return TextAbstraction::FontSlant::NORMAL;
}

FontSlant ToFontSlant(TextAbstraction::FontSlant::Type slant)
{
  switch(slant)
  {
    case TextAbstraction::FontSlant::NORMAL:
    case TextAbstraction::FontSlant::NONE:
    {
      return FontSlant::NORMAL;
    }
    case TextAbstraction::FontSlant::ITALIC:
    {
      return FontSlant::ITALIC;
    }
    case TextAbstraction::FontSlant::OBLIQUE:
    {
      return FontSlant::OBLIQUE;
    }
  }

  return FontSlant::NORMAL;
}

void SetFontFamilyProperty(ControllerPtr controller, const Property::Value& value)
{
  if(controller)
  {
    const std::string fontFamilyValue = ToStdString(value);

    if(fontFamilyValue.empty())
    {
      // Resets the default's font family name.
      controller->SetDefaultFontFamily("");
      return;
    }

    Property::Map map;
    ParsePropertyString(fontFamilyValue, map);

    if(map.Empty())
    {
      // There is no map. The font has been passed as a font's family name with no format.
      controller->SetDefaultFontFamily(fontFamilyValue);
    }
    else
    {
      Property::Value* familyValue = map.Find(FAMILY_KEY);

      std::string fontFamilyName;
      if(NULL != familyValue)
      {
        fontFamilyName = ToStdString(*familyValue);
      }

      Property::Value* typeValue = map.Find(TYPE_KEY);

      std::string typeStr;
      if(NULL != typeValue)
      {
        typeStr = ToStdString(*typeValue);
      }

      if(TokenComparison(SYSTEM_TOKEN, typeStr.c_str(), typeStr.size()))
      {
        controller->UpdateAfterFontChange(fontFamilyName);
      }
      else
      {
        controller->SetDefaultFontFamily(fontFamilyName);
      }
    }
  }
}

void SetFontStyleProperty(ControllerPtr controller, const Property::Value& value, FontStyle::Type type)
{
  if(controller)
  {
    Property::Map map;
    if(Property::STRING == value.GetType())
    {
      const std::string& fontStyleProperties = ToStdString(value);

      ParsePropertyString(fontStyleProperties, map);
      controller->FontStyleSetByString(true);
    }
    else
    {
      map = value.Get<Property::Map>();
      controller->FontStyleSetByString(false);
    }

    if(!map.Empty())
    {
      Property::Value* weightValue = map.Find(WEIGHT_KEY);

      TextAbstraction::FontWeight::Type weight        = TextAbstraction::FontWeight::NONE;
      const bool                        weightDefined = weightValue != NULL;
      if(weightDefined)
      {
        const std::string weightStr = ToStdString(*weightValue);
        weight                      = ToTextAbstractionFontWeight(StringToWeight(weightStr.c_str()));
      }

      Property::Value* widthValue = map.Find(WIDTH_KEY);

      TextAbstraction::FontWidth::Type width        = TextAbstraction::FontWidth::NONE;
      const bool                       widthDefined = widthValue != NULL;
      if(widthDefined)
      {
        const std::string widthStr = ToStdString(*widthValue);
        width                      = ToTextAbstractionFontWidth(StringToWidth(widthStr.c_str()));
      }

      Property::Value* slantValue = map.Find(SLANT_KEY);

      TextAbstraction::FontSlant::Type slant        = TextAbstraction::FontSlant::NONE;
      const bool                       slantDefined = slantValue != NULL;
      if(slantDefined)
      {
        const std::string slantStr = ToStdString(*slantValue);
        slant                      = ToTextAbstractionFontSlant(StringToSlant(slantStr.c_str()));
      }

      switch(type)
      {
        case FontStyle::DEFAULT:
        {
          if(!weightDefined || (weightDefined && (controller->GetDefaultFontWeight() != weight)))
          {
            controller->SetDefaultFontWeight(weight);
          }

          if(!widthDefined || (widthDefined && (controller->GetDefaultFontWidth() != width)))
          {
            controller->SetDefaultFontWidth(width);
          }

          if(!slantDefined || (slantDefined && (controller->GetDefaultFontSlant() != slant)))
          {
            controller->SetDefaultFontSlant(slant);
          }
          break;
        }
        case FontStyle::INPUT:
        {
          if(!weightDefined || (weightDefined && (controller->GetInputFontWeight() != weight)))
          {
            controller->SetInputFontWeight(weight);
          }

          if(!widthDefined || (widthDefined && (controller->GetInputFontWidth() != width)))
          {
            controller->SetInputFontWidth(width);
          }

          if(!slantDefined || (slantDefined && (controller->GetInputFontSlant() != slant)))
          {
            controller->SetInputFontSlant(slant);
          }
          break;
        }
        case FontStyle::PLACEHOLDER:
        {
          if(!weightDefined || (weightDefined && (controller->GetPlaceholderTextFontWeight() != weight)))
          {
            controller->SetPlaceholderTextFontWeight(weight);
          }

          if(!widthDefined || (widthDefined && (controller->GetPlaceholderTextFontWidth() != width)))
          {
            controller->SetPlaceholderTextFontWidth(width);
          }

          if(!slantDefined || (slantDefined && (controller->GetPlaceholderTextFontSlant() != slant)))
          {
            controller->SetPlaceholderTextFontSlant(slant);
          }
          break;
        }
      }
    }
    else
    {
      switch(type)
      {
        case FontStyle::DEFAULT:
        {
          controller->SetDefaultFontWeight(TextAbstraction::FontWeight::NONE);
          controller->SetDefaultFontWidth(TextAbstraction::FontWidth::NONE);
          controller->SetDefaultFontSlant(TextAbstraction::FontSlant::NONE);
          break;
        }
        case FontStyle::INPUT:
        {
          controller->SetInputFontWeight(TextAbstraction::FontWeight::NONE);
          controller->SetInputFontWidth(TextAbstraction::FontWidth::NONE);
          controller->SetInputFontSlant(TextAbstraction::FontSlant::NONE);
          break;
        }
        case FontStyle::PLACEHOLDER:
        {
          controller->SetPlaceholderTextFontWeight(TextAbstraction::FontWeight::NONE);
          controller->SetPlaceholderTextFontWidth(TextAbstraction::FontWidth::NONE);
          controller->SetPlaceholderTextFontSlant(TextAbstraction::FontSlant::NONE);
          break;
        }
      }
    }
  }
}

void GetFontStyleProperty(ControllerPtr controller, Property::Value& value, FontStyle::Type type)
{
  if(controller)
  {
    const bool isSetbyString = controller->IsFontStyleSetByString();

    bool                              weightDefined = false;
    bool                              widthDefined  = false;
    bool                              slantDefined  = false;
    TextAbstraction::FontWeight::Type weight        = TextAbstraction::FontWeight::NONE;
    TextAbstraction::FontWidth::Type  width         = TextAbstraction::FontWidth::NONE;
    TextAbstraction::FontSlant::Type  slant         = TextAbstraction::FontSlant::NONE;

    switch(type)
    {
      case FontStyle::DEFAULT:
      {
        weightDefined = controller->IsDefaultFontWeightDefined();
        widthDefined  = controller->IsDefaultFontWidthDefined();
        slantDefined  = controller->IsDefaultFontSlantDefined();

        if(weightDefined)
        {
          weight = controller->GetDefaultFontWeight();
        }

        if(widthDefined)
        {
          width = controller->GetDefaultFontWidth();
        }

        if(slantDefined)
        {
          slant = controller->GetDefaultFontSlant();
        }
        break;
      }
      case FontStyle::INPUT:
      {
        weightDefined = controller->IsInputFontWeightDefined();
        widthDefined  = controller->IsInputFontWidthDefined();
        slantDefined  = controller->IsInputFontSlantDefined();

        if(weightDefined)
        {
          weight = controller->GetInputFontWeight();
        }

        if(widthDefined)
        {
          width = controller->GetInputFontWidth();
        }

        if(slantDefined)
        {
          slant = controller->GetInputFontSlant();
        }
        break;
      }
      case FontStyle::PLACEHOLDER:
      {
        weightDefined = controller->IsPlaceholderTextFontWeightDefined();
        widthDefined  = controller->IsPlaceholderTextFontWidthDefined();
        slantDefined  = controller->IsPlaceholderTextFontSlantDefined();

        if(weightDefined)
        {
          weight = controller->GetPlaceholderTextFontWeight();
        }

        if(widthDefined)
        {
          width = controller->GetPlaceholderTextFontWidth();
        }

        if(slantDefined)
        {
          slant = controller->GetPlaceholderTextFontSlant();
        }
        break;
      }
    }

    if(!isSetbyString)
    {
      Property::Map map;

      if(weightDefined && (TextAbstraction::FontWeight::NONE != weight))
      {
        const std::string weightStr(GetFontWeightName(ToFontWeight(weight)));
        map.Insert(WEIGHT_KEY, ToDaliString(weightStr));
      }

      if(widthDefined && (TextAbstraction::FontWidth::NONE != width))
      {
        const std::string widthStr(GetFontWidthName(ToFontWidth(width)));
        map.Insert(WIDTH_KEY, ToDaliString(widthStr));
      }

      if(slantDefined && (TextAbstraction::FontSlant::NONE != slant))
      {
        const std::string slantStr(GetFontSlantName(ToFontSlant(slant)));
        map.Insert(SLANT_KEY, ToDaliString(slantStr));
      }

      value = map;
    }
    else
    {
      std::string fontStyleProperties = "{";

      if(weightDefined && (TextAbstraction::FontWeight::NONE != weight))
      {
        const std::string weightStr(GetFontWeightName(ToFontWeight(weight)));
        fontStyleProperties += "\"weight\":\"" + weightStr + "\",";
      }

      if(widthDefined && (TextAbstraction::FontWidth::NONE != width))
      {
        const std::string widthStr(GetFontWidthName(ToFontWidth(width)));
        fontStyleProperties += "\"width\":\"" + widthStr + "\",";
      }

      if(slantDefined && (TextAbstraction::FontSlant::NONE != slant))
      {
        const std::string slantStr(GetFontSlantName(ToFontSlant(slant)));
        fontStyleProperties += "\"slant\":\"" + slantStr + "\"";
      }

      if((*fontStyleProperties.rbegin()) == ',')
      {
        fontStyleProperties = fontStyleProperties.substr(0, fontStyleProperties.size() - 1);
      }

      fontStyleProperties += "}";

      value = ToPropertyValue(fontStyleProperties);
    }
  }
}

FontWeight StringToWeight(const char* const weightStr)
{
  FontWeight weight = FontWeight::NORMAL;
  Scripting::GetEnumeration<FontWeight>(weightStr, FONT_WEIGHT_STRING_TABLE, FONT_WEIGHT_STRING_TABLE_COUNT, weight);

  return weight;
}

FontWidth StringToWidth(const char* const widthStr)
{
  FontWidth width = FontWidth::NORMAL;
  Scripting::GetEnumeration<FontWidth>(widthStr, FONT_WIDTH_STRING_TABLE, FONT_WIDTH_STRING_TABLE_COUNT, width);

  return width;
}

FontSlant StringToSlant(const char* const slantStr)
{
  FontSlant slant = FontSlant::NORMAL;
  Scripting::GetEnumeration<FontSlant>(slantStr, FONT_SLANT_STRING_TABLE, FONT_SLANT_STRING_TABLE_COUNT, slant);

  return slant;
}

bool GetFontWeightEnumeration(const Property::Value& propertyValue, FontWeight& fontWeight)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           FONT_WEIGHT_STRING_TABLE,
                                           FONT_WEIGHT_STRING_TABLE_COUNT,
                                           fontWeight);
}

bool GetFontWidthEnumeration(const Property::Value& propertyValue, FontWidth& fontWidth)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           FONT_WIDTH_STRING_TABLE,
                                           FONT_WIDTH_STRING_TABLE_COUNT,
                                           fontWidth);
}

bool GetFontSlantEnumeration(const Property::Value& propertyValue, FontSlant& fontSlant)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           FONT_SLANT_STRING_TABLE,
                                           FONT_SLANT_STRING_TABLE_COUNT,
                                           fontSlant);
}

} // namespace Text
} // namespace Ui
} // namespace Dali
