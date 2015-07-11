#include"BF.h"


/*
 * 函数功能：位图压缩
 * 参数1：   文件指针
 *
 * */
void * CompreesBF(FILE * fp)
{
	if(!fp)
		return (void *)ERRO ;

	/*
	 * 1.统计每一列的属性 ： 得到有 val_cnt 和 c_a[i][0] c_a[i][1]
	 *
	 * */

	struct BF * pbf ;
	struct S_Character COL_Charcter ; //属性

	pbf = (struct BF *)calloc(1,sizeof(struct BF));
	UpdateUsedMem(&BF_Mem,sizeof(struct BF));

	if(!pbf)
	{
		fprintf(OUTPUT,"%s\n",strerror(errno));
		return (void *)ERRO ;
	}


	int i = 0 ;
	int j = 0 ;
	int k = 0 ;
	int len =0;
	char list[COLUMN_LEN][STRING_MAX]={'\0'};
	char line[LINE_MAX]={0};
	int data =-1  ;
	int index ;
	/*对所有的列进行压缩*/
	for(i = 0 ; i < ROW_LEN ; i++)
	{
		//统计改列的属性
		memset(&COL_Charcter,0,sizeof(COL_Charcter));
		CountColumnAttribute(&COL_Charcter,fp,i);

		len= COL_Charcter.val_cnt ;
		pbf->each_len[i]=len ;
		pbf->bf[i] = (struct Bits *)calloc(len,sizeof(struct Bits));
		UpdateUsedMem(&BF_Mem,sizeof(struct Bits)*len);

		for(j=0 ; j < len ; j++ )
		{
			pbf->bf[i][j].val = COL_Charcter.x.c_a[j][0];
			pbf->bf[i][j].len = COL_Charcter.x.c_a[j][1];
			pbf->bf[i][j].pb  = (int16 *)calloc(COL_Charcter.x.c_a[j][1],sizeof(int16));
			UpdateUsedMem(&BF_Mem,sizeof(int16)*COL_Charcter.x.c_a[j][1]);

			//找出该值在文件中对应列的位置 
			index=0 ;
			k=0;
			fseek(fp,0,SEEK_SET);
			while(fgets(line,LINE_MAX,fp)!=NULL)
			{
				if(line[0]=='\0')
					continue ;
				if(++index<=2)
					continue ;
				if(ROW_LEN != cvs_parse(line,list,ROW_LEN))
				{
					fprintf(OUTPUT,"cvs_parse erro\n");
					return (void *)ERRO ;
				}

				if(list[i][0]=='\0')
					continue ;

				if(0==sscanf(list[i],"%d",&data))
					continue ;

				if(data==pbf->bf[i][j].val)
				{
					pbf->bf[i][j].pb[k++]=index;
				}

				if(k>=COL_Charcter.x.c_a[j][1])
					break ;
			}

		}

	}
	//ShowUsedMem(&BF_Mem,BITMAP_COMPRESSION);
	return (void *)pbf;
}
/*
 * 函数功能：内存压缩态空间大小统计
 * */
int ShowBFFileMemoryUsed()
{
	struct BF *pbf ;
	FILE *fp = fopen(FILE_NAME,"r");

	InitMem(&BF_Mem);
	pbf = CompreesBF(fp);
	ShowUsedMem(&BF_Mem,BITMAP_COMPRESSION);
	ReleaseBF(pbf);
	fclose(fp);
}


/*
 * 函数功能： 释放内存中的位图压缩文件占用的内存
 * 参数1：    位图文件占用内存指针  struct BF *
 * */

void ReleaseBF(void * p)
{
	if(!p)
		return ;

	struct BF * pb = (struct BF*)p;
	/*
	 * 1.释放里里面每个元素的重复的index
	 * 2.释放每一列的
	 * 3.释放本身
	 * */

	int i = 0 ;
	int j = 0 ;
	int l =0 ;
	int16 *pt =NULL;
	for(i = 0 ; i < ROW_LEN ; i++)
	{
		l = pb->each_len[i];
		for(j=0;j<l;j++)
		{
			pt=pb->bf[i][j].pb;
			free(pt);
		}
		free(pb->bf[i]);
	}
	free(pb);

}

/*
 * 函数功能：显示位图压缩后的文件
 * 参数1：   位图文件内存指针 struct BF *
 *
 * */
void ShowBF(void * p)
{
	if(!p)
		return ;

	FILE *pt ;
	pt = fopen(BFCOMPRESSED_FILE,"w+");
	if(pt==NULL)
	{
		fprintf(OUTPUT,"%s\n",strerror(errno));
		return ;
	}

	struct BF * pbf = (struct BF *)p ;

	int i =0 ;
	int j =0 ;
	int k =0 ;
	int kk=0 ;
	int l =0 ;

	for(i = 0 ;  i <  ROW_LEN ;  i++ )
	{
		j = pbf->each_len[i] ;
		for( k = 0 ; k < j ; k++ )
		{
			l=pbf->bf[i][k].len ;
			fprintf(pt,"%d :",pbf->bf[i][k].val);
			for(kk=0;kk<l;kk++)
			{
				fprintf(pt,"%d ",pbf->bf[i][k].pb[kk]);
			}
			fprintf(pt,";");
		} 
		fprintf(pt,"\n");
	}

	fclose(pt);

}

