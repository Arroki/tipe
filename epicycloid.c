#include "include/epicycloid.h"
#include "include/dft.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

void affiche_vec(Vector2 x) { printf("x: %f, y: %f\n", x.x, x.y); }

void affiche_angle(Vector2 x) { printf("%f\n", atan2(x.y, x.x)); }

set *empty_set(int c) {
  set *j = malloc(sizeof(set));
  j->capacity = c;
  j->size = 0;
  j->data = malloc(c * sizeof(Vector2));
  return j;
}

void free_set(set *s) {
  free(s->data);
  free(s);
}

void cp(set *s, set *e) {
  assert(e->capacity >= s->size);

  for (int i = 0; i < s->size; i++) {
    // printf("s: ");
    // affiche_vec(s->data[i]);
    e->data[i] = s->data[i];
    // printf("e: ");
    // affiche_vec(e->data[i]);
    e->size = s->size;
  }
}

void add_set(set *s, Vector2 x) {

  // affiche_vec(x);
  if ((s->size) > (s->capacity / 2)) {
    // printf("size : %d, capacity: %d\n", s->size, s->capacity);

    set *e = empty_set(s->capacity);
    cp(s, e);

    free(s->data);

    s->capacity *= 2;
    s->data = malloc(s->capacity * sizeof(Vector2));

    cp(e, s);

    free_set(e);
  }

  s->data[s->size++] = x;
}

Vector2 cyltoxy(vcyl u, Vector2 origin) {
  Vector2 v = {(float)(u.r * cos(u.theta) + origin.x),
               (float)(-u.r * sin(u.theta) + origin.y)};
  // printf("%f %f! \n", v.x, v.y);
  return v;
}

vcyl inc_theta(vcyl u, float a) {
  double b = a + u.theta;
  while (b > 2. * _Pi) {
    b -= 2. * _Pi;
  }
  while (b <= 0) {
    b += 2. * _Pi;
  }

  vcyl v = {u.r, b};
  return v;
}

vcyl ctov(circle c) {
  vcyl v = {c.radius, c.s_angle};
  return v;
}

vcyl *etovtab(epicycloid e) {
  vcyl *tab = malloc(e.n * sizeof(vcyl));

  tab[0] = ctov(e.data[0]);

  for (int i = 1; i < e.n; i++) {
    // tab[i] = add(ctov(e.data[i]), tab[i - 1]);
    tab[i] = ctov(e.data[i]);
  }

  return tab;
}

float *etoatab(epicycloid e) {
  float *tab = malloc(e.n * sizeof(float));
  tab[0] = e.data[0].a_speed;
  for (int i = 1; i < e.n; i++) {
    tab[i] = e.data[i].a_speed;
  }

  return tab;
}

void iter_inc_theta(vcyl *t, float *a, int n, double fv) {
  for (int i = 0; i < n; i++) {
    t[i] = inc_theta(t[i], (a[i] * fv) / FPS);
  }
}

void iter_draw(vcyl *t, int n, set *s, double fr, epicycloid e, bool scheme,
               bool circle) {
  vcyl h = t[0];
  h.r *= fr;

  Vector2 v = cyltoxy(h, ORIGIN);
  Vector2 w = v;
  if (scheme) {
    DrawCircleV(v, 5.0f, BLACK);
    DrawLineV(ORIGIN, v, BLACK);
  }
  if (circle && n >= 1) {
    DrawCircleLines(ORIGIN.x, ORIGIN.y, e.data[0].radius * fr, MAROON);
    DrawCircleLines(w.x, w.y, e.data[1].radius * fr, MAROON);
  }

  for (int i = 1; i < n; i++) {
    h = t[i];
    h.r *= fr;
    w = cyltoxy(h, v);
    if (scheme) {
      DrawCircleV(w, 5.0f, BLACK);
      DrawLineV(v, w, BLACK);
    }
    if (circle && i < n - 1) {
      DrawCircleLines(w.x, w.y, e.data[i + 1].radius * fr, MAROON);
    }
    v = w;
  }

  // printf("%f\n", t[n - 1].theta);

  add_set(s, w);
}

void draw_fct(fct f, Vector2 decal) {
  for (int i = 0; i < f.n; i++) {
    Vector2 w = {.x = f.points[i].x + ORIGIN.x - decal.x,
                 .y = -f.points[i].y + ORIGIN.y + decal.y};
    DrawCircleV(w, 5, BLACK);
  }
}

void iter_draw_set(set *s) {
  for (int i = 1; i < s->size; i++) {
    DrawLineV(s->data[i - 1], s->data[i], BLACK);
  }
}

/*
void draw_circles(epicycloid e) {

  DrawCircle(int centerX, int centerY, float radius, Color color);
}*/

void free_epi(epicycloid e) { free(e.data); }

/*
int main() {
  InitWindow((float)SWIDTH, (float)SLENGTH, "Bonjour !");

  SetTargetFPS(FPS);

  double v0 = _Pi / 3;

  set *s = empty_set(8);
  ///////////////////////////////////////////////////////////////////////////////////
  //////// ONLY PART TO MODIFY TO ADD / REMOVE / CHANGE THE CIRCLES (Avoidgoing
  /// over too fast speed)//////////////////

  int n = 2;

  circle c1 = {307.387218, -v0, 0.785398};
  circle c2 = {19.810598401, -v0, 1.580403};
  circle c3 = {125.447508869, 2 * v0, 1.257182};
  circle c4 = {56.126526005, -2 * v0, 2.365192};
  circle c5 = {52.739309, 3 * v0, 0.785398};
  circle c6 = {81.811424473, -3 * v0, 2.338138};
  circle c7 = {125.474876928, 4 * v0, 1.900867};

  circle tab[2] = {c1, c5};

  ///////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////

  epicycloid e1;

  e1.n = n;
  e1.data = tab;

  vcyl *coord = etovtab(e1);
  float *speed = etoatab(e1);

  bool fin = false;
  bool pause = false;
  float compt = 0;

  while (!fin && !WindowShouldClose()) {

    if ((IsKeyPressed(KEY_ESCAPE)) ||
        IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
      fin = true;
    }

    if (IsKeyPressed(KEY_SPACE)) {
      pause = !pause;
    }

    if (!pause) {
      iter_inc_theta(coord, speed, n, 1);
    }

    BeginDrawing();

    {
      ClearBackground(LIGHTGRAY);

      iter_draw(coord, n, s, 1);
      iter_draw_set(s);

      DrawCircleV(ORIGIN, 5.0f, BLACK);
    }

    EndDrawing();
  }

  free_set(s);

  return 0;
}
*/
