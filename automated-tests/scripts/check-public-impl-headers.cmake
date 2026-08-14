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
