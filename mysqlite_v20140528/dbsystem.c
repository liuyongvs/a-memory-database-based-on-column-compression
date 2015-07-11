#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include"lib/common.h"
#include"lib/dbtime.h"
#include"runlength.h"
#include"lib/dbmem.h"
#include"dbsystem.h"
#include"nullmark.h"
#include"original.h"
#include"diction.h"
#include"bvector.h"
#include"apptable.h"
#include"debug.h"
static TaskPCB tasklet[NUMMD] ;



void* CreateDBTable(char* name ,int n , int r)
{	
	if(n<=0) {
		debug("parameter error");
		return (void*)NULL ;
	}

	char tablename[32]="";
	pDBTable pdbtable = NULL ;
	pdbtable = DBcalloc(1,sizeof(DBTable));

	pKColumn colarray = (pKColumn)DBcalloc(n,sizeof(KColumn));

	if(name) {
		sprintf(tablename,"%s",name);
	} else {
		sprintf(tablename,"%s","sample.db");
	}
	sprintf(pdbtable->name,"%s",tablename);

	pdbtable->col = n ;
	pdbtable->row = r ;
	pdbtable->pnext=NULL ;
	pdbtable->colarray = colarray ;
	return (void*) pdbtable ;
}

int DestroyDBTable(void * pt)
{
	if(!pt) {
		debug("parameter error");
		return ERR ;
	}
	pDBTable pdbtable =(pDBTable) pt ;
	pDBTable  tmp =pdbtable ;
	while(pdbtable) {
		tmp = pdbtable;
		
		DBfree(pdbtable);
		pdbtable= tmp->pnext ;
	}
	return OK ;
}

int GetCompressionInfo(pSegment in_psg, pSEFT selinfo)
{
	if(!in_psg || !selinfo) {
		debug("parameter error");
		return ERR ;
	}
	pSegment psg = (pSegment)in_psg ;

	double allsame =(double) psg->all_same_items;
	double rows =(double) psg->rows ;
	double null_items = (double) psg->null_items;
	double diff_items = (double)psg->diff_items;


	selinfo->same_rate =allsame/rows;
	selinfo->null_rate = null_items /rows ;
	selinfo->sequence_avg = psg->avrg_items;
	selinfo->diff_rate =diff_items/(rows-diff_items) ;
	selinfo->length_avg =psg->len_item;
	return OK ;
}

int mem_consume_data(DBtype type,int size)
{
	switch(type)
	{
		case bool_type:  return sizeof(bool)*size;
		case char_type:  return sizeof(char)*size;
		case int_type:   return sizeof(int)*size;
		case float_type: return sizeof(float)*size;
		case double_type:return sizeof(double)*size;
		case Date_type:  return sizeof(Date)*size;
		case DateTime_type: return sizeof(DateTime)*size;
		case Interger_type: return sizeof(Interger)*size;
		case Long_type:     return sizeof(Long)*size;
		case Single_type:   return sizeof(Single)*size;
		case Byte_type:     return sizeof(Byte)*size;
		case varchar_type:  return sizeof(varchar)*size;
		case string_type :  return sizeof(char)*size;
		default: return ERR;
	}
	return ERR;
}

CMethod GetCompressionMethod(pSegment selinfo)
{
	if(! selinfo) {
		debug("parameter error");
		return ERR ;
	}
	

	/*get memory consume of data field*/
	int D  =  mem_consume_data(selinfo->type,selinfo->len);

	int l  = selinfo->rows;
	int L  = selinfo->all_diff_items ;
	int M  = selinfo->diff_items;
	int K  = selinfo->same_items;
	int N  = selinfo->null_items ;

	 //"pos" or "pnext" field in runlength method
	int z = sizeof(short); 


	double list[10];
	list[mOrin]      = D*(l-N)+(l/8) ;
	list[mRunLenth]  = (2*z+D)*(K+M)+l/8;
	list[mDicBase]   = D*L+2*l;
	//list[mNullSp ]   = (l-N);
	list[mNullSp]    = 0 ;
	list[mBitVector] = D*L+l*L/8;



	if(N == l){
		return mNullSp;
	}else {
		int i =0 ;
		for(i=0;i<5;i++){
			if(list[i]<0){
				debug("predict mem consume error list[%d] %lf",i,list[i]);
				return ERR;
			}
		}

		CMethod min = mOrin ;
		CMethod j = mOrin;
		for(j = mRunLenth ; j <= mBitVector ;j++ ){
			if(j!= mNullSp && list[min] > list[j] ){
				min = j ;
			}
		}
		return min;
	}
}

