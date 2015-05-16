PATHS = -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/
INCLUDES = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video \
		   -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib \
		   -lopencv_legacy

all:
	g++ $(PATHS) -g main.cpp -o main -Wall $(INCLUDES)

clean:
	rm main
