#include "shed_mat.h"
// GLOBAL VARIABLES
int golden_index_max = 0;
int nsats;
int ntasks;
static float tic; // proportion of the number of different tasks in combination
static float max = 0;
static int duplicates;

/* It computes the total amount of occurrences of all tasks. */
int total_occurrences(Satellite *sats, int *combination) {
  int i, k;
  int *sum; // number of repetitions of each task
  int error;
  int n = 0;
  int nzeros = 0; // tasks not present in combination

  error = generate_array(ntasks, &sum);
  if (error == -1)
    return error;

  for (i = 0; i < ntasks; i++) {
    sum[i] = 0;
  }
  for (k = 0; k < nsats; k++) {
    if (combination[k] == 0)
      continue;
    for (i = 0; i < ntasks; i++) {
      if (sats[k].local_solutions[combination[k] - 1].tasks[i] == 1)
        sum[i]++;
    }
  }
  for (i = 0; i < ntasks; i++) {
    if (sum[i] == 0) {
      nzeros++;
      continue;
    }
    if (sum[i] == 1)
      continue;
    n += sum[i];
  }
  tic = (ntasks - nzeros) * 10 / ntasks;
  // printf("tic: %.2f\n", tic);
  free(sum);
  if (n == 0)
    return 1;
  else
    return n;
}
/*It computes the sum of the reward function for each satellite and solution*/
float total_reward(Satellite *sats, int *combination) {
  float sum_reward = 0;
  int k;
  for (k = 0; k < nsats; k++) {
    if (combination[k] == 0)
      continue;
    sum_reward += sats[k].local_solutions[combination[k] - 1].F;
  }
  sum_reward *= tic;
  return sum_reward;
}
/*Checks if the current solution contains any deleted solution*/
int check_solution(Satellite *sats, int *combination) {
  int k;
  for (k = 0; k < nsats; k++) {
    if (combination[k] == 0)
      continue;
    if (sats[k].local_solutions[combination[k] - 1].F == 0.) {
      duplicates++;
      return 0;
    }
  }
  return 1;
}
/*It computes the next combination given the current combination.
 It starts by adding 1 to the last position of the vector and if the result
 is greater than the satellite's golden index (number of solutions), this 1
 is carried to the previous position. */
void next_combination(Satellite *sats, int *combination) {
  int n; // satellite index. It starts from the last satellite.

  n = nsats - 1;

  while (n >= 0) {
    combination[n]++;
    if (combination[n] > sats[n].golden_index) {
      combination[n] = 0;
      n--;
    } else
      return;
  }
}
/*It computes the number of combinations, which is in fact the product
 of all golden index plus one (the combination of not using the satellite)*/
long long int number_of_combinations(Satellite *sats) {
  long long int combs = sats[0].golden_index + 1;
  int k;

  for (k = 1; k < nsats; k++) {
    combs *= (sats[k].golden_index + 1);
  }
  return combs;
}

/*It updates the final solution*/
void copy_solution(int *combination, int *solution) {
  int k;

  for (k = 0; k < nsats; k++) {
    solution[k] = combination[k];
  }
}

/* Returns the maximum golden index among all satellites*/
void get_golden_index_max(Satellite *sats) {
  int k;

  for (k = 0; k < nsats; k++) {
    if (sats[k].golden_index > golden_index_max) {
      golden_index_max = sats[k].golden_index;
    }
  }
}

/*It compares if two arrays are equal. If they are, it returns 1, otherwise
 * 0.*/
int compare_solutions(int *solution1, int *solution2) {
  int i;

  for (i = 0; i < ntasks; i++) {
    if (solution1[i] != solution2[i]) {
      return 0;
    }
  }
  return 1;
}

/*If there are two equal solutions with different figure of merit in the same
 satellite, only the one with the highest figure of merit is kept.*/
void delete_duplicates(Satellite *sats, int sol, int sat) {
  int j;
  int equals; // 1 if two solutions are equal, 0 otherwise

  for (j = sol + 1; j < sats[sat].golden_index; j++) {
    if (sats[sat].local_solutions[j].F == 0.)
      continue;
    equals = compare_solutions(sats[sat].local_solutions[sol].tasks,
                               sats[sat].local_solutions[j].tasks);
    if (equals)
      sats[sat].local_solutions[j].F = 0; // assuming that solutions are sorted
  }
}

