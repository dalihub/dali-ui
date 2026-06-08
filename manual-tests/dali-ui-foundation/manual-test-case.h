/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#pragma once

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <functional>
#include <memory>
#include <vector>

namespace ManualTest
{

inline Dali::Ui::View MakeWeightedSpacer()
{
  Dali::Ui::View spacer = Dali::Ui::View::New();
  spacer.SetRequestedWidth(Dali::Ui::WRAP_CONTENT);
  spacer.SetLayoutParams(Dali::Ui::StackLayoutParams::New().SetWeight(1.0f));
  return spacer;
}

/**
 * @brief Abstract base class for all manual test cases.
 *
 * Each test case represents a single visual/interactive scenario that a
 * developer wants to verify by hand.  The launcher app discovers all
 * registered test cases at startup, lists them on screen, and navigates
 * into the selected one.
 *
 * --------------------------------------------------------------------------
 * How to write a new test case
 * --------------------------------------------------------------------------
 * 1. Create a new .cpp file under manual-tests/src/dali-ui-foundation/tc/
 *    (name it tc-<your-feature>.cpp).
 * 2. Include this header.
 * 3. Define a class that inherits from ManualTest::TestCase.
 * 4. Implement GetName(), GetDescription(), and OnEnter().
 * 5. Call REGISTER_MANUAL_TEST(YourClassName) at file scope.
 *
 * The CMakeLists.txt auto-discovers every .cpp in the tc/ directory, so no
 * build-file edits are needed when adding a new test case.
 *
 * --------------------------------------------------------------------------
 * Example
 * --------------------------------------------------------------------------
 * @code
 * #include "manual-test-case.h"
 *
 * using namespace Dali;
 * using namespace Dali::Ui;
 *
 * class TcMyFeature : public ManualTest::TestCase
 * {
 * public:
 *   Dali::String GetName() const override        { return "My Feature"; }
 *   Dali::String GetDescription() const override { return "Verifies my feature works"; }
 *
 *   void OnEnter(View contentArea) override
 *   {
 *     contentArea.Add(Label::New("My Feature is working!")
 *       .SetRequestedWidth(MATCH_PARENT)
 *       .SetRequestedHeight(MATCH_PARENT)
 *       .SetHorizontalTextAlignment(Text::Alignment::CENTER)
 *       .SetVerticalTextAlignment(Text::Alignment::CENTER));
 *   }
 * };
 *
 * REGISTER_MANUAL_TEST(TcMyFeature)
 * @endcode
 */
class TestCase
{
public:
  virtual ~TestCase() = default;

  /**
   * @brief Returns the display name shown in the TC list.
   */
  virtual Dali::String GetName() const = 0;

  /**
   * @brief Returns a short description of what this TC verifies.
   *
   * Shown as a subtitle in the TC list.
   */
  virtual Dali::String GetDescription() const = 0;

  /**
   * @brief Called when the user selects this TC.
   *
   * Add all UI elements for this test to @p contentArea.  The content area
   * is a vertical StackLayout that occupies the screen below the navigation
   * header.  Its width is MATCH_PARENT and height fills the remaining space.
   *
   * @param[in] contentArea  View to add test UI into
   */
  virtual void OnEnter(Dali::Ui::View contentArea) = 0;

  /**
   * @brief Called when the user navigates back from this TC.
   *
   * Override to release resources (timers, signal connections, etc.).
   * The default implementation does nothing.
   */
  virtual void OnExit()
  {
  }
};

// ---------------------------------------------------------------------------
// Registry
// ---------------------------------------------------------------------------

/**
 * @brief Singleton that keeps a list of factory functions for all TCs.
 *
 * TC files self-register via the REGISTER_MANUAL_TEST macro; you never need
 * to call Registry directly.
 */
class Registry
{
public:
  using CreateFunc = std::function<std::unique_ptr<TestCase>()>;

  static Registry& Get()
  {
    static Registry instance;
    return instance;
  }

  void Add(CreateFunc creator)
  {
    mCreators.push_back(std::move(creator));
  }

  std::vector<std::unique_ptr<TestCase>> CreateAll() const
  {
    std::vector<std::unique_ptr<TestCase>> cases;
    cases.reserve(mCreators.size());
    for(const auto& creator : mCreators)
    {
      cases.push_back(creator());
    }
    return cases;
  }

private:
  Registry() = default;

  std::vector<CreateFunc> mCreators;
};

// ---------------------------------------------------------------------------
// Registrar helper
// ---------------------------------------------------------------------------

/**
 * @brief Instantiated as a file-scope static to register a TC at startup.
 */
template<typename T>
struct Registrar
{
  Registrar()
  {
    Registry::Get().Add([]()
    { return std::make_unique<T>(); });
  }
};

} // namespace ManualTest

// ---------------------------------------------------------------------------
// Public macro
// ---------------------------------------------------------------------------

/**
 * @brief Registers a TestCase subclass with the global launcher registry.
 *
 * Place this at file scope (outside any namespace or function) in the .cpp
 * file where the test case class is defined.
 *
 * @param ClassName  The TestCase subclass to register
 */
#define REGISTER_MANUAL_TEST(ClassName)                             \
  namespace                                                         \
  {                                                                 \
  static ManualTest::Registrar<ClassName> g_##ClassName##Registrar; \
  }
