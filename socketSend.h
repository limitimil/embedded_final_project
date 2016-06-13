#ifndef __SOCKETSEND__H__
#define __SOCKETSEND__H__
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
void socket_prepare(int, const char*[]);
void sendMat(cv::Mat& img);
void socket_stop();
void get_letter_20(char* );
#endif 
