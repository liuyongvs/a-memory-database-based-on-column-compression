/*
 * =============================================================================
 *
 *       Filename:  rbtree-tst.c
 *
 *    Description:  rbtree testcase.
 *
 *        Created:  05/26/2014 
 *
 *         Author:  JackyLau, liuyongvs@gmail.com
 *        Company:  USTC(University of Science and Technology)
 *
 * =============================================================================
 */

#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "apptable.h"
#include "csv.h"
#include "lib/common.h"
#include "lib/dbmem.h"


static int sub_cols ;
static int sub_rows ;
struct mynode * search(struct rb_root *root, char *string)
{
  	struct rb_node *node = root->rb_node;

  	while (node) {
  		struct mynode *data = container_of(node, struct mynode, node);
		int result;

		result = strcmp(string, data->string);

		if (result < 0)
  			node = node->rb_left;
		else if (result > 0)
  			node = node->rb_right;
		else
  			return data;
	}
	return NULL;
}

struct mynode * my_search(struct rb_root *root, int id)
{
	char str[10];
	sprintf(str, "%d", id);
	struct mynode *data=search(root,str);
	return data;

}


int insert(struct rb_root *root, struct mynode *data)
{

  	struct rb_node **new = &(root->rb_node), *parent = NULL;

  	/* Figure out where to put new node */
  	while (*new) {
  		struct mynode *this = container_of(*new, struct mynode, node);
  		int result = strcmp(data->string, this->string);

		parent = *new;
  		if (result < 0)
  			new = &((*new)->rb_left);
  		else if (result > 0)
  			new = &((*new)->rb_right);
  		else
  			return 0;
  	}

  	/* Add new node and rebalance tree. */
  	rb_link_node(&data->node, parent, new);
  	rb_insert_color(&data->node, root);

	return 1;
}

int my_insert(struct rb_root *root, int id)
{
	struct mynode *node;
	node= (struct mynode *)malloc(sizeof(struct mynode));
	node->string = (char *)malloc(sizeof(char) * 10);
	sprintf(node->string, "%d", id);		
	if( insert(root, node) )
	{
		root->pcurrent=node;
		node->from=root->records;
		node->to=node->from;
		return 1;
	}
	else
		return 0;
}


void my_free(struct mynode *node)
{
	if (node != NULL) 
	{
		if (node->string != NULL) 
		{
			free(node->string);
			node->string = NULL;
		}
		if (node->pdata != NULL) 
		{
		//Table *table=node->pdata;
		//DestroyTable(table);
		}
		free(node);
		node = NULL;
	}
}


void my_delete(struct rb_root *root, int id)
{
    struct mynode *data = my_search(root, id);
    if (!data) { 
	fprintf(stderr, "Not found %d.\n", id);
	return;
    }
    
    rb_erase(&data->node, root);
    my_free(data);
}

void my_printrbtree(struct rb_root *tree)
{
    struct rb_node *node;   
    for (node = rb_first(tree); node; node = rb_next(node))
	{
	if(rb_entry(node, struct mynode, node)->string==NULL)
	  return ;
    	printf("key = %s\n", rb_entry(node, struct mynode, node)->string);
	}
    printf("\n");
}
static int append_subtable(pAPPTable aptable)
{
	if(!aptable){
		debug("parameter error");
		return ERR;
	}
	const int scols        = aptable->cols;     //get field of column number of subtable
	const int srows        = aptable->subrows ; //get row number of subtable 
	const pKColumn proper  = aptable->proper;   //get properties of subtable
	const int ncnt         = aptable->ncnt;     //get node counter field
	struct rb_root* header = aptable->_root; 

	pItem  table = DBcalloc(scols*srows,sizeof(Item));

	int i =0 ,j=0 ;
	DBtype  type;

	/*inital each column's type*/
	for( i =0 ; i < scols ; i++ ){
		type  = proper[i].type ; 
		for( j =0 ; j < srows ; j ++ ){
			table[i*srows+j].type = type ; 
		}
	}

	if(!header){
		debug("bad field of rb root ");
		return ERR;
	}else{
		my_insert(header,ncnt);                 //add a new node to store a subtable
		aptable->ncnt++;                        //update node counter
		aptable->lines+=srows;                  //update space   
	}

	pNode current = header->pcurrent ;                  //get address of just insert node
	if(current){
		current->pdata = table ;                         //set  address of subtable
	}else{
		debug("fail in dispatch funtion:my_insert");
		return ERR;
	}

	return OK;
}

static int destroy_subtable(void* sbtable,const int cols, const int rows )
{
	if(!sbtable || cols <0 || rows<0 ){
		debug("parameter error");
		return ERR;
	}

	pItem table      = sbtable ;
	void * pdata ;
	if(table){
		int i = 0 ,len = rows*cols ; 
		for( i = 0 ; i < len ; i++ ){
			if(table[i].len!=0){
				if(table[i].pdata!=0UL){
					pdata = (void*)table[i].pdata ;
					DBfree(pdata);
					pdata =NULL;
					table[i].pdata=0UL;
				}
			}
		}
		DBfree(table);
	}else{
		debug("error: fail to free bad table field of sbtable ");
		return ERR ;
	}
	sbtable=NULL;
	return OK ;
}
void my_destroytree(struct rb_node *node)
{
  	if(node){
		my_destroytree(node->rb_left);
		my_destroytree(node->rb_right);	
  		struct mynode *data = container_of(node, struct mynode, node); 
		//added by lanhxg
		pItem table = data->pdata;                           //get subtbale
		destroy_subtable(table,sub_rows,sub_cols);     
		//endof add
		my_free(data);			
	}
	return;
}

