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

// Mock implementation of Dali::CanvasRenderer and its drawable/gradient family used by the
// dali-ui automated-tests.
//
// The real implementation (dali-adaptor's Internal::Adaptor::CanvasRenderer and friends) is backed
// by thorvg, which may not be installed on the test machine. Without thorvg every one of those
// methods is compiled down to `return false;` / `return 0u;` by the THORVG_SUPPORT guards, so a
// test that checks a return value fails on a machine without thorvg and passes on one with it.
// Symbol interposition (this file is compiled directly into the test binary, ahead of
// libdali2-adaptor.so on the link line) lets the test suite exercise the CanvasRenderer API surface
// without depending on thorvg being present at all, matching the approach already used by
// dali-toolkit's dali-toolkit-test-utils/toolkit-canvas-renderer.cpp.
//
// The whole family - Drawable and every class deriving from it, plus Gradient - is mocked together
// on purpose. Each public handle passes its BaseObject to GetImplementation(), which static_casts
// it to the Internal::Adaptor type, so a mocked public class holding a real Internal::Adaptor
// object (or the other way round) would reinterpret unrelated memory. A drawable type added to
// dali-adaptor has to be added here too.
//
// Behaviour mirrors the thorvg-backed implementation wherever a test can observe it: path
// command/point validation, colour stops kept in insertion order and quantised to 8 bits per
// channel, and the "a drawable can only be added to one parent" rule. The same test cases
// therefore pass on a machine that does have thorvg installed. Rasterization itself is not
// modelled - GetRasterizedTexture() hands back an empty texture of the requested size, and
// GetBoundingBox() has no geometry to measure.

#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-drawable-group.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-drawable.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-gradient.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-linear-gradient.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-picture.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-radial-gradient.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/renderer.h>

#include <algorithm>
#include <vector>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class Drawable;
class Shape;
class DrawableGroup;
class Picture;
class Gradient;
class LinearGradient;
class RadialGradient;

Drawable&             GetImplementation(Dali::CanvasRenderer::Drawable& drawable);
const Drawable&       GetImplementation(const Dali::CanvasRenderer::Drawable& drawable);
Shape&                GetImplementation(Dali::CanvasRenderer::Shape& shape);
const Shape&          GetImplementation(const Dali::CanvasRenderer::Shape& shape);
DrawableGroup&        GetImplementation(Dali::CanvasRenderer::DrawableGroup& group);
Picture&              GetImplementation(Dali::CanvasRenderer::Picture& picture);
const Picture&        GetImplementation(const Dali::CanvasRenderer::Picture& picture);
Gradient&             GetImplementation(Dali::CanvasRenderer::Gradient& gradient);
const Gradient&       GetImplementation(const Dali::CanvasRenderer::Gradient& gradient);
LinearGradient&       GetImplementation(Dali::CanvasRenderer::LinearGradient& gradient);
const LinearGradient& GetImplementation(const Dali::CanvasRenderer::LinearGradient& gradient);
RadialGradient&       GetImplementation(Dali::CanvasRenderer::RadialGradient& gradient);
const RadialGradient& GetImplementation(const Dali::CanvasRenderer::RadialGradient& gradient);

using DrawableVector = std::vector<Dali::CanvasRenderer::Drawable>;

/**
 * @brief Quantises a colour the way thorvg stores it, so that a colour written through the mock
 * reads back with the same precision loss as it would through the real implementation.
 */
inline Vector4 QuantizeColor(const Vector4& color)
{
  return Vector4(static_cast<uint8_t>(color.r * 255.0f) / 255.0f,
                 static_cast<uint8_t>(color.g * 255.0f) / 255.0f,
                 static_cast<uint8_t>(color.b * 255.0f) / 255.0f,
                 static_cast<uint8_t>(color.a * 255.0f) / 255.0f);
}

/**
 * @brief Returns how many points the given path command consumes, as shape-impl.cpp does.
 */
inline uint32_t GetRequiredPointCount(Dali::CanvasRenderer::Shape::PathCommandType command)
{
  switch(command)
  {
    case Dali::CanvasRenderer::Shape::PathCommandType::CLOSE:
    {
      return 0u;
    }
    case Dali::CanvasRenderer::Shape::PathCommandType::MOVE_TO:
    case Dali::CanvasRenderer::Shape::PathCommandType::LINE_TO:
    {
      return 1u;
    }
    case Dali::CanvasRenderer::Shape::PathCommandType::CUBIC_TO:
    {
      return 3u;
    }
  }
  return 0u;
}

class Drawable : public Dali::BaseObject
{
public:
  enum class CompositionType
  {
    NONE = 0,
    CLIP_PATH,
    ALPHA_MASK,
    ALPHA_MASK_INVERSE
  };

  bool SetOpacity(float opacity)
  {
    mOpacity = opacity;
    mChanged = true;
    return true;
  }

  float GetOpacity() const
  {
    return mOpacity;
  }

