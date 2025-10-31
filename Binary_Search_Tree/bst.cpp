//Owen Youngblood
//this lab is a custom implementation of a binary search tree
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include "bstree.hpp"
using namespace std;
using CS140::BSTree;
using CS140::BSTNode;

int BSTree::Depth(const string &key) const
{
	BSTNode *current = sentinel->right;
	int depth = 0;

	while (current != sentinel) 
	{
		if (key == current->key) 
		{
			return depth;
		}
		else if (key < current->key)
		{
			current = current->left;
			depth++;
		}
		else
		{
			current = current->right;
			depth++;
		}
	}
	return -1;
}

int BSTree::Height() const
{
	return recursive_find_height(sentinel->right);
}

vector <string> BSTree::Ordered_Keys() const
{
	vector<string> rv;
	make_key_vector(sentinel->right, rv);
	return rv;
}

BSTree::BSTree(const BSTree &t)        
{
	sentinel = new BSTNode;
	sentinel->left = NULL;
	sentinel->right = sentinel;
	sentinel->parent = NULL;
	sentinel->key = "---SENTINEL---";
	sentinel->val = NULL;
	size = 0;

	if (t.sentinel->right != t.sentinel) 
	{
		vector<string> sorted_keys = t.Ordered_Keys();
		vector<void*> vals = t.Ordered_Vals();
		BSTNode *newRoot = make_balanced_tree(sorted_keys, vals, 0, sorted_keys.size());
		sentinel->right = newRoot;
		newRoot->parent = sentinel;
		size = sorted_keys.size();
	}
}

BSTree& BSTree::operator= (const BSTree &t) 
{
	if (this != &t) 
	{
		Clear();	
		vector<string> sorted_keys = t.Ordered_Keys();
		vector<void*> vals = t.Ordered_Vals();
		sentinel->right = make_balanced_tree(sorted_keys, vals, 0, sorted_keys.size());
		sentinel->right->parent = sentinel;
		size = sorted_keys.size();
	}
	return *this;
}

int BSTree::recursive_find_height(const BSTNode *n) const
{
	if (n == sentinel)
	{
		return 0;
	}
	else if (n->left == sentinel && n->right == sentinel)
	{
		return 1;
	}
	int left_height = recursive_find_height(n->left);
	int right_height = recursive_find_height(n->right);

	return max(left_height, right_height) + 1;
}

void BSTree::make_key_vector(const BSTNode *n, vector<string> &v) const
{
	if (n == sentinel)
	{
		return;
	}
	make_key_vector(n->left, v);
	v.push_back(n->key);
	make_key_vector(n->right, v);
}

BSTNode *BSTree::make_balanced_tree(const vector<string> &sorted_keys, const vector<void *> &vals, size_t first_index, size_t num_indices) const 
{
	if(num_indices == 0)
	{
		return sentinel;
	}
	size_t middle_index = first_index + num_indices / 2;
	BSTNode *temp = new BSTNode;
    temp->key = sorted_keys[middle_index];
    temp->val = vals[middle_index];
    temp->left = make_balanced_tree(sorted_keys, vals, first_index, middle_index - first_index);
    if (temp->left != NULL) 
	{
        temp->left->parent = temp;
    }

    temp->right = make_balanced_tree(sorted_keys, vals, middle_index + 1, num_indices - (middle_index - first_index + 1));
    if (temp->right != NULL) 
	{
        temp->right->parent = temp;
    }
	return temp;
}
