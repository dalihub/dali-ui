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
#include <dali-test-suite-utils.h>
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
  DALI_TEST_CHECK(state == UiState::Normal);
  DALI_TEST_EQUALS(state.ToString(), std::string("Normal"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiStateNormalIsZero(void)
{
  // UiState::Normal should have no bits set
  DALI_TEST_CHECK(UiState::Normal.IsNormal());
  DALI_TEST_CHECK(!static_cast<bool>(UiState::Normal));

  END_TEST;
}

// =============================================================================
// Predefined States
// =============================================================================

int UtcDaliUiStatePredefinedStatesAreDistinct(void)
{
  // All predefined single states must be mutually distinct and non-zero
  DALI_TEST_CHECK(UiState::Focused        != UiState::Normal);
  DALI_TEST_CHECK(UiState::Pressed        != UiState::Normal);
  DALI_TEST_CHECK(UiState::Disabled       != UiState::Normal);
  DALI_TEST_CHECK(UiState::PseudoDisabled != UiState::Normal);
  DALI_TEST_CHECK(UiState::Selected       != UiState::Normal);

  DALI_TEST_CHECK(UiState::Focused        != UiState::Pressed);
  DALI_TEST_CHECK(UiState::Focused        != UiState::Disabled);
  DALI_TEST_CHECK(UiState::Focused        != UiState::PseudoDisabled);
  DALI_TEST_CHECK(UiState::Focused        != UiState::Selected);
  DALI_TEST_CHECK(UiState::Pressed        != UiState::Disabled);
  DALI_TEST_CHECK(UiState::Pressed        != UiState::PseudoDisabled);
  DALI_TEST_CHECK(UiState::Pressed        != UiState::Selected);
  DALI_TEST_CHECK(UiState::Disabled       != UiState::PseudoDisabled);
  DALI_TEST_CHECK(UiState::Disabled       != UiState::Selected);
  DALI_TEST_CHECK(UiState::PseudoDisabled != UiState::Selected);

  END_TEST;
}

int UtcDaliUiStatePredefinedStatesAreNonNormal(void)
{
  DALI_TEST_CHECK(!UiState::Focused.IsNormal());
  DALI_TEST_CHECK(!UiState::Pressed.IsNormal());
  DALI_TEST_CHECK(!UiState::Disabled.IsNormal());
  DALI_TEST_CHECK(!UiState::PseudoDisabled.IsNormal());
  DALI_TEST_CHECK(!UiState::Selected.IsNormal());

  END_TEST;
}

int UtcDaliUiStateAllContainsAllPredefined(void)
{
  // UiState::All should contain every predefined single state
  DALI_TEST_CHECK(UiState::All.Contains(UiState::Focused));
  DALI_TEST_CHECK(UiState::All.Contains(UiState::Pressed));
  DALI_TEST_CHECK(UiState::All.Contains(UiState::Disabled));
  DALI_TEST_CHECK(UiState::All.Contains(UiState::PseudoDisabled));
  DALI_TEST_CHECK(UiState::All.Contains(UiState::Selected));

  END_TEST;
}

// =============================================================================
// Predefined Composite States
// =============================================================================

int UtcDaliUiStateCompositeSelectedPressed(void)
{
  DALI_TEST_CHECK(UiState::SelectedPressed.Contains(UiState::Selected));
  DALI_TEST_CHECK(UiState::SelectedPressed.Contains(UiState::Pressed));
  DALI_TEST_CHECK(!UiState::SelectedPressed.Contains(UiState::Focused));

  END_TEST;
}

int UtcDaliUiStateCompositeDisabledSelected(void)
{
  DALI_TEST_CHECK(UiState::DisabledSelected.Contains(UiState::Disabled));
  DALI_TEST_CHECK(UiState::DisabledSelected.Contains(UiState::Selected));
  DALI_TEST_CHECK(!UiState::DisabledSelected.Contains(UiState::Focused));

  END_TEST;
}

int UtcDaliUiStateCompositeDisabledFocused(void)
{
  DALI_TEST_CHECK(UiState::DisabledFocused.Contains(UiState::Disabled));
  DALI_TEST_CHECK(UiState::DisabledFocused.Contains(UiState::Focused));
  DALI_TEST_CHECK(!UiState::DisabledFocused.Contains(UiState::Pressed));

  END_TEST;
}

int UtcDaliUiStateCompositeSelectedFocused(void)
{
  DALI_TEST_CHECK(UiState::SelectedFocused.Contains(UiState::Selected));
  DALI_TEST_CHECK(UiState::SelectedFocused.Contains(UiState::Focused));
  DALI_TEST_CHECK(!UiState::SelectedFocused.Contains(UiState::Pressed));

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
  DALI_TEST_CHECK(result == UiState::Normal);

  END_TEST;
}

int UtcDaliUiStateCreateAllReturnsAll(void)
{
  UiState result = UiState::Create("All");
  DALI_TEST_CHECK(result == UiState::All);

  END_TEST;
}

// =============================================================================
// Contains
// =============================================================================

int UtcDaliUiStateContainsSelf(void)
{
  DALI_TEST_CHECK(UiState::Focused.Contains(UiState::Focused));
  DALI_TEST_CHECK(UiState::SelectedPressed.Contains(UiState::SelectedPressed));

  END_TEST;
}

int UtcDaliUiStateContainsSubset(void)
{
  UiState composite = UiState::Focused + UiState::Pressed + UiState::Selected;
  DALI_TEST_CHECK(composite.Contains(UiState::Focused));
  DALI_TEST_CHECK(composite.Contains(UiState::Pressed));
  DALI_TEST_CHECK(composite.Contains(UiState::Selected));
  DALI_TEST_CHECK(!composite.Contains(UiState::Disabled));

  END_TEST;
}

int UtcDaliUiStateContainsNormalAlwaysTrue(void)
{
  // Every state contains Normal (zero bits) — vacuously true
  DALI_TEST_CHECK(UiState::Normal.Contains(UiState::Normal));
  DALI_TEST_CHECK(UiState::Focused.Contains(UiState::Normal));
  DALI_TEST_CHECK(UiState::SelectedPressed.Contains(UiState::Normal));

  END_TEST;
}

int UtcDaliUiStateContainsStrictSubsetFalseForSuperset(void)
{
  // Focused does NOT contain SelectedFocused (which requires Selected too)
  DALI_TEST_CHECK(!UiState::Focused.Contains(UiState::SelectedFocused));

  END_TEST;
}

// =============================================================================
// HasIntersectionWith
// =============================================================================

int UtcDaliUiStateHasIntersectionWithOverlapping(void)
{
  UiState composite = UiState::Focused + UiState::Pressed;
  DALI_TEST_CHECK(composite.HasIntersectionWith(UiState::Focused));
  DALI_TEST_CHECK(composite.HasIntersectionWith(UiState::Pressed));
  DALI_TEST_CHECK(composite.HasIntersectionWith(UiState::SelectedFocused)); // shares Focused bit

  END_TEST;
}

int UtcDaliUiStateHasIntersectionWithDisjoint(void)
{
  DALI_TEST_CHECK(!UiState::Focused.HasIntersectionWith(UiState::Pressed));
  DALI_TEST_CHECK(!UiState::Disabled.HasIntersectionWith(UiState::Selected));

  END_TEST;
}

int UtcDaliUiStateHasIntersectionWithNormal(void)
{
  // Normal (0) has no intersection with any state
  DALI_TEST_CHECK(!UiState::Normal.HasIntersectionWith(UiState::Focused));
  DALI_TEST_CHECK(!UiState::Focused.HasIntersectionWith(UiState::Normal));

  END_TEST;
}

// =============================================================================
// Operators: +, -, |, &, ^, ~
// =============================================================================

int UtcDaliUiStateOperatorPlus(void)
{
  UiState combined = UiState::Focused + UiState::Pressed;
  DALI_TEST_CHECK(combined.Contains(UiState::Focused));
  DALI_TEST_CHECK(combined.Contains(UiState::Pressed));
  DALI_TEST_CHECK(!combined.Contains(UiState::Disabled));

  END_TEST;
}

int UtcDaliUiStateOperatorMinus(void)
{
  UiState combined = UiState::Focused + UiState::Pressed + UiState::Selected;
  UiState removed  = combined - UiState::Pressed;
  DALI_TEST_CHECK(removed.Contains(UiState::Focused));
  DALI_TEST_CHECK(!removed.Contains(UiState::Pressed));
  DALI_TEST_CHECK(removed.Contains(UiState::Selected));

  END_TEST;
}

int UtcDaliUiStateOperatorMinusNonPresentIsNoop(void)
{
  UiState state   = UiState::Focused;
  UiState result  = state - UiState::Pressed; // Pressed wasn't set
  DALI_TEST_CHECK(result == UiState::Focused);

  END_TEST;
}

int UtcDaliUiStateOperatorBitwiseOr(void)
{
  UiState result = UiState::Focused | UiState::Pressed;
  DALI_TEST_CHECK(result == (UiState::Focused + UiState::Pressed));

  END_TEST;
}

int UtcDaliUiStateOperatorBitwiseAnd(void)
{
  UiState composite = UiState::Focused + UiState::Pressed;
  UiState masked    = composite & UiState::Focused;
  DALI_TEST_CHECK(masked == UiState::Focused);
  DALI_TEST_CHECK(!masked.Contains(UiState::Pressed));

  END_TEST;
}

int UtcDaliUiStateOperatorBitwiseXor(void)
{
  UiState a      = UiState::Focused + UiState::Pressed;
  UiState b      = UiState::Pressed + UiState::Selected;
  UiState result = a ^ b; // Focused and Selected remain; Pressed cancels out
  DALI_TEST_CHECK(result.Contains(UiState::Focused));
  DALI_TEST_CHECK(!result.Contains(UiState::Pressed));
  DALI_TEST_CHECK(result.Contains(UiState::Selected));

  END_TEST;
}

int UtcDaliUiStateOperatorBitwiseNot(void)
{
  UiState notNormal = ~UiState::Normal;
  // ~Normal should contain all predefined states
  DALI_TEST_CHECK(notNormal.Contains(UiState::Focused));
  DALI_TEST_CHECK(notNormal.Contains(UiState::Pressed));
  DALI_TEST_CHECK(notNormal.Contains(UiState::Disabled));

  UiState notFocused = ~UiState::Focused;
  DALI_TEST_CHECK(!notFocused.Contains(UiState::Focused));
  DALI_TEST_CHECK(notFocused.Contains(UiState::Pressed));

  END_TEST;
}

int UtcDaliUiStateOperatorEquality(void)
{
  DALI_TEST_CHECK(UiState::Focused == UiState::Focused);
  DALI_TEST_CHECK(!(UiState::Focused == UiState::Pressed));

  END_TEST;
}

int UtcDaliUiStateOperatorInequality(void)
{
  DALI_TEST_CHECK(UiState::Focused != UiState::Pressed);
  DALI_TEST_CHECK(!(UiState::Focused != UiState::Focused));

  END_TEST;
}

int UtcDaliUiStateOperatorBool(void)
{
  DALI_TEST_CHECK(!static_cast<bool>(UiState::Normal));
  DALI_TEST_CHECK(static_cast<bool>(UiState::Focused));
  DALI_TEST_CHECK(static_cast<bool>(UiState::SelectedPressed));

  END_TEST;
}

// =============================================================================
// ToString
// =============================================================================

int UtcDaliUiStateToStringNormal(void)
{
  DALI_TEST_EQUALS(UiState::Normal.ToString(), std::string("Normal"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiStateToStringAll(void)
{
  DALI_TEST_EQUALS(UiState::All.ToString(), std::string("All"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiStateToStringSingleState(void)
{
  std::string str = UiState::Focused.ToString();
  DALI_TEST_CHECK(str.find("Focused") != std::string::npos);

  END_TEST;
}

int UtcDaliUiStateToStringCompositeContainsBothNames(void)
{
  UiState composite = UiState::Focused + UiState::Pressed;
  std::string str   = composite.ToString();
  DALI_TEST_CHECK(str.find("Focused") != std::string::npos);
  DALI_TEST_CHECK(str.find("Pressed") != std::string::npos);

  END_TEST;
}

// =============================================================================
// State Change Filters: WasAdded, WasRemoved, WasChanged, AnyChanged
// =============================================================================

int UtcDaliUiStateWasAdded(void)
{
  UiState prev = UiState::Normal;
  UiState cur  = UiState::Focused;

  DALI_TEST_CHECK(UiState::Focused.WasAdded(prev, cur));
  DALI_TEST_CHECK(!UiState::Pressed.WasAdded(prev, cur));
  // Already present — not "added"
  DALI_TEST_CHECK(!UiState::Focused.WasAdded(cur, cur));

  END_TEST;
}

int UtcDaliUiStateWasRemoved(void)
{
  UiState prev = UiState::Focused + UiState::Pressed;
  UiState cur  = UiState::Focused;

  DALI_TEST_CHECK(UiState::Pressed.WasRemoved(prev, cur));
  DALI_TEST_CHECK(!UiState::Focused.WasRemoved(prev, cur));
  DALI_TEST_CHECK(!UiState::Disabled.WasRemoved(prev, cur));

  END_TEST;
}

int UtcDaliUiStateWasChanged(void)
{
  UiState prev = UiState::Focused;
  UiState cur  = UiState::Pressed;

  // Focused was removed → changed
  DALI_TEST_CHECK(UiState::Focused.WasChanged(prev, cur));
  // Pressed was added → changed
  DALI_TEST_CHECK(UiState::Pressed.WasChanged(prev, cur));
  // Disabled untouched → not changed
  DALI_TEST_CHECK(!UiState::Disabled.WasChanged(prev, cur));

  END_TEST;
}

int UtcDaliUiStateWasChangedUnchanged(void)
{
  UiState state = UiState::Focused;
  DALI_TEST_CHECK(!UiState::Focused.WasChanged(state, state));

  END_TEST;
}

int UtcDaliUiStateAnyChangedComposite(void)
{
  UiState prev      = UiState::Focused;
  UiState cur       = UiState::Focused + UiState::Pressed;
  UiState watchMask = UiState::Pressed + UiState::Selected;

  // Pressed bit toggled within watchMask → true
  DALI_TEST_CHECK(watchMask.AnyChanged(prev, cur));

  END_TEST;
}

int UtcDaliUiStateAnyChangedNoOverlap(void)
{
  UiState prev      = UiState::Focused;
  UiState cur       = UiState::Pressed;
  UiState watchMask = UiState::Disabled + UiState::Selected; // no overlap with transition

  DALI_TEST_CHECK(!watchMask.AnyChanged(prev, cur));

  END_TEST;
}

int UtcDaliUiStateAnyChangedNoTransition(void)
{
  UiState state     = UiState::Focused + UiState::Pressed;
  UiState watchMask = UiState::Focused + UiState::Pressed;

  DALI_TEST_CHECK(!watchMask.AnyChanged(state, state));

  END_TEST;
}

// =============================================================================
// Idempotent Combination
// =============================================================================

int UtcDaliUiStateAddSameStateIsIdempotent(void)
{
  UiState result = UiState::Focused + UiState::Focused;
  DALI_TEST_CHECK(result == UiState::Focused);

  END_TEST;
}

int UtcDaliUiStateRemoveSameStateGivesNormal(void)
{
  UiState result = UiState::Focused - UiState::Focused;
  DALI_TEST_CHECK(result.IsNormal());

  END_TEST;
}
