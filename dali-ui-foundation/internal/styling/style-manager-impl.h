#ifndef DALI_UI_INTERNAL_STYLE_MANAGER_H
#define DALI_UI_INTERNAL_STYLE_MANAGER_H

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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/style-monitor.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/builder/builder.h>
#include <dali-ui-foundation/devel-api/styling/style-manager-devel.h>
#include <dali-ui-foundation/internal/builder/style.h>
#include <dali-ui-foundation/public-api/styling/style-manager.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class FeedbackStyle;

/**
 * @copydoc Ui::StyleManager
 */
class StyleManager : public Dali::BaseObject, public ConnectionTracker
{
public:
  /**
   * Singleton access
   *
   * @return The StyleManager object
   */
  static Ui::StyleManager Get();

  /**
   * Construct a new StyleManager.
   */
  StyleManager();

protected:
  /**
   * @brief Destructor
   */
  virtual ~StyleManager();

public: // Public API
  /**
   * @copydoc Ui::StyleManager::ApplyTheme
   */
  void ApplyTheme(const std::string& themeFile);

  /**
   * @copydoc Ui::StyleManager::ApplyDefaultTheme
   */
  void ApplyDefaultTheme();

  /**
   * @copydoc Ui::StyleManager::GetDefaultFontFamily
   */
  const std::string& GetDefaultFontFamily() const;

  /**
   * @copydoc Ui::StyleManager::SetStyleConstant
   */
  void SetStyleConstant(const std::string& key, const Property::Value& value);

  /**
   * @copydoc Ui::StyleManager::GetStyleConstant
   */
  bool GetStyleConstant(const std::string& key, Property::Value& valueOut);

  /**
   * @copydoc Ui::StyleManager::GetConfigurations
   */
  const Property::Map& GetConfigurations();

  /**
   * @copydoc Ui::DevelStyleManager::SetBrokenImageUrl
   */
  void SetBrokenImageUrl(DevelStyleManager::BrokenImageType brokenImageType, const std::string& brokenImageUrl);

  /**
   * @copydoc Ui::DevelStyleManager::GetBrokenImageUrl
   */
  std::string GetBrokenImageUrl(DevelStyleManager::BrokenImageType brokenImageType);

  /**
   * @copydoc Ui::DevelStyleManager::GetBrokenImageUrlList
   */
  std::vector<std::string> GetBrokenImageUrlList();

  /**
   * @brief Apply the theme style to a control.
   *
   * @param[in] control The control to apply style.
   */
  void ApplyThemeStyle(Ui::Control control);

  /**
   * @brief Apply the theme style to a control at initialization.
   *
   * @param[in] control The control to apply style.
   */
  void ApplyThemeStyleAtInit(Ui::Control control);

  /**
   * @copydoc Ui::StyleManager::ApplyStyle
   */
  void ApplyStyle(Ui::Control control, const std::string& jsonFileName, const std::string& styleName);

  /**
   * @brief Get the properties for a specific style with Property::Index keys.
   * @param[in] styleName The name of the style (e.g., "TextLabel").
   * @param[in] controlType A handle to a control instance to get Property::Index mapping.
   * @return A map containing the style's properties with Property::Index as keys.
   */
  Property::Map GetStyleProperties(const std::string& styleName, const Handle& controlType);

  /**
   * Get the state/style information for the given control
   * @param[in] control The control to get state information for
   * @return The style information (or empty ptr if not found)
   */
  const StylePtr GetRecordedStyle(Ui::Control control);

public:
  // SIGNALS

  /**
   * @copydoc Ui::StyleManager::StyleChangeSignal
   * This signal is sent after all the controls have been updated
   * due to style change
   */
  Ui::StyleManager::StyleChangedSignalType& StyleChangedSignal();

  /**
   * This signal is sent to the controls following a style change.
   * It should not be exposed in the public API
   */
  Ui::StyleManager::StyleChangedSignalType& ControlStyleChangeSignal();

  /**
   * This signal is sent to the visual factory following a broken image change.
   * It should not be exposed in the public API
   */
  Ui::DevelStyleManager::BrokenImageChangedSignalType& BrokenImageChangedSignal();

private:
  typedef std::vector<std::string> StringList;

  /**
   * This will be called when the adaptor is initialized
   */
  void OnAdaptorInit();

