/*
 * AUTHOR: lanhxg
 * DATE:   20131006
 * FUNTION: macro defination must be here
 * */
#ifndef __COMMON_H__
#define __COMMON_H__

#include<stdio.h>




#define NUMMD 5  /*number of compression method */
#define OK 0
#define ERR -1
#define true 0x01
#define false 0x00 
#define EPSILON 0.000000001f
#define MAXLEN 20000UL

#define abs(x) ((x)<0?(-x):(x))
#define ValueOf(p,type)  (*(type)(p))
#define AddrOf(p,type)  ((type)(p))
#define SetValueOf(p,v,type) ((ValueOf(p,type))=(v))
#define FFILTER(stream) ((stream)==NULL)?(stdout):(stream)


/* compression method list */
typedef enum CMethod
{
	/*original data*/
	mOrin,
	/*run-length compression method*/
	mRunLenth ,
	/*dictionary-based compression*/
	mDicBase ,
	/*null-suppresion compression*/
	mNullSp  ,
	/*bit vector compression*/
	mBitVector 
}CMethod;

/*data type */
typedef struct Date
{
	int year;
	unsigned char month;
	unsigned char date;
} Date;
typedef struct DateTime
{
	int year;
	unsigned char month;
	unsigned char date;
	unsigned char hour;
	unsigned char minute;
	unsigned char sec;
}DateTime;
typedef int Interger;
typedef long Long;
typedef float Single;
typedef double Double;
typedef unsigned char Byte;
typedef unsigned char bool;
typedef unsigned char varchar;
/*belowing data type also should be supported*/
/*
 * int 
 * float
 * double
 * char
 * */
/*data type end*/

/*common data type*/
typedef enum DBtype
{
	bool_type,
	char_type,  
	int_type,  
	float_type,
	double_type,
	Date_type,
	DateTime_type,
	Interger_type,
	Long_type,
	Single_type,
	Byte_type,
	varchar_type,
	string_type 
}DBtype;

typedef struct CAttribute
{
	char colName[32];
	DBtype type ;
	int size ;
}CAttribute,*pCAttribute;

typedef CAttribute CAttrtList[1024];
typedef struct Column
{
	/*pointer that */
	void * pdata ;
	DBtype type ;
	/*row number of this column*/
	 int len ;
	
}Column,*pColumn;

typedef struct KColumn
{
	/*extra data*/
	unsigned long pdata ;

	/*pointer  of the colun address */
	/*
	 * if addr =0 ,means all the column is null
	 * */
	unsigned long addr ;

	/*segment*/
	unsigned long psg;

	/*data type*/
	DBtype type ;

	/*compression method*/
	CMethod me ;

	/*column name*/
	char name[32];

	/* row number of this column*/
	 int len ;

	 /*the item data size*/
	 int maxsize;

	 /* 0: represent whole column is empty and addr ==0
	  * 1: */
	char flag ;
}KColumn,*pKColumn;



typedef pColumn pColArray[1024];

typedef struct ColSet
{
	pColArray colArray ;
	int len ;
}ColSet,*pColSet ;

typedef struct kColSet
{
	int len ;
#if 1
	pKColumn  kcolarray[1024];
#else
	pKColumn  kcolarray[0];
#endif
}KColSet,*pKColSet;




typedef struct DBTable
{
	//the table file name
	char name[100];
	//the column number
	int col ;
	//the row number
	int row ;
	//the next DB file
	struct DBTable * pnext ;
	//all the column ,index by 0 ...
	KColumn *colarray;
	void * tmptable ;//additional space for append records
}DBTable,*pDBTable ;




typedef struct TaskPCB
{
	/*the number of column of this table*/
	int column;

	/*the amount of columns  suits to be compressed by this compression method*/
	int count ;

	/*row number*/
	int row;

	/*the column index array, whose lenght given by count field */
	int* indexlist;

	/*needed memery size of each column */
	int * memsizelist ;

	/*data type of each column*/
	DBtype* typelist;

	/*the pitem of each column*/
	//pItem* pitlist;
	unsigned long pitlist;

	/*the compressed columns */
	pKColumn kcollist;

	/*compress method */
	CMethod  method ;

	/*pointer of compression funtion*/
}TaskPCB,*pTaskPCB;



#define debug(format,...) printf("%s:%s:%d:--"format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define debug_err(format,...) fprintf(stderr,"%s:%d:--"format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)
#define debug_ok(format,...) fprintf(stdout,"%s:%d:--"format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)
#endif
