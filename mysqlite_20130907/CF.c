#include "CF.h"


/*
 * funtion : compressed the csv file with column compression method
 * args 1 fp: csv file pointer 
 * return : the mememory space for cf compressed  file
 * */


void * CF_Malloc(FILE *fp)
{
	if(!fp)
	{
		fprintf(stdout , "file-point fp is NULL\n");
		return (void *)NULL ;
	}
	struct CF * pf = (struct CF *)calloc(1,sizeof(struct CF)) ;
	UpdateUsedMem(&CF_Mem,sizeof(struct CF)); //update mem used
	struct S_Character tmp ;
	int size = 0 ;

	memset(&(pf->PrimaryKey),0,sizeof(pf->PrimaryKey));
	//malloc the mem for cf file
	{
		int i =0 ;
		struct Unit * pt = NULL;
		Byte * pb =NULL ;
		for( i = 0 ; i < ROW_LEN ; i++)
		{
			if(i==0) continue ;

			memset(&tmp,0,sizeof(struct S_Character));
			CountColumnAttribute(&tmp,fp,i);
			//malloc Unit array memory space for column
			//size = (int)(1.1 * tmp.segs ) ;
			size=tmp.segs;
			pt = (struct Unit *)calloc(size,sizeof(struct Unit));
			if(!pt)
			{
				fprintf(stdout,"%s",strerror(errno));
				return (void *)NULL ;
			}
			UpdateUsedMem(&CF_Mem,sizeof(struct Unit)*size);
			pf->cf[i]=pt ;
			pf->len[i]=tmp.segs;
			pf->limit[i]=size ;
			pt=NULL;
		}
	} 
	//end of malloc 
	return (void *)pf ;
}

/*
 *  funtion : free the memory that malloc for cf
 *  args 1: the cf file structure  or the cf file memory space
 * */

int ReleaseCF(void * in)
{
	if(!in)
		return ERRO;

	int i = 0 ;
	struct CF * pf = (struct CF *)in ;
	for( i = 1 ; i < ROW_LEN ; i++ )
	{
		if(pf->cf[i])
		{
			free(pf->cf[i]) ;
			pf->cf[i]=NULL ;
		}
	}
	free(pf) ;
	pf=NULL ;
	return SUCCESS ;
}
/*
 * funtion : test whether the CF_Malloc is right
 * */
void TestCFMem()
{
	struct CF * pf =NULL ;
	FILE *fp = fopen(FILE_NAME,"r");
	
	if(!fp)
	{
		fprintf(stdout,"%s\n",strerror(errno));
		return ;
	}

	pf = CF_Malloc(fp);
	//if(ERRO==pf)
	//{
	//	fprintf(stdout,"CF_Malloc erro\n");
	//	return ;
	//}

	//show the mem
	{
		int i ;
		for(i = 0 ; i  < ROW_LEN ; i++)
			fprintf(stdout,"len[%d]=%d\n",i,pf->len[i]);
	}
	ReleaseCF(pf);
	fclose(fp);
}

/*
 * funtion :  compressed the csv file
 * args fp : FILE pointer   
 * return  : the compressed file structure 
 * */

