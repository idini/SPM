#include "SharedObject.hpp"
#include <sys/time.h>
#include <opencv2/opencv.hpp>  

// #define DEBUG

using namespace std;
using namespace cv;

struct Task{
  Task(Mat* mat, int pos) : mat(mat), pos(pos){}
      Mat* mat;
      int pos;
    };

struct timeval t0,t1;

class ReduceVideo {

public:
  ReduceVideo();
  void create_and_run(int num, const char *inputName, const char* ray, const char *outputName);
protected:
      
  SharedQueue<Task> * em_to_wor;
  SharedMap<Mat> * wor_to_col;
  queue<thread *> * threads;
  
  void *emitter(const char *filename, const char* _ray);
  void *worker();
  void *collector(const char* outputName);

private:
  const char* inputName;
  int    fourcc;
  double fps;
  Size   frameSize;
  bool   isColor = true;
  int    ray;
  int    num_threads;

};
