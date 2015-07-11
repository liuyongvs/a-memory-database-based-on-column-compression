#ifndef __DEBUG_H__
#define __DEBUG_H__



//output properties of each column of memory file
#define DEBUG_PROPERTIES 0

//output memery file with the format of csv
#define DEBUG_MFILE 1

//output compress method of each column 
#define DEBUG_METHOD 0

//output size of mememry consume ,and DEBUG_STRUCT must be true
#define DEBUG_STRUCT 1
#define DEBUG_MEMORY_COUNTER 0

//test append records
#define DEBUG_APPRECORDS 0
#define DEBUG_APPEND_COUNTER 0
//APPEND_TIMES will be append
#define APPEND_TIMES 100000
#define APPEND_ACCESS_TIMES 100000

//output the access time consume
#define DEBUG_ACCESS_FIX     0
#define DEBUG_ACCESS_COUNTER 0
//if need to test multithread , DEBUG_ACCESS_COUNTER must be set to true
#define DEBUG_ACCESS_THREAD  0    
#define ACCESS_TIMES 100000


//out put RL compressed unit
#define DEBUG_RL_PRINT 0
#endif
