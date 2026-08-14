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

#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
class FittingModeTestVisual : public Dali::Ui::Internal::Visual::Base
{
public:
  using Ptr = IntrusivePtr<FittingModeTestVisual>;

  static Ptr New(Dali::Ui::Internal::VisualFactoryCache&         factoryCache,
                 Dali::Ui::Integration::InternalVisualType type = Dali::Ui::Integration::InternalVisualType::IMAGE)
  {
    Ptr visual(new FittingModeTestVisual(factoryCache, type));
    visual->Initialize();
    return visual;
  }

  int     applyCount{0};
  Vector2 lastControlSize{Vector2::ZERO};

protected:
  FittingModeTestVisual(Dali::Ui::Internal::VisualFactoryCache& factoryCache,
                        Dali::Ui::Integration::InternalVisualType type)
  : Dali::Ui::Internal::Visual::Base(factoryCache, type)
  {
    mImpl->mFittingModeRequired = true;
  }

  ~FittingModeTestVisual() override = default;

  void OnInitialize() override
  {
  }

  void DoCreatePropertyMap(Property::Map&) const override
  {
  }

  void DoCreateInstancePropertyMap(Property::Map&) const override
  {
  }

  void DoSetProperties(const Property::Map&) override
  {
  }

  void OnSetTransform() override
  {
  }

  void DoSetOnScene(Actor&) override
  {
  }

  void OnApplyFittingMode(const Vector2& controlSize, const Insets&, float) override
  {
    ++applyCount;
    lastControlSize = controlSize;
  }
};
} // namespace

void utc_dali_view_fitting_mode_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_fitting_mode_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliViewFittingModeAppliedAfterLayout(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              view   = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);

  auto  factory      = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();

  auto fittingVisualA = FittingModeTestVisual::New(factoryCache);
  auto fittingVisualB = FittingModeTestVisual::New(factoryCache);

  Dali::Ui::Integration::Visual::Base visualA(fittingVisualA.Get());
  Dali::Ui::Integration::Visual::Base visualB(fittingVisualB.Get());

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));
  viewData.RegisterVisual(View::Property::BACKGROUND, visualA);
  viewData.RegisterVisual(View::Property::SHADOW, visualB);

  window.Add(view);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();

  DALI_TEST_EQUALS(fittingVisualA->applyCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingVisualB->applyCount, 0, TEST_LOCATION);

  application.SendNotification();

  const Vector2 arrangedSize(
    view.GetProperty<float>(Actor::Property::SIZE_WIDTH),
    view.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  DALI_TEST_CHECK(arrangedSize.width > 0.0f);
  DALI_TEST_CHECK(arrangedSize.height > 0.0f);
  DALI_TEST_EQUALS(fittingVisualA->applyCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingVisualB->applyCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingVisualA->lastControlSize, arrangedSize, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingVisualB->lastControlSize, arrangedSize, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewFittingModeAfterLayoutSkipsText(void)
{
  UiTestApplication application;
  View              view = View::New();

  auto  factory      = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();

  auto fittingImageVisual = FittingModeTestVisual::New(factoryCache);
  auto fittingTextVisual  = FittingModeTestVisual::New(factoryCache, Dali::Ui::Integration::InternalVisualType::TEXT);

  Dali::Ui::Integration::Visual::Base imageVisual(fittingImageVisual.Get());
  Dali::Ui::Integration::Visual::Base textVisual(fittingTextVisual.Get());

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));
  viewData.RegisterVisual(View::Property::BACKGROUND, imageVisual);
  viewData.RegisterVisual(View::Property::SHADOW, textVisual);

  viewData.EmitLayoutFinishedSignal(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  DALI_TEST_EQUALS(fittingImageVisual->applyCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingTextVisual->applyCount, 0, TEST_LOCATION);

  END_TEST;
}
