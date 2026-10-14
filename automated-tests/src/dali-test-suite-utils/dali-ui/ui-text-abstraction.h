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

#ifndef DALI_UI_TEST_TEXT_ABSTRACTION_H
#define DALI_UI_TEST_TEXT_ABSTRACTION_H

#include <dali/devel-api/text-abstraction/bidirectional-support.h>

namespace DALI_NAMESPACE
{
namespace TextAbstraction
{
using TestReorderCallback = void (*)(BidiInfoIndex, CharacterIndex, Length, CharacterIndex*);

/**
 * @brief Installs a mapping oracle in the UTC adaptor mock for the current thread.
 * @param[in] callback Oracle receiving a paragraph-relative range and writing
 * a range-relative visual-to-logical map, or nullptr for the default stub.
 * @return Previous callback, which the test must restore before its state expires.
 */
TestReorderCallback SetTestReorderCallback(TestReorderCallback callback);
} // namespace TextAbstraction
} // namespace DALI_NAMESPACE

#endif // DALI_UI_TEST_TEXT_ABSTRACTION_H
