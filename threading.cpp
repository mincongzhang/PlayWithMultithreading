#include "boost/shared_ptr.hpp"
#include <boost/thread.hpp>
#include <iostream>

using namespace boost;

void task1() {
  std::cout<<"task1..."<<std::endl;
}

void task2() {
  std::cout<<"task2..."<<std::endl;
}

int main(){
  std::cout<<"Play with multithreading..."<<std::endl;

  thread thread_1 = thread(task1);
  thread thread_2 = thread(task2);

  // do other stuff
  thread_2.join();
  thread_1.join();
  return 0;
}

