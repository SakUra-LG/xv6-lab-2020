#include"kernel/types.h"
#include"user/user.h"

void prime_cul(int read_fd)
{
	int prime;
	if(read(read_fd,&prime,sizeof(int))==0)		//no num in pipe
	{
		exit(0);
	}

	printf("prime %d\n",prime);

	int p2[2];		//for child 2 child
	pipe(p2);
	int pid=fork();
	if(pid==0)
	{
		close(p2[1]);
		prime_cul(p2[0]);
		exit(0);
	}
	else
	{
		close(p2[0]);
		int n;
		while(read(read_fd,&n,sizeof(int)))
		{
			if(n%prime!=0)
			{
				write(p2[1],&n,sizeof(int));
			}
		}
		close(p2[1]);
		wait(0);
	}
	exit(0);
}

int main(int argc,char *argv[])
{
	int p1[2];	//the 1st pipe for father to put 35nums
	pipe(p1);

	int pid=fork();
	if(pid==0)	//child
	{
		close(p1[1]);
		prime_cul(p1[0]);	

		exit(0);

	}
	else		//father
	{
		close(p1[0]);
		for(int i=2;i<=35;i++)
		{
			write(p1[1],&i,sizeof(int));
		}
		close(p1[1]);
		wait(0);		//wait for children

	}

	exit(0);

}
