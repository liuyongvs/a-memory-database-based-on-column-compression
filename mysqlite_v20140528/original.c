
#include<string.h>
#include"original.h"
#include"csv.h"
#include"lib/dbmem.h"
#include"diction.h"




static void InitColumn(pSegment psg,pKColumn pclmn)
{
	if(!psg || !pclmn) {
		debug("parameter error");
		return  ;
	}
	pclmn->len  = psg->rows ;
	/*intial kcolumn*/
	pclmn->type = psg->type ;

	/*row number*/
	pclmn->maxsize = psg->len;
}

void* CopyColumn(pItem src ,DBtype datatype,int len )
{
	//debug("parameter erro:0x%x %d %d",src,datatype,len);
	switch(datatype)
	{
		case bool_type:
			{
				bool * dst=NULL ;
				MakeColumn(dst,src,bool,len);
				return (void*)dst;
			}
		case char_type:  
			{
				char * dst=NULL ;
				MakeColumn(dst,src,char,len);
				return (void*)dst;
			}
		case int_type:
			{
				int * dst=NULL ;
				MakeColumn(dst,src,int,len);
				return (void*)dst;
			}
		case float_type:
			{
				float * dst=NULL ;
				MakeColumn(dst,src,float,len);
				return (void*)dst;
			}
		case double_type:
			{
				double * dst=NULL ;
				MakeColumn(dst,src,double,len);
				return (void*)dst;
			}
			break ;
		case Date_type:
			{
				int i=0 ;
				Date * val1;
				Date*dst = (Date*)DBcalloc(len,sizeof(Date));
				for( i = 0 ; i < len ; i++ )
				{
					val1=(Date*)src[i].pdata;
					if(src[i].len!=0 && src[i].pdata!=0)
						DateCpy(&dst[i],val1);
				}
				return (void*)dst;
			}
		case DateTime_type:
			{
				int i = 0 ; 
				DateTime * val1 ;
				DateTime *dst = (DateTime*)DBcalloc(len,sizeof(DateTime));
				for( i = 0 ; i < len ; i++ )
				{
					val1 = (DateTime*)src[i].pdata;
					if(src[i].len!=0 && src[i].pdata!=0)
						DateTimeCpy(&dst[i],val1);
				}
				return (void*)dst;
			}
		case Interger_type:
			{
				Interger * dst=NULL ;
				MakeColumn(dst,src,Interger,len);
				return (void*)dst;
			}
		case Long_type:
			{
				Long * dst=NULL ;
				MakeColumn(dst,src,Long,len);
				return (void*)dst;
			}
		case Single_type:
			{
				Single * dst=NULL ;
				MakeColumn(dst,src,Single,len);
				return (void*)dst;
			}
		case Byte_type:
			{
				Byte * dst=NULL ;
				MakeColumn(dst,src,Byte,len);
				return (void*)dst;
			}
		case varchar_type:
			{
				varchar * dst=NULL ;
				MakeColumn(dst,src,varchar,len);
				return (void*)dst;
			}
		case string_type:
			{
				int i = 0 ;
				long l =0; 
				char * str1=NULL ;
				char * str2=NULL ;
				char **dst = (char**)DBcalloc(len,sizeof(char*));
				for( i = 0 ; i < len ; i++ ) {
					str1 = (char*)src[i].pdata;
					str2=NULL ;
					l = src[i].len ; //get length of string len
					if(l>0 && str1 ) { 
						dst[i]=str2 = (char*)DBcalloc(l+2,sizeof(char));
						setlen(str2 ,l);
						str2 = (char*)(&str2[2]);
						strncpy(str2,str1,l);
					}
				}
				return (void*)dst;
			}
		default:
			debug("undefine type");
			return (void*)NULL ;
	};
	return (void*)NULL ;
}
int OriginalDispatch(void*pdata)
{
	if(!pdata) {
		debug("parameter error");
		return ERR ;
	}
	pTaskPCB tpcb=(pTaskPCB)pdata;

	/*the table's row number*/
	int len_dep = tpcb->row;

	/*this task's column number*/
	int len = tpcb->count;
	/*all task share this kcollist*/
	pKColumn  pkc = (pKColumn)tpcb->kcollist;
	pSegArray psga;
	
	pItem* pitlist; 
	pitlist = (pItem*)tpcb->pitlist;
	int * indexlist = tpcb->indexlist; 
	int i = 0 ; 
	//undefine
	DBtype datatype =100;
	for(i=0;i<len;i++) {
		psga[i]=(pSegment) pkc[indexlist[i]].psg;
		InitColumn(psga[i],&pkc[indexlist[i]]);

		/*datatype,make diffrent with char and varchar*/
		datatype=pkc[indexlist[i]].type;
		if(datatype==char_type || datatype==varchar_type) {
			if(pkc[indexlist[i]].maxsize>1)
			  datatype= string_type ;
		}
		pkc[indexlist[i]].addr= (unsigned long)CopyColumn(pitlist[i],datatype,len_dep);
	}
	return OK ;
}

