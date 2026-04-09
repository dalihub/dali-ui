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
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/property-value.h>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>

namespace Dali
{
namespace Ui
{

class View;

namespace Internal
{

class ViewAnimationSpecImpl;
using ViewAnimationSpecImplPtr = IntrusivePtr<ViewAnimationSpecImpl>;

/**
 * @brief Internal implementation of ViewAnimationSpec.
 */
class DALI_UI_API ViewAnimationSpecImpl : public BaseObject
{
public:
  static ViewAnimationSpecImplPtr New();

  struct Entry
  {
    using ApplyFunction = void(*)(Animation&, View, const Entry&);

    Property::Index index;
    Property::Value value;
    Duration        duration;
    AlphaFunction   alpha;
    Duration        delay;
    ApplyFunction   apply;
  };

  static void ApplyAnimateTo(Animation& animation, View view, const Entry& entry);
  static void ApplyAnimateBy(Animation& animation, View view, const Entry& entry);

  void AddAnimateToEntry(Property::Index index, Property::Value value,
                         Duration duration, AlphaFunction alpha, Duration delay);
  void AddAnimateToEntry(Property::Index index, Property::Value value,
                         Duration duration, AlphaFunction alpha, Duration delay,
                         Entry::ApplyFunction apply);
  void AddAnimateByEntry(Property::Index index, Property::Value value,
                         Duration duration, AlphaFunction alpha, Duration delay);
  void AddAnimateByEntry(Property::Index index, Property::Value value,
                         Duration duration, AlphaFunction alpha, Duration delay,
                         Entry::ApplyFunction apply);

  void ApplyEntries(Animation animation, View view) const;

  static void ApplyBackgroundColorTo(Animation& animation, View view, const Entry& entry);
  static void ApplyBackgroundColorBy(Animation& animation, View view, const Entry& entry);
  static void ApplySizeWidthTo(Animation& animation, View view, const Entry& entry);
  static void ApplySizeWidthBy(Animation& animation, View view, const Entry& entry);
  static void ApplySizeHeightTo(Animation& animation, View view, const Entry& entry);
  static void ApplySizeHeightBy(Animation& animation, View view, const Entry& entry);

protected:
  ViewAnimationSpecImpl();
  ~ViewAnimationSpecImpl() override;

  std::vector<Entry> mEntries;

private:
  ViewAnimationSpecImpl(const ViewAnimationSpecImpl&) = delete;
  ViewAnimationSpecImpl(ViewAnimationSpecImpl&&) = delete;
  ViewAnimationSpecImpl& operator=(const ViewAnimationSpecImpl&) = delete;
  ViewAnimationSpecImpl& operator=(ViewAnimationSpecImpl&&) = delete;
};

inline Internal::ViewAnimationSpecImpl& GetImpl(ViewAnimationSpec& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::ViewAnimationSpecImpl&>(handle);
}

inline const Internal::ViewAnimationSpecImpl& GetImpl(const ViewAnimationSpec& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::ViewAnimationSpecImpl&>(handle);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
