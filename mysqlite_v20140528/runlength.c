#include"runlength.h"
#include"csv.h"
#include"lib/common.h"
#include"lib/dbmem.h"
#include"lib/bitop.c"
#include"diction.h"
#include<string.h>

int set_null_list(pnArray pnstruct , int index )
{
	if(!pnstruct || index >= pnstruct->len || index <0){
		debug("parameter error");
		return ERR;
	}
	return set_bit(pnstruct->map,index,1,pnstruct->len);
}
static pKColumn InitColumn_v2(pSegment psg, pKColumn pclmn)
{
	if(!psg || !pclmn) {
		debug("null parameter");
		return NULL ;
	}


	 //psg->len >1 it means a string 
	const unsigned int  len      = psg->len ;  //get sizeof data
	const unsigned int  nlen     = psg->rows;  //get row number of this column
	const unsigned int  size_map = (nlen+7)/8; //get storage size of null item of this column
	const unsigned int  size     = psg->same_items + psg->diff_items ;//get storage size of item of this column
	const unsigned int  null     = psg->null_items;
	DBtype type = psg->type;

	/*it means there is null among this array */
	pnArray nlist=NULL ;
	if(null>0) {
		nlist= (pnArray)DBcalloc(1,sizeof(nArray));
		nlist->map = (Byte*)DBcalloc(size_map,sizeof(Byte));
		nlist->len = nlen ;
		pclmn->pdata = (unsigned long)nlist ;
	} else{
	  pclmn->pdata = 0x00;
	}
	
	void * pm    = NULL ;
	if(size !=0) {
			switch(type)
			{
				case bool_type:     pm = DBcalloc(size,sizeof(Rbool_unit)); break ;
				case char_type:     pm = DBcalloc(size,sizeof(Rchar_unit)); break ;
				case int_type:      pm = DBcalloc(size,sizeof(Rint_unit)); break ;
				case float_type:    pm = DBcalloc(size,sizeof(Rfloat_unit)); break ;
				case double_type:   pm = DBcalloc(size,sizeof(Rdouble_unit)); break ;
				case Date_type:     pm = DBcalloc(size,sizeof(RDate_unit)); break ;
				case DateTime_type: pm = DBcalloc(size,sizeof(RDateTime_unit)); break ;
				case Interger_type: pm = DBcalloc(size,sizeof(RInterger_unit)); break ;
				case Long_type:     pm = DBcalloc(size,sizeof(RLong_unit)); break ;
				case Single_type:   pm = DBcalloc(size,sizeof(RSingle_unit)); break ;
				case Byte_type:     pm = DBcalloc(size,sizeof(RByte_unit)); break ;
				case varchar_type:  pm = DBcalloc(size,sizeof(Rvarchar_unit)); break ;
				case string_type:   pm = DBcalloc(size,sizeof(Rstring_unit)); break ;
				default :           debug("type out of define"); return NULL;
			};
			pclmn->flag =1 ; 
	} else {
	  pclmn->flag = 0;
	  pm =0x0UL ;
	}

	pclmn->type = type ;
	pclmn->len  = size ;                 //set storage len field of storage 
	pclmn->addr = (unsigned long)pm  ;   //set address of Compress unit filed of this column
	pclmn->maxsize = len;                //set data size field of this column
	return pclmn ;
}


