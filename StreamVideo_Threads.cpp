#include "inc/StreamVideo_Threads.hpp"

// #define DEBUG

using namespace std;
using namespace cv;

StreamVideo::StreamVideo() {
    em_to_wor  = new SharedQueue<Task>();
    wor_to_col = new SharedMap<Mat>();
}

void *StreamVideo::emitter( const char* filename, const char* _ray ) {
  ray = atoi(_ray);
  inputName = filename;
  VideoCapture inputVideo(inputName);
  if (!inputVideo.isOpened()) {
    cerr << "Could not open video." << endl;
  }
  fourcc = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));
  fps = inputVideo.get(CAP_PROP_FPS);
  frameSize = Size((int) inputVideo.get(CAP_PROP_FRAME_WIDTH)/ray,(int) inputVideo.get(CAP_PROP_FRAME_HEIGHT)/ray);
  int cnt=0;
  for(;;){
    Mat* frame = new Mat();            
    inputVideo.read(*frame);
    if(frame->empty()) break;
    em_to_wor->push(new Task(frame,cnt));
    cnt++;
  }
  em_to_wor->push(NULL);
  inputVideo.release();
  return (void*)NULL;
}

void *StreamVideo::worker() {
  Task* t;
  int cnt=0;
  while (!em_to_wor->empty()) {
    if(t = em_to_wor->pop()){
      Mat *result = new Mat();
      result->create(t->mat->size()/ray, t->mat->type());
      for(int i=0; i<result->rows; i++)
        for(int j=0; j<result->cols; j++){
          resize(Mat(*t->mat,Rect(j*ray,i*ray,ray,ray)), Mat(*result,Rect(j,i,1,1)), Mat(*result,Rect(j,i,1,1)).size() );
        }
      cnt++;
      wor_to_col->push(result,t->pos);
      free(t);
    }
  } 
  return (void*)NULL;
}

void *StreamVideo::collector(const char* outputName) {
  VideoWriter outputVideo;
  outputVideo.open(outputName, fourcc, fps, frameSize, isColor);
  if(!outputVideo.isOpened())
    cerr << "Could not open videowriter\n";
  Mat * t;
  int cnt=0;
  while(t = wor_to_col->pop(cnt)) {
    outputVideo.write(*t);
    cnt++;
    t->release();
    delete t;
  }
  return (void*)NULL;
}

void StreamVideo::create_and_run(int num, const char *inputName, const char* ray, const char *outputName) {
  queue<thread *> * threads = new queue<thread *>();
  num_threads = num;
  thread * e = new thread( &StreamVideo::emitter, this, inputName, ray);//[=] { this->emitter(inputName, ray); } );
  e->join();
  while (num > 0) {
    thread * t = new thread( &StreamVideo::worker,this); //[=] { this->worker(); } );
    threads->push(t);
    num--;
  }
  while (!threads->empty()) {
    threads->front()->join();
    threads->pop();
  }
  thread * c = new thread( &StreamVideo::collector, this, outputName); //[=] { this->collector(outputName); } );
  c->join();
}

void Usage(){
    printf("Usage: StreamVideo_Threads <input filename> <ray> <output filename> <workers>\n"
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
         "   Implemented with Threads C++11  \n"
         "         Maurizio Idini            \n"
         "---------------------------------\n\n");
  if(argc<4){
    Usage();
    return -1;
  }
#endif
  StreamVideo r = StreamVideo();
  gettimeofday(&t0,NULL); 
  r.create_and_run(atoi(argv[4]), argv[1], argv[2], argv[3]);
  gettimeofday(&t1,NULL); 
  float total = (t1.tv_sec-t0.tv_sec)*1000.0 + ((float) (t1.tv_usec - t0.tv_usec))/1000.0;
#ifdef DEBUG
  printf("File %s | Ray %d | Workers %d | Time %f \n", argv[1], atoi(argv[2]), atoi(argv[4]), total/1000);
  system("echo -e '\a'");
#else
  printf("%f \n", total/1000);
#endif
  return 0;
}