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
#include <dali-ui-foundation/internal/render-effects/render-effect-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-utility.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
static constexpr uint32_t RENDER_EFFECT_RENDER_PASS_TAG = 11;
static constexpr float    DEFAULT_DITHER_NOISE_STRENGTH = 0.1f;
static constexpr float    DITHER_NOISE_UNIFORM_SCALE    = 0.2f;

float GetDitherNoiseUniformStrength(float strength)
{
  return Dali::Clamp(strength, 0.0f, 1.0f) * DITHER_NOISE_UNIFORM_SCALE;
}
} // namespace

#if defined(DEBUG_ENABLED)
// Keep this log filter inside of Dali::Ui::Internal, so subclass of RenderEffect can also use this.
Debug::Filter* gRenderEffectLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_EFFECT");
#endif

uint32_t RenderEffectImpl::GetRenderPassTag()
{
  return RENDER_EFFECT_RENDER_PASS_TAG;
}

RenderEffectImpl::RenderEffectImpl()
: mRenderer(),
  mOwnerView(),
  mTargetSize(Vector2::ZERO),
  mDitherNoiseStrength(DEFAULT_DITHER_NOISE_STRENGTH),
  mIsActivated(false)
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[RenderEffect:%p] Constructor\n", this);
}

RenderEffectImpl::~RenderEffectImpl()
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[RenderEffect:%p] Destructor.\n", this);

  // Reset weak handle first. (Since it might not valid during destruction.)
  mOwnerView.Reset();
  mPlacementSceneHolder.Reset();

  // Don't call Deactivate here, since we cannot call virtual function during destruction.
  // Deactivate already be called at View's destructor, and InheritVisibilityChanged signal.
}

void RenderEffectImpl::SetOwnerView(Dali::Ui::View view)
{
  Dali::Ui::View ownerView = mOwnerView.GetHandle();
  if(ownerView != view)
  {
    // Clear previous owner view
    ClearOwnerView();

    mOwnerView = (ownerView = view);

    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] SetOwnerView [ID:%d]\n", this,
                  ownerView ? ownerView.GetProperty<int>(Actor::Property::ID) : -1);

    if(ownerView)
    {
      UpdateTargetSize();

      if(mAnimationConstraints.empty())
      {
        Renderer        renderer    = GetTargetRenderer();
        Property::Index radiusIndex = renderer.GetPropertyIndex("uCornerRadius");
        if(radiusIndex != Property::INVALID_INDEX)
        {
          Constraint cornerRadiusConstraint = Constraint::New<Vector4>(renderer, radiusIndex, EqualToConstraint());
          cornerRadiusConstraint.AddSource(Source(ownerView, Ui::View::Property::CORNER_RADIUS));
          cornerRadiusConstraint.Apply();
          mAnimationConstraints.push_back(cornerRadiusConstraint);
        }

        Property::Index squarenessIndex = renderer.GetPropertyIndex("uCornerSquareness");
        if(squarenessIndex != Property::INVALID_INDEX)
        {
          Constraint cornerSquarenessConstraint =
            Constraint::New<Vector4>(renderer, squarenessIndex, EqualToConstraint());
          cornerSquarenessConstraint.AddSource(Source(ownerView, Ui::View::Property::CORNER_SQUARENESS));
          cornerSquarenessConstraint.Apply();
          mAnimationConstraints.push_back(cornerSquarenessConstraint);
        }
      }

      ownerView.EffectiveVisibilityChangedSignal().Connect(this,
                                                           &RenderEffectImpl::OnViewEffectiveVisibilityChanged);

      Activate(); // Dev note : Activate after set the owner view.
    }
  }
}

void RenderEffectImpl::ClearOwnerView()
{
  Deactivate(); // Dev note : Deactivate before clearing the owner view.

  for(auto constraint : mAnimationConstraints)
  {
    constraint.Remove();
  }
  mAnimationConstraints.clear();

  Dali::Ui::View ownerView = mOwnerView.GetHandle();
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] ClearOwnerView [ID:%d]\n", this,
                ownerView ? ownerView.GetProperty<int>(Actor::Property::ID) : -1);
  if(ownerView)
  {
    ownerView.EffectiveVisibilityChangedSignal().Disconnect(this,
                                                            &RenderEffectImpl::OnViewEffectiveVisibilityChanged);

    auto previousOwnerView = ownerView;
    mOwnerView.Reset();
    mPlacementSceneHolder.Reset();

    // Make previous owner don't have render effect, after make we don't have owner view now.
    if(previousOwnerView.GetRenderEffect().GetObjectPtr() == this)
    {
      previousOwnerView.ClearRenderEffect();
    }
  }
}

