#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

// Applies a 5x5 Gaussian filter to an image
void fgauss (int *, int *, int, int);

/**
 * Main function
 * Reads a binary video file ('movie.in'), applies a Gaussian filter to each frame,
 * and writes the filtered frames to a new file ('movie.out').
 * The number of frames processed in parallel is determined by 'seq' (default 8, or from argv).
 */
int main(int argc, char *argv[]) {

   FILE *in;
   FILE *out;
   int i, j, size, seq = 8;
   int **pixels, **filtered;


   if (argc == 2) seq = atoi (argv[1]);

  
//   chdir("/tmp");
   in = fopen("movie.in", "rb");
   if (in == NULL) {
      perror("movie.in");
      exit(EXIT_FAILURE);
   }

   out = fopen("movie.out", "wb");
   if (out == NULL) {
      perror("movie.out");
      exit(EXIT_FAILURE);
   }

   int width, height;

   fread(&width, sizeof(width), 1, in);
   fread(&height, sizeof(height), 1, in);

   fwrite(&width, sizeof(width), 1, out);
   fwrite(&height, sizeof(height), 1, out);

   pixels = (int **) malloc (seq * sizeof (int *));
   filtered = (int **) malloc (seq * sizeof (int *));

   for (i=0; i<seq; i++)
   {
      pixels[i] = (int *) malloc((height+2) * (width+2) * sizeof(int));
      filtered[i] = (int *) malloc((height+2) * (width+2) * sizeof(int));
   }
   i = 0;
   do 
   {
      size = fread(pixels[i], (height+2) * (width+2) * sizeof(int), 1, in);

      if (size)
      {
         fgauss (pixels[i], filtered[i], height, width);
         fwrite(filtered[i], (height+2) * (width + 2) * sizeof(int), 1, out);
      } 
            
   } while (!feof(in));

   for (i=0; i<seq; i++)
   {
      free (pixels[i]);
      free (filtered[i]);
   }
   free(pixels);
   free(filtered);

   fclose(out);
   fclose(in);

   return EXIT_SUCCESS;
}

/**
 * fgauss
 * Applies a 5x5 Gaussian filter to the input image.
 * The filter smooths the image by averaging pixel values with their neighbors.
 * Only pixels within the image bounds are considered for filtering.
 *
 * @param pixels    Pointer to input image data
 * @param filtered  Pointer to output (filtered) image data
 * @param heigh     Image height
 * @param width     Image width
 */
void fgauss (int *pixels, int *filtered, int heigh, int width)
{
	int y, x, dx, dy;
	int filter[5][5] = {1, 4, 6, 4, 1, 4, 16, 26, 16, 4, 6, 26, 41, 26, 6, 4, 16, 26, 16, 4, 1, 4, 6, 4, 1};
	int sum;

	for (x = 0; x < width; x++) {
		for (y = 0; y < heigh; y++)
		{		
			sum = 0;
			for (dx = 0; dx < 5; dx++)
				for (dy = 0; dy < 5; dy++)
					if (((x+dx-2) >= 0) && ((x+dx-2) < width) && ((y+dy-2) >=0) && ((y+dy-2) < heigh))
						sum += pixels[(x+dx-2),(y+dy-2)] * filter[dx][dy];
			filtered[x*heigh+y] = (int) sum/273;
		}
	}
}


