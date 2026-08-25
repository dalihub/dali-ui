#ifndef DALI_UI_SAMPLE_GEOMETRY_TOUCH_GESTURE_SAMPLE_COMMON_H
#define DALI_UI_SAMPLE_GEOMETRY_TOUCH_GESTURE_SAMPLE_COMMON_H

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

// EXTERNAL INCLUDES
#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>

namespace GeometryTouchGestureSample
{
constexpr float       DEFAULT_WINDOW_WIDTH    = 600.0f;
constexpr float       DEFAULT_WINDOW_HEIGHT   = 1080.0f;
constexpr float       NAVIGATION_HEIGHT       = 82.0f;
constexpr float       INSTRUCTION_HEIGHT      = 78.0f;
constexpr float       REFERENCE_CANVAS_HEIGHT = 630.0f;
constexpr float       DEFAULT_LOG_HEIGHT      = 400.0f;
constexpr float       MINIMUM_LOG_HEIGHT      = 240.0f;
constexpr std::size_t SCENARIO_COUNT          = 6u;

/**
 * @brief Maintains the bounded on-screen and standard-output diagnostic log.
 */
class EventLog
{
public:
  /** @brief Creates an empty diagnostic log. */
  EventLog();

  /** @brief Destroys the diagnostic log. */
  ~EventLog();

  EventLog(const EventLog&)            = delete;
  EventLog& operator=(const EventLog&) = delete;

  /** @brief Attaches the label that displays the buffered entries. */
  void Attach(Dali::Ui::Label label);

  /** @brief Removes all buffered entries. */
  void Clear();

  /** @brief Appends a categorized text entry. */
  void Add(const std::string& category, const std::string& message);

  /** @brief Extracts every point from a touch event and immediately appends it. */
  void AddTouch(const std::string& category, Dali::Actor callbackActor, const Dali::TouchEvent& touch, const std::string& result);

private:
  /** @brief Rebuilds the attached label from the bounded entry buffer. */
  void Refresh();

private:
  Dali::Ui::Label         mLabel;
  std::deque<std::string> mLines;
};

/**
 * @brief Defines the lifecycle and presentation contract for one diagnostic page.
 */
class Scenario : public Dali::ConnectionTracker
{
public:
  /** @brief Creates a scenario that writes to the supplied diagnostic log. */
  Scenario(EventLog& eventLog, const Dali::Vector2& canvasSize);

  /** @brief Destroys the scenario and disconnects its tracked signals. */
  ~Scenario() override;

  Scenario(const Scenario&)            = delete;
  Scenario& operator=(const Scenario&) = delete;

  /** @brief Returns the page title displayed by the shell. */
  virtual const char* GetTitle() const = 0;

  /** @brief Returns the concise interaction instructions displayed by the shell. */
  virtual const char* GetInstructions() const = 0;

  /** @brief Creates and connects a fresh Actor tree for the page. */
  virtual Dali::Ui::View Build() = 0;

  /** @brief Observes a Scene-level touch boundary without retaining the event. */
  virtual bool OnSceneTouch(const Dali::TouchEvent& touch, bool insideCanvas);

protected:
  /** @brief Returns the shared diagnostic log. */
  EventLog& GetEventLog();

  /** @brief Returns the current test canvas size. */
  const Dali::Vector2& GetCanvasSize() const;

private:
  EventLog&     mEventLog;
  Dali::Vector2 mCanvasSize;
};

/** @brief Converts a touch point state into the wording used by the event log. */
const char* PointStateToString(Dali::PointState::Type state);

/** @brief Converts a gesture state into the wording used by the event log. */
const char* GestureStateToString(Dali::GestureState state);

/** @brief Returns an Actor's diagnostic name, or a stable fallback based on its ID. */
std::string ActorName(Dali::Actor actor);

/** @brief Creates a positioned text label with the sample's common defaults. */
Dali::Ui::Label CreateLabel(const std::string& text,
                            float              x,
                            float              y,
                            float              width,
                            float              height,
                            float              fontSize,
                            Dali::Vector4      textColor = Dali::Color::WHITE);

/** @brief Creates a plain View used as a geometry hit-test target. */
Dali::Ui::View CreateTestView(const std::string& name,
                              const std::string& labelText,
                              Dali::Vector4      color,
                              float              x,
                              float              y,
                              float              width,
                              float              height);

/** @brief Creates a navigation or configuration button with a tracked callback. */
Dali::Ui::InteractiveView CreateButton(Dali::ConnectionTracker*     tracker,
                                       const std::string&           text,
                                       float                        x,
                                       float                        y,
                                       float                        width,
                                       float                        height,
                                       const std::function<void()>& callback,
                                       Dali::Vector4                color = Dali::Vector4(0.18f, 0.25f, 0.34f, 1.0f));

/** @brief Creates a fresh diagnostic page for the zero-based scenario index. */
std::unique_ptr<Scenario> CreateScenario(std::size_t index, EventLog& eventLog, const Dali::Vector2& canvasSize);

} // namespace GeometryTouchGestureSample

#endif // DALI_UI_SAMPLE_GEOMETRY_TOUCH_GESTURE_SAMPLE_COMMON_H
