#pragma once

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
#include <dali/public-api/object/property.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-atlas-gradient-state.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

struct EditableTextGradientPropertyData
{
  Ui::Text::Internal::Gradient::EditableAtlasResources atlasResources;

  Property::Index gradientAnimOffsetIndex{Property::INVALID_INDEX};
  Property::Index placeholderGradientAnimOffsetIndex{Property::INVALID_INDEX};

  int gradientAnimCount{0};
  int placeholderGradientAnimCount{0};
};

using EditableTextGradientPropertyDataPtr = std::unique_ptr<EditableTextGradientPropertyData>;

inline EditableTextGradientPropertyData* GetEditableTextGradientPropertyData(EditableTextGradientPropertyDataPtr& data)
{
  return data.get();
}

inline const EditableTextGradientPropertyData* GetEditableTextGradientPropertyData(const EditableTextGradientPropertyDataPtr& data)
{
  return data.get();
}

EditableTextGradientPropertyData& GetOrCreateEditableTextGradientPropertyData(EditableTextGradientPropertyDataPtr& data);

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
