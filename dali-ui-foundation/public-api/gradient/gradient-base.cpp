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

// CLASS HEADER
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>

#define DALI_ASSERT_VALID_GRADIENT_BASE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Gradient::Base object")

namespace Dali
{
namespace Ui
{
namespace Gradient
{

static_assert(sizeof(Linear) == sizeof(Base), "Gradient::Linear must not add data members.");
static_assert(sizeof(Radial) == sizeof(Base), "Gradient::Radial must not add data members.");
static_assert(sizeof(Conic) == sizeof(Base), "Gradient::Conic must not add data members.");

class Base::Impl
{
public:
  struct StopData
  {
    float   offset{0.0f};
    UiColor color;
  };

  struct LinearData
  {
    Vector2 startPosition{Vector2::ZERO};
    Vector2 endPosition{Vector2::ONE};
  };

  struct RadialData
  {
    Vector2 center{Vector2::ZERO};
    float   radius{0.0f};
  };

  struct ConicData
  {
    Vector2 center{Vector2::ZERO};
    Radian  startAngle{0.0f};
  };

  Type         type{Type::NONE};
  Units        units{Units::OBJECT_BOUNDING_BOX};
  SpreadMethod spreadMethod{SpreadMethod::PAD};
  float        startOffset{0.0f};

  Dali::Vector<StopData> stops;

  LinearData linear;
  RadialData radial;
  ConicData  conic;
};

Base::Base()
: mImpl(new Impl())
{
}

Base::Base(const Base& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Base::Base(Base&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Base::~Base()
{
  delete mImpl;
}

Base& Base::operator=(const Base& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_GRADIENT_BASE(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }

  return *this;
}

Base& Base::operator=(Base&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }

  return *this;
}

const Base& Base::None()
{
  static const Base none;
  return none;
}

Type Base::GetType() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return mImpl->type;
}

void Base::SetStopNodes(const Dali::Vector<StopNode>& stopNodes)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);

  mImpl->stops.Clear();
  mImpl->stops.Reserve(stopNodes.Count());

  for(auto i = 0u; i < stopNodes.Count(); ++i)
  {
    Impl::StopData stopData;
    stopData.offset = stopNodes[i].GetOffset();
    stopData.color  = stopNodes[i].GetColor();
    mImpl->stops.PushBack(stopData);
  }
}

Dali::Vector<StopNode> Base::GetStopNodes() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);

  Dali::Vector<StopNode> stopNodes;
  stopNodes.Reserve(mImpl->stops.Count());

  for(auto i = 0u; i < mImpl->stops.Count(); ++i)
  {
    stopNodes.PushBack(StopNode(mImpl->stops[i].offset, mImpl->stops[i].color));
  }

  return stopNodes;
}

void Base::SetUnits(Units units)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  mImpl->units = units;
}

Units Base::GetUnits() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return mImpl->units;
}

void Base::SetSpreadMethod(SpreadMethod spreadMethod)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  mImpl->spreadMethod = spreadMethod;
}

SpreadMethod Base::GetSpreadMethod() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return mImpl->spreadMethod;
}

void Base::SetStartOffset(float offset)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  mImpl->startOffset = offset;
}

float Base::GetStartOffset() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return mImpl->startOffset;
}

Linear::Linear()
: Base()
{
  mImpl->type = Type::LINEAR;
}

Linear::Linear(const Vector2& startPosition, const Vector2& endPosition)
: Linear()
{
  SetStartAndEndPosition(startPosition, endPosition);
}

void Linear::SetStartAndEndPosition(const Vector2& startPosition, const Vector2& endPosition)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);

  mImpl->type                 = Type::LINEAR;
  mImpl->linear.startPosition = startPosition;
  mImpl->linear.endPosition   = endPosition;
}

Vector2 Linear::GetStartPosition() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return (mImpl->type == Type::LINEAR) ? mImpl->linear.startPosition : Vector2::ZERO;
}

Vector2 Linear::GetEndPosition() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return (mImpl->type == Type::LINEAR) ? mImpl->linear.endPosition : Vector2::ZERO;
}

Linear Linear::DownCast(const Base& base)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(base.mImpl);

  Linear result;
  if(base.mImpl->type != Type::LINEAR)
  {
    result.mImpl->type = Type::NONE;
    return result;
  }

  static_cast<Base&>(result) = base;
  return result;
}

Radial::Radial()
: Base()
{
  mImpl->type = Type::RADIAL;
}

Radial::Radial(const Vector2& center, float radius)
: Radial()
{
  SetCenterAndRadius(center, radius);
}

void Radial::SetCenterAndRadius(const Vector2& center, float radius)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);

  mImpl->type          = Type::RADIAL;
  mImpl->radial.center = center;
  mImpl->radial.radius = radius;
}

Vector2 Radial::GetCenter() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return (mImpl->type == Type::RADIAL) ? mImpl->radial.center : Vector2::ZERO;
}

float Radial::GetRadius() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return (mImpl->type == Type::RADIAL) ? mImpl->radial.radius : 0.0f;
}

Radial Radial::DownCast(const Base& base)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(base.mImpl);

  Radial result;
  if(base.mImpl->type != Type::RADIAL)
  {
    result.mImpl->type = Type::NONE;
    return result;
  }

  static_cast<Base&>(result) = base;
  return result;
}

Conic::Conic()
: Base()
{
  mImpl->type = Type::CONIC;
}

Conic::Conic(const Vector2& center, Radian startAngle)
: Conic()
{
  SetCenterAndStartAngle(center, startAngle);
}

void Conic::SetCenterAndStartAngle(const Vector2& center, Radian startAngle)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);

  mImpl->type             = Type::CONIC;
  mImpl->conic.center     = center;
  mImpl->conic.startAngle = startAngle;
}

Vector2 Conic::GetCenter() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return (mImpl->type == Type::CONIC) ? mImpl->conic.center : Vector2::ZERO;
}

Radian Conic::GetStartAngle() const
{
  DALI_ASSERT_VALID_GRADIENT_BASE(mImpl);
  return (mImpl->type == Type::CONIC) ? mImpl->conic.startAngle : Radian(0.0f);
}

Conic Conic::DownCast(const Base& base)
{
  DALI_ASSERT_VALID_GRADIENT_BASE(base.mImpl);

  Conic result;
  if(base.mImpl->type != Type::CONIC)
  {
    result.mImpl->type = Type::NONE;
    return result;
  }

  static_cast<Base&>(result) = base;
  return result;
}

} // namespace Gradient
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_GRADIENT_BASE