  // The transform calls are accepted but not modelled: the public API has no getter for them, and
  // nothing here rasterizes, so there is nothing a test could observe.
  bool Rotate(Degree degree)
  {
    mChanged = true;
    return true;
  }

  bool Scale(float factor)
  {
    mChanged = true;
    return true;
  }

  bool Translate(const Vector2& translate)
  {
    mChanged = true;
    return true;
  }

  bool Transform(const Dali::Matrix3& matrix)
  {
    mChanged = true;
    return true;
  }

  // Nothing is rasterized here, so there is no geometry to measure. The real implementation asks
  // thorvg for the bounds of the accumulated path.
  Bounds GetBoundingBox() const
  {
    return Bounds(0.0f, 0.0f, 0.0f, 0.0f);
  }

  bool SetClipPath(Dali::CanvasRenderer::Drawable& clip)
  {
    if(!TakeOwnershipOf(clip))
    {
      return false;
    }
    mCompositionDrawable = clip;
    mCompositionType     = CompositionType::CLIP_PATH;
    mChanged             = true;
    return true;
  }

  bool SetMask(Dali::CanvasRenderer::Drawable& mask, Dali::CanvasRenderer::Drawable::MaskType type)
  {
    if(!TakeOwnershipOf(mask))
    {
      return false;
    }
    mCompositionDrawable = mask;
    mCompositionType     = (type == Dali::CanvasRenderer::Drawable::MaskType::ALPHA_INVERSE)
                             ? CompositionType::ALPHA_MASK_INVERSE
                             : CompositionType::ALPHA_MASK;
    mChanged             = true;
    return true;
  }

  Dali::CanvasRenderer::Drawable GetCompositionDrawable() const
  {
    return mCompositionDrawable;
  }

  CompositionType GetCompositionType() const
  {
    return mCompositionType;
  }

  void SetAdded(bool added)
  {
    mAdded = added;
  }

  bool IsAdded() const
  {
    return mAdded;
  }

  bool GetChanged() const
  {
    return mChanged;
  }

protected:
  Drawable()                           = default;
  ~Drawable() override                 = default;
  Drawable(const Drawable&)            = delete;
  Drawable& operator=(const Drawable&) = delete;

private:
  /**
   * @brief Claims a drawable as this one's composition target, refusing one that already has a
   * parent, exactly as drawable-impl.cpp does.
   */
  bool TakeOwnershipOf(Dali::CanvasRenderer::Drawable& drawable)
  {
    Drawable& impl = GetImplementation(drawable);
    if(impl.IsAdded())
    {
      return false;
    }
    impl.SetAdded(true);
    return true;
  }

  Dali::CanvasRenderer::Drawable mCompositionDrawable;
  CompositionType                mCompositionType{CompositionType::NONE};
  float                          mOpacity{1.0f};
  bool                           mAdded{false};
  bool                           mChanged{false};
};

class Shape : public Drawable
{
public:
  using PathCommandType = Dali::CanvasRenderer::Shape::PathCommandType;

  // The primitives have no path commands of their own to record; thorvg expands them internally.
  bool AddRect(const Bounds& rect, const Vector2& roundedCorner)
  {
    return true;
  }

  bool AddCircle(const Vector2& center, const Vector2& radius)
  {
    return true;
  }

  bool AddArc(const Vector2& center, float radius, float startAngle, float sweep, bool pie)
  {
    return true;
  }

  bool AddMoveTo(const Vector2& point)
  {
    return AppendCommand(PathCommandType::MOVE_TO, &point, 1u);
  }

  bool AddLineTo(const Vector2& line)
  {
    return AppendCommand(PathCommandType::LINE_TO, &line, 1u);
  }

  bool AddCubicTo(const Vector2& controlPoint1, const Vector2& controlPoint2, const Vector2& endPoint)
  {
    const Vector2 points[3] = {controlPoint1, controlPoint2, endPoint};
    return AppendCommand(PathCommandType::CUBIC_TO, points, 3u);
  }

  // Mirrors shape-impl.cpp: an empty path, or one whose commands and points do not agree, is
  // rejected rather than read past the end of the point array.
  bool AddPath(const Dali::Vector<PathCommandType>& commands, const Dali::Vector<Vector2>& points)
  {
    if(commands.Count() == 0u || points.Count() == 0u)
    {
      return false;
    }

    uint32_t requiredPointCount = 0u;
    for(auto&& command : commands)
    {
      requiredPointCount += GetRequiredPointCount(command);
    }
    if(requiredPointCount != points.Count())
    {
      return false;
    }

    mCommands.insert(mCommands.end(), commands.Begin(), commands.End());
    mPoints.insert(mPoints.end(), points.Begin(), points.End());
    return true;
  }

  bool Close()
  {
    return AppendCommand(PathCommandType::CLOSE, nullptr, 0u);
  }

  bool ResetPath()
  {
    mCommands.clear();
    mPoints.clear();
    return true;
  }

