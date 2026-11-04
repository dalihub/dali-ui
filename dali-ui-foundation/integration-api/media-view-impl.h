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
#include <dali/integration-api/adaptor-framework/media-view-backend.h>
// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/public-api/views/media/media-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
/**
 * @brief Registers a streamed image visual for the shared frame backend and manages its lifetime.
 */
class DALI_UI_API MediaViewImpl : public ViewImpl
{
public:
  MediaViewImpl();
  ~MediaViewImpl() override;
  static Ui::MediaView                 New();
  Dali::Integration::MediaViewBackend& Backend()
  {
    return *mBackend;
  }

  const Dali::Integration::MediaViewBackend& Backend() const
  {
    return *mBackend;
  }

private:
  /**
   * @brief Updates the visual from an event-thread presentation notification.
   */
  void OnPresentationChanged();

  /**
   * @brief Unregisters the content visual.
   */
  void ClearVisual();

  /**
   * @copydoc ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc ViewImpl::OnArrange
   */
  LayoutRect OnArrange(const LayoutRect& bounds) override;

  void OnInitialize() override;
  void OnSceneConnection(int depth) override;
  void OnSceneDisconnection() override;
  MediaViewImpl(const MediaViewImpl&)                                                  = delete;
  MediaViewImpl&                                       operator=(const MediaViewImpl&) = delete;
  Ui::Integration::Visual::Base                        mVisual;
  Dali::UniquePtr<Dali::Integration::MediaViewBackend> mBackend;

  /**
   * @brief Stores the property index allocated for this view's content visual.
   */
  Dali::Property::Index mContentVisualIndex{Dali::Property::INVALID_INDEX};
};
} // namespace Integration
} // namespace Ui
} // namespace DALI_NAMESPACE
