#include <unistd.h>
#include <iostream>
#include <queue>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

template<typename T>
class SharedMap {
  private:
    map<int,T *> q;
    mutex m;
    condition_variable cv;

  public:
    SharedMap() {}
        
    inline void push(T * element, int pos) {
      unique_lock<mutex> lck(this->m);
      this->q.insert( pair<int,T *> (pos,element));
      this->cv.notify_all();
    }

    inline T * pop(int pos) {
      unique_lock<mutex> lck(this->m);
      while (this->q.empty()) {
        return  NULL;
      }
      T * result = this->q.at(pos);
      this->q.erase(pos);
      return result;
    }

    inline bool empty(){
      return q.empty();
    }
    inline int size(){
      return q.size();
    }

};


template<typename T>
class SharedQueue {
  private:
    queue<T *> q;
    mutex m;
    condition_variable cv;

  public:
    SharedQueue() {}

    inline void push(T * element) {
      unique_lock<mutex> lck(this->m);
      this->q.push(element);
      this->cv.notify_all();
    }

    inline T * pop() {
      unique_lock<mutex> lck(this->m);
      while (this->q.empty()) {
        this->cv.wait(lck);
      }
      T * result = this->q.front();
      this->q.pop();
      return result;
    }

    inline bool empty(){
      return q.empty();
    }
    inline int size(){
      return q.size();
    }
};
