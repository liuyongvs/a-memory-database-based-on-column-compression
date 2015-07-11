#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<time.h>
#define FILE_NAME "huawei_2.data"
//#define FILE_IN "in_1.data"
#define FILE_IN "in_2.data"
//#define COLUMN 24
//#define ROW  13072
#define COLUMN 43 
#define ROW 17665 
void CreateColumnData(void * in , int row ,int col , int sam , int seg ,int witch);
int main()
{
	int col = COLUMN, row = ROW;

	//calloc memory space for data
	int **p = (int **)calloc(row,sizeof(int *));

	{
		int i = 0 ;
		for( i = 0 ; i < row ; i ++ )
		{
			p[i] = (int *)calloc(col,sizeof(int));
		}
	}
	int s[COLUMN][2]={0};
	{
		FILE * fp = fopen(FILE_IN,"r");
		int i = 0;
		for( i = 0 ; i < COLUMN ; i++ )
		{
			fscanf(fp,"%d %d\n",&s[i][0],&s[i][1]);
			fprintf(stdout,"%d %d\n",s[i][0],s[i][1]);
		}
		fclose(fp);
	}
	//create file
	{
		FILE * fp = fopen(FILE_NAME,"w");
		
		int i=0 , j;
		for(i = 0 ; i < col ; i++ )
		{
			CreateColumnData(p,row,col,s[i][0],s[i][1],i);
		}
		for(i = 0 ; i < row ; i++ )
		{
			for( j = 0 ; j < col ; j++)
			{
				if(j!=col-1)
					fprintf(fp,"%d,",p[i][j]);
				else
					fprintf(fp,"%d\n",p[i][j]);
			}
		}
		fclose(fp);
		
	}


	//release memory space
	{
		int i = 0 ;
		for( i = 0 ; i < row ; i ++ )
		{
			free(p[i]);
		}
		free(p);
	}


}

/*
 * FUNTION: create a column data that meets the attribute given
 * argument-in: pointer of memory space 
 * argument-row: the aim file has a row-number rows data
 * argument-col: the aim file has a col-number columns data
 * argument-sam: the given column has sample samples
 * argument-seg: the given column has seg segments
 * argument-which: we need to init the given which column
 * */
void CreateColumnData(void * in , int row ,int col , int sam , int seg ,int which)
{
	int ** p =(int **)in ;
	int repeat[2]={0};

	repeat[0]= row/seg ;
	repeat[1]= row - (repeat[0]*(seg-1));

	int val = 0 ;
	int i = 0 , j =0;
	int index = 0 ;
	for( i = 0 ; i < seg ; i++)
	{
		if(i != seg-1)
		{
			for(j =0 ; j < repeat[0] ; j++)
			{
				p[index++][which]=val ;

			}
		}
		else
		{
			for( j = 0 ; j < repeat[1]; j++)
			{
				p[index++][which]=val;
			}

		}
		val ++ ;
		val%=sam ;
	}
}
