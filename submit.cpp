/**
 * Assignment 5: Life in Cilk
 * Team Member 1 Name: Karl Wang
 * Team Member 2 Name: Wilson Mui
 *
 */

#include "life.h"
#include <time.h>
#include <stdlib.h>
#include <fstream>

//Generate the life matrix any way you want. We would highly recommend that you print the generated
//matrix into a file, so that you can share it with other teams for checking correctness.
void genlife(int *a, unsigned int n)
{
    int i=0;
    printf("genlife started... \n");
	for(i = 0; i < n*n; i++){
		a[i*sizeof(int)] = rand() % 2;
        printf("%d", a[i*sizeof(int)]);
	}
}

//Read the life matrix from a file
void readlife(int *a, unsigned int n, char *filename)
{
    /*
	std::ifstream file;
	file.open(filename);

	if (!file.is_open()) {
		printf("readlife() failed to open file .. Exiting\n");
		exit(-1);
	}

	int word;
	for(unsigned i = 0; i < n*n; i++){
		file >> word;
		a[i] = word;
	}
	file.close();
     */
    
    FILE *fp = fopen(filename, "r");
    for(int i = 0; i < n*n; i++)
    {
        fscanf(fp, "%d", &a[i]);
    }
    fclose(fp);
}

unsigned getNeighborCount(const int *a, const unsigned int n, const unsigned int i, const unsigned int j){
	unsigned ui = (i == 0) ? n-1 : i-1;
	unsigned di = (i == n-1) ? 0 : i+1;
	unsigned lj = (j == 0) ? n-1 : j-1;
	unsigned rj = (j == n-1) ? 0 : j+1;

	return a[ui*n + lj] + a[ui*n +  j] + a[ui*n + rj]
		 + a[ i*n + lj] + 			     a[ i*n + rj]
		 + a[di*n + lj] + a[di*n +  j] + a[di*n + rj];
}


//Life function
//note the way the game is stored. Divide by rows to minimize cache misses.
void life(int *a, unsigned int n, unsigned int iter, int *livecount)
{
    // You need to store the total number of livecounts for every 1/10th of the total iterations into the livecount array.
	// For example, if there are 50 iterations in your code, you need to store the livecount for iteration number 5 10 15
	// 20 ... 50. The countlive function is defined in life.cpp, which you can use. Note that you can
	// do the debugging only if the number of iterations is a multiple of 10.
	// Furthermore, you will need to wrap your counting code inside the wrapper #if DEBUG == 1 .. #endif to remove
	// it during performance evaluation.
	// For example, your code in this function could look like -
	//
	//
	//	for(each iteration)
	//      {
	//
	//		Calculate_next_life();
	//
	//		#if DEBUG == 1
	//		  if_current_iteration == debug_iteration
	//		  total_lives = countlive();
	//		  Store_into_livecount(total_lives);
	//		#ENDIF
	//
	//	}
	


	for(unsigned currIter = 0; currIter < iter; currIter++){
		//use b as the result grid; calloc'ed to all 0s
		int *b = (int *)calloc(n*n, sizeof(int));

		cilk_for(unsigned i = 0; i < n; i++){
			for(unsigned j = 0; j < n; j++){
				unsigned neighborCount = getNeighborCount(a, n, i, j);

				if( neighborCount == 3 || (neighborCount == 2 && a[i*n + j] == 1) ){
					b[i*n + j] = 1;
				} //otherwise it is 0, but there is no need to set it as it is calloc'ed
			}
		}

		//a will not be used again
		free(a);
		//swap a and b
		a = b;

		#if DEBUG == 1
			int period = (iter/10);					//the collection period						e.g. period = 55 / 10 = 5
			if(iter >= 10 							//if there is at least 10 iteration			e.g. 55 >= 10 == true, 
				&& (currIter + 1) % period == 0 	//if it is right at the end of a period		e.g. (4 + 1) % 5 == 0, (49 + 1) % 5 == 0
				&& currIter / period < 10){			//if it is among the first 10 period		e.g. 49 / 5 < 10 == true, 54 / 5 < 10 == false

				livecount[currIter / period] = countlive(a, n);
			}
		#endif

	}
}
