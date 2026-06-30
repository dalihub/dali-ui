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
#include <dali-ui-foundation/internal/drag-drop-detector/drag-and-drop-detector-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/events/point-state.h>
#include <dali/public-api/events/touch-event.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/types/ui-color.h>

using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Internal
{
Dali::Ui::Integration::DragAndDropDetector DragAndDropDetector::New()
{
  Dali::Ui::Integration::DragAndDropDetector detector = Dali::Ui::Integration::DragAndDropDetector(new DragAndDropDetector());

  return detector;
}

void DragAndDropDetector::Attach(Dali::Ui::View& view)
{
  if(view)
  {
    if(!mViews.empty())
    {
      auto match = std::find(mViews.begin(), mViews.end(), view);
      if(match != mViews.end())
      {
        return;
      }
    }
    mViews.push_back(view);
    view.TouchEventSignal().Connect(this, &DragAndDropDetector::OnDrag);
    mFirstEnter.push_back(view.GetProperty<int>(Actor::Property::ID));
    mPanGestureDetector.Attach(view);
    mPanGestureDetector.DetectedSignal().Connect(this, &DragAndDropDetector::OnPan);
  }
}

void DragAndDropDetector::Detach(Dali::Ui::View& view)
{
  if(!mViews.empty())
  {
    if(!view)
    {
      return;
    }

    auto match = std::find(mViews.begin(), mViews.end(), view);

    if(match != mViews.end())
    {
      match->TouchEventSignal().Disconnect(this, &DragAndDropDetector::OnDrag);
      mPanGestureDetector.Detach(*match);
      mFirstEnter.erase(
        std::find(mFirstEnter.begin(), mFirstEnter.end(), view.GetProperty<int>(Actor::Property::ID)));
      mViews.erase(match);
    }
  }
}

void DragAndDropDetector::DetachAll()
{
  if(!mViews.empty())
  {
    auto iter = mViews.begin();
    for(; iter != mViews.end();)
    {
      iter->TouchEventSignal().Disconnect(this, &DragAndDropDetector::OnDrag);
      mPanGestureDetector.Detach(*iter);
      iter = mViews.erase(iter);
    }
  }

  if(!mFirstEnter.empty())
  {
    auto iter = mFirstEnter.begin();
    for(; iter != mFirstEnter.end();)
    {
      iter = mFirstEnter.erase(iter);
    }
  }
}

uint32_t DragAndDropDetector::GetAttachedViewCount() const
{
  return mViews.size();
}

Dali::Ui::View DragAndDropDetector::GetAttachedView(uint32_t index) const
{
  Dali::Ui::View view;

  if(index < mViews.size())
  {
    view = mViews[index];
  }

  return view;
}

void DragAndDropDetector::OnPan(Dali::Actor actor, PanGesture gesture)
{
  Dali::Ui::View view = Dali::Ui::View::DownCast(actor);

  GestureState state = gesture.GetState();

  if(state == GestureState::STARTED)
  {
    mDragLocalPosition = gesture.GetPosition();
    mPointDown         = true;
    mDragView          = view;
    mFirstEnter.clear();
    for(auto&& view : mViews)
    {
      mFirstEnter.push_back(view.GetProperty<int>(Actor::Property::ID));
    }
    float   width    = view.GetProperty<float>(Dali::Actor::Property::SIZE_WIDTH);
    float   height   = view.GetProperty<float>(Dali::Actor::Property::SIZE_HEIGHT);
    Vector3 actorPos = view.GetProperty<Vector3>(Dali::Actor::Property::POSITION);

    mShadowView = Dali::Ui::View::New();
    mShadowView.SetProperty(Actor::Property::POSITION, actorPos);
    mShadowView.SetProperty(Actor::Property::SIZE, Vector2(width, height));
    mShadowView.SetBackgroundColor(UiColor(0.3f, 0.3f, 0.3f, 0.7f));
    mShadowView.SetProperty(Actor::Property::PARENT_ORIGIN,
                            view.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN));
    mShadowView.SetProperty(Actor::Property::PIVOT,
                            view.GetCurrentProperty<Vector3>(Actor::Property::PIVOT));
    view.GetParent().Add(mShadowView);
    SetPosition(gesture.GetScreenPosition());
    EmitStartedSignal(view);
  }
  if(state == GestureState::CONTINUING)
  {
    Vector2 screenPosition = gesture.GetScreenPosition();
    view.GetParent().ScreenToLocal(mLocalPosition.x, mLocalPosition.y, screenPosition.x, screenPosition.y);
    mShadowView.SetProperty(Actor::Property::POSITION, Vector2(mLocalPosition.x - mDragLocalPosition.x,
                                                               mLocalPosition.y - mDragLocalPosition.y));
  }
  if(state == GestureState::FINISHED)
  {
    mDragView.GetParent().Remove(mShadowView);
    EmitEndedSignal(view);
  }
}

