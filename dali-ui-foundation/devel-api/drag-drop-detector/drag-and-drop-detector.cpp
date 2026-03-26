/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/devel-api/drag-drop-detector/drag-and-drop-detector.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/drag-drop-detector/drag-and-drop-detector-impl.h>

namespace Dali
{
namespace Ui
{
DragAndDropDetector::DragAndDropDetector()
{
}

DragAndDropDetector::~DragAndDropDetector()
{
}

DragAndDropDetector DragAndDropDetector::New()
{
  return Internal::DragAndDropDetector::New();
}

void DragAndDropDetector::Attach(View view)
{
  GetImplementation(*this).Attach(view);
}

void DragAndDropDetector::Detach(View view)
{
  GetImplementation(*this).Detach(view);
}

void DragAndDropDetector::DetachAll()
{
  GetImplementation(*this).DetachAll();
}

uint32_t DragAndDropDetector::GetAttachedViewCount() const
{
  return GetImplementation(*this).GetAttachedViewCount();
}

View DragAndDropDetector::GetAttachedView(uint32_t index) const
{
  return GetImplementation(*this).GetAttachedView(index);
}

const std::string& DragAndDropDetector::GetContent() const
{
  return GetImplementation(*this).GetContent();
}

const Vector2& DragAndDropDetector::GetCurrentScreenPosition() const
{
  return GetImplementation(*this).GetCurrentScreenPosition();
}

DragAndDropDetector::DragAndDropSignal& DragAndDropDetector::StartedSignal()
{
  return GetImplementation(*this).StartedSignal();
}

DragAndDropDetector::DragAndDropSignal& DragAndDropDetector::EnteredSignal()
{
  return GetImplementation(*this).EnteredSignal();
}

DragAndDropDetector::DragAndDropSignal& DragAndDropDetector::ExitedSignal()
{
  return GetImplementation(*this).ExitedSignal();
}

DragAndDropDetector::DragAndDropSignal& DragAndDropDetector::MovedSignal()
{
  return GetImplementation(*this).MovedSignal();
}

DragAndDropDetector::DragAndDropSignal& DragAndDropDetector::DroppedSignal()
{
  return GetImplementation(*this).DroppedSignal();
}

DragAndDropDetector::DragAndDropSignal& DragAndDropDetector::EndedSignal()
{
  return GetImplementation(*this).EndedSignal();
}

DragAndDropDetector::DragAndDropDetector(Internal::DragAndDropDetector* detector)
: BaseHandle(detector)
{
}

} // namespace Ui

} // namespace Dali
