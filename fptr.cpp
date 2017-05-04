// g++ -o fptr fptr.cpp -pthread


#include <vector>
#include <cstring>
#include <iostream>

#include <pthread.h>

using namespace std;

struct Coffee {
  int beanCount;
  void *handle;
  char liquid[16];
};

typedef void *(*CallbackFunctionVoid)(void *);
// This function return a void * by reference
// it creates a typedef named CallbackFunctionVoid
// it takes an argument of void * by reference

typedef int(*CallbackFunction)(struct Coffee, void *);
// this function returns an int
// it creates a typedef named CallbackFunction
// it takes an argument of struct Coffee by value
// it takes a void * by reference



struct CallbackStruct {
  CallbackFunction func;
  void *arg;
};

class FunctionPointerDemo {
  struct Coffee cup;
  vector<CallbackStruct> callbacks;

public:

  FunctionPointerDemo() {
    cup.beanCount = 0;
    cup.handle = (void *)0x12345678;
    strncpy(cup.liquid, "callback demo", sizeof(cup.liquid));
    
  }

  void add(CallbackFunction f, void *arg) {
    CallbackStruct s;
    s.func = f;
    s.arg = arg;
    callbacks.push_back(s);
    cup.beanCount++;
  }

  void add(CallbackFunction f) {
    add(f, NULL);
  }

  void brew() {
    // call each function
    for(int i=0;i<callbacks.size();i++) {
      cout << "Brewing... " << i << " : " << callbacks[i].func(cup, callbacks[i].arg) << endl;
    }
    cup.beanCount++;
  }
};

int callback(struct Coffee c, void *l) {
  const char *f = (const char *)l;
  if(f == NULL)
    f = (const char *)"nothing";
  cout << "Callback: " << c.beanCount << " is " << c.liquid << " with " << f << endl;
  return c.beanCount;
}

int callback2(struct Coffee c, void *l) {
  cout << "Callback 2 does less" << endl;
  return 0;
}

void *thrfunc(void *arg) {
  FunctionPointerDemo *d = (FunctionPointerDemo *)arg;
  cout << "In thread...reading data from device..." << endl;
  usleep(1000000);
  for(int i=0;i<10;i++) {
    d->brew();
  }
  return 0;
}

int main() {
  FunctionPointerDemo d;
  char *cream = (char *)"cream";
  char *sugar = (char *)"sugar";

  // add 4 instances of the callbacks
  // the first argument is the function, the
  // second argument is an optional arg passed by the user
  // this lets the user register one function that
  // handles multiple callback sources (like
  // multiple devices) by associating user data
  // with each function invocation
  d.add(callback, cream);
  d.add(callback, sugar);
  d.add(callback);
  d.add(callback2, cream);

  // now do it. Just for Tony, this is in a thread.
  pthread_t thr;
  pthread_create(&thr, NULL, thrfunc, &d);
  // note that this code is not reentrant, so we can only have 1 
  // thread at a time calling brew()
  pthread_join(thr, NULL);
  // so we wait til it exits before accessing it again in the main thread
  // in more sophisticated code, we'd use locks since multiple threads
  // have to run at the same time.
  d.brew();

}

