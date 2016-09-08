#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*自定义的错误处理函数*/
void my_err(const char *err_string, int line)
{
	fprintf(stderr, "line:%d\n", line);
	perror(err_string);
	exit(1);
}

int main()
{
	int fd;
	int ret;
	int access_mode;

	if((fd = open("MyFile.txt", O_RDONLY)) == -1)
	{
		if((fd = open("MyFile.txt", O_CREAT | O_TRUNC | O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
		{
			my_err("open", __LINE__);
		}
		else
		{
			printf("creat and open file success\r\n");
		}
	}
	else
	{
		if(lseek(fd, 0, SEEK_END) == -1)
		{
			my_err("lseek", __LINE__);
		}
		printf("open file success\r\n");
	}

	/*设置文件打开方式*/
	if((ret = fcntl(fd, F_SETFL, O_APPEND)) < 0)
	{
		my_err("fcntl", __LINE__);
	}

	/*获取文件打开方式*/
	if((ret = fcntl(fd, F_GETFL, O_APPEND)) < 0)
	{
		my_err("fcntl", __LINE__);
	}

	access_mode = ret & O_ACCMODE;
	if(access_mode == O_RDONLY)
	{
		printf("The file access mode: read only");
	}else if (access_mode == O_WRONLY)
	{
		printf("The file access mode: write only");
	}else if (access_mode == O_RDWR)
	{
		printf("The file access mode: read + write");
	}

	if(ret & O_APPEND)
	{
		printf(" ,append");
	}

	if(ret & O_NONBLOCK)
	{
		printf(" ,nonblock");
	}

	if(ret & O_SYNC)
	{
		printf(" ,sync");
	}

	printf("\n");

	close(fd);

	return 0;
}
