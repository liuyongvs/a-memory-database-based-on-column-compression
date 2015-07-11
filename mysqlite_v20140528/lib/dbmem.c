#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include"dbmem.h"
#include"common.h"



static int flag = 0;

static unsigned int memused = 0 ;
static char * dbmem_fileName[1000] ;
static  FILE * dbmem_outFile = NULL;
static  char * default_name  ="dbmem.msg";

void * DBcalloc(unsigned int nmemb,unsigned int size)
{
	void * ptr  = calloc(nmemb,size);
	if(ptr==NULL)
	{
		debug_err("%s",strerror(errno));
		return ptr ;
	}
	if(flag) 
	{
	  memused+=nmemb*size ;
	}

	return (void *)ptr ;
}
void DBfree(void * ptr)
{
	free(ptr);
	ptr=NULL;
}
void DBmem_StartCount()
{
	if(flag)
	{
		debug_err("please make sure flag equal to false");
		return ;
	}
	flag = 1 ;
	memused=0 ;
	memset( dbmem_fileName,0,sizeof(dbmem_fileName));
	return ;
}
/*
 * funtion:make memory compressed rate count
 * args-filename:the output filename
 * args-description:extra infomation addy by user
 * args-m:memory consumed by uncompressed table 
 * */
void DBmem_EndCountAndShow(char * filename,char * description ,int m)
{
	//flag must be 1
	if(!flag) {
		debug_err("make sure dispatch DBmem_StartCount");
		return ;
	}
	if(filename==NULL) {
		strcpy((char *)dbmem_fileName,default_name);
	} else {
		strcpy((char *)dbmem_fileName,filename);
	}


	dbmem_outFile = fopen((char *)dbmem_fileName,"w");
	if(dbmem_outFile == NULL ) {
		debug_err("open file <%s> error:%s",(char *)dbmem_fileName,strerror(errno));
		return ;
	}

	flag = 0 ;
	char tmp [1000]="";
	if(description) {
		strcpy(tmp,description);
	}

	if(dbmem_outFile) {
		fprintf(dbmem_outFile,"%s:%d/%d(Byte) rate:%lf\%\n",tmp,memused,m,(100.0*memused)/m);
		fprintf(stdout,"%s:%d(Byte)\n",tmp,memused);
		fclose(dbmem_outFile);
	}
	else
		debug_err("no file open for DBmem_StartCount");
	memused = 0 ;
	return ;
}
