/*
 * DATE:20140123
 * AUTHOR:lanhxg
 * FUNTION:measure csv file
 * */
#ifndef __CSV_H__
#define __CSV_H__

#include"lib/common.h"
#define DEFAULT_CSV_FILENAME "sample.csv"
#define COLUMN_LEN 20000
#define ROW_LEN 1000

/*
 * 列属性结构体
 * 例如十个items的例子：eg:1,1,1,2,2,3,4,5,6,7
*/
typedef struct Segment
{
	/*该段中包含的item数目---eg:10*/
	unsigned int rows ; 

	/*该段中包含相同值的item(必须两个以上的值，并且不同的) ---eg:2*/
	unsigned int same_items ;

	/*该段中包含相同值的item总数目(两个以上的值)----eg:3+2=5*/
	unsigned int all_same_items;

	/*该段中包含不同值的数目----eg:'3' '4' '5' '6' '7' =5*/
	unsigned int diff_items;

	/*该段中所有不相同的数目*/
	unsigned int all_diff_items;

	/*该段中包含空值的数目---eg:0*/
	unsigned int null_items;

	/*该段中相同值连续的平均数目----(all_same_items/same_items)---eg:5/2*/
	unsigned int avrg_items;

	/*该段中各个item的平均长度------eg:rows/(same_items+diff_items)= 10/(2+5)*/
	unsigned int len_item;

	/*数据的长度(1)，如果是字符串那么应该是字串的长度,*/
	/*数据的字节长度=len*sizeof(type)*/
	unsigned int len ;

	/*该段所属的数据类型*/
	DBtype type;

	char name[32];
}Segment,*pSegment;
typedef pSegment pSegArray[1024];

typedef struct Item
{
	//void * pdata ;
	unsigned long pdata ;

	/* if len equal to 0  : it means a null
	 *                 1  : 
	 *                >1  : it means a array
	 * */
	long len;
	DBtype type;
}Item, *pItem;
typedef pItem pItemArray[1024];

/*end of common data type*/

/*hash table*/
typedef struct HashNode
{
	/*第一个出现的值*/
	pItem pitem ;
	/*该值重复出现的长度*/
	unsigned int repeat_items ;
	/*该值有几个连续的段*/
	unsigned int times ;
	
	struct HashNode * pnext ;
}HashNode,*pHashNode;

typedef struct HashTable
{
	/*the length of hash table */
	unsigned int len ;
	
	/*the max sizeof of the data,measure by byte*/
	unsigned int max_len ; 
	
	pItem preInsertItem ;
	pItem ppreInsertItem ;
	//pItem ppreInsertItem ;
	/*该段中包含的item数目*/
	unsigned int rows ; 
	/*该段中包含空值的数目*/
	unsigned int null_items;
	unsigned int diff ; 

	unsigned int all_diff;

	/*node number in this hash table*/
	unsigned int nodes ;

	DBtype type ;
	/*the pointer of the hash table */
	pHashNode hasharray[1024];
}HashTable,*pHashTable;
/*end of hash table*/

/*mcsvfile define*/


typedef struct MCsvFile
{
	CAttrtList attlist ;

	//contain each column,the column number must less than 1024
	pItemArray colarray ; 
	pItem memcsv;

	//the array length
	int len ;

	//column has it's row number
	int rows ;
}MCsvFile, *pMCsvFile;

/*funtion declare */


int csvfile_parse(FILE * stream,CAttrtList list,int * frow, int *fcol );

/*slim white space from a strimg*/
void mytrim(char * dst , char *src );

/*
 * funtion:copy dst form src
 * return:if successful,it will return OK 
 * */
int DateCpy(Date* dst ,Date* src) ;
/*
 * funtion:compare dst with src
 * return :if equal, it will return 0 or it will return ERR
 * */
int cmpDate(Date* dst ,Date *src) ;
int DateTimeCpy(DateTime* dst ,DateTime* src);
int cmpDateTime(DateTime* dst ,DateTime* src);









/*
 * funtion:clone a item ,this funtion will malloc a new item,and malloc it's pdata ,and copy all the data form psrc 
 * args-psrc:address of source item 
 * return:the new item
 * */
pItem CloneItem(pItem psrc);
/*
 * funtion: clone a item from source item to dest item, but this 
 *          funtion just only malloc space pointed by pdata
 * args-dst:the dstination of item
 * args-src:the source of item 
 * return:OK is return if success, or ERR
 * */
