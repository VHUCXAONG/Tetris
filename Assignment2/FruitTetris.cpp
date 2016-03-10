/*
CMPT 361 Assignment 1 - FruitTetris implementation Sample Skeleton Code

- This is ONLY a skeleton code showing:
How to use multiple buffers to store different objects
An efficient scheme to represent the grids and blocks

- Compile and Run:
Type make in terminal, then type ./FruitTetris

This code is extracted from Connor MacLeod's (crmacleo@sfu.ca) assignment submission
by Rui Ma (ruim@sfu.ca) on 2014-03-04. 

Modified in Sep 2014 by Honghua Li (honghual@sfu.ca).
*/

#include "include/Angel.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <vector>
#include <math.h>
#include <GL/glew.h>
#include <GL/glu.h> 
using namespace std;

int N;
int type;
char text[40] = "Time: ";
char over[40] = "Game Over";
char instructor1[100] = "Use a,s,d,w to control";
char instructor2[100] = "robot arm move";
char instructor3[100] = "on x-y plane";
char instructor4[100] = "Use z, x to control";
char instructor5[100] = "robot arm to";
char instructor6[100] = "rotate by y axis";
char instructor7[100] = "using n, m to make";
char instructor8[100] = "make tile move";
char instructor9[100] = "towards the z-axis";

int count = 0;
bool beginTimer;

int mode; //kinds of the rotating shapes
int mode_type = 6;
int cur_index;//current index in rotation vec
int flag;
bool end = false;

vec3 last_pos[4] = {vec3(-1,-1,-1),vec3(-1,-1,-1),vec3(-1,-1,-1),vec3(-1,-1,-1)};
vec3 last_tile;
// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 720; 
int ysize = 720;

GLfloat R = 1000.0;
GLfloat theta = 0;

GLfloat armtheta = 0;
GLfloat armphi = 0;
GLfloat armbeta = 0;

float armlen = 400;
float armthick = 25;

GLfloat  zNear = 2, zFar = 3000;

GLint model_view;
GLint projection;

int timer = 0;

mat4 mv;
// current tile
vec3 tile[4]; // An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec3 tilepos; // The position of the current tile using grid coordinates ((0,0) is the bottom left corner)

// An array storing all possible orientations of all possible tiles
// The 'tile' array will always be some element [i][j] of this array (an array of vec3)
vec3 allRotationsLshape[24][4] = 
	//I
	{{vec3(0,0,0), vec3(-2,0,0), vec3(1, 0,0), vec3(-1, 0,0)},
	{vec3(0, 0,0), vec3(0,-2,0), vec3(0, 1,0), vec3(0, -1,0)},
	{vec3(0, 0,0), vec3(2, 0,0), vec3(-1,0,0), vec3(1,  0,0)},
	{vec3(0, 0,0), vec3(0, 2,0), vec3(0, -1,0), vec3(0, 1,0)},
	//Right S
	{vec3(0, 0,0), vec3(-1,-1,0), vec3(1, 0,0), vec3(0,-1,0)},
	{vec3(0, 0,0), vec3(1,-1,0), vec3(0, 1,0), vec3(1,  0,0)},
	{vec3(0, 0,0), vec3(1, 1,0), vec3(-1,0,0), vec3(0,  1,0)},
	{vec3(0, 0,0), vec3(-1,1,0), vec3(0,-1,0), vec3(-1, 0,0)},
	//Left L
	{vec3(0, 0,0), vec3(-1,-1,0), vec3(1, 0,0), vec3(-1,0,0)},
	{vec3(0, 0,0), vec3(1,-1,0), vec3(0, 1,0), vec3(0, -1,0)},
	{vec3(0, 0,0), vec3(1, 1,0), vec3(-1,0,0), vec3(1,  0,0)},  
	{vec3(0, 0,0), vec3(-1,1,0), vec3(0,-1,0), vec3(0,  1,0)},
	//Left S
	{vec3(0, 0,0), vec3(0,-1,0), vec3(-1,0,0), vec3(1, -1,0)},
	{vec3(0, 0,0), vec3(1, 0,0), vec3(0,-1,0), vec3(1,  1,0)},
	{vec3(0, 0,0), vec3(0, 1,0), vec3(1, 0,0), vec3(-1, 1,0)},
	{vec3(0, 0,0), vec3(-1,0,0), vec3(0, 1,0), vec3(-1,-1,0)},
	//Right L   
	{vec3(0, 0,0), vec3(1,-1,0), vec3(-1,0,0), vec3(1,  0,0)},
	{vec3(0, 0,0), vec3(1, 1,0), vec3(0,-1,0), vec3(0,  1,0)},
	{vec3(0 ,0,0), vec3(-1,1,0), vec3(1, 0,0), vec3(-1, 0,0)},
	{vec3(0, 0,0), vec3(-1,-1,0), vec3(0, 1,0), vec3(0,-1,0)},
	//T
	{vec3(0, 0,0), vec3(0, 1,0), vec3(-1,0,0), vec3(1,  0,0)},
	{vec3(0, 0,0), vec3(-1,0,0), vec3(0, 1,0), vec3(0, -1,0)},
	{vec3(0, 0,0), vec3(0,-1,0), vec3(-1,0,0), vec3(1,  0,0)},
	{vec3(0, 0,0), vec3(1, 0,0), vec3(0,-1,0), vec3(0,  1,0)}};


