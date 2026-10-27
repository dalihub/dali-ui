#ifndef DALI_CHART_SAMPLE_CHART_CASE_PRESENTER_H
#define DALI_CHART_SAMPLE_CHART_CASE_PRESENTER_H

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
 */

#include <dali-ui-components/dali-ui-components.h>
#include <dali/public-api/common/unique-ptr.h>
#include <functional>
#include "chart-number-transition.h"

namespace ChartStandard
{
/**
 * @brief Owns one native case presentation; persistent state belongs to the gallery.
 */
class CasePresenter : public Dali::ConnectionTracker
{
public:
  /**
   * @brief Creates a card while retaining references to its specification and persistent state.
   *
   * @param[in] spec The authored specification, which must outlive this presenter
   * @param[in] state The persistent state, which must outlive this presenter
   * @param[in] width The card width in pixels
   * @param[in] height The card height in pixels
   * @param[in] dark Whether to use dark-theme colors
   * @param[in] freeze Whether to disable sample transitions
   */
  CasePresenter(const ReferenceCase& spec, CaseState& state, float width, float height, bool dark, bool freeze);

  /**
   * @brief Saves transient input state and releases the card view.
   */
  ~CasePresenter();
  CasePresenter(const CasePresenter&)            = delete;
  CasePresenter& operator=(const CasePresenter&) = delete;
  CasePresenter(CasePresenter&&)                 = delete;
  CasePresenter& operator=(CasePresenter&&)      = delete;

  /**
   * @brief Returns the card root for placement in the gallery.
   *
   * @return The card root
   */
  Dali::Ui::AbsoluteLayout GetView() const;

  /**
   * @brief Returns the primary chart of this card.
   *
   * @return The primary chart, or an empty handle for number-only cards
   */
  Dali::Ui::ChartView GetChart() const;

  /**
   * @brief Applies the next authored data or numeric-input update.
   */
  void Update();

  /**
   * @brief Moves the visible data page.
   *
   * @param[in] direction The signed page increment
   */
  void Navigate(int direction);

  /**
   * @brief Moves selection within existing data, ignoring empty datasets.
   *
   * @param[in] direction The signed point increment
   */
  void Select(int direction);

  /**
   * @brief Advances sample-local numeric transitions.
   *
   * @param[in] seconds The elapsed time in seconds
   */
  void Advance(float seconds);

  /**
   * @brief Toggles one authored series and refreshes its legend.
   *
   * @param[in] index The series index
   */
  void ToggleSeries(size_t index);

  /**
   * @brief Tests whether the primary and companion charts have completed their content updates.
   *
   * @return True when every chart is ready
   */
  bool IsReady() const;

  /**
   * @brief Serializes observable case state for capture verification.
   *
   * @return The case state as a JSON object
   */
  std::string           Snapshot() const;
  std::function<void()> recreate;
  std::function<void()> activated;

private:
  /**
   * @brief Creates and places an authored text slot.
   *
   * @param[in] parent The parent view
   * @param[in] text The displayed text
   * @param[in] size The font size
   * @param[in] x The left offset in pixels
   * @param[in] y The top offset in pixels
   * @param[in] width The slot width in pixels
   * @param[in] height The slot height in pixels
   * @param[in] color The RGB override, or zero to use the theme text color
   * @return The new label
   */
  Dali::Ui::Label AddLabel(Dali::Ui::View parent, const std::string& text, float size, float x, float y, float width, float height, uint32_t color = 0u);

  /**
   * @brief Creates an on-screen action for the card.
   *
   * @param[in] text The button caption
   * @param[in] x The left offset in pixels
   * @param[in] y The top offset in pixels
   * @param[in] width The button width in pixels
   * @param[in] callback The action retained for the button lifetime
   * @return The interactive button
   */
  Dali::Ui::InteractiveView AddButton(const std::string& text, float x, float y, float width, std::function<void()> callback);

  /**
   * @brief Builds this case using its authored layout and controls.
   */
  void Build();

