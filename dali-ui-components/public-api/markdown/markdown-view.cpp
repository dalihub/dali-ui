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
#include <dali-ui-components/public-api/markdown/markdown-view.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/markdown/markdown-view-impl.h>

namespace Dali
{
namespace Ui
{

MarkdownView::MarkdownView() = default;

MarkdownView::~MarkdownView() = default;

MarkdownView MarkdownView::New()
{
  return Internal::MarkdownViewImpl::New();
}

MarkdownView MarkdownView::DownCast(BaseHandle handle)
{
  MarkdownView result;
  Ui::View     view = Ui::View::DownCast(handle);
  if(view)
  {
    CustomActorImpl&            customImpl = view.GetImplementation();
    Internal::MarkdownViewImpl* impl       = dynamic_cast<Internal::MarkdownViewImpl*>(&customImpl);
    if(impl)
    {
      result = MarkdownView(customImpl.GetOwner());
    }
  }
  return result;
}

Dali::String MarkdownView::ToPlainText(const Dali::String& markdown)
{
  Internal::MarkdownParser::Options options = Internal::MarkdownParser::Options::PlainText();
  Internal::MarkdownParser          parser;
  return Internal::MarkdownSnapshotToPlainText(parser.Parse(markdown, 0u, options));
}

MarkdownView::MarkdownView(const MarkdownView& handle)
: View(handle)
{
}

MarkdownView::MarkdownView(MarkdownView&& rhs) noexcept
: View(std::move(rhs))
{
}

MarkdownView& MarkdownView::operator=(const MarkdownView& handle)
{
  if(&handle != this)
  {
    Ui::View::operator=(handle);
  }
  return *this;
}

MarkdownView& MarkdownView::operator=(MarkdownView&& rhs) noexcept
{
  Ui::View::operator=(std::move(rhs));
  return *this;
}

void MarkdownView::SetMarkdown(const Dali::String& markdown)
{
  Internal::GetImpl(*this).SetMarkdown(markdown);
}

Dali::String MarkdownView::GetMarkdown() const
{
  return Internal::GetImpl(*this).GetMarkdown();
}

void MarkdownView::Clear()
{
  Internal::GetImpl(*this).Clear();
}

MarkdownView::MarkdownView(Internal::MarkdownViewImpl& implementation)
: View(implementation)
{
}

MarkdownView::MarkdownView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Internal::MarkdownViewImpl>(internal);
}

} // namespace Ui
} // namespace Dali
