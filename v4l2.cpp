#include "v4l2grab.h"
#include "v4l2.h"
#define  TRUE	1
#define  FALSE	0

#define FILE_VIDEO 	"/dev/video0"
#define BMP      	"./image_bmp.bmp"
#define YUV			"./image_yuv.yuv"

#define  IMAGEWIDTH    640
#define  IMAGEHEIGHT   480
/*opencv includes*/
#include "cv.h"
#include "highgui.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "socketSend.h"
using namespace cv;
static  int     fd;
static  struct  v4l2_capability     cap;
        struct  v4l2_fmtdesc        fmtdesc;
        struct  v4l2_format         fmt,fmtack;
        struct  v4l2_streamparm     setfps;
        struct  v4l2_requestbuffers req;
        struct  v4l2_buffer         buf;
        enum    v4l2_buf_type       type;

unsigned char frame_buffer[IMAGEWIDTH*IMAGEHEIGHT*3];
BITMAPFILEHEADER bf;
BITMAPINFOHEADER bi;

struct buffer
{
	void * start;
	unsigned int length;
} *buffers;
/***lab2.3***/
int fbfd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
char *fbp = 0;
int x = 0, y = 0;
long int location = 0;
/************/
/****bonus***/
enum bonus{origin,left,right};
typedef enum bonus bonus;
bonus bonus_type=origin;
/************/
/**final*****/
int requestCount=4;
/************/

int init_v4l2(void)
{
	// 1. Open device////////////////////////////////////
	if ((fd = open(FILE_VIDEO, O_RDWR)) == -1)
	{
		printf("Error opening V4L interface\n");
		return  FALSE;
	}
    /////////////////////////////////////////////////////

    // 2. Device setup///////////////////////////////////
	// Query device capabilities
	if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
	{
		printf("Error opening device %s: unable to query device.\n",FILE_VIDEO);
		return  FALSE;
	}
	else
	{
     	printf("driver:\t\t%s\n"    , cap.driver);
     	printf("card:\t\t%s\n"      , cap.card);
     	printf("bus_info:\t%s\n"    , cap.bus_info);
     	printf("version:\t%d\n"     , cap.version);
     	printf("capabilities:\t%x\n", cap.capabilities);

     	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
			printf("Device %s: supports capture.\n",FILE_VIDEO);

		if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
			printf("Device %s: supports streaming.\n",FILE_VIDEO);
	}

	// Print support format
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("Support format:\n");
	while(ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
	{
		printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
		fmtdesc.index++;
	}

    // Set format
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.height = IMAGEHEIGHT;
	fmt.fmt.pix.width = IMAGEWIDTH;
	fmt.fmt.pix.field = V4L2_FIELD_BOTTOM;

    // Set the data format
	if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
	{
		printf("Unable to set format\n");
		return FALSE;
	}
    // Get the data format
	if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)
	{
		printf("Unable to get format\n");
		return FALSE;
	}
	{
     	printf("fmt.type:\t\t%d\n", fmt.type);
     	printf("pix.pixelformat:\t%c%c%c%c\n", fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF, (fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
     	printf("pix.height:\t\t%d\n", fmt.fmt.pix.height);
     	printf("pix.width:\t\t%d\n", fmt.fmt.pix.width);
     	printf("pix.field:\t\t%d\n", fmt.fmt.pix.field);
	}
	// Set fps
	setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	setfps.parm.capture.timeperframe.numerator = 10;
	setfps.parm.capture.timeperframe.denominator = 10;
    /////////////////////////////////////////////////////

	printf("init %s \t[OK]\n", FILE_VIDEO);

	return TRUE;
}

int v4l2_grab(void)
{
	unsigned int n_buffers;

    // 3. Memory map/////////////////////////////////////
	// Request for 16 buffers
	req.count  = requestCount;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
    // Initiate memory mapping or user pointer I/O
	if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1)
		perror("Request for buffers error:");

	// Memory allocate
	buffers = (struct buffer *)malloc(req.count * sizeof(*buffers));
	if (!buffers) {
		perror("Memory allocate error:");
		return FALSE;
	}

	for (n_buffers = 0; n_buffers < req.count; n_buffers++)
	{
		buf.type      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory    = V4L2_MEMORY_MMAP;
		buf.index     = n_buffers;
		// Query buffers
		if (ioctl (fd, VIDIOC_QUERYBUF, &buf) == -1) {
			perror("query buffer error\n");
			return FALSE;
		}

		buffers[n_buffers].length = buf.length;
		// Memory map
		buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ |PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
		if (buffers[n_buffers].start == MAP_FAILED) {
			printf("buffer map error\n");
			return FALSE;
		}
	}

	// Queue
	//for (n_buffers = 0; n_buffers < req.count; n_buffers++) {
	//	buf.index = n_buffers;
        // Enqueue an empty (capturing) or filled (output) buffer in the driver's incoming queue
	//	if (ioctl(fd, VIDIOC_QBUF, &buf))
        //    perror("Queue buffer error:");
	//}
    /////////////////////////////////////////////////////

    // 4. Start capturing////////////////////////////////
    // Start streaming I/O
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &type)) {
        perror("Start streaming error:");
        return FALSE;
    }
    /////////////////////////////////////////////////////

    // 5. Read buffer////////////////////////////////////
    // Dequeue
	//if (ioctl(fd, VIDIOC_DQBUF, &buf)) {
        //perror("Dequeue buffer error:");
        //return FALSE;
    //}
    /////////////////////////////////////////////////////

    printf("grab yuyv OK\n");

	return TRUE;
}

