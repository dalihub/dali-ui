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
#include <dali-ui-foundation/public-api/page-scroll-view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/page-scroll-view-impl.h>

namespace Dali
{

namespace Ui
{

namespace
{
inline Integration::PageScrollViewImpl& GetImpl(PageScrollView& view)
{
  DALI_ASSERT_ALWAYS(view);
  Dali::RefObject& handle = view.GetImplementation();
  return static_cast<Integration::PageScrollViewImpl&>(handle);
}

inline const Integration::PageScrollViewImpl& GetImpl(const PageScrollView& view)
{
  DALI_ASSERT_ALWAYS(view);
  const Dali::RefObject& handle = view.GetImplementation();
  return static_cast<const Integration::PageScrollViewImpl&>(handle);
}
} // namespace

PageScrollView::PageScrollView() = default;

PageScrollView PageScrollView::New()
{
  Integration::PageScrollViewImplPtr impl = Integration::PageScrollViewImpl::New();
  PageScrollView                     view(*impl);
  impl->Initialize();
  return view;
}

PageScrollView::PageScrollView(const PageScrollView& rhs)
: ScrollView(rhs)
{
}

PageScrollView::PageScrollView(PageScrollView&& rhs) noexcept
: ScrollView(std::move(rhs))
{
}

PageScrollView::~PageScrollView() = default;

PageScrollView& PageScrollView::operator=(const PageScrollView& rhs)
{
  if(&rhs != this)
  {
    ScrollView::operator=(rhs);
  }
  return *this;
}

PageScrollView& PageScrollView::operator=(PageScrollView&& rhs) noexcept
{
  ScrollView::operator=(std::move(rhs));
  return *this;
}

PageScrollView PageScrollView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<PageScrollView, Integration::PageScrollViewImpl>(handle);
}

PageScrollView::PageScrollView(Integration::PageScrollViewImpl& impl)
: ScrollView(impl)
{
}

PageScrollView::PageScrollView(Dali::Internal::CustomActor* internal)
: ScrollView(internal)
{
  VerifyCustomActorPointer<Integration::PageScrollViewImpl>(internal);
}

void PageScrollView::SetPageSize(const Vector2& size)
{
  GetImpl(*this).SetPageSize(size);
}

Vector2 PageScrollView::GetPageSize() const
{
  return GetImpl(*this).GetPageSize();
}

int PageScrollView::GetCurrentPage() const
{
  return GetImpl(*this).GetCurrentPage();
}

int PageScrollView::GetPageCount() const
{
  return GetImpl(*this).GetPageCount();
}

void PageScrollView::ScrollToPage(int page, bool animate)
{
  GetImpl(*this).ScrollToPage(page, animate);
}

IPageScrollable::PageChangedSignalType& PageScrollView::PageChangedSignal()
{
  return GetImpl(*this).PageChangedSignal();
}

IPageScrollable::DestroyingSignalType& PageScrollView::DestroyingSignal()
{
  return GetImpl(*this).DestroyingSignal();
}

void PageScrollView::NotifyPagesInserted(int atIndex, int insertedCount)
{
  GetImpl(*this).NotifyPagesInserted(atIndex, insertedCount);
}

void PageScrollView::NotifyPagesRemoved(int atIndex, int removedCount)
{
  GetImpl(*this).NotifyPagesRemoved(atIndex, removedCount);
}

} // namespace Ui

} // namespace Dali
