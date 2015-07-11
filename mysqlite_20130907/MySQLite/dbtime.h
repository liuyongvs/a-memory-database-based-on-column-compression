/*
 * AUTHOR:lanhxg
 * DATE  :20131006
 * FUNTION: define time consuming funtion
 * */

#ifndef __DBTIME_H__
#define __DBTIME_H__
/*
 * FUNTION : give a output filename and start to count the time used
 * argument-fileName  :the output file name ,if given NULL ,it would used default output name "DBTIME.TIME"
 
 * */
void DBtime_StartCount(void * fileName);

/*
 * FUNTION:  end to count the time consuming and out put the time used to a outfile file 
 * */
void DBtime_EndCountAndShow();

#endif  
