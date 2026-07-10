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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class TextPixelSnapData
{
public:
  Property::Index EnsurePixelSnapFactorProperty(View owner);

  Property::Index GetPixelSnapFactorPropertyIndex() const;

private:
  Property::Index mPixelSnapFactorPropertyIndex{Property::INVALID_INDEX};
};

Property::Index EnsureTextPixelSnapFactorProperty(View owner);

Property::Index GetTextPixelSnapFactorPropertyIndex(View owner);

} // namespace Internal
} // namespace Ui
} // namespace Dali
