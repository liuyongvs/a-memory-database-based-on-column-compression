
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<errno.h>
#include<string.h>
#include"dbtime.h"
#include"common.h"

static char  dbtime_fileName[1000] ;
static char * default_name = "dbtime.msg";

static FILE * dbtime_outFile = NULL ;

static struct timeval  tpstart;
static struct timeval  tpend  ;

static int flag = 0 ;


void DBtime_StartCount()
{
	if(flag) {
		debug_err("make sure dispatch DBtime_EndCountAndShow before");
		return ;
	}
	flag = 1 ;
	memset( dbtime_fileName,0,sizeof(dbtime_fileName));
	memset(&tpstart,0,sizeof(struct timeval));
	memset(&tpend  ,0,sizeof(struct timeval));

	gettimeofday(&tpstart,NULL);
	return ;
}
void DBtime_EndCountAndShow(char * filename ,char * descrip)
{

	//flag must be 1
	if(!flag) {
		debug_err("make sure dispatch DBtime_StartCount before");
		return ;
	}

	flag = 0 ;
	if(filename==NULL) {
		strcpy((char *)dbtime_fileName,default_name);
	} else {
		strcpy((char *)dbtime_fileName,filename);
	}

	dbtime_outFile  = fopen((char *)dbtime_fileName,"a");

   if(dbtime_outFile==NULL) {
		debug_err("%s",strerror(errno));
	    return  ;
   }	

	gettimeofday(&tpend,NULL);

	struct timeval tmp ;
	memset(&tmp,0,sizeof(struct timeval));
	tmp.tv_usec = tpend.tv_usec - tpstart.tv_usec ;
	tmp.tv_sec  = tpend.tv_sec  - tpstart.tv_sec  ;
	long int usedtime = 0;
	usedtime = 1000000*tmp.tv_sec + tmp.tv_usec ;
	
	if(dbtime_outFile) {
		fprintf(dbtime_outFile,"%s time consume:%ld(us)\n",descrip,usedtime);
		debug("time consume:%ld(us)",usedtime);
		fclose(dbtime_outFile);
	} else {
		debug_err("no file open");
	}
	return ;
}
void random_list(int n ,int * list, int mod)
{
	if(!list || n < 0){
		debug_err("parameter error");
		return ;
	}
	int i  =0 ; 
    struct timeval time ;
	gettimeofday(&time,NULL);
	srand(time.tv_sec*1000 +time.tv_usec/1000);
	for(i = 0 ; i < n ; i++ ){
		list[i]=random()%mod;
	}
	return ;
}
