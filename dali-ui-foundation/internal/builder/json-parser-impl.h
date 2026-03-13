#ifndef DALI_JSON_PARSER_IMPL_H
#define DALI_JSON_PARSER_IMPL_H

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <list>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/builder/json-parser.h>
#include <dali-ui-foundation/devel-api/builder/tree-node.h>

#include <dali-ui-foundation/internal/builder/builder-get-is.inl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
/*
 * Parses JSON
 */
class JsonParser : public BaseObject
{
public:
  /*
   * @copydoc Ui::JsonParser::JsonParser()
   */
  JsonParser();

  /*
   * @copydoc Ui::JsonParser::JsonParser(const TreeNode& tree)
   */
  explicit JsonParser(const TreeNode& tree);

  /*
   */
  ~JsonParser() override;

  /*
   * @copydoc Ui::JsonParser::Parse()
   */
  bool Parse(const std::string& source);

  /*
   * @copydoc Ui::JsonParser::Pack()
   */
  void Pack(void);

  /*
   * @copydoc Ui::JsonParser::GetRoot()
   */
  const TreeNode* GetRoot() const;

  /*
   * @copydoc Ui::JsonParser::ParseError()
   */
  bool ParseError() const;

  /*
   * @copydoc Ui::JsonParser::GetErrorPosition()
   */
  int GetErrorPosition() const;

  /*
   * @copydoc Ui::JsonParser::GetErrorDescription()
   */
  std::string GetErrorDescription() const;

  /*
   * @copydoc Ui::JsonParser::GetErrorLineNumber()
   */
  int GetErrorLineNumber() const;

  /*
   * @copydoc Ui::JsonParser::GetErrorColumn()
   */
  int GetErrorColumn() const;

  /*
   * @copydoc Ui::JsonParser::Write()
   */
  void Write(std::ostream& output, int indent) const;

private:
  /**
   * @brief Ensure to destroy and release all memory under tree root
   */
  void DeleteRootNode() noexcept;

private:
  typedef std::vector<char>    VectorChar;
  typedef VectorChar::iterator VectorCharIter;

  typedef std::list<VectorChar>           SourceContainer;
  typedef std::list<VectorChar>::iterator SourceContainerIter;

  JsonParser(JsonParser&);
  JsonParser& operator=(const JsonParser&);

  SourceContainer mSources; ///< List of strings from Parse() merge operations

  TreeNode* mRoot; ///< Tree root

  const char* mErrorDescription; ///< Last parse error description
  int         mErrorPosition;    ///< Last parse error position
  int         mErrorLine;        ///< Last parse error line
  int         mErrorColumn;      ///< Last parse error column

  int mNumberOfChars; ///< The size of string data for all nodes
  int mNumberOfNodes; ///< Node count
};

} // namespace Internal

inline const Internal::JsonParser& GetImplementation(const Ui::JsonParser& parser)
{
  DALI_ASSERT_ALWAYS(parser && "JsonParser handle is empty");

  const BaseObject& handle = parser.GetBaseObject();

  return static_cast<const Internal::JsonParser&>(handle);
}

inline Internal::JsonParser& GetImplementation(Ui::JsonParser& parser)
{
  DALI_ASSERT_ALWAYS(parser && "JsonParser handle is empty");

  BaseObject& handle = parser.GetBaseObject();

  return static_cast<Internal::JsonParser&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_JSON_PARSER_IMPL_H