int item_clonev2(pItem dst,pItem src);
/*
 *
 * funtion:compare two item
 * return: return 1 if two items equal,or else 0
 * */
int CompareItems(pItem pitem1 , pItem pitem2);
/*
 * funtion: map a item into interger
 * */
int MapItem2Int(Item item ,unsigned int threshold);


/*
 * funtion: create hashtable for each table column
 * return: if fails,it will return ERR
 * */
pHashTable CreateHashTable(int length);


/*destroy a hash table*/
int DestroyHashTable(pHashTable pt);

/*
 * funtion:add a new item into a hash table ,and after that we will get updated segs's related information
 * args-item:the new item that will be insert into specific hash table
 * return:if fails ,it will return err
 * */
int add_item2hashtable_v2(pItem pitem ,pHashTable ptable);
int AddItem2HashTable(pItem pitem ,pHashTable ptable);

/*
 *funtion:extraction of other properties from hash table
 *args-ptable:the hashtable prepared pointer
 *args-pseg:the seg that contains all the necessary attributes
 *return:if fails, it will return err
 * */
int ExtractPropertiesFromHashTable(pHashTable ptable,pSegment pseg);


/*
 * funtion:scan each column of the csv file ,and extract properties of each column
 * args-psa:the array of pSegments, each pSegment contains column's neccessary properties 
 * return:if fails, return err
 * */
int BuildFileProperties(pSegArray psa,pMCsvFile mfile); 
/*
 * funtion:print the member of the struct Segment to output stream
 * args-dsc:additional description infomation added by usr
 * args-fp: the output stream 
 * args-psg:the pointer of the struct Segment that contain relative colomn properites
 * return:if fails,return err
 * */
int print_properties(char * dsc , void * fp , pSegment psg);

int print_mfile_properties(pSegArray list,int len);


/*
 * funtion: given a type output it descri
 * args-type : the type enum value
 * return: OK if success
 * */
int Debug4Type(DBtype type, char * typename);

/*
 * funtion: convert varchar_type or char_type whose size>1 to string_type 
 * args-mfile:the memory csv file
 * no return value
 * */
void FixDataType(pMCsvFile mfile);

void AttributeParse(char * line ,int n , CAttrtList alist);

/*parse*/
int cvs_parse(char * line ,  char list[][1024] );

/*
 * funtion: import the format csv file into the memery
 * args-filename: name of csv file 
 * return: the point handler of the csv file
 * */
pMCsvFile csvfile_loader(const char * filename);
/*
 * funtion: free all the memory of memery csv file 
 * args-mcsvfile: the point handler of the csv file
 * */
int destroy_mcsvfile(pMCsvFile mcsvfile);

int PrintMemCvsFile(pMCsvFile mfile);



/*
 * funtion:获取csv文件的行和列
 * args-col:返回的列值
 * args-row:返回的行值
 * return:如果失败，回ERR ；如果成功，OK 
*/
int GetFileColumnAndRow(int * col,int *row ,pMCsvFile mfile );

/*
 * funtion:get the specific colomn addr and its length
 * args-len_dep:the column's lenght ,it means the row nunber
 * args-index:the index of column address 
 * return:the addr of the column specificed by index
*/

pItem GetSpecifiedColumn(int *len_dep,int index,pMCsvFile mfile);

int print_item(Item it,char*split);
int print_items(pItem list,int len,char*split);


/*
 * funtion: set all the field of Item by given type and src string
 * args-dst: address of Item
 * args-type: type of Item needed to be set
 * args-string: string contains the Item's pdata filed 's data
 * */
int SetItem(pItem dst,DBtype type,char * string);

int GetItemDscr(pItem dst,char *dscr);
/*end of funtion declare*/

#define CLONEPDATA_FOR_CASE(pdata,datatype,len)\
({\
		(pdata) = (unsigned long)calloc(len,sizeof(datatype));\
		len=len*sizeof(datatype);\
		break ;\
})\


#define SI_CASE(Type,src,format,item)\
({\
	item->len         = 1 ; \
	Type *data  = DBcalloc(1,sizeof(Type));\
	sscanf(src,format,data);\
	item->pdata = (unsigned long)data;\
	break ;\
})


#endif
