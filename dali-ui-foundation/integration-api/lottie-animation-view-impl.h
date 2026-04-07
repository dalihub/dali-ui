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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/intrusive-ptr.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class LottieAnimationViewImpl;
using LottieAnimationViewImplPtr = IntrusivePtr<LottieAnimationViewImpl>;

/**
 * @brief This is the internal implementation class for LottieAnimationView.
 *
 * @see Dali::Ui::LottieAnimationView
 */
class DALI_UI_API LottieAnimationViewImpl : public ViewImpl
{
public: // Properties
  /// @brief The start and end property ranges for this impl.
  enum
  {
    PROPERTY_START_INDEX = Dali::PROPERTY_REGISTRATION_START_INDEX,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the LottieAnimationViewImpl class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief The URL of the Lottie JSON file.
       * @details Name "image", type Property::STRING.
       */
      IMAGE = PROPERTY_START_INDEX,
    };
  };

protected: // Construction & Destruction
  /**
   * @brief LottieAnimationViewImpl constructor.
   */
  LottieAnimationViewImpl();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~LottieAnimationViewImpl();

public: // Creation
  /**
   * @brief Creates a new LottieAnimationViewImpl.
   *
   * @return An intrusive pointer to the newly allocated LottieAnimationViewImpl
   */
  static LottieAnimationViewImplPtr New();

public: // Properties (required by DALI_PROPERTY_REGISTRATION)
  /// @cond internal
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);
  /// @endcond

public: // API
  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetResourceUrl
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @brief Gets the current Lottie animation file URL.
   *
   * @return The URL of the Lottie JSON file currently set on this view
   */
  Dali::String GetResourceUrl() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::Play
   */
  void Play();

  /**
   * @copydoc Dali::Ui::LottieAnimationView::Pause
   */
  void Pause();

  /**
   * @copydoc Dali::Ui::LottieAnimationView::Stop
   */
  void Stop();

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetLoopCount
   */
  void SetLoopCount(int count);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetLoopCount
   */
  int GetLoopCount() const;

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
   * @brief Rebuilds and re-registers the Lottie visual from current property values.
   */
  void UpdateVisual();

private:
  // Not copyable or movable
  LottieAnimationViewImpl(const LottieAnimationViewImpl&)            = delete;
  LottieAnimationViewImpl(LottieAnimationViewImpl&&)                 = delete;
  LottieAnimationViewImpl& operator=(const LottieAnimationViewImpl&) = delete;
  LottieAnimationViewImpl& operator=(LottieAnimationViewImpl&&)      = delete;

private: // Data
  Dali::String mUrl;
  int          mLoopCount;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
