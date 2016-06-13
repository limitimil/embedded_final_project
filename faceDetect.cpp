//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <errno.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <sstream>


#include "cv.h"
#include "cxcore.h"
#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"

#include "step.h"
using namespace cv;
using namespace cv::face;
using namespace std;

typedef struct result
{
	double angle;
	bool stranger;
};

Ptr<LBPHFaceRecognizer> model;

void training();
result returnResult( IplImage* );
double returnAngle( CvRect*, IplImage* );
bool returnStranger( CvRect*, IplImage* );

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(Error::StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

int facedetect_main(Mat& m)
{
//	training();

	result rst;
	
	//load img
	IplImage image_detect = IplImage(m);

	rst = returnResult( &image_detect );
	printf( "angle = %lf\n", rst.angle );
	step_degree(int(125* rst.angle));
	return rst.stranger;
}


//資料訓練(到時候再找方法刪除)
void training()
{
	string fn_csv = "database.csv";
	vector<Mat> images;
    vector<int> labels;
	try 
	{
		read_csv(fn_csv, images, labels);
	} 
	catch (cv::Exception& e) 
	{
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		// nothing more we can do
		exit(1);
	}
	// Quit if there are not enough images for this demo.
	if(images.size() <= 1) {
		string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
		CV_Error(Error::StsError, error_message);
	}
    model = createLBPHFaceRecognizer();
    model->train(images, labels);
}



//獲取畫面中所有人臉->回傳是否移動馬達, 有無陌生人
result returnResult( IplImage *image_detect )
{
	result rst;
	rst.angle = 0;
	rst.stranger = 0;
	
	//load cascade 
	string cascade_name="haarcascade_frontalface_alt.xml";
	CvHaarClassifierCascade *classifier=(CvHaarClassifierCascade*)cvLoad(cascade_name.c_str(),0,0,0);
	if(!classifier)
	{ 
		printf("error to load classifier cascade!\n");
		return rst;  
	}

	//create mem 
	CvMemStorage *facesMemStorage=cvCreateMemStorage(0);

	//initial mem
	cvClearMemStorage(facesMemStorage);

	//detect face
	CvSeq* face=cvHaarDetectObjects( image_detect, classifier, facesMemStorage,1.1,3, CV_HAAR_DO_CANNY_PRUNING, cvSize(50,50) );

	if( face -> total > 0 )
	{
		int i = 0;
		for(i = 0; i<face->total; i++)
		{
			//取得人臉範圍
			CvRect *rec=(CvRect*)cvGetSeqElem( face,i );
	
			rst.angle += returnAngle( rec, image_detect );
			//rst.stranger = returnStranger( rec, image_detect );	
		}
		rst.angle /= face -> total;
	}

   return rst;
}


//回傳馬達角度( -1 ~ 1 )
double returnAngle( CvRect *rec, IplImage *image )
{
	double angle = 0;
	double x = rec->x + (double)rec->width / 2;

	angle = ( x / (double)image->width ) - 0.5;
	angle *= 2;

	return angle;
}


//回傳是否為陌生人( T or F )
bool returnStranger( CvRect *rec, IplImage *image )
{ 
	//裁切圖片
	cvSetImageROI( image, cvRect( rec->x, rec->y, rec->width, rec->height ) );

	//轉成Mat
	Mat mat = cvarrToMat(image);
	imshow("A_A", mat);
	waitKey(0);

	//判別是否為陌生人
	int predictedLabel = -1;
	predictedLabel = model->predict(mat);
	printf( "%d\n", predictedLabel );

	//還原圖片
	cvResetImageROI(image);  
	return true;
}
