#include<stdlib.h>
#include<string.h>
#include<stdlib.h>
#include"diction.h"
#include"lib/dbmem.h"
#include"runlength.h"
#include"csv.h"

long setlen(char* str ,long len)
{
	if(!str || len <= 0 || len>=65536){
		debug("parameter error");
		return ERR ; 
	}
	unsigned char low  = len & ((1<<8)-1);
	unsigned char high = (len>>8) & ((1<<8)-1);

	str[0]=high;
	str[1]=low;
	return len ;
}

long getlen(char*str)
{
	if(!str){
		debug("parameter error");
		return ERR ;
	}
	unsigned char low  = (unsigned char)str[1];
	unsigned char high = (unsigned char)str[0];
	long len = (high<<8) |low ;
	if(len<0 || len >= 65536){
		debug("warning :len out of range");
	}
	return len ;
}

static pKColumn init_kcolumn(pSegment psg,pKColumn pclmn)
{
	if( !psg  || !pclmn ) {
		debug("null parameter");
		return NULL ;
	}

	unsigned int this_dic_len   = psg->all_diff_items ;
	unsigned int datalen        = psg->len ; //if bigger than 1,it must be a string
	unsigned int this_index_len = psg->rows; //this column's row number
	DBtype type                 = psg->type;

	if(this_dic_len <=0 || this_index_len <=0) {
		debug("diclen or indexlen less than zero");
		return NULL;
	}
#if 1
	pclmn->addr =0; 
#else
	//for debug
#endif
    pclmn->flag = 0; // represent the pdata is invalid
	pclmn->type = type ;
	pclmn->len  = this_index_len ;
	pclmn->maxsize = datalen;
	return pclmn ;
}


/*fordebug*/
int printf_tdic(ptTDic _t_dic)
{
	if(!_t_dic){
		debug("parameter erro");
		return ERR ;
	}
	ptTDic t_dic = _t_dic ;
	int len      = t_dic->len ;
	pItem list   = (pItem)t_dic->list ;
	
	/*print content*/
	print_items(list,len,",");
	return OK ;
}

void * create_diction_box(pItem src_item ,pSegment psg)
{
	if(! src_item || ! psg){
		debug("parameter error");
		return (void*)NULL ;
	}
	pItem i_list       = NULL ;
	pHashTable h_table = NULL ;

	h_table            = CreateHashTable(100);
	h_table->type      = src_item[0].type ;

	int i  = 0 , rows = psg->rows;
	i_list = src_item ;
	for( i = 0 ; i < rows ; i++ ) {
		add_item2hashtable_v2(&i_list[i],h_table);
	}
	return (void * ) h_table;
}

