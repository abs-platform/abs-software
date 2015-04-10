/*It generates an empty array of a certain length*/
int generate_array(int length, int **p_array);

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

/*It prints a 2D array of integers with title the text in label */
void print_2D_array_int(char *label, int **array, int rows, int cols);

/*It prints a 2D array of floats with title the text in label */
void print_2D_array_float(char *label, float **array, int rows, int cols);

/*It prints a 3D array with title the text in label */
void print_3D_array(char *label, int ***array, int pages, int rows, int cols);