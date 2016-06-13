PREFIX=/home/embedded/FinalProject/opencv/
SOURCE=faceDetect.cpp
MAIN4SOURCE=step.c myMemMap.c socketSend.c v4l2.cpp main4.cpp faceDetect.cpp
FLAGS=-g
.PHONY: send main330 main4 main321
ALL:
	arm-xilinx-linux-gnueabi-g++  ${FLAGS} -I ${PREFIX}/install/include  -I ${PREFIX}/install/include/opencv -L ${PREFIX}/install/lib \
  -lopencv_superres -lopencv_videostab   -lopencv_video -lopencv_calib3d -lopencv_features2d -lopencv_flann  -lopencv_objdetect -lopencv_ml -lopencv_highgui -lopencv_photo -lopencv_imgproc -lopencv_core -lpthread ${SOURCE} -o faceDetect
send:
	arm-xilinx-linux-gnueabi-g++  ${FLAGS} -I ${PREFIX}/install/include  -I ${PREFIX}/install/include/opencv -L ${PREFIX}/install/lib \
  -lopencv_superres -lopencv_videostab   -lopencv_video -lopencv_calib3d -lopencv_features2d -lopencv_flann  -lopencv_objdetect -lopencv_ml -lopencv_highgui -lopencv_photo -lopencv_imgproc -lopencv_core -lpthread socketSend.c -o send
	cp send /srv/tftp/
main321:
	arm-xilinx-linux-gnueabi-g++ -o main321 socketSend.c  main3_2_1.cpp v4l2.cpp \
	${FLAGS} -I ${PREFIX}/install/include  -I ${PREFIX}/install/include/opencv -L ${PREFIX}/install/lib \
	`cat pkg`
main21:
	arm-xilinx-linux-gnueabi-gcc -o main21 main2_1.c  myMemMap.c 
main330:
	arm-xilinx-linux-gnueabi-g++ -o main330 step.c myMemMap.c  main3_3_0.cpp \
	-lpthread
main4:
	arm-xilinx-linux-gnueabi-g++ ${FLAGS} -I ${PREFIX}/install/include -I ${PREFIX}/install/include/opencv -L ${PREFIX}/install/lib \
	`cat pkg` \
	${MAIN4SOURCE} -o main4
