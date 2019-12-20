/*
Erik Ratchford
December 8, 2019
hw11
rbt_collection.h
*/

#ifndef RBT_COLLECTION_H
#define RBT_COLLECTION_H

#include <vector>
#include <iostream>
#include "collection.h"

template <typename K, typename V>
class RBTCollection : public Collection<K,V>
{
	public:

	// create an empty linked list
	RBTCollection();												

	// copy a linked list
	RBTCollection(const RBTCollection<K,V>& rhs);					

	// assign a linked list
	RBTCollection<K,V>& operator=(const RBTCollection<K,V>& rhs);	

	// delete a linked list
	~RBTCollection();											

	// insert a key - value pair into the collection
	void insert(const K& key, const V& val);						

	// remove a key - value pair from the collection
	void remove(const K& key);										

	// find the value associated with the key
	bool find(const K& key, V& val) const;							

	// find the keys associated with the range
	void find(const K& k1, const K& k2, std::vector<K>& keys) const; 

	// return all keys in the collection
	void keys(std::vector<K>& keys) const;							

	// return collection keys in sorted order
	void sort(std::vector<K>& keys) const;							

	// return the number of keys in collection
	int size() const;												

	// return the height of the tree
	int height() const;												

	private:

	// binary search tree node structure
	struct Node {
		K key;
		V value;
		Node* left;
		Node* right;
		bool is_black ; // true if black , false if red
		bool is_dbl_black_left;
		bool is_dbl_black_right;
	};

	// root node of the search tree
	Node* root;

	// number of k-v pairs in the collection
	int collection_size;

	// helper to recursively empty search tree
	void make_empty(Node* subtree_root);											

	// recursive helper to remove node with given key
	Node* remove(const K& key, Node* parent, Node* subtree_root, bool& found);				

	// helper to perform a single rebalance step on a red - black tree on remove
	Node* remove_color_adjust(Node* parent);


	// recursive helper to do red - black insert key - val pair (backtracking)
	Node* insert(const K& key, const V& val, Node* subtree_root);					

	// helper functions to perform a single right rotation
	Node* rotate_right(Node* k2);													

	// helper functions to perform a single left rotation
	Node* rotate_left(Node* k2);													

	// helper to recursively build sorted list of keys
	void inorder(const Node* subtree, std::vector<K>& keys) const;					

	// helper to recursively build sorted list of keys
	void preorder(const Node* subtree, std::vector<K>& keys) const;					

	// helper to recursively find range of keys
	void range_search(const Node* subtree_root, const K& k1, const K& k2,
	std::vector<K>& keys) const;

	// return the height of the tree rooted at subtree_root
	int height(const Node* subtree_root) const;										
};

template <typename K, typename V>
RBTCollection<K,V>::RBTCollection() :
	collection_size(0), root(nullptr)
{ }

template <typename K, typename V>
RBTCollection<K,V>::RBTCollection(const RBTCollection<K,V>& rhs) : collection_size(0), root(nullptr) {
	*this = rhs;
}

template <typename K, typename V>
RBTCollection<K,V>& RBTCollection<K,V>::operator=(const RBTCollection<K,V>& rhs) {
	if (this == &rhs) {
		return *this;
	}
	
	// delete current
	make_empty(root);
	//build tree
	std::vector<K> ks;
	preorder(rhs.root, ks);
	for (int i = 0; i < ks.size(); i++) {
		V tmp;
		find(ks[i], tmp);
		insert(ks[i], tmp);
	}
	return *this;
}

template <typename K, typename V>
RBTCollection<K,V>::~RBTCollection() {
	make_empty(root);
}

template <typename K, typename V>
void RBTCollection<K,V>::insert(const K& key, const V& val) {
	root = insert(key, val, root);
	root -> is_black = true;
	collection_size++;
}

