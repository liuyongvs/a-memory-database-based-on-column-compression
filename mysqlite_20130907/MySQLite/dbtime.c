
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<errno.h>
#include<string.h>
#include"dbtime.h"
#include"common.h"

char * dbtime_fileName = NULL ;
static FILE * dbtime_outFile = NULL ;

static char * default_name = "DBTIME.TIME";

static struct timeval  tpstart;
static struct timeval  tpend  ;


static int flag = 0 ;


void DBtime_StartCount(void * fileName)
{
	if(flag)
	{
		fprintf(stderr,"please dispatch DBtime_EndCountAndShow\n");
		return ;
	}

	char file_name[1000]="";
	strcpy(file_name,OUT_DIR);

	if(fileName!=NULL)
	{

		dbtime_fileName= (char *)fileName ;
	}

	if(NULL==dbtime_fileName)
	{
		dbtime_fileName = default_name ;
		strcat(file_name,dbtime_fileName);
		dbtime_outFile  = fopen(file_name,"w+");
	}
	else if(!dbtime_outFile)
	{

		strcat(file_name,dbtime_fileName);
		strcat(file_name,".TIME");
		dbtime_outFile = fopen(file_name,"w+");
	}
       if(dbtime_outFile==NULL)
       {
	       fprintf(stderr,"%s\n",strerror(errno));
	       return  ;
       }	

	flag = 1 ;
	memset(&tpstart,0,sizeof(struct timeval));
	memset(&tpend  ,0,sizeof(struct timeval));

	gettimeofday(&tpstart,NULL);
	return ;
}
void DBtime_EndCountAndShow()
{

	//flag must be 1
	if(!flag)
	{
		fprintf(stderr,"Miss DBtime_StartCount\n");
		return ;
	}
	flag = 0 ;
	gettimeofday(&tpend,NULL);

	struct timeval tmp ;
	memset(&tmp,0,sizeof(struct timeval));
	tmp.tv_usec = tpend.tv_usec - tpstart.tv_usec ;
	tmp.tv_sec  = tpend.tv_sec  - tpstart.tv_sec  ;
	long int usedtime = 0;
	usedtime = 1000000*tmp.tv_sec + tmp.tv_usec ;
	
	if(dbtime_outFile)
	{
		fprintf(dbtime_outFile,"time consume:%ld(us)\n",usedtime);
		fprintf(stdout,"time consume:%ld(us)\n",usedtime);
		fclose(dbtime_outFile);
	}
	else
	{
		fprintf(stderr,"no file open \n");
	}
	return ;
}
