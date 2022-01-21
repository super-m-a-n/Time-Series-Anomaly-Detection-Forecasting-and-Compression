//file:cbtree.hpp//
#ifndef _CBTREE_HPP_
#define _CBTREE_HPP_
#include <iostream>
#include "object.hpp"
#include <list>

// class for complete binary tree, used to find mean curve in clustering
class CBTree
{
private:
	//node of tree implemented with a struct
	struct node 
	{
		Abstract_Object * curve;	// pointer to Abstract Object curve of node
		node * left;
		node * right;
		
		// default constructor for tree node
		node() : curve(nullptr), left(nullptr), right(nullptr){}
		
		// destructor
		~node()
		{
			if (curve != nullptr)
				delete curve;
		}
	};

	//pointer to root of tree (basically way to refer to the tree)
	node * root;
	// destroy function called by destructor, for node cleanup
	void destroy(node * node_ptr);
	// creates a full binary tree of given height, and returns its root, and its leaves in leaves list
	CBTree::node * full_binary_tree(unsigned int height, std::list <node *> & leaves);
	// helper function for post_order_traversal (this function does the actual work xddd)
	Abstract_Object * post_order_traversal(node * root);


public:
	// constructor
	CBTree(const std::vector <const Abstract_Object*> & cluster);
	// destructor
	~CBTree();
	// traverses the nodes of tree in post order fashion and computes mean curves bottom up for each internal node
	// returns the final mean curve of root node
	Abstract_Object * post_order_traversal();
};

#endif