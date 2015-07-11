#ifndef __CF_F__
#define __CF_H__

#include "csv.h"

/*
 *  file funtion: declare the structs and funtions about column compression
 *  author:lanhxg
 *  date:20130831
 * */

/*
 * static variant that used in the cf compression
 *
 * */
static int cf_pos = 0  ;
/*the cf unit struct  */
struct Unit
{
	int val ;   //the value 
	short int  pos ;   //the position of the val in the csv file
	short int  pnext ; //the next column's Unit that belongs to the same row 
};

/*the cf compressed file format struct of csv file */

struct CF
{
	struct Unit * cf[500] ;    //cf file column list array
	int len[500];              //the size of each column list 
	int limit[500];            //the max size of each column list , 1.1*len[i]
	Byte PrimaryKey[2500] ;        //the first column list ,each row represent by a bit (count from 0)
	int key;                       // the primary key we have used 
};

void * CF_Malloc(FILE *fp );
int ReleaseCF(void * in);
void TestCFMem();
void * CompressCF(FILE * fp);
int ShowCFCompressedFile();
int SearchPosInCF(void * list , int l , int r , int row ,int s);
int PickElementFromCF(void * f ,int row , int col ,int * val);
int PickElementsFromCFByROW(void * f ,int row ,int * dst);
int PickElementsFromCFByROW_v2(void * f ,int row ,int * dst);
int TestAccessPerformanceINCF_v2();
int ShowCFFileMemoryUsed();
int ReadLineFromCF();
int TestReadLineFromCF(void * in);
int DeleteRowInCF(void * f ,int row);
int TestCFCompressedTimeConsumed();
int AddRowIntoCF( void * f , int *dst);
int TestAppendRowsIntoCF();
int TestAppendRowsIntoCF_v2();
int WipeAll(void * f);
#endif