template <typename K, typename V>
bool RBTCollection<K,V>::find(const K& key, V& val) const {
	Node* ptr = root;
	if (collection_size == 0) {
		return false;
	}
	while (ptr != nullptr) {
		if (ptr -> key == key) {
			val = ptr -> value;
			return true;
		} else if (ptr -> key > key) {
			ptr = ptr -> left;
		} else if (ptr -> key < key) {
			ptr = ptr -> right;
		}
	}
	return false;
}

template <typename K, typename V> void
RBTCollection<K,V>::find(const K& k1, const K& k2, std::vector<K>& keys) const {
	range_search(root, k1, k2, keys);
}

template <typename K, typename V>
void RBTCollection<K,V>::keys(std::vector<K>& ks) const {
	inorder(root, ks);
}

template <typename K, typename V>
void RBTCollection<K,V>::sort(std::vector<K>& ks) const {
	inorder(root, ks);
}

template <typename K, typename V>
int RBTCollection<K,V>::size() const {
	return collection_size;
}


template <typename K, typename V>
int RBTCollection<K,V>::height() const {
	return height(root);
}


template <typename K, typename V>
void RBTCollection<K,V>::make_empty(Node* subtree_root) {
	if (subtree_root == nullptr) {
		return;
	}
	make_empty(subtree_root -> left);
	make_empty(subtree_root -> right);
	delete subtree_root;
	collection_size = 0;
	root = nullptr;
}

template <typename K, typename V>
typename RBTCollection<K,V>::Node*
RBTCollection<K,V>::insert(const K& key, const V& val, Node* subtree_root) {
	// Recursive BST Insertion
	if (subtree_root == nullptr) {
		Node* ptr;
		ptr = new Node;				// new node to be inserted
		ptr -> key = key;
		ptr -> value = val;
		ptr -> is_black = false;
		ptr -> left = nullptr;
		ptr -> right = nullptr;
		ptr -> is_dbl_black_left = ptr -> is_dbl_black_right = false;
		return ptr;
	}
	if (key < subtree_root -> key) {
		subtree_root -> left = insert(key, val, subtree_root -> left);
	} else if (key > subtree_root -> key) {
		subtree_root -> right = insert(key, val, subtree_root -> right);
	}
	
	// Backtracking/Rebalancing
	if (subtree_root -> is_black) {	// Base Condtion: g is black
		if ( (subtree_root -> left != nullptr) and (subtree_root -> right != nullptr) and !(subtree_root -> left -> is_black) and !(subtree_root -> right -> is_black) ) {									// color-swap base condition
			if (subtree_root -> left -> left != nullptr and !(subtree_root -> left -> left -> is_black)) {																									// grandchild loc 1
				subtree_root -> is_black = false;																
				subtree_root -> left -> is_black = true;
				subtree_root -> right -> is_black = true;
			} else if (subtree_root -> left -> right != nullptr and !(subtree_root -> left -> right -> is_black)) {																							// grandchild loc 2
				subtree_root -> is_black = false;																
				subtree_root -> left -> is_black = true;
				subtree_root -> right -> is_black = true;
			} else if (subtree_root -> right -> left != nullptr and !(subtree_root -> right -> left -> is_black)) {																							// grandchild loc 3
				subtree_root -> is_black = false;																
				subtree_root -> left -> is_black = true;
				subtree_root -> right -> is_black = true;
			} else if (subtree_root -> right -> right != nullptr and !(subtree_root -> right -> right -> is_black)) {																						// grandchild loc 4
				subtree_root -> is_black = false;																
				subtree_root -> left -> is_black = true;
				subtree_root -> right -> is_black = true;
			}
		} else if ( (subtree_root -> left != nullptr) and !(subtree_root -> left -> is_black) and (subtree_root -> left -> left != nullptr) and !(subtree_root -> left -> left -> is_black) ) {				// red-red conflict w/ left outside grandchild
			subtree_root -> is_black = false;
			subtree_root -> left -> is_black = true;
			subtree_root = rotate_right(subtree_root);
		} else if ( (subtree_root -> right != nullptr) and !(subtree_root -> right -> is_black) and (subtree_root -> right -> right != nullptr) and !(subtree_root -> right -> right -> is_black) ) {		// red-red conflict w/ right outside grandchild
			subtree_root -> is_black = false;
			subtree_root -> right -> is_black = true;
			subtree_root = rotate_left(subtree_root);
		} else if ( (subtree_root -> left != nullptr) and !(subtree_root -> left -> is_black) and (subtree_root -> left -> right != nullptr) and !(subtree_root -> left -> right -> is_black) ) {			// red-red conflict with left-right inside grandchild
			subtree_root -> is_black = false;
			subtree_root -> left -> right -> is_black = true;
			subtree_root -> left = rotate_left(subtree_root -> left);
			subtree_root = rotate_right(subtree_root);
		} else if ( (subtree_root -> right != nullptr) and !(subtree_root -> right -> is_black) and (subtree_root -> right -> left != nullptr) and !(subtree_root -> right -> left -> is_black) ) {			// red-red conflict with right-left inside grandchild 
			subtree_root -> is_black = false;
			subtree_root -> right -> left -> is_black = true;
			subtree_root -> right = rotate_right(subtree_root -> right);
			subtree_root = rotate_left(subtree_root);
		}
	}
	return subtree_root;
}

