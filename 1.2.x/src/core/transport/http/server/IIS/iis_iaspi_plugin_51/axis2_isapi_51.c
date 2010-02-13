
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
    
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <httpfilt.h>
    
#include "..\\axis2_isapi_plugin.h"
#include "..\\axis2_iis_constants.h"
    

/*
This function is called by the IIS server at the server 
initialization. So this is the ideal plcae for initializing 
axis2c.
*/ 
    BOOL WINAPI GetFilterVersion(PHTTP_FILTER_VERSION pVer) 
{
    
    
    
    
    {
        
    
    
        /*
           Receive notifictions when
           1. Server preprocessed the headers.
           2. Log 
           3. All the request coming in secure and none secure ports.
         */ 
        pVer->dwFlags =
        (SF_NOTIFY_ORDER_HIGH | 
         
         
    
        // Give a short discription about the module.
        strcpy(pVer->lpszFilterDesc, "axis2c filter");
    
        // initialize axis2
        rc = init_axis2();
    




/*
When a notification happens this function is called by the IIS.
*/ 
    DWORD WINAPI HttpFilterProc(
                                
                                
{
    
    
    
    
        
    {
        
        
            
        {
            
                                                                        "url",
                                                                        modified_url);
            
        
    
    




/*
This is the exit point of the filter. Here we shut down 
the axis2 engine.
*/ 
    BOOL WINAPI TerminateFilter(DWORD dwfalgs) 
{
    


