#pragma once

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
#include <dali/devel-api/adaptor-framework/async-task-manager.h>
#include <vector>

namespace Test
{
namespace AsyncTaskManager
{
// Destroy global static AsyncTaskManager, only for ~UiTestApplication()
void DestroyAsyncTaskManager();

// Execute one completed processes synchronously.
void ProcessSingleCompletedTasks();

// Execute all completed processes synchronously.
void ProcessAllCompletedTasks();

// Let we delay to call NotifyToReady until `UngrabNotifyToReady()`.
void GrabNotifyToReady();

// Call all delayed NotifyToReady calls.
void UngrabNotifyToReady();

/**
 * @brief Captures production tasks so tests can control processing and delivery.
 * @param[in] enabled True to capture newly submitted tasks instead of scheduling them.
 */
void SetTaskCaptureEnabled(bool enabled);

/**
 * @brief Takes the captured tasks from the mock scheduler.
 * @return The captured tasks, leaving the capture queue empty.
 */
std::vector<Dali::AsyncTaskPtr> TakeCapturedTasks();
} // namespace AsyncTaskManager
} // namespace Test
