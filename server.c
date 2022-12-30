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
// server handler.
// flag : if 0 its okey. if 1 - try divide by zero, if 2: wrong input.
void serverHandler(int sig){
    printf("got a servie\n");
    int signalNumber = 1;
    int fdToSrv,fdToCli ,i, j , pid ,fd,bytes, operator, x , y ,resSize, result, flag = 0,numOfLines = 4;
    int arrSize[4];
    char* arg[4];
    char ch;
    char* divZero = "Cannot divide by zero";
    char* wrongInpt = "Wrong input";
    char* solution;
    char* file;
   
    signal(signalNumber,serverHandler);
    

    fd = open("to_srv.txt", (O_RDONLY), 0666);
    if(fd < 0){
        perror("Cannot open file");
        return;
    }
    // get size of the lines.
    for(i = 0;i<numOfLines;i++){
        arrSize[i] = getSize(fd);
        printf("the size of the line %d is : %d \n",i,arrSize[i]);
    }
    close(fd);
    fdToSrv = open("to_srv.txt", (O_RDONLY), 0666);
    if(fdToSrv < 0){
        perror("Cannot open file");
        return;
    }
    // read from file.
    // arg[0] = pid of client. arg[1] = first arg, arg[2] = operator,arg[3] = second arg.
    for( i = 0;i<numOfLines;i++){
        arg[i] = (char*)malloc(arrSize[i]+1);
        j = 0;
        while (j < arrSize[i] && read(fdToSrv, &ch, 1) > 0) {
            if (ch == '/0' || ch == '\n')
                break;
            arg[i][j] = ch;
            j += 1;
	    }

    }
    close(fdToSrv);
    remove("to_srv.txt");
    // fork
    pid = fork();
    if(pid < 0){
        perror("Cannot fork");
        return;
    }
    // child process:
    if(pid == 0){
        
        operator = atoi(arg[2]);
        switch(operator){
            case 1:{
                // add
                x = atoi(arg[1]);
                y = atoi(arg[3]);
                result = x + y;
                break;
            }
            case 2:{
                // subtract
                x = atoi(arg[1]);
                y = atoi(arg[3]);
                result = x - y;
                break;
            }
                
            case 3:{
                // multiply

                x = atoi(arg[1]);
                y = atoi(arg[3]);
                result = x * y;
                break;
            }
            case 4:{
                // divide

                x = atoi(arg[1]);
                y = atoi(arg[3]);
                if( y == 0){
                    flag = 1;
                    break;
                }

                result = x / y;
                break;
            }
            default:{
                flag = 2;
                break;
            }
                
        }
        // size = strlen("to_client_")  + strlen(ClientPid)
        file = (char*)malloc(strlen("to_client_") + strlen(arg[0]) +strlen(".txt"));
        strcpy(file,"to_client_");
        strcat(file,arg[0]);
        strcat(file,".txt");
        fdToCli = open(file, ( O_CREAT | O_WRONLY ), 0666);
        if(fdToCli < 0){
            perror("Cannot open file");
            return;
        }
        if(flag == 0){
            resSize = lenInt(result);
            solution = (char*)malloc(resSize + 1);
            itoa(result,solution);
            resSize = strlen(solution);
            bytes = write(fdToCli,solution,strlen(solution));
            if(bytes <= 0 ){
                perror("Cannot write to the file");
                return;
            }
            free(solution);

        }
        else if(flag == 1){
            bytes = write(fdToCli,divZero,strlen(divZero));
            if(bytes <= 0 ){
                perror("Cannot write to the file");
                return;
            }
            
        }
        else if(flag == 2){
            bytes = write(fdToCli,wrongInpt,strlen(wrongInpt));
            if(bytes <= 0 ){
                perror("Cannot write to the file");
                return;
            }
        }
        close(fdToCli);
        free(file);
        kill(atoi(arg[0]),1);
        for(i = 0;i<numOfLines;i++){
            free(arg[i]);
        }
    }
    // father process:
    else{
        
        return;
    }
    

    
}

// server main
// get only the program name
void main(int argc,char* argv[]){
    int sigNum = 1;
    signal(sigNum,serverHandler);
    while(1){
        pause();
    }
    return;
}