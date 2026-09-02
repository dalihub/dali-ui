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

#include <dali-ui-components/internal/window/window-frame-decoration-validator.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

int UtcDaliWindowFrameValidateEmptyTopology(void)
{
  DecorationTopology empty;
  DALI_TEST_CHECK(ValidateDecorationTopology(empty) == WindowFrameDecorationResult::INSTALLED);
  END_TEST;
}

int UtcDaliWindowFrameValidateOwnedRoles(void)
{
  DecorationTopology valid;
  valid.slotIds    = {10u, 20u, 30u, 40u, 50u, 60u};
  valid.roleIds    = {21u, 22u, 23u, 24u, 50u, 60u};
  valid.roleOwners = {1, 1, 1, 1, 4, 5};
  DALI_TEST_CHECK(ValidateDecorationTopology(valid) == WindowFrameDecorationResult::INSTALLED);
  END_TEST;
}

int UtcDaliWindowFrameValidateParentedSlotN(void)
{
  DecorationTopology parented;
  parented.hasParentedSlot = true;
  DALI_TEST_CHECK(ValidateDecorationTopology(parented) == WindowFrameDecorationResult::SLOT_ALREADY_PARENTED);
  END_TEST;
}

int UtcDaliWindowFrameValidateDuplicateSlotN(void)
{
  DecorationTopology duplicate;
  duplicate.slotIds = {10u, 20u, 10u, 0u, 0u, 0u};
  DALI_TEST_CHECK(ValidateDecorationTopology(duplicate) == WindowFrameDecorationResult::DUPLICATE_SLOT);
  END_TEST;
}

int UtcDaliWindowFrameValidateDuplicateRoleN(void)
{
  DecorationTopology duplicate;
  duplicate.slotIds    = {10u, 20u, 0u, 0u, 0u, 0u};
  duplicate.roleIds    = {21u, 21u, 0u, 0u, 0u, 0u};
  duplicate.roleOwners = {1, 1, -1, -1, -1, -1};
  DALI_TEST_CHECK(ValidateDecorationTopology(duplicate) == WindowFrameDecorationResult::DUPLICATE_ROLE);
  END_TEST;
}

int UtcDaliWindowFrameValidateRoleOutsideDecorationN(void)
{
  // A role reached from outside any slot, and a role whose owner was never
  // resolved, are both rejected the same way.
  DecorationTopology external;
  external.slotIds    = {10u, 0u, 0u, 0u, 0u, 0u};
  external.roleIds    = {99u, 0u, 0u, 0u, 0u, 0u};
  external.roleOwners = {-2, -1, -1, -1, -1, -1};

  DecorationTopology missingOwner;
  missingOwner.slotIds = {10u, 0u, 0u, 0u, 0u, 0u};
  missingOwner.roleIds = {11u, 0u, 0u, 0u, 0u, 0u};

  DALI_TEST_CHECK(ValidateDecorationTopology(external) == WindowFrameDecorationResult::ROLE_OUTSIDE_DECORATION);
  DALI_TEST_CHECK(ValidateDecorationTopology(missingOwner) == WindowFrameDecorationResult::ROLE_OUTSIDE_DECORATION);
  END_TEST;
}
