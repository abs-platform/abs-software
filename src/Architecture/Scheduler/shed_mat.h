#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>
#include <sys/time.h>

extern int ntasks;
extern int nsats;
extern int golden_index_max;

typedef struct local_solution {
  int id;
  float F;
  int *tasks;
} LocalSolution;

typedef struct satellite {
  int id;
  int golden_index;
  LocalSolution *local_solutions;
} Satellite;

/*It generates an empty array of a certain length*/
int generate_array(int length, int **p_array);

/*It generates an empty array of floats of a certain length*/
int generate_array_float(int length, float **p_array);

/*It generates an empty array of structs of a certain length*/
int generate_array_struct(int length, LocalSolution **p_array);

/*It generates an empty array of local solutions of a certain length*/
int generate_array_satellites(int length, Satellite **p_array);

void print_F_matrix(Satellite *sats);

void print_t_matrix(Satellite *sats);

/*It generates an empty 2D matrix (of floats) of certain dimensions*/
int generate_2D_matrix_float(int rows, int cols, float ***p_matrix);

/*It generates an empty 2D matrix (of integers) of certain dimensions*/
int generate_2D_matrix_int(int rows, int cols, int ***p_matrix);

/*It generates an empty 3D matrix of certain dimensions*/
int generate_3D_matrix(int rows, int cols, int pages, int ****p_matrix);

/*It frees the memory space allocated for a 2D matrix*/
void free_2D_matrix(int rows, void **matrix);

/*It frees the memory space allocated for a 3D matrix*/
void free_3D_matrix(int pages, int rows, int ***matrix);

/*It prints an array with title the text in label */
void print_array(char *label, int *array, int length);

/*It prints an array of floats with title the text in label */
void print_array_float(char *label, float *array, int length);

/*It prints a 2D array of integers with title the text in label */
void print_2D_array_int(char *label, int **array, int rows, int cols);

/*It prints a 2D array of floats with title the text in label */
void print_2D_array_float(char *label, float **array, int rows, int cols);

/*It prints a 3D array with title the text in label */
void print_3D_array(char *label, int ***array, int pages, int rows, int cols);

// LOCAL
void generate_solutions(Satellite *sat);
