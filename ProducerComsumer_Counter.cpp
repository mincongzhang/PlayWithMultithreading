#include <boost/thread.hpp>
#include <iostream>

using namespace std;

boost::mutex m;
boost::condition_variable con_producer;
boost::condition_variable con_consumer;
volatile bool ready_to_consume = false;
volatile bool done = false;
int counter(0);


void Consumer()
{
  boost::mutex::scoped_lock lock(m);
  while(!done)
    {
      if(!ready_to_consume)
        {
          con_consumer.wait(lock);
        }
      cout << counter << endl;
      ready_to_consume = false;
      con_producer.notify_one();
    }
}

void Producer()
{
  while(!done)
    {
      boost::this_thread::sleep(boost::posix_time::millisec(500));
      boost::mutex::scoped_lock lock(m);
      if(ready_to_consume)
        {
          con_producer.wait(lock);
        }
      ++counter;
      ready_to_consume = true;
      done = (counter == 10);
      con_consumer.notify_one();
    }
}

int main()
{
  boost::thread consumer_pthread(Consumer);
  boost::thread producer_pthread(Producer);
  consumer_pthread.join();
  producer_pthread.join();
  return 0;
}
