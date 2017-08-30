#include <string>
#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <functional>

//http://blog.csdn.net/cszhouwei/article/details/42427607
//https://stackoverflow.com/questions/29557680/boost-interprocess-unordered-map-compilation/29558548#29558548
//http://www.boost.org/doc/libs/1_55_0/doc/html/interprocess/quick_guide.html
//http://www.boost.org/doc/libs/1_65_0/doc/html/interprocess/allocators_containers.html#interprocess.allocators_containers.additional_containers.unordered

//http://www.boost.org/doc/libs/1_55_0/doc/html/interprocess/managed_memory_segments.html

namespace {
  typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
  typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> ShmString;
  typedef ShmString KeyType;
  typedef ShmString MappedType;
  typedef std::pair<const KeyType, MappedType> ValueType;
  typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager> ShmAlloc;
  typedef boost::unordered_map<KeyType, MappedType, boost::hash<KeyType>, std::equal_to<KeyType>, ShmAlloc> ShmHashMap;
}

template<class T>
inline std::string to_string(const T &val)
{ std::ostringstream ostr; ostr << val; return ostr.str(); }

inline std::string to_string(const ShmString &val)
{ return std::string(val.begin(), val.end());}

template<class T>
inline ShmString to_shm_string(const T &val, boost::interprocess::managed_shared_memory & segment){
  std::ostringstream ostr;
  ostr << val;
  return ShmString(ostr.str().c_str(),segment.get_allocator<ShmString>());
}

int main (int argc, char *argv[])
{
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, "MizhangSharedMemory", 65536);//bytes
  //Construct a shared memory hash map.
  //Note that the first parameter is the initial bucket count and
  //after that, the hash function, the equality function and the allocator
  ShmHashMap *hash_map1 = segment.find_or_construct<ShmHashMap>("ShmHashMap1")
    (3,                                    // initial bucket count
     boost::hash<KeyType>(),               // the hash function
     std::equal_to<KeyType>(),             // the equality function
     segment.get_allocator<ValueType>());  // the allocator

  for(int i = 0; i < 500; ++i) {
    ShmHashMap::iterator iter = hash_map1->find(to_shm_string(i,segment));
    if (iter != hash_map1->end()) {
      std::string val = to_string(iter->second);
      std::cout << "[ShmHashMap1]<" << i << ", " << val << ">" << std::endl;
      iter->second = to_shm_string(i,segment);
    } else {
      hash_map1->insert(ValueType(to_shm_string(i,segment), to_shm_string("init empty",segment)));
    }
  }

  return 0;
}
