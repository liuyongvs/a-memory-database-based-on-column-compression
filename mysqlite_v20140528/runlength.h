/*
 * date:20140104
 * author:lanhxg
 * funtion: Compression algorithm of run length 
 * */
#ifndef __RUNLENGTH_H__
#define __RUNLENGTH_H__
#include "lib/common.h"
#include"csv.h"
#include"debug.h"
/*
 *  tag represent pnext avalable
 *  pos means the position of data in origin array
 *  pnext 
 *  data
 * */
#if DEBUG_STRUCT 
typedef struct Rbool_unit
{
	int tag ;
	int pos;
	int pnext ;
	bool data;
}Rbool_unit,*pRbool_unit;

typedef struct Rchar_unit
{
	int tag ;
	int pos;
	int pnext ;
	char data;
}Rchar_unit,*pRchar_unit;
/*char * or varchar **/
typedef struct Rstring_unit
{
	int tag ;
	int pos ;
	int pnext;
	char* data;
}Rstring_unit,*pRstring_unit;

typedef struct  Rint_unit
{
	int tag ;
	int pos ;
	int pnext;
	int data;
}Rint_unit,*pRint_unit ;

typedef struct Rfloat_unit
{
	int tag ;
	int pos ;
	int pnext;
	float data ;
}Rfloat_unit,*pRfloat_unit;

typedef struct Rdouble_unit
{
	int tag ;
	int pos ;
	int pnext;
	double data;
}Rdouble_unit,*pRdouble_unit;

typedef struct RDate_unit
{
	int tag ;
	int pos ;
	int pnext;
	Date data;
}RDate_unit,*pRDate_unit;

typedef struct RDateTime_unit
{
	int tag ;
	int pos ;
	int pnext;
	DateTime data ;
}RDateTime_unit,*pRDateTime_unit ;

typedef struct RInterger_unit
{
	int tag ;
	int pos ;
	int pnext;
	Interger data ;
} RInterger_unit ,*pRInterger_unit;

typedef struct RLong_unit
{
	int tag ;
	int pos ;
	int pnext;
	long data ;
}RLong_unit,*pRLong_unit ;

typedef struct RSingle_unit
{
	int tag ;
	int pos ;
	int pnext;
	Single data ;
}RSingle_unit,*pRSingle_unit;

typedef struct RByte_unit
{
	int tag ;
	int pos ;
	int pnext;
	Byte data ;
} RByte_unit , *pRByte_unit;

typedef struct Rvarchar_unit
{
	int tag ;
	int pos ;
	int pnext;
	varchar data ;
}Rvarchar_unit,*pRvarchar_unit;
#else
struct Rbool_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15;
	unsigned short pnext ;
	bool data;
} __attribute__((aligned(sizeof(bool))));
typedef struct Rbool_unit Rbool_unit ,*pRbool_unit;


struct Rchar_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15;
	unsigned short pnext ;
	char data;
}__attribute__((aligned(sizeof(char))));
typedef struct Rchar_unit Rchar_unit, *pRchar_unit;
/*char * or varchar **/
struct Rstring_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	char* data;
}__attribute__((aligned(sizeof(short))));
typedef struct Rstring_unit Rstring_unit,*pRstring_unit;

struct  Rint_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	int data;
}__attribute__((aligned(sizeof(short))));
typedef struct Rint_unit Rint_unit,*pRint_unit ;

struct Rfloat_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	float data ;
}__attribute__((aligned(sizeof(short))));
typedef struct Rfloat_unit Rfloat_unit,*pRfloat_unit;

struct Rdouble_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	double data;
}__attribute__((aligned(sizeof(short))));
typedef struct Rdouble_unit Rdouble_unit,*pRdouble_unit;

struct RDate_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	Date data;
}__attribute__((aligned(sizeof(short))));
typedef struct RDate_unit RDate_unit,*pRDate_unit;

struct RDateTime_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	DateTime data ;
}__attribute__((aligned(sizeof(short))));
typedef struct RDateTime_unit RDateTime_unit,*pRDateTime_unit ;

struct RInterger_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	Interger data ;
}__attribute__((aligned(sizeof(short))));
typedef struct RInterger_unit RInterger_unit, *pRInterger_unit;

struct RLong_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	long data ;
}__attribute__((aligned(sizeof(short))));
typedef  struct RLong_unit RLong_unit,*pRLong_unit ;

struct RSingle_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	Single data ;
}__attribute__((aligned(sizeof(short))));
typedef struct RSingle_unit RSingle_unit,*pRSingle_unit;

struct RByte_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	Byte data ;
}__attribute__((aligned(sizeof(short))));
typedef struct RByte_unit RByte_unit, *pRByte_unit;

struct Rvarchar_unit
{
	unsigned short tag:1 ;
	unsigned short pos:15 ;
	unsigned short pnext;
	varchar data ;
}__attribute__((aligned(sizeof(short))));
typedef struct Rvarchar_unit Rvarchar_unit,*pRvarchar_unit;
#endif


