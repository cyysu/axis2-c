
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
#include <guththila_attribute.h>
#include <guththila_stack.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
    
guththila_attr_list_grow(
    guththila_attr_list_t * at_list,
    int addition,
    const axutil_env_t * env) 
{
    
    
         (addition < 0 && at_list->capacity + addition > 0 &&
          at_list->capacity + addition >= at_list->size))
    {
        
            (guththila_attr_t *) realloc(at_list->list,
                                         sizeof(guththila_attr_t) *
                                         (at_list->capacity + addition));
        
        {
            
            {
                
                                      env);
            
            
        
        else
        {
            
        
    
    



    GUTHTHILA_CALL guththila_attr_list_create(const axutil_env_t * env) 
{
    
    
        (guththila_attr_list_t *) AXIS2_MALLOC(env->allocator,
                                               sizeof(guththila_attr_list_t));
    
        return NULL;
    
        (guththila_attr_t *) AXIS2_MALLOC(env->allocator,
                                          sizeof(guththila_attr_t) *
                                          GUTHTHILA_ATTR_DEF_SIZE);
    
        
    {
        
        
        
        {
            
        
        
    
    


guththila_attr_list_init(
    guththila_attr_list_t * at_list,
    const axutil_env_t * env) 
{
    
    
        (guththila_attr_t *) AXIS2_MALLOC(env->allocator,
                                          sizeof(guththila_attr_t) *
                                          GUTHTHILA_ATTR_DEF_SIZE);
    
        
    {
        
        
        
        {
            
        
        
    
    



    GUTHTHILA_CALL guththila_attr_list_get(guththila_attr_list_t * at_list,
                                           const axutil_env_t * env) 
{
    
         guththila_attr_list_grow(at_list, GUTHTHILA_ATTR_DEF_SIZE, env))
    {
        
    
    


guththila_attr_list_release(
    guththila_attr_list_t * at_list,
    guththila_attr_t * attr,
    const axutil_env_t * env) 
{
    


msuila_attr_list_free_data(
    guththila_attr_list_t * at_list,
    const axutil_env_t * env) 
{
    
    


guththila_attr_list_free(
    guththila_attr_list_t * at_list,
    const axutil_env_t * env) 
{
    
    
    
