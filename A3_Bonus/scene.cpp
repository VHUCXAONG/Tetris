//
// this provide functions to set up the scene
//
#include "chess.h"
#include <stdio.h>

extern Point light1;
extern float light1_intensity[3];

extern float global_ambient[3];
extern Chess *scene;

extern RGB_float background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern Point chess1_vertex[154];
extern int chess1_face[304][3];

extern Point chess2_vertex[250];
extern int chess2_face[496][3];

extern char* f1;
extern char* f2;
void Read_smf()
{
  FILE *fp;
  char buf[100], v;
  Point tmp;
  fp = fopen(f1,"r");
  if (fp == NULL)
    cout <<"file open fail"<<endl;
  fgets(buf, 100, fp);
  for (int i = 0; i < 154; i++)
  {
    fscanf(fp, "%c", &v);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%f", &tmp.x);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%f", &tmp.y);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%f", &tmp.z);
    chess1_vertex[i] = tmp;
  }
  fgets(buf, 100, fp);
  for (int i = 0; i < 304; i++)
  {
    fscanf(fp, "%c", &v);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%d", &chess1_face[i][0]);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%d", &chess1_face[i][1]);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%d", &chess1_face[i][2]);
  }
  fclose(fp);

  fp = fopen(f2,"r");
  if (fp == NULL)
    cout <<"file open fail"<<endl;
  fgets(buf, 100, fp);
  for (int i = 0; i < 250; i++)
  {
    fscanf(fp, "%c", &v);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%f", &tmp.x);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%f", &tmp.y);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%f", &tmp.z);
    chess1_vertex[i] = tmp;
  }
  fgets(buf, 100, fp);
  for (int i = 0; i < 496; i++)
  {
    fscanf(fp, "%c", &v);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%d", &chess1_face[i][0]);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%d", &chess1_face[i][1]);
    fscanf(fp, "%c", &v);
    fscanf(fp, "%d", &chess1_face[i][2]);
  }
  fclose(fp);
}

Vector get_nor(Point tmp[3])
{
  Vector re;
  re.x = 1;
  Vector ab = get_vec(tmp[0], tmp[1]);
  Vector ac = get_vec(tmp[0], tmp[2]);
  re.z = (ac.x * ab.y - ab.x * ac.y) / (ab.z * ac.y - ac.z * ab.y);
  re.y = (-ab.x - re.z * ab.z) / ab.y;
  normalize(&re);
  return re;
}

//////////////////////////////////////////////////////////////////////////
void set_up_user_scene() {
  // set background color
  background_clr.r = 0.5;
  background_clr.g = 0.05;
  background_clr.b = 0.8;

  // set up global ambient term
  global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

  // set up light 1
  light1.x = -2.0;
  light1.y = 5.0;
  light1.z = 1.0;
  light1_intensity[0] = light1_intensity[1] = light1_intensity[2] = 1.0;

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  float chess1_ambient[] = {0.7, 0.7, 0.7};
  float chess1_diffuse[] = {0.1, 0.5, 0.8};
  float chess1_specular[] = {1.0, 1.0, 1.0};
  float chess1_shineness = 10;
  float chess1_reflectance = 0.4;
  float chess1_refraction = 0.65;

  float eta = 1.5;

  Point tmp[3];

  Vector nor;
  for (int i = 0; i < 304; i++)
  {
    for (int j = 0; j < 3; j++)
      tmp[j] = chess1_vertex[chess1_face[i][j] - 1];
    nor = get_nor(tmp);
    scene = add_chess(scene, tmp, nor, chess1_ambient, chess1_diffuse, chess1_specular,
      chess1_shineness, chess1_reflectance, chess1_refraction, eta, i+1);
  }

  float chess2_ambient[] = {0.6, 0.6, 0.6};
  float chess2_diffuse[] = {1, 0, 0.25};
  float chess2_specular[] = {1.0, 1.0, 1.0};
  float chess2_shineness = 6;
  float chess2_reflectance = 0.3;
  float chess2_refraction = 0.6;

  for (int i = 0; i < 496; i++)
  {
    for (int j = 0; j < 3; j++)
      tmp[j] = chess2_vertex[chess2_face[i][j] - 1];
    nor = get_nor(tmp);
    scene = add_chess(scene, tmp, nor, chess2_ambient, chess2_diffuse, chess2_specular,
      chess2_shineness, chess2_reflectance, chess2_refraction, eta, i+1+304);
  }
}
