#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>


#define BTN_ADDRESS 0
#define MOTOR_ADDRESS 0
int fd;
int get_value(int addr){
	int value;
	unsigned page_addr, page_offset;
	void *ptr;
	unsigned page_size=sysconf(_SC_PAGESIZE);


	//Read from switches using /dev/mem
	page_addr = (addr & (~(page_size-1)));
	page_offset = addr - page_addr;
	ptr = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);
	/* Read value from the device register */
	value = *((unsigned *)(ptr + page_offset));
	munmap(ptr, page_size);
	return value;
}
void write_value(int addr,int input){
	int value;
	unsigned page_addr, page_offset;
	void *ptr;
	unsigned page_size=sysconf(_SC_PAGESIZE);


	//Read from switches using /dev/mem
	page_addr = (addr & (~(page_size-1)));
	page_offset = addr - page_addr;
	ptr = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);
	/* Read value from the device register */
	value = *((unsigned *)(ptr + page_offset));
	*((unsigned *)(ptr + page_offset)) = input;
	munmap(ptr, page_size);
	return;
}
int mem_prepare()
{
	
	unsigned page_addr, page_offset;
	void *ptr;
	unsigned page_size=sysconf(_SC_PAGESIZE);


	//Read from switches using /dev/mem
	fd = open ("/dev/mem", O_RDWR);
	if (fd < 1) {
		perror("123");
		return -1;
	}
	return 0;
}

void mem_close(){
	close(fd);
}
