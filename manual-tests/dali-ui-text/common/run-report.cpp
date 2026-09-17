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

#include "run-report.h"

#include <cstdlib>
#include <vector>

namespace TextManualTest
{
bool RunReport::Open(const std::string& aggregatePath, const std::string& testId)
{
  Close();
  mAggregate.clear();
  mRun.clear();
  mPath.clear();
  mAggregate.open(aggregatePath, std::ios::app);
  if(!mAggregate) return false;

  const std::string pattern = aggregatePath + "." + testId + ".XXXXXX";
  std::vector<char> writable(pattern.begin(), pattern.end());
  writable.push_back('\0');
  const char* directory = mkdtemp(writable.data());
  if(!directory)
  {
    Close();
    return false;
  }
  mPath = std::string(directory) + "/report.log";
  mRun.open(mPath);
  if(!mRun)
  {
    Close();
    return false;
  }
  return Write("[REPORT][BEGIN] test=" + testId + " report=" + mPath);
}

bool RunReport::Write(const std::string& line)
{
  mAggregate << line << '\n';
  mRun << line << '\n';
  mAggregate.flush();
  mRun.flush();
  return bool(mAggregate) && bool(mRun);
}

void RunReport::Close()
{
  if(mAggregate.is_open()) mAggregate.close();
  if(mRun.is_open()) mRun.close();
}

bool RunReport::IsOpen() const
{
  return mAggregate.is_open() && mRun.is_open();
}

const std::string& RunReport::GetPath() const
{
  return mPath;
}
} // namespace TextManualTest
