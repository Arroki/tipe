#include "include/dft.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

fct new_f(int n) {
  fct f = {n, malloc(n * sizeof(cpl_xy))};
  return f;
}

void free_fct(fct f) { free(f.points); }

void affiche_coeff(cpl_rt val[], int n) {

  for (int i = 0; i < n; i++) {
    if (i % 2 == 0) {
      printf("coeff %d : r %lf, theta: %lf\n", -i / 2, val[i].r, val[i].theta);
    } else {
      printf("coeff %d : r %lf, theta: %lf\n", ((i / 2) + 1), val[i].r,
             val[i].theta);
    }
  }
}

void affiche_coeff_2(cpl_rt val1[], cpl_rt val2[], int n) {
  for (int i = 0; i < n; i++) {
    if (i % 2 == 0) {
      printf("coeff %d : r : %lf | %lf, theta : %lf | %lf\n", -i / 2, val1[i].r,
             val2[i].r, val1[i].theta, val2[i].theta);
    } else {
      printf("coeff %d : r : %lf | %lf, theta : %lf | %lf\n", (i / 2) + 1,
             val1[i].r, val2[i].r, val1[i].theta, val2[i].theta);
    }
  }
}

cpl_xy rttoxy(cpl_rt z) {
  cpl_xy z0 = {z.r * cos(z.theta), z.r * sin(z.theta)};
  return z0;
}

cpl_rt xytort(cpl_xy z) {
  cpl_rt z0;

  if (z.x == 0 && z.y == 0) {
    z0.r = 0;
    z0.theta = 0;
    return z0;
  }

  double theta;

  if (z.x == 0) {
    if (z.y > 0) {
      theta = _Pi / 2;
    } else {
      theta = -_Pi / 2;
    }
  } else {
    theta = atan2(z.y, z.x);
  }

  z0.r = sqrt(pow(z.x, 2) + pow(z.y, 2));
  z0.theta = theta;
  return z0;
}

cpl_rt expim(double theta) {
  cpl_rt z = {1, theta};
  return z;
}

cpl_xy expimx(double theta) { return rttoxy(expim(theta)); }

cpl_xy add(cpl_xy z1, cpl_xy z2) {
  cpl_xy z0 = {z1.x + z2.x, z1.y + z2.y};
  return z0;
}

cpl_rt add_rt(cpl_rt z1, cpl_rt z2) {
  return xytort(add(rttoxy(z1), rttoxy(z2)));
}

cpl_xy neg(cpl_xy z) {
  cpl_xy z0 = {-z.x, -z.y};
  return z0;
}

cpl_rt neg_rt(cpl_rt z) { return xytort(neg(rttoxy(z))); }

cpl_xy mult(cpl_xy z1, cpl_xy z2) {
  cpl_xy z0 = {z1.x * z2.x - z1.y * z2.y, z1.x * z2.y + z1.y * z2.x};
  return z0;
}

cpl_rt mult_rt(cpl_rt z1, cpl_rt z2) {
  return xytort(mult(rttoxy(z1), rttoxy(z2)));
}

cpl_xy conjugate(cpl_xy z) {
  cpl_xy z0 = {z.x, -z.y};
  return z0;
}

fct expcpl(int n, int k) {
  fct e = new_f(n);
  for (int i = 0; i < n; i++) {
    e.points[i] = expimx(k * 2 * i * _Pi / n);
  }

  return e;
}

cpl_xy psh(fct f, fct g, int k) {
  assert(f.n == g.n);

  int n = f.n;

  cpl_xy sum = {0, 0};

  for (int i = 0; i < n; i++) {
    sum = add(sum, mult(f.points[i], conjugate(g.points[i])));
  }

  sum.x /= k;
  sum.y /= k;
  return sum;
}