static int InitKcolumn(pSegment* sglist , KColumn* kclist , int n)
{
	/*inital part of the kcolumn*/
	if(!sglist || !kclist || n<=0) {
		debug("parameter error");
		return ERR ;
	}
	int i =0 ;
	for(i=0;i<n;i++) {
		kclist[i].type = sglist[i]->type; //type filed
		strncpy(kclist[i].name,sglist[i]->name,32);// name filed
		kclist[i].len = sglist[i]->rows ;// len filed, means rows number
		kclist[i].maxsize = sglist[i]->len ;//maxsize of the type
		kclist[i].psg=(unsigned long)sglist[i];
	}
	return OK ;
}

int DoAssignment(pDBTable pdbtable, pSegment *sglist, pItem *itlist,int n)
{
	if(!pdbtable ||!sglist || !itlist || n<=0) {
		debug("parameter error");
		return ERR ;
	}

	int meindex =0 ;
	int  i =0 ,  j = 0 ;
	int indexlist[NUMMD][1024];
	int tail[NUMMD]={0};
	int msindex[1024] ={0};

	memset(indexlist,0,sizeof(indexlist));

    InitKcolumn(sglist,pdbtable->colarray,n); 
	for( i=0 ; i < n ; i++)
	{
		msindex[i]=sglist[i]->diff_items + sglist[i]->same_items;
		meindex = GetCompressionMethod(sglist[i]);
		indexlist[meindex][tail[meindex]]=i; 
		tail[meindex]++;
	}
	
	/*alloc the size*/
	pItem* pitlist =NULL;
	for( i = 0 ; i < NUMMD ; i++  )
	{
		tasklet[i].count    = tail[i];
		tasklet[i].column   = n ;
		tasklet[i].row      = pdbtable->row;
		tasklet[i].kcollist = pdbtable->colarray;
		tasklet[i].method   = i; 

		if(tail[i]>0)
		{
			tasklet[i].indexlist     = (int*)DBcalloc(tail[i],sizeof(int));               //set column index
			tasklet[i].memsizelist   = (int*)DBcalloc(tail[i],sizeof(int));               //set diction number 
			tasklet[i].typelist      = (DBtype*)DBcalloc(tail[i],sizeof(DBtype));
			tasklet[i].pitlist       = (unsigned long)DBcalloc(tail[i],sizeof(pItem));
			pitlist                  = (pItem*)tasklet[i].pitlist;
			for( j= 0 ; j < tail[i] ;j++)
			{
				tasklet[i].indexlist[j]    = indexlist[i][j];
				tasklet[i].memsizelist[j]  = msindex[indexlist[i][j]];
				tasklet[i].typelist[j]     = sglist[indexlist[i][j]]->type;
				pitlist[j]                 = itlist[indexlist[i][j]];

				/*i represent current compression method*/

				((pKColumn)(tasklet[i].kcollist))[indexlist[i][j]].me=i;
			}
		}
	}
#if DEBUG_METHOD
	char line[18024]="";
	char buf[200]="";
	char name[32]="";
	for( i = 0 ; i < NUMMD ; i++ )
	{
		sprintf(line,"md[%d]:",i);
		for(j=0;j<tail[i];j++)
		{
			sprintf(buf,"%d ",indexlist[i][j]);
			strcat(line,buf);
		}
		strcat(line,"\n");
		puts(line);
	}

	for(i=0; i< n ; i++)
	{
		Debug4Type(tasklet[0].kcollist[i].type,name);
		sprintf(line,"[%d](name:%s Type:%s) len:%d maxsize:%d",
					i,
					tasklet[0].kcollist[i].name,
					name,
					tasklet[0].kcollist[i].len,
					tasklet[0].kcollist[i].maxsize);
		puts(line);
	}

	for( i = 0 ; i < NUMMD ; i++ )
	{
		sprintf(line,"md[%d]:",i);
		for(j=0;j<tail[i];j++)
		{
			Debug4Type(tasklet[i].typelist[j],name);
			sprintf(buf,"(type:%s index:%d memsize:%d) ",name,tasklet[i].indexlist[j],tasklet[i].memsizelist[j]);
			strcat(line,buf);
		}
		strcat(line,"\n");
		puts(line);
	}

#endif
	return OK ;
}
#if 1
void print_rlmethod_units()
{
	const int *indexlist = (const int *)(tasklet[mRunLenth].indexlist);
	const int len        = (const int)  (tasklet[mRunLenth].count);

	int i =0 ;
	for(i=0;i<len;i++) {
		print_kcolumn_units(& (tasklet[mRunLenth].kcollist[indexlist[i]]) ,NULL ,0);	
	}
}
#endif

