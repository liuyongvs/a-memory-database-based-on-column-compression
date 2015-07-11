#ifndef __COMMON_H__
#define __COMMON_H__


/*
 * 文件作用：测试中使用的宏定义
 * 作者：lanhxg
 * 时间：20130818
 * */
/*
 * 压缩选项 
 * */

#define COLUMN_COMPRESSION 1
#define DICTIONARY_COMPRESSION 2 
#define BITMAP_COMPRESSION 3 

/* 内存测试*/
//内存统计结果输出到指定的文件
#define MEM_TEST_FILE "./out/MEM_USED.TXT"  

/*文件属性（按列）*/
//文件中的每一列的属性输出到指定文件
#define EACH_COLUMN_ATTRIBUTE_FILE "./out/EACH_COLUMN_ATTRIBUTE.TXT"


/*存取时间测试*/
//多次测试的次数
#define ACCESS_LIMITS 100000
#define ACCESS_TIME_CONSUME_FILE "./out/ACCESS_TIME_CONSUME.TXT"


/*压缩态文件导出*/
#define CFCOMPRESSED_FILE "./out/DATA.CF"
#define DFCOMPRESSED_FILE "./out/DATA.DF"
#define BFCOMPRESSED_FILE "./out/DATA.BF"

/*压缩时间测试导出文件*/
#define CFCOMPRESSING_TIMEUSED_FILE "./out/COMPRESS_TIME.TXT"

/**/
#define CFWRITING_TIMEUSED_FILE "./out/WRITING_TIME.TXT"

#endif
