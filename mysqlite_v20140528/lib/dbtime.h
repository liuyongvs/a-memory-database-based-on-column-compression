/*
 * AUTHOR:lanhxg
 * DATE  :20131006
 * FUNTION: define time consuming funtion
 * */

#ifndef __DBTIME_H__
#define __DBTIME_H__
/*
 * FUNTION : give a output filename and start to count the time used
 
 * */
extern void DBtime_StartCount();

/*
 * FUNTION:  end to count the time consuming and out put the time used to a outfile file 
 * argument-fileName  :the output file name ,if given NULL ,it would used default output name "dbtime.msg"
 * */
extern void DBtime_EndCountAndShow(char * filename ,char * descrip);


/*
 *  FUNTION:get n random int number,and all number must be less than mod
 *  args-n: len of the list
 *  args-list:address of a int list
 *  args-mod: mode number that all random value must be less than it
 * */
extern void random_list(int n ,int * list, int mod);

#endif  
