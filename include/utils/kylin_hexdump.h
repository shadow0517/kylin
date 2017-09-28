#ifndef _KYLIN_HEXDUMP_H_
#define _KYLIN_HEXDUMP_H_

/*
 * kylin/lib/core/utils/kylin_hexdump.c
 */

/**
* Dump out memory in a special hex dump format.
*
* @param f
*		A pointer to a file for output
* @param title
*		If not NULL this string is printed as a header to the output.
* @param buf
*		This is the buffer address to print out.
* @param len
*		The number of bytes to dump out
* @return
*		None.
*/

extern void kylin_hexdump(FILE *f, const char * title, const void * buf, unsigned int len);

/**
* Dump out memory in a hex format with colons between bytes.
*
* @param f
*		A pointer to a file for output
* @param title
*		If not NULL this string is printed as a header to the output.
* @param buf
*		This is the buffer address to print out.
* @param len
*		The number of bytes to dump out
* @return
*		None.
*/

extern void kylin_memdump(FILE *f, const char * title, const void * buf, unsigned int len);

#endif /*_KYLIN_HEXDUMP_H_*/