// colors
vec4 orange = vec4(1.0, 0.5, 0.0, 1.0); 
vec4 purple = vec4(0.5, 0.0, 1.0, 1.0); 
vec4 red    = vec4(1.0, 0.0, 0.0, 1.0);
vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
vec4 green  = vec4(0.0, 1.0, 0.0, 1.0);
vec4 white  = vec4(1.0, 1.0, 1.0, 1.0);
vec4 black  = vec4(0.0, 0.0, 0.0, 1.0); 
vec4 blue 	= vec4(0.0, 1, 1, 1.0);
vec4 grey   = vec4(0.5, 0.5, 0.5, 1.0);
vec4 color[6] = {orange, purple, red, yellow, green, blue};
//board[x][y] represents whether the cell (x,y) is occupied
vector<bool> board;
int board_x;
int board_y;
int board_z;
//An array containing the colour of each of the 10*20*2*3 vertices that make up the board
//Initially, all will be set to black. As tiles are placed, sets of 6 vertices (2 triangles; 1 square)
//will be set to the appropriate colour in this array before updating the corresponding VBO

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;

// locations of uniform variables in shader program
GLuint locxsize;
GLuint locysize;


vec4 newcolours[24 * 6];
vec4 tmpcolours[24 * 6];


vector<vec4> globalcolours;
vector<vec4> globalposition;

// VAO and VBO
GLuint vaoIDs[3]; // One VAO for each object: the grid, the board, the current piece
GLuint vboIDs[6]; // Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)
GLuint vbo2[6];
GLuint vao2[3];
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void settile();
void newtile();
void Remove(int x, int y);

int isCo[4] = {0, 0, 0, 0};


