#include"csv.h"


/*
 * 处理cvs文件
 * 1、所有值都是以字符串的形式进行返回，
 * 2、若是两个逗号之间存在数值，则返回字符串形式的数值
 * 3、若两个逗号之间不存在数值，则返回空字符串
 * */
int cvs_parse(char * line ,  char list[][STRING_MAX] , int num)
{

	if(!line  || ! list)
		return -1 ;

	char curc  = ',' ;
	char prevc = ',';
	int i = 0 ;
	int j = 0 ;
	int index =num ;
	char * p  = line;


	for(i = 0  ; *p!='\0'&&*p!='\r'; prevc = *p,p++ )
	{
		/*
		 * 0. prevc = ',' 1 ） 刚开始  2）处于字符串中间的值
		 * 1. prevc = curc = ',' set list[i]=""
		 * 2. prevc != curc  we should copy *p to list[i]
		 * */

		if(prevc==',')
		{
		     if(*p ==',')	
		     {
			     list[i++][0]='\0';
			    
		     }
		     else
		     {
			     list[i][j++]=*p;

		     }

		}
		else
		{
			if(*p == ',')
			{
				list[i++][j]='\0';
				j=0;
			}
			else
			{
				list[i][j++]=*p;
			}
		} 

	}
	if(prevc==',')
	{
		list[i++][0]='\0';
	}
	else
	{
		list[i++][j]='\0';
	}
	return  i ;
}






/*
 *  函数功能： 添加元素到hash set，同时完成统计功能
 *  参数 1  ： 需要添加的值
 *  返回值  ： 程序的执行状态   
 *
 * */


//添加元素
int AddVal(int new )
{
	int v ;
	v = new%100 ;
	/*
	 * 1.遍历 set[v] 看看存在new值没
	 *   1) 存在 在对应的stuct->cnt ++
	 *   2) 不存在 新建一个结构 然后添加到对应的尾部
	 *
	 * */
	if(v<0)
		return ERRO ;

	struct Val * p ;
	p = set[v] ;
	while(p)
	{
		if(p->val==new)
		{
			if(pre!=new)
			{
				p->times++ ;
				pre=new ;
			}
			p->repeat_cnt++ ;
			return SUCCESS_EXIT;
		}
		p=p->pnext;

	}

	//执行到这里 说明这里不存在这个值应该添加这个数据的结构题
	struct Val *pn = (struct Val *)malloc(sizeof(struct Val));
	pn->val = new ;
	pn->repeat_cnt=1 ;
	pn->times=1 ;
	pn->pnext=NULL ;
	pre=new ;

	if(set[v]==NULL)
	{
		set[v]=pn ;
		return SUCCESS ;
	}
	else
	{
		p = set[v];
		while(p->pnext!=NULL) p =p->pnext ;
		p->pnext =pn ;
		return SUCCESS ;
	}
	
	return ERRO ; //错误值
}
//查询元素
int getRepeatCnt(int s)
{

	int v = s%100 ;
	struct Val* p = set[v] ;
	while(p!=NULL)
	{
		if(p->val==s)
			return p->repeat_cnt ;
	}
	return -1 ;
}
//释放空间
void releaseHset()
{
	int i = 0 ;
	struct Val * pre = NULL;
	struct Val * p   = NULL;
	for(i = 0 ; i <  100 ; i++ )
	{
		p = set[i];
		while(p!=NULL)
		{
		
			pre = p ;
			p=p->pnext ;
			free (pre) ;
		}
		set[i]=NULL;

	}

}




//列属性统计
/*
 * tip :由于使用hash表来进行数据分析,能够支撑列长可达数万长度
 *
 * 函数功能： 统计一列的数据属性 ：
 * 			1、有多少个数据 
 * 			2、每个数据重复次数 
 * 			3、重复次数在元素的个数 
 * 			4、每个元素重复的平均长度 
 * 			5、对大元素的存储长度（字符单位）
 * 参数1：返回的统计结果，用结构体表示
 * 参数2：cvs文件指针
 * 参数3: 指定统计的列 从0开始计数
 *
 * */
