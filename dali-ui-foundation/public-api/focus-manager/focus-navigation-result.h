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
 */

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
class View;

namespace Internal
{
struct FocusNavigationResultImpl;
} //namespace Internal

/**
 * @brief Describes how a focus-navigation policy handled a request.
 */
enum class FocusNavigationResultType : uint8_t
{
  NOT_HANDLED, ///< Continue with the next navigation policy.
  MOVE,        ///< Resolve and move focus to the supplied candidate.
  STAY,        ///< Stop navigation and keep the current focus unchanged.
};

/**
 * @brief Borrowed result written by a custom focus-navigation policy.
 *
 * The framework supplies a result initialized to NOT_HANDLED. Leaving it unchanged
 * delegates to the next policy. Mutators record a decision; they do not move focus.
 * The last decision written by the policy takes effect.
 *
 * @note Valid only during the policy invocation. Do not retain pointers or
 * references to this object, including in asynchronous work.
 * @note Applications must use the framework-supplied result. Copying and moving
 * are not supported.
 */
class DALI_UI_API FocusNavigationResult
{
public:
  FocusNavigationResult(const FocusNavigationResult&)            = delete;
  FocusNavigationResult& operator=(const FocusNavigationResult&) = delete;
  FocusNavigationResult(FocusNavigationResult&&)                 = delete;
  FocusNavigationResult& operator=(FocusNavigationResult&&)      = delete;

  /**
   * @brief Delegates to the next policy and clears any previously selected candidate.
   */
  void NotHandled();

  /**
   * @brief Selects a focus candidate without immediately moving focus.
   *
   * An empty or otherwise invalid candidate stops fallback processing and causes
   * the navigation request to fail. The candidate is retained until overwritten
   * or until the framework finishes processing the result.
   *
   * @param[in] candidate The View to resolve and focus
   */
  void MoveTo(View candidate);

  /**
   * @brief Consumes the request without moving focus and clears the candidate.
   */
  void Stay();

  /**
   * @brief Gets the result type.
   */
  FocusNavigationResultType GetType() const;

  /**
   * @brief Gets the candidate supplied to MoveTo().
   *
   * @return The candidate, or an empty View for other result types
   */
  View GetCandidate() const;

  /// @cond internal
  /**
   * @brief Connects the result to framework-owned storage.
   * @param[in] impl Internal storage that must outlive this result
   */
  DALI_INTERNAL explicit FocusNavigationResult(Internal::FocusNavigationResultImpl& impl);
  /// @endcond

private:
  Internal::FocusNavigationResultImpl* mImpl;
};

} // namespace Ui
} //namespace DALI_NAMESPACE
