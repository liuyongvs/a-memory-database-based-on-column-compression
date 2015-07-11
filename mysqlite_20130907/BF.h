#ifndef __BF_H__
#define __BF_H__

#include"csv.h"

/*
 *  文件功能：该文件主要包含位图压缩的相关函数声明和结构体
 *  具体的函数定义在BF.c中定义
 *  作者：lanhxg
 *  时间：20130817
 * */



/*位图压缩结构体*/
/*  Bits  用来存储某一列中某个元素的信息*/
struct Bits
{
	int val ;       //值 
	int len ;       //改值重复次数
	int16 *pb ;      //位置指针
};

/*位图压缩文件*/
struct BF
{
	struct Bits * bf[ROW_LEN] ;//文件指针
	int each_len[ROW_LEN];
};


void * CompreesBF(FILE * fp);
void ReleaseBF(void * p);
void ShowBF(void * p);

int PickELementFromBF(void  * f ,  int row , int col ,int * val);
int PickElementsFromBFByColumn(void * f , int col , int * dst);
int PickElementsFromBFByRow(void * f , int row , int * dst);
int TestAccessPerformanceInBF();

int ShowBFFileMemoryUsed() ;
int ShowBFCompressedFile();
int SearchPos(void *in,int l , int r ,int16 * val);
#endif 
