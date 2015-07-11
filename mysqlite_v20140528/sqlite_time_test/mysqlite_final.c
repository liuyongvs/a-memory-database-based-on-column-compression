/*
 * DATE:20140520
 * AUTHOR:JackyLau
 * FUNTION:test sqlite insert search time
 * */
#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<time.h>
#include<sys/time.h>
#include "sqlite3.h"

sqlite3 * create_table();
int import_csv(char * csvname, sqlite3 *db, int *rows);
int insert_records(sqlite3 *db, char *line);
void search_time(int times, sqlite3 *db ,int mod);
void search_records(int id, sqlite3 *db ,int maxid);
void delete_records(int id, sqlite3 *db);
void delete_time(int times, sqlite3 *db, int mod);
int callback(void *params,int n_column,char **column_value,char **column_name);
int csv_parse(char * line ,  char list[][1024]);

int id=0;

sqlite3 * create_database()
{
	char *errmsg = 0;
	int ret = 0;
	sqlite3 *db = 0;
	ret = sqlite3_open("./sqlite3-demo.db",&db);
	sqlite3_exec(db, "PRAGMA synchronous = OFF; ", 0,0,0);
	if(ret != SQLITE_OK)
	{
		fprintf(stderr,"Cannot open db: %s\n",sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}
	printf("Open database\n");

	const char *sql=
		"create table t(Idnum int primary key, Name varchar(20),Blank_1 float,Blank_2 double,Type varchar(2),ID varchar(18),Sex char,Birth Date,Addr varchar(100),ZipCode varchar(10),Unkonw_1 char,Blank_3 char,Country varchar(3),Unkonw_2 int,Unkonw_3 int,Blank_4 char,Blank_5 char,Blank_6 varchar(6),NamePinyin varchar(20),Blank_7 varchar(6),Tel varchar(11),Call varchar(11),Blank_8 char,Email varchar(30),Blank_9 varchar(5),Blank_10 varchar(6),Blank_11 varchar(6) ,Company varchar(50),Blank_12 char,Blank_13 char,Blank_14 char,Zero int,Checkin datetime,Unkonw_4 int)";

	//其中对Blank_6进行了修改有两个Blank_6,有一处出现错误 6 char() ,对所有类似char(5)修改为varchar(5)

	ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(ret != SQLITE_OK){
		fprintf(stderr,"create table fail: %s\n",errmsg);
		exit(EXIT_FAILURE);
	}
	sqlite3_free(errmsg);
	return db;
}


int insert_records(sqlite3 *db, char *line)
{
	char *errmsg = 0;
	int ret=0;

	char split[50][1024];
	cvs_parse(line,split);	
	char sql[1024];

	sprintf(sql,"insert into t values(%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",id++,split[0],split[1],split[2],split[3],split[4],split[5],split[6],split[7],split[8],split[9],split[10],split[11],split[12],split[13],split[14],split[15],split[16],split[17],split[18],split[19],split[20],split[21],split[22],split[23],split[24],split[25],split[26],split[27],split[28],split[29],split[30],split[31],split[32]);

	sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret != SQLITE_OK)
	{
		fprintf(stderr,"insert SQL error: %s\n",errmsg);
		return -1;
	}
	sqlite3_free(errmsg);
	return 0;
}

//import_csv file 

int import_csv(char * csvname, sqlite3 *db, int *rows)
{
	char line[1024];
	FILE *fp;
	if((fp=fopen(csvname,"r"))==NULL)
	{
		printf("can't open this file\n");
		exit(EXIT_FAILURE);
	}

	//get rows and cols
	fgets(line,1024,fp);
	char s1[20];
	char s2[20];
	sscanf(line,"Row:%[^,],Col:%s",s1,s2);
	*rows=atoi(s1);
	int cols=atoi(s2);

	//get column properties 
	fgets(line,1024,fp);

	int i;
	printf("debug: rows %d\n", *rows);
	for(i = 0; i < *rows ;i++)
	{
		fgets(line,1024,fp );
		insert_records(db,line);
	}
	fclose(fp);
	return 0;

}

//search_records

void search_records(int id, sqlite3 *db, int maxid)
{ 
	if(id >maxid)
	{
		fprintf(stderr,"%d beyond the rows\n",id);
		exit(EXIT_FAILURE);
	}
	int ret=0;
	char *errmsg = 0;
	char sql[128];
	sprintf(sql,"select * from t where Idnum = %d",id);
	ret = sqlite3_exec(db,sql,callback,NULL,&errmsg);
	if(ret != SQLITE_OK)
	{
		fprintf(stderr,"query SQL error: %s\n",errmsg);
	}
	sqlite3_free(errmsg);
}

//delete_records

void delete_records(int id, sqlite3 *db)
{
	int ret=0;
	char *errmsg=0;
	char sql[128];
	sprintf(sql, "delete from t where Idnum=%d",id);
	ret=sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if(ret != SQLITE_OK)
	{
		fprintf(stderr,"delete SQL error: %s\n",errmsg);
	}
	sqlite3_free(errmsg);

}

void delete_time(int times, sqlite3 *db, int mod)
{
	struct timeval tpstart,tpend;
	float timeuse;
	int i;
	int * list = calloc(times,sizeof(int));

	srand((unsigned int)time(0));
	for(i = 0 ; i < times ; i++  )
	{
		list[i] = rand() % mod;
	}

	gettimeofday(&tpstart,NULL);
	for(i=0; i<times; i++)
	{
		delete_records(list[i], db);
	}
	gettimeofday(&tpend,NULL);

	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
	timeuse/=1000000;
	printf("Delete %d records by random. Used Time: %f seconds\n", times, timeuse);
	free(list);

}
int callback(void *params,int n_column,char **column_value,char **column_name)
{
	int i;
	for(i=0;i<n_column;i++){
		printf("\t%s",column_value[i]);
	}
	printf("\n");
	return 0;
}

void search_time(int times, sqlite3 *db, int mod)
{
	struct timeval tpstart,tpend;
	float timeuse;
	int i;
	int * list = calloc(times,sizeof(int));

	srand((unsigned int)time(0));
	for(i = 0 ; i < times ; i++  )
	{
		list[i] = rand() % mod;
	}

	gettimeofday(&tpstart,NULL);
	for(i=0; i<times; i++)
	{
		search_records(list[i], db, mod);
	}
	gettimeofday(&tpend,NULL);

	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
	timeuse/=1000000;
	printf("Search %d records by random. Used Time: %f seconds\n", times,timeuse);
	free(list);
}

int cvs_parse(char * line ,  char list[][1024])  //csv cvs error
{

	if(!line  || ! list)
		return -1 ;

	char prevc = ',';
	int i = 0 ;
	int j = 0 ;
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


int main()
{
	sqlite3 * db=create_database();
	char filename[20]="log.csv";

	struct timeval tpstart,tpend;
	float timeuse;
	gettimeofday(&tpstart,NULL);
	int rows;
	if(import_csv(filename, db, &rows)==0)
	{
		printf("import csv success.\n");
	}
	gettimeofday(&tpend,NULL);

	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
	timeuse/=1000000;
	printf("import csv file Used Time: %f seconds\n" ,timeuse );


	//test time
	search_time(1000, db, rows);
	delete_time(1000, db, rows);

        //drop table,防止垃圾数据
	char *errmsg = 0;
	const char *sql="drop table if exists t"; 
	sqlite3_exec(db,sql,0,0,&errmsg); 
	sqlite3_close(db);
	printf("Close database\n");
	return 0;
}
