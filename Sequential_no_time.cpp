#include <iostream>                     
#include <string>                       
#include <sys/time.h>
#include <opencv2/opencv.hpp> 

using namespace std;
using namespace cv;

int main(int argc, char **argv){
    //Set OpenCV Degree Of Parallelism  
    setNumThreads(0);
    auto nameVideo = argv[1];
    int ray = atoi(argv[2]);
    VideoCapture inputVideo(nameVideo);
    if (!inputVideo.isOpened()) {
        cerr << "Could not open video." << endl;
        return -1;
    }
    int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form
    Size S = Size((int) inputVideo.get(CAP_PROP_FRAME_WIDTH)/ray,(int) inputVideo.get(CAP_PROP_FRAME_HEIGHT)/ray);
    
    VideoWriter outputVideo;                                        
    outputVideo.open(argv[3], ex, inputVideo.get(CAP_PROP_FPS), S, true);
    
    Mat* currentFrame = new Mat();
    for(;;) {
        inputVideo.read(*currentFrame);
        Mat* result = new Mat();
        result->create(currentFrame->size()/ray, currentFrame->type());
        if (currentFrame->empty()) {
            cerr << "EmptyFrame";
            break;
        }
        if(ray>=result->rows || ray >= result->cols){
            cerr << "Ray too high. Break.\n";
            return -1;
        }
        for (unsigned int i=0; i<result->rows; i++)
            for(unsigned int j=0; j<result->cols; j++){
                resize(Mat(*currentFrame,Rect(j*ray,i*ray,ray,ray)), Mat(*result,Rect(j,i,1,1)), Mat(*result,Rect(j,i,1,1)).size());
            }
        outputVideo.write(*result);
    }
    return 0;
}