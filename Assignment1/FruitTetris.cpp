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
using namespace std;

int count = 0;
int mode; //kinds of the rotating shapes
int mode_type = 6;
int cur_index;//current index in rotation vec
int flag;
bool end = false;

vec2 last_pos[4] = {vec2(-1,-1),vec2(-1,-1),vec2(-1,-1),vec2(-1,-1)};
vec2 last_tile;
// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 400; 
int ysize = 720;

// current tile
vec2 tile[4]; // An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec2 tilepos = vec2(5, 19); // The position of the current tile using grid coordinates ((0,0) is the bottom left corner)

// An array storing all possible orientations of all possible tiles
// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
vec2 allRotationsLshape[24][4] = 
	//I
	{{vec2(0,0), vec2(-2,0), vec2(1, 0), vec2(-1, 0)},
	{vec2(0, 0), vec2(0,-2), vec2(0, 1), vec2(0, -1)},
	{vec2(0, 0), vec2(2, 0), vec2(-1,0), vec2(1,  0)},
	{vec2(0, 0), vec2(0, 2), vec2(0, -1), vec2(0, 1)},
	//Right S
	{vec2(0, 0), vec2(-1,-1), vec2(1, 0), vec2(0,-1)},
	{vec2(0, 0), vec2(1,-1), vec2(0, 1), vec2(1,  0)},
	{vec2(0, 0), vec2(1, 1), vec2(-1,0), vec2(0,  1)},
	{vec2(0, 0), vec2(-1,1), vec2(0,-1), vec2(-1, 0)},
	//Left L
	{vec2(0, 0), vec2(-1,-1), vec2(1, 0), vec2(-1,0)},
	{vec2(0, 0), vec2(1,-1), vec2(0, 1), vec2(0, -1)},
	{vec2(0, 0), vec2(1, 1), vec2(-1,0), vec2(1,  0)},  
	{vec2(0, 0), vec2(-1,1), vec2(0,-1), vec2(0,  1)},
	//Left S
	{vec2(0, 0), vec2(0,-1), vec2(-1,0), vec2(1, -1)},
	{vec2(0, 0), vec2(1, 0), vec2(0,-1), vec2(1,  1)},
	{vec2(0, 0), vec2(0, 1), vec2(1, 0), vec2(-1, 1)},
	{vec2(0, 0), vec2(-1,0), vec2(0, 1), vec2(-1,-1)},
	//Right L   
	{vec2(0, 0), vec2(1,-1), vec2(-1,0), vec2(1,  0)},
	{vec2(0, 0), vec2(1, 1), vec2(0,-1), vec2(0,  1)},
	{vec2(0 ,0), vec2(-1,1), vec2(1, 0), vec2(-1, 0)},
	{vec2(0, 0), vec2(-1,-1), vec2(0, 1), vec2(0,-1)},
	//T
	{vec2(0, 0), vec2(0, 1), vec2(-1,0), vec2(1,  0)},
	{vec2(0, 0), vec2(-1,0), vec2(0, 1), vec2(0, -1)},
	{vec2(0, 0), vec2(0,-1), vec2(-1,0), vec2(1,  0)},
	{vec2(0, 0), vec2(1, 0), vec2(0,-1), vec2(0,  1)}};


// colors
vec4 orange = vec4(1.0, 0.5, 0.0, 1.0); 
vec4 purple = vec4(0.5, 0.0, 1.0, 1.0); 
vec4 red    = vec4(1.0, 0.0, 0.0, 1.0);
vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
vec4 green  = vec4(0.0, 1.0, 0.0, 1.0);
vec4 white  = vec4(1.0, 1.0, 1.0, 1.0);
vec4 black  = vec4(0.0, 0.0, 0.0, 1.0); 
vec4 color[5] = {orange, purple, red, yellow, green};
//board[x][y] represents whether the cell (x,y) is occupied
bool board[10][20]; 

//An array containing the colour of each of the 10*20*2*3 vertices that make up the board
//Initially, all will be set to black. As tiles are placed, sets of 6 vertices (2 triangles; 1 square)
//will be set to the appropriate colour in this array before updating the corresponding VBO
vec4 boardcolours[1200];

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;

// locations of uniform variables in shader program
GLuint locxsize;
GLuint locysize;

// VAO and VBO
GLuint vaoIDs[3]; // One VAO for each object: the grid, the board, the current piece
GLuint vboIDs[6]; // Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void settile();
void newtile();
void Remove(int x, int y);
void DrawLast()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	for (int i = 0; i < 4; i++) 
	{

		// Calculate the grid coordinates of the cell
		GLfloat x = tilepos.x + tile[i].x;
		last_pos[i].x = x;
		GLfloat y = tilepos.y + tile[i].y;
		last_pos[i].y = y;
		board[(int)x][(int)y] = true;
		// Create the 4 corners of the square - these vertices are using location in pixels
		// These vertices are later converted by the vertex shader
		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1); 
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

		// Two points are used by two triangles each
		vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4}; 

		// Put new data in the VBO
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), newpoints); 
	}

	glBindVertexArray(0);
	
}