bool DragAndDropDetector::OnDrag(Dali::Actor actor, Dali::TouchEvent data)
{
  Dali::Ui::View   view = Dali::Ui::View::DownCast(actor);
  PointState::Type type = data.GetState(0);

  if(type == PointState::MOTION)
  {
    if(mDragView != view && mPointDown)
    {
      auto found = std::find(mFirstEnter.begin(), mFirstEnter.end(), view.GetProperty<int>(Actor::Property::ID));
      if(mFirstEnter.end() != found)
      {
        SetPosition(data.GetScreenPosition(0));
        mFirstEnter.erase(found);
        EmitEnteredSignal(view);
      }
      else
      {
        SetPosition(data.GetScreenPosition(0));
        EmitMovedSignal(view);
      }
    }
  }

  if(type == PointState::LEAVE)
  {
    if(mDragView != view && mPointDown)
    {
      mFirstEnter.push_back(view.GetProperty<int>(Actor::Property::ID));
      EmitExitedSignal(view);
    }
  }

  if(type == PointState::UP)
  {
    if(mDragView != view && mPointDown)
    {
      SetPosition(data.GetScreenPosition(0));
      ClearContent();
      SetContent(ToStdString(mDragView.GetProperty(Dali::Actor::Property::NAME)));
      EmitDroppedSignal(view);
    }

    if(mShadowView)
    {
      view.GetParent().Remove(mShadowView);
    }
    mPointDown = false;
  }
  return false;
}

const std::string& DragAndDropDetector::GetContent() const
{
  return mContent;
}

const Vector2& DragAndDropDetector::GetCurrentScreenPosition() const
{
  return mScreenPosition;
}

void DragAndDropDetector::SetContent(const std::string& content)
{
  mContent = content;
}

void DragAndDropDetector::ClearContent()
{
  mContent.clear();
}

void DragAndDropDetector::SetPosition(const Vector2& screenPosition)
{
  mScreenPosition = screenPosition;
}

void DragAndDropDetector::EmitStartedSignal(Dali::Ui::View& view)
{
  if(!mStartedSignal.Empty())
  {
    Dali::Ui::Integration::DragAndDropDetector handle(this);
    mStartedSignal.Emit(view, handle);
  }
}
void DragAndDropDetector::EmitEnteredSignal(Dali::Ui::View& view)
{
  if(!mEnteredSignal.Empty())
  {
    Dali::Ui::Integration::DragAndDropDetector handle(this);
    mEnteredSignal.Emit(view, handle);
  }
}

void DragAndDropDetector::EmitExitedSignal(Dali::Ui::View& view)
{
  if(!mExitedSignal.Empty())
  {
    Dali::Ui::Integration::DragAndDropDetector handle(this);
    mExitedSignal.Emit(view, handle);
  }
}

void DragAndDropDetector::EmitMovedSignal(Dali::Ui::View& view)
{
  if(!mMovedSignal.Empty())
  {
    Dali::Ui::Integration::DragAndDropDetector handle(this);
    mMovedSignal.Emit(view, handle);
  }
}

void DragAndDropDetector::EmitDroppedSignal(Dali::Ui::View& view)
{
  if(!mDroppedSignal.Empty())
  {
    Dali::Ui::Integration::DragAndDropDetector handle(this);
    mDroppedSignal.Emit(view, handle);
  }
}

void DragAndDropDetector::EmitEndedSignal(Dali::Ui::View& view)
{
  if(!mEndedSignal.Empty())
  {
    Dali::Ui::Integration::DragAndDropDetector handle(this);
    mEndedSignal.Emit(view, handle);
  }
}

DragAndDropDetector::DragAndDropDetector()
: mContent(),
  mScreenPosition()
{
  mPanGestureDetector = Dali::PanGestureDetector::New();
  mPointDown          = false;
}

DragAndDropDetector::~DragAndDropDetector()
{
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