void CreateCube(vector<vec4> &bp, int i, int j, int k, int type)
{
	vec4 p1, p2, p3, p4, p5, p6, p7, p8;
	if (type)
	{
		int index = k - (N - 3) / 2;
		//cout << index <<endl;
		p1 = vec4(33.0 + (i * 33.0), 33.0 + (j * 33.0), -16.5 + 33.0 * index, 1);
		p2 = vec4(33.0 + (i * 33.0), 66.0 + (j * 33.0), -16.5 + 33.0 * index, 1);
		p3 = vec4(66.0 + (i * 33.0), 33.0 + (j * 33.0), -16.5 + 33.0 * index, 1);
		p4 = vec4(66.0 + (i * 33.0), 66.0 + (j * 33.0), -16.5 + 33.0 * index, 1);

		p5 = vec4(33.0 + (i * 33.0), 33.0 + (j * 33.0), -16.5 + 33.0 * (index - 1), 1);
		p6 = vec4(33.0 + (i * 33.0), 66.0 + (j * 33.0), -16.5 + 33.0 * (index - 1), 1);
		p7 = vec4(66.0 + (i * 33.0), 33.0 + (j * 33.0), -16.5 + 33.0 * (index - 1), 1);
		p8 = vec4(66.0 + (i * 33.0), 66.0 + (j * 33.0), -16.5 + 33.0 * (index - 1), 1);
	}
	else
	{
		int index = k - N / 2 + 1;
		p1 = vec4(33.0 + (i * 33.0), 33.0 + (j * 33.0), 33.0 * index, 1);
		p2 = vec4(33.0 + (i * 33.0), 66.0 + (j * 33.0), 33.0 * index, 1);
		p3 = vec4(66.0 + (i * 33.0), 33.0 + (j * 33.0), 33.0 * index, 1);
		p4 = vec4(66.0 + (i * 33.0), 66.0 + (j * 33.0), 33.0 * index, 1);
		p5 = vec4(33.0 + (i * 33.0), 33.0 + (j * 33.0), 33.0 * (index - 1), 1);
		p6 = vec4(33.0 + (i * 33.0), 66.0 + (j * 33.0), 33.0 * (index - 1), 1);
		p7 = vec4(66.0 + (i * 33.0), 33.0 + (j * 33.0), 33.0 * (index - 1), 1);
		p8 = vec4(66.0 + (i * 33.0), 66.0 + (j * 33.0), 33.0 * (index - 1), 1);
	}

	bp.push_back(p1);
	bp.push_back(p2);
	bp.push_back(p3);
	bp.push_back(p2);
	bp.push_back(p3);
	bp.push_back(p4);

	bp.push_back(p3);
	bp.push_back(p4);
	bp.push_back(p7);
	bp.push_back(p4);
	bp.push_back(p7);
	bp.push_back(p8);

	bp.push_back(p5);
	bp.push_back(p6);
	bp.push_back(p7);
	bp.push_back(p6);
	bp.push_back(p7);
	bp.push_back(p8);

	bp.push_back(p1);
	bp.push_back(p2);
	bp.push_back(p5);
	bp.push_back(p2);
	bp.push_back(p5);
	bp.push_back(p6);

	bp.push_back(p2);
	bp.push_back(p4);
	bp.push_back(p6);
	bp.push_back(p6);
	bp.push_back(p4);
	bp.push_back(p8);

	bp.push_back(p1);
	bp.push_back(p3);
	bp.push_back(p5);
	bp.push_back(p5);
	bp.push_back(p3);
	bp.push_back(p7);
}

