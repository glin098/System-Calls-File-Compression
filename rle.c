#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFERSIZE 1024

int main(int argc, char* argv[]){
	if (argc != 5){
		printf("error: program expects <inputFile> <outputFile> <runLength> <mode>\n");
		
		return -1;
	}
	char* inputFile;
        char* outputFile;
        int runLength;
        int mode;
        int counter = 0;

	inputFile = argv[1];
    	outputFile = argv[2];
    	runLength = atoi(argv[3]);
    	mode = atoi(argv[4]);
	
	if(runLength < 1){
		perror("Compression length cannot be less than 1");
		exit(-1);	
	}

    	printf("got input file: %s\n", inputFile);
    	printf("got output file: %s\n", outputFile);
    	printf("got runLength: %d\n", runLength);

    	char buffer[BUFFERSIZE];
	
    	//open input file
    	int inFD = open(inputFile, O_RDONLY); // open for reading only
        if(inFD == -1){
                perror("Could not open input file");
                exit(-1);
        }

	//Creating variable to store what is being compared and compressed
	char* currentRun[runLength+1];
	//adding the NULL to the end of currentRun;
	currentRun[runLength] = '\0';


	// compressing file
    	if(mode == 0){
		int bytesRead = read(inFD, buffer, runLength);
        	buffer[bytesRead] = '\0';
        	counter = 0;
		memcpy(currentRun, buffer, runLength);	
		
		int outFD = open(outputFile,
			O_WRONLY | O_CREAT | O_TRUNC,
		      	S_IRUSR | S_IWUSR);

		if(outFD == -1){
			perror("Could not open output file");
			exit(-1);
		}

		while(bytesRead > 0){
			bytesRead = read(inFD, buffer, runLength);
			
			counter++;

			if (strncmp(currentRun, buffer, runLength) == 0){
				if(counter == 255){
					write(outFD, &counter, 1);
					bytesRead = write(outFD, currentRun, runLength);
					if(bytesRead == -1){
						perror("File could not be written to");
						exit(-1);
					}
					counter = 0;
					
					memcpy(currentRun, buffer, bytesRead);
				}
				


			}else{
				write(outFD, &counter, 1);
				bytesRead = write(outFD, currentRun, runLength);
				if(bytesRead == -1){
					perror("File could not be written to");
					exit(-1);
				}
				counter = 0;
				memcpy(currentRun, buffer, bytesRead);
			}
		}
		write(outFD, &counter, 1);
		write(outFD, currentRun, 1);
	}

	// decompressing file
	else if(mode == 1){
		int bytesRead = read(inFD, buffer, 1);
		buffer[bytesRead] = '\0';
		
		int numRepetitions = buffer[0];		

		int outFD = open(outputFile,
			O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR);

		if( outFD == -1 ){
			perror("Could not open output file");
			exit(-1);
		}
	
		while(bytesRead > 0){
			bytesRead = read(inFD, buffer, runLength);
			memcpy(currentRun, buffer, runLength);
			
			for(int i = 0; i < numRepetitions; i++){		
				bytesRead = write(outFD, currentRun, runLength);
				
				if(bytesRead == -1){
					perror("File could not be written to");
					exit(-1);
				}
			}

			bytesRead = read(inFD, buffer, 1);
			numRepetitions = buffer[0];
		}
	}
	else if(mode != 1 || mode != 0){
		perror("Enter a mode value of 0 or 1");
		exit(-1);
	}
	return 0;
}
