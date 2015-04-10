#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

int generate_array(int length, int **p_array) {
    int *array;

    *p_array = malloc(length * sizeof (int));
    array = *p_array;
    if (array == NULL) {
        fprintf(stderr, "out of memory\n");
        return (-1);
    } else return (0);
}

int generate_2D_matrix_float(int rows, int cols, float ***p_matrix) {
    float **matrix;
    int j;
    *p_matrix = malloc(rows * sizeof (float *));
    matrix = *p_matrix;
    if (matrix == NULL) {
        fprintf(stderr, "out of memory\n");
        return (-1);
    }
    for (j = 0; j < rows; j++) {
        matrix[j] = malloc(cols * sizeof (float));
        if (matrix[j] == NULL) {
            fprintf(stderr, "out of memory\n");
            return (-1);
        }
    }
    return (0);
}

int generate_2D_matrix_int(int rows, int cols, int ***p_matrix) {
    int **matrix;
    int j;

    *p_matrix = malloc(rows * sizeof (int *));
    matrix = *p_matrix;
    if (matrix == NULL) {
        fprintf(stderr, "out of memory\n");
        return (-1);
    }
    for (j = 0; j < rows; j++) {
        matrix[j] = malloc(cols * sizeof (int));
        if (matrix[j] == NULL) {
            fprintf(stderr, "out of memory\n");
            return (-1);
        }
    }
    return (0);
}

int generate_3D_matrix(int rows, int cols, int pages, int ****p_matrix) {
    int i, j;
    int ***matrix;

    *p_matrix = malloc(pages * sizeof (int**));
    matrix = *p_matrix;
    if (matrix == NULL) {
        fprintf(stderr, "out of memory\n");
        return (-1);
    }
    for (i = 0; i < pages; i++) {
        matrix[i] = malloc(rows * sizeof (int*));
        if (matrix[i] == NULL) {
            fprintf(stderr, "out of memory\n");
            return (-1);
        }
    }
    for (i = 0; i < pages; i++) {
        for (j = 0; j < rows; j++) {
            matrix[i][j] = malloc(cols * sizeof (int));
            if (matrix[i][j] == NULL) {
                fprintf(stderr, "out of memory\n");
                return (-1);
            }
        }
    }
    return (0);
}

void free_2D_matrix(int rows, void **matrix) {
    int j;

    for (j = 0; j < rows; j++) {
        free(matrix[j]);
    }
    free(matrix);
}

void free_3D_matrix(int pages, int rows, int ***matrix) {
    int i, j;

    for (i = 0; i < pages; i++) {
        for (j = 0; j < rows; j++) {
            free(matrix[i][j]);
        }
    }
    for (i = 0; i < pages; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void print_array(char *label, int *array, int length) {
    int i;
    printf("%s: ", label);
    for (i = 0; i < length; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void print_2D_array_int(char *label, int **array, int rows, int cols) {
    int i, j;
    printf("%s:\n", label);
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_2D_array_float(char *label, float **array, int rows, int cols) {
    int i, j;
    printf("%s:\n", label);
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            printf("%.1f ", array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_3D_array(char *label, int ***array, int pages, int rows, int cols) {
    int i, j, k;
    printf("%s:\n", label);
    for (i = 0; i < pages; i++) {
        for (j = 0; j < rows; j++) {
            for (k = 0; k < cols; k++) {
                printf("%d ", array[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("\n");
}