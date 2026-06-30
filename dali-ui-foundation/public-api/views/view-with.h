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
#include <dali/public-api/common/type-traits.h>

/**
 * @brief Declares a typed extension hook for View-derived public handle classes.
 *
 * The hook invokes @p action with this instance as the first argument and forwards
 * any extra arguments after it. The return type of @p action is propagated to the
 * caller. This does not return @c *this for fluent chaining.
 */
#define DALI_UI_VIEW_WITH(SelfType)                                \
  template<typename F, typename... Args>                           \
  decltype(auto) With(F&& action, Args&&... args)                  \
  {                                                                \
    return Dali::Forward<F>(action)(static_cast<SelfType&>(*this), \
                                    Dali::Forward<Args>(args)...); \
  }
