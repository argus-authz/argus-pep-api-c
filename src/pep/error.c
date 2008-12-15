/*
 * Copyright 2008 Members of the EGEE Collaboration.
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
 *
 * $Id: error.c,v 1.3 2008/12/15 10:29:52 vtschopp Exp $
 */

#include "pep/error.h"

static const char * ERROR_STRINGS[]= {
		"OK",
		"PEP initialization failed: unable to allocate list.",
		"PEP initialization failed: unable to initialize CURL.",
		// FIXME: finish the array
};

const char * pep_strerror(pep_error_t errno) {
	return "pep_strerror(...) not yet implemented.";
	//return ERROR_STRINGS[errno];
}