void Recover();
int updatetile()
{
	for (int i = 0; i < 4; i++)
		isCo[i] = 0;
	if (end)
		return 0;
	if (beginTimer)
		Recover();
	vector <vec4> tempcolor(36, grey);
	for (int i = 0; i < 4; i++)
	{
		int x = (int)(tilepos.x + tile[i].x);
		int y = (int)(tilepos.y + tile[i].y);
		int z = (int)(tilepos.z + tile[i].z);
		if ((x < 0 || x >9 ||y < 0 || y > 19 || z < 0 || z >= N || board[z * 200 + y * 10 + x] == true))
		{
			if (beginTimer)
			{
				isCo[i] = 1;
				glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
				glBufferSubData(GL_ARRAY_BUFFER, (7200 * z + (x + 10 * y) * 36) * sizeof(vec4), 36 * sizeof(vec4), &tempcolor[0]);
			}
			else
			{
				if ((x < 0 || x >9 ||y < 0 || y > 19 || z < 0 || z >= N || board[z * 200 + y * 10 + x] == true))
				{
					if (last_tile.y != tilepos.y)//drop to the bottom, should create new tile
					{
						tilepos = last_tile;
						settile();
						return 1;
					}
					//can still drop down
					tilepos = last_tile;
					return 2;
				}
			}
		}
	}
	for (int i = 0; i < 36 * 4; i++)
		if (isCo[i / 36])
			newcolours[i] = grey;
		else
			newcolours[i] = tmpcolours[i];
				
	

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]); 
	for (int i = 0; i < 4; i++) 
	{
		vec4 p1, p2, p3, p4, p5, p6, p7, p8;
		// Calculate the grid coordinates of the cell
		GLfloat x = tilepos.x + tile[i].x;
		last_pos[i].x = x;
		GLfloat y = tilepos.y + tile[i].y;
		last_pos[i].y = y;
		GLfloat z = tilepos.z + tile[i].z;
		last_pos[i].z = z;
		//cout <<x <<" "<<y<<" "<<z<<endl; 
		// Create the 4 corners of the square - these vertices are using location in pixels
		// These vertices are later converted by the vertex shader
		vector <vec4> newpoints;
		CreateCube(newpoints,x,y,z,type);
		// Put new data in the VBO
		glBufferSubData(GL_ARRAY_BUFFER, i*36*sizeof(vec4), 36*sizeof(vec4), &newpoints[0]); 
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours);
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	return 0;
}


void newtile()
{
	timer = 0;
	beginTimer = true;
	mode = rand() % mode_type;//get random shape
	cur_index = mode * 4 + rand()%4; 
	//vec3 max = GetTop(cur_index);
	int x = (int)(((- armlen * sin(armtheta * DegreesToRadians) + armlen * cos(armphi * DegreesToRadians)) * cos(armbeta * DegreesToRadians) - 33.0 -120) / 33.0);
	int y = (int)((armlen * cos(armtheta * DegreesToRadians) + armlen * sin(armphi * DegreesToRadians) - 33.0) / 33.0) + 1;
	int z;
	if (armbeta >= 0)
		z = type ? ((N - 1) / 2 - (int)((sin(armbeta * DegreesToRadians) * armlen) / 33.0)) : (N / 2 - 1 - (int)(sin(armbeta * DegreesToRadians) * armlen / 33.0));
	else 
		z = type ? ((N - 1) / 2 + (int)((sin(-armbeta * DegreesToRadians) * armlen) / 33.0)) : (N / 2 + (int)(sin(-armbeta * DegreesToRadians) * armlen / 33.0));
	tilepos = vec3(x, y, z);
	last_tile = tilepos;
	flag = 0;
	// Update the geometry VBO of current tile
	for (int i = 0; i < 4; i++)
		tile[i] = allRotationsLshape[cur_index][i]; // Get the 4 pieces of the new tile

	for (int i = 0; i < 4; i++)
	{
		int c = rand() % 5;
			for (int j = i * 36; j < (i + 1) * 36; j++)
				tmpcolours[j] = color[c];
	}
	if (!end)
		updatetile();

}
//-------------------------------------------------------------------------------------------------------------------


