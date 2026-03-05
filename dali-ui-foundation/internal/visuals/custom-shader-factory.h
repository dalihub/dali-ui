#ifndef DALI_UI_CUSTOM_SHADER_FACTORY_H
#define DALI_UI_CUSTOM_SHADER_FACTORY_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/adaptor-framework/shader-precompiler.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-shader-factory-interface.h>
#include <string_view>

namespace Dali
{
namespace UI
{
namespace Internal
{
/**
 * CustomShaderFactory is an object that provides custom shader
 */
class CustomShaderFactory : public VisualShaderFactoryInterface
{
public:
  /**
   * @brief Constructor
   */
  CustomShaderFactory();

  /**
   * @brief Destructor
   */
  ~CustomShaderFactory() override;

public: // Implementation of VisualShaderFactoryInterface
  /**
   * @copydoc Dali::UI::VisualShaderFactoryInterface::AddPrecompiledShader
   */
  bool AddPrecompiledShader(PrecompileShaderOption& option) override;

  /**
   * @copydoc Dali::UI::VisualShaderFactoryInterface::GetPreCompiledShader
   */
  void GetPreCompiledShader(ShaderPreCompiler::RawShaderData& shaders) override;

private:
  /**
   * @brief Save the custom shader
   */
  bool SavePrecompileShader(std::string&& shaderName, std::string&& vertexPrefix, std::string&& fragmentPrefix);

protected:
  /**
   * Undefined copy constructor.
   */
  CustomShaderFactory(const CustomShaderFactory&) = delete;

  /**
   * Undefined assignment operator.
   */
  CustomShaderFactory& operator=(const CustomShaderFactory& rhs) = delete;
};

} // namespace Internal

} // namespace UI

} // namespace Dali

#endif // DALI_UI_CUSTOM_SHADER_FACTORY_H
