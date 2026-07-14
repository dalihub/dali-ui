#pragma once

#include <dali/public-api/common/extents.h>

#include <dali-ui-foundation/public-api/dali-ui-common.h>

#include <iosfwd>

namespace Dali
{
namespace Ui
{
/**
 * @brief Four logical edge insets with floating-point precision.
 *
 * Values are ordered as start, end, top, and bottom.
 */
struct DALI_UI_API Insets
{
  Insets();

  Insets(float start, float end, float top, float bottom);

  Insets(const Dali::Extents& extents);

  Insets& operator=(const float* array);

  // TODO(DALIUI-XXXX): Remove after applications migrate from Extents to Insets.
  operator Dali::Extents() const;

  bool operator==(const Insets& rhs) const;

  bool operator!=(const Insets& rhs) const;

  float start;
  float end;
  float top;
  float bottom;
};

DALI_UI_API std::ostream& operator<<(std::ostream& stream, const Insets& insets);
} // namespace Ui
} // namespace Dali