  /**
   * @brief The path built so far. Not part of the public API - it is what AddPath() validates
   * against and what ResetPath() clears, and it gives a test hook should one ever be needed.
   */
  const std::vector<PathCommandType>& GetPathCommands() const
  {
    return mCommands;
  }

  const std::vector<Vector2>& GetPathPoints() const
  {
    return mPoints;
  }

  bool SetFillColor(const Vector4& color)
  {
    mFillColor = QuantizeColor(color);
    return true;
  }

  Vector4 GetFillColor() const
  {
    return mFillColor;
  }

  bool SetFillGradient(Dali::CanvasRenderer::Gradient& gradient)
  {
    mFillGradient = gradient;
    return true;
  }

  Dali::CanvasRenderer::Gradient GetFillGradient() const
  {
    return mFillGradient;
  }

  bool SetFillRule(Dali::CanvasRenderer::Shape::FillRule rule)
  {
    mFillRule = rule;
    return true;
  }

  Dali::CanvasRenderer::Shape::FillRule GetFillRule() const
  {
    return mFillRule;
  }

  bool SetStrokeWidth(float width)
  {
    mStrokeWidth = width;
    return true;
  }

  float GetStrokeWidth() const
  {
    return mStrokeWidth;
  }

  bool SetStrokeColor(const Vector4& color)
  {
    mStrokeColor = QuantizeColor(color);
    return true;
  }

  Vector4 GetStrokeColor() const
  {
    return mStrokeColor;
  }

  bool SetStrokeGradient(Dali::CanvasRenderer::Gradient& gradient)
  {
    mStrokeGradient = gradient;
    return true;
  }

  Dali::CanvasRenderer::Gradient GetStrokeGradient() const
  {
    return mStrokeGradient;
  }

  bool SetStrokeDash(const Dali::Vector<float>& dashPattern)
  {
    mStrokeDash = dashPattern;
    return true;
  }

  Dali::Vector<float> GetStrokeDash() const
  {
    return mStrokeDash;
  }

  bool SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap cap)
  {
    mStrokeCap = cap;
    return true;
  }

  Dali::CanvasRenderer::Shape::StrokeCap GetStrokeCap() const
  {
    return mStrokeCap;
  }

  bool SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin join)
  {
    mStrokeJoin = join;
    return true;
  }

  Dali::CanvasRenderer::Shape::StrokeJoin GetStrokeJoin() const
  {
    return mStrokeJoin;
  }

protected:
  ~Shape() override = default;

private:
  bool AppendCommand(PathCommandType command, const Vector2* points, uint32_t pointCount)
  {
    mCommands.push_back(command);
    for(uint32_t i = 0u; i < pointCount; ++i)
    {
      mPoints.push_back(points[i]);
    }
    return true;
  }

  std::vector<PathCommandType> mCommands;
  std::vector<Vector2>         mPoints;

  Dali::CanvasRenderer::Gradient mFillGradient;
  Dali::CanvasRenderer::Gradient mStrokeGradient;
  Dali::Vector<float>            mStrokeDash;

  Vector4                                 mFillColor{Vector4::ZERO};
  Vector4                                 mStrokeColor{Vector4::ZERO};
  Dali::CanvasRenderer::Shape::FillRule   mFillRule{Dali::CanvasRenderer::Shape::FillRule::WINDING};
  Dali::CanvasRenderer::Shape::StrokeCap  mStrokeCap{Dali::CanvasRenderer::Shape::StrokeCap::SQUARE};
  Dali::CanvasRenderer::Shape::StrokeJoin mStrokeJoin{Dali::CanvasRenderer::Shape::StrokeJoin::BEVEL};
  float                                   mStrokeWidth{0.0f};
};

class DrawableGroup : public Drawable
{
public:
  bool AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
  {
    Drawable& impl = GetImplementation(drawable);
    if(impl.IsAdded())
    {
      return false;
    }
    impl.SetAdded(true);
    mDrawables.push_back(drawable);
    return true;
  }

  bool RemoveDrawable(Dali::CanvasRenderer::Drawable drawable)
  {
    DrawableVector::iterator it = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    if(it == mDrawables.end())
    {
      return false;
    }
    GetImplementation(*it).SetAdded(false);
    mDrawables.erase(it);
    return true;
  }

  bool RemoveAllDrawables()
  {
    for(auto&& it : mDrawables)
    {
      GetImplementation(it).SetAdded(false);
    }
    mDrawables.clear();
    return true;
  }

  const DrawableVector& GetDrawables() const
  {
    return mDrawables;
  }

protected:
  ~DrawableGroup() override = default;

private:
  DrawableVector mDrawables;
};

class Picture : public Drawable
{
public:
  // The real implementation hands the url to thorvg's picture loader; here only the "no url given"
  // failure is observable.
  bool Load(const Dali::StringView& url)
  {
    return !url.Empty();
  }

  bool SetSize(const Vector2& size)
  {
    mSize = size;
    return true;
  }

  Vector2 GetSize() const
  {
    return mSize;
  }

protected:
  ~Picture() override = default;

private:
  Vector2 mSize{Vector2::ZERO};
};