void * CompressCF(FILE * fp)
{
	if(!fp)
	{
		fprintf(stderr,"file-point is null\n");
		return (void *)NULL ;
	}
	struct CF * pf =NULL ;
	pf = CF_Malloc(fp);

	if(pf==NULL)
	{
		fprintf(stderr,"cf mem can't access\n");
		return (void *)NULL ;
	}

	char list[ROW_LEN][STRING_MAX]={'\0'};
	char line[LINE_MAX]={'\0'};
	int poslist[ROW_LEN]={0};
	int datalist[ROW_LEN] ={0} ;
	int prelist[ROW_LEN] ={0} ;
	int index=0,len=0,size=0;
	int i ;
	struct Unit * pt =NULL ;
	struct Byte * pb =NULL ;

	fseek(fp,0,SEEK_SET);
	index=0 ;
	
	while(fgets(line,LINE_MAX,fp)!=NULL)
	{
		//in case of a null line
		if(line[0]=='\0')
			continue ;
		//special file format 
		//if(index++<2)
		//	continue ;
		index++ ;
		memset(list,0,sizeof(list));
		memset(datalist,0,sizeof(datalist));

		if(ROW_LEN != cvs_parse(line,list,ROW_LEN))
		{
			fprintf(stderr,"cvs_parse erro\n");
			return (void *)NULL ;
		}
		
		//csv line parse into int 
		for( i = 0 ; i < ROW_LEN  ; i++ )
		{
			if(0==sscanf(list[i],"%d",&datalist[i]))
			{
				fprintf(stderr,"%s\n",strerror(errno));
				return (void *)NULL ;
			}
		}
		//end of csv line parse

		//Init 
		//firt line of csv data
		if(0==(index-1))
		{
			for( i = 0 ; i < ROW_LEN ; i++)
			{
				if(i==0)
				{
					int a = 0 , b = 0 ,c =0 ;
					a = datalist[i]-1;
					b=(a/8);
					c=a%8 ;
					pf->PrimaryKey[b] |= (0x01<<c);
					continue ;
				}

				poslist[i]=0;
				prelist[i]=datalist[i];
				pf->cf[i][poslist[i]].val=datalist[i];
				pf->cf[i][poslist[i]].pos=datalist[0]-1 ; // primary key counts form one

				//add by lanhxg
				if(i==0 || i==1)
				{
					continue ;
				}
				//pf->cf[i-1][poslist[i-1]].pnext = &(pf->cf[i][poslist[i]]);
				pf->cf[i-1][poslist[i-1]].pnext = poslist[i];
			}
			continue ;
		}
		//update
		for( i = 0 ; i < ROW_LEN ; i++ )
		{
			if(i==0)
			{
				int a =0 , b = 0 , c = 0 ;
				 //the first column is primary key and it counts form one
				a = datalist[0]-1;
				b = (a/8) ;
				c = a%8 ;
				pf->PrimaryKey[b] |= 0x01<<c ;
				continue ;
			}
			if(prelist[i]!=datalist[i])
			{
				poslist[i]++ ;
				prelist[i]=datalist[i];
				pf->cf[i][poslist[i]].val=datalist[i];
				pf->cf[i][poslist[i]].pos=datalist[0]-1 ;
				
				//add by lanhxg
				if(i!= (ROW_LEN-1) )
				{
					pf->cf[i][poslist[i]].pnext=poslist[i+1];
				}
				else
					pf->cf[i][poslist[i]].pnext=-1;

			}
		}
	}
	pf->key=datalist[0];
	return (void *)pf ;
}

/*
 * funtion : show compressed file
 * */

int ShowCFCompressedFile()
{
	struct CF * pf =NULL;
	FILE *fpout = fopen(CFCOMPRESSED_FILE,"w");//cf file
	FILE *fpin  = fopen(FILE_NAME,"r"); //csv file
	if(!fpout || !fpin)
	{
		fprintf(stdout,"%s\n",strerror(errno));
		return ERRO ;
	}

	pf = (struct CF *)CompressCF(fpin) ;
	if(pf==NULL)
	{
		fprintf(stdout,"cf mem access erro\n");
		return ERRO ;
	}

	int i =0 ;
	int j =0 ;
	int len =0 ;
	struct Unit * pl =NULL ;
	for( i = 0 ; i < ROW_LEN ; i++)
	{
		if(i==0)
		{
			int j = 0;
			int k = 0 ;
			for(j=0 ; j < 1000/8;j++)
			{
				for(k=0;k<8;k++)
				{
					if(pf->PrimaryKey[j]&(0x01<<k))
						fprintf(fpout,"1");
					else
						fprintf(fpout,"0");
				}
				
			}
			fprintf(fpout,"\n");
			continue ;
		}
		pl=(struct Unit *)pf->cf[i];
		len = pf->len[i] ;
		for(j = 0 ; j < len-1 ; j++)
			fprintf(fpout,"(%d,%d)",pl[j].val,pl[j+1].pos-pl[j].pos);
		fprintf(fpout,"(%d,%d)\n",pl[j].val,COLUMN_LEN-pl[j].pos);
		
	}
	fclose(fpout);
	fclose(fpin);
	ReleaseCF(pf);
}

/*
 * funtion :search a specific row val in CF file
 * arg list :the pionter of struct Unit 
 * arg l    :the most left val's position
 * arg r    :the most right val's position
 * arg row  :the specific row number 
 * arg s    :the array's len-1
 * */

