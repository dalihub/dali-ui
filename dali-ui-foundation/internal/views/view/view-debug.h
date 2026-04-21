#ifndef DALI_UI_INTERNAL_VIEW_DEBUG_H
#define DALI_UI_INTERNAL_VIEW_DEBUG_H
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#if defined(DEBUG_ENABLED)

#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali/public-api/object/handle.h>
#include <iostream>
#include <string>

namespace Dali
{
namespace Ui
{
namespace Internal
{
/**
 * Convert properties of handle into JSON output, separated into 'normal' and 'child' properties.
 * @param[in] outputStream the output stream to write to
 * @param[in] handle The handle of the object from which to retrieve properties
 * @return The output stream
 */
std::ostream& DumpProperties(std::ostream& outputStream, Handle handle);

/**
 * Dumps view internals, visuals and properties to a string in JSON format
 */
std::string DumpView(const ViewImpl& view);

/**
 * Dumps actor internals and properties to a string in JSON format
 */
std::string DumpActor(Actor actor);

/**
 * Dumps actor hierarchy from a given root, but expands View output to encompass View internals.
 * Formats the output in JSON.
 * @param[in] outputStream the output stream to write to
 * @param[in] root The root actor
 */
void DumpViewHierarchy(std::ostream& outputStream, Actor rootActor);

} // namespace Internal

} // namespace Ui

} // namespace Dali

#endif

#endif // DALI_UI_INTERNAL_VIEW_DEBUG_H