int CountColumnAttribute(struct S_Character * att , FILE *fp ,int which)
{
	if(!fp || !att  )	
		return ERRO ;

	if(which<0 || which > COLUMN_LEN)
		return ERRO ;

	char list[COLUMN_LEN][STRING_MAX]={'\0'};
	char line[LINE_MAX]={0};


	memset(att,0,sizeof(struct S_Character));

	int data =-1 ,index=0,len=0,size=0;

	fseek(fp,0,SEEK_SET);	
	index=0 ;

	while(fgets(line,LINE_MAX,fp)!=NULL)
	{
		if(line[0]=='\0') //怕空行导致的错误
			continue ;

		//前面两行描述的行列属性 这里略过
		//if(++index>2)
		index++;
		{
			if(ROW_LEN!=cvs_parse(line,list,ROW_LEN))
			{
				fprintf(OUTPUT,"cvs_parse erro\n");
				return  ERRO;
			}
		}    

		if(list[which][0] =='\0')
			continue ;

		att->cnt++ ;
		size = size < strlen(list[which]) ? strlen(list[which]) : size; 
		if(0==sscanf(list[which],"%d",&data))
			continue ;
		AddVal(data);
	}

	att->max_size = size ;

	/*
	 * 把统计在hash表中的结果，存储在结构体中
	 * 统计结果放在set这个全局变量中
	 *
	 * */

	int i=0,j=0 ;
	struct Val * p ;
	for(i = 0 ; i < COLUMN_LEN ; )
	{
		p = set[j++] ;

		while(p!=NULL)
		{
			att->x.c_a[i][VAL]=p->val ;
			att->x.c_a[i][REPEAT]=p->repeat_cnt ;
			att->x.c_a[i][AVERAGE_LEN] = p->repeat_cnt/p->times ;
			att->segs+=p->times ;

			if(p->repeat_cnt>0)
			{
				att->val_cnt++;

				if(p->repeat_cnt>1)
					att->repeat_cnt++;
			}
			p=p->pnext ;
			i++;
		}

		//hash table len is 100
		if(j>99) 
			break ;

	}
	releaseHset();
	return SUCCESS ;

}


//行属性统计
/*
 * tip :由于使用hash表来进行数据分析,能够支撑列长可达数万长度
 *
 * 函数功能： 统计一行的数据属性 ：
 * 			1、有多少个数据 
 * 			2、每个数据重复次数 
 * 			3、重复次数在元素的个数 
 * 			4、每个元素重复的平均长度 
 * 			5、对大元素的存储长度（字符单位）
 * 参数1：返回的统计结果，用结构体表示
 * 参数2：cvs文件指针
 * 参数3: 指定统计的列 从0开始计数
 *
 * */
int CountRowAttribute(struct S_Character * att , FILE *fp ,int which)
{

	if(!att || !fp)
		return ERRO ;
	if(which<0 || which > COLUMN_LEN)
		return ERRO ;

	int i = 0 ;
	char list[ROW_LEN][STRING_MAX]={'\0'};
	char line[LINE_MAX]={0};
	int size =0;
	int data=-1 ;

	fseek(fp,0,SEEK_SET);
	while(fgets(line,LINE_MAX,fp) != NULL)
	{
		if(i++ != which)
			continue ;

		if(line[0]=='\0')
			continue ;

		//当前行满足要求

		if(ROW_LEN!=cvs_parse(line,list,ROW_LEN))
		{
			fprintf(OUTPUT,"cvs_parse erro\n");
		        return  ERRO;
		}

		int j ;
		for( j = 0 ; j < ROW_LEN ; j++  )
		{
			if(list[j][0]=='\0')
				continue ;
			 
			att->cnt++ ;
			size = size < strlen(list[j]) ? strlen(list[j]) : size ;

			if(0==sscanf(list[j],"%d",&data))
				continue ;

			AddVal(data);
		}

		att->max_size = size ;

		int k =0 ;
		j=0; 
		struct Val * p ;
		for(k = 0 ; k < ROW_LEN  ; )
		{
			p = set[j++] ;

			while(p!=NULL)
			{
				att->x.c_a[k][VAL]=p->val ;
				att->x.c_a[k][REPEAT]=p->repeat_cnt ;
				att->x.c_a[k][AVERAGE_LEN] = p->repeat_cnt/p->times ;

				if(p->repeat_cnt>0)
				{
					att->val_cnt++;

					if(p->repeat_cnt>1)
						att->repeat_cnt++;
				}
				p=p->pnext ;
				k++;
			}

			//hash table len is 100
			if(j>99) 
				break ;
		}	
		break ;
	}
	releaseHset();
	return SUCCESS ; 

}

