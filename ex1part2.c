#include <stdio.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
// ahigad genish
// ahigad.genish@gmail.com

// inputs : name, path
// path = config.txt conatains the path of the submition dir, input file and expected output file.
// this program check if student code output equal to the expected output.

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
// check grade function 
// inputs: file descriptor of results, string of submit path, array of arguments to run the student program.
// expected output and name of student.

void checkGrade(int fdRes, char* submit, char* arr[], char* expecOut, char* name) {

	// initialize variables and create output file.
	pid_t pid1, pid2;
	int ret, retVal, stat, fdSc, fdNew;
	char* grade;
	char* path = "./ex1part1.out";
	char* files[4] = { path,"outPro.txt",expecOut,NULL };
	int fdOut = open("outPro.txt", (O_WRONLY | O_CREAT | O_TRUNC), 0666);

	if (fdOut < 0) {
		perror(" cannot open file  ");
		exit(-1);
	}


	// first fork : run student program with the inputs.
	pid1 = fork();
	if (pid1 < 0) {
		perror("unable to fork");
	}

	// after fork
	if (pid1 == 0) {
		// child
		close(1);
		dup(fdOut);
		close(fdOut);
		ret = execvp(submit, arr);
		if (ret == -1)
		{
			perror("exec failed ");
			exit(-1);
		}
	}
	else {
		// father wait for child
		if (waitpid(pid1, NULL, 0) != pid1) {
			perror("waitpid error");
		}

		// the second fork
		// fork for check if the output file equal to the expected file by running part1 program.
		pid2 = fork();
		// after fork
		if (pid2 < 0) {
			perror("unable to fork");
		}
		if (pid2 == 0) {
			// child
			ret = execvp(path, files);
			if (ret == -1)
			{
				perror("exec failed ");
				exit(-1);
			}

		}
		else {
			// father
			wait(&stat);
			retVal = WEXITSTATUS(stat);
			// check if equal or not
			if (retVal == 2) {
				// grade = 100
				grade = (char*)malloc(strlen(name) + 5);
				strcat(grade, name);
				strcat(grade, ",100");

			}
			else {
				// grade = 0
				grade = (char*)malloc(strlen(name) + 3);
				strcat(grade, name);
				strcat(grade, ",0");

			}
			if (fdRes < 0) {
				perror(" cannot open file  ");
				exit(-1);
			}
			// print the grades to the results file
			fdSc = dup(1);
			close(1);
			dup(fdRes);
			close(fdRes);
			printf("%s\n", grade);
			close(fdRes);
			dup(1);
			close(fdSc);

			// deallocate memory
			free(grade);

		}

	}
	// remove temporary file
	remove("outPro.txt");
	return;
}
int main(int argc, char* argv[]) {
	// initialize variables and open the config file

	char * submit;
	char* input;
	char* expecOut;
	char* temp;
	DIR *dir;
	int i = 0;
	char* arr[4];
	struct dirent *entry;
	char ch;
	int j;
	int subSize, inSize, outSize;
	int fd2, pid1, len, fdResult, fd3, len1, len2;
	int fd1 = open(argv[1], O_RDONLY);
	if (fd1 < 0) {
		perror(" cannot open file  ");
		exit(-1);
	}
	// getSize of the lines in order to allocate dynamically
	subSize = getSize(fd1);
	inSize = getSize(fd1);
	outSize = getSize(fd1);
	close(fd1);
	//allocate the paths strings
	submit = (char*)malloc(subSize);
	input = (char*)malloc(inSize);
	expecOut = (char*)malloc(outSize);
	fd1 = open(argv[1], O_RDONLY);
	if (fd1 < 0) {
		perror(" cannot open file  ");
		exit(-1);
	}
	while (i < subSize && read(fd1, &ch, 1) > 0) {
		if (ch == '/0' || ch == '\n')
			break;
		submit[i] = ch;
		i += 1;
	}
	i = 0;

	while (i < inSize && read(fd1, &ch, 1) > 0) {
		if (ch == '/0' || ch == '\n')
			break;
		input[i] = ch;
		i += 1;
	}
	i = 0;
	while (i < outSize && read(fd1, &ch, 1) > 0) {
		if (ch == '/0' || ch == '\n')
			break;
		expecOut[i] = ch;
		i += 1;
	}
	close(fd1);
	fd3 = open(input, O_RDONLY);
	if (fd3 < 0) {
		perror(" cannot open file  ");
		exit(-1);
	}
	// get arguments from inputs file 
	// assume arguments are two lines one after line.
	len1 = getSize(fd3);
	arr[1] = (char*)malloc(len1);
	len2 = getSize(fd3);
	arr[2] = (char*)malloc(len2);
	arr[3] = NULL;
	close(fd3);
	fd3 = open(input, O_RDONLY);
	if (fd3 < 0) {
		perror(" cannot open file  ");
		exit(-1);
	}
	i = 1;
	j = 0;
	while (read(fd3, &ch, 1) > 0) {
		if (ch == '\n') {
			j = 0;
			i = 2;
		}
		else {
			arr[i][j] = ch;
			j += 1;
		}
	}
	close(fd3);

	// open submission dir 
	dir = opendir(submit);
	if (dir == NULL) {
		perror(" cannot open dir ");
		exit(-1);
	}
	i = 0;
	// open results file (grades)
	fdResult = open("results.csv", (O_WRONLY | O_CREAT | O_TRUNC), 0666);
	if (fdResult < 0) {
		perror(" cannot open file  ");
		exit(-1);
	}
	// run by iterator on the dir
	while ((entry = readdir(dir)) != NULL) {
		// check if the directory is name of student
		if (entry->d_name[0] != '.') {
			// update temp to be the string of the compiled program 
			// concatenate the path and the name of the students and .out
			// assume that all student submit the c file and the compiled file into the directory.
			temp = (char*)malloc(subSize);
			strcat(temp, submit);
			len = strlen(entry->d_name);
			// the string is path + / + name + / + name + .out
			temp = (char*)realloc(temp, subSize + 1 + len + 1 + len + 4);
			strcat(temp, "/");
			strcat(temp, entry->d_name);
			strcat(temp, "/");
			strcat(temp, entry->d_name);
			strcat(temp, ".out");
			arr[0] = (char*)malloc(strlen(entry->d_name) + 6);
			strcat(arr[0], "./");
			strcat(arr[0], entry->d_name);
			strcat(arr[0], ".out");
			// call check grade to run the
			checkGrade(fdResult, temp, arr, expecOut, entry->d_name);
			// free allocate memory
			free(arr[0]);
			free(temp);
		}
	}
	// close grade file and submit directory
	close(fdResult);
	closedir(dir);
	// deallocate memory:
	if (submit) {
		free(submit);
		submit = NULL;
	}
	if (input) {
		free(input);
		input = NULL;
	}
	if (expecOut) {
		free(expecOut);
		expecOut = NULL;
	}
	for (i = 1; i < 4; i++) {
		if (arr[i] != NULL)
			free(arr[i]);
	}

	// done
	return 0;
}