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

void sig_parent_handle(int signo)
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
	int s_fd, c_fd, pid;
	socklen_t addr_len;
	struct sockaddr_un s_addr;
	struct sockaddr_un c_addr;
	char buf[1024];
	ssize_t size = 0;

	/* 进程通知信号 */
	signal(SIGCHLD, sig_parent_handle);

	/* 创建socket */
	if ((s_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		perror("socket create failed:");
		exit(1);
	}

	/* 绑定地址 */
	memset((char*)&s_addr, 0, sizeof(s_addr));
	s_addr.sun_family = AF_UNIX;
	strncpy(s_addr.sun_path, SERVER_PATH, sizeof(s_addr.sun_path) - 1);/* 服务器端地址 */
	if (bind(s_fd, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0)
	{
		perror("bind error");
		close(s_fd);
		exit(1);
	}

	/* 监听socket */
	if (listen(s_fd, 5) < 0)	/* 最大监听5个客户端 */
	{
		perror("listen error");
		close(s_fd);
		exit(1);
	}
	printf("waiting client connecting\n");

	addr_len = sizeof(struct sockaddr);
	c_fd = accept(s_fd, (struct sockaddr*)&c_addr, (socklen_t*)&addr_len);

	if (c_fd < 0)
	{
		perror("accept error");
		close(s_fd);
		unlink(SERVER_PATH);
		exit(1);
	}
	else
	{
		printf("connected with socket: %s\n", c_addr.sun_path);
	}

	/* 创建进程 */
	pid = fork();
	if (pid < 0)
	{
		perror("fork error");
		close(s_fd);
		unlink(SERVER_PATH);
		exit(1);
	}

	if (pid == 0)  /* 子进程发送消息 */
	{
		signal(SIGHUP, sig_child_handle);
		prctl(PR_SET_PDEATHSIG, SIGHUP);

		for (; ;)
		{
			printf("please enter message to send:\n");
			fflush(stdout);
			memset(buf, 0, sizeof(buf));
			size = read(STDIN_FILENO, buf, sizeof(buf) - 1);
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
	else 			/* 父进程接收消息 */
	{
		for (;;)
		{
			memset(buf, 0, sizeof(buf));
			size = read(c_fd, buf, sizeof(buf) - 1);
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
				printf("client disconnect!\n");
				break;
			}
		}
	}
	unlink(SERVER_PATH);	/* 删除socket文件 */
	close(s_fd);
	close(c_fd);

	return 0;
}
