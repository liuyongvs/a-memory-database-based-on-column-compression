#ifndef __DF_H__
#define __DF_H__

#include"csv.h"



/*
 *  文件功能：该文件主要包含字典压缩的相关函数声明和结构体
 *  具体的函数定义在CF.c中定义
 *  作者：lanhxg
 *  时间：20130817
 * */




/*
 * 字典压缩的样本
 *
 * */
struct S_Sample
{
	int len ;//样本长度
	int * dic ;//样本数组
};
/*字典压缩*/
struct DF
{
	struct S_Sample * smp[ROW_LEN] ; //样本指针
	Byte **pm ;                      //压缩文件
};



void ReleaseSample(void * sp);
int GetIndex(void *dic ,int val);
void * Dictionary_Compress(FILE * fp);
void ShowDF(  void  * p);
void ReleaseDF(struct  DF * fp);


int PickElementFromDF(void * f ,int row , int col ,int * val);
int PickElementsFromDFByColumn(void * f , int col , int * dst);
int PickElementsFromDFByRow(void * f , int row , int *dst);
int TestAccessPerformanceInDF();

int ShowDFFileMemoryUsed();
int ShowDFCompressedFile();
#endif