class Gradient : public Dali::BaseObject
{
public:
  bool AddColorStop(float offset, const Vector4& color)
  {
    mColorStops.push_back(ColorStop{offset, QuantizeColor(color)});
    return true;
  }

  bool ClearColorStops()
  {
    mColorStops.clear();
    return true;
  }

  uint32_t GetColorStopCount() const
  {
    return static_cast<uint32_t>(mColorStops.size());
  }

  float GetColorStopOffset(uint32_t index) const
  {
    if(index >= mColorStops.size())
    {
      return 0.0f;
    }
    return mColorStops[index].offset;
  }

  Vector4 GetColorStopColor(uint32_t index) const
  {
    if(index >= mColorStops.size())
    {
      return Vector4::ZERO;
    }
    return mColorStops[index].color;
  }

  bool SetSpread(Dali::CanvasRenderer::Gradient::Spread spread)
  {
    mSpread = spread;
    return true;
  }

  Dali::CanvasRenderer::Gradient::Spread GetSpread() const
  {
    return mSpread;
  }

protected:
  Gradient()                           = default;
  ~Gradient() override                 = default;
  Gradient(const Gradient&)            = delete;
  Gradient& operator=(const Gradient&) = delete;

private:
  struct ColorStop
  {
    float   offset;
    Vector4 color;
  };

  std::vector<ColorStop>                 mColorStops;
  Dali::CanvasRenderer::Gradient::Spread mSpread{Dali::CanvasRenderer::Gradient::Spread::PAD};
};

class LinearGradient : public Gradient
{
public:
  bool SetBounds(const Vector2& firstPoint, const Vector2& secondPoint)
  {
    mFirstPoint  = firstPoint;
    mSecondPoint = secondPoint;
    return true;
  }

  bool GetBounds(Vector2& firstPoint, Vector2& secondPoint) const
  {
    firstPoint  = mFirstPoint;
    secondPoint = mSecondPoint;
    return true;
  }

protected:
  ~LinearGradient() override = default;

private:
  Vector2 mFirstPoint{Vector2::ZERO};
  Vector2 mSecondPoint{Vector2::ZERO};
};

class RadialGradient : public Gradient
{
public:
  bool SetBounds(const Vector2& centerPoint, float radius)
  {
    mCenterPoint = centerPoint;
    mRadius      = radius;
    return true;
  }

  bool GetBounds(Vector2& centerPoint, float& radius) const
  {
    centerPoint = mCenterPoint;
    radius      = mRadius;
    return true;
  }

protected:
  ~RadialGradient() override = default;

private:
  Vector2 mCenterPoint{Vector2::ZERO};
  float   mRadius{0.0f};
};

class CanvasRenderer : public Dali::BaseObject
{
public:
  CanvasRenderer(const Vector2& size)
  : mTexture(Dali::Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::RGBA8888, size.width, size.height)),
    mSize(size),
    mViewBox(size)
  {
  }

  bool Commit()
  {
    return true;
  }

  bool IsCanvasChanged() const
  {
    return true;
  }

  Dali::Texture GetRasterizedTexture()
  {
    return mTexture;
  }

  bool Rasterize()
  {
    return true;
  }

  bool AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
  {
    // Matches the real implementation (canvas-renderer-impl.cpp), which asserts via the
    // Drawable's own GetImplementation() before checking thorvg support, and refuses a drawable
    // that already belongs to another canvas or drawable group.
    Internal::Adaptor::Drawable& impl = GetImplementation(drawable);
    if(impl.IsAdded())
    {
      return false;
    }
    impl.SetAdded(true);
    mDrawables.push_back(drawable);
    return true;
  }

  bool RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable)
  {
    DrawableVector::iterator it = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    if(it == mDrawables.end())
    {
      return false;
    }
    GetImplementation(*it).SetAdded(false);
    mDrawables.erase(it);
    return true;
  }

  bool RemoveAllDrawables()
  {
    for(auto&& it : mDrawables)
    {
      GetImplementation(it).SetAdded(false);
    }
    mDrawables.clear();
    return true;
  }

  bool SetSize(Vector2 size)
  {
    mSize = size;
    return true;
  }

  Vector2 GetSize() const
  {
    return mSize;
  }

  bool SetViewBox(const Vector2& viewBox)
  {
    mViewBox = viewBox;
    return true;
  }

  const Vector2& GetViewBox() const
  {
    return mViewBox;
  }

  bool SetDropShadow(const Vector4& color, float offsetX, float offsetY, float blurRadius)
  {
    mHasDropShadow   = true;
    mHasGaussianBlur = false;
    return true;
  }

  bool ClearDropShadow()
  {
    mHasDropShadow = false;
    return true;
  }

  bool HasDropShadow() const
  {
    return mHasDropShadow;
  }

  bool SetGaussianBlur(float blurRadius)
  {
    mHasGaussianBlur = true;
    mHasDropShadow   = false;
    return true;
  }

  bool ClearGaussianBlur()
  {
    mHasGaussianBlur = false;
    return true;
  }

  bool HasGaussianBlur() const
  {
    return mHasGaussianBlur;
  }

  void SetEffectAutoPaddingEnabled(bool enable)
  {
    mEffectAutoPaddingEnabled = enable;
  }

  bool IsEffectAutoPaddingEnabled() const
  {
    return mEffectAutoPaddingEnabled;
  }

