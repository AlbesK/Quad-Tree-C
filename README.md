# Quad-Tree-C
Simple node structure for a Quad tree in bottom-up approach written in C. 

The functions inside the main.c were made as simple as possible, with plenty of comments. So far keeping them in one file but might make a makefile in the near future. 

It was quite tricky to find resources online for quad-trees written purely in C and simple enough to understand, so thought to write my own for practice by generalising the binary tree concept to 4 instead of 2 nodes, not revolutionary but quite fun!

Dynamic memory handlement to segment 2D space for bodies occupying it is also used with appropriate functions of allocating and deallocating the heap memory.

![alt text](https://github.com/AlbesK/Quad-Tree-C/blob/master/100_particles_bottomup_Quad_Tree.png)

Image was produced with Pythons matplotlib after saving the data in csv format.

Supports: ( so far )

# insert
Inserting new bodies recursively from a dynamically allocated array of them. 

It also takes care of moving the body occupied by the parent node when a new body is assigned inside the nodes boundary and moves both down the tree to new leaf nodes.

It also uses unique identifiers for the data variable for each node, with twigs referring to ones being empty or that had more than 2 particles with negative integers, 0 for root and all positive values correspond to the same index of the respective body it is pointing at.

# search

Using the unique identifiers above one can search in in-order format the tree and find the body or twig they need.

# display_tree

Displays Tree in in-order recursion.

# level_order

Displays Tree in Breadth First travel using dynamic queue made by 2 single linkedLists.

(https://github.com/AlbesK/Quad-Tree-C/blob/master/queue.png)

# xyt_data_particles

Saves x, y coordinates of Particles plus time taken to build the quad tree. Change path for it to work.

# xy_trees

Saves x, y coordinates of centres of nodes inside quad trees plus the size of the node. Change path for it to work.

