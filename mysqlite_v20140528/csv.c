/*
 * date:20140123
 * author:lanhxg
 * funtion:measure the csv file 
 * */
#include<string.h>
#include<ctype.h>
#include<errno.h>
#include<stdlib.h>

#include"csv.h"
#include"./lib/common.h"
#include"./lib/dbmem.h"


void mytrim(char * dst , char *src )
{
	if(!dst || !src )
	  return ;
	char str[1024] ;
	int len = strlen(src);
	int i ,j ;
	for( i = 0 , j=0 ; i < len ; i++){
		if(src[i]==' '){
			continue ;
		} else{ 
			str[j++] = src[i];
		}
	}
		  
	str[j] ='\0';
	strncpy(dst,str,j+1);
}
int DateCpy(Date* dst ,Date* src) 
{
	if(!dst||!src) {
		debug("null parameter");
		return ERR ;
	}
	memcpy(dst,src,sizeof(Date));
	return OK ;
}
int cmpDate(Date* dst ,Date *src)
{
	if(!dst||!src) {
		debug("null parameter");
		return ERR ;
	}

	if(dst->year!=src->year ||
		dst->month != src->month ||
		dst->date  != src->date ) {
		return 0 ;
	}
	return 1 ;
}

int DateTimeCpy(DateTime* dst ,DateTime* src)
{
	if(!dst||!src) {
		debug("null parameter");
		return ERR ;
	}
	memcpy(dst,src,sizeof(DateTime));
	return OK ;
}

int cmpDateTime(DateTime* dst ,DateTime* src)
{

	if(!dst||!src) {
		debug("null parameter");
		return ERR ;
	}

	if(dst->year!= src->year||
				dst->month != src->month||
				dst->date !=src->date||
				dst->hour !=src->hour||
				dst->minute != src->minute||
				dst->sec != src->sec
				) {
		return 0 ;
	}
	return 1  ;
}
 
/**/
pItem CloneItem(pItem psrc)
{
	if(!psrc) {
		debug("null parameter");
		return NULL;
	}

	pItem pnew = (pItem)DBcalloc(1,sizeof(Item));
	int len = psrc->len ;
	switch(psrc->type)
	{
		
		case bool_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(bool));
		len=len*sizeof(bool);
			break ;
		case char_type:  
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(char));
		len=len*sizeof(char);
			break ;
		case int_type:  
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(int));
		len=len*sizeof(int);
			break ;
		case float_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(float));
		len=len*sizeof(float);
			break ;
		case double_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(double));
		len=len*sizeof(double);
			break ;
		case Date_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(Date));
		len=len*sizeof(sizeof(Date));
			break ;
		case DateTime_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(DateTime));
		len=len*sizeof(DateTime);
			break ;
		case Interger_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(Interger));
		len=len*sizeof(Interger);
			break ;
		case Long_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(Long));
		len=len*sizeof(Long);
			break ;
		case Single_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(Single));
		len=len*sizeof(Single);
			break ;
		case Byte_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(Byte));
		len=len*sizeof(Byte);
			break ;
		case varchar_type:
		pnew->pdata = (unsigned long)DBcalloc(len,sizeof(varchar));
		len=len*sizeof(varchar);
			break ;
		case string_type:
			pnew->pdata =(unsigned long)DBcalloc(len,sizeof(char));
			len=len*sizeof(char);
			break;
		default:
			debug("undefine type");
			return NULL ;
	};
	memcpy((void*)pnew,(void*)psrc,sizeof(Item));
	memcpy((void*)(pnew->pdata),(void*)(psrc->pdata),len);
	pnew->type = psrc->type ;
	return pnew;
}
int item_clonev2(pItem dst,pItem src)
{
	if(!src || ! dst) {
		debug("null parameter");
		return ERR;
	}

	pItem pnew = dst , psrc = src ;
	int len = psrc->len ;
	switch(psrc->type)
	{
		case bool_type: CLONEPDATA_FOR_CASE(pnew->pdata,bool,len);
		case char_type:  CLONEPDATA_FOR_CASE(pnew->pdata,char,len);
		case int_type:  CLONEPDATA_FOR_CASE(pnew->pdata,int,len);
		case float_type: CLONEPDATA_FOR_CASE(pnew->pdata,float,len);
		case double_type: CLONEPDATA_FOR_CASE(pnew->pdata,double,len);
		case Date_type: CLONEPDATA_FOR_CASE(pnew->pdata,Date,len);
		case DateTime_type: CLONEPDATA_FOR_CASE(pnew->pdata,DateTime,len);
		case Interger_type: CLONEPDATA_FOR_CASE(pnew->pdata,Interger,len);
		case Long_type: CLONEPDATA_FOR_CASE(pnew->pdata,Long,len);
		case Single_type: CLONEPDATA_FOR_CASE(pnew->pdata,Single,len);
		case Byte_type: CLONEPDATA_FOR_CASE(pnew->pdata,Byte,len);
		case varchar_type: CLONEPDATA_FOR_CASE(pnew->pdata,varchar,len);
		case string_type: CLONEPDATA_FOR_CASE(pnew->pdata,char,len);
		default:
			debug("undefine type");
			return ERR ;
	};
	memcpy((void*)pnew,(void*)psrc,sizeof(Item));
	memcpy((void*)(pnew->pdata),(void*)(psrc->pdata),len);
	pnew->type = psrc->type ;
	return OK;
}
static int mapInter2Int(Item item ,unsigned int threshold)
{
	int  tmp =  ValueOf(item.pdata,int*);
	tmp = tmp < 0 ? -tmp:tmp ;
	return (tmp)%(threshold);
}
static int mapDouble2Int(Item item,unsigned int threshold)
{
	double d = ValueOf(item.pdata,double*);
	int tmp = (int)d ;
	tmp = tmp < 0 ? -tmp:tmp ;
	return tmp%(threshold) ;
}
static int mapFloat2Int(Item item,unsigned int threshold)
{
	float f = ValueOf(item.pdata,float*);
	int tmp = (int)f ;
	tmp = tmp<0?-tmp:tmp ;
	return tmp %threshold ;
}
static int mapLong2Int(Item item,unsigned int threshold)
{
	Long l = ValueOf(item.pdata,Long*);
	int tmp = (int)l;
	tmp = tmp<0?-tmp:tmp ;
	return tmp%threshold;
}
static int mapDate2Int(Item item,unsigned int threshold)
{
	Date tmp = *(Date*)item.pdata ;
	int tmp2 = (int)((tmp.year + tmp.month + tmp.date));
	tmp2 = tmp2 < 0 ? -tmp2:tmp2 ;
	return tmp2 %(threshold) ;
}