void Init()
{
	pre=0 ;
	pos=0;
	fprintf(stdout,"Init pre=%d ,pos =%d\n",pre,pos);
}

/*函数功能:统计整个文件的列属性*/
int ShowFileColumnsAttribute() 
{

	FILE *fp =  NULL ;
	FILE *FOUT = NULL ;

	fp = fopen(FILE_NAME,"r");
	FOUT = fopen(EACH_COLUMN_ATTRIBUTE_FILE,"w+");

	struct S_Character COL_Charcter ;

	int j = 0 ;
	int i = 0 ; 
	for(j = 0 ; j < ROW_LEN ; j++ )
	{
		fprintf(FOUT,"--------------<%d>-------------- \n",j);
		memset(&COL_Charcter,0,sizeof(COL_Charcter));
		CountColumnAttribute(&COL_Charcter,fp,j);
		fprintf(FOUT,"all：%d \n",COL_Charcter.cnt);
		fprintf(FOUT,"sample：%d \n",COL_Charcter.val_cnt);
		fprintf(FOUT,"repeat：%d \n",COL_Charcter.repeat_cnt);
		fprintf(FOUT,"width：%d \n",COL_Charcter.max_size);
		fprintf(FOUT,"segs：%d \n",COL_Charcter.segs);
/*
		for(i=0 ; i< COL_Charcter.val_cnt ; i++)
		{
			fprintf(FOUT,"[%d][VAL]=%d ,",i,COL_Charcter.x.c_a[i][0]);
			fprintf(FOUT,"[%d][REPEAT]=%d ,",i,COL_Charcter.x.c_a[i][1]);
			fprintf(FOUT,"[%d][AVERAGE_LEN]=%d \n",i,COL_Charcter.x.c_a[i][2]);

		}
	*/
	}
}

int PickElementFromOr(FILE* fp ,int row , int col ,int * val)
{
	if(!fp||!val)
		return ERRO ;
	if(row<0 || row > COLUMN_LEN)
		return ERRO ;
	if(col<0||col>ROW_LEN)
		return ERRO ;

	int i = 0 ;
	char list[ROW_LEN][STRING_MAX]={'\0'};
	char line[LINE_MAX]={0};

	fseek(fp,0,SEEK_SET);
	while(fgets(line,LINE_MAX,fp) != NULL)
	{
		if(i++ != row)
			continue ;

		if(line[0]=='\0')
		{
			return ERRO ;
		}

		//当前行满足要求

		if(ROW_LEN!=cvs_parse(line,list,ROW_LEN))
		{
			fprintf(OUTPUT,"cvs_parse erro\n");
		        return  ERRO;
		}


		if(0==sscanf(list[col],"%d",val))
				continue ;

	}
	return SUCCESS ; 


}

int PickElementsFromOrByColumn(FILE * fp , int col , int * dst)
{
	if(!fp || !dst)
		return ERRO ;
	if(col < 0 || col > ROW_LEN )
		return ERRO ;

	int i = 0 ;
	char list[ROW_LEN][STRING_MAX]={'\0'};
	char line[LINE_MAX]={0};

	fseek(fp,0,SEEK_SET);
	while(fgets(line,LINE_MAX,fp) != NULL)
	{

		if(line[0]=='\0')
			continue ;

		//当前行满足要求

		if(ROW_LEN!=cvs_parse(line,list,ROW_LEN))
		{
			fprintf(OUTPUT,"cvs_parse erro\n");
		        return  ERRO;
		}


		if(0==sscanf(list[col],"%d",&dst[i++]))
				continue ;

	}

}

