#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.h"
#include <iostream>
#include <time.h>
#include <vector>
using namespace std;
//
// Global variables
//
#define PI 3.14159265

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
extern int f;
extern int p;
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

bool Refract(Point in, Point hit, Vector n, float eta1, float eta2, Vector *t, int *c)
{
  Vector v;
  v = get_vec(hit, in);
  normalize(&v);
  normalize(&n);
  float e = eta1 / eta2;
  float a = 1 - e * e * (1 - vec_dot(n, v) * vec_dot(n, v));
  float x = 1 - vec_dot(n, v) * vec_dot(n, v);
  if (a < 0)
    return false;
  float k = e * vec_dot(n, v) - sqrt(a);
  *t = vec_minus(vec_scale(n, k), vec_scale(v, e));
  return true;
}

int trans(float x)
{
  if (x < 0)
     return (-(int)(abs(x)));
  return (int)x + 1;
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

vector<Vector> Generate_diffuse(int n, Vector nor)
{
  vector<Vector> re;
  // float cos_theta = nor.x / sqrt(nor.x * nor.x + nor.z * nor.z);
  // float sin_theta = nor.z / sqrt(nor.x * nor.x + nor.z * nor.z);
  for (int i = 0; i < n; i++)
  {
    Vector tmp;
    // float arph = 0;
    // float cos_arph = -cos(arph * PI / 180);
    // float sin_arph = sin(arph * PI / 180);
    // Vector tmp = nor;
    // tmp.x = nor.x * cos_theta + nor.z * sin_theta;
    // tmp.y = nor.y;
    // tmp.z = 0;

    // tmp.x = tmp.x * cos_arph - tmp.y * sin_arph;
    // tmp.y = tmp.x * sin_arph + tmp.y * cos_arph;
    // tmp.z = tmp.z;

    // tmp.x = -tmp.x * cos_theta + tmp.z * sin_theta;
    // tmp.y = tmp.y;
    // tmp.z = -tmp.x * sin_theta - tmp.z * cos_theta;

    tmp.x = rand()%10;
    tmp.y = rand()%10;
    tmp.z = rand()%10;
    normalize(&tmp);
    tmp = vec_plus(tmp, nor);
    normalize(&tmp);
    re.push_back(tmp);
  }
  return re;
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
	RGB_float color, color_refl, color_refr;
  color = phong(q, ve, sph, nor, index, hit, type);
  //color_refl = white;
  //color_refr = white; 
  srand(time(NULL));
  if (step > 0)
  {
    if (type == 1)
    {
      if (*index != -1)
      {
        refl = Reflect(q, *hit, *nor);
        normalize(&refl);
        int i = 2 - *index;
        while (i) 
        {
          cur = cur->next;
          i--;
        }
        if (l)
        {
          color_refl = clr_scale(recursive_ray_trace(*hit, refl, sph, step - 1, 1), cur->reflectance);
          color = clr_add(color, color_refl);
        }
        if (f)
        {
          vector<Vector> v;
          v.clear();
          v = Generate_diffuse(diffuse_num, *nor);
          color_refl.r = color_refl.g = color_refl.b = 0;
          for (int i = 0; i < diffuse_num; i++)
          {
            RGB_float tmp_color = recursive_ray_trace(*hit, v[i], sph, step - 1, 1);
            color_refl.r += tmp_color.r * cur->mat_diffuse[0];
            color_refl.g += tmp_color.g * cur->mat_diffuse[1];
            color_refl.b += tmp_color.b * cur->mat_diffuse[2];
          }
          color = clr_add(color, clr_scale(color_refl, 0.2 * 0.2));
        }    
        if (r)
        {
          float eta1 = sky_eta;
          float eta2 = cur->eta;
          int a;
          if (Refract(q, *hit, *nor, eta1, eta2, &refr, &a))
          {
            color_refr = clr_scale(recursive_ray_trace(*hit, refr, sph, step - 1, 2), cur->refraction);
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

void normalize_color(RGB_float *c)
{
  float max = -1;
  if (c->r <= 1 && c->g <= 1 && c->b <= 1)
    return;
  if (c->r > max)
    max = c->r;
  if (c->g > max)
    max = c->g;
  if (c->b > max)
    max = c->b;
  c->r /= max;
  c->g /= max;
  c->b /= max;
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
      if (p)
      {
        float s_x = 0.25 * 0.5 * x_grid_size;
        float s_y = 0.25 * 0.5 * y_grid_size;
        Point p1 = {cur_pixel_pos.x - s_x, cur_pixel_pos.y - s_y, cur_pixel_pos.z};
        Vector ray1 = get_vec(eye_pos, p1);
        ret_color = clr_add(ret_color, recursive_ray_trace(eye_pos, ray1, scene, step_max, 1));

        Point p2 = {cur_pixel_pos.x - s_x, cur_pixel_pos.y + s_y, cur_pixel_pos.z};
        Vector ray2 = get_vec(eye_pos, p2);
        ret_color = clr_add(ret_color, recursive_ray_trace(eye_pos, ray2, scene, step_max, 1));

        Point p3 = {cur_pixel_pos.x + s_x, cur_pixel_pos.y - s_y, cur_pixel_pos.z};
        Vector ray3 = get_vec(eye_pos, p3);
        ret_color = clr_add(ret_color, recursive_ray_trace(eye_pos, ray3, scene, step_max, 1));

        Point p4 = {cur_pixel_pos.x + s_x, cur_pixel_pos.y + s_y, cur_pixel_pos.z};
        Vector ray4 = get_vec(eye_pos, p4);
        ret_color = clr_add(ret_color, recursive_ray_trace(eye_pos, ray4, scene, step_max, 1));
        ret_color = clr_scale(ret_color, 0.2);
      }
      //normalize_color(&ret_color);
      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

// Checkboard for testing
//RGB_float clr = {float(i/32), 0, float(j/32)};
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
