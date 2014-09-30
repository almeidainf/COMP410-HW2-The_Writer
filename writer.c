/*
 * LOYOLA UNIVERSITY CHICAGO
 * COMP 410-001 - OPERATING SYSTEMS - FALL 2014
 *
 * Tiago de Almeida - tdealmeida@luc.edu
 * 1394611
 *
 * Homework #2
 *
 * This program writes TOTAL_DATA to a file through varying buffer sizes and measures the time it takes for the job to complete. The results are written to the results.csv file.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>

/* Debugging macro */
#define db_line()\
	do { fprintf(stderr, "line %d\n", __LINE__); fflush(stderr); } while(0)

#define KBYTE 1024L
#define MBYTE (1024*KBYTE)
#define GBYTE (1024*MBYTE)

// Smallest buffer
#define INITIAL_BUFFER 1
// Biggest buffer
#define FINAL_BUFFER (128*MBYTE)

// Total data size to be written
#define TOTAL_DATA (256*MBYTE)

// Number of repetitions for each run
#define REPS 10

// Macro to calculate time
#define time_calc()\
do{\
	secs = (tf.tv_sec - ti.tv_sec);\
	msecs = (tf.tv_usec - ti.tv_usec)/1000;\
	usecs = (tf.tv_usec - ti.tv_usec)%1000;\
	if(msecs < 0){secs--; msecs += 1000;}\
	if(usecs < 0){msecs--; usecs += 1000;}\
} while(0)

// Data to measure time
struct timeval ti, tf;
long int secs, msecs, usecs;

// Buffer from where the data is read
uint8_t buffer[FINAL_BUFFER];

int main(){

// Dump file to write data
	int dump = open("dump_file", O_WRONLY | O_CREAT);
	if(dump < 0){
		perror("Opening dump file");
		exit(EXIT_FAILURE);
	}

// Results file to write time results
	FILE *results;
	results = fopen("results.csv", "w");
	if(results == NULL){
		perror("Opening results file");
		exit(EXIT_FAILURE);
	}

// Writing label to results file
	uint64_t bsize, wcount, i, rep;
	for(bsize = INITIAL_BUFFER; bsize <= FINAL_BUFFER; bsize *= 2){
		wcount = TOTAL_DATA / bsize;
		fprintf(results, "%d x %d bytes, ", wcount, bsize);
	}
	fprintf(results, "\n");

// Repeting REPS times
	for(rep = 0; rep < REPS; rep++){
		printf("Repetition # %d\n", rep);
		
		// Dump file offset = 0
		if(lseek(dump, 0, SEEK_SET) != 0){
			perror("Repositioning the offset of the file descriptor");
			exit(EXIT_FAILURE);
		}

		// Manages the buffer size and write count
		for(bsize = INITIAL_BUFFER; bsize <= FINAL_BUFFER; bsize *= 2){
			wcount = TOTAL_DATA / bsize;

			// Reporting
			printf("Writing %ld times %ld bytes = %ld bytes\n", wcount, bsize, wcount*bsize);

			gettimeofday(&ti, NULL);
			
			// Writes wcount times the buffer size
			for(i = 0; i < wcount; i++)
				write(dump, &buffer, bsize);

			gettimeofday(&tf, NULL);
			time_calc();

			// Writes time measurements to results file
			fprintf(results, "%ld.%ld%ld, ", secs, msecs, usecs);
		}
		fprintf(results, "\n");
	}

	if(close(dump) != 0)
		perror("Closing dump file");
	
	if(fclose(results) != 0)
		perror("Closing results file");

}
