#include<stdio.h>
#include"csv.h"
#include"CF.h"
#include"DF.h"
#include"BF.h"

int main(void)
{
	//show counted csv column atrributes in txt file
	ShowFileColumnsAttribute(); 
	//test the cf file's access performance 
	{
		int i =0;
		for(i=0 ; i < 10 ; i++)
		{
			//TestAccessPerformanceInCF();
			//v2 adds pnext
			TestAccessPerformanceINCF_v2();
		}
	}
	
	//show memory consumed and rate of cf file
	ShowCFCompressedFile();
	//ReadLineFromCF();
	
	//ShowCFFileMemoryUsed();
	//TestCFCompressedTimeConsumed();
	//TestAppendRowsIntoCF();
	//TestAppendRowsIntoCF_v2();
	//TestReadCSVFilleTimeConsume();
	//ShowMemCSVFile();
}

