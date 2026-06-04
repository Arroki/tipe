#ifndef EPICYCLOID_H_

#define EPICYCLOID_H_

#include "dft.h"
#include <raylib.h>

#define SWIDTH 1500
#define SLENGTH 1100
#define FPS 144
#define ORIGIN (Vector2){SWIDTH / 2.0f, SLENGTH / 2.0f}

struct vec_cyl {
  double r;
  double theta;
};

struct circle {
  double radius;
  double a_speed;
  double s_angle;
};

struct epicycloid {
  int n;
  struct circle *data;
};

struct set {
  Vector2 *data;
  int size;
  int capacity;
};

typedef struct vec_cyl vcyl;
typedef struct circle circle;
typedef struct epicycloid epicycloid;
typedef struct set set;

void affiche_vec(Vector2 x);

void affiche_angle(Vector2 x);

set *empty_set(int c);

void free_set(set *s);

void cp(set *s, set *e);

void add_set(set *s, Vector2 x);

Vector2 cyltoxy(vcyl u, Vector2 origin);

vcyl inc_theta(vcyl u, float a);

vcyl ctov(circle c);

vcyl *etovtab(epicycloid e);

float *etoatab(epicycloid e);

void iter_inc_theta(vcyl *t, float *a, int n, double fv);

void iter_draw(vcyl *t, int n, set *s, double fr, epicycloid e, bool scheme,
               bool circle);

void draw_fct(fct f, Vector2 decal);

void iter_draw_set(set *s);

void free_epi(epicycloid e);

#endif
