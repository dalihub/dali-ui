#ifndef DALI_UI_INTERNAL_TEXT_ANCHOR_INTERACTION_DATA_H
#define DALI_UI_INTERNAL_TEXT_ANCHOR_INTERACTION_DATA_H

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
#include <dali/public-api/math/vector2.h>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/text-anchor.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

struct AnchorHitResult
{
  bool        hit{false};
  bool        newlyClicked{false};
  std::string href{};
};

/**
 * @brief Stores view-owned anchor interaction state.
 *
 * Async anchor hit regions and clicked-anchor keys are kept here. The same storage
 * also owns the current accessibility TextAnchor actors, regardless of whether they
 * were produced from the sync controller path or async hit regions.
 */
class AnchorInteractionData
{
public:
  ~AnchorInteractionData();

  bool HasHitRegions() const;

  void Clear();

  void SetHitRegions(std::vector<Dali::Ui::Text::AsyncAnchorHitRegion>&& hitRegions);

  void ClearA11yAnchors();

  bool SetA11yAnchors(Dali::Ui::View owner, std::vector<Dali::Ui::TextAnchor>&& anchorActors);

  bool UpdateA11yAnchorsFromHitRegions(Dali::Ui::View owner, const Vector2& contentOffset);

  Dali::Ui::Text::AsyncAnchorHitRegion* FindHitRegion(const Vector2& point);

  Dali::Ui::Text::AsyncAnchorHitRegion* FindHitRegion(Dali::Ui::Text::CharacterIndex characterIndex);

  bool MarkClicked(Dali::Ui::Text::AsyncAnchorHitRegion& region);

  void PruneClickedAnchors();

  std::vector<Dali::Ui::Text::AsyncAnchorClickedState> GetClickedAnchors() const;

private:
  std::vector<Dali::Ui::Text::AsyncAnchorHitRegion>    mHitRegions;
  std::vector<Dali::Ui::Text::AsyncAnchorClickedState> mClickedAnchors;
  std::vector<Dali::Ui::TextAnchor>                    mTextAnchorActors;
};

AnchorInteractionData* GetAnchorInteractionData(Dali::Ui::View owner);

AnchorInteractionData& GetOrCreateAnchorInteractionData(Dali::Ui::View owner);

void ClearAnchorInteractionData(Dali::Ui::View owner);

void ClearA11yAnchors(Dali::Ui::View owner);

bool SetAnchorHitRegions(Dali::Ui::View                                      owner,
                         std::vector<Dali::Ui::Text::AsyncAnchorHitRegion>&& hitRegions);

bool SetA11yAnchors(Dali::Ui::View owner, std::vector<Dali::Ui::TextAnchor>&& anchorActors);

bool UpdateA11yAnchorsFromHitRegions(Dali::Ui::View owner, const Vector2& contentOffset);

std::vector<Dali::Ui::Text::AsyncAnchorClickedState> GetAnchorClickedStates(Dali::Ui::View owner);

AnchorHitResult HitTestAnchor(Dali::Ui::View owner, const Vector2& point);

AnchorHitResult ActivateAnchor(Dali::Ui::View owner, Dali::Ui::Text::CharacterIndex characterIndex);

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_TEXT_ANCHOR_INTERACTION_DATA_H
