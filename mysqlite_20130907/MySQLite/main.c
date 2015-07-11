#include"dbtime.h"
#include"dbmem.h"
#include<stdio.h>

int main()
{
	DBtime_StartCount("a");
	int i = 0 ;
	for( i = 0 ; i < 100000000 ; i++);
	DBtime_EndCountAndShow();

	DBmem_StartCount("b");
	DBcalloc(1,sizeof(int));
	DBmem_EndCountAndShow("test");


}
