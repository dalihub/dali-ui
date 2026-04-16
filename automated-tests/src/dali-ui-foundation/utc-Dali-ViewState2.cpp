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
#include <dali-ui-foundation/public-api/view-state.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_viewstate_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_viewstate_cleanup(void)
{
  test_return_value = TET_PASS;
}

// =============================================================================
// Default Constructor / Normal State
// =============================================================================

int UtcDaliViewStateDefaultConstructor(void)
{
  // Default-constructed ViewState should be Normal (bits == 0)
  ViewState state;
  DALI_TEST_CHECK(state.IsNormal());
  DALI_TEST_CHECK(state == ViewState::NORMAL);
  DALI_TEST_EQUALS(state.ToString(), String("Normal"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewStateNormalIsZero(void)
{
  // ViewState::NORMAL should have no bits set
  DALI_TEST_CHECK(ViewState::NORMAL.IsNormal());
  DALI_TEST_CHECK(!static_cast<bool>(ViewState::NORMAL));

  END_TEST;
}

// =============================================================================
// Predefined States
// =============================================================================

int UtcDaliViewStatePredefinedStatesAreDistinct(void)
{
  // All predefined single states must be mutually distinct and non-zero
  DALI_TEST_CHECK(ViewState::FOCUSED        != ViewState::NORMAL);
  DALI_TEST_CHECK(ViewState::PRESSED        != ViewState::NORMAL);
  DALI_TEST_CHECK(ViewState::DISABLED       != ViewState::NORMAL);
  DALI_TEST_CHECK(ViewState::PSEUDO_DISABLED != ViewState::NORMAL);
  DALI_TEST_CHECK(ViewState::SELECTED       != ViewState::NORMAL);

  DALI_TEST_CHECK(ViewState::FOCUSED        != ViewState::PRESSED);
  DALI_TEST_CHECK(ViewState::FOCUSED        != ViewState::DISABLED);
  DALI_TEST_CHECK(ViewState::FOCUSED        != ViewState::PSEUDO_DISABLED);
  DALI_TEST_CHECK(ViewState::FOCUSED        != ViewState::SELECTED);
  DALI_TEST_CHECK(ViewState::PRESSED        != ViewState::DISABLED);
  DALI_TEST_CHECK(ViewState::PRESSED        != ViewState::PSEUDO_DISABLED);
  DALI_TEST_CHECK(ViewState::PRESSED        != ViewState::SELECTED);
  DALI_TEST_CHECK(ViewState::DISABLED       != ViewState::PSEUDO_DISABLED);
  DALI_TEST_CHECK(ViewState::DISABLED       != ViewState::SELECTED);
  DALI_TEST_CHECK(ViewState::PSEUDO_DISABLED != ViewState::SELECTED);

  END_TEST;
}

int UtcDaliViewStatePredefinedStatesAreNonNormal(void)
{
  DALI_TEST_CHECK(!ViewState::FOCUSED.IsNormal());
  DALI_TEST_CHECK(!ViewState::PRESSED.IsNormal());
  DALI_TEST_CHECK(!ViewState::DISABLED.IsNormal());
  DALI_TEST_CHECK(!ViewState::PSEUDO_DISABLED.IsNormal());
  DALI_TEST_CHECK(!ViewState::SELECTED.IsNormal());

  END_TEST;
}

int UtcDaliViewStateAllContainsAllPredefined(void)
{
  // ViewState::ALL should contain every predefined single state
  DALI_TEST_CHECK(ViewState::ALL.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(ViewState::ALL.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(ViewState::ALL.Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(ViewState::ALL.Contains(ViewState::PSEUDO_DISABLED));
  DALI_TEST_CHECK(ViewState::ALL.Contains(ViewState::SELECTED));

  END_TEST;
}

// =============================================================================
// Predefined Composite States
// =============================================================================

int UtcDaliViewStateCompositeSelectedPressed(void)
{
  DALI_TEST_CHECK(ViewState::SELECTED_PRESSED.Contains(ViewState::SELECTED));
  DALI_TEST_CHECK(ViewState::SELECTED_PRESSED.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(!ViewState::SELECTED_PRESSED.Contains(ViewState::FOCUSED));

  END_TEST;
}

int UtcDaliViewStateCompositeDisabledSelected(void)
{
  DALI_TEST_CHECK(ViewState::DISABLED_SELECTED.Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(ViewState::DISABLED_SELECTED.Contains(ViewState::SELECTED));
  DALI_TEST_CHECK(!ViewState::DISABLED_SELECTED.Contains(ViewState::FOCUSED));

  END_TEST;
}


int UtcDaliViewStateCompositeSelectedFocused(void)
{
  DALI_TEST_CHECK(ViewState::SELECTED_FOCUSED.Contains(ViewState::SELECTED));
  DALI_TEST_CHECK(ViewState::SELECTED_FOCUSED.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!ViewState::SELECTED_FOCUSED.Contains(ViewState::PRESSED));

  END_TEST;
}

// =============================================================================
// Create (Custom States)
// =============================================================================

int UtcDaliViewStateCreateReturnsNonNormal(void)
{
  ViewState custom = ViewState::Create("UtcCustomStateA");
  DALI_TEST_CHECK(!custom.IsNormal());
  DALI_TEST_CHECK(static_cast<bool>(custom));

  END_TEST;
}

int UtcDaliViewStateCreateSameNameReturnsSameBits(void)
{
  ViewState first  = ViewState::Create("UtcCustomStateB");
  ViewState second = ViewState::Create("UtcCustomStateB");
  DALI_TEST_CHECK(first == second);

  END_TEST;
}

int UtcDaliViewStateCreateDifferentNamesReturnsDifferentBits(void)
{
  ViewState stateC = ViewState::Create("UtcCustomStateC");
  ViewState stateD = ViewState::Create("UtcCustomStateD");
  DALI_TEST_CHECK(stateC != stateD);

  END_TEST;
}

int UtcDaliViewStateCreateNormalReturnsNormal(void)
{
  ViewState result = ViewState::Create("Normal");
  DALI_TEST_CHECK(result.IsNormal());
  DALI_TEST_CHECK(result == ViewState::NORMAL);

  END_TEST;
}

int UtcDaliViewStateCreateAllReturnsAll(void)
{
  ViewState result = ViewState::Create("All");
  DALI_TEST_CHECK(result == ViewState::ALL);

  END_TEST;
}

// =============================================================================
// Contains
// =============================================================================

int UtcDaliViewStateContainsSelf(void)
{
  DALI_TEST_CHECK(ViewState::FOCUSED.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(ViewState::SELECTED_PRESSED.Contains(ViewState::SELECTED_PRESSED));

  END_TEST;
}

int UtcDaliViewStateContainsSubset(void)
{
  ViewState composite = ViewState::FOCUSED + ViewState::PRESSED + ViewState::SELECTED;
  DALI_TEST_CHECK(composite.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(composite.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(composite.Contains(ViewState::SELECTED));
  DALI_TEST_CHECK(!composite.Contains(ViewState::DISABLED));

  END_TEST;
}

int UtcDaliViewStateContainsNormalAlwaysTrue(void)
{
  // Every state contains Normal (zero bits) — vacuously true
  DALI_TEST_CHECK(ViewState::NORMAL.Contains(ViewState::NORMAL));
  DALI_TEST_CHECK(ViewState::FOCUSED.Contains(ViewState::NORMAL));
  DALI_TEST_CHECK(ViewState::SELECTED_PRESSED.Contains(ViewState::NORMAL));

  END_TEST;
}

int UtcDaliViewStateContainsStrictSubsetFalseForSuperset(void)
{
  // Focused does NOT contain SelectedFocused (which requires Selected too)
  DALI_TEST_CHECK(!ViewState::FOCUSED.Contains(ViewState::SELECTED_FOCUSED));

  END_TEST;
}

// =============================================================================
// HasIntersectionWith
// =============================================================================

int UtcDaliViewStateHasIntersectionWithOverlapping(void)
{
  ViewState composite = ViewState::FOCUSED + ViewState::PRESSED;
  DALI_TEST_CHECK(composite.HasIntersectionWith(ViewState::FOCUSED));
  DALI_TEST_CHECK(composite.HasIntersectionWith(ViewState::PRESSED));
  DALI_TEST_CHECK(composite.HasIntersectionWith(ViewState::SELECTED_FOCUSED)); // shares Focused bit

  END_TEST;
}

int UtcDaliViewStateHasIntersectionWithDisjoint(void)
{
  DALI_TEST_CHECK(!ViewState::FOCUSED.HasIntersectionWith(ViewState::PRESSED));
  DALI_TEST_CHECK(!ViewState::DISABLED.HasIntersectionWith(ViewState::SELECTED));

  END_TEST;
}

int UtcDaliViewStateHasIntersectionWithNormal(void)
{
  // Normal (0) has no intersection with any state
  DALI_TEST_CHECK(!ViewState::NORMAL.HasIntersectionWith(ViewState::FOCUSED));
  DALI_TEST_CHECK(!ViewState::FOCUSED.HasIntersectionWith(ViewState::NORMAL));

  END_TEST;
}

// =============================================================================
// Operators: +, -, |, &, ^, ~
// =============================================================================

int UtcDaliViewStateOperatorPlus(void)
{
  ViewState combined = ViewState::FOCUSED + ViewState::PRESSED;
  DALI_TEST_CHECK(combined.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(combined.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(!combined.Contains(ViewState::DISABLED));

  END_TEST;
}

int UtcDaliViewStateOperatorMinus(void)
{
  ViewState combined = ViewState::FOCUSED + ViewState::PRESSED + ViewState::SELECTED;
  ViewState removed  = combined - ViewState::PRESSED;
  DALI_TEST_CHECK(removed.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!removed.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(removed.Contains(ViewState::SELECTED));

  END_TEST;
}

int UtcDaliViewStateOperatorMinusNonPresentIsNoop(void)
{
  ViewState state   = ViewState::FOCUSED;
  ViewState result  = state - ViewState::PRESSED; // Pressed wasn't set
  DALI_TEST_CHECK(result == ViewState::FOCUSED);

  END_TEST;
}

int UtcDaliViewStateOperatorBitwiseOr(void)
{
  ViewState result = ViewState::FOCUSED | ViewState::PRESSED;
  DALI_TEST_CHECK(result == (ViewState::FOCUSED + ViewState::PRESSED));

  END_TEST;
}

int UtcDaliViewStateOperatorBitwiseAnd(void)
{
  ViewState composite = ViewState::FOCUSED + ViewState::PRESSED;
  ViewState masked    = composite & ViewState::FOCUSED;
  DALI_TEST_CHECK(masked == ViewState::FOCUSED);
  DALI_TEST_CHECK(!masked.Contains(ViewState::PRESSED));

  END_TEST;
}

int UtcDaliViewStateOperatorBitwiseXor(void)
{
  ViewState a      = ViewState::FOCUSED + ViewState::PRESSED;
  ViewState b      = ViewState::PRESSED + ViewState::SELECTED;
  ViewState result = a ^ b; // Focused and Selected remain; Pressed cancels out
  DALI_TEST_CHECK(result.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!result.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(result.Contains(ViewState::SELECTED));

  END_TEST;
}

int UtcDaliViewStateOperatorBitwiseNot(void)
{
  ViewState notNormal = ~ViewState::NORMAL;
  // ~Normal should contain all predefined states
  DALI_TEST_CHECK(notNormal.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(notNormal.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(notNormal.Contains(ViewState::DISABLED));

  ViewState notFocused = ~ViewState::FOCUSED;
  DALI_TEST_CHECK(!notFocused.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(notFocused.Contains(ViewState::PRESSED));

  END_TEST;
}

int UtcDaliViewStateOperatorEquality(void)
{
  DALI_TEST_CHECK(ViewState::FOCUSED == ViewState::FOCUSED);
  DALI_TEST_CHECK(!(ViewState::FOCUSED == ViewState::PRESSED));

  END_TEST;
}

int UtcDaliViewStateOperatorInequality(void)
{
  DALI_TEST_CHECK(ViewState::FOCUSED != ViewState::PRESSED);
  DALI_TEST_CHECK(!(ViewState::FOCUSED != ViewState::FOCUSED));

  END_TEST;
}

int UtcDaliViewStateOperatorBool(void)
{
  DALI_TEST_CHECK(!static_cast<bool>(ViewState::NORMAL));
  DALI_TEST_CHECK(static_cast<bool>(ViewState::FOCUSED));
  DALI_TEST_CHECK(static_cast<bool>(ViewState::SELECTED_PRESSED));

  END_TEST;
}

// =============================================================================
// ToString
// =============================================================================

int UtcDaliViewStateToStringNormal(void)
{
  DALI_TEST_EQUALS(ViewState::NORMAL.ToString(), String("Normal"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewStateToStringAll(void)
{
  DALI_TEST_EQUALS(ViewState::ALL.ToString(), String("All"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewStateToStringSingleState(void)
{
  String str = ViewState::FOCUSED.ToString();
  DALI_TEST_CHECK(strstr(str.CStr(), "Focused") != nullptr);

  END_TEST;
}

int UtcDaliViewStateToStringCompositeContainsBothNames(void)
{
  ViewState composite = ViewState::FOCUSED + ViewState::PRESSED;
  String str = composite.ToString();
  DALI_TEST_CHECK(strstr(str.CStr(), "Focused") != nullptr);
  DALI_TEST_CHECK(strstr(str.CStr(), "Pressed") != nullptr);

  END_TEST;
}

// =============================================================================
// State Change Filters: WasAdded, WasRemoved, WasChanged, AnyChanged
// =============================================================================

int UtcDaliViewStateWasAdded(void)
{
  ViewState prev = ViewState::NORMAL;
  ViewState cur  = ViewState::FOCUSED;

  DALI_TEST_CHECK(ViewState::FOCUSED.WasAdded(prev, cur));
  DALI_TEST_CHECK(!ViewState::PRESSED.WasAdded(prev, cur));
  // Already present — not "added"
  DALI_TEST_CHECK(!ViewState::FOCUSED.WasAdded(cur, cur));

  END_TEST;
}

int UtcDaliViewStateWasRemoved(void)
{
  ViewState prev = ViewState::FOCUSED + ViewState::PRESSED;
  ViewState cur  = ViewState::FOCUSED;

  DALI_TEST_CHECK(ViewState::PRESSED.WasRemoved(prev, cur));
  DALI_TEST_CHECK(!ViewState::FOCUSED.WasRemoved(prev, cur));
  DALI_TEST_CHECK(!ViewState::DISABLED.WasRemoved(prev, cur));

  END_TEST;
}

int UtcDaliViewStateWasChanged(void)
{
  ViewState prev = ViewState::FOCUSED;
  ViewState cur  = ViewState::PRESSED;

  // Focused was removed → changed
  DALI_TEST_CHECK(ViewState::FOCUSED.WasChanged(prev, cur));
  // Pressed was added → changed
  DALI_TEST_CHECK(ViewState::PRESSED.WasChanged(prev, cur));
  // Disabled untouched → not changed
  DALI_TEST_CHECK(!ViewState::DISABLED.WasChanged(prev, cur));

  END_TEST;
}

int UtcDaliViewStateWasChangedUnchanged(void)
{
  ViewState state = ViewState::FOCUSED;
  DALI_TEST_CHECK(!ViewState::FOCUSED.WasChanged(state, state));

  END_TEST;
}

int UtcDaliViewStateAnyChangedComposite(void)
{
  ViewState prev      = ViewState::FOCUSED;
  ViewState cur       = ViewState::FOCUSED + ViewState::PRESSED;
  ViewState watchMask = ViewState::PRESSED + ViewState::SELECTED;

  // Pressed bit toggled within watchMask → true
  DALI_TEST_CHECK(watchMask.AnyChanged(prev, cur));

  END_TEST;
}

int UtcDaliViewStateAnyChangedNoOverlap(void)
{
  ViewState prev      = ViewState::FOCUSED;
  ViewState cur       = ViewState::PRESSED;
  ViewState watchMask = ViewState::DISABLED + ViewState::SELECTED; // no overlap with transition

  DALI_TEST_CHECK(!watchMask.AnyChanged(prev, cur));

  END_TEST;
}

int UtcDaliViewStateAnyChangedNoTransition(void)
{
  ViewState state     = ViewState::FOCUSED + ViewState::PRESSED;
  ViewState watchMask = ViewState::FOCUSED + ViewState::PRESSED;

  DALI_TEST_CHECK(!watchMask.AnyChanged(state, state));

  END_TEST;
}

// =============================================================================
// Idempotent Combination
// =============================================================================

int UtcDaliViewStateAddSameStateIsIdempotent(void)
{
  ViewState result = ViewState::FOCUSED + ViewState::FOCUSED;
  DALI_TEST_CHECK(result == ViewState::FOCUSED);

  END_TEST;
}

int UtcDaliViewStateRemoveSameStateGivesNormal(void)
{
  ViewState result = ViewState::FOCUSED - ViewState::FOCUSED;
  DALI_TEST_CHECK(result.IsNormal());

  END_TEST;
}