typedef struct nArray
{
	/*the length of the null array*/
	int len ;
	Byte *map; //bit list
}nArray,*pnArray;

typedef struct Pointer
{
	DBtype type ;
	union 
	{
		void * p ;
		pRbool_unit               bool_pt ;
		pRchar_unit               char_pt ;
		pRint_unit                int_pt ;
		pRfloat_unit              float_pt ;
		pRdouble_unit             double_pt ;
		pRDate_unit               Date_pt ;
		pRDateTime_unit           DateTime_pt ;
		pRInterger_unit           Interger_pt ;
		pRLong_unit               Long_pt ;
		pRSingle_unit             Single_pt ;
		pRByte_unit               Byte_pt ;
		pRvarchar_unit            varchar_pt ;
		pRstring_unit             string_pt ;
	}pt ;
}Pointer;

typedef Pointer PointerArray[1024] ;

typedef int PosArray[1024] ;


/*
 * fution: compression method of run length
 * args-psga: the array of psegments,each psegment represent a compressed column data
 * args-pit: the array of pItem ,each pItem represent a oringinal column 
 * args-len_ps:the compressed column number
 * return:the compressed column
 *
 * */
pKColSet RunLengthCompressionME(pSegArray psga ,pItemArray pita , int len_dep ,int len);


pKColSet RunLengthCompressionME_v2(pSegArray psga ,pItemArray pita,pKColumn colarray, int len_dep ,int len,const int *indexlist);

/*
 * funtion:print a sigle kcolumn units compressed by rl method
 * args-pkc:address of kcolumn
 * args-file:FILE * ,out put file descripter 
 * args-flag:means:file field is avalible
 * */
int print_kcolumn_units(pKColumn pkc ,void * file ,int flag);



/*
 * funtion: output all the data compressed by run-length compression method
 * filename: if you give a file name ,all the context will be print in this file, or stdout 
 * */
void PrintRLCFile(char* filename,pKColSet pkcs);

/*
 * funtion: check whether this is a null value on the oring position
 * args-pkg:pointer of the KColumn ,which point to the target column
 * args-index: index  of this column positon
 * return: if index position is null ,it return it's position index in the nullarray ,or it return -1
 * */
int CheckNull(pKColumn pkc , int index);

/*  
 *  funtion:output the  null array into a specific file
 *  args-pkg: the target column address
 *  args-file: file pointer where you wanto contain the output content
 *  args-type: if 1 ,it represent file is a file pointer  
 * */
int printNullArray(pKColumn pkc,void *file ,int type);


/*
 * funtion:runLength compression method funtion dispatch
 * args-pdata: mRunLenth  TaskPCB address
 * return:OK if success, or ERR
 * */
int RunLengthDispatch(void * pdata);


/*
 * funtion:search index specific row from runlength compressed data
 * args-pdata:address of task
 * args-index: row number specific by index field, count by 0
 * return: modify later
 * */
int SearchRLByRowNumber(void* pdata,int index);



/*
 * funtion:printf all the kcolumn info 
 * args-pkc:the address of specific kcolumn
 * return: OK if successfuly, or ERR will be return if any error occur 
 * */
int prin_kcolumn_info(pKColumn pkc);


/*
* funtion: search record form RL task , and the record 
*          will be return as the form of string that stored in'_line'
* args-pdata: address of RL task
* args-id : the primary key, also represent of the row number 
* args-_line: result of seach will be stored in this string,and each field
*             split by ',' ,like the format of csv file
* return:OK will  be return if funtion success, otherwise be ERR 
 * */
int search_RLtask_record(void * pdata , int id , char * _line);


#define SU_CASE(Type1,id,Type2,ulist,ilist,pos,next,tag)\
({\
    Type1 *_ulist= (Type1*)ulist;\
	Type1 *_unit = &(_ulist[id]) ;\
	Type2 *data = (Type2*)(ilist[pos].pdata);\
	_unit->data  = *data;\
	_unit->pos   = pos ;\
	_unit->pnext = next;\
	_unit->tag   = tag==0?0:1; \
	break ;\
})


#define SP_CASE(Type,ulist,from,to,id,current)\
({\
	Type* _ulist = (Type*)ulist;\
	int  mid = (from+to)/2;\
	if(mid == rows-1){\
		if(_ulist[mid].pos <= id && id<rows) return mid ;\
	}else{\
		if(_ulist[mid].pos<=id && id< _ulist[mid+1].pos) return mid;\
	}\
	if(_ulist[mid].pos> id){\
		return SearchPos(current,id,from,mid-1); \
	}else{\
		return SearchPos(current,id,mid+1,to);\
	}\
	break;\
})



#define GNR_CASE(Type,ulist,pos,rows,from,to)\
({\
	Type * _ulist = ulist;\
	*from = _ulist[pos].pnext;\
	if(pos==rows-1){\
		*to =-1;\
	}else{\
		*to = _ulist[pos+1].pnext;\
	}\
	break;\
})

#endif
