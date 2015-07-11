

/*
 * author : lanhxg
 * data   : 20140527
 * funtion: append table 
 * */
#ifndef __APPTABLE_H__
#define __APPTABLE_H__
#include "rbtree.h"

#define MAX_RECORDS  10

typedef struct APPTable{
	void * _root ;  /* pointer of root of rb tree,
					   this struct contains:
					   1.  void *pcurrent  ;  address of latest insert node 
					   2.  int  cols       ;  column number of table      
					   3.  int records     ;  
					   4.  struct mynode * node;
					   
					   */
	char name[32];   //name of append table, and it must be  consider with compressed table
	int bid      ;   //base id of apptable
	int lines    ;   //storage size of append table 
	int cols     ;   //column number of this table
	int subrows  ;   //row number of subtable
	int ncnt     ;   //nodes counter
	void* proper ;   //type list of table [pKColumn]
	int (*func_destroy_apptable)(struct APPTable*);
	int (*func_append_record)(char *,struct APPTable*);
	int (*func_searchAPP_record)(const int, char*,struct APPTable*);
	int (*func_get_apptable_records)(struct APPTable*);
}tAPPTable,*pAPPTable;

typedef struct mynode {
  	struct rb_node node;
  	char *string;  

	int from;
	int to;
	void* pdata;      //pointer of  a subtable
}Node, *pNode;



pAPPTable create_apptable(const int cols,const int srow ,const int bid, const char * tablename, void*proper );
#endif
