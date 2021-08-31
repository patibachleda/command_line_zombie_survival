/*

  Author: Patrycja Bachleda
  Email: pbachleda2019@my.fit.edu	
  Course: CSE2010 Data Structures and Algorithms
  Section: 2

  Description of this file: The first part of a zombie simulation"
  	0 is Player 0
  	* is the destination
  	A,B,C... are the creatures

  This file covers the first round of the game where the player selects a direction (up, down
  left, right) and then each creature will use BFS to display a path to the player. This version
  is the full game (addition to hw6.c) with multiple rounds so monsters move according to the shortest
  path that was found. The path is calculated to move through other monsters and obstacles other than #
  since it was not specified in the directions.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int visited[100];//keeps track of the visited nodes for BFS
int parent[100];//each index/data corosponds to child/parent
int dz[100]; //sum of each distance from creature
int tree[100];//the binary tree that BFS creates
	int curr_index;//track of the tree index
int path[50];//the shortest path found by BFS (50 moves max)
	int path_index = 0;//keeps track of number of steps in path
int player_path[50];
	int player_path_index = 0;
int mon_title[26];//an array of the monster letters for abc order purposes (26 monsters max)
					//26 monsters because 26 letters in alphabet

int curr_loc[2];//keeps the players current location
int mon_location[26][2]; //30 monsters max keeps their location
int dest_loc[2];


// Queue parameters for enqueue, dequeue, etc functions
#define CAPACITY 100
int size  = 0;
int rear  = CAPACITY - 1;
int front = 0;


/* Function declaration for various operations on queue */
//FUNCTIONS FOR THE QUEUE
int enqueue(int data, int *queue);
int dequeue(int *queue);
int isFull(int *queue);
int isEmpty(int *queue);
int getFront(int *queue);

//FUNCTIONS CREATED TO RUN THE GAME
void move(int rows, int col, int x, int y, int *curr_loc, char grid[rows][col], int *can_move);
void BFS(char grid[][8], char player, int start_r, int start_c);
void organize(int num_monsters, int mon_title[], int mon_location[num_monsters][2]);
int findDistance(int starting_node, int second_node);

int end_game = 0;//keeps track of if the game is ended by being eaten or reaching goal
int num_monsters = 0;//keeps track of 

int rows, col;

