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
#include <dali/public-api/common/dali-string.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/span-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace Text
{

/**
 * @brief Implementation for annotation span payloads.
 */
class AnnotationSpan : public Span
{
public:
  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL AnnotationSpan* NewForBaseHandle(const Dali::String& key, const Dali::String& value)
  {
    return new AnnotationSpan(key, value);
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<AnnotationSpan> New(const Dali::String& key, const Dali::String& value)
  {
    return IntrusivePtr<AnnotationSpan>(new AnnotationSpan(key, value));
  }

  const Dali::String& GetKey() const;
  const Dali::String& GetValue() const;

protected:
  AnnotationSpan(const Dali::String& key, const Dali::String& value);
  ~AnnotationSpan() override;

private:
  Dali::String mKey;
  Dali::String mValue;
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