static int mapDateTime2Int(Item item,unsigned int threshold)
{
	DateTime tmp = *(DateTime*)item.pdata;
	int tmp2 = (int)((tmp.year 
				+ tmp.month 
				+ tmp.date
				+ tmp.hour
				+ tmp.minute
				+ tmp.sec));
	
	tmp2 = tmp2 < 0 ? -tmp2:tmp2 ;
	return tmp2%(threshold) ;
}
static int mapChar2Int(Item item,unsigned int threshold)
{
	int len = item.len ;
	len = len>1 ? len-2 : len ;

	int i =0 ;
	int all=0;
	for( i = 0 ; i < len ; i++ ) {
		all+= ((char*)(item.pdata))[i];
	}
	all = all<0?-all:all ;
	return all%threshold ;
}
static int mapVarchar2Int(Item item, unsigned int threshold)
{
	int len  = item.len ;
	int i =0, all =0 ;
	len  = len>1?len-2:len ;
	for(i =0 ; i < len ; i++) {
		all+=((varchar*)(item.pdata))[i];
	}
	return all%threshold ;
}
static int mapBool2Int(Item item,unsigned int threshold)
{
	return 0 ;
}
static int mapByte2Int(Item item, unsigned int threshold)
{
	return (ValueOf(item.pdata,Byte*))%threshold;
}
int CompareItems(pItem pitem1 , pItem pitem2)
{
	if(!pitem1 || !pitem2 || pitem1->type!= pitem2->type) {
		debug(" param error");
		return ERR;
	}

	if(pitem1 == pitem2) return 1;

	DBtype type = pitem1->type;
	if((type ==string_type )) {
		if(pitem1->len != pitem2->len )
		  return 0 ;
	}

	int len2 = pitem2->len ;
	int len1 = pitem1->len ;
	int len = len1 < len2 ? len1: len2 ;
	if(len==0 || len1!=len2) {
		return 0 ;
	}
	/*1 : means item are the same */
	switch(type)
	{
		case bool_type:
			return ValueOf(pitem1->pdata,bool*) == ValueOf(pitem2->pdata,bool*) ? 1:0;
		case char_type:
			return ValueOf(pitem1->pdata,char*) == ValueOf(pitem2->pdata,char*)?1:0;
		case int_type:
			return ValueOf(pitem1->pdata,int*) == ValueOf(pitem2->pdata,int*) ?1:0;
		case float_type:
			{
				float diff =0.0;
				float  fa ;
				float  fb ;
				fa = ValueOf(pitem1->pdata,float*) ;
				fb = ValueOf(pitem2->pdata,float*) ;
				diff = fa- fb ;
				diff = abs(diff);
				return diff < EPSILON? 1:0 ;
			}
		case double_type:
			{
				double ddiff =0.0 ;
				double da ;
				double db ;

				da = ValueOf(pitem1->pdata,double*) ;
				db = ValueOf(pitem2->pdata,double*) ;
				ddiff = da-db ;
				ddiff = abs(ddiff);
				return ddiff < EPSILON ? 1:0;
			}
		case Interger_type:
			return ValueOf(pitem1->pdata,int*) == ValueOf(pitem2->pdata,int*)?1:0;
		case Long_type:
			return ValueOf(pitem1->pdata,Long*) == ValueOf(pitem2->pdata,Long*)?1:0;
		case Single_type:
			{
				Single diff =0.0;
				Single fa ;
				Single fb ;
				fa = ValueOf(pitem1->pdata,Single*) ;
				fb = ValueOf(pitem2->pdata,Single*) ;
				diff = fa- fb ;
				diff = abs(diff);
				return diff < EPSILON ?1:0 ;
			}
		case Byte_type:
			return ValueOf(pitem1->pdata,Byte*) == ValueOf(pitem2->pdata,Byte*)?1:0;
		case Date_type:
				return cmpDate(((Date*)pitem1->pdata),(Date*)(pitem2->pdata));
		case DateTime_type:
				return cmpDateTime((DateTime*)(pitem1->pdata),(DateTime*)(pitem2->pdata));
		case varchar_type:
			return (ValueOf(pitem1->pdata,varchar*) == ValueOf(pitem2->pdata,varchar*))?1:0;
		case string_type:
			return strncmp((char*)(pitem1->pdata),(char*)(pitem2->pdata),len)==0?1:0;

		default:
			debug("undefine type");
			return ERR;
	};
}
int MapItem2Int(Item item ,unsigned int threshold)
{
	int re = ERR; //by default
	switch(item.type)
	{
		case bool_type: re = mapBool2Int(item,threshold); break ;
		case char_type: re = mapChar2Int(item,threshold); break ;
		case int_type: re = mapInter2Int(item,threshold); break ;
		case float_type: re = mapFloat2Int(item,threshold); break ;
		case double_type: re = mapDouble2Int(item,threshold); break ;
		case Date_type: re = mapDate2Int(item,threshold); break ;
		case DateTime_type: re = mapDateTime2Int(item,threshold); break ;
		case Interger_type: re = mapInter2Int(item,threshold); break ;
		case Long_type: re = mapLong2Int(item,threshold); break ;
		case Single_type: re = mapDouble2Int(item,threshold); break ;
		case Byte_type: re = mapByte2Int(item,threshold); break ;
		case varchar_type: re = mapVarchar2Int(item,threshold); break ;
		case string_type: re = mapVarchar2Int(item,threshold); break ;
		default: debug("undefine type"); return ERR;
	};
	return re ;
}
pHashTable CreateHashTable(int length)
{
	if(length <=0 ) {
		debug("param length less or equal to zero");
		return NULL;
	}

	pHashTable ptable = (pHashTable)DBcalloc(1,sizeof(HashTable));
	ptable->len = length ;
	return ptable ;
}
int DestroyHashTable(pHashTable pt)
{
	if(!pt) {
		debug("null parameter funtion dispatch");
		return ERR;
	}

	int i =0 ;
	int len = pt->len ;
	pHashNode node =NULL ;
	pHashNode tg   =NULL ;
	for( i = 0 ; i < len ;i++) {
		node = pt->hasharray[i];
		while(node) {
			tg=node ;
			node = node->pnext ;
			if(tg->pitem){
#if 0
				if(tg->pitem->pdata){
					DBfree((void*)(tg->pitem->pdata));
				}
#endif
				DBfree(tg->pitem);
			}
			DBfree(tg);
		}
	}
	DBfree(pt);
	return OK ;
}

