#include<string.h>
#include"nullmark.h"

static int InitColumn_v2(pKColumn pkcl)
{
	if(!pkcl) {
		debug("parameter error");
		return ERR ;
	}

	pkcl->addr = 0UL;
	pkcl->flag = 0;    //the most import

	return OK ;
}

int NullSuspendDispatch(void * pdata)
{
	if(!pdata) {
		debug("parameter error");
		return ERR ;
	}
	/*task control block*/
	pTaskPCB tpcb    = (pTaskPCB) pdata;

	pKColumn  compkc      = (pKColumn)tpcb->kcollist;       //get common kcolumn list
	const int * indexlist = (const  int *)(tpcb->indexlist);//get subtable index list of common kcolumn list
	const int len        = (const int)tpcb->count;          //get column number  of  subtable of table

	int i =0 ;
	pSegment psg = NULL;
	for( i = 0 ; i < len ;i++ ) {
		psg = (pSegment) compkc[indexlist[i]].psg;
		if(psg && (psg->rows == psg->null_items)){
			InitColumn_v2(&compkc[indexlist[i]]);
		}else{
			debug("error: null mark compressed funtion dispatch, rows != null_items");
			return ERR;
		}
		psg=NULL;
	}
	return OK;
}
static int search_field_record(const int id , char * field,pKColumn kclmn)
{
	if(id <0 || id >20000 || !field || !kclmn){
		debug("parameter error");
		return ERR ;
	}
	int clen       = kclmn->len ; //get row number of this column
	CMethod method = kclmn->me  ; //get compress method of this column
	unsigned long raddr = kclmn->addr ; //get data list of original column 
	if(raddr!=0){
		debug("bad nullmark column field");
		return ERR;
	}else if(id>clen){
		debug("search id out of range");
		return ERR ;
	}else if(method == mNullSp){
		field[0]='\0';
	}else{
		debug("error: mNullSp can not match");
		return ERR;
	}
	return OK;
}
int search_NMtask_record(void * pdata,int id ,char * _line)
{
#if 1
	if(!pdata || id<0 || id >20000 || !_line){
		debug("parameter error");
		return ERR ;
	}

	pTaskPCB task_pcb = (pTaskPCB)pdata;
	int t_cols        = task_pcb->count;      // subtable column number
	int *index_list   = task_pcb->indexlist;  //subtable index list

	/*all task share this kcollist*/
    pKColumn  com_kclist = (pKColumn)task_pcb->kcollist;

	int i  = 0 ,iterator   = -1 ;
	char line[2048]="";
	char field[200]="";

	pKColumn ref_pkclmn   = NULL ;
	for( i = 0 ; i < t_cols ; i++ ){
		iterator         = index_list[i]; //get index of kcolumn
		ref_pkclmn       = &com_kclist[iterator];

		search_field_record(id, field,ref_pkclmn);
		strcat(line,field);
		if(i!= t_cols-1){
			strcat(line,",");
		}

		ref_pkclmn  =NULL;
	}
	sprintf(_line,"%s",line);
	return OK ;
#else
	debug("in nm");
#endif
}
