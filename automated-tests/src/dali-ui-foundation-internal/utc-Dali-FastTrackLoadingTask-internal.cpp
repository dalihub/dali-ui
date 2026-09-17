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
 */

#include <algorithm>
#include <string>

#include <dali-ui-foundation/internal/image-loader/fast-track-loading-task.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
std::string GetRepositoryResourcePath(const char* relativePath)
{
  std::string testResourceDirectory(__FILE__);
  std::replace(testResourceDirectory.begin(), testResourceDirectory.end(), '\\', '/');
  const std::string marker("/automated-tests/");
  const std::size_t markerOffset = testResourceDirectory.rfind(marker);
  return markerOffset == std::string::npos
           ? std::string(relativePath)
           : testResourceDirectory.substr(0u, markerOffset + 1u) + relativePath;
}

bool gCallbackCalled = false;

void OnImageLoaded(Dali::Ui::Internal::FastTrackLoadingTaskPtr task)
{
  gCallbackCalled = true;
  DALI_TEST_CHECK(task);
}

void CompleteTask(const Dali::Ui::Internal::FastTrackLoadingTaskPtr& task)
{
  CallbackBase::Execute(*task->GetCompletedCallback(), AsyncTaskPtr(task.Get()));
}
} // namespace

void utc_dali_fast_track_loading_task_internal_startup(void)
{
  test_return_value = TET_UNDEF;
  gCallbackCalled   = false;
}

void utc_dali_fast_track_loading_task_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliFastTrackLoadingTaskLoadP(void)
{
  UiTestApplication application;
  const Dali::Ui::Internal::VisualUrl url(GetRepositoryResourcePath("dali-ui-components/images/border/close.png"));
  Dali::Ui::Internal::FastTrackLoadingTaskPtr task = new Dali::Ui::Internal::FastTrackLoadingTask(
    url,
    ImageDimensions(),
    SamplingMode::BOX_THEN_LINEAR,
    true,
    Dali::Ui::Integration::PreMultiplyOnLoad::ON,
    false,
    MakeCallback(&OnImageLoaded));

  DALI_TEST_EQUALS(task->GetTaskName(), Dali::StringView("FastTrackLoadingTask"), TEST_LOCATION);
  DALI_TEST_EQUALS(task->GetPriorityType(), AsyncTask::PriorityType::HIGH, TEST_LOCATION);
  DALI_TEST_EQUALS(task->mTextures.size(), 1u, TEST_LOCATION);
  task->Process();
  DALI_TEST_CHECK(task->mLoadSuccess);
  DALI_TEST_CHECK(task->mPremultiplied);
  DALI_TEST_CHECK(!task->mPlanesLoaded);
  CompleteTask(task);
  DALI_TEST_CHECK(gCallbackCalled);
  DALI_TEST_EQUALS(task->mTextures.size(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFastTrackLoadingTaskPlanesAndFailureP(void)
{
  UiTestApplication application;
  const Dali::Ui::Internal::VisualUrl url(GetRepositoryResourcePath("dali-ui-components/images/border/close.png"));
  Dali::Ui::Internal::FastTrackLoadingTaskPtr planeTask = new Dali::Ui::Internal::FastTrackLoadingTask(
    url,
    ImageDimensions(16u, 16u),
    SamplingMode::NEAREST,
    false,
    Dali::Ui::Integration::PreMultiplyOnLoad::OFF,
    true,
    nullptr);
  DALI_TEST_EQUALS(planeTask->mTextures.size(), 4u, TEST_LOCATION);
  planeTask->Process();
  DALI_TEST_CHECK(planeTask->mLoadSuccess);
  DALI_TEST_CHECK(!planeTask->mPlanesLoaded);
  CompleteTask(planeTask);
  DALI_TEST_EQUALS(planeTask->mTextures.size(), 1u, TEST_LOCATION);

  Dali::Ui::Internal::FastTrackLoadingTaskPtr failedTask = new Dali::Ui::Internal::FastTrackLoadingTask(
    Dali::Ui::Internal::VisualUrl(""),
    ImageDimensions(),
    SamplingMode::DEFAULT,
    false,
    Dali::Ui::Integration::PreMultiplyOnLoad::OFF,
    false,
    nullptr);
  failedTask->Process();
  DALI_TEST_CHECK(!failedTask->mLoadSuccess);
  CompleteTask(failedTask);
  DALI_TEST_CHECK(failedTask->mTextures.empty());
  END_TEST;
}
