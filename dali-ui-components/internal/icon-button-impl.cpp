#include <dali-ui-components/internal/icon-button-impl.h>
#include <dali-ui-components/internal/styles/style-validation.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <algorithm>
#include <cmath>

namespace DALI_NAMESPACE::Ui::Internal
{
namespace
{
BaseHandle Create()
{
  return BaseHandle();
}

// The callbacks use only the child argument: an application may retain the
// exposed ImageView after destroying its IconButton.
MeasuredSize MeasureIcon(Ui::View view, float widthConstraint, float heightConstraint)
{
  auto          image              = Ui::ImageView::DownCast(view);
  const Vector3 natural            = image.GetNaturalSize(); // Also starts deferred loading.
  const bool    ready              = image.GetLoadingStatus() == Ui::Visual::ResourceStatus::READY;
  const bool    naturalWidthValid  = ready && std::isfinite(natural.x) && natural.x > 0.0f;
  const bool    naturalHeightValid = ready && std::isfinite(natural.y) && natural.y > 0.0f;
  const float   scale              = Ui::GetImpl(view).GetEffectiveScale();
  const float   requestedWidth     = view.GetRequestedWidth();
  const float   requestedHeight    = view.GetRequestedHeight();
  const bool    widthFixed         = requestedWidth >= 0.0f || requestedWidth == MATCH_PARENT;
  const bool    heightFixed        = requestedHeight >= 0.0f || requestedHeight == MATCH_PARENT;
  float         width              = naturalWidthValid ? natural.x : 56.0f;
  float         height             = naturalHeightValid ? natural.y : 56.0f;
  if(requestedWidth >= 0.0f)
  {
    width = requestedWidth;
  }
  else if(requestedWidth == MATCH_PARENT)
  {
    width = scale > 0.0f ? std::max(0.0f, widthConstraint / scale) : 0.0f;
  }
  if(requestedHeight >= 0.0f)
  {
    height = requestedHeight;
  }
  else if(requestedHeight == MATCH_PARENT)
  {
    height = scale > 0.0f ? std::max(0.0f, heightConstraint / scale) : 0.0f;
  }
  if(naturalWidthValid && naturalHeightValid)
  {
    if(widthFixed && width > 0.0f && !heightFixed)
    {
      height = width * (natural.y / natural.x);
    }
    else if(heightFixed && height > 0.0f && !widthFixed)
    {
      width = height * (natural.x / natural.y);
    }
  }
  return MeasuredSize(width * scale, height * scale);
}

void OnIconResourceReady(Ui::View view)
{
  view.InvalidateMeasure();
}
DALI_TYPE_REGISTRATION_BEGIN(IconButtonImpl, Extension::InteractiveViewImpl, Create)
DALI_TYPE_REGISTRATION_END()
} //namespace
Ui::IconButton IconButtonImpl::New(IconButtonStyle style)
{
  DALI_ASSERT_ALWAYS(style && "IconButtonStyle must be initialized");

  IntrusivePtr<IconButtonImpl> impl(new IconButtonImpl());
  Ui::IconButton               handle(*impl);
  impl->Initialize();
  impl->ApplyInitialStyle(style);
  return handle;
}
void IconButtonImpl::OnInitialize()
{
  Ui::Extension::InteractiveViewImpl::OnInitialize();
  Ui::View self = Ui::View::DownCast(Self());
  self.SetAccessibilityRole(Accessibility::Role::BUTTON);
  self.StateChangedSignal().Connect(this, &IconButtonImpl::OnViewStateChanged);
  self.SetStateEffect(OverlayEffect::Round());
  mIcon = Ui::ImageView::New();
  mIcon.SetAccessibilityHidden(true);
  mIcon.SetMeasureCallback(MeasureCallback::New(&MeasureIcon));
  mIcon.ResourceReadySignal().Connect(&OnIconResourceReady);
  Self().Add(mIcon);
}
void IconButtonImpl::OnViewStateChanged(Ui::View view, StateEvent event)
{
  if(event.Added(ViewState::DISABLED))
  {
    view.RemoveAccessibilityState(Accessibility::State::ENABLED);
  }
  else if(event.Removed(ViewState::DISABLED))
  {
    view.AddAccessibilityState(Accessibility::State::ENABLED);
  }
}
void IconButtonImpl::ApplyInitialStyle(IconButtonStyle style)
{
  SetIconUrl(style.GetIconUrl());
  SetIconWidth(style.GetIconWidth());
  SetIconHeight(style.GetIconHeight());
  SetIconColor(style.GetIconColor());
  Ui::View::DownCast(Self()).SetPadding(style.GetPadding());
}
void IconButtonImpl::SetIconUrl(const Dali::String& iconUrl)
{
  mIcon.SetResourceUrl(iconUrl);
  mIcon.InvalidateMeasure();
}
Dali::String IconButtonImpl::GetIconUrl() const
{
  return mIcon.GetResourceUrl();
}
void IconButtonImpl::SetIconWidth(float width)
{
  DALI_ASSERT_ALWAYS(StyleValidation::IsRequestedDimension(width) && "IconButton icon dimension must be finite and non-negative or a layout sentinel");
  mIcon.SetRequestedWidth(width);
}
float IconButtonImpl::GetIconWidth() const
{
  return mIcon.GetRequestedWidth();
}
void IconButtonImpl::SetIconHeight(float height)
{
  DALI_ASSERT_ALWAYS(StyleValidation::IsRequestedDimension(height) && "IconButton icon dimension must be finite and non-negative or a layout sentinel");
  mIcon.SetRequestedHeight(height);
}
float IconButtonImpl::GetIconHeight() const
{
  return mIcon.GetRequestedHeight();
}
void IconButtonImpl::SetIconColor(const UiColor& color)
{
  mIcon.SetImageColor(color);
}
UiColor IconButtonImpl::GetIconColor() const
{
  return const_cast<Ui::ImageView&>(mIcon).GetImageColor();
}
void IconButtonImpl::SetIconCornerRadius(const Vector4& radius)
{
  DALI_ASSERT_ALWAYS(StyleValidation::IsNonNegative(radius) && "IconButton corner radius must be finite and non-negative");
  mIcon.SetCornerRadius(radius);
}
Vector4 IconButtonImpl::GetIconCornerRadius() const
{
  return mIcon.GetCornerRadius();
}
} //namespace DALI_NAMESPACE::Ui::Internal
