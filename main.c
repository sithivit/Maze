#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "graphics.h"
#include <math.h>

#define SIZE 40

const int waitTime = 100;
const int squareLength = 60;
const int moveDistance = 8;
int location[2];
int checked_node[25*25][2];
int path[25*25][2];
int to_check_node[25*25][2];
int top_stack = -1;
int popped_node[2];
int direction;
int visited[25*25][2];

/* 0 for going
 * 1 for going right
 * 2 for going down
 * 3 for going left
 */

struct two_D_array 
{
	char map[20][20];
};

struct Neighbour
{
	int array[4][2];
};

void move(struct two_D_array maze, int width, int height);
void drawBackground(struct two_D_array maze,int width, int height);
void update(int x, int y);
struct two_D_array getMap(char *mapName);
void searchElement(int element, struct two_D_array maze);
void DFS(struct two_D_array maze, int width, int height, int * current_position, int * goal_position);
struct Neighbour findNeighbour(struct two_D_array maze,int * node, int width, int height);  
bool findNode(int (*array)[2], int * node);
void insertNode(int (*array)[2], int * node);
void pop();
void push(int * node);
void backtrack(struct two_D_array maze, int width, int height, int * current_position, int * goal_position);

int main(int argc, char *argv[])
{
	struct two_D_array maze = getMap(argv[1]);
	int width = sizeof(maze.map[0])/sizeof(maze.map[0][0]);
	int height =  sizeof(maze)/sizeof(maze.map[0]);
	
	for( int i=0; i<25*25; i++)
	{
		checked_node[i][0] = -1; checked_node[i][1] = -1;
		path[i][0] = -1; path[i][1] = -1;
		to_check_node[i][0] = -1; to_check_node[i][1] = -1;
		visited[i][0] = -1; visited[i][0] = -1;
	}

	setWindowSize(width*squareLength, height*squareLength);
	drawBackground(maze, width, height);
	move(maze, width, height);
	return 0;
}

struct two_D_array getMap(char *mapName)
{
	struct two_D_array maze;
	FILE* ptr;
	char ch;

	ptr = fopen(mapName, "r");
	if( NULL == ptr)
	{
		printf("file can't be opened\n");
		assert(false);
	}
	
	int row= 0;
	int column = 0;
	int ascii = 0;
	do {
		ch = fgetc(ptr);
		ascii = ch;
		if (ascii==32 && column != 20) //path
		{
			strcpy(&maze.map[row][column], " ");
		}
		else if(ascii==35 && column != 20) // Solid wall 
		{
			strcpy(&maze.map[row][column], "#");
		}
		else if(ascii==64 && column != 20) //start position
		{
			strcpy(&maze.map[row][column], "@");
		}
		else if(ascii==63 && column != 20) //final start
		{
			strcpy(&maze.map[row][column], "?");
		}
		else if(ascii==10)
		{
			row++;
			column=-1;
		}
		column++;
		
	}while (ch != EOF);

	fclose(ptr);
	return maze;
}
		


void drawBackground(struct two_D_array maze, int  width, int  height)
{
	background();
	// black for solid and white for path
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			int ascii  = maze.map[i][j];
			if (ascii==35)
			{
				setColour(black);
			}
			else if(ascii==32)
			{
				setColour(white);
			}
			else if(ascii==64)
			{
				setColour(green);
			}
			else if(ascii==63)
			{
				setColour(red);
			}
			else if(ascii==10)
			{
				continue;
			}
			fillRect(j*60, i*60, squareLength, squareLength);
		}
	}
}

void update(int x, int y)
{
	clear();
	fillRect(x*squareLength, y*squareLength, squareLength, squareLength);
}

void move(struct two_D_array maze, int width, int height)
{
	foreground();
	setColour(blue);
	searchElement(64, maze); //find the starting point on the maze
	int y = location[0];
	int x = location[1];
	fillArc(x*squareLength, y*squareLength, squareLength, squareLength, 0, 360);
	int current_position[2]; current_position[0]  = y; current_position[1] = x;
	searchElement(63, maze);
	int goal_position[2]; goal_position[0] = location[0]; goal_position[1] = location[1];
	setColour(green);
	DFS(maze, width, height, current_position, goal_position);
}


void searchElement(int element, struct two_D_array maze)
{
	int width = sizeof(maze.map[0])/sizeof(maze.map[0][0]);
	int height = sizeof(maze)/sizeof(maze.map[0]);
	for(int i=0; i<height; i++)
	{
		for(int j=0; j< width; j++)
		{
			int ascii = maze.map[i][j];
			if(ascii == element)
			{
				location[0] = i;
				location[1] = j;
				return ;
			}
		}
	}
	location[0] = -1;
	location[1] = -1;
}


