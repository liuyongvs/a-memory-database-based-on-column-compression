#include"DF.h"


/*
 * 函数功能：创建字典压缩中的字典样本
 * 参数1：fp 文件指针
 * 参数2：指定列 从0开始计数
 * */
void * MakeSamle(FILE *fp,int col)
{
	Init();//init the pre
	struct S_Character tmp ;
	memset(&tmp,0,sizeof(tmp));

	if(ERRO==CountColumnAttribute(&tmp,fp,col))
	{
		fprintf(OUTPUT,"CountColumnAttribute erro\n");
		return (void *)ERRO;
	}
	
	struct S_Sample *ps = (struct S_Sample *) malloc(sizeof(struct S_Sample));
	UpdateUsedMem(&DF_Mem,sizeof(struct S_Sample));

	ps->len=tmp.val_cnt ;
	ps->dic=(int *)calloc(tmp.val_cnt,sizeof(int)) ;
	UpdateUsedMem(&DF_Mem,sizeof(int)*tmp.val_cnt);

	if(!ps)
	{
		fprintf(OUTPUT,"mem erro\n");
		return (void *)ERRO;
	}
	int i=0 ;
	for( i=0 ; i<tmp.val_cnt ; i++ )
	{
		((int *)ps->dic)[i]=tmp.x.c_a[i][0];
	}
	return (void *)ps ;
}
/*
 * 函数功能： 释放某一列的数据的字典样本
 * 参数1   ： 某列的样本指针
 * */
void ReleaseSample(void * sp)
{
	struct S_Sample *psp = (struct S_Sample *)sp ;
	if(!psp)
		return ;
	int *p = (int *)psp->dic;
	free(p);
	p=NULL;
	free(psp);
	psp=NULL;
}
/*
 * 函数功能：获取val值在字典中的index
 * 参数1：   字典结构体
 * 参数2：   传入的要索引的值
 * 返回值：  返回val在字典中的index
 * */
int GetIndex(void *dic ,int val)
{
	int i=0 ;
	struct S_Sample *ps = (struct S_Sample *)dic ;
	int *p  = (int *)ps->dic;
	int len =ps->len;
	if(!dic||val==-1||len<0)
		return 0 ;
	for(i=0 ; i<len ; i++)
	{
		if(p[i]==val)
			return i ;
	}
	return -1 ;
}

/*
 * 函数功能：完成字典压缩
 * 参数1：   文件指针
 * 返回值：  压缩后的二维数组，里面整个文件的索引 和每一列的样本值 
 * */
void * Dictionary_Compress(FILE * fp)
{

	if(!fp)
		return (void *)ERRO ;
	struct DF * df =NULL;

	Byte ** map=NULL ;
	int i =0 ;

	char list[COLUMN_LEN][STRING_MAX]={'\0'};
	char line[LINE_MAX]={0};
	int data =-1 ,index=0,tmp=0,size=0;
	int j = 0 ;

	df = (struct DF *)malloc(sizeof(struct DF));
	UpdateUsedMem(&DF_Mem,sizeof(struct DF));

	map= (Byte **)calloc(ROW_LEN , sizeof(Byte *));
	if(!map)
	{
		fprintf(OUTPUT,"%s\n",strerror(errno));
		return (void *)ERRO;
	}
	UpdateUsedMem(&DF_Mem,sizeof(struct Byte *)*ROW_LEN);

	for(i = 0 ; i < ROW_LEN ; i++)
	{
		
		/*reate a single column mem*/
		map[i] = (Byte *)calloc(COLUMN_LEN,sizeof(Byte));
		UpdateUsedMem(&DF_Mem,sizeof(Byte)*COLUMN_LEN);
		if(!map[i])
		{
			fprintf(OUTPUT,"%s\n",strerror(errno));
			return (void *)ERRO;
		}

		//make is column datas to dic as index samples 
		df->smp[i] = MakeSamle(fp,i);  

		index = 0 ;
		j=0 ;
		fseek(fp,0,SEEK_SET);
		while(fgets(line,LINE_MAX,fp)!=NULL)
		{
			if(line[0]=='\0')
				continue ;
			if(++index<=2)
				continue ;

			if(ROW_LEN!=cvs_parse(line,list,ROW_LEN))
			{
				fprintf(OUTPUT,"cvs_parse erro\n");
				return (void *)ERRO;
			}

			if(list[i][0]=='\0')
				continue ;
			if(0==sscanf(list[i],"%d",&data))
				continue ;
			tmp = GetIndex(df->smp[i],data);
			map[i][j++]=(Byte)(tmp&0xFF);

		}
	}
	df->pm=map ;
	return (void *)df;
}
/*
 * 函数功能：显示字典压缩态文件在内存中消耗空间
 *
 * */