  /**
   * @brief Creates the primary Cartesian chart in the supplied slot.
   *
   * @param[in] x The left offset in pixels
   * @param[in] y The top offset in pixels
   * @param[in] width The chart width in pixels
   * @param[in] height The chart height in pixels
   */
  void BuildChart(float x, float y, float width, float height);

  /**
   * @brief Creates a native pie, donut, gauge or scatter card.
   */
  void BuildAdditionalChart();

  /**
   * @brief Applies current state to a native additional chart.
   */
  void ApplyAdditionalData();

  /**
   * @brief Refreshes additional-chart totals, captions and legends.
   */
  void RefreshAdditionalLabels();

  /**
   * @brief Applies authored axis limits, labels, ticks and colors.
   */
  void ConfigureAxes();

  /**
   * @brief Selects the Day, Week or Month dataset.
   *
   * @param[in] period The zero-based period index
   */
  void SetPeriod(int period);

  /**
   * @brief Refreshes the selected state of period buttons.
   */
  void RefreshPeriodButtons();

  /**
   * @brief Applies current primary and companion data and per-point appearance.
   */
  void ApplyData();

  /**
   * @brief Restores a stored viewport or computes the current page bounds.
   */
  void ApplyViewport();

  /**
   * @brief Builds authored point, coordinate and threshold decorations.
   */
  void BuildAnnotations();

  /**
   * @brief Marks content ready and restores a persistent point selection.
   *
   * @param[in] chart The chart whose content was updated
   */
  void OnContentUpdated(Dali::Ui::ChartView chart);

  /**
   * @brief Stores a valid selection and refreshes its caption.
   *
   * @param[in] chart The chart emitting the selection
   * @param[in] point The selected point information
   */
  void OnSelected(Dali::Ui::ChartView chart, const Dali::Ui::ChartPointEventArgs& point);

  /**
   * @brief Converts a completed drag into viewport or page navigation.
   *
   * @param[in] actor The gesture target
   * @param[in] gesture The current pan gesture
   */
  void OnPan(Dali::Actor actor, Dali::PanGesture gesture);

  /**
   * @brief Updates the visible range description and requested accessibility announcement.
   */
  void Announce();

  /**
   * @brief Resolves a fixed RGB sample color with explicit alpha.
   *
   * @param[in] rgb The packed RGB value
   * @param[in] alpha The alpha component
   * @return The resolved RGBA value
   */
  Dali::Vector4                          Color(uint32_t rgb, float alpha = 1.0f) const;
  std::vector<bool>                      mCompanionReady;
  std::vector<Dali::Ui::ChartView>       mCompanions;
  std::vector<Dali::Ui::ChartSeries>     mCompanionSeries;
  std::vector<Dali::Ui::Label>           mLegendValues;
  std::vector<Dali::Ui::InteractiveView> mTabButtons;
  std::vector<Dali::Ui::ChartSeries>     mSeries;
  const ReferenceCase&                   mSpec;
  CaseState&                             mState;
  Dali::Ui::AbsoluteLayout               mRoot;
  Dali::Ui::ChartView                    mChart;
  Dali::Ui::ChartAxis                    mXAxis;
  Dali::Ui::ChartAxis                    mYAxis;
  Dali::Ui::Label                        mSelection;
  Dali::Ui::Label                        mError;
  Dali::Ui::Label                        mEmpty;
  Dali::Ui::Label                        mCenterValue;
  Dali::Ui::InputField                   mInput;
  Dali::UniquePtr<NumberTransition>      mNumber;
  Dali::PanGestureDetector               mPan;
  Dali::Vector2                          mPanStart;
  float                                  mWidth;
  float                                  mHeight;
  bool                                   mDark;
  bool                                   mFreeze;
  bool                                   mReady{false};
  bool                                   mRestoringSelection{false};
};
} //namespace ChartStandard

#endif // DALI_CHART_SAMPLE_CHART_CASE_PRESENTER_H
