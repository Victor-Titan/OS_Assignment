#include<stdio.h>
#include<string.h>
#include <unistd.h>



int main(){
    char s[5];
    printf("Enter choice of scheduling algorithm: ");
    scanf("%s",s);

    printf("Reached here.");
    
    if(strcmp(s,"fcfs")==0) execl("./fcfs",NULL,NULL);
    else if(strcmp(s,"rr")==0) execl("./rr",NULL,NULL);
    else printf("Invalid option.\n");
    return 0;
    
}