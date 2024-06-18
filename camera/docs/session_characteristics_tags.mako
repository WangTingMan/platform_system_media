## -*- coding: utf-8 -*-
/*
 * Copyright (C) 2024 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <map>
#include <vector>
#pragma once
/**
 * ! Do not edit this file directly !
 *
 * Generated automatically from session_characteristics_tags.mako. To be included in
 * libcameraservice only by aidl/AidlUtils.cpp.
 */

/**
 * Mapping of session characteristics to the INFO_SESSION_CONFIGURATION_QUERY_VERSION value
 * at which they were introduced.
 */
std::map<int, std::vector<camera_metadata_tag>> api_level_to_session_characteristic_keys {
%for api_level, keys in get_api_level_to_session_characteristic_keys(find_all_sections(metadata)).items():
        {${api_level},
         {
        %for idx, key in enumerate(keys):
                 ${key |csym},
        %endfor
         }},
    %endfor
};