public:
  DrawableVector mDrawables;
  Dali::Texture  mTexture;
  Vector2        mSize;
  Vector2        mViewBox;
  bool           mHasDropShadow{false};
  bool           mHasGaussianBlur{false};
  bool           mEffectAutoPaddingEnabled{true};
};

inline CanvasRenderer& GetImplementation(Dali::CanvasRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "CanvasRenderer handle is empty.");
  BaseObject& handle = renderer.GetBaseObject();
  return static_cast<Internal::Adaptor::CanvasRenderer&>(handle);
}

inline const CanvasRenderer& GetImplementation(const Dali::CanvasRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "CanvasRenderer handle is empty.");
  const BaseObject& handle = renderer.GetBaseObject();
  return static_cast<const Internal::Adaptor::CanvasRenderer&>(handle);
}

// The assertion messages match dali-adaptor's, so DALI_TEST_ASSERTION expectations hold either way.
#define DEFINE_GET_IMPLEMENTATION(PublicType, ImplType, EmptyMessage)               \
  ImplType& GetImplementation(Dali::CanvasRenderer::PublicType& handle)             \
  {                                                                                 \
    DALI_ASSERT_ALWAYS(handle && EmptyMessage);                                     \
    return static_cast<ImplType&>(handle.GetBaseObject());                          \
  }                                                                                 \
  const ImplType& GetImplementation(const Dali::CanvasRenderer::PublicType& handle) \
  {                                                                                 \
    DALI_ASSERT_ALWAYS(handle && EmptyMessage);                                     \
    return static_cast<const ImplType&>(handle.GetBaseObject());                    \
  }

DEFINE_GET_IMPLEMENTATION(Drawable, Drawable, "Drawable handle is empty.")
DEFINE_GET_IMPLEMENTATION(Shape, Shape, "Shape handle is empty.")
DEFINE_GET_IMPLEMENTATION(DrawableGroup, DrawableGroup, "DrawableGroup handle is empty.")
DEFINE_GET_IMPLEMENTATION(Picture, Picture, "Picture handle is empty.")
DEFINE_GET_IMPLEMENTATION(Gradient, Gradient, "Gradient handle is empty.")
DEFINE_GET_IMPLEMENTATION(LinearGradient, LinearGradient, "LinearGradient handle is empty.")
DEFINE_GET_IMPLEMENTATION(RadialGradient, RadialGradient, "RadialGradient handle is empty.")

#undef DEFINE_GET_IMPLEMENTATION

} // namespace Adaptor
} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

CanvasRenderer CanvasRenderer::New(const Vector2& size)
{
  Internal::Adaptor::CanvasRenderer* renderer = new Internal::Adaptor::CanvasRenderer(size);
  return CanvasRenderer(renderer);
}

CanvasRenderer::CanvasRenderer()
{
}

CanvasRenderer::~CanvasRenderer()
{
}

CanvasRenderer::CanvasRenderer(Internal::Adaptor::CanvasRenderer* internal)
: BaseHandle(internal)
{
}

bool CanvasRenderer::Commit()
{
  return Internal::Adaptor::GetImplementation(*this).Commit();
}

bool CanvasRenderer::IsCanvasChanged() const
{
  return Internal::Adaptor::GetImplementation(*this).IsCanvasChanged();
}

bool CanvasRenderer::Rasterize()
{
  return Internal::Adaptor::GetImplementation(*this).Rasterize();
}

Dali::Texture CanvasRenderer::GetRasterizedTexture()
{
  return Internal::Adaptor::GetImplementation(*this).GetRasterizedTexture();
}

bool CanvasRenderer::AddDrawable(Dali::CanvasRenderer::Drawable drawable)
{
  return Internal::Adaptor::GetImplementation(*this).AddDrawable(drawable);
}

bool CanvasRenderer::RemoveDrawable(Dali::CanvasRenderer::Drawable drawable)
{
  return Internal::Adaptor::GetImplementation(*this).RemoveDrawable(drawable);
}

bool CanvasRenderer::RemoveAllDrawables()
{
  return Internal::Adaptor::GetImplementation(*this).RemoveAllDrawables();
}

bool CanvasRenderer::SetSize(const Vector2& size)
{
  return Internal::Adaptor::GetImplementation(*this).SetSize(size);
}

Vector2 CanvasRenderer::GetSize() const
{
  return Internal::Adaptor::GetImplementation(*this).GetSize();
}

bool CanvasRenderer::SetViewBox(const Vector2& viewBox)
{
  return Internal::Adaptor::GetImplementation(*this).SetViewBox(viewBox);
}

