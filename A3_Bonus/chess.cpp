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
  Point p1 = che->vertex[0];
  Point p2 = che->vertex[1];
  Point p3 = che->vertex[2];
  Point p;
  Vector nor = che->nor;
  if (vec_dot(u, nor) > 0)
    inverse(&nor);
  float y = (p1.x - o.x) * nor.x + (p1.y - o.y) * nor.y + (p1.z - o.z) *nor.z;
  float x = u.x * nor.x + u.y * nor.y + u.z * nor.z;
  float t = y / x;
  p.x = o.x + t * u.x;
  p.y = o.y + t * u.y;
  p.z = o.z + t * u.z;

  hit->x = o.x + t * u.x;
  hit->y = o.y + t * u.y;
  hit->z = o.z + t * u.z;

  float alpha = ((p2.y - p3.y)*(p.x - p3.x) + (p3.x - p2.x)*(p.y - p3.y)) /
        ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
  float beta = ((p3.y - p1.y)*(p.x - p3.x) + (p1.x - p3.x)*(p.y - p3.y)) /
       ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
  float gamma = 1.0f - alpha - beta;

  //cout << (vec_dot(AH, nor) < 0.0001) <<endl;
  //float n = ((b.y - c.y) * (hit->x - c.x) + (c.x - b.x) * (hit->y - c.y)) / ((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y));
  //float m = ((c.y - a.y) * (hit->x - c.x) + (a.x - c.x) * (hit->y - c.y)) / ((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y));
  if (t > 0.0001 && alpha >= 0 && beta >= 0 && gamma >= 0)
  {
    return t;
  }
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
  return ((abs(*x) < 30.0) && (abs(*y) < 30.0));
}
/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
chess *intersect_scene(Point o, Vector u, Chess *che, Point *hit) {
  Chess *head = che;
  Chess *re = NULL;
  Point *temp = new Point;
  float closest = 50000;

  while (head)
  {
    float t = intersect_chess(o, u, head, temp);
    if (t > 0.0001 && t <closest)
    {
      closest = t;
      *hit = *temp;
      re =head;
    }
    head = head->next;
  }
  delete temp;
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