int DoSchedule(pMCsvFile mfile)
{
	if(!mfile) {
		debug("parameter error");
		return ERR ;
	}
	
	/*proprity collect */
	pSegArray psa ;
	pDBTable ptable=NULL;
	int trows=-1 ;             //row number of table
	int tcols=1  ;             //column number of table

	BuildFileProperties(psa,mfile);
	trows   = mfile->rows ;     //get row number of this table  
	tcols   = mfile->len ;
#if DEBUG_MEMORY_COUNTER
	int mconsumed=0;
	mconsumed = MemoryConsueCount(psa,tcols);
	DBmem_StartCount();
#endif

	ptable=(pDBTable)CreateDBTable("table_v1" ,tcols,trows);

#if DEBUG_PROPERTIES
	print_mfile_properties(psa,tcols);
#endif

#if DEBUG_MFILE
	PrintMemCvsFile(mfile);
#endif
	DoAssignment(ptable,psa,mfile->colarray,tcols);
	RunLengthDispatch(&tasklet[mRunLenth]);
	diction_compression_method_dispatch(&tasklet[mDicBase]);
	NullSuspendDispatch(&tasklet[mNullSp]);
	OriginalDispatch(&tasklet[mOrin]);
	bvector_compression_method_dispatch(&tasklet[mBitVector]);


	//print_pkcolumns(&tasklet[mBitVector],print_diction);

	//print_pkcolumns(&tasklet[mBitVector],print_bvector);
#if DEBUG_RL_PRINT
	print_rlmethod_units();
#endif
	
	debug("completed compressed");

#if DEBUG_APPRECORDS 
	append_records_test(APPEND_TIMES , ptable);
#endif
#if DEBUG_APPEND_COUNTER
	random_appendtable_read(APPEND_ACCESS_TIMES,APPEND_TIMES,ptable);
#endif

#if DEBUG_ACCESS_COUNTER
	random_read_test(ACCESS_TIMES,trows);
#if DEBUG_ACCESS_THREAD  
	random_read_test_v2(ACCESS_TIMES,trows);
#endif
#endif 
	
#if DEBUG_MEMORY_COUNTER
	DBmem_EndCountAndShow("rate.mem","Integrated use 5 method, and gains:",mconsumed);
#endif

	return OK;
}
int MemoryConsueCount(pSegArray psglist , int len)
{
	if(!psglist) {
		debug("parameter error");
		return ERR;
	}

	int i =0 ,all =0 , l, rows;
	DBtype type ;
	int usize = 0 ;
	
	for(i=0;i<len;i++) {
		type= psglist[i]->type;
		l     = psglist[i]->len;
		rows  = psglist[i]->rows;
		usize   = mem_consume_data(type,l);
		all  += usize*rows;
	}
	return all ;
}

int print_pkcolumns(void* pdata,int(*fun)(void*))
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
#if 0
	ptDiction ref_diction= NULL;
#else
	void * ref = NULL;