cpl_rt *fourier(fct pts) {
  int n = pts.n;
  cpl_rt *coeff = malloc(n * sizeof(cpl_rt));

  fct e;

  for (int i = 0; i < n; i++) {
    if (i % 2 == 0) {
      e = expcpl(n, -i / 2);
    } else {
      e = expcpl(n, (i / 2) + 1);
    }

    coeff[i] = xytort(psh(pts, e, n));
    free_fct(e);
  }

  return coeff;
}

cpl_rt *_ditfft2(fct pts) {
  // On suppose sans vérifier que le nombre de points est une puissance de 2
  int n = pts.n;
  cpl_rt *coeff = malloc(n * sizeof(cpl_rt));

  if (pts.n == 1) {
    coeff[0] = xytort(pts.points[0]);
    return coeff;
  } else {
    fct f1 = {n / 2, malloc(n / 2 * sizeof(cpl_rt))};
    fct f2 = {n / 2, malloc(n / 2 * sizeof(cpl_rt))};
    for (int i = 0; i < n / 2; i++) {
      f1.points[i] = pts.points[2 * i];
      f2.points[i] = pts.points[2 * i + 1];
    }

    cpl_rt *coeff1 = _ditfft2(f1);
    cpl_rt *coeff2 = _ditfft2(f2);

    for (int i = 0; i < n / 2; i++) {
      cpl_rt p = coeff1[i];
      cpl_rt q = mult_rt(expim(-2 * _Pi * i / n), coeff2[i]);
      coeff[i] = add_rt(p, q);
      coeff[i + (n / 2)] = add_rt(p, neg_rt(q));
    }
    free(coeff1);
    free(coeff2);
    free_fct(f1);
    free_fct(f2);
  }
  return coeff;
}

cpl_rt *ditfft2(fct pts) {
  int n = pts.n;
  cpl_rt *coeff = _ditfft2(pts);
  cpl_rt *coeff2 = malloc(n * sizeof(cpl_rt));
  coeff2[0] = coeff[0];
  for (int i = 1; i < n / 2; i++) {
    coeff2[2 * i - 1] = coeff[i];
    coeff2[2 * i] = coeff[n - i];
    coeff2[2 * i - 1].r = coeff2[2 * i - 1].r / n;
    coeff2[2 * i].r = coeff2[2 * i].r / n;
    //    coeff[i].r = coeff[i].r * (1.0 / n);
    //  coeff[n - 1 - i].r = coeff[n - 1 - i].r * (1.0 / n);
  }
  coeff2[n - 1] = coeff[n / 2];
  coeff2[n - 1].r = coeff2[n - 1].r / n;
  free(coeff);
  return coeff2;
}

/*cpl_rt *ditfft4(cpl_xy *arr, int N, int s) {
  cpl_rt *coeff = malloc(N * sizeof(cpl_rt));
  if (N == 1) {
    coeff[0] = xytort(arr[0]);
    return coeff;
  } else {
    cpl_rt *coeff1 = ditfft4(arr, N / 2, 2 * s);
    cpl_rt *coeff2 = ditfft4(arr + s, N / 2, 2 * s);
    for (int i = 0; i < N / 2; i++) {
      coeff[i] = coeff1[i];
      coeff[i + (N / 2)] = coeff2[i];
    }
    for (int k = 0; k < N / 2; k++) {
      cpl_rt p = coeff[k];
      cpl_rt q = mult_rt(expim((-2.0 * _Pi * k) / N), coeff[k + (N / 2)]);
      coeff[k] = add_rt(p, q);
      coeff[k + N / 2] = add_rt(p, neg_rt(q));
    }
    free(coeff1);
    free(coeff2);
  }

  return coeff;
}*/

void print_rt(cpl_rt z) { printf("%lf * exp(i %lf)\n", z.r, z.theta); }

