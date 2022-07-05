#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#define SERVER_PATH "../s_socket"	/* 服务端地址 */
#define CLIENT_PATH "../c_socket"	/* 客户端地址 */

void sig_parent_handle(int sig)
{
	printf("receive process child signal\n");

	if (waitpid(-1, NULL, 0) < 0)
	{
		perror("waitpid error:");
	}
	exit(0);
}

void sig_child_handle(int signo)
{
	if (signo == SIGHUP)
	{
		printf("receive process parent SIGHUP signal\n");
		exit(0);
	}
}

int main(int argc, char* argv[])
{
	int c_fd, pid;
	int ret;
	struct sockaddr_un c_addr;
	struct sockaddr_un s_addr;
	char buf[1024];
	ssize_t size;

	/* 进程通知信号 */
	signal(SIGCHLD, sig_parent_handle);

	/* 创建socket */
	c_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (c_fd < 0)
	{
		perror("socket create failed");
		return -1;
	}

	/* 显式绑定客户端地址，服务端可以获取该地址 */
	memset((char*)&c_addr, 0, sizeof(c_addr));
	c_addr.sun_family = AF_UNIX;
	strncpy(c_addr.sun_path, CLIENT_PATH, sizeof(c_addr.sun_path) - 1);
	unlink(CLIENT_PATH);
	if (bind(c_fd, (struct sockaddr*)&c_addr, sizeof(c_addr)) < 0)
	{
		perror("bind error");
		close(c_fd);
		exit(1);
	}

	/* 服务器端地址 */
	s_addr.sun_family = AF_UNIX;
	strncpy(s_addr.sun_path, SERVER_PATH, sizeof(s_addr.sun_path) - 1);

	/* 连接服务器*/
	ret = connect(c_fd, (struct sockaddr*)&s_addr, sizeof(s_addr));
	if (ret < 0)
	{
		perror("connect server failed");
		close(c_fd);
		unlink(CLIENT_PATH);
		exit(1);
	}
	printf("connect to server: %s\n", s_addr.sun_path);

	/* 创建进程 */
	pid = fork();
	if (pid < 0)
	{
		perror("fork error");
		close(c_fd);
		unlink(CLIENT_PATH);
		exit(1);
	}

	if (pid == 0)		/* 子进程发送消息 */
	{
		signal(SIGHUP, sig_child_handle);
		prctl(PR_SET_PDEATHSIG, SIGHUP);

		for (;;)
		{
			memset(buf, 0, sizeof(buf));
			printf("please enter message to send:\n");
			fflush(stdout);

			memset(buf, 0, sizeof(buf));
			size = read(STDIN_FILENO, buf, sizeof(buf) - 1); /* 从终端读取输入信息 */
			if (size > 0)
			{
				buf[size - 1] = '\0';
			}
			else if (size == 0)
			{
				printf("read is done...\n");
				break;
			}
			else
			{
				perror("read stdin error");
				break;
			}
			if (!strncmp(buf, "quit", 4))
			{
				printf("close the connect!\n");
				break;
			}
			if (buf[0] == '\0')
			{
				continue;
			}
			size = write(c_fd, buf, strlen(buf));
			if (size <= 0)
			{
				printf("message'%s' send failed!errno code is %d,errno message is '%s'\n", buf, errno, strerror(errno));
				break;
			}
		}
	}
	else				/* 父进程接收消息 */
	{
		for (;;)
		{
			memset(buf, 0, sizeof(buf));
			size = read(c_fd, buf, sizeof(buf)); /* 读取服务器消息 */
			if (size > 0)
			{
				printf("message recv %dByte: \n%s\n", (int)size, buf);
			}
			else if (size < 0)
			{
				printf("recv failed!errno code is %d,errno message is '%s'\n", errno, strerror(errno));
				break;
			}
			else
			{
				printf("server disconnect!\n");
				break;
			}
		}
	}
	unlink(CLIENT_PATH);	/* 删除socket文件 */
	close(c_fd);

	return 0;
}