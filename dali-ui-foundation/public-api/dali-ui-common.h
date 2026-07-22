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
#include <dali/public-api/dali-adaptor-common.h>

/*
 * Definitions for shared library support.
 *
 * DALI UI is split into foundation and components shared libraries. On
 * Windows each library must export only the declarations that it owns. On
 * platforms with ELF visibility, HIDE_DALI_INTERNALS selects the explicit
 * public symbols while export-all and debug builds leave visibility unchanged.
 */
#if defined(_WIN32)
#if defined(BUILDING_DALI_UI_FOUNDATION)
#define DALI_UI_API __declspec(dllexport)
#else
#define DALI_UI_API __declspec(dllimport)
#endif

#if defined(BUILDING_DALI_UI_COMPONENTS)
#define DALI_UI_COMPONENTS_API __declspec(dllexport)
#else
#define DALI_UI_COMPONENTS_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#ifndef HIDE_DALI_INTERNALS
#define DALI_UI_API
#define DALI_UI_COMPONENTS_API
#else
#define DALI_UI_API __attribute__((visibility("default")))
#define DALI_UI_COMPONENTS_API __attribute__((visibility("default")))
#endif
#else
/** Visibility attribute to show declarations */
#define DALI_UI_API
#define DALI_UI_COMPONENTS_API
#endif
