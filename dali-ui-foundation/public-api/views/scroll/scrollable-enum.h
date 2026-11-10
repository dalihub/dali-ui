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
#include <dali/public-api/common/dali-namespace.h>

#pragma once

namespace DALI_NAMESPACE
{

namespace Ui
{

/**
 * @brief Scroll direction enumeration.
 * @SINCE_3_0.0
 */
enum class ScrollDirection
{
  Vertical,   ///< Vertical scrolling only @SINCE_3_0.0
  Horizontal, ///< Horizontal scrolling only @SINCE_3_0.0
  Both        ///< Both horizontal and vertical scrolling @SINCE_3_0.0
};

/**
 * @brief Scroll bar visibility enumeration.
 * @SINCE_3_0.0
 */
enum class ScrollBarVisibility
{
  Auto,   ///< Show scroll bar only when scrolling @SINCE_3_0.0
  Always, ///< Always show scroll bar @SINCE_3_0.0
  Never   ///< Never show scroll bar @SINCE_3_0.0
};

/**
 * @brief Scroll to position enumeration.
 * @SINCE_3_0.0
 */
enum class ScrollToPosition
{
  Start,      ///< Scroll to start of view @SINCE_3_0.0
  Center,     ///< Scroll to center of view @SINCE_3_0.0
  End,        ///< Scroll to end of view @SINCE_3_0.0
  MakeVisible ///< Scroll to make the view visible @SINCE_3_0.0
};

/**
 * @brief Over scroll mode enumeration.
 * @SINCE_3_0.0
 */
enum class OverScrollMode
{
  Never,         ///< Never allow over-scrolling @SINCE_3_0.0
  Always,        ///< Always allow over-scrolling @SINCE_3_0.0
  ContentScrolls ///< Allow over-scrolling only when content is scrollable @SINCE_3_0.0
};

} // namespace Ui

} //namespace DALI_NAMESPACE