int  SetUnit(void * ulist,int id,pItem ilist, int pos ,int next ,int tag,DBtype type)
{
	if(!ulist || !ilist || pos <0 || pos >20000 ){
		debug("parameter error");
		return ERR ;
	}
    /* SU_CASE(Type1,id,Type2,ulist,ilist,pos,next,tag)*/
	switch(type)
	{
			case bool_type:     SU_CASE(Rbool_unit,id,bool,ulist,ilist,pos,next,tag);
			case char_type:     SU_CASE(Rchar_unit,id,char,ulist,ilist,pos,next,tag);
			case int_type:      SU_CASE(Rint_unit ,id,int,ulist,ilist,pos,next,tag);
			case float_type:    SU_CASE(Rfloat_unit,id,float,ulist,ilist,pos,next,tag);
			case double_type:   SU_CASE(Rdouble_unit,id,double,ulist,ilist,pos,next,tag);
			case Date_type:
				{	
					RDate_unit *_ulist = (RDate_unit *)ulist;
					RDate_unit *_unit  = (RDate_unit *)(&_ulist[id]);
					Date * dst       = &(_unit->data); 
					Date * src       = (Date*)(ilist[pos].pdata);
					DateCpy(dst,src) ;
					_unit->pos        = pos;
					_unit->pnext      = next;
					_unit->tag        = tag==0?0:1;
					break;
				}
			case DateTime_type:
				{
					RDateTime_unit * _ulist= (RDateTime_unit *)ulist;
					RDateTime_unit * _unit = (RDateTime_unit *)(&_ulist[id]);
					DateTime * dst        = &(_unit->data);
					DateTime * src        = (DateTime*)(ilist[pos].pdata);
					DateTimeCpy(dst,src);
					_unit->pos             = pos;
					_unit->pnext           = next;
					_unit->tag             = tag==0?0:1;
					break;
				}
			case Interger_type: SU_CASE(RInterger_unit,id,Interger,ulist,ilist,pos,next,tag);
			case Long_type:     SU_CASE(RLong_unit,id,Long,ulist,ilist,pos,next,tag);
			case Single_type:   SU_CASE(RSingle_unit,id,Single,ulist,ilist,pos,next,tag);
			case Byte_type:     SU_CASE(RByte_unit,id,Byte,ulist,ilist,pos,next,tag);
			case varchar_type:  SU_CASE(Rvarchar_unit,id,varchar,ulist,ilist,pos,next,tag);
			case string_type:
 				{
					const long slen = (long)(ilist[pos].len) ;
					char * string   = DBcalloc(slen+2,sizeof(char));
					setlen(string,slen);
					Rstring_unit *_ulist = (Rstring_unit *)ulist; 
					Rstring_unit *_unit  = (Rstring_unit *)(&_ulist[id]);

					_unit->data      = string;
					_unit->pos       = pos;
					_unit->pnext     = next;
					_unit->tag       = tag==0?0:1; 

					string          = (char*)(&string[2]);
					memcpy(string,(void*)(ilist[pos].pdata),slen);
					break ;
				}
			default:
				debug("undefine type");
				return ERR;
		}

	return OK;
}

int isNeedNewUnit(int id, pItem ilist)
{
	if(id < 1  || ! ilist){
		debug("parameter error");
		return ERR;
	}

	const pItem curent  = (const pItem)&ilist[id];
	const pItem prev    = (const pItem)&ilist[id-1];
	if(prev->len==0) return 0;
	return CompareItems(curent,prev);
}

int RunLengthMakeFirtLine(PointerArray ptarray,pItemArray pita , int len , int * poslist,pKColSet pclmnset)
{
	int i =0 ,tag=1; 
	DBtype type ;
	void * ulist=NULL;
	pItem ilist =NULL;
	//compress len-column data parallel
	for( i = 0 ; i < len ; i++ ) {

		poslist[i]=0 ;            //becase it is first line
		// null value
		if(pita[i][0].len==0) {
			pnArray  nlist = NULL ;
			nlist = (pnArray)pclmnset->kcolarray[i]->pdata  ;
			if(nlist==NULL){
			  debug("pdata is null");
			  return ERR;
			}
			set_null_list(nlist ,0 ); //mark the null position
			nlist=NULL ;
			continue ;
		}
		type = ptarray[i].type;
		tag  = (i==len-1)?0:1;
		ulist= ptarray[i].pt.p;
		ilist= pita[i];
		SetUnit(ulist,0,ilist,0,0,tag,type);
		poslist[i]++ ;

		ulist=NULL;
		ilist=NULL;
	}
	return OK ;
}

