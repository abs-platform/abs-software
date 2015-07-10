PATHS = -I/usr/local/include/opencv2 -L/usr/local/lib/
INCLUDES = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video \
           -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib \
           -lopencv_legacy


all:
	g++ $(PATHS) -g sat_camera.cpp sat_detector.cpp sat_velocity.cpp sat_tracker.cpp -o sat_tracker -Wall $(INCLUDES)

help:
	@echo
	@echo "Makefile for SatTacker - Jaume Anguera, Spring 2015"
	@echo
	@echo "USAGE: make [all | clean]"
	@echo
	@echo "   all (default)   builds sat_tracker algorithm"
	@echo "   clean           removes sat_tracker executable and the results obtained"
	@echo "                   by the algorithm (saved in carpeta_test folder)"
	@echo

clean:
	rm sat_tracker
	rm carpeta_test/result_*.png
