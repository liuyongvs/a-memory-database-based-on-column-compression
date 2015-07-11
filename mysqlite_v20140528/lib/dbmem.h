/*
 * AUTHOR: lanhxg
 * DATE  :20131006
 * FUNTION: wrap some funtions like calloc  
 * */


#ifndef __DBMEM_H__
#define __DBMEM_H__


extern void * DBcalloc(unsigned int nmemb,unsigned int size);
extern void DBfree(void * ptr);
extern void DBmem_StartCount();
extern void DBmem_EndCountAndShow(char * filename,char * description ,int m);

#endif
