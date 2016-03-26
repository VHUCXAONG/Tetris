#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.h"
#include <iostream>
using namespace std;
//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;

// light 1 position and color
extern Point light1;
extern float light1_intensity[3];

// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int step_max;
extern int l;
extern int r;
extern int board;

extern float board_ambient;
extern float board_k;

extern float sky_eta;
/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
 RGB_float white = {0,0,0};
 RGB_float black = {1,1,1};
bool CheckShadow(Point *hit) {
  Vector n;
  Spheres *head = scene;
  float x1, y1, z1, a, b, c;
  n = get_vec(light1, *hit);
  while (head) {
    x1 = light1.x - head->center.x;
    y1 = light1.y - head->center.y;
    z1 = light1.z - head->center.z;
    a = n.x * n.x + n.y * n.y + n.z * n.z;
    b = 2.0 * (n.x * x1 + n.y * y1 + n.z * z1);
    c = x1 * x1 + y1 * y1 + z1 * z1 - (head->radius) * (head->radius);
    float delta = b * b - 4 * a * c;
    if (delta) {
      float t = (-b - sqrt(delta)) / (a * 2);
      if (1 - t > 0.0001)
        return true;
    }
    head = head->next;
  }
  return  false;
}

Vector Reflect(Point in, Point hit, Vector n)
{
  Point mid, asy;
  float t = ((in.x - hit.x) * n.x + (in.y - hit.y) * n.y + (in.z - hit.z) * n.z) / (n.x * n.x + n.y * n.y + n.z * n.z);
  mid.x = hit.x + t * n.x;
  mid.y = hit.y + t * n.y;
  mid.z = hit.z + t * n.z;
  asy.x = 2 * mid.x - in.x;
  asy.y = 2 * mid.y - in.y;
  asy.z = 2 * mid.z - in.z;
  return get_vec(hit, asy);
}

int trans(float x)
{
  if (x < 0)
     return (-(int)(abs(x)));
  return (int)x + 1;
}

Point intersect_refraction(Point o, Vector u, Spheres *sph)
{
  Point hit;
  float x1 = o.x - sph->center.x;
  float y1 = o.y - sph->center.y;
  float z1 = o.z - sph->center.z;
  float a = u.x * u.x + u.y * u.y + u.z * u.z;
  float b = 2.0 * (u.x * x1 + u.y * y1 + u.z * z1);
  float c = x1 * x1 + y1 * y1 + z1 * z1 - (sph->radius) * (sph->radius);
  float delta = b * b - 4 * a * c;
  float t = (-b + sqrt(delta)) / (a * 2);
  cout <<sph->index <<endl;
  cout <<t<<endl;
  //std::cout <<t<<std::endl;
  hit.x = o.x + t * u.x;
  hit.y = o.y + t * u.y;
  hit.z = o.z + t * u.z;
  return hit;
}