template <typename K, typename V>
void RBTCollection<K,V>::remove(const K& key) {
	// check if anything to remove
	if (root == nullptr)
		return;
	// create a "fake" root to pass in as parent of root
	Node* root_parent = new Node;
	root_parent -> key = root -> key;
	root_parent -> left = nullptr;
	root_parent -> right = root;
	root_parent -> is_black = true;
	root_parent -> is_dbl_black_left = false;
	root_parent -> is_dbl_black_right = false;
	// call remove
	bool found = false;
	root_parent = remove(key, root_parent, root, found);
	// update results
	if (found) {
		collection_size--;
		root = root_parent -> right;
		if (root) {
		root -> is_black = true;
		root -> is_dbl_black_right = false;
		root -> is_dbl_black_left = false;
		}
	}
	delete root_parent;
}


template <typename K, typename V>
typename RBTCollection<K,V>::Node*
RBTCollection<K,V>::remove(const K& key, Node* parent, Node* subtree_root, bool& found) {
	if (subtree_root && key < subtree_root -> key)
		subtree_root = remove(key, subtree_root, subtree_root -> left, found);
	else if (subtree_root && key > subtree_root -> key)
		subtree_root = remove(key, subtree_root, subtree_root -> right, found);
	else if (subtree_root && key == subtree_root -> key) {
		found = true;
		// leaf node
		if (!subtree_root -> left && !subtree_root -> right) {
			// if node is black then set double-black, adjust parent,
			// and delete subtree root...	
			if (subtree_root -> is_black) {
				if (parent -> left == subtree_root) {
					parent -> is_dbl_black_left = true;
					parent -> left = subtree_root -> left;
				} else {
					parent -> is_dbl_black_right = true;
					parent -> right = subtree_root -> right;
				}
			} else if (!subtree_root -> is_black) {
				if (parent -> left == subtree_root) {
					parent -> left = subtree_root -> left;
				} else {
					parent -> right = subtree_root -> right;
				}	
			}
			delete subtree_root;
			subtree_root = nullptr;
		}
		// left non-empty but right empty
		else if (subtree_root -> left && !subtree_root -> right) {
			// similar to above
			if (subtree_root -> is_black) {
				if (parent -> left == subtree_root) {
					parent -> is_dbl_black_left = true;
					parent -> left = subtree_root -> left;
					subtree_root -> left = nullptr;
				} else {
					parent -> is_dbl_black_right = true;
					parent -> right = subtree_root -> left;
					subtree_root -> left = nullptr;
				}
			} else if (!subtree_root -> is_black) {
				if (parent -> left == subtree_root) {
					parent -> left = subtree_root -> left;
					subtree_root -> left = nullptr;
				} else {
					parent -> right = subtree_root -> left;
					subtree_root -> left = nullptr;
				}
			}
			delete subtree_root;
			subtree_root = nullptr;
		}
		// left empty but right non-empty
		else if (!subtree_root -> left && subtree_root -> right) {
			// similar to above
			if (subtree_root -> is_black) {
				if (parent -> left == subtree_root) {
					parent -> is_dbl_black_left = true;
					parent -> left = subtree_root -> right;
					subtree_root -> right = nullptr;
				} else {
					parent -> is_dbl_black_right = true;
					parent -> right = subtree_root -> right;
					subtree_root -> right = nullptr;
				}
			} else if (!subtree_root -> is_black) {
				if (parent -> left == subtree_root) {
					parent -> left = subtree_root -> right;
					subtree_root -> right = nullptr;
				} else {
					parent -> right = subtree_root -> right;
					subtree_root -> right = nullptr;
				}
			}
			delete subtree_root;
			subtree_root = nullptr;
		}
		// left and right non empty
		else {
			// find inorder successor (right, then iterate left)
			// then call remove again on inorder successor key and subtree root’s
			// right child once the key and value copy is complete
			Node* ptr = subtree_root -> right;
			while (ptr -> left != nullptr) {
				ptr = ptr -> left;
			}
			subtree_root -> key = ptr -> key;
			subtree_root -> value = ptr -> value;
			subtree_root = remove(ptr -> key, subtree_root, subtree_root -> right, found);
		}
	}
	
	if (!found)
		return parent;

	// backtracking , adjust color at parent
	return remove_color_adjust(parent);
}


