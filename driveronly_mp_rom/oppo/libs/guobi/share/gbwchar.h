#ifndef __MY_WCHAR__
#define __MY_WCHAR__
#if !defined(WIN32) && !defined(_WIN32_WCE) 

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
#ifndef _gb_far
#ifdef _C166
#define _gb_far _huge
#else
#define _gb_far
#endif
#endif

unsigned short _gb_far *  gbwcscat(unsigned short _gb_far *, const unsigned short _gb_far * );
#define wcscat  gbwcscat
unsigned short _gb_far *  gbwcschr(const unsigned short _gb_far * , unsigned short);
#define wcschr  gbwcschr
int  gbwcscmp(const unsigned short _gb_far * , const unsigned short _gb_far * );
#define wcscmp  gbwcscmp
unsigned short _gb_far *  gbwcscpy(unsigned short _gb_far * , const unsigned short _gb_far * );
#define wcscpy  gbwcscpy
unsigned int  gbwcscspn(const unsigned short _gb_far * , const unsigned short _gb_far * );
#define wcscspn gbwcscspn
unsigned int  gbwcslen(const unsigned short _gb_far * );
#define wcslen  gbwcslen
int  gbwcsncmp(const unsigned short _gb_far * , const unsigned short _gb_far * , unsigned int);
#define wcsncmp gbwcsncmp
unsigned short _gb_far *  gbwcsncpy(unsigned short _gb_far * , const unsigned short _gb_far * , unsigned int);
#define wcsncpy gbwcsncpy
unsigned int  gbwcsspn(const unsigned short _gb_far * , const unsigned short _gb_far * );
#define wcsspn  gbwcsspn
unsigned short _gb_far * gbwcsncat(unsigned short _gb_far * s1, const unsigned short _gb_far * s2, unsigned int n);
#define	wcsncat gbwcsncat
unsigned short _gb_far * gbwcsrev (unsigned short _gb_far * string);
#define wcsrev	gbwcsrev
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //WIN32
#endif //__MY_WCHAR__