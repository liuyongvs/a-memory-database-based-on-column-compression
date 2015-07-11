#include "Mem.h"

static FILE * PF_MEMMERY_ACCOUNT = NULL ;
/*
 * 函数功能: 初始化内存统计值，把对应位置清0
 * 参数1  :内存统计结构提 struct Mem 指针
 * */
void InitMem(struct Mem * p)
{
	if(!p)
		return  ;
	memset(p,0,sizeof(struct Mem));


}
/*
 *  函数功能:  更新内存使用情况统计数据
 *  参数1   ： 新发现内存使用数 单位为字节 
 * */

void UpdateUsedMem(struct Mem * p,int num)
{

	if(num<0 || !p) return  ;
	p->B+=num ;
	p->K+=p->B/1024;
	p->M+=p->K/1024;
	
	p->B=p->B%1024;
	p->K=p->K%1024;

	//fprintf(stdout,"%d\n",num);

}


void ShowUsedMem(struct Mem * p ,int me)
{
	if(!p) return ;
	int tmp = 0 ; 
	int all =0 ;
	tmp = p->M*1024*1024+p->K*1024+p->B ;
	//all = 1000*26*4 ;
	all = COLUMN_LEN *ROW_LEN*4 ;

	FILE *pf = NULL ;
	pf=fopen(MEM_TEST_FILE,"a+");
	if(!pf)
	{
		fprintf(stdout,"funtion:ShowUsedMem->me=%d file open occur erro\n",me);
		return ;
	}
	switch(me)
	{
		/*游程压缩*/
		case COLUMN_COMPRESSION:
			{
				fprintf(pf,"游程压缩:");
				fprintf(pf,"%dB / %dB :%f\n",tmp,all,(1.0*tmp)/all);
			}
			break ;
		/*字典压缩*/
		case DICTIONARY_COMPRESSION:
			{
				fprintf(pf,"字典压缩:");
				fprintf(pf,"%dB / %dB :%f\n",tmp,all,(1.0*tmp)/all);
			}
			break ;
		/*位图压缩*/
		case BITMAP_COMPRESSION:
			{
				fprintf(pf,"位图压缩:");
				fprintf(pf,"%dB / %dB :%f\n",tmp,all,(1.0*tmp)/all);
			}
			break ;
	}

}