static int search_field_record(const int id , char * field ,pKColumn kclmn )
{
	if(id <0 || id >20000 || !field || !kclmn){
		debug("parameter error");
		return ERR ;
	}
	DBtype type    = kclmn->type; //get type of this column
	int clen       = kclmn->len ; //get row number of this column
	CMethod method = kclmn->me  ; //get compress method of this column
	unsigned long raddr = kclmn->addr ; //get data list of original column 
	char line[1024]="";
	if(id>clen){
		debug("search id out of range");
		return ERR ;
	}else if(method == mOrin){
			switch(type)
			{
				case bool_type:    SFR_CASE(bool,raddr,line,"%d",id);
				case char_type:    SFR_CASE(char,raddr,line,"%c",id);
				case int_type:     SFR_CASE(int,raddr,line,"%d",id);
				case float_type:   SFR_CASE(float,raddr,line,"%f",id);
				case double_type:  SFR_CASE(double,raddr,line,"%lf",id);
				case Date_type:
					{
						Date * datalist = (Date*)raddr ;
						if(datalist) {
							sprintf(line,"%d-%d-%d",datalist[id].year,datalist[id].month,datalist[id].date);
						}else{
							debug("error : bad pkcolumn addr field that points to Date column");
							return ERR ;
						}
					}
				case DateTime_type:
					{
						DateTime * datalist = (DateTime*)raddr ;
						if(datalist){
							sprintf(line,"%d-%d-%d %d:%d:%d",datalist[id].year,datalist[id].month,datalist[id].date,
										datalist[id].hour,datalist[id].minute,datalist[id].sec);
						}else{
							debug("error : bad pkcolumn addr field that points to DateTime column");
							return ERR;
						}
					}
				case Interger_type: SFR_CASE(Interger,raddr,line,"%d",id);
				case Long_type:     SFR_CASE(Long,raddr,line,"%ld",id);
				case Single_type:   SFR_CASE(Single,raddr,line,"%f",id);
				case Byte_type:     SFR_CASE(Byte,raddr,line,"%d",id);
				case varchar_type:  SFR_CASE(varchar,raddr,line,"%c",id);
				case string_type:
					{
						char**strlist = (char**)raddr ;
						char *string=NULL;
						if(strlist){
							string=strlist[id];
							if(string){
								string = (char*)(&string[2]);
								sprintf(line,"%s",string);
							}
						}else{
							debug("error : bad pkcolumn addr field that points to string column");
							return ERR;
						}
						break;
					}
				default:
					{
						debug("undefined data type");
						return ERR ;
					}
			};
	}else{
		debug("error: mOrin can not match");
		return ERR;
	}
	return OK;
}
int search_ORtask_record(void * pdata , int id ,char * _line)
{

#if 1
	if(!pdata || id<0 || id>20000 || !_line){
		debug("parameter errror");
		return ERR ;
	}

	pTaskPCB task_pcb = (pTaskPCB)pdata;
	const int t_cols        = (const int)task_pcb->count;      // subtable column number
	const int *index_list   = (const int *)(task_pcb->indexlist);  //subtable index list

	/*all task share this kcollist*/
    pKColumn  com_kclist = (pKColumn)task_pcb->kcollist;

	int i  = 0 ,iterator   = -1 ;
	char line[2048]="";
	char field[200]="";

	pKColumn ref_pkclmn   = NULL ;
	for( i = 0 ; i < t_cols ; i++ ){
		iterator         = index_list[i]; //get index of kcolumn
		ref_pkclmn       = &com_kclist[iterator];

		if(ERR==search_field_record(id, field ,ref_pkclmn)){
			debug("or sarch file record error");
			return ERR;
		}

		strcat(line,field);
		if(i!= t_cols-1){
			strcat(line,",");
		}
		ref_pkclmn  =NULL;
	}
	sprintf(_line,"%s",line);
	return OK;
#else
	debug("in or");
#endif
}