int PickElementsFromOrByRow(FILE *fp , int row , int *dst)
{
	if(!fp || !dst)
		return ERRO ;
	if(row < 0 || row > COLUMN_LEN )
		return ERRO ;
	int i = 0 ;
	int j = 0 ; 
	char list[ROW_LEN][STRING_MAX]={'\0'};
	char line[LINE_MAX]={0};

	fseek(fp,0,SEEK_SET);
	while(fgets(line,LINE_MAX,fp) != NULL)
	{
		if(i++!=row)
			continue ;

		if(line[0]=='\0')
			continue ;

		//当前行满足要求

		if(ROW_LEN!=cvs_parse(line,list,ROW_LEN))
		{
			fprintf(OUTPUT,"cvs_parse erro\n");
		        return  ERRO;
		}
		for(j = 0 ; j < ROW_LEN ; j++)
		{

			if(0==sscanf(list[j],"%d",&dst[j]))
				continue ;

		}

	}

}
/*
 * 函数功能能：原始文件单元素的存取时间
 * */ 
int TestAccessPerformanceInOr()
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

	//PickElementFromOr(fp ,0 , 0 ,&val);
	//PickElementsFromOrByColumn(fp , 0, l);
	//PickElementsFromOrByRow(fp , 0 , r);
	 fprintf(POUT ,"-----------原始文件存取性能测试(时间单位us)------------\n");
	/*开始存取时间统计*/

	//获取单个数据
	{
		srand((unsigned)time(NULL));
		row[0] = rand()%998;
		srand((unsigned)time(NULL));
		col[0] = rand()%26 ;
		
		gettimeofday(&tpstart,NULL);
		PickElementFromOr(fp,row[0] , col[0] ,&val);
		gettimeofday(&tpend,NULL);
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"单元素存取时间：%ld\n",usedtime);
	}


	//获取十个多数据的存取时间 
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
			PickElementFromOr(fp,row[i] , col[i] ,&val);
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
		PickElementsFromOrByRow(fp , row[0] , r);
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
			PickElementsFromOrByRow(fp , row[i] , r);
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
		PickElementsFromOrByColumn(fp , col[0], l);
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
			PickElementsFromOrByColumn(fp , col[i], l);
		gettimeofday(&tpend,NULL);
		usedtime=0;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"多列存取时间:%ld\n",usedtime);

	}
	fclose(POUT);


}

int PickElementFromMem(int row , int col ,int * val)
{
	int a[1000];
	*val=a[row];
}
int PickElementsFromMemByColumn( int col , int * dst)
{
	int a[1000];
	int i = 0 ; 
	for(i = 0 ; i < 1000 ; i++)
	{
		dst[i] = a[i] ;
	}
}
int PickElementsFromMemByRow(int row , int *dst)
{
	int a[26];
	int i = 0 ; 
	for(i = 0 ; i < 26 ; i++)
		dst[i]=a[i] ;
}
/*
 * 函数功能：内存存取时间
 * */
int TestAccessPerformanceInMem()
{
	int arry[1000];
	struct timeval tpstart , tpend ;
	memset(&tpstart,0,sizeof(struct timeval));
	memset(&tpend,0,sizeof(struct timeval));
	FILE *POUT=NULL ;

	POUT = fopen(ACCESS_TIME_CONSUME_FILE,"a+");


	int val = -1 ;
	int r[ROW_LEN]={0};
	int l[COLUMN_LEN]={0};
	int row[ACCESS_LIMITS] ={0};
	int col[ACCESS_LIMITS] ={0};
	long int usedtime = 0 ; 

	 fprintf(POUT ,"-----------内存文件存取性能测试(时间单位us)------------\n");
	/*开始存取时间统计*/

	//获取单个数据
	{
		srand((unsigned)time(NULL));
		row[0] = rand()%998;
		srand((unsigned)time(NULL));
		col[0] = rand()%26 ;
		
		gettimeofday(&tpstart,NULL);
		PickElementFromMem(row[0] ,col[0],&val);
		gettimeofday(&tpend,NULL);
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"单元素存取时间：%ld\n",usedtime);
	}


	//获取十个多数据的存取时间 
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
			PickElementFromMem(row[i] , col[i] ,&val);
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
		PickElementsFromMemByRow(row[0] , r);
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
			PickElementsFromMemByRow(row[i] , r);
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
		PickElementsFromMemByColumn(col[0], l);
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
			PickElementsFromMemByColumn( col[i], l);
		gettimeofday(&tpend,NULL);
		usedtime=0;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"多列存取时间:%ld\n",usedtime);

	}
	fclose(POUT);

}

