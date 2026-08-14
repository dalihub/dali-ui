# Copyright (c) 2026 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if(NOT DEFINED FOUNDATION_ROOT_SRC_DIR)
  message(FATAL_ERROR "FOUNDATION_ROOT_SRC_DIR must name the dali-ui-foundation source directory")
endif()

set(forbidden_headers
  "${FOUNDATION_ROOT_SRC_DIR}/public-api/views/scroll/edge-effect-impl.h"
  "${FOUNDATION_ROOT_SRC_DIR}/public-api/views/scroll/bounce-edge-effect-impl.h"
  "${FOUNDATION_ROOT_SRC_DIR}/public-api/views/recycler/items-layouter-impl.h"
  "${FOUNDATION_ROOT_SRC_DIR}/public-api/views/recycler/linear-items-layouter-impl.h")

foreach(header IN LISTS forbidden_headers)
  if(EXISTS "${header}")
    message(FATAL_ERROR "Implementation header remains public: ${header}")
  endif()
endforeach()

function(assert_impl_namespace header namespace_name class_name)
  file(STRINGS "${header}" lines)
  set(awaiting_namespace_brace FALSE)
  set(in_expected_namespace FALSE)
  set(found_declaration FALSE)

  foreach(line IN LISTS lines)
    string(STRIP "${line}" trimmed_line)

    if(trimmed_line MATCHES "^namespace[ \t]+${namespace_name}$")
      set(awaiting_namespace_brace TRUE)
    elseif(awaiting_namespace_brace AND trimmed_line STREQUAL "{")
      set(awaiting_namespace_brace FALSE)
      set(in_expected_namespace TRUE)
    elseif(in_expected_namespace AND trimmed_line MATCHES "^}")
      set(in_expected_namespace FALSE)
    endif()

    if(trimmed_line MATCHES "^class[ \t]+DALI_UI_API[ \t]+${class_name}([ \t:]|$)")
      if(NOT in_expected_namespace)
        message(FATAL_ERROR "${class_name} must be declared in Dali::Ui::${namespace_name}: ${header}")
      endif()
      set(found_declaration TRUE)
    endif()
  endforeach()

  if(NOT found_declaration)
    message(FATAL_ERROR "${class_name} must be declared in Dali::Ui::${namespace_name}: ${header}")
  endif()
endfunction()

assert_impl_namespace("${FOUNDATION_ROOT_SRC_DIR}/integration-api/edge-effect-impl.h" "Integration" "EdgeEffectImpl")
assert_impl_namespace("${FOUNDATION_ROOT_SRC_DIR}/integration-api/items-layouter-impl.h" "Integration" "ItemsLayouterImpl")
assert_impl_namespace("${FOUNDATION_ROOT_SRC_DIR}/internal/bounce-edge-effect-impl.h" "Internal" "BounceEdgeEffectImpl")
assert_impl_namespace("${FOUNDATION_ROOT_SRC_DIR}/internal/linear-items-layouter-impl.h" "Internal" "LinearItemsLayouterImpl")
