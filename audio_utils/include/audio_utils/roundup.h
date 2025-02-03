/*
 * Copyright (C) 2012 The Android Open Source Project
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

#ifndef ANDROID_AUDIO_ROUNDUP_H
#define ANDROID_AUDIO_ROUNDUP_H

#include <cutils/cutils_export.h>
#ifndef _MSC_VER
#include <sys/cdefs.h>
#endif

#include <audio_utils/libaudioutils_export.h>

/** \cond */
__BEGIN_DECLS
/** \endcond */

/** Round up to the next highest power of 2 */
LIBAUDIOUTILS_EXPORT unsigned roundup(unsigned v);

/** \cond */
__END_DECLS
/** \endcond */

#endif  // ANDROID_AUDIO_ROUNDUP_H