int extract_items_from_table(pHashTable table,ptTDic _t_dic)
{
	if(!table || !_t_dic ){
		debug("parameter error");
		return ERR ;
	}

	ptTDic t_dic = _t_dic ;
	int nodes  = table->nodes;    
	if(nodes <=0 ){
		debug("nodes <= 0");
		return ERR ;
	}
	
	//t_dic->type = table->type ;
	t_dic->len  = nodes;
	t_dic->type = table->type ;
	
	/*for debug*/
	int index   = 0 , i=0  ;
	int hash_len = table->len ; 
	pHashNode tmp_node =NULL;

	/*assign the memmery for tmp diction */
	pItem list   = calloc(nodes,sizeof(Item));
	t_dic->list  = (void*)list;

	for( i =0 ; i < hash_len ; i++ ){

		tmp_node = table->hasharray[i];
		while(tmp_node){
			item_clonev2(&list[index++],tmp_node->pitem);
			tmp_node =tmp_node->pnext;
		}
		tmp_node = NULL ;
	}
	return OK ;
}
int get_index_form_temdic(const ptTDic _t_dic, const pItem target)
{
	if(! _t_dic || ! target){
		debug("parameter error");
		return -2 ; //error
	}
	int len = _t_dic->len ;
	int i   = 0 ;
	if(target->len==0)
	  return -1 ; //mean a null value

	pItem list = (pItem)_t_dic->list ;
	for(i = 0 ; i < len ; i++ ){
	  if(1 == CompareItems(&list[i],target)) return i ;
	}
	return -2 ;
}
short *creat_diction_index(const ptTDic _t_dic , const pItem src, const int len)
{
	if(!_t_dic || ! src || len<=0 ){
		debug("parameter error");
		return NULL ;
	}
	
	int i = 0,res = 0 ;
	short * ilist = (short*)DBcalloc(len,sizeof(short));
	for( i = 0 ; i < len ; i++ ){
		res = get_index_form_temdic(_t_dic, &src[i]);
		if(res == -2){
			debug("fail to get index in temp diction list");
			return NULL ;
		}
		ilist[i]=res;
	}
	return ilist ;
}
short *creat_diction_index_v2(const ptTDic _t_dic , const pItem src, const int len)
{
	if(!_t_dic || ! src || len<=0 ){
		debug("parameter error");
		return NULL ;
	}
	
	int i = 0,res = 0 ;
	short * ilist = (short*)calloc(len,sizeof(short));
	for( i = 0 ; i < len ; i++ ){
		res = get_index_form_temdic(_t_dic, &src[i]);
		if(res == -2){
			debug("fail to get index in temp diction list");
			return NULL ;
		}
		ilist[i]=res;
	}
	return ilist ;
}
void get_dictiontable_form_tmp(const ptTDic _t_dic, ptDiction _real_dic,int version)
{
	if(!_t_dic || !_real_dic){
		debug("parameter error");
		return ;
	}

	ptTDic    tmp_dic  = _t_dic ;
	ptDiction real_dic = _real_dic ;

	int dic_len        = tmp_dic->len ; //diction table's length
	DBtype data_type   = tmp_dic->type;

	switch(data_type)
	{
		case bool_type: GDFT_CASE(bool,dic_len,tmp_dic,real_dic,version);
		case char_type:   GDFT_CASE(char,dic_len,tmp_dic,real_dic,version);
		case int_type:   GDFT_CASE(int,dic_len,tmp_dic,real_dic,version);
		case float_type: GDFT_CASE(float,dic_len,tmp_dic,real_dic,version);
		case double_type: GDFT_CASE(double,dic_len,tmp_dic,real_dic,version);
		case Date_type: GDFT_CASE(Date,dic_len,tmp_dic,real_dic,version);
		case DateTime_type: GDFT_CASE(DateTime,dic_len,tmp_dic,real_dic,version);
		case Interger_type: GDFT_CASE(Interger,dic_len,tmp_dic,real_dic,version);
		case Long_type: GDFT_CASE(Long,dic_len,tmp_dic,real_dic,version);
		case Single_type: GDFT_CASE(Single,dic_len,tmp_dic,real_dic,version);
		case Byte_type: GDFT_CASE(Byte,dic_len,tmp_dic,real_dic,version);
		case varchar_type: GDFT_CASE(varchar,dic_len,tmp_dic,real_dic,version);
		case string_type : /*special diction*/
			{
				pItem item_list = (pItem)(tmp_dic->list);
				long str_len    = -1;
				char * space    = NULL; 
				char * dst      = NULL;
				char * src      = NULL;
				char **dst_str_list = NULL ;  //used for the address array of diction
				int i=0 ;

				if(version){
					dst_str_list  = (char**)DBcalloc(dic_len,sizeof(char**));
				}else{
					dst_str_list  = (char**)calloc(dic_len,sizeof(char**));
				}
				real_dic->pdic  = (unsigned long)dst_str_list;

				for( i = 0 ; i < dic_len ; i++ ){
					str_len = item_list[i].len ;  //each string diction's length

					if(str_len<=0 || str_len>65536) {
						debug("warning: string length out of range");
					}
					else{
						src   = (char*)(item_list[i].pdata);
						if(src) {
							if(version){
								space = DBcalloc(2+str_len,sizeof(char));
							}else{
								space = calloc(2+str_len,sizeof(char));
							}
							dst_str_list[i] = space ;
							setlen(space,str_len); 
							dst   = &space[2]; 
							strncpy(dst,src,str_len);
						}
						else{
							debug("item_list[%d]->pdata=0x%x",i,(unsigned int)src);
						}
					}
				}
			}
			break ;
		default:
			debug("undefined type");
			return ;
	}

}
void * diction_compression_method(pItem src_item ,pSegment psg )
{
	if(!src_item || !psg){
		debug("parameter error");
		return (void*) NULL ;
	}

	/*step1:make a hash table will accelate refering*/

	pHashTable hash_table = NULL ;
	ptTDic t_diction      = calloc(1,sizeof(tTDic)) ; //make a tmp dic
	hash_table = create_diction_box(src_item,psg); //add all items of this column into hash table

	/*step2:get tmp diction and tmp index list */
	extract_items_from_table(hash_table,t_diction); //make tmp diction table from hash table
	t_diction->ilist = creat_diction_index(t_diction,src_item,psg->rows);


	/*step 3: make real diction table and index list*/
	ptDiction  real_diction = DBcalloc(1,sizeof(tDiction));
	get_dictiontable_form_tmp(t_diction,real_diction,1);
	real_diction->len       = t_diction->len;   //set diction table length
	real_diction->type      = t_diction->type;

	/*set the ilist length*/
	real_diction->ilen      = psg->rows;
	real_diction->index_list= t_diction->ilist ; //get ilist address from tmp diction
	t_diction->ilist = NULL ;  //just in case  destroy real_diction->index_list when destroy temp diction

	
    //last step: destroy the hash table
	
	free(t_diction->list);  t_diction->list=NULL;
	free(t_diction);        t_diction      =NULL;
	DestroyHashTable(hash_table);
	return (void*)real_diction;
}
void * diction_compression_method_v2(pItem src_item ,pSegment psg )
{
	if(!src_item || !psg){
		debug("parameter error");
		return (void*) NULL ;
	}

	/*step1:make a hash table will accelate refering*/

	pHashTable hash_table = NULL ;
	ptTDic t_diction      = calloc(1,sizeof(tTDic)) ; //make a tmp dic
	hash_table = create_diction_box(src_item,psg); //add all items of this column into hash table

	/*step2:get tmp diction and tmp index list */
	extract_items_from_table(hash_table,t_diction); //make tmp diction table from hash table
	t_diction->ilist = creat_diction_index_v2(t_diction,src_item,psg->rows);


	/*step 3: make real diction table and index list*/
	ptDiction  real_diction = calloc(1,sizeof(tDiction));
	get_dictiontable_form_tmp(t_diction,real_diction,0); // for bvector
	real_diction->len       = t_diction->len;   //set diction table length
	real_diction->type      = t_diction->type;

	/*set the ilist length*/
	real_diction->ilen      = psg->rows;
	real_diction->index_list= t_diction->ilist ; //get ilist address from tmp diction
	t_diction->ilist = NULL ;  //just in case  destroy real_diction->index_list when destroy temp diction
	
	
	free(t_diction->list);  t_diction->list=NULL;
	free(t_diction);        t_diction      =NULL;
	DestroyHashTable(hash_table);
	return (void*)real_diction;
}

