#include <iostream>
#include <string>
#include <unistd.h>

#include <boost/scope_exit.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/upgradable_lock.hpp>

// http://stackoverflow.com/questions/12439099/interprocess-reader-writer-lock-with-boost/

#define SHARED_MEMORY_NAME "SO12439099-MySharedMemory"

struct shared_data {
private:
  typedef boost::interprocess::interprocess_upgradable_mutex upgradable_mutex_type;

  mutable upgradable_mutex_type mutex;
  volatile int counter;

public:
  shared_data()
    : counter(0)
  {
  }

  int count() const {
    boost::interprocess::sharable_lock<upgradable_mutex_type> lock(mutex);
    return counter;
  }

  void set_counter(int counter) {
    boost::interprocess::scoped_lock<upgradable_mutex_type> lock(mutex);
    this->counter = counter;
  }
};

int main(int argc, char *argv[])
{
  using namespace boost::interprocess;

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " WHICH" << std::endl;
    return 1;
  }

  const std::string which = argv[1];
  if (which == "parent") {
    shared_memory_object::remove(SHARED_MEMORY_NAME);
    shared_memory_object shm(create_only, SHARED_MEMORY_NAME, read_write);

    BOOST_SCOPE_EXIT(argc) {
      shared_memory_object::remove(SHARED_MEMORY_NAME);
    } BOOST_SCOPE_EXIT_END;

    shm.truncate(sizeof (shared_data));

    // Map the whole shared memory into this process.
    mapped_region region(shm, read_write);

    // Construct the shared_data.
    new (region.get_address()) shared_data;

    // Go to sleep for a minute.
    std::cout<<"sleep 60s"<<std::endl;
    sleep(60);

    return 0;
  } else if (which == "reader_child") {
    shared_memory_object shm(open_only, SHARED_MEMORY_NAME, read_write);

    mapped_region region(shm, read_write);
    shared_data& d = *static_cast<shared_data *>(region.get_address());

    for (int i = 0; i < 100000; ++i) {
      std::cout << "reader_child: " << d.count() << std::endl;
    }
  } else if (which == "writer_child") {
    shared_memory_object shm(open_only, SHARED_MEMORY_NAME, read_write);

    mapped_region region(shm, read_write);
    shared_data& d = *static_cast<shared_data *>(region.get_address());

    for (int i = 0; i < 100000; ++i) {
      d.set_counter(i);
      std::cout << "writer_child: " << i << std::endl;
    }
  }
}


/*
./a.out parent
./a.out reader_child &
./a.out reader_child &
./a.out writer_child &
./a.out reader_child &
./a.out reader_child &

*/
