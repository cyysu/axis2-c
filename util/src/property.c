/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <axis2_property.h>

struct axis2_property
{
    axis2_scope_t scope;
    AXIS2_FREE_VOID_ARG free_func;
    void *value;
};

axis2_property_t *AXIS2_CALL
axis2_property_create(const axis2_env_t *env)
{
    axis2_property_t *property = NULL;

    AXIS2_ENV_CHECK(env, NULL);

    property = (axis2_property_t *) AXIS2_MALLOC(env->allocator,
            sizeof(axis2_property_t));

    if (NULL == property)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    property->value = NULL;
    property->scope = AXIS2_SCOPE_REQUEST;
    property->free_func = 0;

    return property;
}
/*****************************************************************************/
axis2_property_t *AXIS2_CALL
axis2_property_create_with_args(
    const axis2_env_t *env,
    axis2_scope_t scope,
    AXIS2_FREE_VOID_ARG free_func,
    void *value) 
{
    axis2_property_t *property = NULL;

    AXIS2_ENV_CHECK(env, NULL);

    property = (axis2_property_t *) axis2_property_create(env);

    if (NULL == property)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    property->value = value;
    property->scope = scope;
    property->free_func = free_func;

    return property;
}

/***************************Function implementation****************************/

axis2_status_t AXIS2_CALL
axis2_property_free(axis2_property_t *property,
        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (property->value)
    {
        if (property->scope != AXIS2_SCOPE_APPLICATION)
        {
            if (property->free_func)
            {
                property->free_func(property->value, env);
            }
            else
            {
                AXIS2_FREE(env->allocator, property->value);
            }
            property->value = NULL;
        }
    }

    if (property)
    {
        AXIS2_FREE(env->allocator, property);
        property = NULL;
    }
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_property_set_scope(axis2_property_t *property,
        const axis2_env_t *env,
        axis2_scope_t scope)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    property->scope = scope;
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_property_set_free_func(axis2_property_t *property,
        const axis2_env_t *env,
        AXIS2_FREE_VOID_ARG free_func)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    property->free_func = free_func;
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_property_set_value(
    axis2_property_t *property,
    const axis2_env_t *env,
    void *value)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (property->value)
    {
        if (property->scope != AXIS2_SCOPE_APPLICATION)
        {
            if (property->free_func)
            {
                property->free_func(property->value, env);
            }
            else
            {
                AXIS2_FREE(env->allocator, property->value);
            }
            property->value = NULL;
        }
    }

    property->value = value;
    return AXIS2_SUCCESS;
}

void *AXIS2_CALL
axis2_property_get_value(axis2_property_t *property,
        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    return property->value;
}


axis2_property_t* AXIS2_CALL
axis2_property_clone(
    axis2_property_t *property,
    const axis2_env_t *env)
{
    axis2_property_t *new_property = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    new_property = axis2_property_create(env);
    axis2_property_set_free_func(new_property, env, property->free_func);
    axis2_property_set_scope(new_property, env, property->scope);
    axis2_property_set_value(new_property, env, property->value);
    return new_property; 
}


