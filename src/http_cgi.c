/**MIT License

Copyright (c) 2018 Vysakh P Pillai

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**/


#include <string.h>
#include <stdlib.h>
#include "http_cgi.h"
#include "http_config.h"
#include "http_common.h"

/*defining this as array for the following reasons
1) lookup speed is better than linked list
2) configuration is static and fixed at compile time. This can be adjusted in http_config.h 

return actual buffer content length on success. -1 on failure
*/
http_CGI_pathFunction_t CGI_path[HTTP_MAX_CGI_SIZE]; //hoping compiler to do its job and init to 0

int http_CGI_exec_pathFunction(char *CGIPath, char *replacerBuffer, unsigned int bufferLength)
{
    int i;
    for (i = 0; i < HTTP_MAX_CGI_SIZE; i++)
    {
        if (NULL != CGI_path[i].CGI_path)
        {
            if (0 == strcmp(CGIPath, CGI_path[i].CGI_path))
            {
                return CGI_path[i].CGI_pathFunction(CGIPath, replacerBuffer, bufferLength);
            }
        }
    }
    return -1;
}

http_CGI_pathFunctionHandle_t http_CGI_register_pathFunction(const char *CGIPath, http_CGI_pathFunction_cb CGIPathFunctionCb, http_response_contenttype_t contentType)
{
    //find a uninitialized array element
    if ((NULL == CGIPath) || (NULL == CGIPathFunctionCb) )
    {
        PRINT_ERROR("ERROR: http_CGI_register_pathFunction - NULL CGIPath or CB (%p)\r\n", (void *)CGIPath);
        return 0;
    }
    else
    {
        int i = 0;
        for (i = 0; i < HTTP_MAX_CGI_SIZE; i++)
        { //find an empty slot and fill in the registration
            if (NULL == CGI_path[i].CGI_path)
            {
                //allocate memory and copy string to it.
                CGI_path[i].CGI_path = (char *)malloc((size_t)strlen(CGIPath) + 1);
                strncpy(CGI_path[i].CGI_path, CGIPath, strlen(CGIPath));
                CGI_path[i].CGI_path[strlen(CGIPath)] = 0;

                CGI_path[i].CGI_pathFunction = CGIPathFunctionCb;
                CGI_path[i].contentType = contentType;
                return &CGI_path[i];
            }
        }
        PRINT_ERROR("ERROR: http_CGI_register_pathFunction - no more slots to register(%d)\r\n", i);
        return NULL;
    }
    return NULL; //Just a safety net.
}

void http_CGI_deRegister_pathFunction(http_CGI_pathFunctionHandle_t deregisterHandle)
{
    //free the strings?
    //make contents in array 0
    free(deregisterHandle->CGI_path);
    deregisterHandle->CGI_path = NULL;
    deregisterHandle->CGI_pathFunction = NULL;
    deregisterHandle = NULL;
}

//deregister all registrations in one-go. Useful to implement a shutdown function
void http_CGI_deRegister_all(void)
{
    int i = 0;
    for (i = 0; i < HTTP_MAX_CGI_SIZE; i++)
    { //find an empty slot and fill in the registration
        if (0 != CGI_path[i].CGI_path)
        {
            //free the string memory from heap
            free(CGI_path[i].CGI_path);
            CGI_path[i].CGI_path = NULL;
            CGI_path[i].CGI_pathFunction = NULL;
        }
    }
}

//function to get contentType form a pathFunction handle
http_response_contenttype_t http_cgi_get_contentType(http_CGI_pathFunctionHandle_t pathFunctionHandle)
{
    if (NULL != pathFunctionHandle)
    {
        return pathFunctionHandle->contentType;
    }
    return 0; //no content type registered
}

//function to get pathFunction handle of a path
http_CGI_pathFunctionHandle_t http_CGI_get_pathFunctionHandle(char *CGIPath)
{
    int i;
    for (i = 0; i < HTTP_MAX_CGI_SIZE; i++)
    {
        if (NULL != CGI_path[i].CGI_path)
        {
            if (0 == strcmp(CGIPath, CGI_path[i].CGI_path))
            {
                return &CGI_path[i];
            }
        }
    }
    return NULL;
}

//temporarily commented off to fix pedantic errors. Not major since this is this is a test function. ISSUE #2
#ifdef HTTP_REPLACER_TABLE_DEBUG_PRINTF
//test function to print complete registration table. to be used only for debugging
void http_CGI_printPathFunctionTable(void)
{
    int i;
    printf("FUNCTION\t\tPATH\r\n");
    for (i = 0; i < HTTP_MAX_CGI_SIZE; i++)
    {
        printf("%p\t\t", (void *)CGI_path[i].CGI_pathFunction);
        printf("%s\r\n", CGI_path[i].CGI_path);
    }
    printf("==============================================================\r\n");
}
#endif