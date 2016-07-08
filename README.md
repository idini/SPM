# SPM

Distributed Systems (SPM) Course @ UniPi 

"Reducing Video Size" C++ implementation for multiCore architecture using FastFlow, Thread C++11 and OpenCV.

Algorithm works in this way:
1) In the Emitter, open Video file and set a stream of frames
2) In the Workers, apply cv::resize for each frame
3) In the Collector, write new frame in new video

Rather than "cv::resize" operation, you can apply any operation, or filter, just modifying files.

Files:
ReduceVideo_FastFlow.cpp is the implementation using FastFlow Library [a relative link](http://calvados.di.unipi.it/)
ReduceVideo_Threads.cpp is the implementation using Standard Library Thread C++11 

Obvoiusly, there is a "Sequential" Version of filter. You can see it in "Sequential.cpp" file.

