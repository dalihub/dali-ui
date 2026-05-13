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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/trait-object.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

// LayoutCallbacksObject
// ============================================================================

/**
 * @brief Internal object that stores layout callback payloads.
 *
 * Stores measure/arrange callbacks as typed Callback objects.
 */
class LayoutCallbacksObject : public TraitObject
{
public:
  LayoutCallbacksObject();

  void SetMeasureCallback(MeasureCallback callback);
  void SetArrangeCallback(ArrangeCallback callback);

  MeasureCallback* GetMeasureCallback();
  ArrangeCallback* GetArrangeCallback();

protected:
  ~LayoutCallbacksObject() override;

private:
  MeasureCallback mOnMeasure;
  ArrangeCallback mOnArrange;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
