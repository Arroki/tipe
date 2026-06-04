#ifndef DFT_H_

#define DFT_H_

#define _Pi 3.14159265358979323846

struct cpl_xy {
  double x;
  double y;
};

struct cpl_rt {
  double r;
  double theta;
};

struct fct {
  int n;
  struct cpl_xy *points;
};

typedef struct cpl_xy cpl_xy;
typedef struct cpl_rt cpl_rt;
typedef struct fct fct;

void affiche_coeff(cpl_rt val[], int n);

void free_fct(fct f);

cpl_rt *fourier(fct pts);

cpl_rt *ditfft2(fct pts);

#endif
