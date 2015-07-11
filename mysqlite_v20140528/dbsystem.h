/*
 * date:20130313
 * author:lanhxg
 * mail:lanhxg@gmail.com
 * funtion: compression method dispatch 
 * */
#include"lib/common.h"
#include"csv.h"

#ifndef __DBSYSTEM_H__
#define __DBSYSTEM_H__


typedef struct tARG_SEARCH
{
	void * task ;
	int * idlist;
	int times;
	int (*fun)(void*,int,char*);
}tARG_SEARCH,*pARG_SEARCH ;

typedef struct SELFT
{
	/*相同值所占的百分比*/
	float same_rate ;
	
	/*空值所占的百分比*/
	float null_rate ;

	/*连续的平均长度*/
	int sequence_avg;

	/*不同值的数目*/
	float diff_rate;

	/*平均长度*/
	int length_avg; 

}SEFT,*pSEFT;

/*
 * funtion: create the handle of db table
 * args-name:the table name
 * args-n: the number of column of the table
 * args-r:the row number of this table
 * return:the handle of the table 
 * */
void* CreateDBTable(char* name ,int n , int r);

/*
 * funtion: destroy the dbtable 
 * args-pt:the handle of the dbtable
 * return : OK if success, or ERR
 * */
int DestroyDBTable(void * pt);



/*
 * funtion : analysis the property of each column,and chose compression method
 * */
int GetCompressionInfo(pSegment in_psg ,pSEFT selinfo);

/*
 * funtion:according to selection info ,then chose a best compression method.
 * args-selinfo: selection infomation,define by below
 * return: the chosen compression method
 * */
CMethod GetCompressionMethod(pSegment selinfo);

/*
 * funtion: Do asssignment for each cloumn
 * args-sglist: segment list 
 * args-itlist: item list
 * n: the column number
 * */
int DoAssignment(pDBTable pdbtable, pSegment *sglist, pItem* itlist,int n);


/*
 * funtion:<multi thread> accordint to taskpcb, then dispatch compression method
 * return:OK if success
 * */
int DoSchedule(pMCsvFile mfile);

/*
 * funtion:make rough estimate about them memory consue of uncompressed file
 * args-_psglist:the array of pSegment of data table
 * args-len:the length of this arrayy
 * return:all the memory consumed by this table,but if fails,this funtion will return ERR
 * */
int MemoryConsueCount(pSegArray psglist , int len);



/*
 * FUNTION: test time consume by n time random search
 * ARGS-n: n times access times will be perform by this funtion
 * ARGS-mod: search range should be between 0-mod
 * RETURN: OK will be returned if funtion perform successfully
 * */
int random_read_test(int n, int mod);

/*
 * funtion: muthty thread access test
 * */
int random_read_test_v2(int n, int mod);


int random_appendtable_read(int times ,int mod,pDBTable dbtable);

/*
 * FUNTION:get the time consume by 'times' times append operation on apptable
 * ARGS-times: number of operation times, note: this value must be a integer multiple of 10000
 * ARGS-dbtable: address of DBtable
 * RETURN: OK will be returned if funtion perform successfully
 * */
int append_records_test(int times , pDBTable dbtable);
#endif
