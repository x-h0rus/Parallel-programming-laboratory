#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

/*
 * SIZE: 1700 1250
 * TIME: +/-20 min
 *
 */

#define META 128
#define HEADER_META "video custom binary format 0239"
#define image(x,y) pixels[x*width+y]

const char* header = HEADER_META;
const char sfooter[] = {'f','r','a','m','e','s',' ','c','h','u','n','k','\0'};

/**
 * Main function
 * Generates a binary video file ('movie.in') with random pixel data and metadata.
 * The output simulates a sequence of video frames, each with random pixel values and custom metadata.
 * Frame borders are set to zero, and the rest of the pixels are filled with random values (0-255).
 * The file format includes a header, frame data, and metadata for each frame.
 */
int main(void) {
	srand(time(NULL));
	int width, height;
	int x, y, i, max;

	FILE *out;

//	chdir(TEMP);
	out = fopen("movie.in", "wb");
	if (out == NULL) {
		perror("movie.in");
		exit(EXIT_FAILURE);
	}

	width = rand() % 512 + 1536;
	height = rand() % 384 + 1152;
	width =  1920;
	height = 1440;

	fwrite(&width, sizeof(width), 1, out);
	fwrite(&height, sizeof(height), 1, out);
	fflush(out);

	int *pixels = (int*) malloc((height+2) * (width+2) * sizeof(int));
  int *meta = (int*) malloc(sizeof(int) * META);
  memcpy(meta, header, strlen(header)); // rand
  fwrite(meta, META * sizeof(int), 1, out);
  memset(meta, 0, META);
  memcpy(meta, sfooter, strlen(sfooter));

	max = 102;
	for (i = 0; i < max; i++) {
		for (y = 0; y <= height+1; y++) {
			for (x = 0; x <= width+1; x++) {
	// Main function to generate a binary video file with random pixel data
				if ((x == 0) || (x == width+1) || (y == 0) || (y == height+1)) image(y,x) = 0;
                                else image(y,x) = rand() % 256;
			}
		}
   fwrite(pixels, (height+2) * (width+2) * sizeof(int), 1, out);
   fwrite(meta, (META >> 2) * sizeof(char), 1, out);
   }

   fflush(out);
   fclose(out);

   return EXIT_SUCCESS;
}