int RunLengthMakeUpdateRest(PointerArray ptarray,pItemArray pita , int cols , int * poslist ,int row,pKColSet pclmnset)
{
	int next=0 , tag =0 ,i = 0 ,clen=0,id=0;
	void * ulist=NULL;
	pItem ilist =NULL;
	DBtype type;
	for( i = 0 ; i < cols ; i++ ) {
		if(pita[i][row].len==0) {
			pnArray nlist=NULL ;
			nlist = (pnArray)pclmnset->kcolarray[i]->pdata;
			if(ERR == set_null_list(nlist,row) ) {
				debug("error: set null list ");
				return ERR;
			}
			nlist=NULL ;
		}else if(0==isNeedNewUnit(row, pita[i])){
			id  = poslist[i] ;
			if(i<cols-1){
				clen = pclmnset->kcolarray[i+1]->len;
#if 1
				next = poslist[i+1]-1;
				next = (next>=clen-1)?clen:next;
				if(next<0) next=0;
#else
				next = ((poslist[i+1]-1) >= clen-1) ? (clen-1): (poslist[i+1]-1);
#endif
				tag  = 1;
			}else{
				next = 0;
				tag  = 0;
			}

			ulist  = ptarray[i].pt.p;
			ilist  = pita[i];
			type   = ilist[0].type;
			SetUnit(ulist,id,ilist,row,next,tag,type);
			poslist[i]++ ;

			ulist=NULL;
			ilist=NULL;
			id =-1;
		  }
	}
	return OK ;
}

pKColSet RunLengthCompressionME_v2(pSegArray psga ,pItemArray pita,pKColumn colarray, int len_dep ,int len,const int *indexlist)
{
	if(!psga || !pita || !colarray || len <= 0 || len_dep<=0) {
		debug("parameter error: %x  %x %x %d %d ",(unsigned int)psga,(unsigned int)pita,(unsigned int)colarray,len_dep,len);
		return NULL;
	}
	const int rows = len_dep ;  //get row number of this subtbale
	const int cols = len     ;  //get col number of this column

	int i ; 
	int poslist[1024] ;//保存当前各个压缩列的最新位置

	/*keep all the run-length compression array*/
	pKColSet pclmnset = (pKColSet)calloc(1,sizeof(KColSet));


	PointerArray ptarray ;
	DBtype type ;


	pclmnset-> len = cols ; //set length of clmnset
	for( i = 0 ; i < cols ; i++ ) {
		pclmnset->kcolarray[i] = InitColumn_v2(psga[i],&colarray[indexlist[i]]);
		pclmnset->kcolarray[i]->maxsize = psga[i]->len ;

		type = pclmnset->kcolarray[i]->type ;
		ptarray[i].type = type ;
		ptarray[i].pt.p = (void*)(pclmnset->kcolarray[i]->addr);
	}

	// start RL compress
	for( i = 0 ; i < rows ; i++ ) {
		if(i==0) {
			RunLengthMakeFirtLine(ptarray,pita ,cols,poslist,pclmnset );
		} else {
			RunLengthMakeUpdateRest(ptarray,pita,cols,poslist,i,pclmnset);
		}
	}
	DBfree(pclmnset);
	return  OK ;
}
int RunLengthDispatch(void * pdata)
{
	if(!pdata) {
		debug("parameter error");
		return ERR;
	}
	pTaskPCB tpcb=NULL;
	tpcb = (pTaskPCB)pdata; 
	const int *indexlist = (const int*)tpcb->indexlist;  //get index list of this task
	const int rows       = (const int)(tpcb->row);       //get row number of this column
	const int cols       = (const int)(tpcb->count);     //get col number of this column
	pKColumn  pkc        = (pKColumn)tpcb->kcollist;;    //get common Kcolumn list 
	pItem*   ilist       = (pItem*)tpcb->pitlist;        //get item
	
	int i =0 ;
	pItemArray pita;
	pSegArray psga;
	for( i = 0 ; i < cols ; i++)
	{
		psga[i]= (pSegment) pkc[indexlist[i]].psg;
		pita[i]= ilist[i];
	}
	RunLengthCompressionME_v2(psga,pita,pkc,rows,cols,indexlist);

	return OK;
}
int prin_kcolumn_info(pKColumn pkc)
{
	if(!pkc) {
		debug("parameter erro");
		return ERR ;
	}

	pKColumn pk = pkc;

	char line[2048] ="";
	char typename[32];
	Debug4Type(pk->type,typename);
	sprintf(line," name:%s\n type:%s\n pdata:0x%x\n addr:0x%x\n len:%d\n maxsize:%d\n flag:%d\n",pk->name,
					typename,
					(unsigned int)pk->pdata,
					(unsigned int)pk->addr,
					pk->len,
					pk->maxsize,
					(int)pk->flag);
		puts(line);
		
	return OK ;
}