template <typename K, typename V>
typename RBTCollection<K,V>::Node*
RBTCollection<K,V>::remove_color_adjust(Node* subtree_root) {
	// subtree root is "grandparent" g, with left child gl and right child gr
	Node* g = subtree_root;
	Node* gl = g -> left;
	Node* gr = g -> right;
	// parent p is either gl or gr
	Node* p = nullptr;
	bool left_parent = false;
	if (gl && (gl -> is_dbl_black_left || gl -> is_dbl_black_right)) {
		p = gl;
		left_parent = true;
	}
	else if (gr && (gr -> is_dbl_black_left || gr -> is_dbl_black_right))
		p = gr ;
	else
		return subtree_root;

	// parent’s left child is a double black node
	if (p -> is_dbl_black_left) {
		// do the following cases
		if (p -> left and !p -> left -> is_black) {								// case 0: left child exists and is red
			p -> is_dbl_black_left = false;
			p -> left -> is_black = true;
		} else if (!p -> right -> is_black) {									// case 1: red sibling
			p = rotate_left(p);													// left-rotation around p
			// recolor
			p -> is_black = true;
			// rebalance at p -> left, same as case 3a
			p -> left -> is_black = true;
			p -> left -> is_dbl_black_left = false;
			p -> left -> right -> is_black = false; 
		} else if (p -> right -> right and !p -> right -> right -> is_black) {	// case 2: black sibling with red child (outside)
			p -> right -> is_black = p -> is_black;
			p -> right -> right -> is_black = true;
			p -> is_black = true;
			p = rotate_left(p);
			p -> left -> is_dbl_black_left = false;
		} else if (p -> right -> left and !p -> right -> left -> is_black) {	// case 2: black sibling with red child (inside)
			p -> right = rotate_right(p -> right);
			p = rotate_left(p);
			p -> is_black = p -> left -> is_black;
			p -> left -> is_black = true;
			p -> left -> is_dbl_black_left = false;
		} else if (!p -> is_black) {											// case 3: red parent, black sibling
				p -> is_black = true;
				p -> is_dbl_black_left = false;
				p -> right -> is_black = false;
		} else {																// case 3: black parent, black sibling
			if (left_parent) {
					g -> is_dbl_black_left = true;
				} else {
					g -> is_dbl_black_right = true;
				}
				p -> is_dbl_black_left = false;
				p -> right -> is_black = false;
		}
	}

	// parent’s right child is a double black node
	if (p -> is_dbl_black_right) {
		// do the following cases
		if (p -> right and !p -> right -> is_black) {
			p -> is_dbl_black_right = false;
			p -> right -> is_black = true;
		} else if (!p -> left -> is_black) {									// case 1: red sibling
			p = rotate_right(p);	// right-rotation around p
			// recolor
			p -> is_black = true;
			// rebalance again at p -> right, same as case3a
			p -> right -> is_black = true;
			p -> right -> is_dbl_black_right = false;
			p -> right -> left -> is_black = false; 
			
		} else if (p -> left -> left and !p -> left -> left -> is_black) {		// case 2: black sibling with red child (outside)
			p -> left -> is_black = p -> is_black;
			p -> left -> left -> is_black = true;
			p -> is_black = true;
			p = rotate_right(p);
			p -> right -> is_dbl_black_right = false;
		} else if (p -> left -> right and !p -> left -> right -> is_black) {	// case 2: black sibling with red child (inside)
			p -> left = rotate_left(p -> left);
			p = rotate_right(p);
			p -> is_black = p -> right -> is_black;
			p -> right -> is_black = true;
			p -> left -> is_black = true;
			p -> right -> is_dbl_black_right = false;
		} else if (!p -> is_black) {											// case 3: red parent, black sibling
				p -> is_black = true;
				p -> is_dbl_black_right = false;
				p -> left -> is_black = false;
		} else {																// case 3: black parent, black sibling
			if (left_parent) {
					g -> is_dbl_black_left = true;
				} else {
					g -> is_dbl_black_right = true;
				}
				p -> is_dbl_black_right = false;
				p -> left -> is_black = false;
		}
	}

	// connect up the subtree_root to the parent
	if (left_parent)
		subtree_root -> left = p;
	else
		subtree_root -> right = p;

	return subtree_root;
}

