#include <boost/interprocess/managed_shared_memory.hpp>  
#include <boost/interprocess/allocators/allocator.hpp>  
#include <boost/unordered_map.hpp>  
#include <boost/functional/hash.hpp>  
#include <functional>  

//http://blog.csdn.net/cszhouwei/article/details/42427607

int main (int argc, char *argv[])  
{  
    typedef int KeyType;  
    typedef float MappedType;  
    typedef std::pair<const int, float> ValueType;  
    typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager> ShmAlloc;  
    typedef boost::unordered_map<KeyType, MappedType, boost::hash<KeyType>, std::equal_to<KeyType>, ShmAlloc> ShmHashMap;  
  
    boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, "ContainerSharedMemory", 65536);  
    ShmHashMap *hash_map1 = segment.find_or_construct<ShmHashMap>("ShmHashMap1")(3, boost::hash<KeyType>(), std::equal_to<KeyType>(), segment.get_allocator<ValueType>());      
    ShmHashMap *hash_map2 = segment.find_or_construct<ShmHashMap>("ShmHashMap2")(3, boost::hash<KeyType>(), std::equal_to<KeyType>(), segment.get_allocator<ValueType>());      
  
    for(int i = 0; i < 5; ++i) {  
        ShmHashMap::iterator iter = hash_map1->find(i);  
        if (iter != hash_map1->end()) {  
            std::cout << "[ShmHashMap1]<" << i << ", " << iter->second << ">" << std::endl;  
            iter->second += 1.0;  
        }     
        hash_map1->insert(ValueType(i, (MappedType)i));  
    }     
    for(int i = 0; i < 5; ++i) {  
        ShmHashMap::iterator iter = hash_map2->find(i);  
        if (iter != hash_map2->end()) {  
            std::cout << "[ShmHashMap2]<" << i << ", " << iter->second << ">" << std::endl;  
            iter->second += 2.0;  
        }     
        hash_map2->insert(ValueType(i, (MappedType)i));  
    }     
  
    return 0;  
}  
