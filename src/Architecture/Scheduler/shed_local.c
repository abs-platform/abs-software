#include "shed_mat.h"

void generate_tasks(int *tasks) {
  int i;
  int n_ones = rand() % ntasks + 1;
  int a = 0; // number of ones put

  for (i = 0; i < ntasks; i++) {
    tasks[i] = 0;
  }
  while (a < n_ones) {
    i = rand() % ntasks;
    if (tasks[i] == 1)
      continue;
    else {
      tasks[i] = 1;
      a++;
    }
  }
}

int generate_local_solution(int id, LocalSolution *ls, float F) {

  ls->id = id;
  ls->F = F;
  generate_tasks(ls->tasks);
  return 0;
}

void generate_solutions(Satellite *sat) {
  int i, j;
  int c, d;
  int error;
  float *F;
  float temp;

  error = generate_array_float(sat->golden_index, &F);
  for (j = 0; j < sat->golden_index; j++) {
    F[j] = (float)((rand() % 99) + 1) / 100;
  }

  for (c = 0; c < (sat->golden_index - 1); c++) {
    for (d = 0; d < sat->golden_index - c - 1; d++) {
      if (F[d] < F[d + 1]) {
        temp = F[d];
        F[d] = F[d + 1];
        F[d + 1] = temp;
      }
    }
  }
  for (i = 0; i < sat->golden_index; i++) {
    generate_local_solution(i + 1, &(sat->local_solutions[i]), F[i]);
  }
  free(F);
}
