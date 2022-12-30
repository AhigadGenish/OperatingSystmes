#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

//Ahigad Genish
//Ahigad.genish@gmail.com

// reverse funtion for itoa.
void reverse(char s[])
{
    int i , j , n;
    char c;
    n = strlen(s) - 1;

    for (i = 0, j = n; i<j; i++, j--){
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
} 
// itoa funtion.
void itoa(int n, char s[])
{
    int i, sign;
    i = 0;

    if ((sign = n) < 0)  
        n = -n;         
    do {      
        s[i++] = n % 10 + '0';   
    } while ((n /= 10) > 0);     
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}   
// len of the pid to convert to string;
int lenInt(int x){
    int count = 1;
    if(x < 0)
        x *= -1;

    while(x >= 10){
        x /= 10;
        count += 1;
    }
    return count;
}
// getSize of line in file.
int getSize(int fd) {
	int ans = 0;
	char ch;
	while (read(fd, &ch, 1) > 0) {
		if (ch == '/0' || ch == '\n')
			break;
		ans += 1;
	}
	return ans + 1;
}
// get the solution from server and print the result, waiting for signal and handle it here.
void clientHandler(int sig){
    

    char* file,*myPid,*solution;
    char ch;
    int r , fdToClie,pid,lenMyPid,resLen, i = 0;
    char* txt = "to_client_";
    pid = getpid();
    lenMyPid = lenInt(pid);
    myPid = (char*)malloc(lenMyPid+1);
    itoa(pid,myPid);     
    file = (char*)malloc(strlen(txt) + strlen(myPid) + strlen(".txt"));
    strcpy(file,txt);
    strcat(file,myPid);
    strcat(file,".txt");

    fdToClie = open(file, (O_RDWR), 0666);
    if(fdToClie < 0){
        perror("Cannot open file");
        exit(-1);
    }
    printf("The result of your question is:\n");
    resLen = getSize(fdToClie);
    close(fdToClie);
    solution = (char*)malloc(resLen);
    fdToClie = open(file, (O_RDWR), 0666);
    if(fdToClie < 0){
        perror("Cannot open file");
        exit(-1);
    }
    while (i < resLen && read(fdToClie, &ch, 1) > 0) {
		if (ch == '/0' || ch == '\n')
			break;
		solution[i] = ch;
		i += 1;
	}
    // close the file and remove it and deallocate memory;
    close(fdToClie);
    printf("%s\n",solution);
    remove(file);
    free(file);
    free(myPid);
    free(solution);

    return;
}
// client main:
// argv[0] = name of the program, argv[1] = serverPid, argv[2] = first arg
// argv[3] = operator , argv[4] = second arg

void main(int argc, char* argv[]){

    int i , fdToSrv,numOfTry,rnd,size,bytes,signalNum,pid,lenMyPid;
    char* str[4];
    if(argc < 5){
        perror("not enough arguments");
        exit(-1);
    }
    signalNum = 1;
    signal(signalNum,clientHandler);
   
    srand(time(NULL));   
    numOfTry = 10;
    for(i = 0; i < numOfTry ;i++){
        fdToSrv = open("to_srv.txt", (O_WRONLY), 0666);
        // exists
        if(fdToSrv >= 0){
            close(fdToSrv);
            rnd = rand()%5 + 1;  
            sleep(rnd);
        }
        else{
            fdToSrv = open("to_srv.txt", (O_WRONLY | O_CREAT | O_APPEND), 0666);
            if(fdToSrv < 0){
                perror("Cannot create file");
                exit(-1);
            }
            break;
        }
    }
    if(i == 10){
        perror("Cannot connect to server ..");
        exit(-1);
    }
    // write arguments to the text file. any argument in different line.
    for(i = 1 ;i < argc ; i++){
        printf("%d\n",i);
        if( i == 1){
            pid = getpid();
            lenMyPid = lenInt(pid);
            str[0] = (char*)malloc(lenMyPid);
            itoa(pid,str[0]);
            str[0] = realloc(str[0],strlen(str[0]) + 1);
            strcat(str[0],"\n");
            bytes = write(fdToSrv,str[0],strlen(str[0]));
           
        }
        else{
            size = strlen(argv[i]);
            str[i-1] = (char*)malloc(size+1);
            strcpy(str[i-1],argv[i]);
            strcat(str[i-1],"\n");
            bytes = write(fdToSrv,str[i-1],strlen(str[i-1]));
           
        }
        if(bytes <= 0 ){
            perror("Cannot write to the file");
            exit(-1);
        }
    }
    // close the file
    close(fdToSrv);
    // send signal to server
    kill(atoi(argv[1]),1);
    // wait for solution from the server
    pause();
    // deallocate memory
    for(i = 0; i < 4; i++){
        free(str[i]);
    }

    return;

    
}