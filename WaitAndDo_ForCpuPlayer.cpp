#include <boost/thread.hpp>
#include <iostream>
#include <stdlib.h>

using namespace std;

boost::mutex m;
boost::condition_variable con_publisher;
int counter(0);
volatile bool done = false;
//Publish for every 1000 ms, prevent producer producing too fast
//However when producer produces kind of slow, wait for it to finish
void Publisher()
{

  boost::this_thread::sleep(boost::posix_time::millisec(1000));
  cout <<"Waited: [1000] ms"<< endl;
  if(!done){//if done, just skip this lock and no waiting
    boost::mutex::scoped_lock lock(m);
    con_publisher.wait(lock);
  }
  cout <<"Publish production: ["<<counter<<"]"<< endl;
  done = false;
}

void Producer()
{
  for(int i=0;i<10;i++){
    ++counter;
    cout <<"Producing: ["<<counter<<"]"<< endl;
    boost::this_thread::sleep(boost::posix_time::millisec(rand() % 100 + 300));//may have deadlock when Producer product too quick
  }
  done = true;
  con_publisher.notify_one();
}

int main()
{

  for(int i=0;i<10;i++){
    boost::thread publisher_pthread(Publisher);
    boost::thread producer_pthread(Producer);
    
    // Function with Arguments
    //void workerFunc(const char* msg, unsigned delaySecs) //...
    //boost::thread workerThread(workerFunc, "Hello, boost!", 3);
    
    publisher_pthread.join();
    producer_pthread.join();
  }

  return 0;
}