void DFS(struct two_D_array maze, int width, int height, int * current_position, int * goal_position)
{
	insertNode(checked_node, current_position);		
	
	struct Neighbour neighbour = findNeighbour(maze, current_position, width, height);

	for(int i=0; i<4; i++)
	{
		if (neighbour.array[i][0] == goal_position[0] && neighbour.array[i][1] == goal_position[1])
		{
			insertNode(path, current_position);
			insertNode(path, goal_position);
			update(current_position[1], current_position[0]);
			sleep(waitTime);
			clear();
			update(goal_position[1], goal_position[0]);
			return ;
		}
		if(neighbour.array[i][0] != -1 && findNode(checked_node, neighbour.array[i])==false)
		{
			push(neighbour.array[i]);
		}
	}

	if(findNode(checked_node, to_check_node[top_stack])==false)
	{
		insertNode(path, current_position);
		update(current_position[1], current_position[0]);
                sleep(waitTime);

		int backtrack_current[2];	
		backtrack_current[0] = current_position[0];
		backtrack_current[1] = current_position[1];
		pop();
		if(pow(backtrack_current[0]-popped_node[0],2) >1 || pow(backtrack_current[1] - popped_node[1],2) >1)
		{
			printf("starting backtrack at : %d %d\n",backtrack_current[0], backtrack_current[1] );
			backtrack(maze, width, height, backtrack_current, popped_node);
		}
		
		DFS(maze, width, height, popped_node, goal_position);
		return ;
	}
}
	


struct Neighbour findNeighbour(struct two_D_array maze ,int * node, int width, int height)
{
	/*
	 * up, first index
	 * right, second index
	 * down, third index
	 * left, fourth index
	 */

	struct Neighbour neighbour;
	for(int i=0; i<4; i++)
	{	
		neighbour.array[i][0] = -1; neighbour.array[i][1] = -1;
	}
	int x = node[1]; int y = node[0];
	if(y!=0)
	{
		if((int)maze.map[y-1][x]==32 || (int)maze.map[y-1][x]==63)
		{
			neighbour.array[0][0] = y-1;
			neighbour.array[0][1] = x;
		}
	}
	if(x != width-1)
	{
		if((int)maze.map[y][x+1]==32 || (int)maze.map[y][x+1]==63)
		{
			neighbour.array[1][0] = y;
			neighbour.array[1][1] = x+1;
		}
	}
	if( y!= height-1)
	{ 
		if((int)maze.map[y+1][x]==32 || (int)maze.map[y+1][x]==63)
		{
			neighbour.array[2][0] = y+1;
			neighbour.array[2][1] = x;
		}
	}
	if(x!=0)
	{
		if((int)maze.map[y][x-1]==32 || (int)maze.map[y][x-1]==63)
		{
			neighbour.array[3][0] = y;
			neighbour.array[3][1] = x-1;
		}
	}
	return neighbour;
}	

bool findNode(int (*array)[2], int * node)
{
	for(int i=0; i<25*25;i++)
	{
		if(array[i][0] == node[0] && array[i][1] == node[1])
		{
			return true;
		}
	}
	return false;
}

void insertNode(int (*array)[2], int * node)
{
	bool complete = false;
	int index = 0;
	while(complete == false || index == 25*25)
	{
		if(array[index][0] == -1)
		{
			array[index][0] = node[0];
			array[index][1] = node[1];
			return ;
		}
		index++;
	}
}	


void pop(){
    if(top_stack == -1)
    {
	    popped_node[0] = -1; popped_node[1] = -1;
    }
    else{
       	popped_node[0] = to_check_node[top_stack][0]; popped_node[1] = to_check_node[top_stack][1];
	top_stack -= 1;
    }
}

void push(int * node)
{
	top_stack += 1;
	to_check_node[top_stack][0] = node[0]; 
	to_check_node[top_stack][1] = node[1];
}

// backtrack would go back the whole path if goal_position is not found, would often time be the end of the branches, so to fix this, we can change the goal position to the central of branches and apply DFS from that point to the goal position
void backtrack(struct two_D_array maze, int width, int height, int * current_position, int * goal_position)
{
	int x = goal_position[1]; int y = goal_position[0];
	int index = 0;
	bool the_end = false;
	while(the_end==false)
	{
		printf("%d %d %d\n", index, path[index][0], path[index][1]);
		if(path[index][0] == -1)
		{
			the_end = true;
		}
		index++;
	}
	for(int i=index-2; i>=0; i--)
	{
		struct Neighbour neighbour = findNeighbour(maze, current_position, width, height);
	       for(int j=0; j<4; j++)
	       {
			if(neighbour.array[j][0]== y && neighbour.array[j][1]== x)
			{
				update(goal_position[1], goal_position[0]);
				return;
			}
	       }
	       printf("%d %d\n", current_position[0], current_position[1]);
	       current_position[0] = path[i][0]; current_position[1] = path[i][1];
	       if(findNode(visited, current_position) == false)
	       {
	       		update(current_position[1], current_position[0]);
	       		insertNode(path, current_position);
	       		insertNode(visited, current_position);
	       		sleep(waitTime);
	       }
	}
}

