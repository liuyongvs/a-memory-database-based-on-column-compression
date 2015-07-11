#include"bitop.h"
#include"common.h"


int set_bit(void * map, int index, int val, int LIMIT)
{
	if(!map || index<0 || index>=LIMIT)
	{
		debug("parameter errror");
		return ERR ;
	}
	Byte * _map = (Byte*)map;
	if(val<=0)
	  _map[index/8] &= ~(1<<(index%8));
	else
	  _map[index/8] |= (1<<(index%8));
	return OK ;
}

int check_bit(void* map, int index, int  LIMIT)
{
	if(!map || index<0 || index>=LIMIT ) {
		debug("parameter error:index=%d  limit=%d",index,LIMIT);
		return ERR ;
	}
	Byte * _map=(Byte*)map;
	Byte set = _map[index/8];
	set>>=(index%8);
	return (set&0x01) == 0x01 ? 1:0 ;
}

int print_bits(void* map,int len)
{
	if(!map || len<0 ) {
		debug("parameter error:map=0x%x len=%d",map,len);
		return ERR ;
	}
	Byte * vector = (Byte*)map;
	int i =0 ; 
	for( i =0 ; i < len ; i++){
			printf("%d",check_bit(vector,i,len));
	}
	return OK;
}
