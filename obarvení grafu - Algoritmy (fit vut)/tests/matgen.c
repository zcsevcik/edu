#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) return EXIT_FAILURE;

	int *mat;
	size_t gsize = strtol(argv[1], NULL, 10);
	
	mat = malloc(gsize*gsize * sizeof(int));
	
	for (size_t row = 0; row < gsize; ++row) {
		for (size_t col = row+1; col < gsize; ++col) {
			
			int val = rand() % 2;
			mat[row * gsize + col] = val;
			mat[col * gsize + row] = val;
		}
	}

    for (size_t diag = 0; diag < gsize; ++diag)
        mat[diag * gsize + diag] = 0;

	printf("%zu\n", gsize);
	for (size_t row = 0; row < gsize; row++) {
		for (size_t col = 0; col < gsize; col++) {
			printf("%d ", mat[row * gsize + col]);
    }
    putchar('\n');
  }
	
	return EXIT_SUCCESS;
}