int updatetile()
{
	if (end)
		return 0;
	// Bind the VBO containing current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]); 
	if (flag)
		for (int i = 0; i < 4; i++)
			board[(int)last_pos[i].x][(int)last_pos[i].y] = false;
	for (int i = 0; i < 4; i++)
	{
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;
		if (board[(int)x][(int)y] == true || x < 0 || x >9 ||y < 0)//collision happens
		{
			flag = 1;
			for (int j = 0; j < 4; j++)
			{
				board[(int)last_pos[j].x][(int)last_pos[j].y] = true;
			}
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
	flag = 1;
	// For each of the 4 'cells' of the tile,
	for (int i = 0; i < 4; i++) 
	{

		// Calculate the grid coordinates of the cell
		GLfloat x = tilepos.x + tile[i].x;
		last_pos[i].x = x;
		GLfloat y = tilepos.y + tile[i].y;
		last_pos[i].y = y;
		board[(int)x][(int)y] = true;
		// Create the 4 corners of the square - these vertices are using location in pixels
		// These vertices are later converted by the vertex shader
		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1); 
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

		// Two points are used by two triangles each
		vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4}; 

		// Put new data in the VBO
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), newpoints); 
	}

	glBindVertexArray(0);
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
vec3 GetTop(int index)
{
	vec3 re = vec3(0, 0, 0);
	for (int i = 0; i < 4; i++)
	{
		if (allRotationsLshape[index][i].y > re.x)
			re.x = allRotationsLshape[index][i].y;
		if (allRotationsLshape[index][i].x < re.y)
			re.y = allRotationsLshape[index][i].x;
		if (allRotationsLshape[index][i].x > re.z)
			re.z = allRotationsLshape[index][i].x;
	}
	return re;
}

// Called at the start of play and every time a tile is placed
vec4 newcolours[24];
void newtile()
{
	mode = rand() % mode_type;//get random shape
	cur_index = mode * 4 + rand()%4; 
	vec3 max = GetTop(cur_index);
	tilepos = vec2(rand() % 5 + 2, 19 - max.x);
	last_tile = tilepos;
	flag = 0;
	// Update the geometry VBO of current tile
	for (int i = 0; i < 4; i++)
		tile[i] = allRotationsLshape[cur_index][i]; // Get the 4 pieces of the new tile
	for (int i = 0; i < 4; i++)
	{
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;
		if (board[(int)x][(int)y] == true)
			end = true;
	}
	if (!end)
		updatetile();
	else
		DrawLast();
		// Update the color VBO of current tile
		for (int i = 0; i < 4; i++)
		{
			int c = rand() % 5;
			for (int j = i * 6; j < (i + 1) * 6; j++)
				newcolours[j] = color[c]; 
		}
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

void initGrid()
{
	// ***Generate geometry data
	vec4 gridpoints[64]; // Array containing the 64 points of the 32 total lines to be later put in the VBO
	vec4 gridcolours[64]; // One colour per vertex
	// Vertical lines 
	for (int i = 0; i < 11; i++){
		gridpoints[2*i] = vec4((33.0 + (33.0 * i)), 33.0, 0, 1);
		gridpoints[2*i + 1] = vec4((33.0 + (33.0 * i)), 693.0, 0, 1);
		
	}
	// Horizontal lines
	for (int i = 0; i < 21; i++){
		gridpoints[22 + 2*i] = vec4(33.0, (33.0 + (33.0 * i)), 0, 1);
		gridpoints[22 + 2*i + 1] = vec4(363.0, (33.0 + (33.0 * i)), 0, 1);
	}
	// Make all grid lines white
	for (int i = 0; i < 64; i++)
		gridcolours[i] = white;


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[0]); // Bind the first VAO
	glGenBuffers(2, vboIDs); // Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]); // Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridpoints, GL_STATIC_DRAW); // Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPosition); // Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]); // Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridcolours, GL_STATIC_DRAW); // Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor); // Enable the attribute
}


void initBoard()
{
	// *** Generate the geometric data
	vec4 boardpoints[1200];
	for (int i = 0; i < 1200; i++)
		boardcolours[i] = black; // Let the empty cells on the board be black
	// Each cell is a square (2 triangles with 6 vertices)
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 10; j++)
		{		
			vec4 p1 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p2 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			vec4 p3 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p4 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			
			// Two points are reused
			boardpoints[6*(10*i + j)    ] = p1;
			boardpoints[6*(10*i + j) + 1] = p2;
			boardpoints[6*(10*i + j) + 2] = p3;
			boardpoints[6*(10*i + j) + 3] = p2;
			boardpoints[6*(10*i + j) + 4] = p3;
			boardpoints[6*(10*i + j) + 5] = p4;
		}
	}

	// Initially no cell is occupied
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = false; 


	// *** set up buffer objects
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

