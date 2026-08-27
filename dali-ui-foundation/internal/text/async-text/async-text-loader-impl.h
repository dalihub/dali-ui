#ifndef DALI_UI_TEXT_ASYNC_TEXT_LOADER_IMPL_H
#define DALI_UI_TEXT_ASYNC_TEXT_LOADER_IMPL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/base-object.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>

#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-resolver.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/layouts/layout-engine.h>
#include <dali-ui-foundation/internal/text/layouts/layout-parameters.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-processing-source.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-render-state.h>
#include <dali-ui-foundation/internal/text/text-model.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{

/**
 * Implementation of the AsyncTextLoader
 */
class AsyncTextLoader : public BaseObject
{
public:
  /**
   * Constructor
   */
  AsyncTextLoader();

  /**
   * Destructor
   */
  ~AsyncTextLoader();

  /**
   * @copydoc Dali::AsyncTextLoader::SetLocale()
   */
  void SetLocale(const std::string& locale);

  /**
   * @copydoc Dali::AsyncTextLoader::SetLocaleUpdateNeeded()
   */
  void SetLocaleUpdateNeeded(bool update);

  /**
   * @copydoc Dali::AsyncTextLoader::IsLocaleUpdateNeeded()
   */
  bool IsLocaleUpdateNeeded();

  /**
   * @copydoc Dali::AsyncTextLoader::ClearModule()
   */
  void ClearModule();

  /**
   * @copydoc Dali::AsyncTextLoader::SetCustomFontDirectories()
   */
  void SetCustomFontDirectories(const TextAbstraction::FontPathList& customFontDirectories);

  /**
   * @copydoc Dali::AsyncTextLoader::RequestAddCustomFont()
   */
  void RequestAddCustomFont(const std::string& path);

  /**
   * @copydoc Dali::AsyncTextLoader::SetModuleClearNeeded()
   */
  void SetModuleClearNeeded(bool clear);

  /**
   * @copydoc Dali::AsyncTextLoader::IsModuleClearNeeded()
   */
  bool IsModuleClearNeeded();

  // Worker thread
  /**
   * @copydoc Dali::AsyncTextLoader::SetupRenderScale()
   */
  Size SetupRenderScale(AsyncTextParameters& parameters, bool& cachedNaturalSize);

  /**
   * @copydoc Dali::AsyncTextLoader::ComputeNaturalSize()
   */
  Size ComputeNaturalSize(AsyncTextParameters& parameters);

  /**
   * @brief Dali::AsyncTextLoader::ComputeHeightForWidth()
   */
  float ComputeHeightForWidth(AsyncTextParameters& parameters, float width, bool layoutOnly);

  /**
   * @copydoc Dali::AsyncTextLoader::RenderText()
   */
  AsyncTextRenderInfo RenderText(AsyncTextParameters& parameters, bool useCachedNaturalSize, const Size& naturalSize);

  /**
   * @copydoc Dali::AsyncTextLoader::RenderTextFit()
   */
  AsyncTextRenderInfo RenderTextFit(AsyncTextParameters& parameters, bool useCachedNaturalSize,
                                    const Size& naturalSize);

  /**
   * @copydoc Dali::AsyncTextLoader::RenderMarquee()
   */
  AsyncTextRenderInfo RenderMarquee(AsyncTextParameters& parameters, bool useCachedNaturalSize,
                                    const Size& naturalSize);

  /**
   * @copydoc Dali::AsyncTextLoader::GetNaturalSize()
   */
  AsyncTextRenderInfo GetNaturalSize(AsyncTextParameters& parameters);

  /**
   * @copydoc Dali::AsyncTextLoader::GetHeightForWidth()
   */
  AsyncTextRenderInfo GetHeightForWidth(AsyncTextParameters& parameters);

  /**
   * @brief Gets the request-local replacement layout state.
   *
   * @return The replacement layout state, or nullptr if no replacement is active.
   */
  const ReplacementRenderState* GetReplacementRenderState() const
  {
    return mReplacementData ? &mReplacementData->renderState : nullptr;
  }

private:
  struct ReplacementData
  {
    ReplacementRenderState renderState;
    Vector<Character>      originalLogicalText;
    uint64_t               finalElisionGeneration{0u};
  };

  // Worker thread
  /**
   * @brief Initializes internal fields.
   *
   * @param[in] parameters All options required to render text.
   */
  void Initialize();

  /**
   * @brief Clear completely data of the text model.
   */
  void ClearTextModelData();

  /**
   * @brief Update text model to render.
   *
   * @param[in] parameters All options required to render text.
   */
  void Update(AsyncTextParameters& parameters);