void initGrid()
{
	// ***Generate geometry data
	vector<vec4> gridpoints; // Array containing the 64 points of the 32 total lines to be later put in the VBO
	vector<vec4> gridcolours; // One colour per vertex
	// Vertical lines 
	if (type)
	{
		for (int i = 0; i < 11; i++)
		{
			for (int k = 0; k < (N + 1) / 2; k++)
			{
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 33.0, 16.5 + 33.0 * k, 1));
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 693.0, 16.5 + 33.0 * k, 1));
				
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 33.0, -16.5 - 33.0 * k, 1));
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 693.0, -16.5 - 33.0 * k, 1));
			}
			for (int j = 0; j < 21; j++)
			{
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 33.0 + (33.0 * j), 16.5 + 33.0 * (N - 1) / 2, 1));
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 33.0 + (33.0 * j), -16.5 - 33.0 * (N - 1) / 2, 1));
			}
		}
		// Horizontal lines
		for (int i = 0; i < 21; i++)
		{
			for (int k = 0; k < (N + 1) / 2; k++)
			{
				gridpoints.push_back(vec4(33.0, (33.0 + (33.0 * i)), 16.5 + 33.0 * k, 1));
				gridpoints.push_back(vec4(363.0, (33.0 + (33.0 * i)), 16.5 + 33.0 * k, 1));

				gridpoints.push_back(vec4(33.0, (33.0 + (33.0 * i)), -16.5 - 33.0 * k, 1));
				gridpoints.push_back(vec4(363.0, (33.0 + (33.0 * i)), -16.5 - 33.0 * k, 1));
			}
		}
	}
	else
	{
		for (int i = 0; i < 11; i++)
		{
			gridpoints.push_back(vec4((33.0 + (33.0 * i)), 33.0,  0, 1));
			gridpoints.push_back(vec4((33.0 + (33.0 * i)), 693.0, 0, 1));
			for (int k = 1; k <= N / 2; k++)
			{
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 33.0,  33.0 * k, 1));
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 693.0, 33.0 * k, 1));
				
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 33.0, -33.0 * k, 1));
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 693.0, -33.0 * k, 1));
			}
			for (int j = 0; j < 21; j++)
			{
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 33.0 + (33.0 * j), 33.0 * N / 2, 1));
				gridpoints.push_back(vec4((33.0 + (33.0 * i)), 33.0 + (33.0 * j), -33.0 * N / 2, 1));
			}
		}
		// Horizontal lines
		for (int i = 0; i < 21; i++)
		{
			gridpoints.push_back(vec4(33.0, (33.0 + (33.0 * i)), 0, 1));
			gridpoints.push_back(vec4(363.0, (33.0 + (33.0 * i)), 0, 1));
			for (int k = 1; k <= N / 2; k++)
			{
				gridpoints.push_back(vec4(33.0, (33.0 + (33.0 * i)), 33.0 * k, 1));
				gridpoints.push_back(vec4(363.0, (33.0 + (33.0 * i)), 33.0 * k, 1));

				gridpoints.push_back(vec4(33.0, (33.0 + (33.0 * i)), -33.0 * k, 1));
				gridpoints.push_back(vec4(363.0, (33.0 + (33.0 * i)), -33.0 * k, 1));
			}
		}
	}

	int size = gridpoints.size();
	// Make all grid lines white
	for (int i = 0; i < size; i++)
		gridcolours.push_back(white);


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[0]); // Bind the first VAO
	glGenBuffers(2, vboIDs); // Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]); // Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(vec4), &gridpoints[0], GL_STATIC_DRAW); // Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPosition); // Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]); // Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(vec4), &gridcolours[0], GL_STATIC_DRAW); // Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor); // Enable the attribute
}

void initBoard()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, N*7200*sizeof(vec4), NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, N*7200*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

	vector<vec4> boardpoints;
	// Each cell is a square (2 triangles with 6 vertices)
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			for (int k = 0; k < N; k++)
				CreateCube(boardpoints, i, j, k, type);

	// Initially no cell is occupied
	board.clear();
	for (int i = 0; i < 200 * N; i++)
		board.push_back(false);

	int size = boardpoints.size();
	// *** set up buffer objects
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(vec4), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}
void OriginalCube(int i)
{
	vec4 cubeColor[36];
	vec4 p1 = vec4 (0, 0, 0.5, 1);
	vec4 p2 = vec4 (0, 1, 0.5, 1);
	vec4 p3 = vec4 (1, 0, 0.5, 1);
	vec4 p4 = vec4 (1, 1, 0.5, 1);
	vec4 p5 = vec4 (0, 0, -0.5, 1);
	vec4 p6 = vec4 (0, 1, -0.5, 1);
	vec4 p7 = vec4 (1, 0, -0.5, 1);
	vec4 p8 = vec4 (1, 1, -0.5, 1);
	vec4 cubePosition[36] = {p1, p2, p3, p2, p3, p4, p2, p6, p4, p6, p4, p8, p5, p6, p7, p6, p7, p8, p3, p4, p7, p4, p7, p8, p1, p2, p5, p2, p5, p6, p1, p5, p3, p5, p3, p7}; 
	for (int j = 0; j < 36; j++)
	{
		cubeColor[j] = blue;
	}

	glBindVertexArray(vao2[i]);
	glGenBuffers(2, &vbo2[2 * i]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2 * i]);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(vec4), cubePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2 * i + 1]);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(vec4), cubeColor, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}
