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
 */

// CLASS HEADER
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-result.h>

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/focus-manager/focus-navigation-result-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

FocusNavigationResult::FocusNavigationResult(Internal::FocusNavigationResultImpl& impl)
: mImpl(&impl)
{
}

void FocusNavigationResult::NotHandled()
{
  mImpl->type = FocusNavigationResultType::NOT_HANDLED;
  mImpl->candidate.Reset();
}

void FocusNavigationResult::MoveTo(View candidate)
{
  mImpl->type      = FocusNavigationResultType::MOVE;
  mImpl->candidate = std::move(candidate);
}

void FocusNavigationResult::Stay()
{
  mImpl->type = FocusNavigationResultType::STAY;
  mImpl->candidate.Reset();
}

FocusNavigationResultType FocusNavigationResult::GetType() const
{
  return mImpl->type;
}

View FocusNavigationResult::GetCandidate() const
{
  return mImpl->candidate;
}

} // namespace Ui
} // namespace DALI_NAMESPACE