int AddItem2HashTable(pItem pitem ,pHashTable ptable)
{
	if(!ptable) {
		debug("null parameter dispatch");
		return ERR;
	}
	pItem  pp , p ,c ;
	pp = ptable->ppreInsertItem ;
	p = ptable->preInsertItem;
	c = pitem ;
	if(p && p->len!=0) {
		//both exits
		if((pp&&pp->len!=0) && (c&&c->len!=0)) {
			//diff from pp and c
			if(CompareItems(p,c)==0 && CompareItems(p,pp)==0 ) {
				ptable->diff++ ;
			}
		} else if(pp&&pp->len!=0) {
			//the last one
			if(CompareItems(p,pp)==0) {
				ptable->diff++ ;
			}
		}
		else if(c&&c->len!=0) {
			//the first one or pp is null
			if(CompareItems(p,c)==0) {
				ptable->diff++;
			}
		} else {
			ptable->diff++ ;
		}
	}

	if(c==NULL) {
		return OK ;
	}

	ptable->rows++ ;
	//null value
	if(pitem->len == 0) {
	  ptable->null_items++ ;
	  ptable->ppreInsertItem=ptable->preInsertItem ;
	  ptable->preInsertItem=pitem ;
	  return OK  ;
	}

	/*update the max length of the data(size by byte)*/
	if(ptable->max_len < pitem->len) {
		ptable->max_len = pitem->len ;
	}



	int hlen = ptable->len ;
	int hashValue = MapItem2Int(*pitem,hlen);

	if(hashValue >= hlen || hashValue<0 ) {
		debug("hash value out of range");
		return ERR ;
	}

	pHashNode pnode = ptable->hasharray[hashValue];
	pHashNode pre = pnode ;

	while(pnode) {
		//找到插入点
		if(CompareItems(pitem,pnode->pitem) == 1) {
			//与上次插入的值不同
			if(ptable->preInsertItem && 
			   CompareItems(ptable->preInsertItem,pitem)==0) {
				/*有几个连续序列*/
				pnode->times++;
			}

			ptable->ppreInsertItem = ptable->preInsertItem;
			ptable->preInsertItem = pnode->pitem;
			/*same counter*/
			pnode->repeat_items++ ;
			return OK ;
		}
		pre = pnode ;
		pnode = pnode->pnext ;
	}

	//执行到这里 说明不存在该值的结点
	pHashNode pNewNode =  DBcalloc(1,sizeof(HashNode));
	pNewNode->pitem = CloneItem(pitem);
	pNewNode->repeat_items =1 ;
	/*第一次为1*/
	pNewNode->times=1;
	pNewNode->pnext=NULL;
	ptable->ppreInsertItem = ptable->preInsertItem;
	ptable->preInsertItem = pitem; 
	ptable->all_diff++; 
	
	if(ptable->hasharray[hashValue]==NULL)
		ptable->hasharray[hashValue]=pNewNode;
	else
		pre->pnext = pNewNode ;
	return OK;
}
int add_item2hashtable_v2(pItem pitem ,pHashTable ptable)
{
	if(!ptable) {
		debug("null parameter dispatch");
		return ERR;
	}

	ptable->rows++ ;
	//null value
	if(pitem->len == 0) {
	  ptable->null_items++ ;
	  return OK  ;
	}

	/*update the max length of the data(size by byte)*/
	if(ptable->max_len < pitem->len) {
		ptable->max_len = pitem->len ;
	}


	int hlen      = ptable->len ;
	int hashValue = MapItem2Int(*pitem,hlen);

	if(hashValue >= hlen || hashValue<0 ) {
		debug("hash value out of range");
		return ERR ;
	}

	pHashNode pnode = ptable->hasharray[hashValue];
	pHashNode pre   = pnode ;

	/*find insert point for hash table*/
	while(pnode) {
		if(CompareItems(pitem,pnode->pitem) == 1) {
			/*same counter*/
			pnode->repeat_items++ ;
			return OK ;
		}
		pre = pnode ;
		pnode = pnode->pnext ;
	}

	//执行到这里 说明不存在该值的结点
	pHashNode pNewNode =  DBcalloc(1,sizeof(HashNode));
	pNewNode->pitem = CloneItem(pitem);
	pNewNode->repeat_items =1 ;
	/*第一次为1*/
	pNewNode->times=1;
	pNewNode->pnext=NULL;

	ptable->nodes++ ;
	if(ptable->hasharray[hashValue]==NULL)
		ptable->hasharray[hashValue]=pNewNode;
	else
		pre->pnext = pNewNode ;
	return OK;
}

