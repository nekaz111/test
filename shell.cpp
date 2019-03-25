//Jeremiah Hsieh ICSI 500 Project 1 Shell
//exit command
//command with no arguements
//command with argument
//command with/without arguments in background using &
//command with/without arguments output redirected to file


//commands verified to work
//cp, vim, rm, ls, sort, mv, wc 
//was not quite able to get pipes working properly with multiple child process calls


#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <fstream> 
#include <fcntl.h>
#include <unistd.h>

//functions
int parseInput(std::string line);
int execute(std::vector<char *> parsed, int waitstate, int filestate);


//loops and gets user input 
int main() 
{
	int running = 1;
	std::string line = "";
	//while loop gets input until exit command is recieved
	while (running == 1)
	{
		//get input from user
		std::cout << "my command line> ";
		getline(std::cin, line);
		

		//check if commands are input
		if (line.empty() != true) {
			//parse commands from user input
			running = parseInput(line);
		}
	}
	return 0;
}


//use execl and execvp 
//char *argv[] = { "ls", "-l", 0 };
//execvp(argv[0], argv);
//execv("/bin/ls", argv);

//get commands from line
int parseInput(std::string line)
{
	//vector to store parsed commands
	std::vector<char *> parsed;
	//lazy strtok from string 
	char* cline = strdup(line.c_str()), *split;
	int filestate = 0, waitstate = 1;
	//split by space token and store in vector
	for(split = strtok(cline, " "); split != NULL; split = strtok(NULL, " "))
	{
		//add string to vector
		parsed.push_back(split);
		//check for file redirect
		if (strcmp(split, ">") == 0)
		{
			filestate = 1;	
		}
		//redirect file to input
		else if (strcmp(split, "<") ==0)
		{
			filestate = 2;
		}
	}

	//exit program condition
        if (strcmp(parsed[0], "exit") == 0)
	{
		//ignore if exit has excess (?)
	        if (parsed.size() > 1) 
		{
			std::cout << "Too many arguements for exit\n";
		}
		//call exit
		else
		{
			std::cout << "System exiting\n";
	        	exit(0);
		}
	}
	//don't check by specific command just parse between pipe calls?
	else
	{
		//check if program wants to do it in background
		//int waitstate = 1;
		if(strcmp(parsed[parsed.size()-1], "&") == 0)
        	{
                	waitstate= 0;
                	//parsed.erase(parsed.begin() + parsed.size()-1);
                	parsed.pop_back();
		}
		//vector data arguement for execvp must end in NULL
		parsed.push_back(NULL);
		//call function to execute fork and execvp command file
		execute(parsed, waitstate, filestate);
	}
	return 1;
}



//fork and call process based on command line input
int execute(std::vector<char *> command, int waitstate, int filestate)
{
	
	//int infile[2], outfile[2];
	//check if pipes are needed
	/*if (filestate != 0)
	{
		//make pipes and error checking
		if(pipe(infile) == -1)
        	{
                	perror("pipe");
	                exit(EXIT_FAILURE);
	        }
		if(pipe(outfile) == -1)
	        {
	                perror("pipe");
        	        exit(EXIT_FAILURE);
	        }

	}*/
	
	

	//fork process and run command with child
        id_t pid = fork();
        //error checking
        if (pid == -1)
        {
        	perror("forK error");
        	return EXIT_FAILURE;
        }
        //child gets replaced by command line program specified
        else if (pid == 0)
	{
		//printf("This is the child process. My pid is %d and my parent's id is %d.\n", getpid(), getppid());
		
		//apparently sigset is depreciated?
		//background program condition
		if (waitstate == 0)
		{
			//seperate child id(so it can run in background after parent)
			setpgid(0, 0);
		}
		//stop process if invalid call otherwise execvp replaces parent process with command line file
		//redirect file
		if (filestate != 0)
		{
			//close read end of pipe
			//close(outfile[0]);
			//redirect write end and stdout
			//dup2(outfile[1], 1);

			//close(outfile[1]);
			
			//open file and store file descriptor fd
			int fd = open(command[command.size()-2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			//dup file to stdout of process for >
			if (filestate == 1) 
			{
				dup2(fd, 1);
			}
			//dup file to stdin of process for <
			else if (filestate = 2)
			{
				dup2(fd, 0);
			}
			//lazy remove last 2 command line arguements
			//actually the vector data function ends on NULL termination
			command[command.size()-2] = NULL;
			command[command.size()-3] = NULL;
			//not sure why pop_back doesn't work?
			//its cuz execvp vector data arguement must be null terminated
		}
		

		//replace child with command program
		if (execvp(command[0], command.data()) == -1)
        	{
			//print error if problem occurred
			std::cout << "exec problem error code " << errno << " invalid command\n";
                	exit(errno);
        	}
       	}
        //parent waits for child (or doesn't if & is included)
        else
        {
		//printf("This is the parent process. My pid is %d and my parent's id is %d.\n", getpid(), pid);
                int status;
		//background child
		if (waitstate == 0)
		{
			waitpid(-1, &status, WNOHANG);
		}
		//regular child
		else 
		{
			waitpid(pid, &status, 0);
		}
	}
	
}