int DestroyRunLength(pKColSet pkc)
{
	if(!pkc) {
		debug("paramenter error:");
		return ERR ;
	}

	const int cols  = pkc->len ;
	int i , j;
	DBtype type ;
	pKColumn column ;
	pRstring_unit ps ;
	

	pnArray NullList = NULL;
	Byte * map       = NULL;

	for( i = 0 ; i < cols ; i++) {
		column = pkc->kcolarray[i];
		if(column->pdata!=0) {
			NullList = (pnArray)column->pdata;
			map      = NullList->map;
			if(map) DBfree(map);
			if(NullList) DBfree(NullList);

			NullList = NULL;
			map     =NULL;
		}

		type = column->type ;
		if( type==string_type  && column->flag==1) {
			const int rows = column->len ;
			ps = (pRstring_unit)column->addr ;
			for( j = 0 ; j < rows ; j++)
			  if(ps && ps[j].data)
				DBfree(ps[j].data);
		}else if(column->flag==1  && column->addr) {
			DBfree((void*)column->addr);
		}
	}
	return OK ;
}

int check_is_null(pnArray null_struct, int index)
{
	if(!null_struct || index < 0 || index > null_struct->len) {
		debug("parameter error: null parameter");
		return ERR ; 
	}
	if(1==check_bit(null_struct->map,index,null_struct->len*8)) {
			return index ;
	}
	return ERR ;
}
int PrintNullArray(pKColumn pkc,void *file ,int flag)
{
	if(!pkc) {
		debug("parameter error: null parameter");
		return ERR; 
	}

	FILE * fp =NULL  ;
	if(flag) fp = file ;

	pnArray nulllist = (pnArray) pkc ->pdata ;
	if(!nulllist) {
		if(fp) fprintf(fp,"no null array:\n");
		else printf("no null array:\n");
		return OK ;
	}

	int i ,tag, len = nulllist->len ;
	char content[200000]= "null array:";
	char tmp[1024] ;
	for( i = 0 ; i < len ; i++ ) {
		if( (tag=check_is_null(nulllist,i)) !=ERR ){
			sprintf(tmp,"%d ",tag);
			strcat(content,tmp);
		}
	}

	if(fp) fprintf(fp,"%s\n",content);
	else printf("%s\n",content);
	return OK ;
}

