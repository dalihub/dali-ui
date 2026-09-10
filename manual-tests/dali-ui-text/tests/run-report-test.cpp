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

#include "common/run-report.h"

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <vector>

namespace
{
void Require(bool condition)
{
  if(!condition)
  {
    std::fprintf(stderr, "run-report test failed\n");
    std::exit(1);
  }
}
std::string Read(const std::string& path)
{
  std::ifstream file(path);
  return {std::istreambuf_iterator<char>(file), {}};
}
} // namespace

int main()
{
  char        pattern[] = "/tmp/text-run-report-test-XXXXXX";
  const char* directory = mkdtemp(pattern);
  Require(directory != nullptr);
  const std::string base = std::string(directory) + "/combined.log";
  {
    std::ofstream old(base);
    old << "PREVIOUS\n";
  }
  std::vector<std::string>  paths, contents;
  TextManualTest::RunReport report;
  for(const auto* id : {"text-ellipsis", "text-marquee", "text-ellipsis", "text-layout"})
  {
    Require(report.Open(base, id));
    Require(report.Write(std::string(id) + " RESULT"));
    paths.push_back(report.GetPath());
    report.Close();
    contents.push_back(Read(paths.back()));
    Require(contents.back().find(std::string(id) + " RESULT") != std::string::npos);
  }
  Require(paths[0] != paths[2]);
  for(size_t i = 0; i < paths.size(); ++i) Require(Read(paths[i]) == contents[i]);
  const auto aggregate = Read(base);
  Require(aggregate.find("PREVIOUS\n") == 0);
  for(const auto& content : contents) Require(aggregate.find(content) != std::string::npos);
  // Recreating a TC object or a process must also preserve previous runs.
  TextManualTest::RunReport reopened;
  Require(reopened.Open(base, "text-ellipsis"));
  paths.push_back(reopened.GetPath());
  reopened.Close();
  Require(Read(base).find(aggregate) == 0);
  Require(!reopened.Open(std::string(directory) + "/missing/report", "text-layout"));
  Require(!reopened.IsOpen());
  for(const auto& path : paths)
  {
    Require(std::remove(path.c_str()) == 0);
    Require(rmdir(path.substr(0, path.rfind('/')).c_str()) == 0);
  }
  Require(std::remove(base.c_str()) == 0);
  Require(rmdir(directory) == 0);
  std::puts("run-report: preserve existing / cross-test / rerun / reopen / I/O failure PASS");
}