  /**
   * @brief Layout the updated text model to render.
   *
   * @param[in] parameters All options required to render text.
   * @param[out] updated true if the text has been laid-out. false means the given width is too small to layout even a
   * single character.
   *
   * @return The size of the text after it has been laid-out.
   */
  Size Layout(AsyncTextParameters& parameters, bool& updated);

  /**
   * @brief Updates replacement state after layout and alignment.
   *
   * @param[in] parameters All options required to render text.
   * @param[in] defaultFontId The font used when a replacement line has no visible text glyph.
   */
  void UpdateReplacementProcessing(AsyncTextParameters& parameters, FontId defaultFontId);

  /**
   * @brief Copies replacement placements to the render result.
   *
   * The worker coordinates are converted to logical coordinates using the
   * render scale.
   *
   * @param[in,out] renderInfo The render result to update.
   * @param[in] renderScale The scale used by the worker layout.
   */
  void CopyReplacementResult(AsyncTextRenderInfo& renderInfo, float renderScale) const;

  /**
   * @brief Gets the worker-local model used for rendering.
   *
   * @return The projected model when replacements are active, or the ordinary
   * model otherwise.
   */
  const Model* GetRenderTextModel() const;

  /**
   * @brief Copies summary information from the render model.
   *
   * @param[in,out] renderInfo The render result to update with the line count
   * and first-line direction.
   */
  void CopyRenderModelSummary(AsyncTextRenderInfo& renderInfo) const;

  /**
   * @brief Off screend render the updated text model to render.
   *
   * @param[in] parameters All options required to render text.
   */
  AsyncTextRenderInfo Render(AsyncTextParameters& parameters);

  /**
   * @brief Compute layout size of text.
   *
   * @param[in] parameters All options required to compute height of text.
   * @param[in] width The width of text to compute.
   * @param[in] height The height of text to compute.
   * @param[in] layoutOnly If there is no need to Initialize/Update, only the Layout is performed.
   *
   * @return The size of laid-out text.
   */
  Size ComputeLayoutSize(AsyncTextParameters& parameters, float width, float height, bool layoutOnly);

  /**
   * @brief Check if the text fits.
   *
   * @param[in] parameters Parameters of the text to check text fit.
   * @param[in] pointSize The point size of the text to check text fit.
   * @param[in] allowedSize The size of the layout to check text fit.
   *
   * @return True if the size of layout performed with parameters information fits, otherwise false.
   */
  bool CheckForTextFit(AsyncTextParameters& parameters, float pointSize, const Size& allowedSize);

private:
  /**
   * private method
   */

private:
  // Undefined copy constructor.
  AsyncTextLoader(const AsyncTextLoader&);

  // Undefined assignment constructor.
  AsyncTextLoader& operator=(const AsyncTextLoader&);

private:
  /**
   * private field
   */
  Text::AsyncTextModule mModule;

  Text::ModelPtr                      mTextModel;
  MetricsPtr                          mMetrics;
  Text::Layout::Engine                mLayoutEngine;
  Text::TypesetterPtr                 mTypesetter;
  std::unique_ptr<ReplacementData>    mReplacementData;
  std::unique_ptr<FinalElisionResult> mEndEllipsisResult;
  std::string                         mLocale;

  TextAbstraction::FontPathList mCustomFonts;

  Length mNumberOfCharacters;
  bool   mFitActualEllipsis : 1; // Used to store actual ellipses during TextFit calculations. Do not use it in other
                                 // sections.
  bool mIsTextDirectionRTL : 1;  // The direction of the first line after layout completion.
  bool mIsTextMirrored : 1;
  bool mModuleClearNeeded : 1;
  bool mLocaleUpdateNeeded : 1;

  Mutex mMutex;
}; // class AsyncTextLoader

inline bool compareByPointSize(Ui::Text::Fit::Candidate& lhs, Ui::Text::Fit::Candidate& rhs)
{
  return lhs.GetFontSize() < rhs.GetFontSize();
}

} // namespace Internal

inline static Internal::AsyncTextLoader& GetImplementation(AsyncTextLoader& asyncTextLoader)
{
  DALI_ASSERT_ALWAYS(asyncTextLoader && "async text loader handle is empty");
  BaseObject& handle = asyncTextLoader.GetBaseObject();
  return static_cast<Internal::AsyncTextLoader&>(handle);
}

inline static const Internal::AsyncTextLoader& GetImplementation(const AsyncTextLoader& asyncTextLoader)
{
  DALI_ASSERT_ALWAYS(asyncTextLoader && "async text loader handle is empty");
  const BaseObject& handle = asyncTextLoader.GetBaseObject();
  return static_cast<const Internal::AsyncTextLoader&>(handle);
}

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_ASYNC_TEXT_LOADER_IMPL_H
