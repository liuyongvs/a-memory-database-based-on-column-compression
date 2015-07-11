#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include"dbmem.h"
#include"common.h"
//#include<stddef.h>

static int flag = 0;

static unsigned int memused = 0 ;

static char * dbmem_fileName = NULL ;
static  FILE * dbmem_outFile = NULL;
static  char * default_name  ="DBMEM.MEM";

void * DBcalloc(size_t nmemb,size_t size)
{
	if(flag) memused+=nmemb*size ;
	return calloc(nmemb,size);
}
void DBFree(void * ptr)
{
	free(ptr);
}
void DBmem_StartCount(void * fileName)
{
	if(flag)
	{
		fprintf(stderr,"please dispatch DBMem_EndCountAndShow\n");
		return ;
	}

	char file_name[1000]="";
	strcpy(file_name,OUT_DIR);

	if(fileName!=NULL)
	{
		dbmem_fileName=(char *)fileName;
	}
	
	if(NULL == dbmem_fileName)
	{
		dbmem_fileName = default_name;
		strcat(file_name,dbmem_fileName);
		dbmem_outFile=fopen(file_name,"w+");
	}
	else if(!dbmem_outFile)
	{
		strcat(file_name,dbmem_fileName);
		strcat(file_name,".MEM");
		dbmem_outFile=fopen(file_name,"w+");
	}

	if(dbmem_outFile == NULL)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return ;
	}

	flag = 1 ;
	memused=0 ;
	return ;
}
void DBmem_EndCountAndShow(char * description)
{
	//flag must be 1
	if(!flag)
	{
		fprintf(stderr,"Miss DBmem_StartCount\n");
		return ;
	}
	flag = 0 ;
	char tmp [1000]="";
	if(description)
	{
		strcpy(tmp,description);
	}

	if(dbmem_outFile)
	{
		fprintf(dbmem_outFile,"%s:%d(Byte)\n",tmp,memused);
		fprintf(stdout,"%s:%d(Byte)\n",tmp,memused);
		fclose(dbmem_outFile);
	}
	else
		fprintf(stderr,"err:no file open for DBmem_StartCount\n");
	memused = 0 ;
	return ;
}