int ShowDFFileMemoryUsed()
{
	struct DF *pdf ;
	FILE *fp = fopen(FILE_NAME,"r");

	InitMem(&DF_Mem);
	pdf = Dictionary_Compress(fp);
	ShowUsedMem(&DF_Mem,DICTIONARY_COMPRESSION);
	ReleaseDF(pdf);
	fclose(fp);
}

/*
 *  函数功能：显示字典压缩后的文件
 *  参数1 ：  压缩后的字典内存指针 struct DF * 
 * */

void ShowDF(  void  * p)
{
	if(!p)
		return  ;

	FILE *pt ;
	pt = fopen(DFCOMPRESSED_FILE,"w+");
	if(pt==NULL)
	{
		fprintf(OUTPUT,"%s\n",strerror(errno));
		return ;
	}

	struct DF *pdf  =(struct DF *) p ;
	struct S_Sample *ps  =NULL;
	Byte ** map =pdf->pm;

	int i , j ;

	//先显示每一列的样本
	for( i=0 ; i < ROW_LEN ; i++ )
	{
		ps =  pdf->smp[i];
		fprintf(pt,"S%d: ",i);
		for( j = 0 ; j < ps->len ; j++)
			fprintf(pt,"%d ",ps->dic[j]);
		fprintf(pt,"\n");
	}


	//显示压缩后的数据
	
	for(i=0;i<COLUMN_LEN;i++)
	{
		for(j=0;j<ROW_LEN;j++)
		{
			fprintf(pt,"%0x ",map[j][i]);
		}
		fprintf(pt,"\n");
	}

	fclose(pt);

}

/*
 * 函数功能： 释放内存中存存放的字典压缩文件
 * 参数1：    文件指针
 * */
void ReleaseDF(struct DF *fp)
{
	if(!fp)
		return ;

	Byte **map = fp->pm ;
	int i =0 ;
	for(i=0;i<ROW_LEN;i++)
	{
		ReleaseSample((void *)fp->smp[i]);
		free(map[i]);
		map[i]=NULL;
	}
	free(fp);
}


/*
 * 函数功能:获取指定行列的元素值
 *
 * */
int PickElementFromDF(void* f ,int row , int col ,int * val)
{
	if(row <0 || row > COLUMN_LEN )
		return ERRO ;
	if(col<0 || col > ROW_LEN)
		return ERRO ;
	if(!f)
		return ERRO ;

	int i = row ;
	int j = col ;
	struct DF * df = (struct DF *)f ;
	if(df->pm[j][i]>df->smp[j]->len)
		return ERRO ;

	*val= df->smp[j]->dic[df->pm[j][i]];
	return SUCCESS ;
	
}

/*
 *  函数功能:获取指定列的
 * */

int PickElementsFromDFByColumn(void * f , int col , int * dst)
{
	if(col<0 || col > ROW_LEN)
		return ERRO ;
	if(!f || !dst)
		return ERRO ;


	int i = 0 ;
	struct DF * df = (struct DF *)f ;
	for(i = 0 ; i <  COLUMN_LEN ; i++)
	{
		dst[i] = df->smp[col]->dic[df->pm[col][i]] ;
	}
	return SUCCESS ;
	

}

/*
 * 函数功能:获取指定行数据
 * */
int PickElementsFromDFByRow(void * f , int row , int *dst)
{
	if(!f||!dst)
		return ERRO ;
	if(row<0 || row > COLUMN_LEN)
		return ERRO ;


	int i = 0 ; 
	for(i = 0 ; i < ROW_LEN ; i++)
	{
		PickElementFromDF(f ,row , i ,&dst[i]);

	}
	return SUCCESS ;
	
}


/*
 * 函数功能：测试存取性能
 * 1、获取单元素数据存取性能
 * 2、获取多元素存取性能
 * 3、获取单行的存取性能
 * 4、获取多行的存取性能
 * 5、获取单列的存取性能
 * 6、获取多列的存取性能
 *
 *
 * */

