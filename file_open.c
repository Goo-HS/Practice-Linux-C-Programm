#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


int main()
{
	int fd;

	if((fd = creat("MyFile.txt", S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
	{
		perror("creat");
		exit(1);
	}
	else
	{
		printf("creat file success\r\n");
	}

	close(fd);
	return 0;
}