void test_rt() {
  cpl_rt z1 = {.r = 861, .theta = 0};
  cpl_rt z2 = {.r = 2024, .theta = 0};

  print_rt(add_rt(z1, z2));
  print_rt(mult_rt(z1, z2));
  print_rt(neg_rt(z2));
  print_rt(add_rt(neg_rt(z1), z2));
  print_rt(add_rt(neg_rt(z2), z1));

  cpl_rt z3 = {.r = 2, .theta = 2};
  cpl_rt z4 = {.r = 4, .theta = -1.58};

  print_rt(add_rt(z3, z4));
  print_rt(mult_rt(z3, z4));
  print_rt(neg_rt(z4));
  print_rt(add_rt(neg_rt(z3), z4));
  print_rt(add_rt(neg_rt(z4), z3));

  for (int x = -20; x < 20; x++) {
    for (int y = -20; y < 20; y++) {
      cpl_xy z = {x * 1.0, y * 1.0};
      cpl_xy z2 = rttoxy(xytort(z));
      printf("%d + i %d || ", x, y);
      printf("%lf + i %lf\n", z2.x, z2.y);
    }
  }
  /*
    for (int x = 0; x < 100; x++) {
      for (int y = 0; y < 100; y++) {
        cpl_rt z = {x * 1.0, y * 1.0};
        cpl_rt z2 = xytort(rttoxy(z));
        print_rt(z);
        print_rt(z2);
      }
    }*/
}

void perf_test(char *filename, char *testname) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    assert(false && "le fichier n'existe pas");
  }

  int taille;
  fscanf(file, "%d", &taille);

  cpl_xy *coord = malloc(taille * sizeof(cpl_xy));

  double x = 0.;
  double y = 0.;

  for (int i = 0; i < taille; i++) {
    fscanf(file, "%lf, %lf", &x, &y);
    cpl_xy v = {x, y};
    coord[i] = v;
  }
  fclose(file);
  fct f = {taille, coord};
  printf("Test %s\n", testname);
  clock_t t = clock();
  cpl_rt *coeff1 = fourier(f);
  clock_t t1 = clock() - t;
  printf("Naive DFT took %f seconds.\n", ((float)t1) / CLOCKS_PER_SEC);
  t = clock();
  cpl_rt *coeff2 = ditfft2(f);
  clock_t t2 = clock() - t;

  printf("Cooley-Tukey took %f seconds.\n", ((float)t2) / CLOCKS_PER_SEC);

  printf("\n");
  // affiche_coeff_2(coeff1, coeff2, taille);

  free(coeff1);
  free(coeff2);
  free(coord);
}

int main2() {
  /*
    FILE *file = fopen("drawings/square_65536.pts", "r");

    if (file == NULL) {
      assert(false && "le fichier n'existe pas");
    }

    int taille;
    fscanf(file, "%d", &taille);

    cpl_xy *coord = malloc(taille * sizeof(cpl_xy));

    double x = 0.;
    double y = 0.;

    for (int i = 0; i < taille; i++) {
      fscanf(file, "%lf, %lf", &x, &y);
      cpl_xy v = {x, y};
      coord[i] = v;
    }
    fclose(file);
    fct f = {taille, coord};

    clock_t t = clock();
    cpl_rt *coeff1 = fourier(f);
    clock_t t1 = clock() - t;
    printf("Naive DFT took %f seconds.\n", ((float)t1) / CLOCKS_PER_SEC);
    t = clock();
    cpl_rt *coeff2 = ditfft2(f);
    clock_t t2 = clock() - t;

    printf("Cooley-Tukey took %f seconds.\n", ((float)t2) / CLOCKS_PER_SEC);

    // affiche_coeff_2(coeff1, coeff2, taille);

    free(coeff1);
    free(coeff2);
    // test_rt();
    */

  perf_test("drawings/square64.pts", "64 points");
  perf_test("drawings/square_256.pts", "256 points");
  perf_test("drawings/square_1024.pts", "1024 points");
  perf_test("drawings/square_4096.pts", "4096 points");
  perf_test("drawings/square_16384.pts", "16384 points");
  perf_test("drawings/square_65536.pts", "65536 points");

  return 0;
}
