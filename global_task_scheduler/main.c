#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>
#include <sys/time.h>

/* It computes the total amount of occurrences of all tasks.
 * If the given current solution doesn't include each task at least once,
 * the function returns -1 */
int total_occurrences(int **x, int ***t, int tasks, int sats,
        int *golden_index) {
    int sum = 0, prev_sum;
    int i, j, k;

    for (i = 0; i < tasks; i++) {
        prev_sum = sum;
        for (k = 0; k < sats; k++) {
            for (j = 0; j < golden_index[k]; j++) {
                if (x[j][k] == 1) {
                    sum += x[j][k] * t[i][j][k];
                    break;
                }
            }
        }
        if (sum == prev_sum) { //If the sum doesn't increase for a certain task 
            return (-1);
        }
    }
    return (sum);
}

/*It computes the sum of the reward function for each satellite and solution*/
float total_reward(int **x, float **F, int sats, int *golden_index) {
    float sum_reward = 0;
    int j, k;

    for (k = 0; k < sats; k++) {
        for (j = 0; j < golden_index[k]; j++) {
            /*If a solution is used from a certain satellite, we don't need
             to check other solutions from the same satellite.*/
            if (x[j][k] == 1) {
                sum_reward += F[j][k];
                break; //The code jumps to the next k (satellite).
            }
        }
    }
    return (sum_reward);
}

/*It converts the vector that tells which solution is used for each satellite
 into a matrix that contains 1's in the positions [j][k] where the solution j
 is used in the satellite k. The other positions take the value 0.*/
void solution_to_matrix(int *solution, int **matrix, int sats,
        int golden_index_max) {
    int j, k;

    for (k = 0; k < sats; k++) {
        for (j = 0; j < golden_index_max; j++) {
            if (j == (solution[k] - 1)) {
                matrix[j][k] = 1;
            } else {
                matrix[j][k] = 0;
            }
        }
    }
}

/*It computes the next solution combination given the current combination.
 It starts by adding 1 to the last position of the vector and if the result
 is greater than the satellite's golden index (number of solutions), this 1
 is carried to the previous position. */
void next_solution(int *solution, int dim, int *golden_index) {
    int n = dim - 1;

    while (n >= 0) {
        solution[n]++;
        if (solution[n] > golden_index[n]) {
            solution[n] = 0;
            n--;
        } else return;
    }
}

/*It computes the number of combinations, which is in fact the product
 of all golden index plus one (the combination of not using the satellite)*/
long int number_of_combinations(int *golden_index, int sats) {
    long int combs = golden_index[0] + 1;
    int i;

    for (i = 1; i < sats; i++) {
        combs *= (golden_index[i] + 1);
    }
    return combs;
}

/*It updates the final solution*/
void copy_solution(int *solution, int *final_solution, int sats) {
    int i;

    for (i = 0; i < sats; i++) {
        final_solution[i] = solution[i];
    }
}

/*It generates an empty array of a certain length*/
int generate_array(int length, int **p_array) {
    int *array;

    *p_array = malloc(length * sizeof (int));
    array = *p_array;
    if (array == NULL) {
        fprintf(stderr, "out of memory\n");
        return (-1);
    } else return (0);
}

/*It generates an empty 2D matrix (of floats) of certain dimensions*/
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

/*It generates an empty 2D matrix (of integers) of certain dimensions*/
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

/*It generates an empty 3D matrix of certain dimensions*/
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

int generate_F(int *golden_index, int golden_index_max, int sats, float ***p_F) {
    int j, k;
    float **F;

    int error = generate_2D_matrix_float(golden_index_max, sats, p_F);
    if (error == -1) return (error);
    F = *p_F;

    for (k = 0; k < sats; k++) {
        for (j = 0; j < golden_index[k]; j++) {
            F[j][k] = (float) ((rand() % 9) + 1) / 10;
        }
    }
    return 0;
}

int generate_golden_index(int golden_index_max, int sats, int **p_golden_index) {
    int k;
    int *golden_index;

    int error = generate_array(sats, p_golden_index);
    if (error == -1) return (error);
    golden_index = *p_golden_index;
    for (k = 0; k < sats; k++) {
        golden_index[k] = golden_index_max;
    }
    return 0;
}