RGB_float phong(Point q, Vector ve, Spheres *sph, Vector *nor, int *index, Point *h, int type) {
//
// do your thing here
//
  Vector v, n, r, l;
  int s;
  RGB_float color;
  Spheres *cur;
  float d;
  Point *hit = new Point;
  if (type == 2)
  {
    cur = intersect_scene(q, ve, sph, hit, 2);
    *h = *hit;
    n = get_vec(*hit, cur->center);
  }
  else if (type == 1)
  {
    cur = intersect_scene(q, ve, sph, hit, 1);
    *h = *hit;
    if (cur == NULL)
    {
      *index = -1;
      if (board)
      {
        Point *h = new Point;
        float *x = new float;
        float *y = new float;
        if (intersect_board(q, ve, x, y, h))
        {
          int x1 = trans(*x);
          int y1 = trans(*y);
          int b_s = 1;
          delete x;
          delete y;
          RGB_float c = ((x1 + y1) % 2)?black:white;
          if (shadow_on == 1 && CheckShadow(h))
          {
            c.r = c.r * board_k + board_ambient * global_ambient[0];
            c.g = c.g * board_k + board_ambient * global_ambient[1];
            c.b = c.b * board_k + board_ambient * global_ambient[2];
          }
          delete hit;
          delete h;
          return c;
        }
      }
      delete hit;
      return background_clr;
    }
    n = get_vec(cur->center, *hit);
  }
  *index = cur->index - 1;
  v = get_vec(*hit, q);
  l = get_vec(*hit, light1);
  r = Reflect(light1, *hit, n);
  d = vec_len(l);
  normalize(&v);
  normalize(&n);
  normalize(&l);
  normalize(&r);
  *nor = n;
  if (shadow_on == 1 && CheckShadow(hit))
    s = 0;
  else
    s = 1;
  color.r = global_ambient[0] * cur->mat_ambient[0] + s * light1_intensity[0] / 
     (decay_a + decay_b * (d) + decay_c * (d) * (d)) * (cur->mat_diffuse[0] * vec_dot(n, l) + 
     cur->mat_specular[0] * pow(vec_dot(r, v), cur->mat_shineness));

  color.g = global_ambient[1] * cur->mat_ambient[1] + s * light1_intensity[1] / 
     (decay_a + decay_b * (d) + decay_c * (d) * (d)) * (cur->mat_diffuse[1] * vec_dot(n, l) + 
     cur->mat_specular[1] * pow(vec_dot(r, v), cur->mat_shineness));

  color.b = global_ambient[2] * cur->mat_ambient[2] + s * light1_intensity[2] / 
     (decay_a + decay_b * (d) + decay_c * (d) * (d)) * (cur->mat_diffuse[2] * vec_dot(n, l) + 
     cur->mat_specular[2] * pow(vec_dot(r, v), cur->mat_shineness)); 
  delete hit;
  return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point q, Vector ve, Spheres *sph, int step, int type) {
//
// do your thing here
//
  Vector *nor = new Vector;
  Vector refl, refr;
  Spheres *cur = sph;
  int *index = new int;
  Point *hit = new Point;
  Point mid, asy;
	RGB_float color, color_refl, color_refr;
  color = phong(q, ve, sph, nor, index, hit, type);
  //color_refl = white;
  //color_refr = white;

  if (step > 0)
  {
    if (type == 1)
    {
      if (*index != -1)
      {
        refl = Reflect(q, *hit, *nor);
        normalize(&refl);
        while (*index) 
        {
          cur = cur->next;
          *index--;
        }
        if (l)
        {
          color_refl = clr_scale(recursive_ray_trace(*hit, refl, sph, step - 1, 1), cur->reflectance);
          color = clr_add(color, color_refl);
        }    
        if (r)
        {
          float eta1 = sky_eta;
          float eta2 = cur->eta;
          int a;
          if (Refract(q, *hit, *nor, eta1, eta2, &refr, &a))
          {
            normalize(&refr);
            color_refr = clr_scale(recursive_ray_trace(*hit, refr, sph, step - 1, 2), cur->refraction);
            //if (color_refr.r > 0.9)
            //cout << color_refr.r <<" " << color_refr.g <<" " <<color_refr.b <<" "<<endl;
            color = clr_add(color, color_refr);
          }
        }
      }
      else
      {
        delete nor;
        delete hit;
        // cout << color.r <<" " << color.g <<" " <<color.b <<" "<<endl;
        return color;
      }
    }
    else if (type == 2)
    {
      refl = Reflect(q, *hit, *nor);
      normalize(&refl);
      if (l)
      {
        color_refl = clr_scale(recursive_ray_trace(*hit, refl, sph, step - 1, 2), cur->reflectance);
        color = clr_add(color, color_refl);
      }
      if (r)
      {
        float eta1 = cur->eta;
        float eta2 = sky_eta;
        int a;
        if (Refract(q, *hit, *nor, eta1, eta2, &refr, &a) == true)
        {
          //cout << Refract(q, *hit, *nor, eta1, eta2, &refr, &a) <<endl;
          color_refr = clr_scale(recursive_ray_trace(*hit, refr, sph, step - 1, 1), cur->refraction);
          color = clr_add(color, color_refr);
        }
      }
    }
  }
  delete nor;
  delete hit;
  //delete index;
  return color;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  RGB_float ret_color;
  Point cur_pixel_pos;
  Vector ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;


  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      ray = get_vec(eye_pos, cur_pixel_pos);

      //
      // You need to change this!!!
      //
      // ret_color = recursive_ray_trace();
      //ret_color = background_clr; // just background for now
      //ret_color = phong(eye_pos, ray, scene);
      ret_color = recursive_ray_trace(eye_pos, ray, scene, step_max, 1);
      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

// Checkboard for testing
RGB_float clr = {float(i/32), 0, float(j/32)};
//ret_color = clr;

      frame[i][j][0] = GLfloat(ret_color.r);
      frame[i][j][1] = GLfloat(ret_color.g);
      frame[i][j][2] = GLfloat(ret_color.b);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
