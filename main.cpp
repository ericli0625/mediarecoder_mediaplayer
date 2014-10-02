#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include "MyThread.h"
#include <string.h>
using namespace android;

int main(int argc , char *argv[]){

	char *str = argv[1];

	//printf("c=%d,v=%s\n",argc,argv[1]);
	printf("Input 'c' or 'm' \n");

	if(argc < 2 || argc > 2)
	{
		return 0;	
	}

	if((str[0]=='m'&&strlen(str)==1) || (str[0]=='c'&&strlen(str)==1)){
		sp<MyThread> thread;
		printf("Start up\n");

		thread = new MyThread(str[0]);

		sp<ProcessState> proc(ProcessState::self());
		proc->startThreadPool();

		thread->run("MyThread");
		thread->join();
		return 0;
	}

}
