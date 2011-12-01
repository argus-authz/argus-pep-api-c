/*
 * Copyright (c) Members of the EGEE Collaboration. 2006-2010.
 * See http://www.eu-egee.org/partners/ for details on the copyright holders.
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

#include "config.h"  /* PACKAGE_NAME and PACKAGE_VERSION const */

#include <stdio.h> /* snprintf() */
#include <curl/curl.h> /* curl_version() */

/** buffer for version */
#define VERSION_BUFFER_SIZE 1024
static char VERSION_BUFFER[VERSION_BUFFER_SIZE];
static int VERSION_BUFFER_initialized= 0;

const char * pep_version(void) {
    if (!VERSION_BUFFER_initialized) {
        snprintf(VERSION_BUFFER,VERSION_BUFFER_SIZE,"%s/%s (%s)",PACKAGE_NAME,PACKAGE_VERSION,curl_version());
        VERSION_BUFFER_initialized= 1;
    }
    return VERSION_BUFFER;
}
