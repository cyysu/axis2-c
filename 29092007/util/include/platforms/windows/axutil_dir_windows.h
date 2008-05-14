
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

#ifndef READDIR_H
#define READDIR_H
#include <search.h>
#include <string.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <windows.h>
#include <axutil_utils_defines.h>


/* struct dirent - same as Unix dirent.h */
struct dirent
{
    
    
    
    
    
    /*unsigned char      d_type; *//*type of file */
};


/* def struct DIR - different from Unix DIR */
typedef struct
{
    
    
    
    
    
    
    
}



/* Function prototypes */


/**
  * open a directory on a given name
  * returns a DIR if successful, or NULL if the path cannot be opened
  */
AXIS2_EXTERN DIR * AXIS2_CALL



/**
  * Close the directory stream DIRP.
  * Return 0 if successful, -1 otherwise.
  */
AXIS2_EXTERN int AXIS2_CALL

    


/**
  * Read a directory entry from DIRP.  Return a pointer to a `struct
  * dirent' describing the entry, or NULL for EOF or error.
  */
AXIS2_EXTERN struct dirent *AXIS2_CALL
            
                


/**
  * Reentrant version of `readdir'
  */
AXIS2_EXTERN int AXIS2_CALL

    
    
    


/**
  * Rewind DIRP to the beginning of the directory.
  */
AXIS2_EXTERN int AXIS2_CALL

    


/**
  * Scan the directory DIR
  *  Returns the number of entries selected, or -1 on error
  */
AXIS2_EXTERN int AXIS2_CALL

    
    
    
    
            


/**
  * Compare two `struct dirent's alphabetically
  */
extern int 
        
        

#endif  /* READDIR_H */