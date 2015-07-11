
/*
 * author:lanhxg
 * date:20140504
 * funtion: bit vector compress method
 * */

#ifndef __B_VECTOR_H__
#define __B_VECTOR_H__

#include"lib/common.h"
typedef struct tBDiction
{
	DBtype type ;


	/*length of diction table*/
	int len ; 

	/*get row number of this column  == length of vector*/
	int vlen ;


	/*address of diction table*/
	unsigned long pdic ;


	/*address of map list of diction */
	unsigned char** vlist   ;
}tBDiction,*pBDiction;


int bvector_compression_method_dispatch(void * pdata);


/*
 *  funtion: print diction table and vector content of BDiction
 *  args-pdata:address of BDiction
 *  return:OK will be return if successfully,or ERR
 * */
int	print_bvector(void * pdata);



/*
 * funtion:get one task record by key value
 * args-pdata:address of bvector task
 * args-in: key value
 * args-_line:string format result
 * return:OK if funtion execute successfully.
 * */
int search_VBtask_record(void * pdata,int id, char * _line);

#define CDTB_CASE(Type,from,to,dlen)\
({\
	const Type * src   = (Type*)(from->pdic) ;\
	if(!src){\
		debug("warning:tDiction field was polluted");\
		return (void*)NULL;\
	}\
	Type * dst         = DBcalloc(dlen,sizeof(Type));\
	memcpy(dst,src,sizeof(Type)*dlen);\
	to->pdic = (unsigned long) dst ; \
    break ;\
})


#define PB_CASE(Type,addr,len,format,iterator)\
({\
	Type * table_list = (Type *)(addr) ;\
	for(iterator =0 ; iterator < len ; iterator++ ){\
		printf(format,table_list[iterator]);\
	}\
	break ;\
})\
/*case of funtion  get_field_record */ 
#define GFR_CASE(Type,addr,line,format,pos)\
({\
    Type * table = (Type*)(addr);\
	sprintf(line,format,table[pos]);\
	break ;\
})

#endif