int generate_t(int tasks, int golden_index_max, int *golden_index,
        int sats, int ****p_t) {
    int n_ones, a, task_exist, n = 5;
    int i, j, k;
    int ***t;

    int error = generate_3D_matrix(golden_index_max, sats, tasks, p_t);
    if (error == -1) return (error);

    t = *p_t;
    for (i = 0; i < tasks; i++) {
        for (j = 0; j < golden_index_max; j++) {
            for (k = 0; k < sats; k++) {
                t[i][j][k] = 0;
            }
        }
    }

    for (k = 0; k < sats; k++) {
        for (j = 0; j < golden_index[k]; j++) {
            n_ones = (rand() % tasks) + 1;
            a = 0;
            while (a < n_ones) {
                i = rand() % tasks;
                if (t[i][j][k] == 1) continue;
                else {
                    t[i][j][k] = 1;
                    if (rand() % 100 < n){
                        n += 5;
                        break;
                    }
                    a++;
                }
            }
        }
    }
    
    for (i = 0; i < tasks; i++){
        task_exist = 0;
        for (k = 0; k < sats; k++){
            for (j = 0; j < golden_index[k]; j++){
                if (t[i][j][k] != 0) {
                    task_exist = 1;
                    break;
                }
            }
            if (task_exist) break;
        }
     if (!task_exist) 
     {
       k = rand() % sats;  
       j = rand() % golden_index[k];
       t[i][j][k] = 1;
     }
    }
    return (0);
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

int compare_solutions(int *solution1, int *solution2, int tasks) {
    int i;

    for (i = 0; i < tasks; i++) {
        if (solution1[i] != solution2[i]) {
            return (0);
        }
    }
    return (1);
}

/* If a solution appears only in one solution of one satellite, the final 
 * solution must use this solution to avoid reaching to an incompatibility.*/
void set_solutions(int tasks, int *golden_index, int sats, int ***t, 
        int *fixed_solution) {
    int r, a, b, i, j, k;
    for (i = 0; i < tasks; i++) {
        r = 0;
        for (k = 0; k < sats; k++) {
            if (fixed_solution[k] != 0) continue;
            for (j = 0; j < golden_index[k]; j++) {
                if (t[i][j][k] == 1) {
                    r++;
                    a = j;
                    b = k;
                }
            }
        }
        if (r == 1){
            fixed_solution[b] = a + 1;
        }
    }
}

void delete_duplicates(int sol, int sat, int tasks, int *golden_index, int sats,
        int ***t, float **F, int *fixed_solution) {
    int i, j, k, l;
    int equals, zeros;
    int *sol1, *sol2;

    generate_array(tasks, &sol1);
    generate_array(tasks, &sol2);

    l = sol + 1;
    for (k = sat; k < sats; k++) {
        if (fixed_solution[k] != 0) {
            l = 0;
            continue;
        }
        for (j = l; j < golden_index[k]; j++) {
            zeros = 0;
            for (i = 0; i < tasks; i++) {
                if (t[i][sol][sat] == 0) {
                    zeros++;
                    if (zeros == tasks) {
                        free(sol1);
                        free(sol2);
                        return;
                    }
                }
                sol1[i] = t[i][sol][sat];
                sol2[i] = t[i][j][k];

            }
            equals = compare_solutions(sol1, sol2, tasks);
            if (equals) {

                if (F[sol][sat] >= F[j][k]) {
                    for (i = 0; i < tasks; i++) {
                        t[i][j][k] = 0;
                    }
                    F[j][k] = 0;
                    set_solutions(tasks, golden_index, sats, t, fixed_solution);
                    if (fixed_solution[k] != 0) break;
                      
                } else {
                    for (i = 0; i < tasks; i++) {
                        t[i][sol][sat] = 0;
                    }
                    F[sol][sat] = 0;
                    set_solutions(tasks, golden_index, sats, t, fixed_solution);
                    free(sol1);
                    free(sol2);
                    return;
                }
            }
        }
        l = 0;
    }
    free(sol1);
    free(sol2);
}

int check_solution(int sats, float **F, int *solution, 
        int *fixed_solution, float *duplicates, float *mandatories ) {
    int k;
    for (k = 0; k < sats; k++) {
        if (solution[k] == 0) continue;
        if (F[solution[k] - 1][k] == 0) {
            (*duplicates)++;
            return (0);
        }
        if (fixed_solution[k] == 0) continue;
        if (solution[k] != fixed_solution [k]){
            (*mandatories)++;
            return(0);
        }
    }   
return (1);   
} 

void solve(int tasks, int golden_index_max, int sats,
        int *golden_index, float **F, int ***t, int **x, int *solution, 
        int *final_solution, int *fixed_solution, int brute_force,
        float *duplicates, float *mandatories, float *max) {
    int i = 0, valid = 1;
    long int combs;
    int n;
    float r, num; 

    combs = number_of_combinations(golden_index, sats);
     //  printf("Combinations: %ld\n", combs);
    for (i = 1; i < combs; i++) {
        next_solution(solution, sats, golden_index);
        if (!brute_force) {
            valid = check_solution(sats, F, solution, 
                    fixed_solution, duplicates, mandatories);
            if (!valid) continue;
        }
        solution_to_matrix(solution, x, sats, golden_index_max);
        n = total_occurrences(x, t, tasks, sats, golden_index);
        if (n == -1) continue;
        r = total_reward(x, F, sats, golden_index);
        num = r / n;
        if (num > (*max)) {
            (*max) = num;
            copy_solution(solution, final_solution, sats);
        }
    }
    //    printf("Max: %.2f\n", (*max));
}

void solve_deleting_duplicates(int tasks, int golden_index_max, int sats,
        int *golden_index, float **F, int ***t, int **x, int *solution, 
        int *final_solution, int *fixed_solution, float *duplicates,
        float *mandatories, float *max) {
    int j, k;

    for (k = 0; k < sats; k++) {
        for (j = 0; j < golden_index[k]; j++) {
            delete_duplicates(j, k, tasks, golden_index, sats, t, F, 
                    fixed_solution);
            if (fixed_solution[k] != 0) break;
        }
    }
    solve(tasks, golden_index_max, sats, golden_index, F, t, x,
            solution, final_solution, fixed_solution, 0, duplicates,
            mandatories, max);
}

int main(int argc, char* argcv[]) {

    int tasks, golden_index_max, sats;
    float combs, deleted_combs, bf_time, dd_time, time_difference, duplicates = 0, mandatories = 0; 
    int error; 
    float bf_max = 0, dd_max = 0, optimality_lost;
    int *golden_index, *solution, *final_solution, *fixed_solution;
    int**x;
    float **F;
    int ***t;
    struct timeval temps;
    struct timezone tz;
    suseconds_t start_time_u, final_time_u;
    time_t start_time_s, final_time_s;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s tasks solutions satellites\n", basename(argcv[0]));
        return (1);
    } else {
        tasks = atoi(argcv[1]);
        golden_index_max = atoi(argcv[2]);
        sats = atoi(argcv[3]);
    }
    if (gettimeofday(&temps, &tz)) return (1);
    start_time_u = temps.tv_usec;
    srand(start_time_u); //The seed of the random number
     error = generate_golden_index(golden_index_max, sats, &golden_index);
    if (error == -1) return (error);
    error = generate_F(golden_index, golden_index_max, sats, &F);
    if (error == -1) return (error);
    error = generate_t(tasks, golden_index_max, golden_index, sats, &t);
    if (error == -1) return (error);

    error = generate_golden_index(0, sats, &solution);
    if (error == -1) return (error);
    error = generate_array(sats, &final_solution);
    if (error == -1) return (error);
    error = generate_2D_matrix_int(golden_index_max, sats, &x);
    if (error == -1) return (error);
    error = generate_golden_index(0, sats, &fixed_solution);
    if (error == -1) return (error);
    set_solutions(tasks, golden_index, sats, t, fixed_solution);
  //  print_array("Fixed", fixed_solution, sats);
   
        if (gettimeofday(&temps, &tz)) return (1);
        start_time_u = temps.tv_usec;
        start_time_s = temps.tv_sec;
     //   printf("Initial: %ld\n",start_time_s*1000000+start_time_u);
        solve(tasks, golden_index_max, sats, golden_index, F, t, x, 
                solution, final_solution, fixed_solution, 1, &duplicates, 
                &mandatories, &bf_max);
        if (gettimeofday(&temps, &tz)) return (1);
        final_time_u = temps.tv_usec;
        final_time_s = temps.tv_sec;
    //    printf("Final: %ld\n",final_time_s*1000000+final_time_u);
    //    print_array ("Solution opt", final_solution, sats);
        bf_time = (final_time_s - start_time_s)*1000000 + final_time_u - start_time_u;
    //    printf("%d %d %d %ld\n\n", tasks, golden_index_max, sats, bf_time);
        free(final_solution);
        free(solution);
        error = generate_golden_index(0, sats, &solution);
        if (error == -1) return (error);
        error = generate_golden_index(0, sats, &final_solution);
        if (error == -1) return (error);
     
      print_3D_array("t", t, tasks, golden_index_max, sats);
    //  print_2D_array_float ("F", F, golden_index_max, sats);

    if (gettimeofday(&temps, &tz)) return (1);
    start_time_u = temps.tv_usec;
    start_time_s = temps.tv_sec;
    //   printf("Initial: %ld\n",start_time_s*1000000+start_time_u);
    solve_deleting_duplicates(tasks, golden_index_max, sats,
            golden_index, F, t, x, solution, final_solution, fixed_solution, &duplicates,
            &mandatories, &dd_max);
    if (gettimeofday(&temps, &tz)) return (1);
    final_time_u = temps.tv_usec;
    final_time_s = temps.tv_sec;
    //  print_3D_array("t", t, tasks, golden_index_max, sats);
    //  print_2D_array_float ("F", F, golden_index_max, sats);
    //  printf("Final: %ld\n",final_time_s*1000000+final_time_u);
    //print_array("Solution", final_solution, sats);
    //printf("Duplicates: %.0f\nMandatories: %.0f\n", duplicates, mandatories);
    dd_time = (final_time_s - start_time_s)*1000000 + final_time_u - start_time_u;
    //printf("bf_time: %.0f, dd_time: %.0f\n", bf_time, dd_time);
    time_difference = bf_time/dd_time;
    combs = (float) number_of_combinations(golden_index, sats);
    deleted_combs = ((duplicates + mandatories)/combs)*100;
    //printf("deleted_combs: %.2f\n", deleted_combs);
    optimality_lost = ((bf_max - dd_max)/bf_max) * 100;
    printf("%d %d %d %.0f %.2f %.2f %.2f\n", tasks, golden_index_max, sats, dd_time, 
            time_difference, deleted_combs, optimality_lost);
    free(golden_index);
    free(final_solution);
    free(solution);
    free(fixed_solution);
    free_2D_matrix(golden_index_max, (void **) x);
    free_2D_matrix(golden_index_max, (void **) F);
    free_3D_matrix(tasks, golden_index_max, t);
    return (0);
}

/*ASK: 
 * - Reward function's definition.
 * - Use of the parameter m in the algorithm.
 * - Same solution but different F --> Leave only the one with higher F? 
 */

/*TODO:
 * + Randomize variables (Dynamically Allocating Multidimensional Arrays)
 * + Function that generates t[i][j][k], goldenIndex[j][k] and F[j][k] to test
 *   the algorithm in harder conditions (5 sats - 5 sub-solutions each ...)
 * 
 * + Variables int --> unsigned char (positive integer up to 256) NOT OPTIMIZED
 * + Free memory function
 * 
 * - Optimize algorithm
 * - Think how the local scheduler have to give the information. 
 */

/*ERRORS:
 * + In some cases "t" is incorrectly generated. Changing pointer syntax
 *   there's sometimes a segmentation fault. SOLVED
 * + The program crashes if the number of solutions was greater than the 
 *   number of satellites. SOLVED
 */