/*
 * 函数功能： 读取指定行列的元素
 * */
int PickELementFromBF(void  * f ,  int row , int col ,int * val)
{
	if(!f || ! val)
		return ERRO ;
	if(row < 0 || row > COLUMN_LEN)
		return ERRO ;
	if(col < 0 || col >ROW_LEN)
		return ERRO ;

	int i = row ;
	int j = col ;
	struct BF * pfile  = (struct BF *)f;
	struct Bits * pbits = pfile->bf[j] ;
        int16 * pos =NULL ;	

	/*
	 * 找到该元素的位置
	 * */

	int k = 0 ;
	int len = 0 ;
	int m = 0 ;
	int res = -1;
	int16 pos1=0 ;
	for( k = 0 ; k < pfile->each_len[j] ; k++)
	{
		len = pbits[k].len ;
		pos = pbits[k].pb ;
		/*
		for( m = 0 ; m < len ; m++ )
		{
			if(pos[m]==i)
			{
				*val = pbits[k].val ;
				return SUCCESS ;
			}

		}
		*/
		pos1=(int16)i ;
		res =(int)SearchPos(pos,0,len-1,&pos1);
		if(res==-1)
			continue ;
		*val = pbits[k].val ;
		return SUCCESS ;
	}
	return ERRO ;

}
int SearchPos(void *in,int l , int r ,int16 * val)
{
	int16 *poslist = (int16 *)in;
	if(r-l<0)
		return ERRO;
	int mid=0 ;
	mid = (l+r)/2 ;
	if(poslist[mid]==*val)
		return mid ;
	if(poslist[mid]<*val)
		return SearchPos(in,mid+1,r,val);
	else
		return SearchPos(in,l,mid-1,val);

}

/*
 * 函数功能：根据给出的列获该列的值
 * */
int PickElementsFromBFByColumn(void * f , int col , int * dst)
{
	if(!f || !dst)
		return ERRO ;
	if(col<0 ||  col > ROW_LEN)
		return ERRO ;

	int j = col ;
	struct BF * pfile  = (struct BF *)f;
	struct Bits * pbits = pfile->bf[j] ;
        int16 * pos =NULL ;	

	int i = 0 ;
        int len = 0 ;
	int m = 0 ;

	for( i = 0 ; i < pfile->each_len[col] ; i++)
	{
		len = pbits[i].len ;
		pos = pbits[i].pb  ;
		for( m = 0 ; m < len ; m++)
		{
			dst[pos[m]] =pbits[i].val ; 
	
		}
	}
	return SUCCESS ;

}

/*
 * 函数功能: 给出指定行的的数据
 * */
int PickElementsFromBFByRow(void * f , int row , int * dst)
{
	if(!f || !dst)
		return ERRO ;
	if(row<0 ||  row > COLUMN_LEN)
		return ERRO ;

	int i = 0 ; 
	for(i = 0 ; i < ROW_LEN ; i++ )
	{
		PickELementFromBF(f,row,i,&dst[i]);

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
int TestAccessPerformanceInBF()
{
	struct timeval tpstart , tpend ;
	memset(&tpstart,0,sizeof(struct timeval));
	memset(&tpend,0,sizeof(struct timeval));

	FILE * fp = NULL ;
	fp = fopen(FILE_NAME,"r");
	FILE * POUT = NULL ;
	POUT = fopen(ACCESS_TIME_CONSUME_FILE,"a+");

	int val = -1 ;
	int r[ROW_LEN]={0};
	int l[COLUMN_LEN]={0};
	int row[ACCESS_LIMITS] ={0};
	int col[ACCESS_LIMITS] ={0};
	long int usedtime = 0 ; 
	struct BF * pbf ;

	 pbf =CompreesBF(fp);
	 fprintf(POUT ,"-----------位图压缩存取性能测试（时间单位us）------------\n");
	/*开始存取时间统计*/

	//获取单个数据
	{
		srand((unsigned)time(NULL));
		row[0] = rand()%998;
		srand((unsigned)time(NULL));
		col[0] = rand()%26 ;
		
		gettimeofday(&tpstart,NULL);
		PickELementFromBF(pbf,row[0] , col[0] ,&val);
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
			PickELementFromBF(pbf,row[i] , col[i], &val);
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
		PickElementsFromBFByRow(pbf,row[0] , r);
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
			PickElementsFromBFByRow(pbf,row[i] , r);
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
		PickElementsFromBFByColumn(pbf,col[0],l);
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
			PickElementsFromBFByColumn(pbf,col[i],l);
		gettimeofday(&tpend,NULL);
		usedtime=0;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"多列存取时间:%ld\n",usedtime);

	}


	ReleaseBF(pbf);
	fclose(fp);
	fclose(POUT);

}
/*
 * 函数功能：导出压缩态文件到指定硬盘文件
 * */

int ShowBFCompressedFile()
{

	FILE *fp =  NULL ;
	fp = fopen(FILE_NAME,"r");
	struct BF *pbf ;
	pbf = CompreesBF(fp);
	ShowBF(pbf);
	ReleaseBF(pbf);
	fclose(fp);
}