int ExtractPropertiesFromHashTable(pHashTable ptable,pSegment pseg)
{
	if(!ptable || !pseg) {
		debug("null parameter funtion dispatch");
		return ERR;
	}


	
	memset((void *)pseg,0,sizeof(Segment));
	pseg->diff_items = ptable->diff ;
	pseg->rows       = ptable->rows;
	pseg->len        = ptable->max_len ; 
	pseg->null_items = ptable->null_items; 
	pseg->type       = ptable->type;
	pseg->all_diff_items = ptable->all_diff;
	
	int i = 0 ;

	/*有程压缩可以分为几段*/
	int alltimes =0;
	/*除去空值 还剩下的值*/
	int okvalue =0 ;
	int len = ptable->len ;
	pHashNode index ;
	for( i = 0 ; i < len ; i++ ) {
		index = ptable->hasharray[i];
		while(index) {
			alltimes += index->times;
			okvalue+= index->repeat_items;
			index= index->pnext ;
		}
	}

	pseg->all_same_items = okvalue - pseg->diff_items;
	pseg->same_items = alltimes-pseg->diff_items ;
	
	if(pseg->same_items!=0) {
		pseg->avrg_items=pseg->all_same_items/pseg->same_items;
	} else {
		pseg->avrg_items=0;
	}

	if(pseg->same_items!=0 && pseg->diff_items!=0) {
		pseg->len_item = pseg->rows/(pseg->same_items+pseg->diff_items);
	} else {
		pseg->len_item=0 ;
	}

	return OK ;
}
int BuildFileProperties(pSegArray psa, pMCsvFile mfile)
{
	if(!psa || !mfile) {
		debug("null parameter funtion dispatch");
		return  ERR;
	}

	int row   =0;                           //storage of row number of mfile         
	int index =0;                           //storage of column number of mfile

	GetFileColumnAndRow(&index,&row,mfile); //get row and col field of mfile
	
    pHashTable phashtable=NULL;	
	pItem ilist    = NULL;
	pSegment psg   = NULL;
	int i = 0 , j = 0 ;
	for( i = 0 ; i < index ; i++ ) {
		ilist=GetSpecifiedColumn(&row,i,mfile);

		/*set the threshhold = 100 */
		phashtable = CreateHashTable(100);
		phashtable->type =ilist[0].type;

		for( j = 0 ; j < row ; j++ ) {
			AddItem2HashTable(&(ilist[j]),phashtable);
		}
		AddItem2HashTable(NULL,phashtable);
		psg = (pSegment) DBcalloc(1,sizeof(Segment));
		psa[i]=psg ;
		ExtractPropertiesFromHashTable(phashtable,psg);
		memcpy(psa[i]->name,mfile->attlist[i].colName,32);

		DestroyHashTable(phashtable);
	}
	return OK ;
}
int print_properties(char * dsc , void * fp , pSegment psg)
{
	if(!fp || !psg) {
		debug("null parameters funtion dispatch") ;
		return ERR ;
	}

	FILE * outStream  = (FILE *)fp ;

	char str_type[50];
	Debug4Type(psg->type,str_type);
	char str[1024];
	strcpy(str,dsc);
	strcat(str,"\n");
	fprintf(outStream,"%-30s\n",str);
	fprintf(outStream,"%-10s	%20s\n","name:",psg->name);
	fprintf(outStream,"%-10s	%20s\n","type:",str_type);
	fprintf(outStream,"%-10s	%20d\n","rows:",psg->rows);
	fprintf(outStream,"%-10s	%20d\n","same-items:",psg->same_items);
	fprintf(outStream,"%-10s	%20d\n","all_same_items:",psg->all_same_items);
	fprintf(outStream,"%-10s	%20d\n","diff_items:",psg->diff_items);
	fprintf(outStream,"%-10s	%20d\n","all_diff_items:",psg->all_diff_items);
	fprintf(outStream,"%-10s	%20d\n","null_items:",psg->null_items);
	fprintf(outStream,"%-10s	%20d\n","avrg_items:",psg->avrg_items);
	fprintf(outStream,"%-10s	%20d\n","len_item:",psg->len_item);
	fprintf(outStream,"%-10s	%20d\n","len:",psg->len);
	return OK ;
}

