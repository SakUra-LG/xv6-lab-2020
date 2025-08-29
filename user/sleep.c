#include"kernel/types.h"
#include"kernel/stat.h"
#include"user/user.h"

int main(int argc , char *argv[])
{
	if(argc<2)
	{
		fprintf(2,"Usage:sleep <clock>\n");
		exit(1);
	}

	int clock=atoi(argv[1]);
	sleep(clock);
	exit(0);
	
}

