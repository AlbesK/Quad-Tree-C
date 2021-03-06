/*
    Quad Tree implementation with dynamic memory in C.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

/*
    Point structure, having for 2D 2 components
*/
struct point 
{
    double x;
    double y;
};

/*
    Body structure, having coordinates in 2D.
*/
struct body 
{
    struct point pos; // Position
    // perhaps putting mass etc here
};

/*
    Quad tree node structure
*/
struct quad  
{ 
    int data; // key value
    double s; // Original size corresponding to this quad
    struct point centre; // Center of this node
    bool divided; // Check if it has divided

    struct body *b; // Pointer to body structure

    // Pointers to children of this node taken with respect to compass directions.
    struct quad *NW; 
    struct quad *NE;
    struct quad *SE;
    struct quad *SW; 
}; 
  
/* 
    Allocates a new node and respectively fills data, coordinates and size of square for that node.
*/
struct quad* newNode(int data, double s, double x, double y) 
{ 
    // Allocate memory for new quad  
    struct quad* quad = (struct quad*)malloc(sizeof(struct quad)); 

    // If out of memory abort:
    if(quad == NULL)
    {
        fprintf (stderr, "Out of memory!!! (create_node)\n");
        exit(1);
    }
    // Assign data to this quad, s size of square, centre point of it
    // b as NULL for not storing a pointer to a body yet, false for divided as it has not subdivided yet.
    quad->data = data;
    quad->s = s; 
    quad->centre.x = x; quad->centre.y = y;
    quad->b = NULL;
    quad->divided = false;

    // Initialize all children as NULL 
    quad->NE = NULL; 
    quad->SE = NULL;
    quad->SW = NULL;
    quad->NW = NULL;

    return(quad); 
} 

	
/*
    Recursively display tree or subtree inorder
*/
void display_tree(struct quad* nd)
{
    if (nd == NULL)
        return;
    // Display the data of the node
    printf(" %*c(%d) \n %*c[%f, %f] \n %*c[%f]\n\n",50, ' ', nd->data, 42,' ', nd->centre.x, nd->centre.y, 47, ' ', nd->s);
    
    // Display data for each node plus some formating so it looks better
    if(nd->NE != NULL)
        printf("%*c|NE:%d|  ",34,' ',nd->NE->data);
    if(nd->SE != NULL)
        printf("|SE:%d|  ",nd->SE->data);
    if(nd->SW != NULL)
        printf("|SW:%d|  ",nd->SW->data);
    if(nd->NW != NULL)
        printf("|NW:%d|",nd->NW->data);
    printf("\n\n");
    
    // Recurse through
    display_tree(nd->NE);
    display_tree(nd->SE);
    display_tree(nd->SW);
    display_tree(nd->NW);
}

/*
    Deconstruct quad tree (postorder) also de-allocates memory 
*/ 
void deconstruct_tree(struct quad* root)
{
    if(root != NULL)
    {
        deconstruct_tree(root->NE);
        deconstruct_tree(root->SE);
        deconstruct_tree(root->SW);
        deconstruct_tree(root->NW);

        free(root);
    }
}

/*
    Subdivide node to 4 quadrants and assign memory dynamically through newNode() function
*/
void subdivide(struct quad* nd, int* track){ 
    
    if(nd == NULL){ // If there is no node do not subdive. (safety measure)
        return;
    }

    // Call newNode function for each child node that was Null of the node at hand and assign a memory block of size (struct quad)
    // -n is assigned here if the node is a 'twig' meaning it is not a 'leaf' for now empty cells are also -n where n integers that
    // that are unique for each quad.
    //    _____________________
    //   |          |          |
    //   |   (NW)   |   (NE)   |
    //   |          |          |
    //   |    -+    |    ++    |
    //   |__________|__________|
    //   |          |          |
    //   |    --    |    +-    |
    //   |          |          |
    //   |   (SW)   |   (SE)   |
    //   |__________|__________|

    nd->NE = newNode(*track-1, nd->s/2, nd->centre.x + nd->s/4, nd->centre.y + nd->s/4); 
    nd->SE = newNode(*track-2, nd->s/2, nd->centre.x + nd->s/4, nd->centre.y - nd->s/4); 
    nd->SW = newNode(*track-3, nd->s/2, nd->centre.x - nd->s/4, nd->centre.y - nd->s/4); 
    nd->NW = newNode(*track-4, nd->s/2, nd->centre.x - nd->s/4, nd->centre.y + nd->s/4); 
    
      
    nd->divided = true; // The node subdivided ( safety for not subdividing again the same node )
    *track = *track-4; // track gets last value so it can iterate through without repeat

}