#endif
	for( i = 0 ; i < t_cols ; i++ ){
		iterator         = index_list[i];
		ref_pkclmn       = &com_kclist[iterator];
		prin_kcolumn_info(ref_pkclmn);
#if 0
		ref_diction      = ref_pkclmn->addr;
		print_diction(ref_diction);
#else
		ref = (void*)(ref_pkclmn->addr);
		fun(ref);
#endif
	}
	return OK;
}
int random_read_test(int n, int mod)
{
	char line[10000];
	char dscr[1000];
	sprintf(dscr,"count %d access times :",n);
	int i =0 ,k=0 ;
	int * rlist = calloc(n,sizeof(int));
#if   DEBUG_ACCESS_FIX     
	random_list(n,rlist,19000);
#else
	random_list(n,rlist,mod);
#endif
	
	for(k =0 ; k < 10 ; k++){

		sprintf(dscr,"[%d] %d times ",k,n);
		DBtime_StartCount();
		for( i = 0 ; i < n ; i++){
			search_DTtask_record(&tasklet[mDicBase]  ,rlist[i], line);
			search_VBtask_record(&tasklet[mBitVector],rlist[i], line);
			search_RLtask_record(&tasklet[mRunLenth], rlist[i], line);
			search_ORtask_record(&tasklet[mOrin]    , rlist[i], line);
			search_NMtask_record(&tasklet[mNullSp]  , rlist[i], line);
		}
		DBtime_EndCountAndShow("access.time",dscr);

	}
	free(rlist);
	return OK;
}

void * func_search_dispatch(void * arg)
{
	if(!arg){
		debug("parameter error");
		return NULL ;
	}
	pARG_SEARCH job     = (pARG_SEARCH)arg ;
	void* task          = job->task;
	const int times     = job->times;
	const int * idlist  = job->idlist;
	int (*fun)(void*,int,char*) = job->fun;
	int i =0 ;
	char line[1024];
	if(job){
		for(i=0; i< times ; i++){
			if(fun){
				fun(task,idlist[i],line);
			}else{
				debug("bad funtion field");
				return NULL;
			}
			
		}
	} else{
		debug("error: get bad job in thread ");
		return NULL;
	}
	return NULL;
}
int random_read_test_v2(int n, int mod)
{
	char dscr[1000];
	sprintf(dscr,"%d times access(5 thread) :",n);
	int i =0 ,k=0;
	int * rlist = calloc(n,sizeof(int));
#if DEBUG_ACCESS_FIX
	random_list(n,rlist,19000);
#else
	random_list(n,rlist,mod);
#endif

    //create search task
	tARG_SEARCH  thread_list[5];
	memset(thread_list,0,sizeof(thread_list));
	for(i=0;i<5;i++){
		thread_list[i].task    = &tasklet[i];
		thread_list[i].idlist  = rlist;
		thread_list[i].times   = n;
	}

	thread_list[0].fun    = &search_ORtask_record;
	thread_list[1].fun    = &search_RLtask_record;
	thread_list[2].fun    = &search_DTtask_record;
	thread_list[3].fun    = &search_NMtask_record;
	thread_list[4].fun    = &search_VBtask_record;



	/*create worker thread*/
	int err ;


	int mlen=5;
	pthread_t tid[5];

	for( k = 0 ; k < 10 ; k++){
		DBtime_StartCount();
		for( i = 0 ; i < mlen  ;i++ ){
			err = pthread_create(&tid[i], NULL,func_search_dispatch, &(thread_list[i]));
			if (err != 0){
				debug("\ncan't create thread :[%s]", strerror(err));
			}
		}

		for( i = 0 ; i < mlen ; i++ ){
			err = pthread_join(tid[i],NULL);
			if(err!=0){
				debug("ERROR; return code from pthread_join() is %d\n", err);
			}
		}
		sprintf(dscr,"[%d] %d times",k,n);
		DBtime_EndCountAndShow("thread.time",dscr);
	}

	free(rlist);
	return OK;
}
int create_appendtable(pDBTable dbtable,int sbrows)
{
	if(!dbtable){
		debug("parameter error");
		return ERR ;
	}
	const int cols = dbtable->col ; //get col number of dbtable
	const int sid  = dbtable->row ; //get row number of dbtable

	char tbname[100] ;
	sprintf(tbname,"%s",dbtable->name);
	pKColumn properties = dbtable->colarray; //get pKColumn
	pAPPTable apptable  = NULL ;           
	if(properties){
		apptable = create_apptable(cols,sbrows,sid, tbname,properties);

		if(apptable){
			dbtable->tmptable = apptable ;   //set tmptable field of DBtable
		}else{
			debug("fail to create  tmptable ");
			return ERR;
		}
	}else{
		debug("error: bad pKColumn field of DBTable");
		return ERR;
	}
	return OK;
}

