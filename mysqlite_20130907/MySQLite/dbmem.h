/*
 * AUTHOR: lanhxg
 * DATE  :20131006
 * FUNTION: wrap some funtions like calloc  
 * */


#ifndef __DBMEM_H__
#define __DBMEM_H__
#include<stddef.h>

/*
 *
 * */
void * DBcalloc(size_t nmemb,size_t size);
void DBFree(void * ptr);
void DBmem_StartCount(void * fileName);
void DBmem_EndCountAndShow(char * description);
#endif
