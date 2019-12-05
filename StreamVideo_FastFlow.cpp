#include <iostream>                     // for standard I/O
#include <string>                       // for string
#include <opencv2/opencv.hpp>           // OpenCV
#include <ff/pipeline.hpp>              // FastFlow
#include <ff/farm.hpp>

// #define DEBUG


#define HAS_CXX11_VARIADIC_TEMPLATES 1  // needed to use the ff_pipe pattern

using namespace std;
using namespace cv;
using namespace ff;

struct timeval te1,te2,tr1,tr2,tc1,tc2,tt,te;

int  NWORKERS;
float REDUCE_TIME;

struct SettingVideo {
    const char* inputName;
    int    fourcc;
    double fps;
    Size   frameSize;
    bool   isColor = true;
    int    ray;
    int    frame_number;
};

struct Emitter : ff_node {
    Emitter(const char* inputName, const char* ray, SettingVideo *setting): setting(setting) {
        setting->ray = atoi(ray);
        setting->inputName = inputName;
        inputVideo = VideoCapture(setting->inputName);
        if (!inputVideo.isOpened()) {
            cerr << "Could not open video." << endl;
        }
        setting->frame_number = inputVideo.get(CAP_PROP_FRAME_COUNT);
        setting->fourcc = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));
        setting->fps = inputVideo.get(CAP_PROP_FPS);
        setting->frameSize = Size((int) inputVideo.get(CAP_PROP_FRAME_WIDTH)/setting->ray,(int) inputVideo.get(CAP_PROP_FRAME_HEIGHT)/setting->ray);        
        REDUCE_TIME=0;
    }
    int svc_init(){
        gettimeofday(&te1,NULL);
        return 0;
    }
    void *svc (void * ){     
        for(;;){
            Mat* frame = new Mat();            
            inputVideo.read(*frame);
            if(frame->empty()) break;
            ff_send_out(frame);
        } 
        return EOS;
    }
    void svc_end(){ 
        inputVideo.release(); 
        gettimeofday(&te2,NULL);
    }
    VideoCapture inputVideo;
    SettingVideo *setting;    
};

struct Reduce : ff_node {
    Reduce(SettingVideo * setting) : setting(setting) { }
    int svc_init(){ 
        gettimeofday(&tr1,NULL); 
    }
    void * svc(void* in){
        Mat *t = reinterpret_cast<Mat*>(in);
        Mat *result = new Mat();
        result->create(t->size()/setting->ray, t->type());
        for(unsigned int i=0; i<result->rows; i++)
            for(unsigned int j=0; j<result->cols; j++){
                resize(Mat(*t,Rect(j*setting->ray,i*setting->ray,setting->ray,setting->ray)), Mat(*result,Rect(j,i,1,1)), Mat(*result,Rect(j,i,1,1)).size() );
            }
        free(t);
        return result;
    }
    void svc_end(){ 
        gettimeofday(&tr2,NULL); 
        REDUCE_TIME += (tr2.tv_sec-tr1.tv_sec)*1000.0 +  (tr2.tv_usec - tr1.tv_usec)/1000.0;
    }
    SettingVideo *setting;
};


struct Collector : ff_node {
    Collector (ff_gatherer *const gt, const char* nw, const char* outputName, SettingVideo *setting) 
        : gt(gt), outputName(outputName), setting(setting) {
            NWORKERS = atoi(nw);
        }
    int svc_init() {
        gettimeofday(&tc1, NULL);
        outputVideo.open(outputName, setting->fourcc, setting->fps, setting->frameSize, setting->isColor);
        if(!outputVideo.isOpened())
            cerr << "Could not open videowriter\n";
        return 0;   
    }  
    void * svc(void* in){
        int length = (setting->frame_number - cnt < NWORKERS) ? setting->frame_number - cnt : NWORKERS;
        Mat **binder = new Mat*[NWORKERS];
        gt->all_gather(in, (void**)&binder[0]);
        for(int i=0; i<length; i++){
            outputVideo.write(*binder[i]);
            cnt++;
        }
        if(cnt==setting->frame_number){
            return ((void*) NULL);
        }
        free(in);
        return GO_ON;
    }
    void svc_end(){ 
        outputVideo.release(); 
        gettimeofday(&tc2, NULL);
    }    
    VideoWriter outputVideo;
    SettingVideo *setting;
    const char* outputName;
    ff_gatherer *const gt;
    int cnt;
};

void Usage(){
    printf("Usage: StreamVideo_FastFlow <input filename> <ray> <output filename> <workers>\n"
           "Where:\n"
           " - <input filename> is the name of the file video in input\n"
           " - <ray> is the value that express the magnitude of the submatrices\n"
           " - <output filename> is the name of the file video in output\n"
           " - <workers> is the value that express the number of workers \n");
}

int main(int argc, char** argv) {
    //Set OpenCV Degree Of Parallelism  
    setNumThreads(0);
#ifdef DEBUG
  printf("\n---------------------------------\n"
         "         StreamVideo               \n"
         "   Implemented with FastFlow       \n"
         "         Maurizio Idini            \n"
         "---------------------------------\n\n");
    if(argc<4){
        Usage();
        return -1;
    }
#endif
    SettingVideo* setting = new SettingVideo();
    ff_farm<> farm;
    vector<ff_node* > W;
    for(int i=0; i<atoi(argv[4]); i++)
        W.push_back(new Reduce(setting));
    farm.add_emitter(new Emitter(argv[1], argv[2], setting));
    farm.add_workers(W);
    farm.add_collector(new Collector(farm.getgt(), argv[4], argv[3], setting));
    gettimeofday(&tt,NULL);
    farm.run_and_wait_end();
    gettimeofday(&te,NULL);
    float total          = (te.tv_sec - tt.tv_sec)*1000.0 + ((float) (te.tv_usec - tt.tv_usec))/1000.0;
    float time_emitter   = (te2.tv_sec-te1.tv_sec)*1000.0 + ((float) (te2.tv_usec-te1.tv_usec))/1000.0;
    float time_collector = (tc2.tv_sec-tc1.tv_sec)*1000.0 + ((float) (tc2.tv_usec-tc1.tv_usec))/1000.0;

#ifdef DEBUG
    printf("Time Emitter %f\n",time_emitter/1000.0);
    printf("Time Worker %d %f computed %d frames \n",i,REDUCE_TIME/NWORKERS/1000.0,setting->frame_number/NWORKERS/1000.0);
    printf("Time Collector %f\n",time_collector);
    printf("File %s | Ray %d | Workers %d | Time %f \n", argv[1], atoi(argv[2]), atoi(argv[4]), total/1000);
    free(setting);
    system("set -e '\a'");
#else
    printf("%f ", time_emitter/1000.0);
    printf(" %f %d",REDUCE_TIME/NWORKERS/1000.0,setting->frame_number/NWORKERS);
    printf(" %f %f\n",time_collector/1000.0, total/1000.0 );
#endif
    return 0;
}