Vector2 CanvasRenderer::GetViewBox() const
{
  return Internal::Adaptor::GetImplementation(*this).GetViewBox();
}

bool CanvasRenderer::SetDropShadow(const Vector4& color, float offsetX, float offsetY, float blurRadius)
{
  return Internal::Adaptor::GetImplementation(*this).SetDropShadow(color, offsetX, offsetY, blurRadius);
}

bool CanvasRenderer::ClearDropShadow()
{
  return Internal::Adaptor::GetImplementation(*this).ClearDropShadow();
}

bool CanvasRenderer::HasDropShadow() const
{
  return Internal::Adaptor::GetImplementation(*this).HasDropShadow();
}

bool CanvasRenderer::SetGaussianBlur(float blurRadius)
{
  return Internal::Adaptor::GetImplementation(*this).SetGaussianBlur(blurRadius);
}

bool CanvasRenderer::ClearGaussianBlur()
{
  return Internal::Adaptor::GetImplementation(*this).ClearGaussianBlur();
}

bool CanvasRenderer::HasGaussianBlur() const
{
  return Internal::Adaptor::GetImplementation(*this).HasGaussianBlur();
}

void CanvasRenderer::SetEffectAutoPaddingEnabled(bool enable)
{
  Internal::Adaptor::GetImplementation(*this).SetEffectAutoPaddingEnabled(enable);
}

bool CanvasRenderer::IsEffectAutoPaddingEnabled() const
{
  return Internal::Adaptor::GetImplementation(*this).IsEffectAutoPaddingEnabled();
}

// CanvasRenderer::Drawable
//
CanvasRenderer::Drawable::Drawable()
{
}

CanvasRenderer::Drawable::~Drawable()
{
}

CanvasRenderer::Drawable::Drawable(Internal::Adaptor::Drawable* pImpl)
: BaseHandle(pImpl)
{
}

bool CanvasRenderer::Drawable::SetOpacity(float opacity)
{
  return Internal::Adaptor::GetImplementation(*this).SetOpacity(opacity);
}

float CanvasRenderer::Drawable::GetOpacity() const
{
  return Internal::Adaptor::GetImplementation(*this).GetOpacity();
}

bool CanvasRenderer::Drawable::Rotate(Degree degree)
{
  return Internal::Adaptor::GetImplementation(*this).Rotate(degree);
}

bool CanvasRenderer::Drawable::Scale(float factor)
{
  return Internal::Adaptor::GetImplementation(*this).Scale(factor);
}

bool CanvasRenderer::Drawable::Translate(const Vector2& translate)
{
  return Internal::Adaptor::GetImplementation(*this).Translate(translate);
}

bool CanvasRenderer::Drawable::Transform(const Dali::Matrix3& matrix)
{
  return Internal::Adaptor::GetImplementation(*this).Transform(matrix);
}

Bounds CanvasRenderer::Drawable::GetBoundingBox() const
{
  return Internal::Adaptor::GetImplementation(*this).GetBoundingBox();
}

bool CanvasRenderer::Drawable::SetClipPath(Drawable clip)
{
  return Internal::Adaptor::GetImplementation(*this).SetClipPath(clip);
}

bool CanvasRenderer::Drawable::SetMask(Drawable mask, MaskType type)
{
  return Internal::Adaptor::GetImplementation(*this).SetMask(mask, type);
}

CanvasRenderer::Drawable CanvasRenderer::Drawable::DownCast(BaseHandle handle)
{
  return CanvasRenderer::Drawable(dynamic_cast<Internal::Adaptor::Drawable*>(handle.GetObjectPtr()));
}

// CanvasRenderer::Shape
//
CanvasRenderer::Shape CanvasRenderer::Shape::New()
{
  return Shape(new Internal::Adaptor::Shape());
}

CanvasRenderer::Shape::Shape()
{
}

CanvasRenderer::Shape::~Shape()
{
}

CanvasRenderer::Shape::Shape(Internal::Adaptor::Shape* impl)
: CanvasRenderer::Drawable(impl)
{
}

bool CanvasRenderer::Shape::AddRect(const Bounds& rect, const Vector2& roundedCorner)
{
  return Internal::Adaptor::GetImplementation(*this).AddRect(rect, roundedCorner);
}

bool CanvasRenderer::Shape::AddCircle(const Vector2& center, const Vector2& radius)
{
  return Internal::Adaptor::GetImplementation(*this).AddCircle(center, radius);
}

bool CanvasRenderer::Shape::AddArc(const Vector2& center, float radius, float startAngle, float sweep, bool pie)
{
  return Internal::Adaptor::GetImplementation(*this).AddArc(center, radius, startAngle, sweep, pie);
}

bool CanvasRenderer::Shape::AddMoveTo(const Vector2& point)
{
  return Internal::Adaptor::GetImplementation(*this).AddMoveTo(point);
}

bool CanvasRenderer::Shape::AddLineTo(const Vector2& line)
{
  return Internal::Adaptor::GetImplementation(*this).AddLineTo(line);
}

