/*
 * Copyright 2004,2005 The Apache Software Foundation.
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

#ifndef AXIS2_CORE_UTILS_H
#define AXIS2_CORE_UTILS_H

#include <axis2.h>
#include <axis2_defines.h>
#include <axis2_error.h>
#include <axis2_env.h>
#include <axis2_msg_ctx.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup axis2_core_utils Core Utils
 * @ingroup axis2_core_utils
 * @{
 */
    AXIS2_DECLARE(axis2_msg_ctx_t *)
    axis2_core_utils_create_out_msg_ctx(axis2_env_t **env,
                                axis2_msg_ctx_t *in_msg_ctx);

/** @} */
    
#ifdef __cplusplus
}
#endif

#endif /* AXIS2_CORE_UTILS_H */
