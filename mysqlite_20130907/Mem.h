#ifndef __MEM_H
#define __MEM_H

/*
 * 该文件包含用于统计各个压缩函数的内存使用情况的结构体和函数
 * 作者： lanhxg
 * 时间： 20130816
 * */
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include"common.h"
#include"csv.h"
#define MEM_FILE_NAME "mem"

struct Mem
{
	int M ;  //兆
	int K ;  
	int B ;  //字节
};

static struct Mem CF_Mem ; //游程压缩算法使用内存统计
static struct Mem DF_Mem ; //字典压缩算法使用内存统计
static struct Mem BF_Mem ; //位图压缩算法使用内存统计

/*
 * 函数功能: 初始化内存统计值，把对应位置清0
 * 参数1  :内存统计结构提 struct Mem 指针
 * */
void InitMem(struct Mem * p) ;
/*
 *  函数功能:  更新内存使用情况统计数据
 *  参数1   ： 需要更新的内存统计结构提 struct Mem 指针
 *  参数2   ： 新发现内存使用数 单位为字节 
 * */

void UpdateUsedMem(struct Mem * p,int num);

void ShowUsedMem(struct Mem * p ,int me);



#endif