bool RenderEffectImpl::IsActivated() const
{
  return mIsActivated;
}

void RenderEffectImpl::Initialize()
{
  if(!mRenderer)
  {
    mRenderer =
      CreateRenderer(SHADER_RENDER_EFFECT_VERT, SHADER_RENDER_EFFECT_FRAG,
                     static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL),
                     "RENDER_EFFECT", Uint16Pair(1, 1));
    mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true); // Always use pre-multiply alpha

    mRenderer.RegisterProperty("uCornerRadius", Vector4::ZERO);
    mRenderer.RegisterProperty("uCornerSquareness", Vector4::ZERO);
    mRenderer.RegisterProperty("uCornerRadiusPolicy", static_cast<float>(1.0f));
    mRenderer.RegisterProperty("uDitherNoiseStrength", GetDitherNoiseUniformStrength(mDitherNoiseStrength));
  }

  OnInitialize();
}

Ui::View RenderEffectImpl::GetOwnerView() const
{
  return mOwnerView.GetHandle();
}

Dali::Integration::SceneHolder RenderEffectImpl::GetSceneHolder() const
{
  return mPlacementSceneHolder.GetHandle();
}

Renderer RenderEffectImpl::GetTargetRenderer() const
{
  return mRenderer;
}

Vector2 RenderEffectImpl::GetTargetSize() const
{
  return mTargetSize;
}

void RenderEffectImpl::SetDitherNoiseStrength(float strength)
{
  mDitherNoiseStrength = Dali::Clamp(strength, 0.0f, 1.0f);

  Renderer renderer = GetTargetRenderer();
  if(renderer)
  {
    renderer.RegisterProperty("uDitherNoiseStrength", GetDitherNoiseUniformStrength(mDitherNoiseStrength));
  }
}

float RenderEffectImpl::GetDitherNoiseStrength() const
{
  return mDitherNoiseStrength;
}

void RenderEffectImpl::Activate()
{
  if(!IsActivated() && IsActivateValid())
  {
    Dali::Ui::View ownerView = mOwnerView.GetHandle();
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] Activated! [ID:%d]\n", this,
                  ownerView ? ownerView.GetProperty<int>(Actor::Property::ID) : -1);

    // Keep sceneHolder as weak handle.
    Dali::Integration::SceneHolder sceneHolder = Dali::Integration::SceneHolder::Get(ownerView);
    if(DALI_UNLIKELY(!sceneHolder))
    {
      DALI_LOG_ERROR("RenderEffect Could not be activated due to ownerView's SceneHolder is not exist\n");
      return;
    }
    mIsActivated = true;

    mPlacementSceneHolder = sceneHolder;

    // Activate logic for subclass.
    OnActivate();

    // Set round corner. Default is to sync to owner view's BACKGROUND.
    Vector4 cornerRadius = ownerView.GetProperty<Vector4>(Ui::View::Property::CORNER_RADIUS);
    if(cornerRadius != Vector4::ZERO)
    {
      int32_t cornerRadiusPolicy = ownerView.GetProperty<int32_t>(Ui::View::Property::CORNER_RADIUS_POLICY);
      Vector4 cornerSquareness   = ownerView.GetProperty<Vector4>(Ui::View::Property::CORNER_SQUARENESS);

      Property::Map map;
      map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS, cornerRadius);
      map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS_POLICY,
                 static_cast<Ui::Visual::Transform::Policy::Type>(cornerRadiusPolicy));
      map.Insert(Ui::DevelVisual::Property::CORNER_SQUARENESS, cornerSquareness);

      SetCornerConstants(map);
    }
  }
}

void RenderEffectImpl::Deactivate()
{
  if(IsActivated())
  {
    mIsActivated = false;

    Dali::Ui::View ownerView = mOwnerView.GetHandle();
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] Deactivated! [ID:%d]\n", this,
                  ownerView ? ownerView.GetProperty<int>(Actor::Property::ID) : -1);

    // Deactivate logic for subclass.
    OnDeactivate();
  }
}

