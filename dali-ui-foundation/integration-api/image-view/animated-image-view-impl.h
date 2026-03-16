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

#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class AnimatedImageViewImpl;
using AnimatedImageViewImplPtr = IntrusivePtr<AnimatedImageViewImpl>;

/**
 * @brief This is the internal implementation class for AnimatedImageView.
 *
 * @see Dali::Ui::AnimatedImageView
 */
class DALI_UI_API AnimatedImageViewImpl : public ViewImpl
{
public: // Properties
  /// @brief The start and end property ranges for this impl.
  enum
  {
    PROPERTY_START_INDEX = Dali::PROPERTY_REGISTRATION_START_INDEX,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the AnimatedImageViewImpl class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief The URL of the animated image resource.
       * @details Name "image", type Property::STRING.
       */
      IMAGE = PROPERTY_START_INDEX,
    };
  };

protected: // Construction & Destruction
  /**
   * @brief AnimatedImageViewImpl constructor.
   */
  AnimatedImageViewImpl();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~AnimatedImageViewImpl();

public: // Creation
  /**
   * @brief Creates a new AnimatedImageViewImpl.
   *
   * @return An intrusive pointer to the newly allocated AnimatedImageViewImpl
   */
  static AnimatedImageViewImplPtr New();

public: // Properties (required by DALI_PROPERTY_REGISTRATION)
  /// @cond internal
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);
  /// @endcond

public: // API
  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetResourceUrl
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetResourceUrl
   */
  Dali::String GetResourceUrl() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::Play
   */
  void Play();

  /**
   * @copydoc Dali::Ui::AnimatedImageView::Pause
   */
  void Pause();

  /**
   * @copydoc Dali::Ui::AnimatedImageView::Stop
   */
  void Stop();

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetLoopCount
   */
  void SetLoopCount(int count);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetLoopCount
   */
  int GetLoopCount() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetImageColor
   */
  void SetImageColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetImageColor
   */
  UiColor GetImageColor() const;

public: // Loading Status
  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetLoadingStatus
   */
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

private: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Integration::ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

private: // Internal methods
  /**
   * @brief Rebuilds and re-registers the image visual from current property values.
   */
  void UpdateVisual();

private:
  // Not copyable or movable
  AnimatedImageViewImpl(const AnimatedImageViewImpl&)            = delete;
  AnimatedImageViewImpl(AnimatedImageViewImpl&&)                 = delete;
  AnimatedImageViewImpl& operator=(const AnimatedImageViewImpl&) = delete;
  AnimatedImageViewImpl& operator=(AnimatedImageViewImpl&&)      = delete;

private: // Data
  Dali::String mUrl;
  int          mLoopCount;
  UiColor      mImageColor;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
