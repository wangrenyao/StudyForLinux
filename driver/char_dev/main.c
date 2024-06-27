#include <stdio.h>  

#include <stddef.h>
#include <fcntl.h>  
#include <stdlib.h>  
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>  
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/mman.h> 


int main()


{
	int fd = open("/dev/uio0", O_RDWR);
	
	void *addr = mmap(0x77777777, 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);



}
