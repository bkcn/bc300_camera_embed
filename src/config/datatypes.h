#ifndef  __DATATYPE_H
#define  __DATATYPE_H   


#define MHZ       *1000000UL
#define KHZ       *1000UL
#define HZ        *1UL


/***********************************************************************
 ** 数据类型声明
 **********************************************************************/
#ifndef true
#define true     (1 == 1)
#endif

#ifndef false
#define false    (1 == 0)
#endif

#ifndef TRUE
#define TRUE     (1 == 1)
#endif

#ifndef FALSE
#define FALSE    (1 == 0)
#endif

#ifndef SUCCESS
#define SUCCESS  (1 == 1)
#endif

#ifndef FAIL
#define FAIL     (1 == 0)
#endif

#ifndef ERROR
#define ERROR (-1)
#endif

#ifndef OK
#define OK    (0)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef INT32U
typedef unsigned int        INT32U;   // Unsigned 32 bit quantity
#endif

#ifndef INT32S
typedef signed   int        INT32S;   // Signed   32 bit quantity
#endif

#ifndef INT16U
typedef unsigned short int  INT16U;   // Unsigned 16 bit quantity
#endif

#ifndef INT16S
typedef signed   short int  INT16S;   // Signed   16 bit quantity
#endif

#ifndef INT8U
typedef unsigned char       INT8U;    // Unsigned  8 bit quantity
#endif

#ifndef INT8S
typedef signed   char       INT8S;    // Signed    8 bit quantity
#endif


#endif  // __DATATYPE_H


