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
#include <dali/public-api/common/dali-string.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/types/callback.h>

namespace Dali
{
namespace Ui
{

class ViewImpl;

namespace Internal
{
namespace ViewGradientColorBinding
{

bool HasTokenColor(const Gradient::Base& gradient);

/**
 * @brief Adds gradient binding data.
 * @return True when the first binding data was added and the caller should connect the color table change signal.
 */
bool Add(ViewImpl& viewImpl, StringView bindingId, const Gradient::Base& gradient, Callback<void(const Gradient::Base&)> callback);

bool Update(ViewImpl& viewImpl, StringView bindingId, const Gradient::Base& gradient);
void ApplyAll(ViewImpl& viewImpl);

/**
 * @brief Clears gradient binding data for the given binding ID.
 * @return True when the last binding data was removed and the caller should disconnect the color table change signal.
 */
bool Clear(ViewImpl& viewImpl, StringView bindingId);

} // namespace ViewGradientColorBinding
} // namespace Internal
} // namespace Ui
} // namespace Dali