int print_kcolumn_units(pKColumn pkc ,void * file ,int flag)
{
	if(!pkc) {
		debug("parameter error : null parameter");
		return ERR ;
	}

	FILE *fp =  NULL ;
	if(flag) fp = (FILE*)file ;
	const int cols = pkc->len ;

	DBtype  datatype  = pkc->type;

	Pointer  pointer ;
	pointer.pt.p = (void *)pkc->addr;
	pointer.type = datatype ;

	int i ;
	for( i = 0 ; i < cols ;i++ )
	{
		switch(datatype)
		{
			case bool_type:
					fprintf(FFILTER(fp),"(%d,%d,%d)",pointer.pt.bool_pt[i].data, pointer.pt.bool_pt[i].pos, pointer.pt.bool_pt[i].pnext);
					break;
			case char_type :
					fprintf(FFILTER(fp),"(%c,%d,%d)",pointer.pt.char_pt[i].data, pointer.pt.char_pt[i].pos, pointer.pt.char_pt[i].pnext);
					break;
			case int_type:
					fprintf(FFILTER(fp),"(%d,%d,%d)",pointer.pt.int_pt[i].data, pointer.pt.int_pt[i].pos, pointer.pt.int_pt[i].pnext);
					break;
			case float_type:
					fprintf(FFILTER(fp),"(%f,%d,%d)",pointer.pt.float_pt[i].data, pointer.pt.float_pt[i].pos, pointer.pt.float_pt[i].pnext);
					break;
			case double_type:
					fprintf(FFILTER(fp),"(%lf,%d,%d)",pointer.pt.double_pt[i].data, pointer.pt.double_pt[i].pos, pointer.pt.double_pt[i].pnext);
					break;
			case Date_type:
					fprintf(FFILTER(fp),"(%d:%d:%d,%d,%d)",pointer.pt.Date_pt[i].data.year,
								pointer.pt.Date_pt[i].data.month,
								pointer.pt.Date_pt[i].data.date, 
								pointer.pt.Date_pt[i].pos, 
								pointer.pt.Date_pt[i].pnext);
					break;
			case DateTime_type:
					fprintf(FFILTER(fp),"(%d:%d:%d:%d:%d:%d,%d,%d)",pointer.pt.DateTime_pt[i].data.year,
								pointer.pt.DateTime_pt[i].data.month,
								pointer.pt.DateTime_pt[i].data.date,
								pointer.pt.DateTime_pt[i].data.hour,
								pointer.pt.DateTime_pt[i].data.minute,
								pointer.pt.DateTime_pt[i].data.sec, 
								pointer.pt.DateTime_pt[i].pos, 
								pointer.pt.DateTime_pt[i].pnext);
					break;
			case Interger_type:
					fprintf(FFILTER(fp),"(%d,%d,%d)",pointer.pt.Interger_pt[i].data, pointer.pt.Interger_pt[i].pos, pointer.pt.Interger_pt[i].pnext);
					break;
			case Long_type:
					fprintf(FFILTER(fp),"(%ld,%d,%d)",pointer.pt.Long_pt[i].data, pointer.pt.Long_pt[i].pos, pointer.pt.Long_pt[i].pnext);
					break;
			case Single_type:
					fprintf(FFILTER(fp),"(%f,%d,%d)",pointer.pt.Single_pt[i].data, pointer.pt.Single_pt[i].pos, pointer.pt.Single_pt[i].pnext);
					break;
			case Byte_type:
					fprintf(FFILTER(fp),"(%d,%d,%d)",pointer.pt.Byte_pt[i].data, pointer.pt.Byte_pt[i].pos, pointer.pt.Byte_pt[i].pnext);
					break;
			case varchar_type:
					fprintf(FFILTER(fp),"(%c,%d,%d)",pointer.pt.varchar_pt[i].data, pointer.pt.varchar_pt[i].pos, pointer.pt.varchar_pt[i].pnext);
					break;
			case string_type:
					{
						char * string = (char*)(pointer.pt.string_pt[i].data); 
						//long slen     = getlen(string);
						string        = (char*)(&string[2]); 
						fprintf(FFILTER(fp),"(%s,%d,%d)",string,pointer.pt.varchar_pt[i].pos, pointer.pt.varchar_pt[i].pnext);
						break ;
					}
			default:
					debug("undefine type");
					return ERR;
		}
	}
		fprintf(FFILTER(fp),"\n");

		return OK ;
}

void PrintRLCFile(char* filename,pKColSet pkcs)
{
	FILE * fp =NULL ;
	int flag =0 ;
	
	if(filename) {
		fp =fopen(filename,"w");
		flag=1 ;
	}

	int i = 0 ;
	const int cols = pkcs->len ; 
	pKColumn  colist =NULL;
	for( i = 0 ; i < cols ; i++ ){
		if(i < cols-1){
			printf("%s,",pkcs->kcolarray[i]->name);
		}else{
			printf("%s\n",pkcs->kcolarray[i]->name);
		}
	}
	for( i = 0 ; i < cols ;i++) {
		colist = pkcs->kcolarray[i] ;
		PrintNullArray(colist,fp,flag);
		print_kcolumn_units(colist,fp,flag);
	}
	if(fp)
		fclose(fp);
}