int main(int argc, char* argv[])
{

  FILE *fp = fopen(argv[1], "r");
  
  char buffer, direction;//to store the input file

  fscanf(fp, "%d %d\n", &rows, &col);
  char grid[rows][col];//creates the map to print
  char array[rows*col];//the array to store the numbered locations from the grid.
  /*
  	CALCULATIONS TO TURN COORDINATES INTO ONE NUMBER
	number = (node_row * total_col) + node_col

  */

  //Scans the input file into 2D array of char for the grid and marks the players and monsters
  for(int r = 0; r<rows; r++){
    for(int c = 0; c<col; c++){
      fscanf(fp, "%c", &grid[r][c]);
      if(grid[r][c] == '0'){//marks player
        curr_loc[0] = c;
        curr_loc[1] = r;
      }
      else if(grid[r][c] == '*'){
      	dest_loc[0] = r;
      	dest_loc[1] = c;
      }
      else if(grid[r][c] != ' ' && grid[r][c] != '#' && grid[r][c] != '*'){//marks monsters
      	mon_title[num_monsters] = grid[r][c];
      	mon_location[num_monsters][0] = r;
      	mon_location[num_monsters][1] = c;
        num_monsters++;
      }
    }
    fscanf(fp, "%c", &buffer);//buffer for last line character
  }

      printf(" ");
    //prints out the map first
    for(int i = 0; i<col;i++){
      printf("%d", i);
    }
    printf("\n");
    for(int i = 0; i<rows;i++){
      printf("%d", i);

      for(int j = 0; j<col; j++){
        printf("%c", grid[i][j]);
      }

      printf("\n", buffer);
    }

  while(end_game != 1){//multiple rounds for extra credit
  	  char before = ' ';

    //Then player turn to decide where to move
    int can_move = 1;
    do{
      printf("Player 0, please enter your move [u(p), d(own), l(elf), or r(ight)]: ");
      scanf(" %c", &direction);

   if(direction == 'u'){
      move(rows, col, curr_loc[0], (curr_loc[1])-1, curr_loc, grid, &can_move);
    }
    else if(direction == 'd'){
      move(rows, col, curr_loc[0], (curr_loc[1])+1, curr_loc, grid, &can_move);
    }
    else if(direction == 'l'){
      move(rows, col, (curr_loc[0])-1, curr_loc[1], curr_loc, grid, &can_move);
    }
    else if(direction == 'r'){
      move(rows, col, (curr_loc[0])+1, curr_loc[1], curr_loc, grid, &can_move);
    }
    else{
      printf("Invalid input try again\n");
      can_move = 0;
    }
    }while(can_move == 0);//runs until the player picks a position that he is able to move
    
    //printfs the map
    printf(" ");
    for(int i = 0; i<col;i++){
      printf("%d", i);
    }
    printf("\n");
    for(int i = 0; i<rows;i++){
      printf("%d", i);

      for(int j = 0; j<col; j++){
        printf("%c", grid[i][j]);
      }

      printf("\n", buffer);
    }
    
    //Checks if the player moved into a creature of goal
    if(end_game == 1){
      printf("One creature is not hungry any more!\n");
      break;
    }
    //FINDS THE SHORTEST PATH FROM PLAYER TO GOAL TO USE FOR CREATURE ALGORITHM
    BFS(grid, '*', curr_loc[1], curr_loc[0]);
    int j = (dest_loc[0] *col)+dest_loc[1];
    player_path[player_path_index] = (curr_loc[1]*col)+curr_loc[0];
    player_path_index++;

    while(parent[j]!=0){
    	player_path[player_path_index] = j;
    	player_path_index++;
    	j = parent[j];
    }

    player_path[player_path_index] = j;

/*
        //PRINTS THE PATH
    for(int i = 0; i<player_path_index;i++){
        printf("%3d ", i);
    }
    printf("\n");
    for(int i = player_path_index-1; i>-1;i--){
        printf("%3d ", player_path[i]);
    }
  */
    
    //Organizes the monsters into ABC order
    organize(num_monsters, mon_title, mon_location);

    //goes through each monster's turn
    for(int i = 0; i<num_monsters; i++){
    	//printf("MONSTER LOCATION: (%d %d)\n", mon_location[i][0], mon_location[i][1]);
    	BFS(grid, '0', mon_location[i][0], mon_location[i][1]);//finds path
    	int j= (curr_loc[1]*col)+curr_loc[0];//the last node which is the player    
    	//printf("J: %d", j);
    	int r = mon_location[i][0];//monsters row 
    	int c = mon_location[i][1];//monsters col
    	
    	while(parent[j]!=0){
    		path[path_index] = j;
    		path_index++;
    		j = parent[j];
    	}
    	path[path_index] = j;
/*
    	    //PRINTS THE PATH
    for(int i = 0; i<path_index;i++){
        printf("%3d ", i);
    }
    printf("\n");
    for(int i = path_index-1; i>-1;i--){
        printf("%3d ", path[i]);
    }
    */
    	//This part finds the letter to print out for direction monster travels
    	char direction_letter;
    	int start_row, start_col, second_row, second_col;
    		//finds the first coordinate
    		for(int k = 0; k<col; k++){
        		if(path[path_index]-(k*col)>0){
           			start_row = k;
        		}
        	}

        	start_col = path[path_index]-(start_row*col);

        	//finds the second coordinate
        	for(int k = 0; k<col; k++){
        		if(path[path_index-1]-(k*col)>0){
           			second_row = k;
        		}
        	}

        	second_col = path[path_index-1]-(second_row*col);
        	//printf("--%d %d--\n", start_row, second_row);

/*
        	//PRINTS THE PARENTS
        	for(int p = 0; p<(rows*col)-col;p++){
        		printf("%3d ", p);
        	}
        	printf("\n");
        	for(int p = 0; p<(rows*col)-col;p++){
        		printf("%3d ", parent[p]);
        	}
        	printf("\n");
*/
        //compares the coordinates to print out the correct direction
        if(start_row<second_row){
        	direction_letter = 'd';
        }
        else if(start_row>second_row){
        	direction_letter = 'u';
        }
        else if(start_col<second_col){
        	direction_letter = 'r';
        }
        else{
        	direction_letter = 'l';
        }

    	printf("Creature %c: %c %d ", mon_title[i], direction_letter, dz[(curr_loc[1]*col)+curr_loc[0]]);

    	
    	//then goes through the path backwards to print out the coordinate of each of node
    	//have to reverse the calculation done before to turn the 2d array row and col location 
    	//into one number. the first loop finds the row by subtracting the number of col*curr_row.
    	//EX) with 8 col, a number on the third row will be between 1d array number 24-31 so 8*3 will be
    	//above 0 but 8*4 will be below therefore nodes 24-31 have to be on row 3. Then the col is found
    	//by using the original equation but rearranged
    	if(path_index != 0){
    	    for(int j = path_index; j>-1;j--){
    			for(int k = 0; k<col; k++){
        			if(path[j]-(k*col)>0){
           				r = k;
        			}
        		}

        		c = path[j]-(r*col);

        			//Updates the monster's location and moves them to the next location
        			mon_location[i][0] = second_row;
        			mon_location[i][1] = second_col;
        			grid[second_row][second_col] = mon_title[i];
        			

        			//Since monster's paths may cross, this checks to see if the monster previously in that grid
        			//should be left or if the grid will be empty after the current creature moves
        			if(grid[start_row][start_col] != ' ' && grid[start_row][start_col] != mon_title[i]){
    					//nothing happens it leaves what was there before in case creature's path overlap
    				}
    				else{
    					grid[start_row][start_col] = ' ';
    				}
        			
        		printf("(%d, %d)", r, c);
        	}
    	}
    	else{
    		printf("There is no path from Creature %c to Player 0", mon_title[i]);
    	}

    	//print the map
    	printf("\n");
    	printf(" ");
    	for(int i = 0; i<col;i++){
      		printf("%d", i);
    	}
    	printf("\n");
    	for(int i = 0; i<rows;i++){
      		printf("%d", i);
    		for(int j = 0; j<col; j++){
    			printf("%c", grid[i][j]);
     		}

      		printf("\n", buffer);
    	}

    	//Checks to see if the monster caught the player
        if(mon_location[i][0] == curr_loc[1] && mon_location[i][1] == curr_loc[0]){
        	printf("One creature is not hungry any more!\n");
            end_game = 1; 
            break;
        }

/*
        //PRINTS THE PATH
        for(int i = 0; i<path_index;i++){
        	printf("%3d ", i);
        }
        printf("\n");
        for(int i = path_index-1; i>-1;i--){
        	printf("%3d ", path[i]);
        }
*/        

    	//Sets back all the used indexes and array to original so next monster can start from scratch
    	curr_index = 0;
    	path_index = 0;
    
    	size = 0;

    	for(int i = 0; i<100; i++){
    		tree[i] = 0;
    		parent[i] = 0;
    	}
    }
    player_path_index = 0;
  }
  fclose(fp);//closes the input file
}

