/****************************************************/
/*                                                  */
/*                      v4lgrab.h                   */
/*                                                  */
/****************************************************/


#ifndef __V4LGRAB_H__
#define __V4LGRAB_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <linux/fb.h>

#define  FREE(x)    if((x)){free((x));(x)=NULL;}

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

/**/
#pragma pack(1)

typedef struct tagBITMAPFILEHEADER{
    WORD    bfType;            // the flag of bmp, value is "BM"
    DWORD   bfSize;            // size BMP file ,unit is bytes
    DWORD   bfReserved;        // 0
    DWORD   bfOffBits;         // must be 54

}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    DWORD   biSize;            // must be 0x28
    DWORD   biWidth;           //
    DWORD   biHeight;          //
    WORD    biPlanes;          // must be 1
    WORD    biBitCount;        //
    DWORD   biCompression;     //
    DWORD   biSizeImage;       //
    DWORD   biXPelsPerMeter;   //
    DWORD   biYPelsPerMeter;   //
    DWORD   biClrUsed;         //
    DWORD   biClrImportant;    //
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD{
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
}RGBQUAD;


#endif //__V4LGRAB_H___