int SearchPosInCF(void * list , int l , int r , int row ,int s)
{
	struct Unit * pu  = (struct Unit *)list;

	if(r-l<0)
		return ERRO ;
	int mid = (l+r)/2 ;

	if(s==mid)
	{
		if(pu[mid].pos<=row && row <COLUMN_LEN)
			return mid ;
	}
	else
	{
		if(pu[mid].pos<=row && row< pu[mid+1].pos)
			return mid ;
	}

	if(pu[mid].pos > row)
		return SearchPosInCF(list,l,mid-1,row,s);
	else 
		return SearchPosInCF(list,mid+1,r,row,s);

}

int PickElementFromCF(void * f ,int row , int col ,int * val)
{
	if(!f || !val)
		return ERRO ;
	if(row <0 || row > COLUMN_LEN)
		return ERRO ;
	if(col <0 || col > ROW_LEN )
		return ERRO ;
	int i = row ;
	int j = col ;
	struct CF *pf =(struct CF *)f ;
	struct Unit * pu = pf->cf[j];
	int s = pf->len[j]-1 ;
	int res =-1 ;
	res = SearchPosInCF(pu,0,s,i,s);
	*val= pu[res].val ;
	return SUCCESS ;
	
}
/*
 * funtion: pick a row form cf file by given row num
 * */

