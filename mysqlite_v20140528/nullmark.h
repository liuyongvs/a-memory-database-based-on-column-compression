/*
 * date:20140318
 * author:lanhxg
 * funtion:compression the column whose item all are null
 * */
#ifndef	__NULLMARK_H__
#define	__NULLMARK_H__
#include"lib/common.h"
#include"csv.h"


/*
 * funtion:if whole column is null, the set kcolumn's flag filed to be zero, which means column is null
 * args-pdata: address of  taskpck  of null column
 * return: OK if success
 * */
int NullSuspendDispatch(void * pdata);

int search_NMtask_record(void * pdata,int id ,char * _line);
#endif