int diction_compression_method_dispatch(void * pdata)
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

		ref_pkclmn       = init_kcolumn(seg_array[i],&com_kclist[iterator]);
		ref_pkclmn->addr = (unsigned long)diction_compression_method(p_item_list[i] ,seg_array[i] );
	}

#if 0
	/*test for one column*/

	pHashTable hash_table = NULL ;

	ptTDic t_diction      = calloc(1,sizeof(tTDic)) ;
	hash_table = create_diction_box(item_array[1],seg_array[1]);

	/*get diction list*/
	extract_items_from_table(hash_table,t_diction); //extract items into tmp diction table
	
	/*get diction index list*/
	t_diction->ilist = creat_diction_index(t_diction,item_array[1],t_rows);

    //need to destroy the hash table
	DestroyHashTable(hash_table);
	printf_tdic(t_diction);

	for(i=0;i<t_rows ; i++){
		printf(" [%d],%d ",i,t_diction->ilist[i]);
	}
	printf("\n");
#endif

	return OK ;
}

int print_diction(void* pdata)
{
	if(!pdata ){
		debug("parameter error");
		return ERR ;
	}

	const ptDiction diction  = (ptDiction)pdata;
	DBtype type              = diction->type ; 
	const short * index_list = diction->index_list;
	const int   table_len    = diction->len ; 
	const int   index_len    = diction->ilen ;
	unsigned long addr_table_list = diction->pdic ; //get address of diction table list
	int i = 0 ;

	switch(type)
	{
		case bool_type: 
			PD_CASE(bool,addr_table_list,table_len,"%2d ",i);
		case char_type:   
			PD_CASE(char,addr_table_list,table_len,"%2c ",i);
		case int_type:   
			PD_CASE(int,addr_table_list,table_len,"%2d ",i);
		case float_type: 
			PD_CASE(float,addr_table_list,table_len,"%3f ",i);
		case double_type: 
			PD_CASE(double,addr_table_list,table_len,"%4lf ",i);
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
		case Interger_type: 
			PD_CASE(Interger,addr_table_list,table_len,"%2d ",i);
		case Long_type: 
			PD_CASE(Long,addr_table_list,table_len,"%2d ",i);
		case Single_type: 
			PD_CASE(Single,addr_table_list,table_len,"%2f ",i);
		case Byte_type: 
			PD_CASE(Byte,addr_table_list,table_len,"%2c ",i);
		case varchar_type: 
			PD_CASE(varchar,addr_table_list,table_len,"%2c ",i);
		case string_type :
		{
			long len ;
			char ** table_list = (char**)addr_table_list;
			char *dst ;
			for( i = 0 ; i < table_len ; i++){
				len = getlen(table_list[i]);
				dst = &table_list[i][2];
				printf("%d:%s ",len,dst);
				dst = NULL;
				len = 0;
			}
			break ;
		}
		default:
			debug("undefined type");
	}
	printf("-----------\n");
	for(i =0 ; i < index_len ; i++) 
	  printf("%3d ",index_list[i]);
	printf("\n");
	return OK;
}
int print_pkcolumn_for_diction(void* pdata)
{
	if(!pdata){
		debug("parameter error");
		return ERR ;
	}

	pTaskPCB task_pcb = (pTaskPCB)pdata;
	int t_cols        = task_pcb->count;      // subtable column number
	int *index_list   = task_pcb->indexlist;  //subtable index list

	/*all task share this kcollist*/
    pKColumn  com_kclist = (pKColumn)task_pcb->kcollist;

	int i  = 0 ,iterator = -1 ;

	pKColumn  ref_pkclmn = NULL;
	ptDiction ref_diction= NULL;
	for( i = 0 ; i < t_cols ; i++ ){
		iterator         = index_list[i];
		ref_pkclmn       = &com_kclist[iterator];
		prin_kcolumn_info(ref_pkclmn);
		ref_diction      = ref_pkclmn->addr;
		print_diction(ref_diction);
	}
	return OK;
}


