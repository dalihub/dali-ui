#ifndef DALI_UI_INTERNAL_TEXT_FONT_VARIATION_PROPERTY_DATA_H
#define DALI_UI_INTERNAL_TEXT_FONT_VARIATION_PROPERTY_DATA_H

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/property.h>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

class FontVariationPropertyData
{
public:
  bool Insert(Dali::Property::Index index, const Dali::String& tag);

  bool Find(Dali::Property::Index index, Dali::String& tag) const;

  void ApplyCurrentPropertyValues(Dali::Actor actor, Dali::Property::Map& map) const;

private:
  std::unordered_map<Dali::Property::Index, Dali::String> mVariationIndexMap;
};

FontVariationPropertyData* GetFontVariationPropertyData(Dali::Ui::View owner);

FontVariationPropertyData& GetOrCreateFontVariationPropertyData(Dali::Ui::View owner);

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_TEXT_FONT_VARIATION_PROPERTY_DATA_H
