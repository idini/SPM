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

TARGETS		= StreamVideo_FastFlow \
			  StreamVideo_Threads  \
			  Sequential

all: StreamVideo_FastFlow StreamVideo_Threads Sequential

Sequential: Sequential.cpp
	$(CXX) Sequential.cpp -o Sequential $(CXXFLAGS) $(INCLUDE_CV) $(CV_LIBS) $(OPTFLAGS) $(VECTFLAGS)  $(LDFLAGS) 

StreamVideo_Threads: StreamVideo_Threads.cpp
	$(CXX) StreamVideo_Threads.cpp -o StreamVideo_Threads $(CXXFLAGS) $(INCLUDE_CV) $(CV_LIBS) $(OPTFLAGS) $(VECTFLAGS)  $(LDFLAGS) 

StreamVideo_FastFlow:  StreamVideo_FastFlow.cpp
	$(CXX) StreamVideo_FastFlow.cpp -o StreamVideo_FastFlow $(CXXFLAGS) $(INCLUDE_CV) $(CV_LIBS) $(INCLUDE_FF) $(OPTFLAGS) $(LDFLAGS) 

clean		: 
	rm -f $(TARGETS)
cleanall	: clean
	\rm -f *.o *~