bool CanvasRenderer::Shape::AddCubicTo(const Vector2& controlPoint1, const Vector2& controlPoint2, const Vector2& endPoint)
{
  return Internal::Adaptor::GetImplementation(*this).AddCubicTo(controlPoint1, controlPoint2, endPoint);
}

bool CanvasRenderer::Shape::AddPath(const Dali::Vector<PathCommandType>& commands, const Dali::Vector<Vector2>& points)
{
  return Internal::Adaptor::GetImplementation(*this).AddPath(commands, points);
}

bool CanvasRenderer::Shape::Close()
{
  return Internal::Adaptor::GetImplementation(*this).Close();
}

bool CanvasRenderer::Shape::ResetPath()
{
  return Internal::Adaptor::GetImplementation(*this).ResetPath();
}

bool CanvasRenderer::Shape::SetFillColor(const Vector4& color)
{
  return Internal::Adaptor::GetImplementation(*this).SetFillColor(color);
}

Vector4 CanvasRenderer::Shape::GetFillColor() const
{
  return Internal::Adaptor::GetImplementation(*this).GetFillColor();
}

bool CanvasRenderer::Shape::SetFillGradient(CanvasRenderer::Gradient gradient)
{
  return Internal::Adaptor::GetImplementation(*this).SetFillGradient(gradient);
}

CanvasRenderer::Gradient CanvasRenderer::Shape::GetFillGradient() const
{
  return Internal::Adaptor::GetImplementation(*this).GetFillGradient();
}

bool CanvasRenderer::Shape::SetFillRule(CanvasRenderer::Shape::FillRule rule)
{
  return Internal::Adaptor::GetImplementation(*this).SetFillRule(rule);
}

CanvasRenderer::Shape::FillRule CanvasRenderer::Shape::GetFillRule() const
{
  return Internal::Adaptor::GetImplementation(*this).GetFillRule();
}

bool CanvasRenderer::Shape::SetStrokeWidth(float width)
{
  return Internal::Adaptor::GetImplementation(*this).SetStrokeWidth(width);
}

float CanvasRenderer::Shape::GetStrokeWidth() const
{
  return Internal::Adaptor::GetImplementation(*this).GetStrokeWidth();
}

bool CanvasRenderer::Shape::SetStrokeColor(const Vector4& color)
{
  return Internal::Adaptor::GetImplementation(*this).SetStrokeColor(color);
}

Vector4 CanvasRenderer::Shape::GetStrokeColor() const
{
  return Internal::Adaptor::GetImplementation(*this).GetStrokeColor();
}

bool CanvasRenderer::Shape::SetStrokeGradient(CanvasRenderer::Gradient gradient)
{
  return Internal::Adaptor::GetImplementation(*this).SetStrokeGradient(gradient);
}

CanvasRenderer::Gradient CanvasRenderer::Shape::GetStrokeGradient() const
{
  return Internal::Adaptor::GetImplementation(*this).GetStrokeGradient();
}

bool CanvasRenderer::Shape::SetStrokeDash(const Dali::Vector<float>& dashPattern)
{
  return Internal::Adaptor::GetImplementation(*this).SetStrokeDash(dashPattern);
}

Dali::Vector<float> CanvasRenderer::Shape::GetStrokeDash() const
{
  return Internal::Adaptor::GetImplementation(*this).GetStrokeDash();
}

bool CanvasRenderer::Shape::SetStrokeCap(CanvasRenderer::Shape::StrokeCap cap)
{
  return Internal::Adaptor::GetImplementation(*this).SetStrokeCap(cap);
}

CanvasRenderer::Shape::StrokeCap CanvasRenderer::Shape::GetStrokeCap() const
{
  return Internal::Adaptor::GetImplementation(*this).GetStrokeCap();
}

bool CanvasRenderer::Shape::SetStrokeJoin(CanvasRenderer::Shape::StrokeJoin join)
{
  return Internal::Adaptor::GetImplementation(*this).SetStrokeJoin(join);
}

CanvasRenderer::Shape::StrokeJoin CanvasRenderer::Shape::GetStrokeJoin() const
{
  return Internal::Adaptor::GetImplementation(*this).GetStrokeJoin();
}

// CanvasRenderer::DrawableGroup
//
CanvasRenderer::DrawableGroup CanvasRenderer::DrawableGroup::New()
{
  return DrawableGroup(new Internal::Adaptor::DrawableGroup());
}

CanvasRenderer::DrawableGroup::DrawableGroup()
{
}

CanvasRenderer::DrawableGroup::~DrawableGroup()
{
}

CanvasRenderer::DrawableGroup::DrawableGroup(Internal::Adaptor::DrawableGroup* impl)
: CanvasRenderer::Drawable(impl)
{
}

bool CanvasRenderer::DrawableGroup::AddDrawable(Drawable drawable)
{
  return Internal::Adaptor::GetImplementation(*this).AddDrawable(drawable);
}

