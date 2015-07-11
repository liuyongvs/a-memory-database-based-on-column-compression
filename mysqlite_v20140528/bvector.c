
#include<string.h>
#include<stdlib.h>
#include"bvector.h"
#include"diction.h"
#include"lib/dbmem.h"
#include"lib/bitop.h"




/*dispatch diction compress funtion */


void * convert_diction_to_bvector(ptDiction from)
{
	if(!from){
		debug("parameter error");
		return NULL;
	}


	pBDiction bvector =  (pBDiction)DBcalloc(1,sizeof(tBDiction));
	bvector->type     =  from->type ; //set type of diction of BVector
	bvector->len      =  from->len;   //set length of diction table of BVector
	bvector->vlen     =  (from->ilen+7)/8 ;  //set length of vector

	const short* ilist = from->index_list ;
	const int    dlen  = from->len ;
	const int    ilen  = from->ilen;
	if( !ilist || dlen<0 || dlen>20000 || ilen<0 || ilen>20000){
		debug("warning:tDiction field was polluted");
		return (void*)NULL;
	}


	DBtype type = from->type ;

	switch(type)
	{
		case bool_type:   CDTB_CASE(bool,from,bvector,dlen);
		case char_type:   CDTB_CASE(char,from,bvector,dlen);
		case int_type:    CDTB_CASE(int,from,bvector,dlen);
		case float_type:  CDTB_CASE(float,from,bvector,dlen);
		case double_type: CDTB_CASE(double,from,bvector,dlen);
		case Date_type:   CDTB_CASE(Date,from,bvector,dlen);
		case DateTime_type: CDTB_CASE(DateTime,from,bvector,dlen);
		case Interger_type: CDTB_CASE(Interger,from,bvector,dlen);
		case Long_type:   CDTB_CASE(Long,from,bvector,dlen);
		case Single_type: CDTB_CASE(Single,from,bvector,dlen);
		case Byte_type:   CDTB_CASE(Byte,from,bvector,dlen);
		case varchar_type:CDTB_CASE(varchar,from,bvector,dlen);
		case string_type :
			{
				int i ; long slen ;
				char * string ; char * dstring ;
				char ** strlist = (char**)from->pdic ;
				char ** dstrlist = (char**)DBcalloc(dlen,sizeof(char*));

				if(!strlist){
					debug("warning: tDiction field was polluted");
					return (void*)NULL ;
				}else{
					for( i = 0 ; i < dlen ; i++){
						string = strlist[i] ; 
						slen   = getlen(string) ;
						if(slen<0 || !string){
							debug("error:bad string diction");
							return (void*)NULL;
						}else{
							dstring= (char*)DBcalloc(slen+2,sizeof(char));
							memcpy(dstring,string,slen+2);
							dstrlist[i]=dstring ;

							dstring =NULL ;
							string  =NULL;
							slen    =0 ;
						}
					}
					bvector->pdic = (unsigned long)dstrlist;
				}
				break ;
			}
		default:
			debug("undefined type");
			return (void*)NULL;
	}

	/* make bit vector foreach diction */
	Byte ** vector_list = (Byte**)DBcalloc(dlen,sizeof(Byte*));
	int i = 0 ,j =0 ;
	Byte * vector=NULL ;
	int vlen =0;
	for( i = 0 ; i < dlen ; i++ ){
		vlen   = (ilen+7)/8 ;  //get  length of vectort 
		vector = DBcalloc(vlen,sizeof(Byte));
		for( j = 0 ; j < ilen ; j++ ) {
			if(ilist[j] == i)
			set_bit(vector,j,1,ilen); //inital the bit vector
		}
		vector_list[i] =vector ;

		vlen =-1 ;
		vector=NULL ;
	}

	bvector->vlist = vector_list ;

	//free diction 
	destroy_column_diction(from);
	return bvector ;
}

void * bvector_compression_method(pItem src_item ,pSegment psg )
{
	if(!src_item || !psg){
		debug("parameter error");
		return (void*)NULL ;
	}

	/*use diction compress method as a transistor*/
	ptDiction  from = (ptDiction)diction_compression_method_v2(src_item ,psg );


	/* do convert */
	return convert_diction_to_bvector(from);
}