void Camera()
{
	vec4 eye;

	eye[0] = R * cos(theta * DegreesToRadians) + 33 * 6;
	eye[1] = 693;
	eye[2] = R * sin(theta * DegreesToRadians);

    vec4  at( (33 + 363) / 2, (33 + 693) / 2, 0.0, 1.0 );
    vec4  up( -R * cos(theta * DegreesToRadians), R * R / 330 - 330, -R * sin(theta * DegreesToRadians), 1.0 );	
    //vec4 up(0,1,0,0);
    mv = LookAt(eye, at, up);

    GLdouble ratio = ysize / xsize;
    mat4 p = Perspective(50, 1, zNear, zFar);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p);
    glBindVertexArray(vaoIDs[1]);

    //glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}


// No geometry for current tile Initially
void initCurrentTile()
{
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

void SetText(char *t,int x, int y)
{
	glDisable(GL_TEXTURE_2D); 
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, xsize, 0.0, ysize);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	void * font = GLUT_BITMAP_9_BY_15;

	int i = 0;
	while (t[i])
	{
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font, t[i]);
		i++;
	}
	glMatrixMode(GL_MODELVIEW); 
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	srand(time(0));
	for (int i = 0; i < 4; i++)
		last_pos[i] = vec3(-1,-1,-1);

	globalposition = vector<vec4>(7200 * N);
	globalcolours = vector<vec4>(200 * N);
	globalposition.clear();
	globalcolours.clear();
	// for (int i = 0; i < 7200; i++)
	// 	globolposition[i] = ;
	// Load shaders and use the shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Get the location of the attributes (for glVertexAttribPointer() calls)
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");

	// Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
	glGenVertexArrays(4, &vaoIDs[0]);
	initGrid();
	initBoard();
	initCurrentTile();
	OriginalCube(0);
	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");
	model_view = glGetUniformLocation(program, "ModelView");
    projection = glGetUniformLocation(program, "Projection");
	// Game initialization
	newtile(); // create new next tile

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------

// Check if it's able to rotate
bool CheckRotate()
{
	int next = cur_index + 1;
	vec3 next_tile[4];
	if (next >= (mode + 1) * 4 )
		next = mode * 4;
	for (int i = 0; i < 4; i++)
	{
		next_tile[i] = allRotationsLshape[next][i];
	}
	for (int i = 0; i < 4; i++)
	{
		int x = (int) (tilepos.x + next_tile[i].x);
		int y = (int) (tilepos.y + next_tile[i].y);
		int z = (int) (tilepos.z + next_tile[i].z);
		if ((x < 0 || x >9 ||y < 0 || y > 19 || z < 0 || z >= N || board[z * 200 + y * 10 + x] == true))
		{
			return false;
		}
	}
	return true;
}

// Rotates the current tile, if there is room
void rotate()
{
	if (!CheckRotate())
		return;
	cur_index++;
	if (cur_index >= (mode + 1) * 4 )
		cur_index = mode * 4;
	for (int i = 0; i < 4; i++)
		tile[i] = allRotationsLshape[cur_index][i];
	updatetile();
	last_tile = tilepos;
}