int findDistance(int starting_node, int second_node){
    int distance = 3;
    //printf("PLAYER PATH INDEX: %d\n", player_path_index);
    //printf("WHAT: %d %d", distance, player_path_index);
    //PRINTS THE PATH
    /*
    for(int i = 0; i<player_path_index;i++){
        printf("%3d ", i);
    }
    printf("\n");
    for(int i = player_path_index-1; i>-1;i--){
        printf("%3d ", player_path[i]);
    }
    */
    //printf("STARTING %d, second_node %d (%d)", starting_node, second_node, player_path_index);
    for(int i = 0; i< player_path_index; i++){//goes through each path location
    	//printf("Player path: %d \n", player_path[i]);
    	// if one of the locations (start to end) is on path then value is 2
    	if(player_path[i] == starting_node){
    		distance--;
    	}
    	//if both of the locations (start to end) is on the path then value is 1
    	else if(player_path[i] == second_node){
    		distance--;
    	}
    }
   //printf("FUNCTION DISTANCE %d\n ", distance);
   //player_path_index = 0;
    return distance;

}


//Runs a selection sort through the monsters letters array and the location array to 
//organize the arrays into ABC order. Takes the letter at front and switches it to the location
//it belongs until all letters are in order (selection sort)
void organize(int num_monsters, int mon_title[], int mon_location[num_monsters][2]){
	int min; 
  
    for (int i = 0; i < num_monsters-1; i++) { 
        min = i; 
        for (int j = i + 1; j < num_monsters; j++) 
            if (mon_title[j] < mon_title[min]) 
                min = j; 

  		//letter ordering
        int temp = mon_title[min]; 
    	mon_title[min] = mon_title[i]; 
    	mon_title[i] = temp; 

    	//changes the x values of monster to order
    	int temp_x = mon_location[min][0]; 
    	mon_location[min][0] = mon_location[i][0]; 
    	mon_location[i][0] = temp_x;

		//changes the y values of monster to order
    	int temp_y = mon_location[min][1]; 
    	mon_location[min][1] = mon_location[i][1]; 
    	mon_location[i][1] = temp_y;  
    } 
}