int bvector_compression_method_dispatch(void * pdata)
{
	if(!pdata ){
		debug("parameter error");
		return ERR ;
	}

	pTaskPCB task_pcb = (pTaskPCB)pdata;
	int t_cols        = task_pcb->count; // subtable column number
	int *index_list   = task_pcb->indexlist; //subtable index list

	/*all task share this kcollist*/
    pKColumn  com_kclist = (pKColumn)task_pcb->kcollist;
	pItem * p_item_list  = (pItem*)task_pcb->pitlist; //subtable's item list(needed compressed by this method)


	int i  = 0 ,iterator   = -1 ;
	pSegArray     seg_array  ;

	pKColumn ref_pkclmn  = NULL ;
	for( i = 0 ; i < t_cols ; i++ ){

		iterator         = index_list[i];
		seg_array[i]     = (pSegment)com_kclist[iterator].psg ;

		ref_pkclmn       = &com_kclist[iterator];
		ref_pkclmn->addr = (unsigned long)bvector_compression_method(p_item_list[i] ,seg_array[i] );
	}

	return OK ;
}


int	print_bvector(void * pdata)
{
	if(!pdata ){
		debug("parameter error");
		return ERR ;
	}

	pBDiction bvector = (pBDiction)pdata;

	DBtype type              = bvector->type ;   //get data type
	const int   table_len    = bvector->len ;    //get length of diction table 
	const int   vlen         = bvector->vlen ;   //get length of vector
	unsigned long addr_table_list = bvector->pdic ; //get address of diction table list
	int i = 0 ;

	switch(type)
	{
		case bool_type: PB_CASE(bool,addr_table_list,table_len,"%2d ",i);
		case char_type: PB_CASE(char,addr_table_list,table_len,"%2c ",i);
		case float_type: PB_CASE(float,addr_table_list,table_len,"%3f ",i);
		case double_type: PB_CASE(double,addr_table_list,table_len,"%4lf ",i);
		case Date_type: 
		{
			Date * table_list = (Date*)addr_table_list;
			for( i = 0 ; i < table_len ; i++){
				printf("%d-%d-%d ",table_list[i].year,
							table_list[i].month,
							table_list[i].date);
			}
			break ;
		}
		case DateTime_type: 
		{
			DateTime * table_list = (DateTime*)addr_table_list;
			for( i = 0 ; i < table_len ; i++){
				printf("%d-%d-%d %d:%d:%d ",table_list[i].year,
							table_list[i].month,
							table_list[i].date,
							table_list[i].hour,
							table_list[i].minute,
							table_list[i].sec);
			}
			break ;
		}
		case Interger_type: PB_CASE(Interger,addr_table_list,table_len,"%2d ",i);
		case Long_type: PB_CASE(Long,addr_table_list,table_len,"%2ld ",i);
		case Single_type: PB_CASE(Single,addr_table_list,table_len,"%2f ",i);
		case Byte_type: PB_CASE(Byte,addr_table_list,table_len,"%2c ",i);
		case varchar_type: PB_CASE(varchar,addr_table_list,table_len,"%2c ",i);
		case string_type :
		{
			long len ;
			char ** table_list = (char**)addr_table_list;
			char *dst ;
			for( i = 0 ; i < table_len ; i++){
				len = getlen(table_list[i]);
				dst = &table_list[i][2];
				printf("%ld:%s ",len,dst);
				dst = NULL;
				len = 0;
			}
			break ;
		}
		default: 
			debug("undefined type %d",type);
			return ERR ;
	}
	printf("\n-----------\n");


	Byte * vector= NULL;
	Byte **vlist = bvector->vlist;
	
	if( vlen<0 || vlen > (20000/8) ){
		debug("vector length error");
		return ERR ;
	}else{ /* output */
		for(i=0;i<table_len;i++){
			
			vector=vlist[i];
			if(!vector){
				debug("error:bad vector\n");
				return ERR;
			}else{
				print_bits(vector,vlen*8);
				puts("");
			}
		}
	}
	
	return OK;
}