int GetFileColumnAndRow(int *col,int *row,pMCsvFile mfile )
{
	if(!col || !row || !mfile) {
		debug("error parameter:null parameter");
		return ERR ;
	}
	
	*col = mfile->len ;
	*row = mfile->rows ;
	
	if(*col == 0  || *row  == 0) {
		debug("column number or row equal to zero,maybe you use a  empty mcsfile");
		return ERR ;
	}
	return OK ;
}
pItem GetSpecifiedColumn(int *len_dep,int index,pMCsvFile mfile)
{
	if(!len_dep || !mfile || index < 0 ) {
		debug("parameter error");
		return NULL ;
	}
	int len , row ;
	GetFileColumnAndRow(&len,&row,mfile);
	*len_dep = row ;

	if(index>= len) {
		debug("your quest out of range");
		return NULL ;
	}

	return mfile->colarray[index];
}

int cvs_parse(char * line ,  char list[][1024] )
{

	if(!line  || ! list)
		return -1 ;

	char prevc = ',';
	int i = 0 ;
	int j = 0 ;
	char * p  = line;


	for(i = 0  ; *p!='\0'&&*p!='\r'; prevc = *p,p++ ) {
		/*
		 * 0. prevc = ',' 1 ） 刚开始  2）处于字符串中间的值
		 * 1. prevc = curc = ',' set list[i]=""
		 * 2. prevc != curc  we should copy *p to list[i]
		 * */

		if(prevc==',') {
		     if(*p ==','){
			     list[i++][0]='\0';
		     } else {
			     list[i][j++]=*p;
		     }
		} else {
			if(*p == ',') {
				list[i++][j]='\0';
				j=0;
			} else {
				list[i][j++]=*p;
			}
		} 
	}
	if(prevc==',') {
		list[i++][0]='\0';
	} else {
		list[i++][j]='\0';
	}
	return  i ;
}