/*
    Check if the body is residing inside the node through its coordinates
*/
bool contains(struct quad* nd, struct point p){
        return (p.x < (nd->centre.x+nd->s/2) &&
        p.x > (nd->centre.x-nd->s/2) &&
        p.y < (nd->centre.y+nd->s/2) &&
        p.y > (nd->centre.y-nd->s/2));
        
}


/*
    Construct quad tree from bottom-up by putting bodies inside it.
*/
int insert(struct quad* nd, struct body* b, int *index, int* track){

    // If current quad cannot contain it 
    if (!contains(nd,b->pos)){ 
        return 0; // Not found yet so return 0 and go in the function again
    } 

    if(nd->b==NULL){ // If there is no pointer to body assign it (Essentially capacity is kept at 1 here with this method)
        nd->b = b;
        nd->data = *index; //Assign the number of the body from the Bodies array, this is for getting back with data where the body is stored as a leaf
        return 0; // Found so return 0 so we can exit the recursion
    } 
    else{
    
        if(nd->divided!=true){ // Check if the quad quad has subdivided
            subdivide(nd, track); // If not, subdivide!
            // Check where to put nodes body and then after the if statement check for the current index body
            int temp = nd->data;
            nd->data = *track+4;
            *track = *track-1;
            insert(nd->NE, nd->b, &temp, track);  
            insert(nd->SE, nd->b, &temp, track);  
            insert(nd->SW, nd->b, &temp, track); 
            insert(nd->NW, nd->b, &temp, track);
        }
    }

    // Since insert is an int function, the return statement here returns 0 if 
    // a node is found to point the body and thus save the body at. So for example if we
    // have 2 bodies and the first one is always saved at root ( being empty and not divided)
    // then the second Body 1 if it is at NW subcell after division when it goes to the return state-
    // -ment it will go through the OR terms as the contain function will not let the first 3 OR terms 
    //( being insert(nd->NE,...) || ...  to insert(nd->SW,...) and pick the insert(nd->NW,...) to assign. 
    //The same process is repeated every time the function calls itself and checks for where to put the body.

    return insert(nd->NE, b, index, track)||  
    insert(nd->SE, b, index, track)||  
    insert(nd->SW, b, index, track)||
    insert(nd->NW, b, index, track); 
}

/*
    Search quad tree for node with specific data in inorder format
*/
struct quad* Search(struct quad* root, int data) {
	// base condition for recursion
	// if tree/sub-tree is empty, return and exit
	if(root == NULL){return NULL;}

	printf("%i \n",root->data); // Print data
    if(root->data == data){return root;} // Found so return quad node
	
    Search(root->NW, data);     // Visit NW subtree
	Search(root->SW, data);    // Visit SW subtree
    Search(root->SE, data);   // Visit SE subtree
    Search(root->NE, data);  // Visit NE subtree
}

void xyt_data_particles(struct body* bodies, int* N_PARTICLES, double t){
    FILE * f; 
    f = fopen("/home/albes/Desktop/bodiestd.txt", "w"); /* open the file for writing*/
    printf("Writting...\n");
    /* write 4 lines of text into the file stream*/    
    fprintf(f, "N,X,Y,T\n");

    for(int i = 0; i < *N_PARTICLES;i++){
        fprintf (f, "%d,%f,%f\n", i, bodies[i].pos.x, bodies[i].pos.y);
    }
    fprintf(f,",,,%f",t); // Comas to move to the last column

    /* close the file*/  
    fclose (f);
    printf("Closed file.\n");
}

/*
    Helper function for xy_tree.c to print data to csv file so it can be processed later
*/
void printdata(struct quad* nd, FILE* f){
    if (nd == NULL){return;}
    // Display the data of the node
    fprintf(f,"%d,%f,%f,%f\n",nd->data, nd->centre.x, nd->centre.y, nd->s);
    
    // Recurse through
    printdata(nd->NE,f);    
    printdata(nd->SE,f);
    printdata(nd->SW,f);
    printdata(nd->NW,f);
}

