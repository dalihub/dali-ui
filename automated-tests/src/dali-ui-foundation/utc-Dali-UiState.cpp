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

#include <stdlib.h>
#include <iostream>

#include <dali.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/ui-state.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_uistate_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_uistate_cleanup(void)
{
  test_return_value = TET_PASS;
}

// =============================================================================
// Default Constructor / Normal State
// =============================================================================

int UtcDaliUiStateDefaultConstructor(void)
{
  // Default-constructed UiState should be Normal (bits == 0)
  UiState state;
  DALI_TEST_CHECK(state.IsNormal());
  DALI_TEST_CHECK(state == UiState::NORMAL);
  DALI_TEST_EQUALS(state.ToString(), String("Normal"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiStateNormalIsZero(void)
{
  // UiState::NORMAL should have no bits set
  DALI_TEST_CHECK(UiState::NORMAL.IsNormal());
  DALI_TEST_CHECK(!static_cast<bool>(UiState::NORMAL));

  END_TEST;
}

// =============================================================================
// Predefined States
// =============================================================================

int UtcDaliUiStatePredefinedStatesAreDistinct(void)
{
  // All predefined single states must be mutually distinct and non-zero
  DALI_TEST_CHECK(UiState::FOCUSED        != UiState::NORMAL);
  DALI_TEST_CHECK(UiState::PRESSED        != UiState::NORMAL);
  DALI_TEST_CHECK(UiState::DISABLED       != UiState::NORMAL);
  DALI_TEST_CHECK(UiState::PSEUDO_DISABLED != UiState::NORMAL);
  DALI_TEST_CHECK(UiState::SELECTED       != UiState::NORMAL);

  DALI_TEST_CHECK(UiState::FOCUSED        != UiState::PRESSED);
  DALI_TEST_CHECK(UiState::FOCUSED        != UiState::DISABLED);
  DALI_TEST_CHECK(UiState::FOCUSED        != UiState::PSEUDO_DISABLED);
  DALI_TEST_CHECK(UiState::FOCUSED        != UiState::SELECTED);
  DALI_TEST_CHECK(UiState::PRESSED        != UiState::DISABLED);
  DALI_TEST_CHECK(UiState::PRESSED        != UiState::PSEUDO_DISABLED);
  DALI_TEST_CHECK(UiState::PRESSED        != UiState::SELECTED);
  DALI_TEST_CHECK(UiState::DISABLED       != UiState::PSEUDO_DISABLED);
  DALI_TEST_CHECK(UiState::DISABLED       != UiState::SELECTED);
  DALI_TEST_CHECK(UiState::PSEUDO_DISABLED != UiState::SELECTED);

  END_TEST;
}

int UtcDaliUiStatePredefinedStatesAreNonNormal(void)
{
  DALI_TEST_CHECK(!UiState::FOCUSED.IsNormal());
  DALI_TEST_CHECK(!UiState::PRESSED.IsNormal());
  DALI_TEST_CHECK(!UiState::DISABLED.IsNormal());
  DALI_TEST_CHECK(!UiState::PSEUDO_DISABLED.IsNormal());
  DALI_TEST_CHECK(!UiState::SELECTED.IsNormal());

  END_TEST;
}

int UtcDaliUiStateAllContainsAllPredefined(void)
{
  // UiState::ALL should contain every predefined single state
  DALI_TEST_CHECK(UiState::ALL.Contains(UiState::FOCUSED));
  DALI_TEST_CHECK(UiState::ALL.Contains(UiState::PRESSED));
  DALI_TEST_CHECK(UiState::ALL.Contains(UiState::DISABLED));
  DALI_TEST_CHECK(UiState::ALL.Contains(UiState::PSEUDO_DISABLED));
  DALI_TEST_CHECK(UiState::ALL.Contains(UiState::SELECTED));

  END_TEST;
}

// =============================================================================
// Predefined Composite States
// =============================================================================

int UtcDaliUiStateCompositeSelectedPressed(void)
{
  DALI_TEST_CHECK(UiState::SELECTED_PRESSED.Contains(UiState::SELECTED));
  DALI_TEST_CHECK(UiState::SELECTED_PRESSED.Contains(UiState::PRESSED));
  DALI_TEST_CHECK(!UiState::SELECTED_PRESSED.Contains(UiState::FOCUSED));

  END_TEST;
}

int UtcDaliUiStateCompositeDisabledSelected(void)
{
  DALI_TEST_CHECK(UiState::DISABLED_SELECTED.Contains(UiState::DISABLED));
  DALI_TEST_CHECK(UiState::DISABLED_SELECTED.Contains(UiState::SELECTED));
  DALI_TEST_CHECK(!UiState::DISABLED_SELECTED.Contains(UiState::FOCUSED));

  END_TEST;
}


int UtcDaliUiStateCompositeSelectedFocused(void)
{
  DALI_TEST_CHECK(UiState::SELECTED_FOCUSED.Contains(UiState::SELECTED));
  DALI_TEST_CHECK(UiState::SELECTED_FOCUSED.Contains(UiState::FOCUSED));
  DALI_TEST_CHECK(!UiState::SELECTED_FOCUSED.Contains(UiState::PRESSED));

  END_TEST;
}

// =============================================================================
// Create (Custom States)
// =============================================================================

int UtcDaliUiStateCreateReturnsNonNormal(void)
{
  UiState custom = UiState::Create("UtcCustomStateA");
  DALI_TEST_CHECK(!custom.IsNormal());
  DALI_TEST_CHECK(static_cast<bool>(custom));

  END_TEST;
}

int UtcDaliUiStateCreateSameNameReturnsSameBits(void)
{
  UiState first  = UiState::Create("UtcCustomStateB");
  UiState second = UiState::Create("UtcCustomStateB");
  DALI_TEST_CHECK(first == second);

  END_TEST;
}

int UtcDaliUiStateCreateDifferentNamesReturnsDifferentBits(void)
{
  UiState stateC = UiState::Create("UtcCustomStateC");
  UiState stateD = UiState::Create("UtcCustomStateD");
  DALI_TEST_CHECK(stateC != stateD);

  END_TEST;
}

int UtcDaliUiStateCreateNormalReturnsNormal(void)
{
  UiState result = UiState::Create("Normal");
  DALI_TEST_CHECK(result.IsNormal());
  DALI_TEST_CHECK(result == UiState::NORMAL);

  END_TEST;
}

int UtcDaliUiStateCreateAllReturnsAll(void)
{
  UiState result = UiState::Create("All");
  DALI_TEST_CHECK(result == UiState::ALL);

  END_TEST;
}

// =============================================================================
// Contains
// =============================================================================

int UtcDaliUiStateContainsSelf(void)
{
  DALI_TEST_CHECK(UiState::FOCUSED.Contains(UiState::FOCUSED));
  DALI_TEST_CHECK(UiState::SELECTED_PRESSED.Contains(UiState::SELECTED_PRESSED));

  END_TEST;
}

int UtcDaliUiStateContainsSubset(void)
{
  UiState composite = UiState::FOCUSED + UiState::PRESSED + UiState::SELECTED;
  DALI_TEST_CHECK(composite.Contains(UiState::FOCUSED));
  DALI_TEST_CHECK(composite.Contains(UiState::PRESSED));
  DALI_TEST_CHECK(composite.Contains(UiState::SELECTED));
  DALI_TEST_CHECK(!composite.Contains(UiState::DISABLED));

  END_TEST;
}

int UtcDaliUiStateContainsNormalAlwaysTrue(void)
{
  // Every state contains Normal (zero bits) — vacuously true
  DALI_TEST_CHECK(UiState::NORMAL.Contains(UiState::NORMAL));
  DALI_TEST_CHECK(UiState::FOCUSED.Contains(UiState::NORMAL));
  DALI_TEST_CHECK(UiState::SELECTED_PRESSED.Contains(UiState::NORMAL));

  END_TEST;
}

int UtcDaliUiStateContainsStrictSubsetFalseForSuperset(void)
{
  // Focused does NOT contain SelectedFocused (which requires Selected too)
  DALI_TEST_CHECK(!UiState::FOCUSED.Contains(UiState::SELECTED_FOCUSED));

  END_TEST;
}

// =============================================================================
// HasIntersectionWith
// =============================================================================

int UtcDaliUiStateHasIntersectionWithOverlapping(void)
{
  UiState composite = UiState::FOCUSED + UiState::PRESSED;
  DALI_TEST_CHECK(composite.HasIntersectionWith(UiState::FOCUSED));
  DALI_TEST_CHECK(composite.HasIntersectionWith(UiState::PRESSED));
  DALI_TEST_CHECK(composite.HasIntersectionWith(UiState::SELECTED_FOCUSED)); // shares Focused bit

  END_TEST;
}

int UtcDaliUiStateHasIntersectionWithDisjoint(void)
{
  DALI_TEST_CHECK(!UiState::FOCUSED.HasIntersectionWith(UiState::PRESSED));
  DALI_TEST_CHECK(!UiState::DISABLED.HasIntersectionWith(UiState::SELECTED));

  END_TEST;
}

int UtcDaliUiStateHasIntersectionWithNormal(void)
{
  // Normal (0) has no intersection with any state
  DALI_TEST_CHECK(!UiState::NORMAL.HasIntersectionWith(UiState::FOCUSED));
  DALI_TEST_CHECK(!UiState::FOCUSED.HasIntersectionWith(UiState::NORMAL));

  END_TEST;
}

// =============================================================================
// Operators: +, -, |, &, ^, ~
// =============================================================================

int UtcDaliUiStateOperatorPlus(void)
{
  UiState combined = UiState::FOCUSED + UiState::PRESSED;
  DALI_TEST_CHECK(combined.Contains(UiState::FOCUSED));
  DALI_TEST_CHECK(combined.Contains(UiState::PRESSED));
  DALI_TEST_CHECK(!combined.Contains(UiState::DISABLED));

  END_TEST;
}

int UtcDaliUiStateOperatorMinus(void)
{
  UiState combined = UiState::FOCUSED + UiState::PRESSED + UiState::SELECTED;
  UiState removed  = combined - UiState::PRESSED;
  DALI_TEST_CHECK(removed.Contains(UiState::FOCUSED));
  DALI_TEST_CHECK(!removed.Contains(UiState::PRESSED));
  DALI_TEST_CHECK(removed.Contains(UiState::SELECTED));

  END_TEST;
}

int UtcDaliUiStateOperatorMinusNonPresentIsNoop(void)
{
  UiState state   = UiState::FOCUSED;
  UiState result  = state - UiState::PRESSED; // Pressed wasn't set
  DALI_TEST_CHECK(result == UiState::FOCUSED);

  END_TEST;
}

int UtcDaliUiStateOperatorBitwiseOr(void)
{
  UiState result = UiState::FOCUSED | UiState::PRESSED;
  DALI_TEST_CHECK(result == (UiState::FOCUSED + UiState::PRESSED));

  END_TEST;
}

int UtcDaliUiStateOperatorBitwiseAnd(void)
{
  UiState composite = UiState::FOCUSED + UiState::PRESSED;
  UiState masked    = composite & UiState::FOCUSED;
  DALI_TEST_CHECK(masked == UiState::FOCUSED);
  DALI_TEST_CHECK(!masked.Contains(UiState::PRESSED));

  END_TEST;
}

int UtcDaliUiStateOperatorBitwiseXor(void)
{
  UiState a      = UiState::FOCUSED + UiState::PRESSED;
  UiState b      = UiState::PRESSED + UiState::SELECTED;
  UiState result = a ^ b; // Focused and Selected remain; Pressed cancels out
  DALI_TEST_CHECK(result.Contains(UiState::FOCUSED));
  DALI_TEST_CHECK(!result.Contains(UiState::PRESSED));
  DALI_TEST_CHECK(result.Contains(UiState::SELECTED));

  END_TEST;
}

int UtcDaliUiStateOperatorBitwiseNot(void)
{
  UiState notNormal = ~UiState::NORMAL;
  // ~Normal should contain all predefined states
  DALI_TEST_CHECK(notNormal.Contains(UiState::FOCUSED));
  DALI_TEST_CHECK(notNormal.Contains(UiState::PRESSED));
  DALI_TEST_CHECK(notNormal.Contains(UiState::DISABLED));

  UiState notFocused = ~UiState::FOCUSED;
  DALI_TEST_CHECK(!notFocused.Contains(UiState::FOCUSED));
  DALI_TEST_CHECK(notFocused.Contains(UiState::PRESSED));

  END_TEST;
}

int UtcDaliUiStateOperatorEquality(void)
{
  DALI_TEST_CHECK(UiState::FOCUSED == UiState::FOCUSED);
  DALI_TEST_CHECK(!(UiState::FOCUSED == UiState::PRESSED));

  END_TEST;
}

int UtcDaliUiStateOperatorInequality(void)
{
  DALI_TEST_CHECK(UiState::FOCUSED != UiState::PRESSED);
  DALI_TEST_CHECK(!(UiState::FOCUSED != UiState::FOCUSED));

  END_TEST;
}

int UtcDaliUiStateOperatorBool(void)
{
  DALI_TEST_CHECK(!static_cast<bool>(UiState::NORMAL));
  DALI_TEST_CHECK(static_cast<bool>(UiState::FOCUSED));
  DALI_TEST_CHECK(static_cast<bool>(UiState::SELECTED_PRESSED));

  END_TEST;
}

// =============================================================================
// ToString
// =============================================================================

int UtcDaliUiStateToStringNormal(void)
{
  DALI_TEST_EQUALS(UiState::NORMAL.ToString(), String("Normal"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiStateToStringAll(void)
{
  DALI_TEST_EQUALS(UiState::ALL.ToString(), String("All"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiStateToStringSingleState(void)
{
  String str = UiState::FOCUSED.ToString();
  DALI_TEST_CHECK(strstr(str.CStr(), "Focused") != nullptr);

  END_TEST;
}

int UtcDaliUiStateToStringCompositeContainsBothNames(void)
{
  UiState composite = UiState::FOCUSED + UiState::PRESSED;
  String str = composite.ToString();
  DALI_TEST_CHECK(strstr(str.CStr(), "Focused") != nullptr);
  DALI_TEST_CHECK(strstr(str.CStr(), "Pressed") != nullptr);

  END_TEST;
}

// =============================================================================
// State Change Filters: WasAdded, WasRemoved, WasChanged, AnyChanged
// =============================================================================

int UtcDaliUiStateWasAdded(void)
{
  UiState prev = UiState::NORMAL;
  UiState cur  = UiState::FOCUSED;

  DALI_TEST_CHECK(UiState::FOCUSED.WasAdded(prev, cur));
  DALI_TEST_CHECK(!UiState::PRESSED.WasAdded(prev, cur));
  // Already present — not "added"
  DALI_TEST_CHECK(!UiState::FOCUSED.WasAdded(cur, cur));

  END_TEST;
}

int UtcDaliUiStateWasRemoved(void)
{
  UiState prev = UiState::FOCUSED + UiState::PRESSED;
  UiState cur  = UiState::FOCUSED;

  DALI_TEST_CHECK(UiState::PRESSED.WasRemoved(prev, cur));
  DALI_TEST_CHECK(!UiState::FOCUSED.WasRemoved(prev, cur));
  DALI_TEST_CHECK(!UiState::DISABLED.WasRemoved(prev, cur));

  END_TEST;
}

int UtcDaliUiStateWasChanged(void)
{
  UiState prev = UiState::FOCUSED;
  UiState cur  = UiState::PRESSED;

  // Focused was removed → changed
  DALI_TEST_CHECK(UiState::FOCUSED.WasChanged(prev, cur));
  // Pressed was added → changed
  DALI_TEST_CHECK(UiState::PRESSED.WasChanged(prev, cur));
  // Disabled untouched → not changed
  DALI_TEST_CHECK(!UiState::DISABLED.WasChanged(prev, cur));

  END_TEST;
}

int UtcDaliUiStateWasChangedUnchanged(void)
{
  UiState state = UiState::FOCUSED;
  DALI_TEST_CHECK(!UiState::FOCUSED.WasChanged(state, state));

  END_TEST;
}

int UtcDaliUiStateAnyChangedComposite(void)
{
  UiState prev      = UiState::FOCUSED;
  UiState cur       = UiState::FOCUSED + UiState::PRESSED;
  UiState watchMask = UiState::PRESSED + UiState::SELECTED;

  // Pressed bit toggled within watchMask → true
  DALI_TEST_CHECK(watchMask.AnyChanged(prev, cur));

  END_TEST;
}

int UtcDaliUiStateAnyChangedNoOverlap(void)
{
  UiState prev      = UiState::FOCUSED;
  UiState cur       = UiState::PRESSED;
  UiState watchMask = UiState::DISABLED + UiState::SELECTED; // no overlap with transition

  DALI_TEST_CHECK(!watchMask.AnyChanged(prev, cur));

  END_TEST;
}

int UtcDaliUiStateAnyChangedNoTransition(void)
{
  UiState state     = UiState::FOCUSED + UiState::PRESSED;
  UiState watchMask = UiState::FOCUSED + UiState::PRESSED;

  DALI_TEST_CHECK(!watchMask.AnyChanged(state, state));

  END_TEST;
}

// =============================================================================
// Idempotent Combination
// =============================================================================

int UtcDaliUiStateAddSameStateIsIdempotent(void)
{
  UiState result = UiState::FOCUSED + UiState::FOCUSED;
  DALI_TEST_CHECK(result == UiState::FOCUSED);

  END_TEST;
}

int UtcDaliUiStateRemoveSameStateGivesNormal(void)
{
  UiState result = UiState::FOCUSED - UiState::FOCUSED;
  DALI_TEST_CHECK(result.IsNormal());

  END_TEST;
}
