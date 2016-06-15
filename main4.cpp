#include <stdio.h>
#include "step.h"
#include "socketSend.h"
#include "v4l2.h"
#include "faceDetect.h"

#include <pthread.h>
#define IMAGEWIDTH 640
#define IMAGEHEIGHT 480
using namespace cv;
Mat makeMat(unsigned char* frame){
	Mat m= Mat::zeros(IMAGEHEIGHT,IMAGEWIDTH,CV_8UC3);
	for(int r=0;r< m.rows;r++)
	for(int c=0;c< m.cols;c++)
	{
		int location = (r* IMAGEWIDTH +c ) *3;
		m.at<Vec3b>(m.rows-r-1,c) = Vec3b(
			*(frame+location),*(frame+location + 1), *(frame + location +2)
		);
	}
	return m;
}
void* thread_routine(void* v){
	step_degree(*(int*)v);
	free((int*)v);
	return 0;
}
int main(int argc,const char** argv){
	//prepare webcam
	v4l2_prepare(16);
	//prepare socket
	socket_prepare(argc,argv); /*executable junk ip port*/
	//prepare step motor
	step_prepare();
	
	unsigned char* frame_buffer = get_frame_buffer();
	Mat img;
	char letter[20];

	pthread_t t;
	pthread_attr_t attr;
	while(1){
		v4l2_photo();
		img = makeMat(frame_buffer);
		sendMat(img);
		int* degree = (int*)malloc(sizeof(int));
		*degree = facedetect_main(img);		
		printf("detect result :%d\n",*degree);
		pthread_attr_init(&attr);
		pthread_create(&t,&attr,thread_routine,(void*) degree);
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