int PickElementsFromCFByROW(void * f ,int row ,int * dst)
{
	if(!f ||  !dst)
		return ERRO ;
	if(row <0 || row >COLUMN_LEN)
		return ERRO ;
	int i = 0 ; 
	for ( i = 0 ; i < ROW_LEN ; i++ )
		PickElementFromCF(f,row,i,&dst[i]);
	return SUCCESS ;
}
int PickElementsFromCFByROW_v2(void * f ,int row ,int * dst)
{
	if(!f || !dst) return ERRO ;
	if(row<0 || row > COLUMN_LEN ) return ERRO ;

	struct CF * pf = (struct CF *)f ;
	if(pf==NULL)
	{
		fprintf(stderr,"can't access CF file");
		return ERRO ;
	}

	//struct Unit * pstart =NULL;
	//struct Unit * pend   =NULL;
	int pstart = -1 ;
	int pend   = -1 ;
	struct Unit * pu     =NULL;
	int l =0 , r = -1 ,s =-1;
	int res=-1 ;
	int i = 0 ; 

	{
		int a,b,c ;
		a = row ;
		b=a/8;
		c=a%8;
		if(!(pf->PrimaryKey[b] & (0x1<<c)))
		{
			//the primary key doesn't exits
			return ERRO ;
		}
		dst[0]=row+1 ;

	}

	for( i = 0 ; i  < ROW_LEN ; i++)
	{
		if(i==0)	continue ;
		
		pu = pf->cf[i] ;
		if(i==1)
	       	{
			l=0 ;
			r=pf->len[i]-1 ;
			s=r;
		} 
		else 
		{
			l=pstart;
			s=pf->len[i]-1;
			r= (pend == -1) ? s : pend ;
		}
		res = SearchPosInCF(pu,l,r,row,s);
		dst[i]= pu[res].val ;

		pstart = pu[res].pnext ;
		if(res==s)
		{
			pend =-1 ;
		}
		else
		{
			pend   = pu[res+1].pnext ;
		}
	}
}
int ReadLineFromCF()
{
	FILE * fp = NULL ;
	fp = fopen(FILE_NAME,"r");
	struct CF *pf = NULL ;
	if(!fp)
	{
		fprintf(stdout,"%s\n",strerror(errno));
		return ERRO ;
	}
	pf=(struct CF *) CompressCF(fp);
	if(pf==NULL)
	{
		fprintf(stdout,"cf mem access erro\n");
		return ERRO ;
	}
	
	int dst[ROW_LEN]={0};
	int i = 0 ;
	int j = 0 ;
	for(i=0;i<1000;i++)
	{
		PickElementsFromCFByROW_v2(pf ,i ,dst);
		for(j=0 ; j < ROW_LEN ; j++)
			j != (ROW_LEN-1) ? printf("%d,",dst[j]): printf("%d\n",dst[j]);
	}
	fclose(fp);
	ReleaseCF(pf);

}
int TestReadLineFromCF(void * in)
{
	FILE * fp = NULL ;
	fp = fopen(FILE_NAME,"r");
	if(!fp)
	{
		fprintf(stdout,"%s\n",strerror(errno));
		return ERRO ;
	}
	if(in==NULL)
	{
		fprintf(stdout,"cf mem access erro\n");
		return ERRO ;
	}
	struct CF *pf=(struct CF *)in ;
	
	int dst[ROW_LEN]={0};
	int i = 0 ;
	int j = 0 ;
	for(i=0;i<1000;i++)
	{
		PickElementsFromCFByROW_v2(pf ,i ,dst);
		for(j=0 ; j < ROW_LEN ; j++)
			j != (ROW_LEN-1) ? printf("%d,",dst[j]): printf("%d\n",dst[j]);
	}
	fclose(fp);
	return SUCCESS;
}
int TestAccessPerformanceINCF_v2()
{
	struct timeval tpstart ,tpend ;
	memset(&tpstart,0,sizeof(struct timeval));
	memset(&tpend  ,0,sizeof(struct timeval));
	
	FILE * fp = NULL ;
	FILE * POUT = NULL ;
	fp = fopen(FILE_NAME,"r");
	POUT = fopen(ACCESS_TIME_CONSUME_FILE,"a+");

	if(!fp || !POUT)
	{
		fprintf(stdout,"%s\n",strerror(errno));
		return ERRO ;
	}

	struct CF *pf = NULL ;
	pf=(struct CF *) CompressCF(fp);
	if(pf==NULL)
	{
		fprintf(stdout,"cf mem access erro\n");
		return ERRO ;
	}

	int row[ACCESS_LIMITS] ={0};
	long int usedtime = 0 ; 
	int dst[ROW_LEN]={0};

	fprintf(POUT,"-----------游程压缩存取性能测试（时间单位us）-----------\n");
	{
		int i =0 ;
		srand((unsigned)time(NULL));
		for(i = 0 ; i < ACCESS_LIMITS ; i++)
		{
			row[i] = rand()%997;
			row[i]+=3;
		}
		gettimeofday(&tpstart,NULL);
		for(i = 0 ; i < ACCESS_LIMITS ; i++)
		{
			PickElementsFromCFByROW_v2(pf ,row[i] ,dst);
		}
		gettimeofday(&tpend,NULL);
		usedtime=0;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"多行存取时间:%ld\n",usedtime);

	}
	fclose(fp);
	fclose(POUT);
	ReleaseCF(pf);
}
int TestAccessPerformanceInCF()
{
	
	struct timeval tpstart ,tpend ;
	memset(&tpstart,0,sizeof(struct timeval));
	memset(&tpend  ,0,sizeof(struct timeval));
	
	FILE * fp = NULL ;
	FILE * POUT = NULL ;
	fp = fopen(FILE_NAME,"r");
	POUT = fopen(ACCESS_TIME_CONSUME_FILE,"a+");

	if(!fp || !POUT)
	{
		fprintf(stdout,"%s\n",strerror(errno));
		return ERRO ;
	}

	struct CF *pf = NULL ;
	pf=(struct CF *) CompressCF(fp);
	if(pf==NULL)
	{
		fprintf(stdout,"cf mem access erro\n");
		return ERRO ;
	}

	int row[ACCESS_LIMITS] ={0};
	long int usedtime = 0 ; 
	int dst[ROW_LEN]={0};

	fprintf(POUT,"-----------游程压缩存取性能测试（时间单位us）-----------\n");
	{
		int i =0 ;
		srand((unsigned)time(NULL));
		for(i = 0 ; i < ACCESS_LIMITS ; i++)
		{
			row[i] = rand()%997;
			row[i]+=3;
		}
		gettimeofday(&tpstart,NULL);
		for(i = 0 ; i < ACCESS_LIMITS ; i++)
		{
			PickElementsFromCFByROW(pf ,row[i] ,dst);
		}
		gettimeofday(&tpend,NULL);
		usedtime=0;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"多行存取时间:%ld\n",usedtime);

	}
	fclose(fp);
	fclose(POUT);
	ReleaseCF(pf);
}
int ShowCFFileMemoryUsed()
{
	FILE *fp = fopen(FILE_NAME,"r");

	InitMem(&CF_Mem);

	struct CF *pf = NULL ;
	pf=(struct CF *) CompressCF(fp);

	if(pf==NULL)
	{
		fprintf(stdout,"cf mem access erro\n");
		return ERRO ;
	}
	ShowUsedMem(&CF_Mem,COLUMN_COMPRESSION);

	ReleaseCF(pf);
	fclose(fp);
}
/*
 * funtion: delete a whole row by primary key
 * arguments 1 row: the row number that you want to delete ,and the row count form zero 
 * return  : if the funtion execute successfully it will return SUCESS ,or it return ERRO
 * */

