FLAGS_OCV = `pkg-config --cflags opencv`
LIBS_OCV = `pkg-config --libs opencv`
FLAGS = -MMD -MP -MF
COMP = -c -g

all: svmlight/svm_learn.o svmlight/svm_hideo.o svmlight/svm_common.o main.o
	g++ $(FLAGS_OCV) -o opencvDetector main.o svmlight/svm_learn.o svmlight/svm_hideo.o \
						svmlight/svm_common.o $(LIBS_OCV)

svmlight/svm_learn.o: svmlight/svm_learn.c
	gcc $(COMP) $(FLAGS_OCV) $(FLAGS) svmlight/svm_learn.o.d -o svmlight/svm_learn.o svmlight/svm_learn.c

svmlight/svm_hideo.o: svmlight/svm_hideo.c
	gcc $(COMP) $(FLAGS_OCV) $(FLAGS) svmlight/svm_hideo.o.d -o svmlight/svm_hideo.o svmlight/svm_hideo.c

svmlight/svm_common.o: svmlight/svm_common.c
	gcc $(COMP) $(FLAGS_OCV) $(FLAGS) svmlight/svm_common.o.d -o svmlight/svm_common.o svmlight/svm_common.c

main.o: detection.cpp
	g++ $(FLAGS_OCV) $(COMP) $(FLAGS) main.o.d -o main.o detection.cpp

clear:
	rm *o 
	rm ./svmlight/*o 
	rm opencvDetector