void Recover()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, N*7200*sizeof(vec4), NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, N*7200*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
		{
			for (int k = 0; k < N; k++)
			{
				if (board[k * 200 + j * 10 + i])
				{
					vector<vec4> ncolour(36, globalcolours[k * 200 + j * 10 + i]);
					glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
					glBufferSubData(GL_ARRAY_BUFFER, (7200 * k + (i + 10 * j) * 36) * sizeof(vec4), 36 * sizeof(vec4), &ncolour[0]);
					glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
					glBufferSubData(GL_ARRAY_BUFFER, (7200 * k + (i + 10 * j) * 36) * sizeof(vec4), 36 * sizeof(vec4), &globalposition[k * 7200 + (i + 10 * j) * 36]);
				}
			}
		}	
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
}

void settile()
{
	for (int i = 0; i < 4; i++) // set boardcolor as shape's color
	{
		vector <vec4> tmp;
		int x = (int) (tilepos.x + tile[i].x);
		int y = (int) (tilepos.y + tile[i].y);
		int z = (int) (tilepos.z + tile[i].z);
		board[z * 200 + y * 10 + x] = true;
		globalcolours[z * 200 + y * 10 + x] = newcolours[i * 36];
		CreateCube(tmp,x,y,z,type);
		for (int j = 0; j < 36; j++)
			globalposition[z * 7200 + (x + 10 * y) * 36 + j] = tmp[j];
	}

	Recover();
	newtile();
}

//-------------------------------------------------------------------------------------------------------------------

// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
bool movetile(vec2 direction)
{
	return false;
}
//-------------------------------------------------------------------------------------------------------------------
// Starts the game over - empties the board, creates new tiles, resets line counters
void restart()
{
	end = false;
	init();
}
//-------------------------------------------------------------------------------------------------------------------

void TimeControl()
{
	if (beginTimer)
	{
		timer++;
		text[6] = (9 - timer / 100) + '0';
		if (timer > 900)
		{
			timer = 0;
			beginTimer = false;
		}
	}
	else
	{
		if (isCo[0] + isCo[1] +isCo[2] +isCo[3])
		{
			end = true;
			return;
		}
		text[6] = '0';
		count ++;
		if (count > 50)
		{
			count = 0;
			tilepos.y --;
			updatetile();
			last_tile = tilepos;

		}
	}
}

