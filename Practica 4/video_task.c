/*
 * Parallel Video Processing using OpenMP Tasks
 * This program reads a binary video file, applies a Gaussian filter to each frame
 * using parallel processing with OpenMP tasks, and writes the filtered frames to an output file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

// Global variable to store execution time
double exec_time = 0.0;

// Function declaration: applies a 5x5 Gaussian filter to an image
void fgauss (int *, int *, int, int);

/**
 * Main function
 * Reads a binary video file ('movie.in'), applies a Gaussian filter to each frame
 * using parallel processing with OpenMP tasks, and writes the filtered frames to 'movie.out'.
 * The number of frames processed in batch is determined by 'seq' (default 60, or from argv).
 * Execution time is measured and displayed.
 */
int main(int argc, char *argv[]) {

   FILE *in;
   FILE *out;
   int i, j, seq = 60;
   int *size;
   int **pixels, **filtered;

   // If a command-line argument is provided, use it as the batch size
   if (argc == 2) seq = atoi (argv[1]);
  
//   chdir("/tmp");
   // Open input video file
   in = fopen("movie.in", "rb");
   if (in == NULL) {
      perror("movie.in");
      exit(EXIT_FAILURE);
   }

   // Open output video file
   out = fopen("movie.out", "wb");
   if (out == NULL) {
      perror("movie.out");
      exit(EXIT_FAILURE);
   }

   int width, height;
   size_t fread1;
   // Read frame dimensions from input file (to avoid warnings)
   fread1 = fread(&width, sizeof(width), 1, in);
   fread1 = fread(&height, sizeof(height), 1, in);
  

   // Write frame dimensions to output file
   fwrite(&width, sizeof(width), 1, out);
   fwrite(&height, sizeof(height), 1, out);

   // Allocate memory for pixel data, filtered frames, and size tracking
   pixels = (int **) malloc (seq * sizeof (int *));
   filtered = (int **) malloc (seq * sizeof (int *));
   size = (int *)malloc(sizeof(int) * seq);

   for (i=0; i<seq; i++)
   {
      pixels[i] = (int *) malloc((height+2) * (width+2) * sizeof(int));
      filtered[i] = (int *) malloc((height+2) * (width+2) * sizeof(int));
   }

   // Start timing the parallel processing section
   double start = omp_get_wtime();
   #pragma omp parallel shared(size, pixels, filtered, seq, height, width, in, out)
   #pragma omp single
   {
      // Phase 1: Read frames sequentially (I/O operations must be sequential)
      for (int j = 0; j < seq; j++)
      {
         size[j] = fread(pixels[j], (height+2) * (width+2) * sizeof(int), 1, in);
      }

      // Phase 2: Apply Gaussian filter in parallel using tasks
      for (int j = 0; j < seq; j++)
      {
         if (size[j])
         {
            #pragma omp task
            fgauss (pixels[j], filtered[j], height, width);
         }
      }
      // Wait for all filtering tasks to complete
      #pragma omp taskwait

      // Phase 3: Write filtered frames sequentially (I/O operations must be sequential)
      for (int j = 0; j < seq; j++)
      {
         if (size[j])
         {
            fwrite(filtered[j], (height+2) * (width + 2) * sizeof(int), 1, out);
         }
      }      
   }
   
   // Stop timing and calculate execution time
   double end = omp_get_wtime();
	exec_time = end - start;

   //printf("%f\n", exec_time);
   printf("Computing time: %f\n", exec_time);

   // Free allocated memory
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
 * The filter smooths the image by averaging pixel values with their neighbors
 * using a weighted kernel. Only pixels within the image bounds are considered.
 *
 * @param pixels    Pointer to input image data
 * @param filtered  Pointer to output (filtered) image data
 * @param heigh     Image height
 * @param width     Image width
 */
void fgauss (int *pixels, int *filtered, int heigh, int width)
{
	int y, x, dx, dy;
	// 5x5 Gaussian kernel weights (normalized by dividing by 273)
	int filter[5][5] = {1, 4, 6, 4, 1, 4, 16, 26, 16, 4, 6, 26, 41, 26, 6, 4, 16, 26, 16, 4, 1, 4, 6, 4, 1};
	int sum;

	// Process each pixel in the image
	for (x = 0; x < width; x++) 
   {
		for (y = 0; y < heigh; y++)
		{		
			sum = 0;
			// Apply the 5x5 filter kernel
			for (dx = 0; dx < 5; dx++)
				for (dy = 0; dy < 5; dy++)
					// Check bounds to avoid accessing pixels outside the image
					if (((x+dx-2) >= 0) && ((x+dx-2) < width) && ((y+dy-2) >=0) && ((y+dy-2) < heigh))
						sum += pixels[(x+dx-2),(y+dy-2)] * filter[dx][dy];
			// Normalize the result and store in filtered array
			filtered[x*heigh+y] = (int) sum/273;
		}
	}
}

