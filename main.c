#include "include/dft.h"
#include "include/epicycloid.h"
#include "include/raylib.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_power_of_two(int n) {
  int check = 0;
  int m = n;
  for (int i = 0; i < 63; i++) {
    check += m % 2;
    m = m >> 1;
  }

  return check == 1;
}

fct points(char *filename) {
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

  fct f = {taille, coord};
  fclose(file);
  return f;
}

circle coordtocircle(cpl_rt z, int k, double v0) {
  circle c = {z.r, k * v0, z.theta};
  return c;
}

/*void filter(circle *t, int *n) {
  int sum = 0;
  for (int i = 0; i < *n; i++) {
    if (t[i].radius >= 1) {
      sum += 1;
    }
  }
  *n = sum;
  circle temp;

  int c = 0;
  for (int i = 0; i < sum; i++) {
    while (t[c].radius < 0.1) {
      c++;
    }

    temp = t[i];
    t[i] = t[c];
    t[c] = temp;

    c++;
  }
}*/

void swap(circle *tab, int i, int j) {
  circle tmp = tab[i];
  tab[i] = tab[j];
  tab[j] = tmp;
}

int partition(circle *tab, int n) {
  int i = 1;
  circle c = tab[0];
  for (int j = 1; j < n; j++) {
    if (tab[j].radius > c.radius) {
      swap(tab, i, j);
      i++;
    }
  }
  swap(tab, 0, i - 1);
  return i - 1;
}

void selection_sort(circle *tab, int n) {
  for (int i = 0; i < n; i++) {
    int i_min = i;
    for (int j = i + 1; j < n; j++) {
      if (tab[j].radius >= tab[i_min].radius) {
        i_min = j;
      }
    }
    swap(tab, i, i_min);
  }
}

void sort(circle *tab, int n) {
  if (n <= 1) {
    return;
  }
  if (n <= 16) {
    selection_sort(tab, n);
    return;
  }
  int p = partition(tab, n);
  sort(tab, p);
  sort(tab + p + 1, n - p - 1);
}

epicycloid fcttoepi(fct f, double v0) {
  cpl_rt *coeff;
  if (is_power_of_two(f.n)) {
    coeff = ditfft2(f);
    // coeff = fourier(f);

  } else {
    coeff = fourier(f);
  }
  int n;

  n = f.n;

  circle *tab = malloc(n * sizeof(circle));

  n--;

  for (int i = 1; i < 1 + n; i++) {
    if (i % 2 == 0) {
      tab[i - 1] = coordtocircle(coeff[i], -i / 2, v0);
    } else {
      tab[i - 1] = coordtocircle(coeff[i], (i / 2) + 1, v0);
    }
  }

  tab[n] = coordtocircle(coeff[0], 0, 0);

  // filter(tab, &n);

  sort(tab, n);

  free(coeff);

  epicycloid e = {n, tab};
  return e;
}

/*
epicycloid eofptsfile(char *filename, double v0) {
  return fcttoepi(points(filename), v0);
}*/

epicycloid eofepifile(char *filename, double v0) {
  FILE *file = fopen(filename, "r");

  int n = 0;

  fscanf(file, "%d", &n);

  epicycloid e;

  e.n = n;
  e.data = malloc(n * sizeof(circle));

  circle c;

  for (int i = 0; i < n; i++) {
    fscanf(file, "%lf, %lf, %lf", &c.radius, &c.a_speed, &c.s_angle);
    c.a_speed = c.a_speed * v0;
    e.data[i] = c;
  }

  fclose(file);

  return e;
}

