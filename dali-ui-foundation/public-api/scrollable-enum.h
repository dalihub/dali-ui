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

#pragma once

namespace Dali
{

namespace Ui
{

/**
 * @brief Scroll direction enumeration.
 */
enum class ScrollDirection
{
  Vertical,   ///< Vertical scrolling only
  Horizontal, ///< Horizontal scrolling only
  Both        ///< Both horizontal and vertical scrolling
};

/**
 * @brief Scroll bar visibility enumeration.
 */
enum class ScrollBarVisibility
{
  Auto,   ///< Show scroll bar only when scrolling
  Always, ///< Always show scroll bar
  Never   ///< Never show scroll bar
};

/**
 * @brief Scroll to position enumeration.
 */
enum class ScrollToPosition
{
  Start,      ///< Scroll to start of view
  Center,     ///< Scroll to center of view
  End,        ///< Scroll to end of view
  MakeVisible ///< Scroll to make the view visible
};

/**
 * @brief Over scroll mode enumeration.
 */
enum class OverScrollMode
{
  Never,         ///< Never allow over-scrolling
  Always,        ///< Always allow over-scrolling
  ContentScrolls ///< Allow over-scrolling only when content is scrollable
};

} // namespace Ui

} // namespace Dali
