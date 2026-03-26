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
#include <dali-ui-foundation/internal/controls/text-controls/text-anchor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/text-anchor-impl.h>

namespace Dali
{
namespace Ui
{
TextAnchor TextAnchor::New()
{
  Internal::TextAnchorImplPtr impl = Internal::TextAnchorImpl::New();

  TextAnchor textAnchor = TextAnchor(*impl);

  // Second-phase initialization
  impl->Initialize();
  return textAnchor;
}

TextAnchor::TextAnchor()
{
}

TextAnchor::TextAnchor(const TextAnchor& handle) = default;

TextAnchor& TextAnchor::operator=(const TextAnchor& handle) = default;

TextAnchor::TextAnchor(TextAnchor&& handle) = default;

TextAnchor& TextAnchor::operator=(TextAnchor&& handle) = default;

TextAnchor::~TextAnchor()
{
}

TextAnchor TextAnchor::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<TextAnchor, Internal::TextAnchorImpl>(handle);
}

TextAnchor::TextAnchor(Internal::TextAnchorImpl& implementation)
: View(implementation)
{
}

TextAnchor::TextAnchor(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Internal::TextAnchorImpl>(internal);
}

} // namespace Ui

} // namespace Dali