bool CanvasRenderer::DrawableGroup::RemoveDrawable(Drawable drawable)
{
  return Internal::Adaptor::GetImplementation(*this).RemoveDrawable(drawable);
}

bool CanvasRenderer::DrawableGroup::RemoveAllDrawables()
{
  return Internal::Adaptor::GetImplementation(*this).RemoveAllDrawables();
}

// CanvasRenderer::Picture
//
CanvasRenderer::Picture CanvasRenderer::Picture::New()
{
  return Picture(new Internal::Adaptor::Picture());
}

CanvasRenderer::Picture::Picture()
{
}

CanvasRenderer::Picture::~Picture()
{
}

CanvasRenderer::Picture::Picture(Internal::Adaptor::Picture* impl)
: CanvasRenderer::Drawable(impl)
{
}

bool CanvasRenderer::Picture::Load(const Dali::StringView& url)
{
  return Internal::Adaptor::GetImplementation(*this).Load(url);
}

bool CanvasRenderer::Picture::SetSize(const Vector2& size)
{
  return Internal::Adaptor::GetImplementation(*this).SetSize(size);
}

Vector2 CanvasRenderer::Picture::GetSize() const
{
  return Internal::Adaptor::GetImplementation(*this).GetSize();
}

// CanvasRenderer::Gradient
//
CanvasRenderer::Gradient::Gradient()
{
}

CanvasRenderer::Gradient::~Gradient()
{
}

CanvasRenderer::Gradient::Gradient(Internal::Adaptor::Gradient* pImpl)
: BaseHandle(pImpl)
{
}

bool CanvasRenderer::Gradient::AddColorStop(float offset, const Vector4& color)
{
  return Internal::Adaptor::GetImplementation(*this).AddColorStop(offset, color);
}

bool CanvasRenderer::Gradient::ClearColorStops()
{
  return Internal::Adaptor::GetImplementation(*this).ClearColorStops();
}

uint32_t CanvasRenderer::Gradient::GetColorStopCount() const
{
  return Internal::Adaptor::GetImplementation(*this).GetColorStopCount();
}

float CanvasRenderer::Gradient::GetColorStopOffset(uint32_t index) const
{
  return Internal::Adaptor::GetImplementation(*this).GetColorStopOffset(index);
}

Vector4 CanvasRenderer::Gradient::GetColorStopColor(uint32_t index) const
{
  return Internal::Adaptor::GetImplementation(*this).GetColorStopColor(index);
}

bool CanvasRenderer::Gradient::SetSpread(Dali::CanvasRenderer::Gradient::Spread spread)
{
  return Internal::Adaptor::GetImplementation(*this).SetSpread(spread);
}

Dali::CanvasRenderer::Gradient::Spread CanvasRenderer::Gradient::GetSpread() const
{
  return Internal::Adaptor::GetImplementation(*this).GetSpread();
}

CanvasRenderer::Gradient CanvasRenderer::Gradient::DownCast(BaseHandle handle)
{
  return CanvasRenderer::Gradient(dynamic_cast<Internal::Adaptor::Gradient*>(handle.GetObjectPtr()));
}

// CanvasRenderer::LinearGradient
//
CanvasRenderer::LinearGradient CanvasRenderer::LinearGradient::New()
{
  return LinearGradient(new Internal::Adaptor::LinearGradient());
}

CanvasRenderer::LinearGradient::LinearGradient()
{
}

CanvasRenderer::LinearGradient::~LinearGradient()
{
}

CanvasRenderer::LinearGradient::LinearGradient(Internal::Adaptor::LinearGradient* impl)
: CanvasRenderer::Gradient(impl)
{
}

bool CanvasRenderer::LinearGradient::SetBounds(const Vector2& firstPoint, const Vector2& secondPoint)
{
  return Internal::Adaptor::GetImplementation(*this).SetBounds(firstPoint, secondPoint);
}

bool CanvasRenderer::LinearGradient::GetBounds(Vector2& firstPoint, Vector2& secondPoint) const
{
  return Internal::Adaptor::GetImplementation(*this).GetBounds(firstPoint, secondPoint);
}

// CanvasRenderer::RadialGradient
//
CanvasRenderer::RadialGradient CanvasRenderer::RadialGradient::New()
{
  return RadialGradient(new Internal::Adaptor::RadialGradient());
}

CanvasRenderer::RadialGradient::RadialGradient()
{
}

CanvasRenderer::RadialGradient::~RadialGradient()
{
}

CanvasRenderer::RadialGradient::RadialGradient(Internal::Adaptor::RadialGradient* impl)
: CanvasRenderer::Gradient(impl)
{
}

bool CanvasRenderer::RadialGradient::SetBounds(const Vector2& centerPoint, float radius)
{
  return Internal::Adaptor::GetImplementation(*this).SetBounds(centerPoint, radius);
}

bool CanvasRenderer::RadialGradient::GetBounds(Vector2& centerPoint, float& radius) const
{
  return Internal::Adaptor::GetImplementation(*this).GetBounds(centerPoint, radius);
}

} // namespace Dali
