#include"./lib/dbtime.h"
#include"./lib/dbmem.h"
#include"./lib/common.h"
#include"./lib/bitop.h"
#include"csv.h"
#include<stdio.h>
#include"dbsystem.h"


//test funtion ImportCsv2Mem
int main()
{
	pMCsvFile  mfile = NULL ;
	pSegArray psa ;

	mfile = csvfile_loader("log");

#if 1
	DoSchedule(mfile);
	//BuildFileProperties(psa,mfile);
	
	//ptable=(pDBTable)CreateDBTable("table_v1" ,33);
	//DoAssignment(ptable,psa,mfile->colarray,33);
	//PrintColumn(mfile,27);
#else
	//BuildFileProperties(psa,mfile);
	//PrintMemCvsFile(mfile);
	//TestRunLengthCompressionMethod(mfile,psa);
#endif
	destroy_mcsvfile(mfile);
	return 0;
}
