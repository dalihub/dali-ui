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
#include <dali/public-api/object/base-handle.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
class View;

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
 * @brief Result returned by a custom focus-navigation policy.
 */
class DALI_UI_API FocusNavigationResult
{
public:
  /**
   * @brief Creates a result equivalent to NotHandled().
   */
  FocusNavigationResult();

  /**
   * @brief Creates a result that delegates to the next policy.
   */
  static FocusNavigationResult NotHandled();

  /**
   * @brief Creates a result that selects a focus candidate.
   *
   * Returning an empty or otherwise invalid candidate stops fallback processing
   * and causes the navigation request to fail.
   *
   * @param[in] candidate The View to resolve and focus
   */
  static FocusNavigationResult MoveTo(View candidate);

  /**
   * @brief Creates a result that consumes the request without moving focus.
   */
  static FocusNavigationResult Stay();

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

private:
  FocusNavigationResult(FocusNavigationResultType type, View candidate);

private:
  FocusNavigationResultType mType;
  BaseHandle                mCandidate;
};

} // namespace Ui
} // namespace Dali