pMCsvFile InitMemCsvFile(int row , int col ,CAttrtList attlist )
{
	if(row<=0 || col <=0 ) {
		debug("parameter error");
		return NULL ;
	}
	pMCsvFile mfile = (pMCsvFile)DBcalloc(1,sizeof(MCsvFile));

	pItem memcsv = DBcalloc(col*row,sizeof(Item));
	int i ;
	for( i = 0 ; i < col ; i++) {
		mfile->colarray[i] = &memcsv[i*row];
	}

	memcpy(mfile->attlist,attlist,sizeof(CAttrtList));
	mfile->memcsv = memcsv;//set memcsv field of memory csv file
	mfile->len    = col ;  //set column number field of  memory csv file
	mfile->rows   = row ;  // set rows  number field of  memory csv file

	return mfile ;
}

int destroy_mcsvfile(pMCsvFile mcsvfile)
{
	if(!mcsvfile) {
		debug("parameter error");
		return ERR;
	}

    pMCsvFile mfile = mcsvfile;
	const int col   = mfile->len ;  //get column number of memory csv file
	const int row   = mfile->rows;  //get row number of memory csv file
	pItem memcsv    = mfile->memcsv;//get item list address of memory csv file
	
	if(memcsv){
		int i =0 , len = col*row;
		unsigned long pdata =0UL;
		for( i = 0 ; i < len ; i++ ){
			pdata = memcsv[i].pdata;
			if(pdata){
				DBfree((void*)pdata);
			}
			pdata=0UL;
		}
		DBfree(memcsv);
		mfile->memcsv = NULL;
		memset(mfile,0,sizeof(MCsvFile));
	}else{
		debug("error: bad filed of MCsvFile: memcsv");
		return ERR;
	}

	DBfree(mfile);
	mcsvfile=NULL;
	return OK;
}

void AttributeParse(char * line ,int n , CAttrtList alist)
{

	if(!line || n<=0) {
		debug("parameter error");
		return ;
	}

	char list[1024][1024];
	memset(list,0,sizeof(list));
	
	cvs_parse(line,list);
	int i , j ,len;
	char stype[32] ;
	char tmp[32] ;
	char flag ;
	for( i = 0 , flag= 0x0 ; i< n ; i++) {
		memset(stype,0,sizeof(stype));
		memset(tmp,0,sizeof(tmp));
		sscanf(list[i],"%[a-z,_,0-9,A-Z]:%s",alist[i].colName,stype);

		//check if there is a (  
		len = strlen(stype);
		for( j = 0 ; j < len ; j++) {
			if(stype[j]=='(' || stype[j] ==')') {
				flag= 0x01 ;
				strncpy(tmp,stype,32);
				break ;
			}
		}

		alist[i].size = 1 ;  
		if(flag!=0x00) {
			sscanf(tmp,"%[a-z,A-Z](%d)",stype,&(alist[i].size));
		}
		
		//to lower case
		len = strlen(stype);
		for( j = 0 ; j < len ; j++) stype[j] = tolower(stype[j]);

		if(0==strcmp(stype,"bool")) alist[i].type = bool_type ;
		else if(0==strcmp(stype,"char")) alist[i].type = char_type ;
		else if(0==strcmp(stype,"int")) alist[i].type = int_type ;
		else if(0==strcmp(stype,"float")) alist[i].type = float_type ;
		else if(0==strcmp(stype,"double")) alist[i].type = double_type ;
		else if(0==strcmp(stype,"date")) alist[i].type = Date_type ;
		else if(0==strcmp(stype,"datetime")) alist[i].type = DateTime_type ;
		else if(0==strcmp(stype,"interger")) alist[i].type = Interger_type ;
		else if(0==strcmp(stype,"long")) alist[i].type = Long_type ;
		else if(0==strcmp(stype,"single")) alist[i] . type = Single_type ;
		else if(0==strcmp(stype,"byte")) alist[i].type = Byte_type ;
		else if(0==strcmp(stype,"varchar")) alist[i].type = varchar_type ;
		else debug("undefine type:%d %s %s",i,alist[i].colName,stype);


		/*type fixup*/
		if((alist[i].type==char_type || alist[i].type==varchar_type) && alist[i].size>1){
			alist[i].type=string_type;
		}
	}
	return ;
}

