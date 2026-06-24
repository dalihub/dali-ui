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
#include <dali-ui-foundation/public-api/ui-style-sheet.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <cstddef>
#include <vector>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
enum class StyleEntryState
{
  UNRESOLVED,
  RESOLVING,
  RESOLVED
};

struct StyleEntry
{
  mutable UiStyleCreator  creator{nullptr};
  mutable UiStyle         style;
  mutable StyleEntryState state{StyleEntryState::UNRESOLVED};
};

// UiStyleKey is allocated sequentially, and expected style entry counts are
// small: roughly 20-200 entries, with about 500 as a generous upper bound.
// Dense vector indexing keeps lookup simple and fast for that range.
using StyleEntryContainer = std::vector<StyleEntry>;
} //namespace

class UiStyleSheetImpl : public BaseObject
{
public:
  void SetStyle(UiStyleKey key, UiStyleCreator creator)
  {
    DALI_ASSERT_ALWAYS(!mFrozen && "UiStyleSheet is frozen");
    DALI_ASSERT_ALWAYS(creator && "UiStyleCreator must not be null");

    const auto index = static_cast<std::size_t>(key.value);
    if(index >= mEntries.size())
    {
      mEntries.resize(index + 1u);
    }

    StyleEntry& entry = mEntries[index];
    entry.creator     = creator;
    entry.style       = UiStyle();
    entry.state       = StyleEntryState::UNRESOLVED;
  }

  UiStyle GetStyle(UiStyleKey key) const
  {
    const auto index = static_cast<std::size_t>(key.value);
    if(index >= mEntries.size())
    {
      return UiStyle();
    }

    const StyleEntry& entry = mEntries[index];
    if(entry.state == StyleEntryState::RESOLVING)
    {
      DALI_ASSERT_ALWAYS(false && "UiStyleSheet detected recursive style resolution");
    }

    if(entry.state == StyleEntryState::UNRESOLVED && entry.creator)
    {
      entry.state   = StyleEntryState::RESOLVING;
      entry.style   = entry.creator();
      entry.creator = nullptr;
      entry.state   = StyleEntryState::RESOLVED;
    }

    return entry.style;
  }

  void Freeze()
  {
    mFrozen = true;
  }

  bool IsFrozen() const
  {
    return mFrozen;
  }

protected:
  ~UiStyleSheetImpl() override = default;

private:
  StyleEntryContainer mEntries;
  bool                mFrozen{false};
};

} // namespace Internal
namespace
{

Internal::UiStyleSheetImpl& GetStyleSheetImpl(UiStyleSheet& styleSheet)
{
  BaseObject& handle = styleSheet.GetBaseObject();
  return static_cast<Internal::UiStyleSheetImpl&>(handle);
}

const Internal::UiStyleSheetImpl& GetStyleSheetImpl(const UiStyleSheet& styleSheet)
{
  const BaseObject& handle = styleSheet.GetBaseObject();
  return static_cast<const Internal::UiStyleSheetImpl&>(handle);
}

} // namespace

UiStyleSheet UiStyleSheet::New()
{
  IntrusivePtr<Internal::UiStyleSheetImpl> impl(new Internal::UiStyleSheetImpl());
  return UiStyleSheet(impl.Get());
}

void UiStyleSheet::SetStyle(UiStyleKey key, UiStyleCreator creator)
{
  DALI_ASSERT_ALWAYS(*this && "UiStyleSheet is uninitialized");
  GetStyleSheetImpl(*this).SetStyle(key, creator);
}

UiStyle UiStyleSheet::GetStyle(UiStyleKey key) const
{
  if(!*this)
  {
    return UiStyle();
  }

  return GetStyleSheetImpl(*this).GetStyle(key);
}

bool UiStyleSheet::IsFrozen() const
{
  return *this && GetStyleSheetImpl(*this).IsFrozen();
}

void UiStyleSheet::Freeze()
{
  DALI_ASSERT_ALWAYS(*this && "UiStyleSheet is uninitialized");
  GetStyleSheetImpl(*this).Freeze();
}

UiStyleSheet::UiStyleSheet(Internal::UiStyleSheetImpl* impl)
: BaseHandle(impl)
{
}

} // namespace Ui
} // namespace Dali
