//compilation instructions/examples:
//gcc -fopenmp point_epsilon_starter.c -o point_epsilon_starter
//sometimes you need to link against the math library with -lm:
//gcc -fopenmp point_epsilon_starter.c -lm -o point_epsilon_starter

//math library needed for the square root

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "omp.h"

//N is 100000 for the submission. However, you may use a smaller value of testing/debugging.
#define N 100000
//Do not change the seed, or your answer will not be correct
#define SEED 72


struct pointData{
	double x;
	double y;
};

void generateDataset(struct pointData * data);
void checkPoints(struct pointData* currentPoint, double epsilonSquared, struct pointData** gridSquare, int numPoints, int* count);

int main(int argc, char *argv[])
{


	//Read epsilon distance from command line
	if (argc!=2)
	{
		printf("\nIncorrect number of input parameters. Please input an epsilon distance.\n");
		return 0;
	}


	char inputEpsilon[20];
	strcpy(inputEpsilon,argv[1]);
	double epsilon=atof(inputEpsilon);



	//generate dataset:
	struct pointData * data;
	data=(struct pointData*)malloc(sizeof(struct pointData)*N);
	printf("\nSize of dataset (MiB): %f",(2.0*sizeof(double)*N*1.0)/(1024.0*1024.0));
	generateDataset(data);


	//change OpenMP settings:
	omp_set_num_threads(1);


	double tstart=omp_get_wtime();


	//Write your code here:
	//The data you need to use is stored in the variable "data",
	//which is of type pointData

	double epsilonSquared = epsilon * epsilon;
	double cellSize = ceil(epsilon);
	int gridSize = (floor((1000.0)/cellSize));
	cellSize = 1000.0 / gridSize;
	double cellSizeReciprocal = 1.0 / cellSize;
	int gridArea = gridSize * gridSize;
	int count = 0;
	int i, j;

	struct pointData*** grid;
	grid = (struct pointData***)malloc(sizeof(struct pointData**) * gridArea);

	int* indices;
	indices = (int*)calloc(gridArea, sizeof(int));

	for (i = 0; i < N; i++)
	{
		struct pointData* currentPoint = &(data[i]);
		int gridX = (int)(currentPoint->x * cellSizeReciprocal);
		int gridY = (int)(currentPoint->y * cellSizeReciprocal);
		int index = (gridY * gridSize) + gridX;
		(indices[index])++;
	}

	#pragma omp parallel for shared(grid, indices) private(i) schedule(static)
	for (i = 0; i < (gridArea); i++)
	{
		grid[i] = (struct pointData**)malloc(sizeof(struct pointData*) * indices[i]);
		indices[i] = 0;
	}

	for (i = 0; i < N; i++)
	{
		struct pointData* currentPoint = &(data[i]);
		int gridX = (int)(currentPoint->x * cellSizeReciprocal);
		int gridY = (int)(currentPoint->y * cellSizeReciprocal);
		int index = (gridY * gridSize) + gridX;
		grid[index][indices[index]] = currentPoint;
		(indices[index])++;
	}

	int edgeIndex = gridSize - 1;

	#pragma omp parallel for shared(data, epsilonSquared, grid, gridSize, cellSize, edgeIndex) private(i, j) schedule(static) reduction(+:count)
	for (i = 0; i < gridArea; i++)
	{
		struct pointData** currentGridCell = grid[i];
		int gridX = i % gridSize;
		int gridY = i / gridSize;

		int top = gridY != 0;
		int left = gridX != 0;
		int right = gridX != edgeIndex;
		int bottom = gridY != edgeIndex;
		for (j = 0; j < indices[i]; j++)
		{
			struct pointData* currentPoint = currentGridCell[j];
			if (top)
			{
				if (left)
				{
					checkPoints(currentPoint, epsilonSquared, grid[i - (gridSize + 1)], indices[i - (gridSize + 1)], &count);
				}
				checkPoints(currentPoint, epsilonSquared, grid[i - gridSize], indices[i - gridSize], &count);
				if (right)
				{
					checkPoints(currentPoint, epsilonSquared, grid[i - (gridSize - 1)], indices[i - (gridSize - 1)], &count);
				}
			}

			if (right)
			{
				checkPoints(currentPoint, epsilonSquared, grid[i + 1], indices[i + 1], &count);
			}

			checkPoints(currentPoint, epsilonSquared, grid[i], indices[i], &count);

			if (left)
			{
				checkPoints(currentPoint, epsilonSquared, grid[i - 1], indices[i - 1], &count);
			}

			if (bottom)
			{
				if (left)
				{
					checkPoints(currentPoint, epsilonSquared, grid[i + (gridSize - 1)], indices[i + (gridSize - 1)], &count);
				}
				checkPoints(currentPoint, epsilonSquared, grid[i + gridSize], indices[i + gridSize], &count);
				if (right)
				{
					checkPoints(currentPoint, epsilonSquared, grid[i + (gridSize + 1)], indices[i + (gridSize + 1)], &count);
				}
			}
		}
	}

	for (i = 0; i < gridArea; i++)
	{
		free(grid[i]);
	}

	free(grid);
	free(indices);

	double tend=omp_get_wtime();

	printf("\nTotal time (s): %f",tend-tstart);
	printf("\nCount: %d", count);

	free(data);
	printf("\n");
	return 0;
}


//Do not modify the dataset generator or you will get the wrong answer
void generateDataset(struct pointData * data)
{

	//seed RNG
	srand(SEED);


	for (unsigned int i=0; i<N; i++){
		data[i].x=1000.0*((double)(rand()) / RAND_MAX);
		data[i].y=1000.0*((double)(rand()) / RAND_MAX);
	}
}

void checkPoints(struct pointData* currentPoint, double epsilonSquared, struct pointData** gridSquare, int numPoints, int* count)
{
	struct pointData* pointToBeChecked;
	for (int i = 0; i < numPoints; i++)
	{
		pointToBeChecked = gridSquare[i];
		if ((currentPoint->x - pointToBeChecked->x) * (currentPoint->x - pointToBeChecked->x) + (currentPoint->y - pointToBeChecked->y) * (currentPoint->y - pointToBeChecked->y) <= epsilonSquared)
		{
			(*count)++;
		}
	}
}
