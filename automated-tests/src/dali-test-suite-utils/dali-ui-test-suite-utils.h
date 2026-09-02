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

// dali-core umbrella header (synced from dali-core)
#include <dali-test-suite-utils.h>

// dali-ui specific test application
#include <dali-ui/ui-test-application.h>

#include <dali/public-api/adaptor-framework/window.h>

namespace Test
{
/**
 * @brief Invokes and clears every frame-presented callback queued on a window.
 *
 * The stub window records callbacks added through
 * Window::AddFramePresentedCallback() instead of waiting for a real frame, so a
 * test can drive presentation timing explicitly.
 */
void EmitFramePresented(Dali::Window window);

} // namespace Test
