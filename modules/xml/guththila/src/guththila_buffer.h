/*
 *   Copyright 2004,2005 The Apache Software Foundation.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *	
 * @author Dinesh Premalal (xydinesh@gmail.com, premalwd@cse.mrt.ac.lk)	
 */


#ifndef GUTHTHILA_BUFFER_H
#define GUTHTHILA_BUFFER_H

#include <stdlib.h>
#include "guththila_environment.h"
#include "guththila_defines.h"

typedef struct guththila_buffer_s
{
    int size;
    int next;
    int last;
    int offset;
    guththila_char_t *buff;
} guththila_buffer_t;

GUTHTHILA_DECLARE (guththila_buffer_t *)
guththila_buffer_create (guththila_environment_t * environment, int size);
GUTHTHILA_DECLARE (void)
guththila_buffer_free (guththila_environment_t * environment,
                       guththila_buffer_t * name);

GUTHTHILA_DECLARE (guththila_buffer_t *)
guththila_buffer_grow (guththila_environment_t * environment,
                       guththila_buffer_t * name);

#endif /* GUTHTHILA_BUFFER_H */