int SearchPos(pKColumn current , int id , int from, int to)
{
	if(!current || id < 0 || from <0 ||from >to){
#if 0
		Rstring_unit * ulist   =  (Rstring_unit*)current->addr ;
		debug("parameter error id=%d [%d %d] {%d,%d}",id,from,to,ulist[from].pos ,ulist[to].pos);
#else
		debug("parameter error");
#endif
		return ERR;
	}
	//if(strcmp(current->name,"Country")==0)
	//	debug("----%d [%d %d]",id,from,to);

	DBtype type    =  current->type ;         // get type field  of this column
	const int rows =  current->len  ;         // get row number of this column
	const int flag =  current->flag ;         // get flag field of this column       
	void * ulist   =  (void*)current->addr ;  // get ulist address of this column

	if(to >= rows){
		debug("error :search out of range");
		return ERR;
	}else if(flag==1){
		//star to search pos
		switch(type)
		{
			case bool_type:      SP_CASE(Rbool_unit    ,ulist,from,to,id,current);
			case char_type:      SP_CASE(Rchar_unit    ,ulist,from,to,id,current);
			case int_type:       SP_CASE(Rint_unit     ,ulist,from,to,id,current);
			case float_type:     SP_CASE(Rfloat_unit   ,ulist,from,to,id,current);
			case double_type:    SP_CASE(Rdouble_unit  ,ulist,from,to,id,current);
			case Date_type:      SP_CASE(RDate_unit    ,ulist,from,to,id,current);
			case DateTime_type:  SP_CASE(RDateTime_unit,ulist,from,to,id,current);
			case Interger_type:  SP_CASE(RInterger_unit,ulist,from,to,id,current);
			case Long_type:      SP_CASE(RLong_unit    ,ulist,from,to,id,current);
			case Single_type:    SP_CASE(RSingle_unit  ,ulist,from,to,id,current);
			case Byte_type:      SP_CASE(RByte_unit    ,ulist,from,to,id,current);
			case varchar_type:   SP_CASE(Rvarchar_unit ,ulist,from,to,id,current);
			case string_type:    SP_CASE(Rstring_unit  ,ulist,from,to,id,current);
			default: debug("undefined type "); return ERR;
		}
	}
	return ERR;
}

int IsInNullList(pKColumn current, int id)
{
	if(!current || id < 0) {
		debug("parameter error");
		return ERR;
	}

	/*search result : ERR represent no exit , OK  represnt target exit */
	if(current->pdata) {
		pnArray pnlist_struct=(pnArray)current->pdata;
		if(id == check_is_null(pnlist_struct,id))
		  return OK;
	}
	return ERR ;
}