void save_epi(epicycloid e, char *filename) {
  FILE *f = fopen(filename, "w");
  fprintf(f, "%d\n", e.n);
  for (int i = 0; i < e.n; i++) {
    fprintf(f, "%lf,%lf,%lf\n", e.data[i].radius, e.data[i].a_speed,
            e.data[i].s_angle);
  }

  fclose(f);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    assert(false && "pas bon nombre");
  }

  char *name = argv[2];
  double v0 = PI / 1;
  double factor_r = 1;
  double factor_v = 1;
  epicycloid epi;
  vcyl *coord;
  float *speed;

  fct function = {.n = 0, .points = malloc(sizeof(cpl_xy))};
  cpl_xy z = {.x = 0, .y = 0};
  function.points[0] = z;
  Vector2 decal = {.x = 0, .y = 0};

  if (0 == strcmp(argv[1], "-epi")) {
    epi = eofepifile(name, v0);
  } else if (0 == strcmp(argv[1], "-pts")) {
    free_fct(function);
    function = points(name);
    epi = fcttoepi(function, v0);
    Vector2 temp = {.x = epi.data[epi.n].radius * cos(epi.data[epi.n].s_angle),
                    .y = epi.data[epi.n].radius * sin(epi.data[epi.n].s_angle)};
    decal = temp;
  } else {
    assert(false && "pas bonne extension");
  }

  int n = epi.n;

  InitWindow(SWIDTH, SLENGTH, "Main");

  SetTargetFPS(FPS);

  set *s = empty_set(8);

  coord = etovtab(epi);
  speed = etoatab(epi);

  bool fin = false;
  bool pause = false;
  bool points = false;
  bool curve = true;
  bool circle = false;
  bool scheme = true;
  int compteur = 0;

  while (!fin && !WindowShouldClose()) {

    if ((IsKeyPressed(KEY_ESCAPE)) ||
        (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C))) {
      fin = true;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D)) {
      pause = true;
      free_set(s);
      s = empty_set(8);
      free(coord);
      coord = etovtab(epi);
    }

    if (IsKeyPressed(KEY_SPACE)) {
      pause = !pause;
    }

    if (IsKeyPressed(KEY_LEFT)) {
      factor_v /= 1.2;
      compteur = 0;
    }

    if (IsKeyDown(KEY_LEFT)) {
      if (compteur < 90)
        compteur++;
      else {
        factor_v /= 1.2;
        compteur = 80;
      }
    }

    if (IsKeyPressed(KEY_RIGHT)) {

      factor_v *= 1.2;
      compteur = 0;

      /*if (factor_v > 5) {
        factor_v = 5;
      }*/
    }

    if (IsKeyDown(KEY_RIGHT)) {
      if (compteur < 90)
        compteur++;
      else {
        factor_v *= 1.2;
        compteur = 80;
      }
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S) &&
        (0 == strcmp(argv[1], "-pts"))) {
      char *a = malloc((strlen(name) + 1) * sizeof(char));
      for (int i = 0; i < (int)strlen(name) - 3; i++) {
        a[i] = name[i];
      }
      a[strlen(name) - 3] = 'e';
      a[strlen(name) - 2] = 'p';
      a[strlen(name) - 1] = 'i';
      a[strlen(name)] = '\0';

      save_epi(epi, a);

      printf("save ok \n");

      free(a);
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_KP_ADD)) {
      factor_r *= 1.2;

      free_set(s);
      s = empty_set(8);
    }

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_KP_ADD)) {
      if (compteur < 90)
        compteur++;
      else {
        factor_r *= 1.2;

        free_set(s);
        s = empty_set(8);
        compteur = 80;
      }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_KP_SUBTRACT)) {
      factor_r /= 1.2;

      free_set(s);
      s = empty_set(8);
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_KP_SUBTRACT)) {
      if (compteur < 90)
        compteur++;
      else {
        factor_r /= 1.2;

        free_set(s);
        s = empty_set(8);
        compteur = 80;
      }
    }

    if (IsKeyReleased(KEY_DOWN) || IsKeyReleased(KEY_UP) ||
        IsKeyReleased(KEY_LEFT) || IsKeyReleased(KEY_RIGHT) ||
        IsKeyReleased(KEY_KP_ADD) || IsKeyReleased(KEY_DOWN) ||
        IsKeyReleased(KEY_KP_SUBTRACT)) {
      compteur = 0;
    }
    if (IsKeyPressed(KEY_R)) {
      free_set(s);
      s = empty_set(8);
    }

    if (IsKeyPressed(KEY_P)) {
      points = !points;
    }

    if (IsKeyPressed(KEY_E)) {
      curve = !curve;
    }

    if (IsKeyPressed(KEY_L)) {
      circle = !circle;
    }

    if (IsKeyPressed(KEY_U)) {
      scheme = !scheme;
    }

    if (!pause) {
      iter_inc_theta(coord, speed, n, factor_v);
    }

    BeginDrawing();

    {
      ClearBackground(WHITE);
      iter_draw(coord, n, s, factor_r, epi, scheme, circle);
      if (curve) {
        iter_draw_set(s);
      }
      if (scheme) {
        DrawCircleV(ORIGIN, 5.0f, BLACK);
      }

      DrawText(TextFormat("Vitesse: %lf\nZoom: %lf", factor_v, factor_r), 10,
               10, 10, BLACK);
      if (points) {
        draw_fct(function, decal);
      }
      // DrawLine(200, 0, 200, 1100, BLACK);
      // DrawLine(1300, 0, 1300, 1100, BLACK);
    }

    EndDrawing();
  }

  free_set(s);
  free_epi(epi);
  free_fct(function);
  free(coord);
  free(speed);

  return 0;
}
