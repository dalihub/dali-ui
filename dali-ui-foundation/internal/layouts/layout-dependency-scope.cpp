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
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace LayoutDependency
{
namespace
{
/// Innermost active owner frame, or nullptr when no owner scope is open. Thread-local
/// because a layout pass and every nested Measure() it issues run synchronously on the
/// same (event) thread. A raw pointer with a constant initializer: no dynamic init and
/// no heap allocation; the frames it links are members of stack-resident scope objects.
thread_local Frame* gLayoutOwnerTop = nullptr;

} // namespace

const Frame* Top()
{
  return gLayoutOwnerTop;
}

// The constructors and destructors below are deliberately out-of-line. Release builds
// compile with -fvisibility=hidden, so an inline push/pop would give every DSO that
// includes this header its own copy of gLayoutOwnerTop and silently split the stack.

ArrangeOwnedMeasureScope::ArrangeOwnedMeasureScope(ViewImpl* owner)
: mFrame{owner, nullptr, gLayoutOwnerTop, OwnerKind::ARRANGE, false}
{
  if(owner)
  {
    gLayoutOwnerTop = &mFrame;
  }
}

ArrangeOwnedMeasureScope::~ArrangeOwnedMeasureScope()
{
  // Restore the enclosing frame rather than clearing the stack: scopes nest, and this
  // must also be correct while unwinding. Never touch LayoutController from here.
  if(mFrame.owner)
  {
    gLayoutOwnerTop = mFrame.previous;
  }
}

RecyclerLayoutOwnerScope::RecyclerLayoutOwnerScope(ViewImpl* recycler, const void* layouterIdentity)
: mFrame{recycler, layouterIdentity, gLayoutOwnerTop, OwnerKind::RECYCLER, false}
{
  if(recycler)
  {
    gLayoutOwnerTop = &mFrame;
  }
}

RecyclerLayoutOwnerScope::~RecyclerLayoutOwnerScope()
{
  if(mFrame.owner)
  {
    gLayoutOwnerTop = mFrame.previous;
  }
}

} // namespace LayoutDependency
} // namespace Internal
} // namespace Ui
} // namespace Dali
