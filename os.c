#include <stdio.h> // for getcwd reading directory function
#include <errno.h> // same as above
#include <stdlib.h> // for reading file 
#include <unistd.h> 
#include <string.h> 

#define _GNU_SOURCE // Used for getline() function - note this function will work on linux machines mostly. 
char input[1000]; // whole line input user types in 
char *command; // individual command from input  
char *arguements[500]; // For holding arguements 
char path[500]; // path and home variables to store our path values
char home[500];
char *pathCheck = "$PATH="; // path and home checks - used when someone modifies the the path/home from our shell 
char *homeCheck = "$HOME=";

int main (void) 
{
 	
	printf( "Extra Shell tips:  \n");
	printf( "1) Type exit to.. exit from shell. \n");
	printf( "2) To change path/home variables, type in the format of $PATH=  \n");

	/* 1) Load in the profile file we've made and store the path and home values in variables and then setenv() - reads line by line */	
   	FILE *fp;
  	char *line = NULL;
  	size_t len = 0;
   	size_t read;
	
 	fp = fopen(".profile", "r");
   	if (fp == NULL) 
   	{
   		printf("Specified profile file doesn't exist - exiting Mahi shell \n");
      	exit(EXIT_FAILURE); 
  	}
	
	while ((read = getline(&line, &len, fp)) != -1) 
	{
		char *foundPath = strstr(line, "PATH="); // check if line contains 'PATH=' string - strstr is a substring check
		if(foundPath) 
		{
			// copy into path array 
			memcpy( path, line, strlen(line)-1 ); // memcopy - http://stackoverflow.com/questions/4214314/get-a-substring-of-a-char
			path[strlen(line)] = '\0';

			char *shorten = path;
			memmove(&shorten[0], &shorten[5], (strlen(shorten) - 5 + 1)); // this code removes the PATH= from the path variable - http://stackoverflow.com/questions/12440212/remove-a-substring-in-c-given-positions-of-substring

			setenv("PATH", path, 1); // set the environment variable 
			/* To Print - char* pPath = getenv ("PATH"); printf("Current path is: %s \n", pPath); */
		}
	
		char *foundHome = strstr(line, "HOME=");
		if(foundHome) 
		{
			memcpy( home, line, strlen(line) );
			home[strlen(line)] = '\0';

			char *shorten2 = home;
			memmove(&shorten2[0], &shorten2[5], (strlen(shorten2) - 5 + 1)); // this code removes the HOME= from the home variable 
	
			setenv("HOME", home, 1);
		}

		if(!(foundHome || foundPath)) { // if neither found on a line - will either be home or profile on a line so neither = one or the other not set
			printf("Path or home variable not set in profile \n");
		}				
   	}
	
 	fclose(fp);
   	if (line) 
   	{
       free(line); // free pointer 
	}

	/* 2) Loop - run terminal continously after each command until user quits - Display current directory + > in terminal promt each time  */
	int endShell = 0;
	while (endShell ==0) 
	{ 

		char cwd[1024]; // current working directory 
		if (getcwd(cwd, sizeof(cwd)) != 0) 
		{
	 		fprintf(stdout, "[%s]> ", cwd); /* cwd is current working directory  - [ + ] added just for formatting s */
		}
		else 
		{
			perror("getcwd() error");
		}	
		
		/* 3) Read in entered input - first word is program to run (e.g. ls), second is arguements */
	  	fgets(input,500,stdin); // read in users input 
		arguements[0] = strtok(input, " \n"); //retrieve command from input - strtok () - http://stackoverflow.com/questions/4160273/strtok-with-space-delimiter
		command = arguements[0]; 
		
		/* 4) Modify path and home from terminal - Don't need to edit file teacher said, won't get extra marks.  */
		char *pathChange = strstr(input, pathCheck); // check if user's trying to edit home/path variables 
		char *homeChange = strstr(input, homeCheck);

		if(pathChange || homeChange) 
		{
			if(pathChange) 
			{
				memset(path, 0, sizeof path); // clear old path 
				memcpy(path, input, strlen(input) ); // memcopy - copies string from input into path - http://stackoverflow.com/questions/4214314/get-a-substring-of-a-char
				path[strlen(input)] = '\0';

				char *shorten = path;
				memmove(&shorten[0], &shorten[6], (strlen(shorten) - 6 + 1)); // this code removes the $PATH= from the path variable 
				
				setenv("PATH", path, 1); 
				char* pPath = getenv ("PATH");				
				printf("New path is: %s \n", pPath); 
			}

			else if (homeChange) 
			{	
				memset(home, 0, sizeof home);
				memcpy( home, input, strlen(input) );
				home[strlen(input)] = '\0';

				char *shorten2 = home;
				memmove(&shorten2[0], &shorten2[6], (strlen(shorten2) - 6 + 1)); // this code removes the HOME= from the home variable 

				setenv("HOME", home, 1);
				char* pHome = getenv ("HOME");
				printf("New home is: %s \n", pHome);
			}
			pathChange = NULL; // free variables for next run 	
			homeChange = NULL; 
		}	


		else if (command == NULL) 
		{ 		// command  empty 
				printf("Command is emtpy, please try again \n");
		}	

		else if (command != NULL)  
		{ 
			if(strcmp(command,"exit") == 0) //  to exit shell
			{
				endShell = 1;
			}

			else if(strcmp(command,"cd") == 0) 
			{
				/*4) Handle CD command manually as not a program found in path. Cases - a)  */

				getcwd(cwd, sizeof(cwd));   // get current working directory 

				char *newDirectory;
				char *goToDirectory;

				arguements[1] = strtok(NULL, " \n");
				goToDirectory = arguements[1]; // get arguements to cd 
			
				if(goToDirectory == NULL)  // no arguements to cd 
				{					
					newDirectory = getenv("HOME"); // get home environment variable and go there when just 'cd' run		
				}

				else if(strcmp(goToDirectory, "..") == 0) // if specified directory is '..'' go up a level 
				{	
					char *CurrentDirectory = cwd; 
					char *pointerTo;
					pointerTo = strrchr(CurrentDirectory,'/'); // strrchr() finds the last occurence of '/' and places everything after it into a CurrentDirectory
					*pointerTo = '\0'; // end 
					newDirectory = CurrentDirectory;
					// printf("Newdirectory is: %s \n", newDirectory);
				}
				else if(strcmp(goToDirectory, "..") ==1) // if == 1 then a specific folders been specified 
				{
				    newDirectory = strcat(cwd, "/"); // join strings
					newDirectory = strcat(newDirectory, goToDirectory);
				}			
								  
				if(chdir(newDirectory) != 0) // chdir to newDirectory, give error if it doesn't succesffuly change directory 
				{
				    printf("Current Home Directory does not exist\n");
				}
				
				goToDirectory = NULL; // clear variables 
				newDirectory = NULL;
			}		
			
			else 
			{
				/* 5) Fork new process and run program.  Fork first and then run execvp from it. Break arugements into tokens. */
				char *tokens = strtok(NULL, " \n");
				int n = 1;
				
				while(tokens != NULL) // go through each token 
				{
					arguements[n] = tokens;
					tokens = strtok(NULL, " \n");
					n++;
				}
				
				if(fork() == 0) 
				{
					execvp(arguements[0],arguements); // run command and arguements 
					exit(0);
				}
				wait(NULL);
			}
		}

		// Clear variables 
		command = NULL;

		int m = 0;
		for(m = 0; m < 500; m++) 
		{
			arguements[m] = NULL;
		}	
		
	} // end while loop

return 0;
}


	 




