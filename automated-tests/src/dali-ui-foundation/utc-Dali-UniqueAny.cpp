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

#include <dali-ui-test-suite-utils.h>
#include <dali-ui-foundation/internal/common/attachment-container.h>
#include <dali-ui-foundation/public-api/attachment-id.h>
#include <dali-ui-foundation/public-api/unique-any.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

namespace
{
struct MoveOnly
{
  explicit MoveOnly(int v)
  : value(v)
  {
  }

  MoveOnly(MoveOnly&& rhs) noexcept
  : value(rhs.value)
  {
    rhs.value = 0;
  }

  MoveOnly& operator=(MoveOnly&& rhs) noexcept
  {
    value     = rhs.value;
    rhs.value = 0;
    return *this;
  }

  MoveOnly(const MoveOnly&)            = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;

  int value;
};
} // namespace

void utc_dali_unique_any_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_unique_any_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliUniqueAnyIdAllocP(void)
{
  AttachmentId id1 = AttachmentId::Alloc();
  AttachmentId id2 = AttachmentId::Alloc();

  DALI_TEST_CHECK(id1 != id2);
  END_TEST;
}

int UtcDaliUniqueAnyGetP(void)
{
  UniqueAny attachment(3);

  DALI_TEST_CHECK(attachment);
  DALI_TEST_CHECK(!attachment.Empty());

  int* value = attachment.Get<int>();
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(*value, 3, TEST_LOCATION);
  DALI_TEST_CHECK(!attachment.Get<float>());
  END_TEST;
}

int UtcDaliUniqueAnyResetP(void)
{
  UniqueAny attachment(3);

  DALI_TEST_CHECK(attachment);
  attachment.Reset();
  DALI_TEST_CHECK(!attachment);
  DALI_TEST_CHECK(attachment.Empty());
  DALI_TEST_CHECK(!attachment.Get<int>());
  END_TEST;
}

int UtcDaliUniqueAnyMoveOnlyP(void)
{
  UniqueAny attachment(MoveOnly(7));

  MoveOnly* value = attachment.Get<MoveOnly>();
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->value, 7, TEST_LOCATION);
  END_TEST;
}

int UtcDaliUniqueAnyDetachP(void)
{
  UniqueAny attachment(MoveOnly(17));

  Dali::UniquePtr<int> mismatch = attachment.Detach<int>();
  DALI_TEST_CHECK(!mismatch.Get());
  DALI_TEST_CHECK(attachment.Get<MoveOnly>());

  Dali::UniquePtr<MoveOnly> value = attachment.Detach<MoveOnly>();
  DALI_TEST_CHECK(value.Get());
  DALI_TEST_EQUALS(value->value, 17, TEST_LOCATION);
  DALI_TEST_CHECK(!attachment.Get<MoveOnly>());
  END_TEST;
}

int UtcDaliUniqueAnyContainerSetGetP(void)
{
  AttachmentContainer container;
  AttachmentId        id = AttachmentId::Alloc();

  container.SetAttachment(id, UniqueAny(Dali::MakeUnique<int>(5)));

  UniqueAny*           attachment = container.GetAttachment(id);
  Dali::UniquePtr<int>* value      = attachment ? attachment->Get<Dali::UniquePtr<int>>() : nullptr;
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get());
  DALI_TEST_EQUALS(**value, 5, TEST_LOCATION);
  END_TEST;
}

int UtcDaliUniqueAnyContainerReplaceP(void)
{
  AttachmentContainer container;
  AttachmentId        id = AttachmentId::Alloc();

  container.SetAttachment(id, UniqueAny(Dali::MakeUnique<int>(5)));
  container.SetAttachment(id, UniqueAny(Dali::MakeUnique<int>(9)));

  UniqueAny*           attachment = container.GetAttachment(id);
  Dali::UniquePtr<int>* value      = attachment ? attachment->Get<Dali::UniquePtr<int>>() : nullptr;
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get());
  DALI_TEST_EQUALS(**value, 9, TEST_LOCATION);
  END_TEST;
}

int UtcDaliUniqueAnyContainerRemoveP(void)
{
  AttachmentContainer container;
  AttachmentId        id = AttachmentId::Alloc();

  container.SetAttachment(id, UniqueAny(Dali::MakeUnique<int>(5)));

  DALI_TEST_CHECK(container.RemoveAttachment(id));
  DALI_TEST_CHECK(!container.GetAttachment(id));
  DALI_TEST_CHECK(!container.RemoveAttachment(id));
  END_TEST;
}

int UtcDaliUniqueAnyContainerDetachP(void)
{
  AttachmentContainer container;
  AttachmentId        id = AttachmentId::Alloc();

  container.SetAttachment(id, UniqueAny(Dali::MakeUnique<int>(19)));

  UniqueAny detached = container.DetachAttachment(id);
  DALI_TEST_CHECK(!detached.Empty());
  DALI_TEST_CHECK(!container.GetAttachment(id));

  Dali::UniquePtr<Dali::UniquePtr<int>> storedValue = detached.Detach<Dali::UniquePtr<int>>();
  DALI_TEST_CHECK(storedValue);
  Dali::UniquePtr<int> value = Dali::Move(*storedValue);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(*value, 19, TEST_LOCATION);

  DALI_TEST_CHECK(container.DetachAttachment(id).Empty());
  END_TEST;
}

int UtcDaliUniqueAnyContainerMissingP(void)
{
  AttachmentContainer container;
  AttachmentId        id = AttachmentId::Alloc();

  DALI_TEST_CHECK(!container.GetAttachment(id));
  END_TEST;
}
