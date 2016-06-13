#include <stdio.h>
#include "step.h"
#include "socketSend.h"
#include "v4l2.h"
#include "faceDetect.h"

#define IMAGEWIDTH 640
#define IMAGEHEIGHT 480
using namespace cv;
Mat makeMat(unsigned char* frame){
	Mat m= Mat::zeros(IMAGEHEIGHT,IMAGEWIDTH,CV_8UC3);
	for(int r=0;r< m.rows;r++)
	for(int c=0;c< m.cols;c++)
	{
		int location = (r* IMAGEWIDTH +c ) *3;
		m.at<Vec3b>(r,c) = Vec3b(
			*(frame+location),*(frame+location + 1), *(frame + location +2)
		);
	}
	return m;
}
int main(int argc,const char** argv){
	//prepare webcam
	v4l2_prepare(16);
	//prepare socket
	socket_prepare(argc,argv); /*executable junk ip port*/
	//prepare step motor
	printf("what?\n");
	step_prepare();
	printf("what?\n");
	
	unsigned char* frame_buffer = get_frame_buffer();
	Mat img;
	char letter[20];
	while(1){
	printf("loop?\n");
		v4l2_photo();
	printf("loop?\n");
		img = makeMat(frame_buffer);
	printf("loop?\n");
		sendMat(img);
	printf("loop?\n");
		int rst = facedetect_main(img);		
	printf("loop?\n");
		printf("detect result :%d\n",rst);
		get_letter_20(letter);
		printf("%s\n", letter);//alert function
	}
	//prepare webcam
	v4l2_stop();
	//prepare socket
	socket_stop(); /*executable junk ip port*/
	//prepare step motor
	step_stop();
	
}
