#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <queue>
#include <iostream>
#include <string>
//#include "Producer.hpp"
//#include "Consumer.hpp"

using namespace boost;
using namespace boost::this_thread;
using namespace std;


// Queue class that has thread synchronisation
template <typename T>
class SynchronisedQueue
{
private:
  std::queue<T> m_queue; // Use STL queue to store data
  boost::mutex m_mutex; // The mutex to synchronise on
  boost::condition_variable m_cond; // The condition to wait for

public:

  // Add data to the queue and notify others
  void Enqueue(const T& data)
  {
    // Acquire lock on the queue
    boost::unique_lock<boost::mutex> lock(m_mutex);

    // Add the data to the queue
    m_queue.push(data);

    // Notify others that data is ready
    m_cond.notify_one();

  } // Lock is automatically released here

  // Get data from the queue. Wait for data if not available
  T Dequeue()
  {

    // Acquire lock on the queue
    boost::unique_lock<boost::mutex> lock(m_mutex);

    // When there is no data, wait till someone fills it.
    // Lock is automatically released in the wait and obtained
    // again after the wait
    while (m_queue.size()==0) m_cond.wait(lock);

    // Retrieve the data from the queue
    T result=m_queue.front(); m_queue.pop();
    return result;

  } // Lock is automatically released here
};

// Class that produces objects and puts them in a queue
class Producer
{
private:
  int m_id; // The id of the producer
  SynchronisedQueue<string>* m_queue; // The queue to use

public:

  // Constructor with id and the queue to use
  Producer(int id, SynchronisedQueue<string>* queue)
  {
    m_id=id;
    m_queue=queue;
  }

  // The thread function fills the queue with data
  void operator () ()
  {
    int data=0;
    while (true)
      {
        // Produce a string and store in the queue
        string str = "Producer ["+boost::lexical_cast<string>(m_id+1)+"]: produced data "+boost::lexical_cast<string>(++data)+".";
        m_queue->Enqueue(str);
        cout<<str<<endl;

        // Sleep one second
        boost::this_thread::sleep(boost::posix_time::seconds(1));
      }
  }
};

// Class that consumes objects from a queue
class Consumer
{
private:
  int m_id; // The id of the consumer
  SynchronisedQueue<string>* m_queue; // The queue to use

public:
  // Constructor with id and the queue to use.
  Consumer(int id, SynchronisedQueue<string>* queue)
  {
    m_id=id;
    m_queue=queue;
  }

  // The thread function reads data from the queue
  void operator () ()
  {
    while (true)
      {
        // Get the data from the queue and print it
        cout<<"Consumer ["<<boost::lexical_cast<string>(m_id+1)<<"] consumed: ("<<m_queue->Dequeue()<<")"<<endl;

        // Make sure we can be interrupted
        boost::this_thread::interruption_point();
      }
  }
};

int main()
{
  // Display the number of processors/cores
  cout<<boost::thread::hardware_concurrency()
      <<" processors/cores detected."<<endl<<endl;

  // The number of producers/consumers
  int nrProducers, nrConsumers;

  // The shared queue
  SynchronisedQueue<string> queue;

  // Ask the number of producers
  cout<<"How many producers do you want? : ";
  cin>>nrProducers;

  // Ask the number of consumers
  cout<<"How many consumers do you want? : ";
  cin>>nrConsumers;

  // Create producers
  boost::thread_group producers;
  for (int i=0; i<nrProducers; i++)
    {
      Producer p(i, &queue);
      producers.create_thread(p);
    }

  // Create consumers
  boost::thread_group consumers;
  for (int i=0; i<nrConsumers; i++)
    {
      Consumer c(i, &queue);
      consumers.create_thread(c);
    }

  //Wait for a while
  double wait(0.0);
  while(wait < 500000){ wait+=0.0001; };

  // Interrupt the threads and stop them
  producers.interrupt_all(); producers.join_all();
  consumers.interrupt_all(); consumers.join_all();
}