int GetItemDscr(pItem dst,char *dscr)
{
	if(!dst || !dscr){
		debug("parameter error");
		return ERR ;
	}
	if(dst->len==0){
		dscr[0]='\0';
	}else if(dst->pdata!=0UL){
			switch(dst->type)
			{
				case bool_type:   sprintf(dscr,"%d",*(bool*)(dst->pdata)); break;
				case char_type:   sprintf(dscr,"%c",*(char*)(dst->pdata)); break;
				case int_type:    sprintf(dscr,"%d",*(int*)(dst->pdata)); break;
				case float_type:  sprintf(dscr,"%f",*(float*)(dst->pdata)); break;
				case double_type: sprintf(dscr,"%lf",*(double*)(dst->pdata)); break;
				case Date_type:   sprintf(dscr,"%d-%d-%d",((Date*)(dst->pdata))->year, 
											  ((Date*)(dst->pdata))->month,
											  ((Date*)(dst->pdata))->date);
												  break;
				case DateTime_type: 
					sprintf(dscr,"%d-%d-%d %d:%d:%d", ((DateTime*)(dst->pdata))->year, 
								((DateTime*)(dst->pdata))->month, 
								((DateTime*)(dst->pdata))->date,
								((DateTime*)(dst->pdata))->hour, 
								((DateTime*)(dst->pdata))->minute, 
								((DateTime*)(dst->pdata))->sec);
								break;
				case Interger_type: sprintf(dscr,"%d",*(int*)(dst->pdata)); break;
				case Long_type:     sprintf(dscr,"%ld",*(Long*)(dst->pdata)); break;
				case Single_type:   sprintf(dscr,"%f",*(Single*)(dst->pdata)); break;
				case Byte_type:     sprintf(dscr,"%d",*(Byte*)(dst->pdata)); break;
				case varchar_type:  sprintf(dscr,"%c",*(varchar*)(dst->pdata)); break;
				case string_type :  sprintf(dscr,"%s",(char*)(dst->pdata)); break;
				default:
					debug("undefined type");
					return ERR ;
			}
	}else{
		debug("bad field of Item");
		return ERR ;
	}
	return OK;
}
int SetItem(pItem dst,DBtype type,char * string)
{
	if(!dst || !string){
		debug("parameter error");
		return ERR ;
	}
	mytrim(string,string);

	dst->type = type ;           //set item type
	if(strlen(string)==0){
		dst->len   =0 ;
		dst->pdata =0UL ;
	}else{
			switch(type)
			{
				case bool_type:
					{
						dst->len       = 1 ;  //set item len field
						bool * boolean = DBcalloc(1,sizeof(bool));
						int i = 0 ;
						int len = strlen(string);
						for(i = 0 ; i < len ; i++ ) string[i] =tolower(string[i]);
						
						if(0==strcmp(string,"null") || 0== strcmp(string,"false")){
							*boolean = false ;
						}else{
							*boolean = true ;
						}
						dst->pdata   = (unsigned long) boolean ;  //set item pdata field
						break ;
					}
				case char_type:   SI_CASE(char,string,"%c",dst);
				case int_type:    SI_CASE(int,string,"%d",dst);
				case float_type:  SI_CASE(float,string,"%f",dst);
				case double_type: SI_CASE(double,string,"%lf",dst);
				case Date_type:
					{
						//fmt: YYYY-MM-DD
						int year ,month ,date ;
						dst->len     = 1 ;                   //set item len field
						Date* ddate   = DBcalloc(1,sizeof(Date));

						sscanf(string,"%d-%d-%d",&year,&month,&date);
						ddate->year  = year, ddate->month = (Byte)month, ddate->date  = (Byte)date;

						dst->pdata   = (unsigned long)ddate;  //set item pdata field
						break ;
					}
				case DateTime_type:
					{
						//fmt:YYYY:MM:DD:mm:ss
						int year,month,date,hour,minute,secs;
						dst->len           = 1 ;  //set item len field
						DateTime* datetime = DBcalloc(1,sizeof(DateTime));

						sscanf(string,"%d-%d-%d %d:%d:%d",&year, &month, &date, &hour, &minute, &secs);
						datetime->year = year ,       datetime->month =  (Byte)month , datetime->date= (Byte)date ;
						datetime->hour = (Byte)hour , datetime->minute = (Byte)minute, datetime->sec = (Byte)secs ;

						dst->pdata         = (unsigned long)datetime; //set item pdata field
						break ;
					}
				case Interger_type: SI_CASE(Interger,string,"%d",dst);
				case Long_type:     SI_CASE(Long,string,"%ld",dst);
				case Single_type:   SI_CASE(Single,string,"%f",dst);
				case Byte_type:     SI_CASE(Byte,string,"%d",dst);
				case varchar_type:  SI_CASE(varchar,string,"%c",dst);
				case string_type:
					{
						int slen = strlen(string)+1; 
						dst->len = slen;           //set item len field
						char * tstr = DBcalloc(slen,sizeof(char));
						memcpy(tstr,string,slen-1);
						tstr[slen-1]='\0';
						dst->pdata  = (unsigned long) tstr;
						break ;
					}
				default:
					debug("undefine type");
					return ERR ;
			};

	}

	return OK;
}
int csvfile_parse(FILE * stream,CAttrtList list,int * frow, int *fcol )
{
	if(!stream || !list || !frow || !fcol){
		debug("parameter error");
		return ERR ;
	}

	memset(list,0,sizeof(CAttrtList));
	fseek(stream, 0L, SEEK_SET) ; //set current position indicator at the  start of the file

	char line[1024]="";
	char colstr[20]="";
	char rowstr[20]="";


	//get first line that contains the infomation about column number and row number
	if(fgets(line,2048,stream)){
		sscanf(line,"%[a-z,A-Z]:%d,%[a-z,A-Z]:%d",rowstr,frow,colstr,fcol);
		//get second line
		if(*frow >0 && *fcol >0 && fgets(line,2048,stream)) {
			AttributeParse(line,*fcol,list);
		}
	}
	return OK;
}
pMCsvFile csvfile_loader(const char * filename)
{
	if(!filename) {
		debug("parameter error : file name is null");
		return NULL ;
	}

	FILE * fp = fopen(filename,"r");
	if(!fp){
		debug("fail to open %s:%s",filename,strerror(errno));
		return NULL ;
	}

	int col , row , i;
	pMCsvFile mfile =NULL; 

	char line[2048]={'\0'};
	char list[1024][1024] ;

	CAttrtList attlist ;
	csvfile_parse(fp,attlist,&row, &col);
	
	mfile = (pMCsvFile)InitMemCsvFile(row ,col,attlist ); //construct memory csv file structure
	mfile->len  = col ;                                   // set len field of memory csv file 
	mfile->rows = row ;                                   // set rows field of memory csv file
	memcpy(mfile->attlist,attlist,sizeof(CAttrtList));    // set attlist field of memory csv file 

	//validate the row numner 
	int counter = 0 ;
	//reads the file content into memory 
	while(fgets(line,2048,fp)!=NULL) {

		if(line[0] == '\0' || col!=cvs_parse(line,list)) { 
			debug("csv parse error:at%d line,column number dismatch",counter+2);
			return NULL;
		}

		for( i = 0 ; i < col ; i++) {
			SetItem(&(mfile->colarray[i][counter]),attlist[i].type,list[i]);
		}
		counter++ ; 
		if(counter >= row){
			break ;
		}
	}
	fclose(fp);
	return mfile ;
}

