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
#include <dali/public-api/common/unique-ptr.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
/**
 * @brief Describes the offscreen buffer a GlView renders into.
 *
 * Only read by GlView::New(), and only for GlView::BackendMode::OFFSCREEN_RENDERING -
 * the direct backends draw into the window surface, whose format the window
 * already fixed. Everything here has to be decided before the buffer exists, so
 * there is no way to change it afterwards.
 *
 * @code
 * GlViewOffscreenConfig config;
 * config.SetDepthEnabled(true);
 * config.SetSampleCount(4);
 *
 * GlView glView = GlView::New(GlView::BackendMode::OFFSCREEN_RENDERING, config);
 * @endcode
 *
 * @note Whether a request can be met depends on the platform. Where it cannot be,
 *       the registered callbacks are never invoked and nothing is drawn.
 */
class DALI_UI_API GlViewOffscreenConfig
{
public: // Types
  /**
   * @brief The pixel format of the offscreen buffer.
   */
  enum class ColorFormat
  {
    RGB888,  ///< 8 red bits, 8 green bits, 8 blue bits
    RGBA8888 ///< 8 red bits, 8 green bits, 8 blue bits, 8 alpha bits
  };

  /**
   * @brief The GLES version the rendering context is created for.
   */
  enum class GraphicsApiVersion
  {
    GLES_VERSION_2_0,
    GLES_VERSION_3_0
  };

public: // Creation & Destruction
  /**
   * @brief Creates a configuration with the defaults described on each setter.
   */
  GlViewOffscreenConfig();

  /**
   * @brief Destructor.
   */
  ~GlViewOffscreenConfig();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The configuration to copy
   */
  GlViewOffscreenConfig(const GlViewOffscreenConfig& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs The configuration to move
   */
  GlViewOffscreenConfig(GlViewOffscreenConfig&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The configuration to copy
   * @return A reference to this
   */
  GlViewOffscreenConfig& operator=(const GlViewOffscreenConfig& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs The configuration to move
   * @return A reference to this
   */
  GlViewOffscreenConfig& operator=(GlViewOffscreenConfig&& rhs) noexcept;

public: // Properties
  /**
   * @brief Sets the pixel format of the offscreen buffer.
   *
   * @param[in] format The pixel format to use
   *
   * @note The default is ColorFormat::RGBA8888.
   */
  void SetColorFormat(ColorFormat format);

  /**
   * @brief Returns the pixel format of the offscreen buffer.
   *
   * @return The pixel format
   */
  ColorFormat GetColorFormat() const;

  /**
   * @brief Sets whether a depth buffer is attached.
   *
   * @param[in] enabled True to attach a 24 bit depth buffer
   *
   * @note The default is false.
   */
  void SetDepthEnabled(bool enabled);

  /**
   * @brief Returns whether a depth buffer is attached.
   *
   * @return True if a depth buffer is attached
   */
  bool IsDepthEnabled() const;

  /**
   * @brief Sets whether a stencil buffer is attached.
   *
   * @param[in] enabled True to attach an 8 bit stencil buffer
   *
   * @note The default is false.
   */
  void SetStencilEnabled(bool enabled);

  /**
   * @brief Returns whether a stencil buffer is attached.
   *
   * @return True if a stencil buffer is attached
   */
  bool IsStencilEnabled() const;

  /**
   * @brief Sets how many samples per pixel to ask for.
   *
   * @param[in] samples The sample count, or 0 or 1 to disable multisampling
   *
   * @note The default is 0. A single sample is not anti-aliasing, so 1 disables
   *       multisampling just as 0 does.
   */
  void SetSampleCount(uint32_t samples);

  /**
   * @brief Returns how many samples per pixel were asked for.
   *
   * @return The sample count
   */
  uint32_t GetSampleCount() const;

  /**
   * @brief Sets the GLES version the rendering context is created for.
   *
   * @param[in] version The GLES version to use
   *
   * @note The default is GraphicsApiVersion::GLES_VERSION_3_0.
   */
  void SetGraphicsApiVersion(GraphicsApiVersion version);

  /**
   * @brief Returns the GLES version the rendering context is created for.
   *
   * @return The GLES version
   */
  GraphicsApiVersion GetGraphicsApiVersion() const;

private:
  struct Impl;
  UniquePtr<Impl> mImpl;
};

} // namespace Ui
} //namespace DALI_NAMESPACE