/*
    Function to print quad tree data into csv files
*/
void xy_trees(struct quad* nd){
    FILE * f; 
    f = fopen("/home/albes/Desktop/nodestd.txt", "w"); /* open the file for writing*/
    printf("Writting...\n");
    /* write 10 lines of text into the file stream*/    
    fprintf(f, "N,X,Y,S\n");
    printdata(nd, f);
    /* close the file*/  
    fclose (f);
    printf("Closed file.\n");
}

/*
    Linked list datastructure pointing at quad nodes
*/
struct linkedList
{
  struct quad* data;
  struct linkedList* next;

};

/*
    Queue Datastructure out of 2 single linked lists
*/
struct linkedList* begin;
struct linkedList* end;

/*
    Boolean function checking if queue is empty
*/
bool queue_empty()
{
  return (begin==NULL && end==NULL);
}

/*
    Push element inside the end part of the queue by allocating new memory dynamically
*/ 
void enqueue(struct quad* nd)
{
    struct linkedList* temp = malloc(sizeof(struct linkedList));
    temp->data = nd;
    temp->next = NULL;
    if(begin==NULL && end==NULL){
        begin = end = temp;
    }
    end->next = temp;
    end = temp;
}

/*
    Pop element from queue and free its memory
*/
void dequeue()
{
    struct linkedList* temp = begin;
    if(begin==NULL){return;}
    if(begin==end){
        begin = NULL;
        end = NULL;
    }
    else{
        begin = begin->next;                    
    }
    free(temp);
}

/*
    Level Order Traversal for force summation ( Breadth first traversal)
*/
void levelorder(struct quad* n)
{
    enqueue(n);
    enqueue(NULL); // Extra NUll parameter for checking when the tree goes to the next level after enquing all children in one level.
    struct quad* curr = NULL;
    while (!queue_empty())
    {
    
    curr = begin->data;
    dequeue();
    if(curr!=NULL){
        
        if (curr->NE)
            enqueue(curr->NE);
        if (curr->SE)
            enqueue(curr->SE);
        if (curr->SW)
            enqueue(curr->SW);
        if (curr->NW)
            enqueue(curr->NW);
        printf("%d ",curr->data);
    }
    else{
        printf("\n");
        if(!queue_empty()){
            enqueue(NULL);
        }
    }
  
    }
}

/*
    Main function to run the program
*/
int main() {

    int N_DIMENSIONS = 2; int seed=1;
    int N_PARTICLES;
    char term;
    clock_t ts, te;
    printf("How many particles?\n");
    if (scanf("%d%c", &N_PARTICLES, &term) != 2 || term != '\n') {
        printf("Failure: Not an integer. Try again\n");
        exit(-1);
    } 

    printf("Calculating\n");
    if (N_PARTICLES < 2) {
        printf("Insufficent number of particles %i\n", N_PARTICLES);
        exit(-1);
    } 
    struct body* bodies = malloc(sizeof(struct body)*N_PARTICLES);

    //Initialise values:
    char x[2]={'x', 'y'};
    srand(seed);

    for (int i=0; i < N_PARTICLES; i++){

            struct point p = {.x = 100 * ((double) rand() / (double) RAND_MAX ) - 50, // -50, 50
            .y = 100 * ((double) rand() / (double) RAND_MAX ) - 50};

            struct body b = {.pos = p };

            bodies[i] = b;
            // printf("%c:[%f], %c:[%f] \n", x[0], bodies[i].pos.x, x[1], bodies[i].pos.y );
    
    }

    struct quad *root = newNode(0, 100, 0, 0); //Size of s=100 and pint of reference being (0,0) equiv. to (x_root, y_root)  
    //printf("Root square size is: %f\n", root->s);
    int track = 0;

    // Build up the tree
    ts = clock(); // Start timer
    for(int i=0; i<N_PARTICLES; i++){
        insert(root, &bodies[i], &i, &track);
    }
    te = clock(); // End timer

    double d = (double)(te-ts)/CLOCKS_PER_SEC; // Bottom-up tree construction time
    
    // display_tree(root);
    char c;
    printf("Do you want to save the data? Y/n \n");
    scanf("%c", &c);
    
    if(c=='Y'){
        printf("Saving bodies data...\n");
        xyt_data_particles(bodies,&N_PARTICLES, d);
        printf("Saving tree data...\n");
        xy_trees(root);
        printf("Done\n");
    } else
    {
        printf("Continuing\n");   
    }

    levelorder(root); // Breadth First traversal print in terminal

    deconstruct_tree(root);
    free(bodies);

    printf("Released memory succesfuly\n");
    printf("Program took %f\n", d);

    return 0; 
}