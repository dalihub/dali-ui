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

// CLASS HEADER
#include <dali-ui-components/public-api/styles/components-style-sheet.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/styles/markdown-view-style.h>

namespace Dali
{
namespace Ui
{
namespace Components
{
namespace StyleSheet
{
namespace
{

UiStyle CreateDefaultMarkdownViewStyle()
{
  return MarkdownViewStyle::DefaultPreset();
}

} // namespace

UiStyleSheet New()
{
  UiStyleSheet styleSheet = UiStyleSheet::New();
  styleSheet.SetStyle(MarkdownViewStyle::DefaultKey(), &CreateDefaultMarkdownViewStyle);
  return styleSheet;
}

} // namespace StyleSheet
} // namespace Components
} // namespace Ui
} // namespace Dali
