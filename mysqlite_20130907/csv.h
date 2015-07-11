#ifndef __CVS_H__
#define __CVS_H__


#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<stdio.h>
#include<limits.h>
#include<sys/time.h>
#include<time.h>

#include "Mem.h"
#include "common.h"


//#define FILE_NAME "sample0806.csv"
//#define FILE_NAME "data0805sample13.csv"
#define FILE_NAME "in_3.csv"
#define STRING_MAX 10   //用字符串表示数值的最大长度
#define COLUMN_LEN 13072 //每列长度
//#define COLUMN_LEN 17665 //每列长度
#define ROW_LEN    25   //每行的长度
//#define ROW_LEN    44   //每行的长度

#define ERRO          -1 //哈系值添加错误
#define SUCCESS_EXIT   0 //哈系值更新成功
#define SUCCESS        1 //       添加成功      


#define OUTPUT   stdout  //输出地方，以后修改这个值也输出到文件

#define VAL       0      
#define REPEAT    1
#define AVERAGE_LEN 2

typedef	unsigned char Byte ;
typedef	short int int16 ;

/*
 *
 * 统计行/列属性
 *
 *	1.取值数目（有多少个数值）
 *	2.相同值的数目（每个值的重复次数）
 *	3.相同值的总的样本数目（有多少个数值存在重复出现的）
 *	4.相同值连续长 度平均值
 *
 * */
struct S_Character
{
	int cnt ; //该行/列中存在多少个数值

	/*
	 *  该联合体内的值 按照 
	 *  c_a[i][0],c_a[i][1], c_a[i][2], c_a[i][3] 
	 *  出现的值 ,重复次数 , 连续值的平均长度 ，备用  
	 * */
	union 
	{ 
		int c_a[COLUMN_LEN][4];
		int r_a[ROW_LEN][4];

	} x ;
	int val_cnt   ; //值不相同的元素个数
	int repeat_cnt; //重复(不止一次出现)的个数
	int max_size ;//元素值在字符形态的最大长度

	int segs ;      //分段数
}; 



int cvs_parse(char * line ,  char list[][STRING_MAX] , int num) ;

struct Val
{
	int val ;
	int repeat_cnt;
	int times ;
	struct Val* pnext ;
};

static int pre ;
static int pos ;
struct Val * set[100]; //指针数组  哈系值
void Intipre();
int AddVal(int new );
int getRepeatCnt(int s);
void releaseHset();

//Init the pre
void Init();
//列属性统计
int CountColumnAttribute(struct S_Character * att , FILE *fp ,int which);
//行属性统计
int CountRowAttribute(struct S_Character * att , FILE *fp ,int which);



int ShowFileColumnsAttribute() ;


int PickElementFromOr(FILE* fp ,int row , int col ,int * val);
int PickElementsFromOrByColumn(FILE * fp , int col , int * dst);
int PickElementsFromOrByRow(FILE *pf , int row , int *dst);
int TestAccessPerformanceInOr();


int PickElementFromMem(int row , int col ,int * val);
int PickElementsFromMemByColumn( int col , int * dst);
int PickElementsFromMemByRow(int row , int *dst);
int TestAccessPerformanceInMem();

void * ReadCSVFileIntoMem(int row ,int col);
int FreeCSVMem(void * in , int row );
int ShowMemCSVFile();
#endif
