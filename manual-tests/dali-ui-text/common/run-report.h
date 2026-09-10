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

#include <fstream>
#include <string>

namespace TextManualTest
{
/**
 * @brief Writes aggregate and per-run test reports.
 *
 * The requested path is an append-only combined log. Each run also receives
 * a separate report directory so previous results are preserved.
 */
class RunReport
{
public:
  /**
   * @brief Opens the aggregate log and creates a separate report for the run.
   *
   * @param[in] aggregatePath The path of the combined log to append to.
   * @param[in] testId The test identifier used in the report directory name.
   * @return True if both logs and the run header were written successfully, false otherwise.
   */
  bool Open(const std::string& aggregatePath, const std::string& testId);

  /**
   * @brief Appends a line to both reports and flushes their output.
   *
   * @param[in] line The log line to write, without a trailing newline.
   * @return True if both writes and flushes succeeded, false otherwise.
   */
  bool Write(const std::string& line);

  /**
   * @brief Closes both reports without removing their files.
   */
  void Close();

  /**
   * @brief Returns whether both report files are open.
   *
   * @return True if both files are open, false otherwise.
   */
  bool IsOpen() const;

  /**
   * @brief Returns the path of the current per-run report.
   *
   * @return The per-run report path.
   */
  const std::string& GetPath() const;

private:
  std::ofstream mAggregate, mRun;
  std::string   mPath;
};
} // namespace TextManualTest