int PrintMemCvsFile(pMCsvFile mfile)
{
	if(!mfile) {
		debug("parameter error: null parameter");
		return ERR ;
	}

	int  i  , j;
	int len , len_dep ;


	len = mfile->len ;
	len_dep = mfile ->rows ;

	for( j = 0 ; j < len ; j++ ) {
		if(j==len-1){
			printf("%s\n",mfile->attlist[j].colName);
		}else{
			printf("%s,",mfile->attlist[j].colName);
		}
	}
	for( i = 0 ; i < len_dep ; i++ ) {
		for( j = 0 ; j < len ; j++ ) {
			if(j==len-1){
				print_item(mfile->colarray[j][i],"\n");
			}else{
				print_item(mfile->colarray[j][i],",");
			}
		}
	}
	return OK ;
}

int print_item(Item it ,char * split)
{
	char line[1024];
	GetItemDscr(&it,line);
	fprintf(stdout,"%s%s",line,split);
	return OK;
}
int print_items(pItem list,int len,char*split)
{
	if(!list || len < 0 || len >20000){
		debug("parameter error");
		return ERR ;
	}
	int i = 0 ;
	char dscr[100];
	char line[1024]="";
	for( i = 0 ; i < len ; i++){
		GetItemDscr(&list[i],dscr);
		strcat(line,dscr);
		if(i<len-1){
			strcat(line,split);
		}
	}
	puts(line);
	return OK;
}
int print_mfile_properties(pSegArray list,int len)
{
	int i=0;
	for(i =0 ; i < len ; i++ ){
		print_properties("",stdout,list[i]);
	}
	return OK;
}

int Debug4Type(DBtype type, char * typename)
{
	if(typename){
		switch(type)
		{
			case bool_type:     sprintf(typename,"%s","bool_type");  break ;
			case char_type:     sprintf(typename,"%s","char_type");  break ;
			case int_type:      sprintf(typename,"%s","int_type");   break ;
			case float_type:    sprintf(typename,"%s","float_type"); break ;
			case double_type:   sprintf(typename,"%s","double_type");break ;
			case Date_type:     sprintf(typename,"%s","Date_type");  break ;
			case DateTime_type: sprintf(typename,"%s","DateTime_type"); break ;
			case Interger_type: sprintf(typename,"%s","Interger_type"); break ;
			case Long_type:     sprintf(typename,"%s","Long_type");  break ;
			case Single_type:   sprintf(typename,"%s","Single_type");break ;
			case Byte_type:     sprintf(typename,"%s","Byte_type");  break ;
			case varchar_type:  sprintf(typename,"%s","varchar_type");break ;
			case string_type :  sprintf(typename,"%s","string_type"); break ;
			default:            
								sprintf(typename,"%s","undefined_typoe");
								puts(typename);
								return ERR; ;
		};
	}else{
		return ERR;
	}
	return OK ;
}