template <typename K, typename V>
typename RBTCollection<K,V>::Node*
RBTCollection<K,V>::rotate_right(Node* k2) {
	Node* k1 = k2 -> left;
	k2 -> left = k1 -> right;
	k1 -> right = k2;
	return k1;
}

template <typename K, typename V>
typename RBTCollection<K,V>::Node* 
RBTCollection<K,V>::rotate_left(Node* k2) {
	Node* k1 = k2 -> right;
	k2 -> right = k1 -> left;
	k1 -> left = k2;
	return k1;
}

template <typename K, typename V>
void RBTCollection<K,V>::inorder(const Node* subtree, std::vector<K>& ks) const {
	if (subtree == nullptr) {
		return;
	}
	inorder(subtree -> left, ks);
	ks.push_back(subtree -> key);
	inorder(subtree -> right, ks);
}
	

template <typename K, typename V>
void RBTCollection<K,V>::preorder(const Node* subtree, std::vector<K>& ks) const {
	if (subtree == nullptr) {
		return;
	}
	ks.push_back(subtree->key);		// visit node
	preorder(subtree->left, ks);	// traverse left
	preorder(subtree->right, ks);	// traverse right
}

template <typename K, typename V> void
RBTCollection<K,V>::
range_search(const Node* subtree, const K& k1, const K& k2, std::vector <K>& ks) const {
	if (subtree == nullptr) {
		return;
	}
	if (subtree -> key >= k1 and subtree -> key <= k2) {
		ks.push_back(subtree -> key);
	}
	range_search(subtree -> left, k1, k2, ks);
	range_search(subtree -> right, k1, k2, ks);  
}

template <typename K, typename V>
int RBTCollection<K,V>::height(const Node* subtree_root) const {
	if (subtree_root == nullptr) {
		return 0;
	}
	int left_h = 0;								// height of left side
	int right_h = 0;							// height of right side
	left_h = height(subtree_root -> left);
	right_h = height(subtree_root -> right);
	if (left_h > right_h) {
		return left_h + 1;
	} else {
		return right_h + 1;
	}
}

#endif