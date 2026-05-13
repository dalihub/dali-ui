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

#include <dali-ui-foundation/public-api/trait-object.h>
#include <memory>

namespace Dali
{

namespace Ui
{

class LayoutManager;

namespace Internal
{

// LayoutManagerObject
// ============================================================================

/**
 * @brief Internal object that holds a LayoutManager.
 *
 * This object is stored in a Layout view trait slot to avoid per-instance
 * storage overhead on View.
 */
class LayoutManagerObject : public TraitObject
{
public:
  explicit LayoutManagerObject(LayoutManager* layoutManager);

  LayoutManager* GetLayoutManager() const;

protected:
  ~LayoutManagerObject() override;

private:
  std::unique_ptr<LayoutManager> mLayoutManager;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
