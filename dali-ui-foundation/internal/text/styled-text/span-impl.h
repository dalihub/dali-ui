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
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

/**
 * @brief Base implementation for styled text span payload objects.
 */
class Span : public BaseObject
{
public:
  Span();
  ~Span() override;

private:
  Span(const Span&)            = delete;
  Span(Span&&)                 = delete;
  Span& operator=(const Span&) = delete;
  Span& operator=(Span&&)      = delete;
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
