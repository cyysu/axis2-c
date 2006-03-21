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

#include <stdlib.h>
#include <string.h>
#include <axis2_env.h>
#include <axis2_error_default.h>
#include <axis2_string.h>

AXIS2_DECLARE(axis2_status_t)  axis2_env_free (axis2_env_t *env)
{
    if(NULL != env && NULL != env->log)
        AXIS2_LOG_FREE(env->allocator, env->log);
	
	if(NULL != env && NULL != env->error)
        AXIS2_ERROR_FREE(env->error);

    if(NULL != env && NULL != env->thread_pool)
        AXIS2_THREAD_POOL_FREE(env->thread_pool);

    if(NULL != env)
        free (env);
	
	return 0;  
}

AXIS2_DECLARE(axis2_env_t*)
axis2_env_create (axis2_allocator_t *allocator)
{
	axis2_env_t *environment;
    if (NULL == allocator)
        return NULL;

    environment =
        (axis2_env_t *) AXIS2_MALLOC (allocator, sizeof (axis2_env_t));

    if (NULL == environment)
        return NULL;

    environment->allocator = allocator;
    
    /* Create default error */
    environment->error = axis2_error_create (allocator);
    if(NULL == environment->error)
        return NULL;
    environment->log = NULL;
	environment->thread_pool = NULL;
	return environment;
    
}

AXIS2_DECLARE(axis2_env_t*)
axis2_env_create_with_error (axis2_allocator_t *allocator
                          , axis2_error_t *error)
{
	return axis2_env_create_with_error_log(allocator, error
		, NULL);    
}

AXIS2_DECLARE(axis2_env_t *)
axis2_env_create_with_error_log (axis2_allocator_t *allocator
                          , axis2_error_t *error
                          , axis2_log_t *log)
{
    axis2_env_t *environment;
    if (NULL == allocator)
        return NULL;
	if (NULL == error)
        return NULL;

    environment =
        (axis2_env_t *) AXIS2_MALLOC (allocator,
                                              sizeof (axis2_env_t));

    if (NULL == environment)
        return NULL;

    environment->allocator = allocator;
    environment->error = error;


    if (NULL == log)
        environment->log_enabled = AXIS2_FALSE;
	environment->log_enabled = AXIS2_TRUE;
    environment->log = log;
	environment->thread_pool = NULL;
    return environment;
}

AXIS2_DECLARE(axis2_env_t *) 
axis2_env_create_with_error_log_thread_pool (axis2_allocator_t *allocator
							, axis2_error_t *error
                            , axis2_log_t *log
							, axis2_thread_pool_t *pool)
{
	axis2_env_t *environment;
    if (NULL == allocator)
        return NULL;
	if (NULL == error)
        return NULL;
	if(NULL == pool)
		return NULL;

    environment =
        (axis2_env_t *) AXIS2_MALLOC (allocator, sizeof (axis2_env_t));

    if (NULL == environment)
        return NULL;

    environment->allocator = allocator;
    environment->error = error;
	environment->thread_pool = pool;

    if (NULL == log)
        environment->log_enabled = AXIS2_FALSE;
	environment->log_enabled = AXIS2_TRUE;
    environment->log = log;

    return environment;
}

AXIS2_DECLARE( axis2_status_t )
axis2_env_check_status (axis2_env_t **env)
{
    AXIS2_ENV_CHECK(env, AXIS2_CRTICAL_FAILURE);

	return AXIS2_ERROR_GET_STATUS_CODE((*env)->error);
}

AXIS2_DECLARE(axis2_status_t) axis2_env_enable_log (axis2_env_t **env, axis2_bool_t enable)
{
    AXIS2_ENV_CHECK(env, AXIS2_CRTICAL_FAILURE);

    (*env)->log_enabled = enable;
    
    return AXIS2_SUCCESS;
}

AXIS2_DECLARE(axis2_status_t)  axis2_env_write_log (axis2_env_t **env, const char* message, axis2_log_levels_t level)
{
    AXIS2_ENV_CHECK(env, AXIS2_CRTICAL_FAILURE);

    if (! ((*env)->log_enabled))
        return AXIS2_SUCCESS;
 
    if (message && (*env)->log)
    {
        AXIS2_LOG_WRITE((*env)->log, message, level);
    }
    return AXIS2_SUCCESS;
}
