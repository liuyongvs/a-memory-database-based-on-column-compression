#include<stdio.h>
int SearchPos(int * in , int l , int r , int * val);
int main()
{
	int datalist[10]={0,1,2,3,4,5,6,7,8,9};
	int val = 100 ;
        int pos = SearchPos(datalist,0,9,&val);
	printf("%d\n",pos);	
}
int SearchPos(int * in , int l , int r , int * val)
{
	if(r-l<0)
		return -1 ;
	int mid = (l+r)/2;
	
	if(in[mid]==*val)
		return mid ;
	
	if(in[mid]<*val)
		return SearchPos(in,mid+1,r,val);
	else
		return SearchPos(in,l,mid-1,val);

}
