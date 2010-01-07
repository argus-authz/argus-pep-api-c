/*
 * Copyright 2008 Members of the EGEE Collaboration.
 * See http://www.eu-egee.org/partners for details on the copyright holders.
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

/*
 * $Id: error.h 1299 2009-10-15 15:29:34Z vtschopp $
 * $Name:  $
 * @author Valery Tschopp <valery.tschopp@switch.ch>
 * @version 1.0
 */
#ifndef _PEP_I_ERROR_H_
#define _PEP_I_ERROR_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * INTERNAL ERROR handling functions
 */

/** @internal
 * Sets an optional error message.
 */
void pep_errmsg(const char * format, ...);

/** @internal
 * Reset all the error messages previously set.
 */
void pep_clearerr(void);


#ifdef  __cplusplus
}
#endif

#endif // _PEP_ERROR_H_
