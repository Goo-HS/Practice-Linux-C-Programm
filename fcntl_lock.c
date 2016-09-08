#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/*锁的设置或释放函数*/
int lock_set(int fd, struct flock *lock)
{
	if(fcntl(fd, F_SETLK, lock) == 0)  //执行成功 
	{
		if(lock->l_type == F_RDLCK)
		{
			printf("set read lock, pid:%d\n", getpid());
		}
		if(lock->l_type == F_WRLCK)
		{
			printf("set write lock, pid:%d\n", getpid());
		}
		if(lock->l_type == F_UNLCK)
		{
			printf("release lock, pid:%d\n", getpid());
		}
	}
	else
	{
		perror("lock operation fail\n");
		return -1;
	}
}

/*测试锁，只有当测试发现参数lock指定的锁能被设置时，返回0*/
int lock_test(int fd, struct flock *lock)
{
	if(fcntl(fd, F_GETLK, lock) == 0)	//执行成功
	{
		if(lock->l_type == F_UNLCK)		//测试发现能按参数lock要求设置锁
		{
			printf("lock can be set in fd\n");
			return 0;
		}
		else
		{
			if(lock->l_type == F_RDLCK)
			{
				printf("can't set lock, read lock has been set by:%d\n", lock->l_pid);
			}
			else if(lock->l_type == F_WRLCK)
			{
				printf("can't set lock, write lock has been set by:%d\n", lock->l_pid);
			}
			return -2;
		}
	}
	else							//执行失败，返回-1
	{
		perror("get incompatible locks fail");
		return -1;
	}
}

int main()
{
	int fd;
	int ret;
	struct flock  lock;
	char  read_buf[32];

	if((fd = open("MyFile.txt", O_RDWR | O_APPEND)) == -1)
	{
		if((fd = open("MyFile.txt", O_CREAT | O_TRUNC | O_RDWR | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
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

	/*初始化lock结构*/
	memset(&lock, 0, sizeof(struct flock));
	lock.l_start = SEEK_SET;
	lock.l_whence = 0;
	lock.l_len = 0;

	/*设置读锁*/
	lock.l_type = F_RDLCK;
	if(lock_test(fd, &lock) == 0)
	{
		lock.l_type = F_RDLCK;
		lock_set(fd, &lock);
	}

	/*读数据*/
	lseek(fd, 0, SEEK_SET);
	if((ret = read(fd, read_buf, 10)) < 0)
	{
		my_err("read", __LINE__);
	}
	read_buf[ret] = '\0';
	printf("%s\n", read_buf);

	/*等待任意按键*/
	getchar();

	/*设置写锁*/
	lock.l_type = F_WRLCK;
	if(lock_test(fd, &lock) == 0)
	{
		lock.l_type = F_WRLCK;
		lock_set(fd, &lock);
	}

	/*释放锁*/
	lock.l_type = F_UNLCK;
	lock_set(fd, &lock);

	close(fd);

	return 0;
}
