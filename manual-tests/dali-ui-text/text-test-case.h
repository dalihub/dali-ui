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

// Reuse the foundation manual-test registry and OnEnter/OnExit contract.
// This header and all test code use public DALi APIs only.
#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include "../dali-ui-foundation/manual-test-case.h"
#include "common/run-report.h"

namespace TextManualTest
{
class TestCase : public ManualTest::TestCase
{
public:
  virtual const char* GetId() const = 0;
  virtual void        OnKeyEvent(Dali::KeyEvent)
  {
  }
  virtual void StartAutomation()            = 0;
  virtual bool IsAutomationFinished() const = 0;
  virtual int  GetExitCode() const          = 0;

  void Configure(Dali::Window window, const std::string& reportPath)
  {
    mWindow         = window;
    mReportPath     = reportPath;
    mReportBasePath = reportPath;
  }

protected:
  /**
   * @brief Opens the reports for this test run and updates the displayed report path.
   *
   * @return True if the reports were opened successfully, false otherwise.
   */
  bool OpenReport()
  {
    mReportPath = mReportBasePath;
    if(!mReport.Open(mReportBasePath, GetId())) return false;
    mReportPath = mReport.GetPath();
    return true;
  }

  Dali::Window      mWindow;
  std::string       mReportPath;
  std::string       mReportBasePath;
  mutable RunReport mReport;
};

// Append new IDs here unless a different display position is explicitly wanted.
// Never rely on cross-translation-unit static initialization or glob order.
inline auto CreateOrderedCases()
{
  static constexpr std::array<const char*, 3> ORDER{{"text-layout", "text-ellipsis", "text-marquee"}};
  auto                                        cases = ManualTest::Registry::Get().CreateAll();
  const auto                                  rank  = [](const auto& item)
  {
    auto* test = dynamic_cast<TestCase*>(item.get());
    return test ? std::find(ORDER.begin(), ORDER.end(), std::string(test->GetId())) - ORDER.begin() : std::ptrdiff_t(ORDER.size());
  };
  std::stable_sort(cases.begin(), cases.end(), [&](const auto& a, const auto& b)
  { return rank(a) < rank(b); });
  return cases; // Unlisted tests also go after all explicitly ordered items.
}
} // namespace TextManualTest
