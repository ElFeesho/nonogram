#include <stdlib.h>

/*
	The bitmap representation of non-o-gram
	
	2D sheerly for readability
*/
static int **image = NULL;

/*
	Store the image width and height so that 
	the image_destroy function can be
	argumentless
*/
static int imw = 0;
static int imh = 0;

void image_generate(int xs, int ys)
{
	/* Allocate ys amount of int arrays */
	image = malloc(sizeof(int*)*ys);

	imw = xs;
	imh = ys;

	int x;
	int y;

	for(y = 0;y<ys;y++)
	{
		image[y] = malloc(sizeof(int*)*xs);
	}

	/*
		Generate the image
	*/
	for(y = 0;y<ys;y++)
	{
		for(x = 0;x<xs;x++)
		{
			image[y][x] = rand()%2;
		}
	}
}

int *image_get_row(int r)
{
	return image[r];
}

void image_destroy()
{
	int y;

	/* Free each row of the image */
	for(y = 0;y<imh;y++)
	{
		free(image[y]);
	}

	/* Finally free the image */
	free(image);
}