static int GetUnitDscr(const pKColumn current,char * line, const int id)
{
	if(!current || !line || id <0) {
		debug("parameter error") ;
		return ERR;
	}

	const int rows = current->len ;
	if(id >= rows) {
		debug("id out range of kcolumn's rows");
		return ERR;
	}else{
		Pointer pUnit ;
		pUnit.pt.p = (void*)current->addr;
		pUnit.type = current->type;
		switch(pUnit.type)
		{
			case bool_type:   { sprintf(line,"%d",pUnit.pt.bool_pt[id].data); break ; }
			case char_type:   { sprintf(line,"%c",pUnit.pt.char_pt[id].data); break ; }
			case int_type:    { sprintf(line,"%d",pUnit.pt.int_pt[id].data); break ; }
			case float_type:  { sprintf(line,"%f",pUnit.pt.float_pt[id].data); break ; }
			case double_type: { sprintf(line,"%lf",pUnit.pt.double_pt[id].data); break ; }
			case Date_type: 
				{
					sprintf(line,"%d-%d-%d",pUnit.pt.Date_pt[id].data.year,
								pUnit.pt.Date_pt[id].data.month,
								pUnit.pt.Date_pt[id].data.date);
					break ;
				}
			case DateTime_type: 
				{
					sprintf(line,"%d-%d-%d %d:%d:%d",pUnit.pt.DateTime_pt[id].data.year,
								pUnit.pt.DateTime_pt[id].data.month,
								pUnit.pt.DateTime_pt[id].data.date,
								pUnit.pt.DateTime_pt[id].data.hour,
								pUnit.pt.DateTime_pt[id].data.minute,
								pUnit.pt.DateTime_pt[id].data.sec);
					break ;
				}
			case Interger_type: { sprintf(line,"%d",pUnit.pt.Interger_pt[id].data); break ; }
			case Long_type:     { sprintf(line,"%ld",pUnit.pt.Long_pt[id].data); break ; }
			case Single_type:   { sprintf(line,"%f",pUnit.pt.Single_pt[id].data); break ; }
			case Byte_type:     { sprintf(line,"%d",pUnit.pt.Byte_pt[id].data); break ; }
			case varchar_type:  { sprintf(line,"%c",pUnit.pt.varchar_pt[id].data); break ; }
			case string_type:   
			    { 
					char * string  = pUnit.pt.string_pt[id].data;
					if(string){
						string = (char*)&string[2];
						sprintf(line,"%s",string);
					}else{
						line[0]='\0';
					} 
					break ;
				}
			default:
				debug("parameter error");
				return ERR;
		}
	}
	return OK;
}
static int get_field_record(const pKColumn current,const int id ,char * field , const int from, const int to)
{
	if(!current || id < 0 || !field || from<0 || to < from){
		debug("parameter error");
		return ERR;
	}
	int res        = ERR ;

	if(OK == IsInNullList(current,id)){
		field[0]='\0';
		return -2; 
	}else{
		res = SearchPos(current,id,from,to);
		if(res != ERR){
			GetUnitDscr(current,field,res);
		}else{
			debug("error: search pos in ulist");
			return ERR;
		}
	}
	return res;
}
static int get_next_range(const  pKColumn current,int pos ,int * from,int *to)
{
	if(!current || pos <0 || from<0 || to < from){
		debug("parameter error");
		return ERR;
	}

	const int rows  = current->len ;        //get row number filed
	DBtype  type    = current->type;        //get type filed of this column
	void * ulist    = (void*)current->addr; //get address of compress data
	if(pos<rows){
		switch(type)
		{
			case bool_type:      GNR_CASE(Rbool_unit    ,ulist,pos,rows,from,to);
			case char_type:      GNR_CASE(Rchar_unit    ,ulist,pos,rows,from,to);
			case int_type:       GNR_CASE(Rint_unit     ,ulist,pos,rows,from,to);
			case float_type:     GNR_CASE(Rfloat_unit   ,ulist,pos,rows,from,to);
			case double_type:    GNR_CASE(Rdouble_unit  ,ulist,pos,rows,from,to);
			case Date_type:      GNR_CASE(RDate_unit    ,ulist,pos,rows,from,to);
			case DateTime_type:  GNR_CASE(RDateTime_unit,ulist,pos,rows,from,to);
			case Interger_type:  GNR_CASE(RInterger_unit,ulist,pos,rows,from,to);
			case Long_type:      GNR_CASE(RLong_unit    ,ulist,pos,rows,from,to);
			case Single_type:    GNR_CASE(RSingle_unit  ,ulist,pos,rows,from,to);
			case Byte_type:      GNR_CASE(RByte_unit    ,ulist,pos,rows,from,to);
			case varchar_type:   GNR_CASE(Rvarchar_unit ,ulist,pos,rows,from,to);
			case string_type:    GNR_CASE(Rstring_unit  ,ulist,pos,rows,from,to);
			default: debug("undefined type");
		}
	}else{
		debug("out of range");
		return ERR;
	}
	return OK;
}
int search_RLtask_record(void * pdata,  int id, char * _line)
{
	if(!pdata || id <0 || !_line){
		debug("parameter error");
		return ERR;
	}
	pTaskPCB tpcb= (pTaskPCB) pdata;

	const int * indexlist    = (const int*)(tpcb->indexlist); //get index list of this task
	const int len            = tpcb->count;                   //get the number of column of this task
	const pKColumn com_kclmn = tpcb->kcollist;                //get common column list 

	int i= 0 ,from=0 ,to=0;

	CMethod  method =-1;
	char line[1024]="";
	int pos =-1;
	memset(_line,0,sizeof(_line));
	pKColumn current= NULL;
	for( i = 0 ; i < len ; i++) {
		current = &com_kclmn[indexlist[i]] ;  
		method = current->me;                                //get curent column compressed method            
		if( i== 0){
			from   = 0 ;
			to     = current->len-1 ;
		} else {
			
		}
		if(method == mRunLenth){
			pos = get_field_record(current,id,line, from,to);
			if(ERR!=pos){
				if(pos==-2){   // field[id] is null 
					from = 0;
					to   = -1;
				}else if(i < len-1){
					get_next_range(current,pos ,&from,&to);
				}
				if(to==-1){
					to = com_kclmn[indexlist[i+1]].len-1;
				}
			}else{
				debug("error:fail to dispatch get field record funtion");
				return ERR;
			}
			strcat(_line,line);
			if(i!=len-1){
				strcat(_line,",");
			}else{
				strcat(_line,"\n");
			}
			memset(line,0,sizeof(line));
		} else{
		  debug("compress emthod can not match");
		  return ERR;
		}
	}
	return OK;
}
