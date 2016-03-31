#include "chess.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;

#define PI 3.14159265


extern float grid;
extern Point boardcenter;
extern float theta;
float s = sin(theta * PI / 180);
float c = cos(theta * PI / 180);
/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection,
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
float intersect_chess(Point o, Vector u, Chess *che, Point *hit) {
  Point a = che->vertex[0];
  Point b = che->vertex[1];
  Point c = che->vertex[2];
  Vector nor = che->nor;
  float y = (a.x - o.x) * nor.x + (a.y - o.y) * nor.y + (a.z - o.z) *nor.z;
  float x = u.x * nor.x + u.y * nor.y + u.z * nor.z;
  float t = y / x;

  hit->x = o.x + t * u.x;
  hit->y = o.y + t * u.y;
  hit->z = o.z + t * u.z;

  Vector AC = get_vec(a, c);
  Vector AB = get_vec(a, b);
  Vector AH = get_vec(a, *hit);

  float n = (AH.x * AC.y - AH.y * AC.x) / (AB.x * AC.y - AB.y * AC.x);
  float m = (AH.x - n * AB.x) / AC.x;
  if (t > 0.0001 && m >= 0 && n >=0 && (m + n) <= 1)
    return t;
  return -1.0;
}

bool intersect_board(Point o, Vector u, float *x, float *y, Point *hit)
{
  float t = (s * boardcenter.z + c * boardcenter.y - s * o.z - c * o.y) / (s * u.z + c * u.y);
  if (t < 0)
    return false;
  hit->x = o.x + t * u.x;
  hit->y = o.y + t * u.y;
  hit->z = o.z + t * u.z;
  *x = (o.x + t * u.x - boardcenter.x) / grid;
  *y = (o.y + t * u.y - boardcenter.y) / (s * grid);
  return ((abs(*x) < 4.0) && (abs(*y) < 4.0));
}
/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point o, Vector u, Chess *che, Point *hit) {
  Chess *head = che;
  Chess *re = NULL;
  Point *temp = new Point;
  float closest = 500;

  while (head)
  {
    float t = intersect_chess(o, u, head, tmp);
    if (t > 0.0001 && t <closest)
    {
      closest = t;
      *hit = *temp;
      re =head;
    }
    head = head->next;
  }
  delete tmp;
  return re;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Chess *add_chess(Chess *clist, Point v[], Vector normal, float amb[],
        float dif[], float spe[], float shine,
        float refl, float refr, float eta, int cindex) {
  Chess *new_chess;

  new_chess = (Chess *)malloc(sizeof(Chess));
  new_chess->index = cindex;
  new_chess->nor = normal;
  new_chess->vertex[0] = v[0];
  new_chess->vertex[1] = v[1];
  new_chess->vertex[2] = v[2];
  (new_chess->mat_ambient)[0] = amb[0];
  (new_chess->mat_ambient)[1] = amb[1];
  (new_chess->mat_ambient)[2] = amb[2];
  (new_chess->mat_diffuse)[0] = dif[0];
  (new_chess->mat_diffuse)[1] = dif[1];
  (new_chess->mat_diffuse)[2] = dif[2];
  (new_chess->mat_specular)[0] = spe[0];
  (new_chess->mat_specular)[1] = spe[1];
  (new_chess->mat_specular)[2] = spe[2];
  new_chess->mat_shineness = shine;
  new_chess->reflectance = refl;
  new_chess->refraction = refr;
  new_chess->eta = eta;
  new_chess->next = NULL;

  if (clist == NULL) { // first object
    clist = new_chess;
  } else { // insert at the beginning
    new_chess->next = clist;
    clist = new_chess;
  }

  return clist;
}
/******************************************
 * computes a sphere normal - done for you
 ******************************************/
// Vector sphere_normal(Point q, Spheres *sph) {
//   Vector rc;
//
//   rc = get_vec(sph->center, q);
//   normalize(&rc);
//   return rc;
// }
