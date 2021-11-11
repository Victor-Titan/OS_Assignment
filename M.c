// CPP code to create three child
// process of a parent
#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

char* shared_memory;

union void_cast {
    void* ptr;
    int value;
};

int VOID_TO_INT(void* ptr) {
    union void_cast u;
    u.ptr = ptr;
    return u.value;
}

void* INT_TO_VOID(int value) {
    union void_cast u;
    u.value = value;
    return u.ptr;
}

//Function to convert string to integer, or sti for short. 
int sti(char s[]){
    int num = 0;
    int l = strlen(s);
   
    for(int i=0;i<l-1;i++){
        num+=(int)(s[i]-'0');
        num*=10;
    }
    
    num=num/10;

    return num;
}

void* C3_execution_function(void *arg)
{
 
    FILE* fp;
    fp = fopen("n3.txt" , "r");
    char str[8];
	int sum=0;
 
    

    do {
        
		while(shared_memory!="C3 wake up"){
            printf("[C3]: Locked by monitor...\n");
            //printf("Shared memory inside C3: %s\n",shared_memory);
            sleep(1);
        }

        sum += atoi(str);
    }while(fgets(str,10,fp)!=NULL);


  shared_memory="Die,C3";
  pthread_exit(NULL);
 
}
 
void* C3_monitor_function(void *arg){
	while(shared_memory!="Die,C3"){
        //Just for smooth running, we put to sleep for a few seconds.
        sleep(1);
        printf("[C3 MONITOR THREAD]: Locking execution thread.\n");
        
        int shmid = shmget(ftok("./",65),1024,0666|IPC_CREAT);
        shared_memory=(char*) shmat(shmid,(void*)0,0);

        shared_memory="C3 go to sleep";
        
        
        printf("[C3 MONITOR THREAD]: Unlocking execution thread.\n");
        
        shared_memory="C3 wake up";

		printf("[C3 MONITOR THREAD]: Shared memory: %s",shared_memory);
        sleep(1);

    }
    printf("[C3 MONITOR THREAD]: I'm done.");
	
}
 
void* C2_execution_function(void *arg)
{
    FILE* fp1;
    fp1 = fopen("n2.txt","r");
    char str[8];
   
    
	do{

		while(shared_memory!="C2 wake up"){
            printf("[C2]: Locked by monitor...\n");
            //printf("Shared memory inside C2: %s\n",shared_memory);
            sleep(1);
        }

        int num = atoi(str);
        printf("%d\n" , num);
      } while(fgets(str,10,fp1)!=NULL);
	
	shared_memory="Die,C2";
 
  pthread_exit(NULL);
 
}
 
void* C2_monitor_function(void *arg){
	while(shared_memory!="Die,C2"){
        //Just for smooth running, we put to sleep for a few seconds.
        sleep(1);
        printf("[C2 MONITOR THREAD]: Locking execution thread.\n");
        
        int shmid = shmget(ftok("./",65),1024,0666|IPC_CREAT);
        shared_memory=(char*) shmat(shmid,(void*)0,0);

        shared_memory="C2 go to sleep";
        
        
        printf("[C2 MONITOR THREAD]: Unlocking execution thread.\n");
        
        shared_memory="C2 wake up";

		printf("[C2 MONITOR THREAD]: Shared memory: %s",shared_memory);
        sleep(1);

    }
    printf("[C2 MONITOR THREAD]: I'm done.");
	
}
 

void* C1_execution_function(void* argument){
    
    int arg = VOID_TO_INT(argument);
    printf("Enter number of values when execution thread isn't sleeping:\n");
    int n;
    scanf("%d",&n);

    for(int i=0;i<n;i++){ 
        
        
        while(shared_memory!="C1 wake up"){
            printf("[C1]: Locked by monitor...\n");
            //printf("[C1]: Shared memory inside the while loop: %s\n",shared_memory);

            sleep(1);
        }
          
        printf("Shared memory after the while loop: %s\n",shared_memory);
                
        printf("[C1]: Unlocked by monitor...\n");
        
        //Critical section
        int x;
        scanf("[C1]: Enter a number: %d\n",&x);
        arg += x;  
        //printf("[C1]: Executing.\n");
           
    }
    printf("[C1]: SUM: %d\n",arg);
    shared_memory="Die,C1";
}

