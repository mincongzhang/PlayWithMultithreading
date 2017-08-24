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

template<class T>
inline std::string to_string(const T &val)
{ std::ostringstream ostr; ostr << val; return ostr.str(); }

int main (int argc, char *argv[])
{
  typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
  typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> ShmString;
  typedef ShmString KeyType;
  typedef ShmString MappedType;
  typedef std::pair<const KeyType, MappedType> ValueType;
  typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager> ShmAlloc;
  typedef boost::unordered_map<KeyType, MappedType, boost::hash<KeyType>, std::equal_to<KeyType>, ShmAlloc> ShmHashMap;
  boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, "ContainerSharedMemory", 65535);
  ShmHashMap *hash_map1 = segment.find_or_construct<ShmHashMap>("ShmHashMap1")(3, boost::hash<KeyType>(), std::equal_to<KeyType>(), segment.get_allocator<ValueType>());

  for(int i = 0; i < 500; ++i) {
    ShmHashMap::iterator iter = hash_map1->find(ShmString("1",segment.get_allocator<ShmString>()));
    if (iter != hash_map1->end()) {
      std::cout << "[ShmHashMap1]<" << i << ", " << iter->second << ">" << std::endl;
      iter->second = ShmString(to_string(i).c_str(),segment.get_allocator<ShmString>());
    } else {
      hash_map1->insert(ValueType(ShmString(to_string(i).c_str(),segment.get_allocator<ShmString>()), ShmString("1",segment.get_allocator<ShmString>())));
    }
  }

  return 0;
}