void RenderEffectImpl::Refresh()
{
  Dali::Ui::View ownerView = mOwnerView.GetHandle();
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] Refresh! [ID:%d]\n", this,
                ownerView ? ownerView.GetProperty<int>(Actor::Property::ID) : -1);
  if(ownerView)
  {
    UpdateTargetSize();

    if(IsActivateValid())
    {
      if(!IsActivated())
      {
        Activate();
      }
      else
      {
        DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] OnRefresh()! [ID:%d]\n", this,
                      ownerView ? ownerView.GetProperty<int>(Actor::Property::ID) : -1);
        OnRefresh();
      }
    }
    else
    {
      Deactivate();
    }
  }
}

bool RenderEffectImpl::IsActivateValid() const
{
  bool ret = false;

  Vector2 size = GetTargetSize();
  if(size.x > Math::MACHINE_EPSILON_1000 && size.y > Math::MACHINE_EPSILON_1000)
  {
    Dali::Ui::View ownerView = mOwnerView.GetHandle();
    if(ownerView && DevelActor::IsEffectivelyVisible(ownerView))
    {
      ret = true;
    }
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Concise,
                  "[RenderEffect:%p] IsActivateValid? [ID:%d][size:%fx%f][ret:%d]\n", this,
                  ownerView ? ownerView.GetProperty<int>(Actor::Property::ID) : -1, size.x, size.y, ret);
  }
  else
  {
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Concise,
                  "[RenderEffect:%p] IsActivateValid? [ID:%d][size:%fx%f][ret:%d]\n", this, -1, size.x, size.y, ret);
  }

  return ret;
}

void RenderEffectImpl::UpdateTargetSize()
{
  Dali::Ui::View ownerView = mOwnerView.GetHandle();
  Vector2        size      = Vector2::ZERO;
  if(ownerView)
  {
    size = ownerView.GetProperty<Vector2>(Actor::Property::SIZE);
    if(size == Vector2::ZERO)
    {
      size = ownerView.GetNaturalSize();
    }

    if(size.x < 0.0f || size.y < 0.0f)
    {
      size = Vector2::ZERO;
    }

    const uint32_t maxTextureSize = Dali::GetMaxTextureSize();
    if(static_cast<uint32_t>(size.x) > maxTextureSize || static_cast<uint32_t>(size.y) > maxTextureSize)
    {
      const uint32_t denominator = static_cast<uint32_t>(std::max(size.x, size.y));

      size.x = (size.x * maxTextureSize / denominator);
      size.y = (size.y * maxTextureSize / denominator);
    }
  }

  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Concise,
                "[RenderEffect:%p] UpdateTargetSize [ID:%d][prev:%fx%f][size:%fx%f]\n", this,
                ownerView ? ownerView.GetProperty<int>(Actor::Property::ID) : -1, mTargetSize.x, mTargetSize.y,
                size.x, size.y);
  mTargetSize = size;
}

void RenderEffectImpl::OnViewEffectiveVisibilityChanged(Actor actor, bool visible)
{
  Dali::Ui::View ownerView = mOwnerView.GetHandle();
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Concise, "[RenderEffect:%p] visibility changed [ID:%d][visible:%d]\n",
                this, ownerView ? ownerView.GetProperty<int>(Actor::Property::ID) : -1, visible);
  if(visible)
  {
    Activate();
  }
  else
  {
    Deactivate();
  }
}

void RenderEffectImpl::SetCornerConstants(const Property::Map& map)
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[RenderEffect:%p] Set corner radius constants to shader\n",
                this);

  Vector4 radius = Vector4::ZERO;
  map[Ui::DevelVisual::Property::CORNER_RADIUS].Get(radius);

  Vector4 squareness = Vector4::ZERO;
  map[Ui::DevelVisual::Property::CORNER_SQUARENESS].Get(squareness);

  Ui::Visual::Transform::Policy::Type policy = Ui::Visual::Transform::Policy::Type::ABSOLUTE;
  map[Ui::DevelVisual::Property::CORNER_RADIUS_POLICY].Get(policy);

  Renderer renderer = GetTargetRenderer();
  renderer.RegisterProperty("uCornerRadius", radius);
  renderer.RegisterProperty("uCornerSquareness", squareness);
  renderer.RegisterProperty("uCornerRadiusPolicy", static_cast<float>(policy));
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