// No geometry for current tile initially
void initCurrentTile()
{
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

void init()
{
	srand(time(0));
	// Load shaders and use the shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Get the location of the attributes (for glVertexAttribPointer() calls)
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");

	// Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
	glGenVertexArrays(3, &vaoIDs[0]);

	// Initialize the grid, the board, and the current tile
	initGrid();
	initBoard();
	initCurrentTile();

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");

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
	vec2 next_tile[4];
	if (next >= (mode + 1) * 4 )
		next = mode * 4;
	for (int i = 0; i < 4; i++)
	{
		board[(int)last_pos[i].x][(int)last_pos[i].y] = false;
		next_tile[i] = allRotationsLshape[next][i];
	}
	for (int i = 0; i < 4; i++)
	{
		GLfloat x = tilepos.x + next_tile[i].x;
		GLfloat y = tilepos.y + next_tile[i].y;
		if (board[(int)x][(int)y] == true || x < 0 || x > 9 ||y < 0 || y > 19)//collision happens
		{
			for (int j = 0; j < 4; j++)
				board[(int)last_pos[i].x][(int)last_pos[i].y] = true;
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

//-------------------------------------------------------------------------------------------------------------------

// Checks if the specified row (0 is the bottom 19 the top) is full
// If every cell in the row is occupied, it will clear that cell and everything above it will shift down one row
void checkfullrow(int row)
{
	for (int i = 0; i < 10; i++)
		if (board[i][row] == false)
			return;
	for (int i = 0; i < 10; i++)
		Remove(i, row);
}


bool CheckSameColor(int x1, int y1, int x2, int y2)
{	
	if (x2 < 0 || x2 > 9 || y2 < 0 || y2 > 19)
		return false;
	int x = x1 * 6 + y1 * 60;
	int y = x2 * 6 + y2 * 60;
	if (board[x1][y1] && board[x2][y2]&&
		(abs(boardcolours[x].x - boardcolours[y].x) < 0.0001) && (abs(boardcolours[x].y - boardcolours[y].y) < 0.0001)&&
		(abs(boardcolours[x].z - boardcolours[y].z) < 0.0001) && (abs(boardcolours[x].w - boardcolours[y].w) < 0.0001))
		return true;
	return false;
}

void RemoveCol(int x, int y)
{
	for (int i = y; i < 17; i++)
	{
		for (int j = 0;j < 6; j++)
			boardcolours[x * 6 + i * 60 + j] = boardcolours[x * 6 + i * 60 + 180];
		board[x][i] = board[x][i + 3];
	}
	board[x][19] = false;
	board[x][18] = false;
	board[x][17] = false;
}
void Remove(int x, int y)
{	
	for (int i = y; i < 19; i++)
	{
		for (int j = 0;j < 6; j++)
			boardcolours[x * 6 + i * 60 + j] = boardcolours[x * 6 + i * 60 + 60];
		board[x][i] = board[x][i + 1];
	}
	board[x][19] = false;
}
//-------------------------------------------------------------------------------------------------------------------
void CheckRemove()
{
	for (int i = 19; i >= 0; i--)
		checkfullrow(i);

	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
		{
			if (CheckSameColor(i, j, i - 1, j) && CheckSameColor(i, j, i + 1, j))
			{
				//cout << 1 <<endl;
				Remove(i, j);
				Remove(i - 1, j);
				Remove(i + 1, j);
				CheckRemove();
				return;
			}
			if (CheckSameColor(i, j, i, j - 1) && CheckSameColor(i, j, i, j + 1))
			{
				//cout << 2 <<endl;
				RemoveCol(i, j - 1);
				CheckRemove();
				return;
			}
			if (CheckSameColor(i, j, i - 1, j + 1) && CheckSameColor(i, j, i + 1, j - 1))
			{
				//cout << 3 <<endl;
				Remove(i, j);
				Remove(i - 1, j + 1);
				Remove(i + 1, j - 1);
				CheckRemove();
				return;
			}
			if (CheckSameColor(i, j, i + 1, j + 1) && CheckSameColor(i, j, i - 1, j - 1))
			{
				//cout << 4 <<endl;
				Remove(i, j);
				Remove(i + 1, j + 1);
				Remove(i - 1, j - 1);
				CheckRemove();
				return;
			}

		}
}
// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void settile()
{
	for (int i = 0; i < 4; i++) // set boardcolor as shape's color
	{
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;
		int index = (x + y * 10) * 6;
		for (int j = index; j < index + 6; j++)
			boardcolours[j] = newcolours[i * 6];
	}
	CheckRemove();
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]); // Bind the VBO containing current tile vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(boardcolours), boardcolours); // Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
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

// Draws the game
void display()
{
	count ++;
	if (count > 50)
	{
		count = 0;
		tilepos.y --;
		updatetile();
		last_tile = tilepos;

	}
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize); // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
	glUniform1i(locysize, ysize);

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
	glDrawArrays(GL_TRIANGLES, 0, 1200); // Draw the board (10*20*2 = 400 triangles)

	glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 24); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, 64); // Draw the grid lines (21+11 = 32 lines)


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
			tilepos.x --;
			updatetile();
			last_tile = tilepos;
			break;
		case GLUT_KEY_DOWN:
			tilepos.y --;
			updatetile();
			last_tile = tilepos;
			break;
		case GLUT_KEY_RIGHT:
			tilepos.x ++;
			updatetile();
			last_tile = tilepos;
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------

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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
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