//Sets the starting node (monsters location) to the front of the queue.
//Then a while loop runs that takes the front of the loop and adds all the neighbors
//to the stack and then dequeue the front. It continues to do this until of all the children
//are recorded in order. It keeps track of the parent node through the parent array which has 
//the node as the index and the parent as the data in the array. Then, it dequeues the queue
//and frees the memory for the next time the function is called
void BFS(char grid[rows][col], char player, int start_r, int start_c){
	//the row and col for calculations on turning coordintates to one number
	int r, c;
	r = start_r;
	c = start_c;

	//creates a queue and stores the first node (starting node)
	int queue[100];
	int success = enqueue((r*col)+c, queue);//the starting node
	int v;
	tree[0] = (r*col)+c;
	curr_index++;


	//Initializes the d(s) = 0 and d(v) = infinity
	for(int n = 0; n < (rows*col);n++){
		dz[n] = 100000000;
	}
	dz[(r*col)+c] = 0;


	while(isEmpty(queue) != 1){//stops when the player is reached or empty
		v = getFront(queue);
    	tree[curr_index] = v;
		success = dequeue(queue);
    	visited[v] = 1; //sets to visited so no repeats

   // printf("UP:%c Down:%c LEFT:%c RIGHT:%c\n", grid[r-1][c], grid[r+1][c],grid[r][c-1],grid[r][c+1]);
		
    if(player == '*'){
		if(visited[((r-1)*col)+c] != 1 && grid[r-1][c] != '#'&& (grid[r-1][c]<65)){//up
         	success = enqueue(((r-1)*col)+c, queue);
         	visited[((r-1)*col)+c] =1;
         	parent[((r-1)*col)+c] = v;//CHANGE PARENT IF LESS THAN?
		}
		if(visited[((r+1)*col)+c] != 1 && grid[r+1][c] != '#'&& (grid[r+1][c]<65)){//down
         	success = enqueue(((r+1)*col)+c, queue);
         	visited[((r+1)*col)+c] = 1;
         	parent[((r+1)*col)+c] = v;
		}
        if(visited[(r*col)+(c-1)] != 1 && grid[r][c-1] != '#'&& (grid[r][c-1]<65)){//left
         	success = enqueue((r*col)+(c-1), queue);
         	visited[(r*col)+(c-1)] = 1;
          	parent[(r*col)+(c-1)] = v;
		}
		if(visited[(r*col)+(c+1)] != 1 && grid[r][c+1] != '#' && (grid[r][c+1]<65)){//right
         	success = enqueue((r*col)+(c+1), queue);
         	visited[(r*col)+(c+1)] =1;
         	parent[(r*col)+(c+1)] = v;
		}
    }
    else{
		if(visited[((r-1)*col)+c] != 1 && grid[r-1][c] != '#' && grid[r-1][c] != '*'&& (grid[r-1][c]<65)){//up
         	success = enqueue(((r-1)*col)+c, queue);
         	//visited[((r-1)*col)+c] =1;
         	//printf("DISTANCE: %d", findDistance(v, ((r-1)*col)+c));
         	//printf("CURRENT SUM: %d New: %d", dz[(((r-1)*col)+c)], dz[v] + findDistance(v, ((r-1)*col)+c));
         	if(dz[((r-1)*col)+c] > dz[v] + findDistance(v, ((r-1)*col)+c)){
         		parent[((r-1)*col)+c] = v;//CHANGE PARENT IF LESS THAN?
         		dz[((r-1)*col)+c] = dz[v] + findDistance(v, ((r-1)*col)+c);
         	}
         	
		}
		if(visited[((r+1)*col)+c] != 1 && grid[r+1][c] != '#' && grid[r+1][c] != '*' && (grid[r+1][c]<65)){//down
         	success = enqueue(((r+1)*col)+c, queue);
         	//visited[((r+1)*col)+c] = 1;
         	//printf("CURRENT SUM: %d New: %d", dz[(((r+1)*col)+c)], dz[v] + findDistance(v, ((r+1)*col)+c));
         	if(dz[((r+1)*col)+c] > dz[v] + findDistance(v, ((r+1)*col)+c)){
         		parent[((r+1)*col)+c] = v;//CHANGE PARENT IF LESS THAN?
         		dz[((r+1)*col)+c] = dz[v] + findDistance(v, ((r+1)*col)+c);
         	}
         	//parent[((r+1)*col)+c] = v;
		}
        if(visited[(r*col)+(c-1)] != 1 && grid[r][c-1] != '#' && grid[r][c-1] != '*' && (grid[r][c-1]<65)){//left
         	success = enqueue((r*col)+(c-1), queue);
         	//visited[(r*col)+(c-1)] = 1;
          	//parent[(r*col)+(c-1)] = v;
          	//printf("CURRENT SUM: %d New: %d", dz[(r*col)+(c-1)], dz[v] + findDistance(v, (r*col)+(c-1)));
          	if(dz[(r*col)+(c-1)] > dz[v] + findDistance(v, (r*col)+(c-1))){
         		parent[(r*col)+(c-1)] = v;//CHANGE PARENT IF LESS THAN?
         		dz[(r*col)+(c-1)] = dz[v] + findDistance(v, ((r*col)+(c-1)));
         	}
		}
		if(visited[(r*col)+(c+1)] != 1 && grid[r][c+1] != '#' && grid[r][c+1] != '*'&& (grid[r][c+1]<65)){//right
         	success = enqueue((r*col)+(c+1), queue);
         	//visited[(r*col)+(c+1)] =1;
         	//parent[(r*col)+(c+1)] = v;
          //0("CURRENT SUM: %d New: %d", dz[(r*col)+(c+1)], dz[v] + findDistance(v, (r*col)+(c+1)));
         	if(dz[(r*col)+(c+1)] > dz[v] + findDistance(v, (r*col)+(c+1))){
         		parent[(r*col)+(c+1)] = v;//CHANGE PARENT IF LESS THAN?
         		dz[(r*col)+(c+1)] = dz[v] + findDistance(v, ((r*col)+(c+1)));
         	}
		}
	}
	//printf("dz: %d %d \n", v, dz[v]);

		//creates a row and col for the next node in queue so calculations can be made
    	v = getFront(queue);
    	tree[curr_index] = v;
    	//curr_index++;

        for(int i = 0; i<col; i++){
          if(v-(i*col)>0){
            r = i;
          }
        }
        c = v-(r*col);
    }

    //Set up for next use(dequeues, sets all nodes to unvisited and frees the space)
  	while(!isEmpty(queue)){
  		success = dequeue(queue);
  	}


  	for(int i = 0; i<rows*col;i++){
  		visited[i] = 0;
  	}

/*
  	printf("TREE: ");
    	for(int p = 0; p<curr_index;p++){
    		printf("%d ", tree[p]);
    	}
    	printf("\n");


  	printf("WEIGHTS: ");
    	for(int p = 0; p<(rows*col)-4;p++){
    		printf("%d ", dz[p]);
    	}
    	printf("\n");
*/
  //free(queue);
}

