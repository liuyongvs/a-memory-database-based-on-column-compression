/*
 * date:20140412
 * author:lanhxg
 * funtion:dictionnary compress method
 * */

#ifndef __DICTION_H__
#define __DICTION_H__
#include"lib/common.h"
#include"csv.h"

typedef struct tDiction
{
	/*type of dic*/
	DBtype type ;

	/*point to a dictionnary table*/
	unsigned long pdic ;
	int len ;                 //the length of this diction table

	/*index list*/
	short * index_list;
	int ilen ;               // index len = the row number of the orignal table
}tDiction,*ptDiction;

typedef struct tTDic
{
	/*the length of list*/
	int len ;

	/*current index */
	int index;
	DBtype type ;

	/*diction list*/
	void *list;

	/*index list*/
	short * ilist ;
}tTDic,*ptTDic;


/*
 * funtion: set length of a string into a str[0] and str[1]
 * args-str: address of the target string
 * args-len: length of the string
 * return: args len will be return if success,or ERR is return 
 * */
long setlen(char* str ,long len);

/*
 * funtion:get length of a string, which string length store in str[0] str[1]
 * agrs-str:address of target string
 * return;the length of this string
 * */
long getlen(char*str);

/*
 * funtion: create a tmp diction box, by add all item into a hash table
 * args-src_item: address of cloumn items list
 * args-psg: propority of this column items list
 * return: hash table that holds all items
 * */
void * create_diction_box(pItem src_item ,pSegment psg);


/*
 * funtion: extract items from hashtable to build a tmp diction table, ecah diction store with the form of Item
 * args-table: hash table that contains all of the diction
 * args-_t_dic: address of a tmp diction, 
 * return: OK will be return if success, or ERR
 * */
int extract_items_from_table(pHashTable table,ptTDic _t_dic);



/*
 * funtion: compress the specific column 
 * ags-src_item: address of item list
 * ags-psg: properties of specific column
 * return: address of tDiction contains compressed data, including diction table and index list
 * */
void * diction_compression_method_v2(pItem src_item ,pSegment psg );


int diction_compression_method_dispatch(void * pdata);


/*
 * funtion: get a record by given a index
 * args-index:given a row  index, funtion will return a res
 * return:OK will be return if get target index
 * */
int search_by_rows(const int index , void * res ,ptDiction pdic );


int print_diction(void* pdata);
int print_pkcolumn_for_diction(void* pdata);

/*
 * funtion:get one record in specific task
 * args-pdata:address of task
 * args-id: key number
 * args-id:return string,all result will be return with a string formate
 * return: OK  will be return if success ,or ERR  
 * */
int search_DTtask_record(void * pdata, int id, char * _line);


/*
 * funtion: destroy a diction ,including its diction table and diction index list
 * args-diction: address of specific column diction 
 * return: OK will be return if funtion success, or ERR
 * */
int destroy_column_diction(ptDiction diction);
/*soma macro*/
#define GDFT_CASE(Type,dic_len,tmp_dic,real_dic,v)\
({\
		Type * list =NULL;\
        if(v){\
			list = DBcalloc(dic_len,sizeof(Type));\
		}else{\
			list = calloc(dic_len,sizeof(Type));\
		}\
		real_dic->pdic  = (unsigned long)list;\
		pItem item_list = (pItem)(tmp_dic->list);\
		Type * value    = NULL;\
		int i           = 0;\
		if(!item_list) {\
			debug("tmp_dic->list is null");\
			return ;\
		}\
		for( i = 0 ; i < dic_len ; i++ ) {\
				if(item_list[i].pdata){\
					value = (Type*)(item_list[i].pdata);\
					memcpy(&list[i],value,sizeof(Type));\
				}\
				else{\
					debug("invalid diction in tmp_diction");\
					return ;\
				}\
		}\
		break ;\
})



#define PD_CASE(Type,addr,len,format,iterator)\
({\
	Type * table_list = (Type *)(addr) ;\
	for(iterator =0 ; iterator < len ; iterator++ ){\
		printf(format,table_list[iterator]);\
	}\
	break ;\
})\
	

/*case of funtion  get_one_record */ 
#define GOR_CASE(Type,addr,line,format,pos)\
({\
    Type * table = (Type*)(addr);\
	sprintf(line,format,table[pos]);\
	break ;\
})
#endif
