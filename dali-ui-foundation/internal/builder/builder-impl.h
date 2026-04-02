#ifndef DALI_UI_INTERNAL_BUILDER_H
#define DALI_UI_INTERNAL_BUILDER_H

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
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <list>
#include <map>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/builder/builder.h>
#include <dali-ui-foundation/devel-api/builder/json-parser.h>
#include <dali-ui-foundation/internal/builder/builder-declarations.h>
#include <dali-ui-foundation/internal/builder/style.h>

// Warning messages usually displayed
#define DALI_SCRIPT_WARNING(format, ...) DALI_LOG_WARNING_NOFN("Script:" format, ##__VA_ARGS__)

// Info messages are usually debug build
#define DALI_SCRIPT_INFO(format, ...) \
  DALI_LOG_INFO(Dali::Ui::Internal::gFilterScript, Debug::General, "Script:" format, ##__VA_ARGS__)

// Info Verbose need to be swiched on in gFilterScript filter constructor (by default set to General)
#define DALI_SCRIPT_VERBOSE(format, ...) \
  DALI_LOG_INFO(Dali::Ui::Internal::gFilterScript, Debug::Verbose, "Script:" format, ##__VA_ARGS__)

namespace Dali
{
namespace Ui
{
class TreeNode;
}

namespace Ui
{
namespace Internal
{
#if defined(DEBUG_ENABLED)
extern Dali::Integration::Log::Filter* gFilterScript;
#endif

class Builder;
class Replacement;

/**
 * @copydoc Ui::Builder
 */
class Builder : public Dali::BaseObject
{
public:
  Builder();

  /**
   * @copydoc Ui::Builder::LoadFromString
   */
  void LoadFromString(const std::string& data, Dali::Ui::Builder::UiFormat rep = Dali::Ui::Builder::JSON);

  /**
   * @copydoc Ui::Builder::AddConstants
   */
  void AddConstants(const Property::Map& map);

  /**
   * @copydoc Ui::Builder::AddConstant
   */
  void AddConstant(const std::string& key, const Property::Value& value);

  /**
   * @copydoc Ui::Builder::GetConfigurations
   */
  const Property::Map& GetConfigurations() const;

  /**
   * @copydoc Ui::Builder::GetConstants
   */
  const Property::Map& GetConstants() const;

  /**
   * @copydoc Ui::Builder::GetConstant
   */
  const Property::Value& GetConstant(const std::string& key) const;

  /**
   * @copydoc Ui::Builder::CreateAnimation( const std::string& animationName );
   */
  Animation CreateAnimation(const std::string& animationName);

  /**
   * @copydoc Ui::Builder::CreateAnimation( const std::string& animationName, const Property::Map& map );
   */
  Animation CreateAnimation(const std::string& animationName, const Property::Map& map);

  /**
   * @copydoc Ui::Builder::CreateAnimation( const std::string&,Dali::Actor);
   */
  Animation CreateAnimation(const std::string& animationName, Dali::Actor sourceActor);

  /**
   * @copydoc Ui::Builder::CreateAnimation( const std::string&,const Property::Map&, Dali::Actor);
   */
  Animation CreateAnimation(const std::string& animationName, const Property::Map& map, Dali::Actor sourceActor);

  /**
   * @copydoc Ui::Builder::Create( const std::string& templateName );
   */
  BaseHandle Create(const std::string& templateName);

  /**
   * @copydoc Ui::Builder::Create( const std::string& templateName, const Property::Map& map );
   */
  BaseHandle Create(const std::string& templateName, const Property::Map& map);

  /**
   * @copydoc Ui::Builder::CreateFromJson( const std::string& json );
   */
  BaseHandle CreateFromJson(const std::string& json);

  /**
   * @copydoc Ui::Builder::ApplyFromJson( Handle& handle, const std::string& json );
   */
  bool ApplyFromJson(Handle& handle, const std::string& json);

  /**
   * @copydoc Ui::Builder::ApplyStyle
   */
  bool ApplyStyle(const std::string& styleName, Handle& handle);

  /**
   * Lookup the stylename in builder. If it's found in the parse tree,
   * then return true.
   * @param[in] styleName The style name to search for
   * @return true if the stylename exists
   */
  bool LookupStyleName(const std::string& styleName);

  /**
   * Lookup the stylename in the recorded Styles - if it exists,
   * performs a shallow copy to the passed in style and returns true.
   * Otherwise it returns false.

   * @param[in] styleName The stylename to search for
   * @return A const pointer to the style object
   */
  const StylePtr GetStyle(const std::string& styleName);

  /**
   * @brief Get the properties of a style directly from the parsed tree.
   * @param[in] styleName The name of the style
   * @param[in] controlType The type of control
   * @param[out] result The map to store properties
   * @return True if style found and properties extracted
   */
  bool GetStyleProperties(const std::string& styleName, const Handle& controlType, Property::Map& result);

  /**
   * @copydoc Ui::Builder::AddActors
   */
  void AddActors(Actor toActor);

  /**
   * @copydoc Ui::Builder::AddActors
   */
  void AddActors(const std::string& sectionName, Actor toActor);

  /**
   * @copydoc Ui::Builder::CreateRenderTask
   */
  void CreateRenderTask(const std::string& name);

  /**
   * @copydoc Ui::Builder::GetPath
   */
  Path GetPath(const std::string& name);

  /**
   * @copydoc Ui::Builder::GetPathConstrainer
   */
  Dali::PathConstrainer GetPathConstrainer(const std::string& name);

  /*
   * Check if a given constrainer is of type PathConstrainer
   * @param[in] name The name of the constrainer
   * @return True if constainer is of type PathConstrainer, False otherwise
   *
   */
  bool IsPathConstrainer(const std::string& name);

  /**
   * @copydoc Ui::Builder::GetLinearConstrainer
   */
  Dali::LinearConstrainer GetLinearConstrainer(const std::string& name);

  /*
   * Check if a given constrainer is of type LinearConstrainer
   * @param[in] name The name of the constrainer
   * @return True if constainer is of type LinearConstrainer, False otherwise
   *
   */
  bool IsLinearConstrainer(const std::string& name);

  /**
   * @copydoc Ui::Builder::QuitSignal
   */
  Ui::Builder::BuilderSignalType& QuitSignal();

  /**
   * Emits the quit signal
   */
  void EmitQuitSignal();

protected:
  ~Builder() override;

private:
  typedef std::vector<const char*>           KeyStack;
  typedef std::vector<TreeNode::KeyNodePair> MappingsLut;
  typedef struct
  {
    std::string             name;
    Dali::LinearConstrainer linearConstrainer;
  } LinearConstrainerEntry;
  typedef std::vector<LinearConstrainerEntry> LinearConstrainerLut;
  typedef struct
  {
    std::string           name;
    Dali::PathConstrainer pathConstrainer;
  } PathConstrainerEntry;
  typedef std::vector<PathConstrainerEntry> PathConstrainerLut;
  typedef std::map<const std::string, Path> PathLut;

private:
  // Undefined
  Builder(const Builder&);

  // Undefined
  Builder& operator=(const Builder& rhs);

  void LoadConstants(const TreeNode& root, Property::Map& intoMap);

  void LoadConfiguration(const TreeNode& root, Property::Map& intoMap);

  Animation CreateAnimation(const std::string& animationName, const Replacement& replacement, Dali::Actor sourceActor);

  BaseHandle Create(const std::string& templateName, const Replacement& constant);

  BaseHandle DoCreate(const TreeNode& root, const TreeNode& node, Actor parent, const Replacement& replacements);

  void SetupTask(RenderTask& task, const Ui::TreeNode& node, const Replacement& replacement);

  bool ApplyStyle(const std::string& styleName, Handle& handle, const Replacement& replacement);

  void ApplyAllStyleProperties(const TreeNode& root, const TreeNode& node, Dali::Handle& handle,
                               const Replacement& constant);

  void RecordStyles(const char* styleName, const TreeNode& node, Dali::Handle& handle, const Replacement& replacements);

  void RecordStyle(StylePtr style, const TreeNode& node, Dali::Handle& handle, const Replacement& replacements);

  void ApplyProperties(const TreeNode& root, const TreeNode& node, Dali::Handle& handle, const Replacement& constant);

  void ApplySignals(const TreeNode& root, const TreeNode& node, Dali::Handle& handle);

  void ApplyStylesByActor(const TreeNode& root, const TreeNode& node, Dali::Handle& handle,
                          const Replacement& constant);

  void SetProperties(const TreeNode& node, Handle& handle, const Replacement& constant);

  bool MapToTargetProperty(Handle& propertyObject, const std::string& key, const TreeNode& node,
                           const Replacement& constant, Property::Index& index, Property::Value& value);

  /**
   * Find the key in the mapping table, if it's present, then generate
   * a property value for it (of the given type if available),
   * recursing as necessary, and stopping if any cycles are detected.
   *
   * @param[in] mappingRoot The JSON node containing the mappings
   * @param[in] theKey The key to search for
   * @param[in] propertyType The property type if known, or NONE
   * @param[in,out] value The string value to test and write back to.
   */
  bool GetPropertyMap(const TreeNode& mappingRoot, const char* theKey, Property::Type propertyType,
                      Property::Value& value);

  void SetCustomProperties(const TreeNode& node, Handle& handle, const Replacement& constant,
                           const std::string& childName, Property::AccessMode accessMode);

  /**
   * Find the key in the mapping table, if it's present, then generate
   * a property value for it (of the given type if available),
   * recursing as necessary, and stopping if any cycles are detected.
   *
   * @param[in] mappingRoot The JSON node containing the mappings
   * @param[in] theKey The key to search for
   * @param[in,out] keyStack the stack of visited keys
   * @param[in] propertyType The property type if known, or NONE
   * @param[in,out] value The string value to test and write back to.
   */
  bool RecursePropertyMap(const TreeNode& mappingRoot, KeyStack& keyStack, const char* theKey,
                          Property::Type propertyType, Property::Value& value);

  /**
   * Tests if the value is a string delimited by <>. If it is, then it attempts to
   * change the value to the mapping from a matching key in the mappings table.
   * @param[in] mappingRoot The JSON node containing the mappings
   * @param[in,out] keyStack the stack of visited keys
   * @param[in,out] value The string value to test and write back to.
   * @return true if the value was converted, false otherwise.
   */
  bool ConvertChildValue(const TreeNode& mappingRoot, KeyStack& keyStack, Property::Value& value);

private:
  Ui::JsonParser                 mParser;
  PathLut                        mPathLut;
  PathConstrainerLut             mPathConstrainerLut;
  LinearConstrainerLut           mLinearConstrainerLut;
  SlotDelegate<Builder>          mSlotDelegate;
  Property::Map                  mReplacementMap;
  Property::Map                  mConfigurationMap;
  MappingsLut                    mCompleteMappings;
  Dictionary<StylePtr>           mStyles; // State based styles
  Ui::Builder::BuilderSignalType mQuitSignal;
};

} // namespace Internal

inline Internal::Builder& GetImpl(Dali::Ui::Builder& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::Builder&>(handle);
}

inline const Internal::Builder& GetImpl(const Dali::Ui::Builder& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::Builder&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTERNAL_BUILDER_H
