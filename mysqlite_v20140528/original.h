/*
 * author:lanhxg
 * date:20140321
 * funtion:keep original column data
 * */
#ifndef __ORIGINAL_H__
#define __ORIGINAL_H__

int OriginalDispatch(void*pdata);

int search_ORtask_record(void * pdata , int id ,char * _line);


#define MakeColumn(dst,src,type,len) \
({ \
  int i; \
  dst = (type*)DBcalloc(len,sizeof(type)); \
  for(i=0;i<len;i++)  \
       if(src[i].len!=0 && src[i].pdata!=0) \
			dst[i]=*(type*)(src[i].pdata); \
 })

#define SFR_CASE(Type,addr,line,format,id)\
({\
	Type *datalist  = (Type*)addr;\
	sprintf(line,format,datalist[id]);\
	break;\
})

#endif

