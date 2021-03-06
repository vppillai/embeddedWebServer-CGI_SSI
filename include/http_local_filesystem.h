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

#ifndef __HTTP_LOCAL_FILESYSTEM_H__
#define __HTTP_LOCAL_FILESYSTEM_H__
#include "http_response.h"
#include "http_common.h"
#include "http_config.h"

#define HTTP_LOCAL_FILESYSTEM_NUMFILES 5

typedef struct
{
    const char *filePath;                 //path string for reverse searching
    unsigned int fileNumber;              //index of this file into the file system array
    char *file;                           //actual file contents as a char array
    long int fileLength;                  //length of teh file contents array
    http_response_fileType_t optFileType; //optional field to hold mime file type. this may be useful to indicate alternate mime type.
} http_localfs_filesystem_file_t;

typedef struct
{
    unsigned int fileNumber;   //index into teh file system to be used to identify the file.
    unsigned int filePosition; //current position during file operations
} http_localfs_filesystem_fpRoot_t;

typedef http_localfs_filesystem_fpRoot_t *http_localfs_filesystem_fp_t;

//initialize file system
int http_localfs_init(void);
//returns a file pointer that can be used for future file operations
http_localfs_filesystem_fp_t http_localfs_fopen(const char *filename);
/*register a new file to the local FS
    IN
        - filepath should be a static string constant including / . e.g: "/index.html"
        - file is the char array containing file contents
        - file length is the length of file array
        - optFileType is an optional filetype registration to be used as mime type when serving the page
            - this will be obtained from file extension using default mapings
            -- this parameter can be used to overrode default. otherwise keep as 0.
    OUT
        - returns HTTP_SUCCESS for success, HTTP_FAILURE for failure
*/
int http_localfs_registerFile(const char *filePath, char *file, unsigned int fileLength, http_response_fileType_t optFileType);

/*function to free the function pointer. to be called once fops are done*/
int http_localfs_fclose(http_localfs_filesystem_fp_t fp);

//function to unmount the local file system. all FPs should be closed before calling this to avoid memory leaks
int http_localfs_deinit(void);

/*regular fgetc implementation for the local file system. 
 reads the next character from stream and returns it as an unsigned char cast to an int, or EOF (-1) on end of file or error.
*/
int http_localfs_fgetc(http_localfs_filesystem_fp_t fp);

//generic feof function. returns -1 if EOF
int http_localfs_feof(http_localfs_filesystem_fp_t fp);

//fread implementation for local file system
size_t http_localfs_fread(void *ptr, size_t size, size_t nmemb, http_localfs_filesystem_fp_t fp);

//generic fileno function
int http_localfs_fileno(http_localfs_filesystem_fp_t fp);

//generic ftell function
long http_localfs_ftell(http_localfs_filesystem_fp_t fp);

//generic rewind function
void http_localfs_rewind(http_localfs_filesystem_fp_t fp);

//generic fseek
int http_localfs_fseek(http_localfs_filesystem_fp_t fp, long offset, int whence);


#endif