void* C1_monitor_function(){
    
    while(shared_memory!="Die,C1"){
        //Just for smooth running, we put to sleep for a few seconds.
        sleep(1);
        printf("[C1 MONITOR THREAD]: Locking execution thread.\n");
        
        int shmid = shmget(ftok("./",65),1024,0666|IPC_CREAT);
        shared_memory=(char*) shmat(shmid,(void*)0,0);

        shared_memory="C1 go to sleep";
        printf("Shared memory: %s",shared_memory);
        
        printf("[C1 MONITOR THREAD]: Unlocking execution thread.\n");
        
        shared_memory="C1 wake up";
        sleep(1);

        printf("[C1 MONITOR THREAD]: Shared memory: %s\n",shared_memory);
        sleep(1);
    }
    printf("[C1 MONITOR THREAD] I'm done.");
}
 

// Driver code
int main()
{
	int pid, pid1, pid2;
    int p1[2],p3[2];
    
    //creating pipes
    if (pipe(p1)==-1)
	{
		fprintf(stderr, "Pipe Failed" );
		return 1;
	}
	if (pipe(p3)==-1)
	{
		fprintf(stderr, "Pipe Failed" );
		return 1;
	}
 
 
	// variable pid will store the
	// value returned from fork() system call
	pid = fork();
 
	// If fork() returns zero then it
	// means it is child process.
	if (pid == 0) {
		pthread_t C1_monitor_thread;
		pthread_t C1_execution_thread;
    
    	long sum = 0;

		//Concurrent execution of both threads
		pthread_create(&C1_monitor_thread , NULL, C1_monitor_function,NULL);
    	pthread_create(&C1_execution_thread , NULL, C1_execution_function,INT_TO_VOID(sum));

		//pthread_join waits for the threads passed as argument to finish(terminate).
    	pthread_join(C1_execution_thread , NULL);
    	pthread_join(C1_monitor_thread, NULL);
    	

		printf("Sum: %ld",sum);

        close(p1[0]);
        write(p1[1],&sum,sizeof(sum));
        close(p1[1]);

		
	}

	else {
        wait(NULL);
		pid1 = fork();
		if (pid1 == 0) {
            //C2
			pthread_t C2_monitor_thread;
			pthread_t C2_execution_thread;
    
			//Concurrent execution of both threads
			pthread_create(&C2_monitor_thread , NULL, C2_monitor_function,NULL);
    		pthread_create(&C2_execution_thread , NULL, C2_execution_function,NULL);

			//pthread_join waits for the threads passed as argument to finish(terminate).
    		pthread_join(C2_execution_thread , NULL);
    		pthread_join(C2_monitor_thread, NULL);
		}
		else {
            wait(NULL);
			pid2 = fork();
			if (pid2 == 0) {
                
                //C3
				pthread_t C3_monitor_thread;
				pthread_t C3_execution_thread;
                long sum2 = 0;
				
				//Concurrent execution of both threads
				pthread_create(&C3_monitor_thread , NULL, C3_monitor_function,NULL);
    			pthread_create(&C3_execution_thread , NULL, C3_execution_function,INT_TO_VOID(sum2));

				//pthread_join waits for the threads passed as argument to finish(terminate).
    			pthread_join(C3_execution_thread , NULL);
    			pthread_join(C3_monitor_thread, NULL);

				//sending output via pipes
                close(p3[0]);
                write(p3[1],&sum2,sizeof(sum2));
                close(p3[1]);
				
                //execlp("./C3.out", "C3", NULL);
			}
 
			// If value returned from fork()
			// in not zero and >0 that means
			// this is parent process.
			else {
				
				wait(NULL);
 
                int c1_sum,c3_sum;
 
                //getting message via pipe from C1
                read(p1[0],&c1_sum,sizeof(c1_sum));
                close(p1[0]);
                printf("C1 output: %d\n",c1_sum);

                //getting message via pipe from C3
                read(p3[0],&c3_sum,sizeof(c3_sum));
                close(p3[0]);
                printf("C3 output: %d\n",c3_sum);
                
				printf("parent --> pid = %d\n", getpid());
			}
		}
	}
 
	return 0;
}