void yuyv_2_rgb888(void)
{
	unsigned int n_buffers;
    // 6. Image processing///////////////////////////////
	int           i, j;
    unsigned char y1, y2, u, v;
    int r1, g1, b1, r2, g2, b2;
    char *pointer;

	pointer = (char* )buffers[4].start;
    for(i = 0; i < IMAGEHEIGHT; i++)
    {
    	for(j = 0;j < (IMAGEWIDTH/2); j++)
    	{
    		y1 = *( pointer + (i*(IMAGEWIDTH/2)+j)*4);
    		u  = *( pointer + (i*(IMAGEWIDTH/2)+j)*4 + 1);
    		y2 = *( pointer + (i*(IMAGEWIDTH/2)+j)*4 + 2);
    		v  = *( pointer + (i*(IMAGEWIDTH/2)+j)*4 + 3);

               r1= y1 + 1.042*(v-128);
               g1= y1 - 0.344*(u-128) -0.714 *(v -128);
               b1= y1 + 1.772*(u-128);

               r2= y2 + 1.042*(v-128);
               g2= y2 - 0.344*(u-128) -0.714 *(v -128);
               b2= y2 + 1.772*(u-128);


            r1 = (r1>255)? 255: ((r1<0)? 0: r1);
            b1 = (b1>255)? 255: ((b1<0)? 0: b1);
            g1 = (g1>255)? 255: ((g1<0)? 0: g1);
            r2 = (r2>255)? 255: ((r2<0)? 0: r2);
            b2 = (b2>255)? 255: ((b2<0)? 0: b2);
            g2 = (g2>255)? 255: ((g2<0)? 0: g2);

    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6    ) = (unsigned char)b1;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 1) = (unsigned char)g1;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 2) = (unsigned char)r1;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 3) = (unsigned char)b2;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 4) = (unsigned char)g2;
    		*(frame_buffer + ((IMAGEHEIGHT-1-i)*(IMAGEWIDTH/2)+j)*6 + 5) = (unsigned char)r2;
    	}
    }
	IplImage *frame;
	CvMat cvmat = cvMat( 480, 640, CV_8UC3, (void*)frame_buffer );
	frame = cvDecodeImage(&cvmat, 1);
	//轉成Mat再傳socket
	Mat mat = cvarrToMat(frame);
	sendMat(mat);
    //printf("change to RGB OK \n");
}
int close_v4l2(void)
{
    // 9. Close device///////////////////////////////////
    if(fd != -1) {
        close(fd);
        return TRUE;
    }
    return FALSE;
    /////////////////////////////////////////////////////
}

