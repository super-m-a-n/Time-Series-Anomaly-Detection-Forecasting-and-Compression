//file:cbtree.cpp//
#include <iostream>
#include "object.hpp"
#include "assist_functions.hpp"
#include "cbtree.hpp"
#include <list>

void CBTree::destroy(node *node_ptr)
{
	if (node_ptr != nullptr)
	{
		destroy(node_ptr->left);
		destroy(node_ptr->right);
		delete node_ptr;
		node_ptr = nullptr;
    }
}

CBTree::node * CBTree::full_binary_tree(unsigned int height, std::list <node *> & leaves)
{
	if (height == 0)
		return nullptr;

	// create root node
	node * root_node = new node();
	
	// create empty lists of left, right leaves
	std::list <node *> left_leaves, right_leaves;
	
	// left node of root node will be a full binary tree of height - 1
	root_node->left = full_binary_tree(height - 1, left_leaves);
	
	// right node of root node will be a full binary tree of height - 1
	root_node->right = full_binary_tree(height - 1, right_leaves);

	// append the leaves of left subtree to tree leaves
	leaves.splice(leaves.end(), left_leaves);
	//leaves.insert(leaves.end(), left_leaves.begin(), left_leaves.end());

	// if root node is itself a leaf, push it back to leaves list
	if (root_node->left == nullptr && root_node->right == nullptr)
		leaves.push_back(root_node);

	// append the leaves of right subtree to tree leaves
	leaves.splice(leaves.end(), right_leaves);

	// return root node of full binary tree constructed
	return root_node;
}

Abstract_Object * CBTree::post_order_traversal(node * cur_node)
{
	// if current node is leaf
	if (cur_node->left == nullptr && cur_node->right == nullptr)
		return cur_node->curve;
	else
	{
		//since it is not a leaf, and tree is a complete binary tree, cur_node->left is not null
		// compute mean curve of left subtree
		Abstract_Object * left_curve = post_order_traversal(cur_node->left);
		// compute mean curve of right subtree
		Abstract_Object * right_curve = nullptr;
		if (cur_node->right != nullptr)
			right_curve = post_order_traversal(cur_node->right);

		// mean curve of current node is mean curve of left curve and right curve
		cur_node->curve = mean_curve(left_curve, right_curve);
		return cur_node->curve;
	}
}

CBTree::CBTree(const std::vector <const Abstract_Object*> & cluster)
{   
    // find h = biggest power of 2, smaller than the size of the cluster h = O(#curves)
    unsigned int h = largest_power_of_2_smaller_than(cluster.size());

    // h == 0 iff cluster is empty, this is already checked for, so cluster is for sure not empty
    if (h == 0)
    {
    	root = nullptr;
    	std::cerr << "CBTree::CBTree -> Error: Expected non-empty cluster\n\n";
    	return;
    }

    // list will hold pointers to h leaf nodes of full binary tree for easy access
    std::list <node *> leaves;

    // construct a full/perfect binary tree of log(h)+1 height (this gives h = O(#curves) leaves)
    // and return the h leaves in the leaves list
    this->root = this->full_binary_tree(get_lg(h), leaves);

    int index = 0;

    // assign h of cluster's curves to the leaves of that tree (h <= #curves <= 2*h)
    for (auto const& node : leaves) 
    {
 	   node->curve = new time_series(*cluster[index]);
 	   index++;
	}

    // for the curves remaining, for each node-leaf of last level, from left to right, split the node-leaf to 2 new nodes-leafs on next level
    for (auto const& cur_node : leaves)
    {
    	// if all curves have been assigned to a leaf node, break
    	if (index >= (int) cluster.size())
    		break;
    	
    	// split leaf node of last level, to two leaf node children of next level
    	cur_node->left = new node();
    	cur_node->right = new node();
    	// left leaf node curve of next level becomes curve of former leaf node of last level
    	cur_node->left->curve = cur_node->curve;
    	// right leaf node curve of next level becomes new curve
    	cur_node->right->curve = new time_series(*cluster[index]);
    	// former leaf node of last level, has no curve assigned now
    	cur_node->curve = nullptr;
    	index++;
    }
}

CBTree::~CBTree()
{
	destroy(this->root);
}


Abstract_Object * CBTree::post_order_traversal()
{
	Abstract_Object * mean_curve = this->post_order_traversal(this->root);
	if (mean_curve == nullptr)
		std::cerr << "CBTree::post_order_traversal -> Error: Expected non-null mean_curve\n\n";
	return new time_series(*mean_curve);
}
