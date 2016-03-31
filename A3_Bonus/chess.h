/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
#include "vector.h"

typedef struct chess {
  int index;               // identifies a sphere; must be greater than 0

  Point vertex[3];

  Vector nor;

  float mat_ambient[3];    // material property used in Phong model
  float mat_diffuse[3];
  float mat_specular[3];
  float mat_shineness;

  float reflectance;       // this number [0,1] determines how much
                           // reflected light contributes to the color
                           // of a pixel
  float refraction;
  float eta;
  struct chess *next;
} Chess;   // a list of spheres

// intersect ray with sphere
Chess *intersect_scene(Point, Vector, Chess *, Point *, int);
// return the unit normal at a point on sphere
//Vector sphere_normal(Point, Spheres *);
// add a sphere to the sphere list
Chess *add_chess(Chess *, Point [], Vector, float [], float [], float [], float, float, float, float, int);
bool intersect_board(Point o, Vector u, float *x, float *y, Point *hit);