void stop_streaming(void)
{
    unsigned int n_buffers;

    // 7. Stop streaming/////////////////////////////////
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type))
        perror("Stop streaming error:");
    /////////////////////////////////////////////////////

    // 8. Memory unmap///////////////////////////////////
    for (n_buffers = 0; n_buffers < req.count; n_buffers++)
        munmap(buffers[n_buffers].start, buffers[n_buffers].length);
    /////////////////////////////////////////////////////

    // 9. Close device///////////////////////////////////
    close_v4l2();
    /////////////////////////////////////////////////////
}
void prepareHDMI(void){
	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd == -1){
		perror("Error: cannot open framebuffer device");
		exit(03301);
	}
	if(ioctl(fbfd,FBIOGET_FSCREENINFO, &finfo) == -1){
		perror("Error reading fixed information");
		exit(03302);
	}
	if(ioctl(fbfd,FBIOGET_VSCREENINFO,&vinfo)==-1){
		perror("Error reading variable information");
		exit(03303);
	}
	printf("%dx%d, %dbpp\n",vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	fbp = (char *)mmap(0,screensize, PROT_READ | PROT_WRITE,MAP_SHARED, fbfd,0);
	if ((int)fbp == -1) {
		perror("Error: failed to map framebuffer device to memory");
		exit(03304);
	}


     // Open the file for reading and writing
     fbfd = open("/dev/fb0", O_RDWR);
     if (fbfd == -1) {
         perror("Error: cannot open framebuffer device");
         exit(1);
     }
     printf("The framebuffer device was opened successfully.\n");

     // Get fixed screen information
     if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
         perror("Error reading fixed information");
         exit(2);
     }

     // Get variable screen information
     if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
         perror("Error reading variable information");
         exit(3);
     }

     printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

     // Figure out the size of the screen in bytes
     screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

     // Map the device to memory
     fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbfd, 0);
     if ((int)fbp == -1) {
         perror("Error: failed to map framebuffer device to memory");
         exit(4);
     }
     printf("The framebuffer device was mapped to memory successfully.\n");
     x = 300; y = 100;
}
void closeHDMI(){
	munmap(fbp, screensize);
	close(fbfd);
}
void startvedio(void){
	int i=0;
	for(i=0;i<req.count;++i){
	        buf.index=i;
		if (ioctl(fd, VIDIOC_QBUF, &buf))perror("Queue buffer error:");
	}
	
	for(i=0;i<req.count;++i){
		if (ioctl(fd, VIDIOC_DQBUF, &buf)) {
			perror("Dequeue buffer error:");
			exit(03307);
		}
	}
}
void endvedio(void){
	//set origin
	unsigned originx,originy;	
	switch(bonus_type){
	case left  :
		originx = ( vinfo.xres - IMAGEHEIGHT)/ 2;
		originy = ( vinfo.yres - IMAGEWIDTH )/ 2;
		for(y = 0; y < IMAGEWIDTH; y++)
		for(x = 0; x <IMAGEHEIGHT; x++){
			location = (x + originx + vinfo.xoffset) * (vinfo.bits_per_pixel/8)+(y + originy +vinfo.yoffset) * finfo.line_length;
			unsigned fby,fbx;
			fby = IMAGEHEIGHT - x + 1;
			fbx = IMAGEWIDTH - y + 1;
			*(fbp + location + 0) = *(frame_buffer + ((fby)*(IMAGEWIDTH) + fbx )* 3 + 0);
			*(fbp + location + 1) = *(frame_buffer + ((fby)*(IMAGEWIDTH) + fbx )* 3 + 1);
			*(fbp + location + 2) = *(frame_buffer + ((fby)*(IMAGEWIDTH) + fbx )* 3 + 2);
		}
		break;
	case right :
		originx = ( vinfo.xres - IMAGEHEIGHT)/ 2;
		originy = ( vinfo.yres - IMAGEWIDTH )/ 2;
		
		for(y = 0; y < IMAGEWIDTH; y++)
		for(x = 0; x <IMAGEHEIGHT; x++){
			location = (x+ originx + vinfo.xoffset) * (vinfo.bits_per_pixel/8)+(y +originy+ vinfo.yoffset) * finfo.line_length;
			unsigned fby,fbx;
			fby = x;
			fbx = y;
			*(fbp + location + 0) = *(frame_buffer + ((fby)*(IMAGEWIDTH) + fbx )* 3 + 0);
			*(fbp + location + 1) = *(frame_buffer + ((fby)*(IMAGEWIDTH) + fbx )* 3 + 1);
			*(fbp + location + 2) = *(frame_buffer + ((fby)*(IMAGEWIDTH) + fbx )* 3 + 2);
		}
		break;
	case origin:
		originx = ( vinfo.xres - IMAGEWIDTH )/ 2;
		originy = ( vinfo.yres - IMAGEHEIGHT)/ 2;
		for(y = 0; y < IMAGEHEIGHT; y++){
			for(x = 0;x <IMAGEWIDTH/2; x++){
				location = (x*2+originx+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
					(y+originy+vinfo.yoffset) * finfo.line_length;
				unsigned fby,fbx;
				fby = IMAGEHEIGHT - y - 1;
				fbx = x;
				if(vinfo.bits_per_pixel == 32){
					*(fbp + location) = *(frame_buffer + ((fby)*(IMAGEWIDTH/2)+fbx)*6    );//B
					*(fbp + location + 1) = *(frame_buffer + ((fby)*(IMAGEWIDTH/2)+fbx)*6 + 1);//G
					*(fbp + location + 2) = *(frame_buffer + ((fby)*(IMAGEWIDTH/2)+fbx)*6 + 2);//R
					*(fbp + location + 3) = 0;
					*(fbp + location + 4) =*(frame_buffer + ((fby)*(IMAGEWIDTH/2)+fbx)*6 + 3);
					*(fbp + location + 5) =*(frame_buffer + ((fby)*(IMAGEWIDTH/2)+fbx)*6 + 4);
					*(fbp + location + 6) =*(frame_buffer + ((fby)*(IMAGEWIDTH/2)+fbx)*6 + 5);
					*(fbp + location + 7) =0;
				}else{
					int b = 10;
					int g = (2*x-300-100)/6;     // A little green
					int r = 31-(y-100)/16;    // A lot of red
					unsigned short int t = r<<11 | g << 5 | b;
					*((unsigned short int*)(fbp + location)) = t;
				}
			}
		}
		break;
	}
}
/*
int main(int argc,char** argv)
{
	if(argc >= 2){
		switch (*argv[1]){
			case 'o':
				bonus_type = origin ;
				break;
			case 'l':
				bonus_type = left   ;
				break;
			case 'r':
				bonus_type = right  ;
				break;
			default:
				bonus_type = origin ;
		};
	}else{
		bonus_type = origin;
	}
	if (init_v4l2() == FALSE)return FALSE;
	prepareHDMI();
	v4l2_grab();

	while(1){
		startvedio();
		yuyv_2_rgb888();
		endvedio();
	}


	//Stop device
	stop_streaming();
	closeHDMI();
	return 0;
}
*/
unsigned char* get_frame_buffer(){
	return frame_buffer;
}
void v4l2_prepare(int i){
	requestCount = i;
	if (init_v4l2() == FALSE) exit(602);
	v4l2_grab();
	return;
}
void v4l2_photo(){
	startvedio();
//	yuyv_2_rgb888();
}
void v4l2_stop(){
	stop_streaming();
	return;
}
