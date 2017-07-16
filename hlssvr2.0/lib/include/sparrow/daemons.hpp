#ifndef DAEMONS_HPP_201201161702
#define DAEMONS_HPP_201201161702

#include <unistd.h> 
#include <signal.h> 
#include <sys/param.h> 
#include <sys/types.h> 
#include <sys/stat.h> 

//后台进程

namespace sparrow{
	int run_as_daemons(void){
		int pid; 
		pid = fork();

		if(pid == -1) return -1;
		if(pid != 0) {
			exit(0);
		}

		setsid();

		pid = fork();
		if(pid == -1) return -2;
		if(pid != 0) {
			exit(0);
		}

		umask(0);
		//for(int i = 3; i < NOFILE; ++ i) close(i);
		
		signal(SIGHUP, SIG_IGN);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGUSR1, SIG_IGN);
		signal(SIGUSR2, SIG_IGN);
		
		return 0;
	};

	void set_term_handle(__sighandler_t handler){
		//signal(SIGKILL, handler);
		signal(SIGTERM, handler);
	};


}

#endif




