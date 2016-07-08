#include <iostream>                     
#include <string>                       
#include <sys/time.h>
#include <opencv2/opencv.hpp> 

// #define DEBUG

using namespace std;
using namespace cv;

#ifdef DEBUG
struct timeval t0,t1;
struct timeval t4,t5,t6,t7;
#endif

struct timeval t2,t3;

int main(int argc, char **argv){
    //Set OpenCV Degree Of Parallelism  
    setNumThreads(0);
    auto nameVideo = argv[1];
    int ray = atoi(argv[2]);
#ifdef DEBUG
    gettimeofday(&t0,NULL);
#endif
    VideoCapture inputVideo(nameVideo);
#ifdef DEBUG
    gettimeofday(&t1,NULL);
    double t_c = (t1.tv_sec-t0.tv_sec)*1000.0 + (t1.tv_usec - t0.tv_usec)/1000.0;
    double total_reduce_avg = 0.0;
    int frame_number = 0;
    cout << "constructor video capture " << t_c << endl; 
    gettimeofday(&t0,NULL);
#endif
    if (!inputVideo.isOpened()) {
        cerr << "Could not open video." << endl;
        return -1;
    }

    int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form
    Size S = Size((int) inputVideo.get(CAP_PROP_FRAME_WIDTH)/ray,(int) inputVideo.get(CAP_PROP_FRAME_HEIGHT)/ray);
#ifdef DEBUG
    gettimeofday(&t0,NULL);
#endif
    VideoWriter outputVideo;                                        
    outputVideo.open(argv[3], ex, inputVideo.get(CAP_PROP_FPS), S, true);
#ifdef DEBUG
    gettimeofday(&t1,NULL);
    t_c = (t1.tv_sec-t0.tv_sec)*1000.0 +  (t1.tv_usec - t0.tv_usec)/1000.0;
    cout << "open video writer " << t_c << endl; 
#endif 
    Mat* currentFrame = new Mat();
    gettimeofday(&t2,NULL);
    for(;;) {
#ifdef DEBUG
        gettimeofday(&t4,NULL);
        gettimeofday(&t0,NULL);
#endif
        inputVideo.read(*currentFrame);
#ifdef DEBUG
        cout << "frame no. " << frame_number << endl;
        gettimeofday(&t1,NULL);
        t_c = (t1.tv_sec-t0.tv_sec)*1000.0 + (t1.tv_usec - t0.tv_usec)/1000.0;
        cout << "read frame " << t_c << endl; 
        gettimeofday(&t0,NULL);
#endif
        Mat* result = new Mat();
 #ifdef DEBUG
        gettimeofday(&t1,NULL);
        t_c = (t1.tv_sec-t0.tv_sec)*1000.0 + (t1.tv_usec - t0.tv_usec)/1000.0;
        cout << "create new mat " << t_c << endl; 
        gettimeofday(&t0,NULL);
#endif
        result->create(currentFrame->size()/ray, currentFrame->type());
#ifdef DEBUG
        gettimeofday(&t1,NULL);
        t_c = (t1.tv_sec-t0.tv_sec)*1000.0 + (t1.tv_usec - t0.tv_usec)/1000.0;
        cout << "result.create " << t_c << endl; 
#endif
        if (currentFrame->empty()) {
#ifdef DEBUG
            cout << "EmptyFrame";
#endif
            break;
        }
        if(ray>=result->rows || ray >= result->cols){
#ifdef DEBUG
            cerr << "Ray too high. Break.\n";
#endif
            return -1;
        }
#ifdef DEBUG
        gettimeofday(&t0,NULL);
        int cnt=0; double avg_t=0;
#endif
        for (unsigned int i=0; i<result->rows; i++)
            for(unsigned int j=0; j<result->cols; j++){
                
 #ifdef DEBUG
                cnt++;
                gettimeofday(&t6,NULL);
#endif
                resize(Mat(*currentFrame,Rect(j*ray,i*ray,ray,ray)), Mat(*result,Rect(j,i,1,1)), Mat(*result,Rect(j,i,1,1)).size());
#ifdef DEBUG
                gettimeofday(&t7,NULL);
                t_c = (t7.tv_sec-t6.tv_sec)*1000.0 + (t7.tv_usec - t6.tv_usec)/1000.0;
                avg_t+=t_c;
#endif
            }
#ifdef DEBUG
        gettimeofday(&t1,NULL);
        t_c = (t1.tv_sec-t0.tv_sec)*1000.0 + (t1.tv_usec - t0.tv_usec)/1000.0;
        cout << "result population " << t_c << endl; 
        cout << "result total " << avg_t  << " in avg " << avg_t/cnt << " with "<< cnt << " elements"<< endl;
        total_reduce_avg += avg_t;
        gettimeofday(&t0,NULL);
#endif
        outputVideo.write(*result);
#ifdef DEBUG
        gettimeofday(&t1,NULL);
        t_c = (t1.tv_sec-t0.tv_sec)*1000.0 + (t1.tv_usec - t0.tv_usec)/1000.0;
        cout << "write frame " << t_c << endl; 
        gettimeofday(&t5,NULL);
        t_c = (t5.tv_sec-t4.tv_sec)*1000.0 + (t5.tv_usec - t4.tv_usec)/1000.0;
        cout << "process frame " << t_c << endl << "\n\n\n\n"; 
        frame_number++;
#endif
    }
    gettimeofday(&t3,NULL);
    float t_f = (t3.tv_sec-t2.tv_sec)*1000.0 + (t3.tv_usec - t2.tv_usec)/1000.0;
#ifdef DEBUG
    cout << "\n\nEntire Process " << t_f/1000 << endl; 
    cout << "time to compute Reduce in avg is " << total_reduce_avg/frame_number << endl;
#else
    printf("%f\n",t_f/1000);
#endif
    return 0;
}