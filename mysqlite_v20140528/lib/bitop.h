/*
 * author: lanhxg
 * date:20140325
 * funtion:bit operation set
 * */

/*
 * funtion:set specific bit to 0 or 1 , val <=0 represent 0
 *                                      val>0   represent 1
 * args-map:the bit array
 * args-index:bit position needs to be set
 * args-val:set 0 or 1
 * args-LIMIT:the index must be less than LIMIT
 * */
#ifndef __BITOP_H_
#define __BITOP_H_
int set_bit(void * map, int index, int val, int LIMIT);



/*
 * funtion:checht specific bit is 1 or 0
 * args-map:bit array
 * args-index:bit position needs to check
 * args-LIMIT:index flied must be less than LIMIT
 * return if the specific bit is be set to 1/0, this funtion will return 1/0, if this funtion faild, it will return ERR(-1)
 * */
int check_bit(void* map, int index, int  LIMIT);

int print_bits(void* map,int len);
#endif

