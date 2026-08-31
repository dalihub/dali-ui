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

// CLASS HEADER
#include <dali-ui-components/internal/component-image-path.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/environment-variable.h>

#include <string>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
void AppendPathSeparator(std::string& path)
{
  if(!path.empty() && path.back() != '/')
  {
    path.push_back('/');
  }
}
} // unnamed namespace

Dali::String ResolveComponentImageUrl(Dali::StringView fileName)
{
  DALI_ASSERT_ALWAYS(fileName.Data() && fileName.Size() > 0u && "Component image filename must not be empty");

  const char* configuredDirectory = DALI_UI_COMPONENTS_IMAGE_DIR;
  std::string directory;
  if(configuredDirectory)
  {
    directory = configuredDirectory;
  }
  else
  {
    const char* daliImageDirectory = Dali::EnvironmentVariable::GetEnvironmentVariable("DALI_IMAGE_DIR");
    if(daliImageDirectory)
    {
      directory = daliImageDirectory;
    }
    AppendPathSeparator(directory);
    directory.append("components");
  }

  AppendPathSeparator(directory);
  directory.append(fileName.Data(), fileName.Size());
  return Dali::String(directory.c_str());
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
