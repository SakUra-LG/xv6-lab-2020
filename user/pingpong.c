#include"kernel/types.h"
#include"user/user.h"

int main(int argc,char *argv[])
{
	int p2c[2];//pipe for father process to child 	
	int c2p[2];///pipe for child process to father
	pipe(c2p);
	pipe(p2c);


	int pid = fork();
	if(pid==0)
	{
		close(p2c[1]); // 子进程不写
		close(c2p[0]); // 子进程不读
		
		char buffer[1];		//create a buffer to receive litter from father process
		int MyPid = getpid();
		read(p2c[0],buffer,1);
		if(buffer[0]=='A')		///to make sure that the children have received the exact message from father
		{

			printf("%d: received ping\n",MyPid);
			write(c2p[1],"B",1);
			
		}
		exit(0);
		

	}
	else
	{
	
		close(p2c[0]); // 父进程不读
		close(c2p[1]); // 父进程不写
		
		char buffer2[1];		//create a buffer to receive litter from children process
		int MyPid = getpid();
		write(p2c[1],"A",1);

		read(c2p[0],buffer2,1);
		if(buffer2[0]=='B')		//to make sure that the father have received the exact message from children
		{
			printf("%d: received pong\n",MyPid);
			
		}
		exit(0);
	}
	exit(0);
}