int destroy_apptable(pAPPTable aptable)
{
	if(!aptable){
		debug("parameter error");
		return ERR;
	}

	struct rb_root * header = aptable->_root;
	if(header){
		    struct rb_node * rnode = header->rb_node;
			if(rnode){
				my_destroytree(rnode);
			}else{
				debug("bad rb_node field of header");
				return ERR;
			}
	}else{
		debug("bad rb tree found when destroy rb tree");
		return ERR;
	}

	memset(aptable,0,sizeof(tAPPTable));
	DBfree(aptable);
	return OK;
}

static int append_record(char *record , pAPPTable aptable)
{
	if(!record || !aptable){
		debug("parameter error");
		return ERR;
	}

	struct rb_root * header = aptable->_root;
	int space               = aptable->lines;             //get space lines 
	int cols                = aptable->cols;              //get column number 
	if(header){	
		int records = header->records ;                   //get curent full lines
		if(space == records){                             //need to append empty table
			append_subtable(aptable);
		}
	}else{
		debug("bad field of _root of apptable struct");
		return ERR;
	}

	/*start to insert record*/
	pNode current  = header->pcurrent; 
	pItem table    = current->pdata;
	const int crow = current->to - current->from ;       
	const int srow = aptable->subrows;

	/*parse string record into substring list */
	char sbslist[1024][1024];
	int colcnt = 0 ;
	int i ;
	int pos ;
	memset(sbslist,0,sizeof(sbslist));

	colcnt = cvs_parse(record,sbslist);
	if(colcnt ==  cols){
		for( i = 0 ; i < cols ; i++ ){
			pos = i*srow+crow;
			if(ERR== SetItem(&table[pos],table[pos].type,sbslist[i])){
				debug("error:set item error");
				return ERR ;
			}
		}
		current->to++ ;                       //update to field of current subtable
		header->records++;                    //update records field
	}else {
		debug("error: record fields can not match ");
		return ERR ;
	}
	return OK;
}
int searchAPP_record(const int id , char * _line,pAPPTable aptable )
{
	if(id < 0 || ! _line || !aptable){
		debug("parameter error");
		return ERR;
	}
	memset(_line,0,sizeof(_line));
	const int bid  = aptable->bid ; //get base id of this apptable 
	const int tid  = id -bid ;      //get true id of this apptable

	//first get the key id
	const int key  = tid/10;
	struct rb_root * header       = aptable->_root;
	const int cols                = aptable->cols;     //get column number of subtable
	const int rows                = aptable->subrows ; //get row    number of subtable
	const int records             = header->records  ; //get records store in apptable

	if(tid>=records){
		debug("error: search out of range");
		return ERR;
	}

	if(header){
		const pNode target = (const pNode)my_search(header,key);
		if(target){
			const pItem table = target->pdata;

			if(!table){
				debug("bad table field of target Node");
				return ERR;
			}
			
			int i     =0 ;
			int tid   =id%10;
			char string[100];
			for( i = 0 ; i < cols ; i++ ){
				GetItemDscr(&table[i*rows+tid],string);
				strcat(_line,string);
				if(i!=cols-1){
					strcat(_line,",");
				}else{
					strcat(_line,"\n");
				}
			}
		}else{
#if 0
			sprintf(_line,"id = %d, not find",id);
#else
			debug("fail to find id =%d ",id);
#endif
			return ERR;
		}

	}else{
		debug("error:bad field of root of apptable");
		return ERR;
	}
	return OK;
}
int get_apptable_records(pAPPTable aptable)
{
	if(!aptable){
		debug("parameter error");
		return ERR;
	}

	struct rb_root * header = aptable->_root;
	if(header){
		return header->records;
	}else{
		debug("error : bad root field of aptable");
		return ERR;
	}
}
pAPPTable create_apptable(const int cols,const int srow ,const int bid, const char * tablename, void*proper )
{
	struct rb_root *proot = DBcalloc(1,sizeof(struct rb_root));

	proot->pcurrent = NULL;
	proot->cols     = cols ;         //set cols field of this table
	proot->records  = 0    ;         //initial records field

	pAPPTable apptable = DBcalloc(1,sizeof(tAPPTable));
	apptable->bid     = bid ;       //set base id field
	apptable->_root   = proot;      //set root field of this table
	apptable->lines   = 0   ;       //set size filed of storage
	apptable->cols    = cols;       //set subtable column number
	apptable->subrows = srow;       //set row number of subtable   
	apptable->ncnt    = 0   ;       //set node  counter field
	apptable->proper  = proper;     //set proper field
	sprintf(apptable->name,"%s",tablename);

	sub_rows  = srow ;        // set grobal variable about row number of subtable
	sub_cols  = cols ;        // set grobal variable about col number of subtable
	
	/*  create first subtable for apptable */
	append_subtable(apptable);



	
	apptable->func_destroy_apptable       = destroy_apptable ;
	apptable->func_append_record          = append_record;
	apptable->func_searchAPP_record       = searchAPP_record;
	apptable->func_get_apptable_records   = get_apptable_records;

	return apptable;
}
