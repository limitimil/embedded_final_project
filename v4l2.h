#ifndef __V4L2__H__
#define __V4L2__H__
#include "v4l2grab.h"
unsigned char* get_frame_buffer();
/****
return frame buffer pointor
we can use this pointer to extract image.
****/
void v4l2_prepare(int);
/****
QUERYCAP
ENUM_FMT
SET_FMT
GET_FMT

malloc buffer memory
mmap buffer
REQBUFS
QUERYBUF
QUEUE_BUFFER
STREAMON
****/
void v4l2_photo();
/***
dequeue buffer, transfer yuyv to rgb, then queue buffer again.
***/
void v4l2_stop();
/***
STREAM OFF
***/
#endif