int get_one_field(const int id , char* field ,ptDiction pdic )
{
	if(id >20000  || id <0 || !pdic || !field){
		debug("parameter error: [id %d] [pdic 0x%x] [field 0x%x]",id,(unsigned int)pdic,(unsigned int)field);
		return ERR ;
	}


	unsigned long taddr      = pdic->pdic ; //get address of diction table
	DBtype type              = pdic->type ; //get type of diction
	const short * index_list = pdic->index_list ; // address of  id list
	const int table_len      = pdic->len ;   //get length of diction table
	const int index_len      = pdic->ilen ;  //get length of id list

	if(taddr ==0  || index_list==NULL || id > index_len ){
		debug("warning: bad Diction field");
		return ERR ;
	}
	
	const short pos   = index_list[id];

	if(pos >= table_len || pos < -1){
		debug("error: index out of range of diction list");
		return ERR ;
	}

	if(-1 == pos){
		/*represent it was a null value*/
		field[0]='\0';
	}
	else{
		switch(type)
		{
			case bool_type: GOR_CASE(bool,taddr,field,"%d",pos);
			case char_type: GOR_CASE(char,taddr,field,"%c",pos);
			case int_type:  GOR_CASE(int ,taddr,field,"%d",pos);
			case float_type: GOR_CASE(float,taddr,field,"%f",pos);
			case double_type: GOR_CASE(double,taddr,field,"%lf",pos);
			case Date_type: 
				{
					Date * table = (Date*)taddr ;
					sprintf(field,"%d-%d-%d",table[pos].year,
								table[pos].month,
								table[pos].date);
					break ;
				}
			case DateTime_type: 
				{
					DateTime *table = (DateTime*)taddr;
					sprintf(field,"%d-%d-%d %d:%d:%d",table[pos].year,
								table[pos].month,
								table[pos].date,
								table[pos].hour,
								table[pos].minute,
								table[pos].sec);
					break ;
				}
			case Interger_type: GOR_CASE(Interger,taddr,field,"%d",pos);
			case Long_type:     GOR_CASE(Long,taddr,field,"%d",pos);
			case Single_type:   GOR_CASE(Single,taddr,field,"%f",pos);
			case Byte_type:     GOR_CASE(Byte,taddr,field,"%d",pos);
			case varchar_type:  GOR_CASE(varchar,taddr,field,"%c",pos);
			case string_type : 
				{
					char ** table = (char**)taddr ;
					char * dstring = table[pos];
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

int search_DTtask_record(void * pdata, int id, char * _line)
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

	pKColumn ref_pkclmn   = NULL ;
	ptDiction ref_diction = NULL ;
	for( i = 0 ; i < t_cols ; i++ ){
		iterator         = index_list[i]; //get index of kcolumn
		ref_pkclmn       = &com_kclist[iterator];
		ref_diction      = (ptDiction)(ref_pkclmn->addr );

		if(!ref_diction){  //bug
			continue;
		}
		get_one_field(id ,field, ref_diction);

		strcat(line,field);
		if(i!= t_cols-1){
			strcat(line,",");
		}

		ref_pkclmn  =NULL;
		ref_diction =NULL ;
	}
	sprintf(_line,"%s",line);
	return OK ;
#else
	debug("in dic");
#endif
}


int destroy_column_diction(ptDiction diction)
{
	if(!diction){
		debug("parameter error");
		return ERR ;
	}

	unsigned long table = diction->pdic ;
	short * index_list  = diction->index_list ;
	
	DBtype type         = diction->type ;
	int table_len       = diction->len ;

	/*check if there is any polluted field*/
	if(table==0 || ! index_list || table_len <= 0){
		debug("error:Diction field has been polluted");
		return ERR ;
	}
	if(index_list){
		DBfree(index_list); //free index_list
		diction->index_list = NULL;
	}
	
	switch(type)
	{
		case bool_type: 
		case char_type:
		case int_type:
		case float_type: 
		case double_type:
		case Date_type: 
		case DateTime_type: 
		case Interger_type: 
		case Long_type:  
		case Single_type:
		case Byte_type: 
		case varchar_type: { DBfree((void*)table); break ; }
		case string_type : 
			{
				char ** concrete = (char**) table;
				int i = 0 ;
				char * string = NULL ;
				for( i =0 ; i < table_len ; i ++){
					string = concrete[i];
					if(!string){
						debug("bad string diction");
						return ERR ;
					} else{
						DBfree(string);
					}
					string=NULL ;
				}
				DBfree((void*)table);
				break ;
			}
		default:
			debug("undefine data type");
			return ERR ;
	}
	diction->pdic=0 ;

	DBfree(diction);
	return OK ;
}

int DestroyDictions(void * pdata)
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


	int i  = 0 ,iterator = -1 ;

	pKColumn ref_pkclmn  = NULL ;
	ptDiction ref_table  = NULL ;
	for( i = 0 ; i < t_cols ; i++ ){
		iterator         = index_list[i];
		ref_pkclmn       = &com_kclist[iterator];
		ref_table        = (ptDiction)ref_pkclmn->addr;

		destroy_column_diction(ref_table);
		ref_pkclmn->addr =0 ;

		ref_pkclmn=NULL ;
		ref_table =NULL ;
	}
	return OK ;
}