static int get_field_record(const int id , char* field ,pBDiction bvector )
{
	if(id >20000  || id <0 || !bvector || !field){
		debug("parameter error");
		return ERR ;
	}


	unsigned long taddr      = bvector->pdic ; //get address of diction table
	DBtype type              = bvector->type ; //get type of diction
	const char ** vlist      = (char **)(bvector->vlist) ; //get vector list of dictions 
	const int vlen           = bvector->vlen ;  //get row number of this column
	const int tlen           = bvector->len ;   //get length of diction table

	if(taddr ==0  || vlist==NULL || vlen <0 || vlen> 20000  || tlen <0 || tlen>20000 ){
		debug("warning: bad bvector field");
		return ERR ;
	}
	
	short tid   = -1;

	/*find if exits*/
	int i =0 ;
	for( i = 0 ; i < tlen ; i++){
		if(1==check_bit(vlist[i],id,vlen*8)){
			tid=i;
		}
	}


	if(-1 == tid){
		/*represent it was a null value*/
		field[0]='\0';
	} else{
		switch(type)
		{
			case bool_type:      GFR_CASE(bool,taddr,field,"%d",tid);
			case char_type:      GFR_CASE(char,taddr,field,"%c",tid);
			case int_type:       GFR_CASE(int ,taddr,field,"%d",tid);
			case float_type:     GFR_CASE(float,taddr,field,"%f",tid);
			case double_type:    GFR_CASE(double,taddr,field,"%lf",tid);
			case Date_type: 
				{
					Date * table = (Date*)taddr ;
					sprintf(field,"%d-%d-%d",table[tid].year, table[tid].month, table[tid].date);
					break ;
				}
			case DateTime_type: 
				{
					DateTime *table = (DateTime*)taddr;
					sprintf(field,"%d-%d-%d %d:%d:%d",table[tid].year, table[tid].month, table[tid].date,
								table[tid].hour, table[tid].minute, table[tid].sec);
					break ;
				}
			case Interger_type: GFR_CASE(Interger,taddr,field,"%d",tid);
			case Long_type:     GFR_CASE(Long,taddr,field,"%ld",tid);
			case Single_type:   GFR_CASE(Single,taddr,field,"%f",tid);
			case Byte_type:     GFR_CASE(Byte,taddr,field,"%d",tid);
			case varchar_type:  GFR_CASE(varchar,taddr,field,"%c",tid);
			case string_type : 
				{
					char ** table = (char**)taddr ;
					char * dstring = table[tid];
					long len =-1;
					if(!dstring){
						debug("warning: bad string diction");
						return ERR ;
					}else{
						 len =  getlen(dstring);
						 dstring = &dstring[2];
						 if(len<0 || len > 65536){
							 debug("warning:maybe this string diction is polluted");
						 }else{
							 sprintf(field,"%s",dstring);
						 }
					}
					break ;
				} 
			default:
			{
				debug("undefined data type");
				return ERR ;
			}
		}
	}
	return OK ;
}
int search_VBtask_record(void * pdata,int id, char * _line)
{
#if 1
	if(!pdata || id<0  || id > 20000 || !_line ){
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

	memset(line,0,sizeof(line));
	memset(_line,0,sizeof(_line));

	pKColumn ref_pkclmn   = NULL ;
	ptDiction ref_diction = NULL ;
	for( i = 0 ; i < t_cols ; i++ ){
		iterator         = index_list[i]; //get index of kcolumn
		ref_pkclmn       = &com_kclist[iterator];
		ref_diction      = (pBDiction)(ref_pkclmn->addr );

		if(!ref_diction) continue; //bug

		get_field_record(id ,field, ref_diction);
		strcat(line,field);
		if(i!= t_cols-1){
			strcat(line,",");
		}

		ref_pkclmn  =NULL;
		ref_diction =NULL ;
	}
	sprintf(_line,"%s",line);
#else
	debug("in bv");
#endif
	return OK ;
}