/*It solves the problem. If we are deleteting duplicates, it checks that the
 solution is valid.*/
int solve(Satellite *sats, int *combination, int *solution) {
  int i, j, k;
  long long int combs;
  int n;
  int valid;
  float r, num;

  combs = number_of_combinations(sats);
  //  printf("Combinations: %ld\n", combs);
  get_golden_index_max(sats);
  for (k = 0; k < nsats; k++) {
    combination[k] = 0; // We start from the combination 1 1 .. 1
    for (j = 0; j < sats[k].golden_index - 1; j++) {
      // delete_duplicates(sats, j, k);
    }
  }
  // print_F_matrix(sats);
  for (i = 1; i < combs; i++) {
    printf("%d ", i);
    next_combination(sats, combination);
    valid = check_solution(sats, combination);
    if (!valid)
      continue;
    //  print_array("comb", combination, nsats);
    n = total_occurrences(sats, combination);
    //  printf("tic: %.2f\n", tic);
    r = total_reward(sats, combination);
    num = r / n;
    printf("%.2f\n", num);
    if (num > max) {
      max = num;
      copy_solution(combination, solution);
    }
  }
  return 0;
}

int allocate_satellites(Satellite *sats) {
  int j, k;
  int error;
  for (k = 0; k < nsats; k++) {
    sats[k].id = k + 1;
    sats[k].golden_index = golden_index_max;
    error =
        generate_array_struct(sats[k].golden_index, &(sats[k].local_solutions));
    if (error == -1)
      return error;
    for (j = 0; j < sats[k].golden_index; j++) {
      error = generate_array(ntasks, &(sats[k].local_solutions[j].tasks));
      if (error == -1) {
        return error;
      }
    }
  }
  return 0;
}

int init_satellites(Satellite **sats) {
  int k;
  int error;

  error = generate_array_satellites(nsats, sats);
  if (error == -1)
    return error;
  allocate_satellites(*sats);
  for (k = 0; k < nsats; k++) {
    generate_solutions(&(*sats)[k]);
  }
  return 0;
}

void free_satellites(Satellite *sats) {
  int j, k;
  for (k = 0; k < nsats; k++) {
    for (j = 0; j < sats[k].golden_index; j++) {
      free(sats[k].local_solutions[j].tasks);
    }
    free(sats[k].local_solutions);
  }
  free(sats);
}

int main(int argc, char *argcv[]) {

  float /*combs, deleted_combs,*/ bf_time /*,time_difference*/;
  int error;
  int *combination, *solution;
  Satellite *sats;
  struct timeval time;
  struct timezone tz;
  suseconds_t start_time_u, final_time_u;
  time_t start_time_s, final_time_s;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s tasks solutions satellites\n",
            basename(argcv[0]));
    return 1;
  } else {
    ntasks = atoi(argcv[1]);
    golden_index_max = atoi(argcv[2]);
    nsats = atoi(argcv[3]);
  }
  if (gettimeofday(&time, &tz))
    return 1;
  start_time_u = time.tv_usec;
  srand(start_time_u); // The seed of the random number

  error = generate_array(nsats, &combination);
  if (error == -1)
    return error;
  error = generate_array(nsats, &solution);
  if (error == -1)
    return error;
  error = init_satellites(&sats);
  if (error == -1)
    return error;
  if (gettimeofday(&time, &tz))
    return 1;
  start_time_u = time.tv_usec;
  start_time_s = time.tv_sec;

  //#pragma omp parallel num_threads(4)
  // print_F_matrix(sats);
  // print_t_matrix(sats);
  solve(sats, combination, solution);
  if (gettimeofday(&time, &tz))
    return 1;
  final_time_u = time.tv_usec;
  final_time_s = time.tv_sec;
  bf_time =
      (final_time_s - start_time_s) * 1000000 + final_time_u - start_time_u;
  // printf("%d %d %d %.0f\n\n", ntasks, golden_index_max, nsats, bf_time);
  // print_array("Solution", solution, nsats);
  // printf("%.2f\n", max);
  // printf("Duplicates: %d\n", duplicates);
  free_satellites(sats);
  free(solution);
  free(combination);
  return 0;
}