int destroy_appendtable(pDBTable dbtable)
{
	if(!dbtable){
		debug("parameter error");
		return ERR ;
	}

	pAPPTable appendtable = dbtable->tmptable ;  //get tmptable field form DBTable 
	
	if(appendtable){

		const int records = appendtable->func_get_apptable_records(appendtable);  
		int (*destroy_apptable)(struct APPTable*) = appendtable->func_destroy_apptable;

		if( destroy_apptable  && (OK == destroy_apptable(appendtable))){
			dbtable->row    -= records;
			dbtable->tmptable= NULL;  
		}else{
			debug("error: bad filds  in TmpTable ");
			return ERR;
		}
	}
	return OK;
}
int append_record(char * record , pDBTable dbtable)
{
	if(!record || !dbtable){
		debug("parameter error");
		return ERR ;
	}
	pAPPTable apptable = dbtable->tmptable ; //get address of tmptable
	if(apptable){

		int (*AppendRecord)(char *,struct APPTable*)  = apptable->func_append_record;
		if((AppendRecord) && (OK == AppendRecord(record , apptable))){
			dbtable->row++ ; 
		}else{
			debug("error: fail to append record ");
			return ERR;
		}
	}else{
		debug("error:bad DBtable field: tmptable field");
		return ERR;
	}
	return OK;
}

int search_append_record(const int id , char * _line,pDBTable dbtable)
{
	if(!_line || !dbtable){
		debug("parameter error");
		return ERR ;
	}

	pAPPTable apptable = dbtable->tmptable;
	if(apptable){
		  int (*searchAPP_record)(const int, char*,pAPPTable) = apptable->func_searchAPP_record;
		  if(searchAPP_record && (ERR == searchAPP_record(id,_line,apptable))){
			  debug("error :failed to search tmptable with id=%d",id);
			  return ERR;
		  }
	}
	return OK;
}

int append_records_test(int times , pDBTable dbtable)
{
	if(times<=0 || !dbtable ){
		debug("parameter error");
		return ERR ;
	}

	char dscr[1000];
	sprintf(dscr,"%d times appending operation:",times);

	char trecord[1024]="张春燕,,,ID,320123198501012626,F,1985-01-01,南京市秦淮区凤游射49号,,F,,CHN,32,3202,,,,,,,,,,汉,,,,,,,0,2010-9-2 21:07:44,2094150";


	int i,k;
	for( k = 0 ; k < 10 ; k++ ){
		DBtime_StartCount();

		create_appendtable(dbtable,10);
		for(i =0 ; i < times ; i++){
			append_record(trecord,dbtable);
		}
		sprintf(dscr,"[%d] %d times appending operation:",k,times);

		DBtime_EndCountAndShow("append.time",dscr);

		destroy_appendtable(dbtable);
	}
	return OK;
}
int random_appendtable_read(int times ,int mod,pDBTable dbtable)
{
	char line[10000];
	char dscr[1000];
	int i =0 ,k=0 ;
	int * rlist = calloc(times,sizeof(int));

	random_list(times,rlist,mod);
	
	create_appendtable(dbtable,10);
	char trecord[1024]="张春燕,,,ID,320123198501012626,F,1985-01-01,南京市秦淮区凤游射49号,,F,,CHN,32,3202,,,,,,,,,,汉,,,,,,,0,2010-9-2 21:07:44,2094150";

	for( i = 0 ; i < APPEND_TIMES ; i++){
		append_record(trecord,dbtable);
	}
	//end of build append table
	
	for(k =0 ; k < 10 ; k++){
		DBtime_StartCount();
		for( i = 0 ; i < times ; i++){
			search_append_record(20000+rlist[i], line,dbtable);
		}
		sprintf(dscr,"[%d] %d times append access ",k,times);
		DBtime_EndCountAndShow("append-access.time",dscr);
	}
	free(rlist);
	destroy_appendtable(dbtable);
	return OK;
}
