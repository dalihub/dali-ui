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
#include "sample-common.h"

// EXTERNAL INCLUDES
#include <iomanip>
#include <iostream>
#include <sstream>

namespace GeometryTouchGestureSample
{
namespace
{
constexpr std::size_t MAXIMUM_LOG_LINES = 9u;
}

EventLog::EventLog() = default;

EventLog::~EventLog() = default;

void EventLog::Attach(Dali::Ui::Label label)
{
  mLabel = label;
  Refresh();
}

void EventLog::Clear()
{
  mLines.clear();
  Refresh();
}

void EventLog::Add(const std::string& category, const std::string& message)
{
  std::ostringstream stream;
  stream << '[' << category << "] " << message;
  mLines.push_back(stream.str());
  while(mLines.size() > MAXIMUM_LOG_LINES)
  {
    mLines.pop_front();
  }

  std::cout << stream.str() << std::endl;
  Refresh();
}

void EventLog::AddTouch(const std::string& category, Dali::Actor callbackActor, const Dali::TouchEvent& touch, const std::string& result)
{
  std::ostringstream stream;
  stream << "callback=" << ActorName(callbackActor) << " time=" << touch.GetTime();
  for(uint32_t index = 0u; index < touch.GetPointCount(); ++index)
  {
    const Dali::Vector2& screen = touch.GetScreenPosition(index);
    const Dali::Vector2& local  = touch.GetLocalPosition(index);
    stream << " p" << index << "{device=" << touch.GetDeviceId(index)
           << " state=" << PointStateToString(touch.GetState(index))
           << " hit=" << ActorName(touch.GetHitActor(index))
           << std::fixed << std::setprecision(0)
           << " screen=(" << screen.x << ',' << screen.y << ')'
           << " local=(" << local.x << ',' << local.y << ")}";
  }
  if(!result.empty())
  {
    stream << ' ' << result;
  }
  Add(category, stream.str());
}

void EventLog::Refresh()
{
  if(!mLabel)
  {
    return;
  }

  std::ostringstream stream;
  for(const std::string& line : mLines)
  {
    stream << line << '\n';
  }
  mLabel.SetText(stream.str().c_str());
}

Scenario::Scenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
: mEventLog(eventLog),
  mCanvasSize(canvasSize)
{
}

Scenario::~Scenario() = default;

bool Scenario::OnSceneTouch(const Dali::TouchEvent&, bool insideCanvas)
{
  return insideCanvas;
}

EventLog& Scenario::GetEventLog()
{
  return mEventLog;
}

const Dali::Vector2& Scenario::GetCanvasSize() const
{
  return mCanvasSize;
}

const char* PointStateToString(Dali::PointState::Type state)
{
  switch(state)
  {
    case Dali::PointState::STARTED:
      return "DOWN";
    case Dali::PointState::FINISHED:
      return "UP";
    case Dali::PointState::MOTION:
      return "MOTION";
    case Dali::PointState::LEAVE:
      return "LEAVE";
    case Dali::PointState::STATIONARY:
      return "STATIONARY";
    case Dali::PointState::INTERRUPTED:
      return "INTERRUPTED";
  }
  return "UNKNOWN";
}

const char* GestureStateToString(Dali::GestureState state)
{
  switch(state)
  {
    case Dali::GestureState::CLEAR:
      return "CLEAR";
    case Dali::GestureState::STARTED:
      return "STARTED";
    case Dali::GestureState::CONTINUING:
      return "CONTINUING";
    case Dali::GestureState::FINISHED:
      return "FINISHED";
    case Dali::GestureState::CANCELLED:
      return "CANCELLED";
    case Dali::GestureState::POSSIBLE:
      return "POSSIBLE";
  }
  return "UNKNOWN";
}

std::string ActorName(Dali::Actor actor)
{
  if(!actor)
  {
    return "<none>";
  }

  const Dali::String name = actor.GetProperty<Dali::String>(Dali::Actor::Property::NAME);
  if(name.Empty())
  {
    std::ostringstream stream;
    stream << "Actor#" << actor.GetProperty<int32_t>(Dali::Actor::Property::ID);
    return stream.str();
  }
  return name.CStr();
}

Dali::Ui::Label CreateLabel(const std::string& text,
                            float              x,
                            float              y,
                            float              width,
                            float              height,
                            float              fontSize,
                            Dali::Vector4      textColor)
{
  Dali::Ui::Label label = Dali::Ui::Label::New(text.c_str());
  label.SetRequestedX(x);
  label.SetRequestedY(y);
  label.SetRequestedWidth(width);
  label.SetRequestedHeight(height);
  label.SetFontSize(fontSize);
  label.SetTextColor(textColor);
  label.SetMultiLine(true);
  label.SetHorizontalTextAlignment(Dali::Ui::Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Dali::Ui::Text::Alignment::CENTER);
  return label;
}

Dali::Ui::View CreateTestView(const std::string& name,
                              const std::string& labelText,
                              Dali::Vector4      color,
                              float              x,
                              float              y,
                              float              width,
                              float              height)
{
  Dali::Ui::View view = Dali::Ui::View::New();
  view.SetProperty(Dali::Actor::Property::NAME, name.c_str());
  view.SetBackgroundColor(color);
  view.SetRequestedX(x);
  view.SetRequestedY(y);
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);

  Dali::Ui::Label label = CreateLabel(labelText, 0.0f, 0.0f, width, height, 13.0f);
  label.SetProperty(Dali::Actor::Property::SENSITIVE, false);
  view.Add(label);
  return view;
}

Dali::Ui::InteractiveView CreateButton(Dali::ConnectionTracker*     tracker,
                                       const std::string&           text,
                                       float                        x,
                                       float                        y,
                                       float                        width,
                                       float                        height,
                                       const std::function<void()>& callback,
                                       Dali::Vector4                color)
{
  Dali::Ui::InteractiveView button = Dali::Ui::InteractiveView::New();
  button.SetBackgroundColor(color);
  button.SetRequestedX(x);
  button.SetRequestedY(y);
  button.SetRequestedWidth(width);
  button.SetRequestedHeight(height);

  Dali::Ui::Label label = CreateLabel(text, 0.0f, 0.0f, width, height, 12.0f);
  label.SetProperty(Dali::Actor::Property::SENSITIVE, false);
  button.Add(label);
  button.ClickedSignal().Connect(tracker, [callback](Dali::Ui::View, Dali::Ui::InputEvent)
  {
    callback();
  });
  return button;
}

} // namespace GeometryTouchGestureSample