int DeleteRowInCF(void * f ,int row)
{
	if(row < 0  || row > COLUMN_LEN )
		return ERRO ;
	if(!f)
		return ERRO ;

	struct CF * pf = (struct CF *)f;

	int a =0 , b =0 ,c = 0 ;
	a = row ;
	b = row/8 ;
	c = row%8 ;

	if(pf->PrimaryKey[b] & (0x01<<c) )
	{
		//the primary key exits 
		pf->PrimaryKey[b] &= ~(0x01<<c);
		return SUCCESS ;
	}
	return ERRO ;
}
/*
 * funtion : wipe all the seg records in cf file ,after execute this funtion ,cf file is empty
 * argument-f : cf file structure
 * return value : if success it will return macro SUCCESS ,or it returns ERRO
 * */
int WipeAll(void * f)
{
	if(!f) return ERRO ;

	struct CF *pf = (struct CF *)f ;
	//wipe all the primary key records 
	memset(pf->PrimaryKey,0,sizeof(pf->PrimaryKey));
	//wipe all the cf seg records 
	int i = 0 ;
	for( i = 0 ; i < ROW_LEN ; i++)
	{
		if(i==0) continue ;
		memset(pf->cf[i],0,sizeof(pf->cf[i]));
	}
	memset(pf->limit,0,sizeof(pf->limit));
	memset(pf->len,0,sizeof(pf->len));
	//wipe the top primary key record
	pf->key = 0 ;

	return SUCCESS ;
}

/*
 * funtion : add a row to cf file
 * argument- f : CF struct pointer
 * argument-dst: the elements that needs to insert into CF file
 * return      : if success it return 0 ,or -1
 * */

int AddRowIntoCF( void * f , int *dst)
{
	if(!f || ! dst ) return ERRO ;

	struct CF * pf = (struct CF *)f ;

	int a = 0 , b = 0 ,c = 0;
	a = dst[0]-1;
	b = a/8;
	c=a%8;
	//check if primary key exits or not
	if(pf->PrimaryKey[b] & (0x1<<c))
	{
		fprintf(stderr,"primary key exits\n");
		return ERRO;
	}	
	pf->PrimaryKey[b] |=(0x01<<c);
	
	int i = 0 ;	
	int top = 0 ;
	int pre = 0 , pcu = 0 ;
	for( i = 0 ; i < ROW_LEN ; i++ )
	{
		if(i==0) continue ;

		top=pf->len[i]-1;
		//check if cf is empty or not
		if(top == -1 )
		{
			 pf->len[i]++; 
			 pf->cf[i][top+1].val=dst[i];
			 pf->cf[i][top+1].pos=dst[0]-1;
			 if(i!=(ROW_LEN-1))
				 pf->cf[i][top+1].pnext=0 ;
			 else
				 pf->cf[i][top+1].pnext=-1;
			 

		}
		else
		{
			if(dst[i] != pf->cf[i][top].val)
			{
				pf->len[i]++;
				pf->cf[i][top+1].val=dst[i];
				pf->cf[i][top+1].pos=dst[0]-1;
				
				if(i != ROW_LEN-1)
				{
					pf->cf[i][top+1].pnext=pf->len[i+1]-1;
				}
				else
				{
					pf->cf[i][top+1].pnext=-1;
				}
			}
		}
	}
	int tmp1 = pf->key +1 ;
	int tmp2 = dst[0] - 1;
	pf->key = tmp1 > tmp2 ? tmp1 :tmp2 ;
	return SUCCESS ;
}

/*
 * funtion : appening rows into cf file time consuming
 * 
 * */