/*
 * funtion : read csv file into Mem
 * argument-row : the file has row number rows
 * argument-col : the file has col number columns
 * return-value : if funtion executes success ,it would return the Memory address that contain csv file datas ,or returns NULL 
 * */
void * ReadCSVFileIntoMem(int row ,int col)
{
	FILE *fp = fopen(FILE_NAME,"r");
	if(fp==NULL)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return (void *)NULL ;
	}
	    //int *total_data[row];
	    int ** total_data ;
	    total_data = (int**)calloc(row , sizeof(int*));
	    int i=0,
		j=0;
	    char line[LINE_MAX]={'\0'};
	    char list[col][STRING_MAX];
	    memset(list,0,sizeof(list));


	   while(fgets(line,LINE_MAX,fp)!=NULL)
	   {
	       if(line[0]=='\0')
		       return NULL;
	       if(j==row)
		       break ;

	       total_data[j]=(int*)malloc(col*sizeof(int));
	       memset(list,0,sizeof(list));
	       memset(total_data[j],0,col*sizeof(int));

	       if(col != cvs_parse(line,list,col))
	       {
		   fprintf(stderr,"cvs_parse erro\n");
		   return (int *)NULL ;
	       }

	       for( i = 0 ; i < col  ; i++ )
	       {
		   if(0==sscanf(list[i],"%d",&total_data[j][i]))
		   {
		       fprintf(stdout,"%s\n",strerror(errno));
		       return (void *)NULL ;
		   }
	       }
	       ++j;
	   }
	   fclose(fp);
	   return (void *)total_data;
}
/*
 * FUTION: release the memory for store csv origin file data
 * argument-in  :ponter of memory of csv file data
 * argument-row : the length of the pointer array 
 * */
int FreeCSVMem(void * in , int row )
{
	int ** pt = (int **)in ;
	int  i=0 ,j=0 ;
	for(i = 0 ; i < row ; i++)
	{
		free(pt[i]);
	}
	free(pt);
	return SUCCESS ;
}

/*
 * funtion : testing read csv file into memory time consume
 * */

int TestReadCSVFilleTimeConsume()
{

	struct timeval tpstart ,tpend ;
	memset(&tpstart,0,sizeof(struct timeval));
	memset(&tpend  ,0,sizeof(struct timeval));

	int i = 0 ;
        int j = 0 ;	
	long int usedtime = 0 ;
	gettimeofday(&tpstart,NULL);
	int **pa = (int **)ReadCSVFileIntoMem(1000,27);	
	gettimeofday(&tpend,NULL);
	usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;

	printf(" read csv file time consume:%ld  %ld\n",usedtime,27*usedtime);
	FreeCSVMem(pa,1000);
	return SUCCESS ;
}
int ShowMemCSVFile()
{
	int ** p = ReadCSVFileIntoMem(COLUMN_LEN,ROW_LEN);
	int i =0 ;
	int j =0 ;
	for( i = 0 ; i < COLUMN_LEN ; i++ )
	{
		for( j = 0 ; j < ROW_LEN ; j++)
		{
			if(j==ROW_LEN-1)
			{
				fprintf(stdout,"%d\n",p[i][j]);
				continue;
			}
			fprintf(stdout,"%d,",p[i][j]);
		}

	}
	FreeCSVMem(p , COLUMN_LEN );
	return SUCCESS;
}
