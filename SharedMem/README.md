#### mutex in shared memory

https://stackoverflow.com/questions/1700299/mutex-in-shared-memory-when-one-user-crashes

http://boost.2283326.n4.nabble.com/interprocess-mutex-in-shared-memory-crash-recovery-td2586629.html

https://stackoverflow.com/questions/6267973/boostinterprocessscoped-lock-application-crash-inside-lock

https://stackoverflow.com/questions/7808431/boost-interprocess-named-mutex-remains-acquired-after-a-crash

TODO:  
virtual memory  
physical memory  
RSS/PSS  

PSS: proportional set size (PSS) is the portion of main memory (RAM) occupied by a process and is composed by the private memory of that process plus the proportion of shared memory with one or more other processes. 

RSS: resident set size (RSS) is the portion of memory occupied by a process that is held in main memory (RAM).  

ulimit only restrict virtual memory, so when physical memory too high, box crashes  

https://www.selenic.com/smem/  
https://stackoverflow.com/questions/7880784/what-is-rss-and-vsz-in-linux-memory-management/21049737#21049737  