int TestAppendRowsIntoCF()
{
	/*
	 * 1. cf file should auto create a primary key for a new line
	 * 2.the insert columns = ROW_LEN-1
	 * */

	int src[ROW_LEN]={0};
	int i = 0 ; 
	srand((unsigned int)time(NULL));
	for( i = 0 ; i < ROW_LEN ; i++ )
	{
		src[i]=rand()%10000;
	}

	FILE *fp = fopen(FILE_NAME,"r");
	
	if(fp==NULL)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return ERRO ;
	}

	struct CF * pf = (struct CF *)CompressCF(fp) ; 
	if(pf==NULL)
	{
		fprintf(stderr,"can not access cf mem\n");
		return ERRO;
	}
	AddRowIntoCF(pf,src);
	fclose(fp);
	ReleaseCF(pf);
}
/*
 * FUNTION: wipe all records and write data into cf file
 * */
int TestAppendRowsIntoCF_v2()
{
	/*
	 * 1.read csv file into memory
	 * 2.wipe all the records in cf file structure
	 * */

	int **pm = (int **) ReadCSVFileIntoMem(COLUMN_LEN,ROW_LEN);
	if(pm==NULL)
	{
		fprintf(stderr,"can't access mem csv file\n");
		return ERRO ;
	}

	FILE  * fp = fopen(FILE_NAME,"r");
	if(fp==NULL)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return ERRO ;
	}
	struct CF * pf = (struct CF*) CompressCF(fp);
	if(pf==NULL)
	{
		fprintf(stderr,"cf mem access erro\n");
		return ERRO ;
	}
	//wipe all the records
	int src[ROW_LEN]={0};
	int i = 0  , j = 0 , k=0 , m =0 ;
	struct timeval tpstart,tpend ;
	long int usedtime=0;
	FILE * POUT = fopen(CFWRITING_TIMEUSED_FILE,"w");

	for(m=0;m<10;m++)
	{

		memset(&tpstart,0,sizeof(struct timeval));
		memset(&tpend  ,0,sizeof(struct timeval));

		gettimeofday(&tpstart,NULL);
		for( k = 0 ; k < 100 ; k++)
		{
			WipeAll(pf);
			for(i = 0 ; i < COLUMN_LEN ; i++ )
			{
				//copy mem csv data to src array
				for( j = 0 ; j < ROW_LEN ; j++)
				{
					src[j]=pm[i][j]; 
				}//end of copy
				/*
				for(j = 0 ; j < ROW_LEN ; j++)
				{
					if(j== ROW_LEN-1)
					{
						fprintf(stdout,"%d\n",src[j]);
						continue ;
					}
					fprintf(stdout,"%d,",src[j]);
				}
				*/
			       //append a row to cf file
			       AddRowIntoCF( pf , src);
			}
		}
		gettimeofday(&tpend,NULL); 
		usedtime=0;
		usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
		fprintf(POUT,"------------------writing test time consume(us)------------------\n");
		fprintf(POUT,"%ld\n",usedtime);
	}

	//TestReadLineFromCF(pf);

	if(pm) FreeCSVMem(pm , COLUMN_LEN );
	if(pf) ReleaseCF(pf);
	if(fp) fclose(fp);
	if(POUT) fclose(POUT);
}

/*
 * funtion : testing the CF compressing time used
 * */

int TestCFCompressedTimeConsumed()
{
	long int usedtime=0;
	//open compressing time consuming test file
	FILE * fpout = fopen(CFCOMPRESSING_TIMEUSED_FILE,"w");
	if(fpout==NULL)
	{
		fprintf(stdout,"%s\n",strerror(errno));
		return ERRO ;
	}

	FILE * fpin =  fopen(FILE_NAME ,"r");
	if(fpin==NULL)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return ERRO ;
	}


	struct timeval tpstart, tpend ;
	memset(&tpstart,0,sizeof(struct timeval));
	memset(&tpend,0,sizeof(struct timeval));

	gettimeofday(&tpstart,NULL);
	struct CF * pf = (struct CF *)CompressCF(fpin) ; 
	gettimeofday(&tpend,NULL);

	if(pf==NULL)
	{
		fprintf(stderr,"cf mem access erro\n");
		return ERRO ;
	}

	usedtime=0;
	usedtime=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
	fprintf(fpout,"-----------compressed time consuming(us)-----------\n");
	fprintf(fpout,"%ld\n",usedtime);
	fclose(fpout);
	fclose(fpin);
	ReleaseCF(pf);
}
