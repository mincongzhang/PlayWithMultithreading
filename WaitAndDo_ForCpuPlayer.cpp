#include <boost/thread.hpp>
#include <iostream>
#include <stdlib.h>

using namespace std;

boost::mutex m;
boost::condition_variable con_publisher;
int counter(0);

//Publish for every 1000 ms, prevent producer producing too fast
//However when producer produces kind of slow, wait for it to finish
void Publisher()
{
  boost::mutex::scoped_lock lock(m);
  boost::this_thread::sleep(boost::posix_time::millisec(1000));
  con_publisher.wait(lock);
  cout <<"Publish production: ["<<counter<<"]"<< endl;
}

void Producer()
{
  for(int i=0;i<10;i++){
    ++counter;
    cout <<"Producing: ["<<counter<<"]"<< endl;
    boost::this_thread::sleep(boost::posix_time::millisec(rand() % 100 + 300));
  }
  con_publisher.notify_one();
}

int main()
{

  for(int i=0;i<10;i++){
    boost::thread publisher_pthread(Publisher);
    boost::thread producer_pthread(Producer);
    publisher_pthread.join();
    producer_pthread.join();
  }

  return 0;
}