// Draws the game
void display()
{
	TimeControl();
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	Camera();

	// glUniform1i(locxsize, xsize); // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
	// glUniform1i(locysize, ysize);
	int lineNumber = (type) ? (64 * (N + 1) + 462) : (64 * N + 528);

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
	glDrawArrays(GL_TRIANGLES, 0, 200 * N * 360); // Draw the board (10*20*2 = 400 triangles)

	glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 24 * 6); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, lineNumber); // Draws the grid lines (21+11 = 32 lines)



	mat4 Modelv;
    Modelv = Translate( -160 , 33, 0 );
    mat4 M = Modelv * Scale( 120, 40, 120);
    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv * M );
    glBindVertexArray(vao2[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36); 

	Modelv = Translate( -120, 33, 0);
	//mat4 M3 = Modelv * RotateY(armbeta);
	mat4 M1 = Modelv * RotateZ(armtheta);
	M = M1 * Scale(armthick, armlen, armthick);
	glUniformMatrix4fv( model_view, 1, GL_TRUE, mv * M );
	glBindVertexArray(vao2[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); 

	float x = -120 - armlen * sin(armtheta * DegreesToRadians);
	float y = armlen * cos(armtheta * DegreesToRadians) + 33;
	Modelv = Translate( x, y, 0);
	mat4 M2 = Modelv * RotateY(armbeta);
	M1 = M2 * RotateZ(armphi);
	M = M1 * Scale(armlen, armthick, armthick);
	glUniformMatrix4fv( model_view, 1, GL_TRUE, mv * M );
	glBindVertexArray(vao2[2]);
	glDrawArrays(GL_TRIANGLES, 0, 36); 

	if (!end)
		SetText(text,525,600);
	else
		SetText(over,525,600);
	SetText(instructor1,525,550);
	SetText(instructor2,525,525);
	SetText(instructor3,525,500);
	SetText(instructor4,525,450);
	SetText(instructor5,525,425);
	SetText(instructor6,525,400);
	SetText(instructor7,525,350);
	SetText(instructor8,525,325);
	SetText(instructor9,525,300);
	glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------------------------------

// Reshape callback will simply change xsize and ysize variables, which are passed to the vertex shader
// to keep the game the same from stretching if the window is stretched
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

//-------------------------------------------------------------------------------------------------------------------

// Handle arrow key keypresses
void special(int key, int x, int y)
{

	switch(key) 
	{
		case GLUT_KEY_UP:
			rotate();
			last_tile = tilepos;
			break;
		case GLUT_KEY_LEFT:
			if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
				theta -= 5;
			else if (!beginTimer)
			{
				tilepos.x --;
				updatetile();
				last_tile = tilepos;
			}
			break;
		case GLUT_KEY_DOWN:
			if (!beginTimer)
			{
				tilepos.y --;
				updatetile();
				last_tile = tilepos;
			}
			break;
		case GLUT_KEY_RIGHT:
			if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
					theta += 5;
			else if (!beginTimer)
			{
				tilepos.x ++;
				updatetile();
				last_tile = tilepos;
			}
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------

void updateArm()
{
	if (beginTimer)
	{
		tilepos.x = (int)(((- armlen * sin(armtheta * DegreesToRadians) + armlen * cos(armphi * DegreesToRadians)) * cos(armbeta * DegreesToRadians)- 33.0 - 120 ) / 33.0);
		tilepos.y = (int)((armlen * cos(armtheta * DegreesToRadians) + armlen * sin(armphi * DegreesToRadians) - 33.0) / 33.0) + 1;
		int z;
		if (armbeta >= 0)
			z = type ? ((N - 1) / 2 - (int)((sin(armbeta * DegreesToRadians) * armlen) / 33.0)) : (N / 2 - 1 - (int)(sin(armbeta * DegreesToRadians) * armlen / 33.0));
		else 
			z = type ? ((N - 1) / 2 + (int)((sin(-armbeta * DegreesToRadians) * armlen) / 33.0)) : (N / 2 + (int)(sin(-armbeta * DegreesToRadians) * armlen / 33.0));
		tilepos.z = z;
		updatetile();
		last_tile = tilepos;
	}
}

// Handles standard keypresses
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case 033: // Both escape key and 'q' cause the game to exit
		    exit(EXIT_SUCCESS);
		    break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		case 'r': // 'r' key restarts the game
			restart();
			break;
		case 'w':
			armphi += 2;
			updateArm();
			break;
		case 's':
			armphi -= 2;
			updateArm();
			break;
		case 'a':
			armtheta -= 2;
			updateArm();
			break;
		case 'd':
			armtheta += 2;
			updateArm();
			break;
		case 'z':
			armbeta += 2;
			updateArm();
			break;
		case 'x':
			armbeta -= 2;
			updateArm();
			break;
		case ' ':
			beginTimer = false;
			break;
		case 'm':
			if (!beginTimer)
			{
				tilepos.z ++;
				updatetile();
				last_tile = tilepos;
			}
			break;
		case 'n':
			if (!beginTimer)
			{
				tilepos.z --;
				updatetile();
				last_tile = tilepos;
			}
			break;
	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

void idle(void)
{
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		cout << "wrong command"<<endl;
		return 0;
	}
	N = atoi(argv[1]);
	type = (N % 2) ? 1 : 0;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(680, 178); // Center the game window (well, on a 1920x1080 display)
	glutCreateWindow("Fruit Tetris");
	glewInit();
	init();


	// Callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop(); // Start main loop
	return 0;
}
