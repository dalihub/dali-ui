#ifndef DALI_UI_INTERNAL_LOCALIZATION_GETTEXT_WRAPPER_H
#define DALI_UI_INTERNAL_LOCALIZATION_GETTEXT_WRAPPER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(_WIN32) || defined(DALI_UI_HAS_GETTEXT)
#define DALI_UI_GETTEXT_AVAILABLE
#include <libintl.h>
#endif

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Localization
{

inline const char* BindTextDomain(const char* domain, const char* localePath)
{
#if defined(DALI_UI_GETTEXT_AVAILABLE)
  return bindtextdomain(domain, localePath);
#else
  (void)domain;
  return localePath;
#endif
}

inline const char* BindTextDomainCodeset(const char* domain, const char* codeset)
{
#if defined(DALI_UI_GETTEXT_AVAILABLE)
  return bind_textdomain_codeset(domain, codeset);
#else
  (void)domain;
  return codeset;
#endif
}

inline const char* GetText(const char* domain, const char* resourceId)
{
#if defined(DALI_UI_GETTEXT_AVAILABLE)
  return dgettext(domain, resourceId);
#else
  (void)domain;
  return resourceId;
#endif
}

inline const char* GetPluralText(const char*   domain,
                                 const char*   resourceId,
                                 const char*   pluralResourceId,
                                 unsigned long quantity)
{
#if defined(DALI_UI_GETTEXT_AVAILABLE)
  return dngettext(domain, resourceId, pluralResourceId, quantity);
#else
  (void)domain;
  return quantity == 1ul ? resourceId : pluralResourceId;
#endif
}

} // namespace Localization
} // namespace Internal
} // namespace Ui
} // namespace Dali

#undef DALI_UI_GETTEXT_AVAILABLE

#endif // DALI_UI_INTERNAL_LOCALIZATION_GETTEXT_WRAPPER_H