  /**
   * @brief Set the current theme. Called only once per event processing cycle.
   * @param[in] themeFile The name of the theme file to read.
   */
  void SetTheme(const std::string& themeFile);

  /**
   * @brief Internal helper method to read a file from file system.
   * @param filename The name of the file to read.
   * @param[out] stringOut The string to return the file in
   *
   * @param Return true if file was found
   */
  bool LoadFile(const std::string& filename, std::string& stringOut);

  /**
   * @brief Create a new builder.
   *
   * @param[in] constants A map of constants to be used by the builder
   *
   * @return Return the newly created builder
   */
  Ui::Builder CreateBuilder(const Property::Map& constants);

  /**
   * @brief Load a JSON file into given builder
   *
   * @param[in] builder The builder object to load the theme file
   * @param[in] jsonFileName The name of the JSON file to load
   * @return Return true if file was loaded
   */
  bool LoadJSON(Ui::Builder builder, const std::string& jsonFileName);

  /**
   * @brief Apply a style to the control using the given builder
   *
   * @param[in] builder The builder to apply the style from
   * @param[in] control The control to apply the style to
   */
  void ApplyStyle(Ui::Builder builder, Ui::Control control);

  /**
   * Search for a builder in the cache
   *
   * @param[in] key The key the builder was cached under
   * @return Return the cached builder if found or an empty builder object if not found
   */
  Ui::Builder FindCachedBuilder(const std::string& key);

  /**
   * Store a given builder in the cache keyed to the given key
   *
   * @param[in] builder The builder object to store
   * @param[in] key The key to store the builder under
   */
  void CacheBuilder(Ui::Builder builder, const std::string& key);

  /**
   * Callback for when style monitor raises a signal
   *
   * @param[in] styleMonitor The style monitor object
   * @param[in] styleChange The style change type
   */
  void StyleMonitorChange(StyleMonitor styleMonitor, StyleChange::Type styleChange);

  /**
   * Emit signals to controls first, app second
   */
  void EmitStyleChangeSignals(StyleChange::Type styleChange);

  // Undefined
  StyleManager(const StyleManager&);

  StyleManager& operator=(const StyleManager& rhs);

private:
  // Map to store builders keyed by JSON file name
  typedef std::map<std::string, Ui::Builder> BuilderMap;

  Ui::Builder mThemeBuilder;  ///< Builder for all default theme properties
  StyleMonitor mStyleMonitor; ///< Style monitor handle

  int mDefaultFontSize; ///< Logical size, not a point-size
  std::string mDefaultFontFamily;
  std::string mDefaultThemeFilePath; ///< The full path of the default theme file
  std::string mThemeFile;            ///< The full path of the current theme file

  Property::Map mThemeBuilderConstants; ///< Contants to give the theme builder
  Property::Map mStyleBuilderConstants; ///< Constants specific to building styles

  BuilderMap mBuilderCache; ///< Cache of builders keyed by JSON file name

  Ui::Internal::FeedbackStyle* mFeedbackStyle; ///< Feedback style

  std::vector<std::string> mBrokenImageUrls; ///< Broken Image Urls received from user

  std::vector<Dali::WeakHandle<Ui::Control>>
      mInitializedControlsBeforeAdaptorInit{}; ///< Controls to initialized before the adaptor is initialized
  std::vector<Dali::WeakHandle<Ui::Control>>
      mThemeAppliedControlsBeforeAdaptorInit{}; ///< Controls to theme applied before the adaptor is initialized

  bool mAdaptorInitialized : 1; ///< Whether the adaptor has been initialized

  // Signals
  Ui::StyleManager::StyleChangedSignalType
      mControlStyleChangeSignal; ///< Emitted when the style( theme/font ) changes for the controls to style themselves
  Ui::StyleManager::StyleChangedSignalType mStyleChangedSignal; ///< Emitted after the controls have been styled
  Ui::DevelStyleManager::BrokenImageChangedSignalType
      mBrokenImageChangedSignal; ///< Emitted after brokenImageChangedSignal
};

} // namespace Internal

inline Internal::StyleManager& GetImpl(Dali::Ui::StyleManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::StyleManager&>(handle);
}

inline const Internal::StyleManager& GetImpl(const Dali::Ui::StyleManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::StyleManager&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTERNAL_STYLE_MANAGER_H
