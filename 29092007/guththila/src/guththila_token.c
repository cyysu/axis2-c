
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
#include <guththila_token.h>
    
#include <stdlib.h>
#include <stdio.h>
    
#define TOK_LIST_FREE(tok_list) \
    (if (tok_list)
     {
     AXIS2_FREE(tok_list)}

)
    
#define TOK_LIST_SIZE(tok_list) (tok_list->size)
        
        GUTHTHILA_CALL guththila_tok_list_create(const axutil_env_t * env) 
    {
        
        
            (guththila_tok_list_t *) AXIS2_MALLOC(env->allocator,
                                                  sizeof(guththila_tok_list_t));
        
            return NULL;
        
            (guththila_token_t **) AXIS2_MALLOC(env->allocator,
                                                sizeof(guththila_token_t *) *
                                                GUTHTHILA_TOK_DEF_LIST_SIZE);
        
        {
            
                (int *) AXIS2_MALLOC(env->allocator,
                                     sizeof(int) * GUTHTHILA_TOK_DEF_LIST_SIZE);
            
            {
                
                
                    (guththila_token_t *) AXIS2_MALLOC(env->allocator,
                                                       sizeof(guththila_token_t)
                                                       *
                                                       GUTHTHILA_TOK_DEF_SIZE);
                
                {
                    
                                          &tok_list->list[0][i], env);
                
                
                
                
                
            
        
        
        
    

guththila_tok_list_grow(
    guththila_tok_list_t * tok_list,
    const axutil_env_t * env) 
{
    
    
    
    
    
    {
        
        
        
            (guththila_token_t *) AXIS2_MALLOC(env->allocator,
                                               sizeof(guththila_token_t) *
                                               cur_cap);
        
        {
            
                                  env);
        
        
        
    
    else
    {
        
            (guththila_token_t **) AXIS2_MALLOC(env->allocator,
                                                sizeof(guththila_token_t *) *
                                                tok_list->no_list * 2);
        
        {
            
            {
                
            
            
            
            
            
        
    
    


guththila_tok_list_init(
    guththila_tok_list_t * tok_list,
    const axutil_env_t * env) 
{
    
    
        (guththila_token_t **) AXIS2_MALLOC(env->allocator,
                                            sizeof(guththila_token_t *) *
                                            GUTHTHILA_TOK_DEF_LIST_SIZE);
    
    {
        
            (int *) AXIS2_MALLOC(env->allocator,
                                 sizeof(int) * GUTHTHILA_TOK_DEF_LIST_SIZE);
        
        {
            
            
                (guththila_token_t *) AXIS2_MALLOC(env->allocator,
                                                   sizeof(guththila_token_t) *
                                                   GUTHTHILA_TOK_DEF_SIZE);
            
            {
                
                                      &tok_list->list[0][i], env);
            
            
            
            
            
        
    
    


guththila_tok_list_free(
    guththila_tok_list_t * tok_list,
    const axutil_env_t * env) 
{
    
    
    
    {
        
    
    
    


guththila_tok_list_free_data(
    guththila_tok_list_t * tok_list,
    const axutil_env_t * env) 
{
    
    
    
    {
        
    
    


    GUTHTHILA_CALL guththila_tok_list_get_token(guththila_tok_list_t * tok_list,
                                                const axutil_env_t * env) 
{
    
    {
        
    
    


guththila_tok_list_release_token(
    guththila_tok_list_t * tok_list,
    guththila_token_t * token,
    const axutil_env_t * env) 
{
    


guththila_tok_str_cmp(
    guththila_token_t * tok,
    char *str,
    size_t str_len,
    const axutil_env_t * env) 
{
    
    
        return -1;
    
    {
        
        {
            
        
    
    


guththila_tok_tok_cmp(
    guththila_token_t * tok1,
    guththila_token_t * tok2,
    const axutil_env_t * env) 
{
    
    
        return -1;
    
    {
        
        {
            
        
    
    


