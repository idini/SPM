CV_ROOT		= /usr/local/include/
FF_ROOT		= /home/idini/fastflow/
CV_LIBS 	= -L/usr/local/lib/ `pkg-config --cflags --libs opencv`
#-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio  -lopencv_video -lopencv_imgproc -lpthread 
CXX			= g++ 
INCLUDE_CV	= -I $(CV_ROOT)  
INCLUDE_FF	= -I $(FF_ROOT)  
CXXFLAGS  	= -Wall -std=c++11 -w #--verbose  
LDFLAGS 	= -pthread
OPTFLAGS	= -O3 
VECTFLAGS	= -ftree-vectorize -floop-optimize -funroll-all-loops -finline-functions #-ftree-vectorizer-verbose=5

TARGETS		= ReduceVideo_FastFlow \
			  ReduceVideo_Threads  \
			  Sequential

all: ReduceVideo_FastFlow ReduceVideo_Threads Sequential

Sequential: Sequential.cpp
	$(CXX) Sequential.cpp -o Sequential $(CXXFLAGS) $(INCLUDE_CV) $(CV_LIBS) $(OPTFLAGS) $(VECTFLAGS)  $(LDFLAGS) 

ReduceVideo_Threads: ReduceVideo_Threads.cpp
	$(CXX) ReduceVideo_Threads.cpp -o ReduceVideo_Threads $(CXXFLAGS) $(INCLUDE_CV) $(CV_LIBS) $(OPTFLAGS) $(VECTFLAGS)  $(LDFLAGS) 

ReduceVideo_FastFlow:  ReduceVideo_FastFlow.cpp
	$(CXX) ReduceVideo_FastFlow.cpp -o ReduceVideo_FastFlow $(CXXFLAGS) $(INCLUDE_CV) $(CV_LIBS) $(INCLUDE_FF) $(OPTFLAGS) $(LDFLAGS) 

clean		: 
	rm -f $(TARGETS)
cleanall	: clean
	\rm -f *.o *~