//
// this provide functions to set up the scene
//
#include "chess.h"
#include <stdio.h>
#include <iostream>
using namespace std;
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
  char buf[100];
  char v;
  int c;
  Point tmp;
  fp = fopen("./chess_pieces/chess_piece.smf","r");
  if (fp == NULL)
    cout <<"file open fail"<<endl;
  fgets(buf, 100, fp);
  for (int i = 0; i < 154; i++)
  {
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%f", &tmp.x);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%f", &tmp.y);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%f", &tmp.z);
    fgets(buf, 100, fp);
    chess1_vertex[i] = tmp;
  }
  for (int i = 0; i < 304; i++)
  {
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%d", &chess1_face[i][0]);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%d", &chess1_face[i][1]);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%d", &chess1_face[i][2]);
    fgets(buf, 100, fp);
  }
  fclose(fp);
  //cout <<chess1_face[303][0]<<" "<<chess1_face[303][1]<<" "<<chess1_face[303][2]<<endl;
  fp = fopen("./chess_pieces/bishop.smf","r");
  if (fp == NULL)
    cout <<"file open fail"<<endl;
  fgets(buf, 100, fp);
  for (int i = 0; i < 250; i++)
  {
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%f", &tmp.x);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%f", &tmp.y);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%f", &tmp.z);
    fgets(buf, 100, fp);
    chess2_vertex[i] = tmp;
    //cout <<i<<" "<<tmp.x<<" "<<tmp.y<<" "<<tmp.z<<endl; 
  }
  for (int i = 0; i < 496; i++)
  {
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%d", &chess2_face[i][0]);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%d", &chess2_face[i][1]);
    c = fscanf(fp, "%c", &v);
    c = fscanf(fp, "%d", &chess2_face[i][2]);
    fgets(buf, 100, fp);
  }
  fclose(fp);
}

Vector get_nor(Point tmp[3])
{
  Vector re;
  // re.x = 1;
  // Vector ab = get_vec(tmp[0], tmp[1]);
  // Vector ac = get_vec(tmp[0], tmp[2]);
  Vector ab = get_vec(tmp[0], tmp[1]);
  Vector bc = get_vec(tmp[1], tmp[2]);
  // re.z = (ac.x * ab.y - ab.x * ac.y) / (ab.z * ac.y - ac.z * ab.y);
  // re.y = (-ab.x - re.z * ab.z) / ab.y;
  re.x = (ab.y * bc.z) - (ab.z * bc.y);
  re.y = -(ab.x * bc.z - ab.z * bc.x);
  re.z = ab.x * bc.y - ab.y * bc.x;
  normalize(&re);
  //cout << vec_dot(ab,re)<<" "<<vec_dot(bc,re)<<endl;
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
    {
      //cout << chess1_face[i][j]<<endl;
      tmp[j].x = chess1_vertex[chess1_face[i][j] - 1].x * 6 - 4;
      tmp[j].y = chess1_vertex[chess1_face[i][j] - 1].y * 6 - 3;
      tmp[j].z = chess1_vertex[chess1_face[i][j] - 1].z * 6 - 8;
      //cout <<tmp[j].x<<" "<<tmp[j].y<<" "<<tmp[j].z<<endl;
    }
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
    {
      tmp[j].x = chess2_vertex[chess2_face[i][j] - 1].x * 100 + 2;
      tmp[j].y = chess2_vertex[chess2_face[i][j] - 1].y * 100 - 3;
      tmp[j].z = chess2_vertex[chess2_face[i][j] - 1].z * 100 - 8;
    }
    nor = get_nor(tmp);
    scene = add_chess(scene, tmp, nor, chess2_ambient, chess2_diffuse, chess2_specular,
      chess2_shineness, chess2_reflectance, chess2_refraction, eta, i+1+304);
  }
}