//Reads the players input and moves them properly, making sure they don't run into obstacles
//the goal or monster
void move(int rows, int col, int x, int y, int *curr_loc, char grid[rows][col], int *can_move){
  if(grid[y][x] == ' '){//can move
    grid[curr_loc[1]][curr_loc[0]] = ' ';
    curr_loc[0] = x;
    curr_loc[1] = y;

    grid[y][x] = '0';
    *can_move = 1;
  }
  else if(grid[y][x] == '#'){//cannot move to obstacle
    printf("You cannot travel there, obstacle in your way!\n");
    *can_move = 0;
  }
  else if(grid[y][x] == '*'){//ends the game since success
  	grid[curr_loc[1]][curr_loc[0]] = ' ';
    curr_loc[0] = x;
    curr_loc[1] = y;

    grid[y][x] = '0';
    printf("Player 0 beats the hungry creatures!\n");
    end_game = 1;
    *can_move = 1;
  }
  else{//ends the game since failure
    grid[curr_loc[1]][curr_loc[0]] = ' ';

    end_game = 1;
    *can_move = 1;
  }
}

//Puts data at the end of the queue 
int enqueue(int data, int *queue){
    if (isFull(queue)) 
    {
        return 0;
    }
    rear = (rear + 1) % CAPACITY;
    size++;
    queue[rear] = data;
    return 1;
}

//Since a queue is FIFO it takes data from froont and removes
int dequeue(int *queue){
    int data = INT_MIN;
    if (isEmpty(queue))
    {
        return INT_MIN;
    }

    data = queue[front];
    front = (front + 1) % CAPACITY;
    size--;

    return data;
}

//Checks if queue is full
int isFull(int *queue){
    return (size == CAPACITY);
}

//Checks if queue is empty
int isEmpty(int *queue){
    return (size == 0);
}

//Gets the front number in queue
int getFront(int *queue){
    return (isEmpty(queue))
            ? INT_MIN
            : queue[front];
}

