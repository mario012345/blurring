#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define OMP_NUM_THREADS 10
#define HEADER_SIZE 138

const int valid_dimensions[] = {9, 15, 23, 29, 35, 41};

int main (int argc, char *argv[])
{
  char img[] = "f1.bmp";
  FILE *ip = fopen(img, "rb+");
  unsigned char header[HEADER_SIZE];
  int width = 0, height = 0, isValid = 0, redMean = 0;
  int kernel = 9;

  if (ip != NULL) {
    for (int i = 0; i < HEADER_SIZE; i++) {
      header[i] = fgetc(ip);
    }

    width = (int)header[20] * 65536 + (int)header[19] * 256 + (int)header[18];
    height = (int)header[24] * 65536 + (int)header[23] * 256+ (int)header[22];

    #pragma omp parallel for
    for (int i = 0; i < 6; i++) {
      if (kernel == valid_dimensions[i]) {
        #pragma single 
        {
          isValid = 1;
        }
      }
    }

    if (!isValid) {
      printf("Size not valid\n");
      exit(1);
    }

    fseek(ip, HEADER_SIZE, SEEK_SET);

    for (int i = 0; i < height; i++) {
      redMean = 0;
      for (int j = 0; j < width + kernel; j++) {
        redMean += (int)fgetc(ip);
        fgetc(ip);
        fgetc(ip);
      }
      fseek(ip, -3 * (width + kernel), SEEK_CUR);
    }

    redMean = redMean / width;

    fseek(ip, HEADER_SIZE, SEEK_SET);

    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        fputc(redMean, ip);
        fputc(fgetc(ip), ip);
        fputc(fgetc(ip), ip);
      }
    }

  } else {
    printf("Image is null\n");
    exit(1);
  }
  
  fclose(ip);

  return 0;
}