int TestAccessPerformanceInDF()
{
	struct timeval tpstart , tpend ;
	memset(&tpstart,0,sizeof(struct timeval));
	memset(&tpend,0,sizeof(struct timeval));

	FILE * fp = NULL ;
	FILE * POUT = NULL ;
	fp = fopen(FILE_NAME,"r");
	POUT = fopen(ACCESS_TIME_CONSUME_FILE,"a+");

	int val = -1 ;
	int r[ROW_LEN]={0};
	int l[COLUMN_LEN]={0};
	int row[ACCESS_LIMITS] ={0};
	int col[ACCESS_LIMITS] ={0};
	long int usedtime = 0 ; 
	struct DF * pdf ;

	pdf = Dictionary_Compress(fp);
	 fprintf(POUT ,"-----------字典压缩存取性能测试（时间单位us）------------\n");
	/*开始存取时间统计*/

	//获取单个数据
	{
		srand((unsigned)time(NULL));
		row[0] = rand()%998;
		srand((unsigned)time(NULL));
		col[0] = rand()%26 ;
		
		gettimeofday(&tpstart,NULL);
		PickElementFromDF(pdf,row[0] , col[0] ,&val);
		gettimeofday(&tpend,NULL);
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"单元素存取时间：%ld\n",usedtime);
	}


	//获取多数据的存取时间 
	{
		int i = 0 ;
		for( i = 0 ; i < ACCESS_LIMITS ; i++)
		{
			srand((unsigned)time(NULL));
			row[i]=rand()%998;
			srand((unsigned)time(NULL));
			col[i]=rand()%26;
		}

		gettimeofday(&tpstart,NULL);
		for(i = 0 ; i < ACCESS_LIMITS ; i++)
		{
			PickElementFromDF(pdf,row[i] , col[i], &val);
		}
		gettimeofday(&tpend,NULL);
		usedtime=0 ;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"多元素存取时间:%ld\n",usedtime);

	}

	//单行存取时间
	{
		srand((unsigned)time(NULL));
		row[0] = rand()%998;

		gettimeofday(&tpstart,NULL);
		PickElementsFromDFByRow(pdf,row[0] , r);
		gettimeofday(&tpend,NULL);
		usedtime =0 ;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"单行存取时间:%ld\n",usedtime);

	}

	//多行存取时间
	{
		int i = 0 ;
		for(i = 0 ; i<ACCESS_LIMITS ; i++)
		{
			srand((unsigned)time(NULL));
			row[i] = rand()%998;
		}

		gettimeofday(&tpstart,NULL);
		for(i = 0 ; i < ACCESS_LIMITS ; i++)
			PickElementsFromDFByRow(pdf,row[i] , r);
		gettimeofday(&tpend,NULL);
		usedtime=0;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"多行存取时间:%ld\n",usedtime);
	}


	//单列存取时间
	{
		srand((unsigned)time(NULL));
		col[0] = rand()%26;

		gettimeofday(&tpstart,NULL);
		PickElementsFromDFByColumn(pdf,col[0],l);
		gettimeofday(&tpend,NULL);
		usedtime=0;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"单列存取时间:%ld\n",usedtime);

	}
	//多列存取时间 
	{
		int i =0 ;
		for(i = 0 ; i < ACCESS_LIMITS ; i++)
		{
			srand((unsigned)time(NULL));
			col[i] = rand()%26;
		}
		gettimeofday(&tpstart,NULL); 
		for(i=0 ; i < ACCESS_LIMITS ; i++)
			PickElementsFromDFByColumn(pdf,col[i],l);
		gettimeofday(&tpend,NULL);
		usedtime=0;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"多列存取时间:%ld\n",usedtime);

	}


	ReleaseDF((struct DF * )pdf);
	fclose(fp);
	fclose(POUT);

}
/*
 * 函数功能：导出压缩太文件到指定文件 
 * */

//	DFCOMPRESSED_FILE
int ShowDFCompressedFile()
{
	FILE *fp =  NULL ;
	fp = fopen(FILE_NAME,"r");
	struct DF * pdf ;
	pdf = Dictionary_Compress(fp);
	ShowDF(pdf);
	ReleaseDF((struct DF * )pdf);
	fclose(fp);
}
