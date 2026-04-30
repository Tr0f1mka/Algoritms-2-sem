#define _CRT_SECURE_NO_WARNINGS
#define MAX_LEN 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum { Black, Red } Color;

typedef struct Node {
	char key[7];
	double data;
	Color color;
	struct Node* left, * right, * parent;
} Node;

typedef struct {
	Node* root;
	Node* nil;
} RBTree;


void ShowTree(RBTree* tree);


RBTree* create_tree() {
	RBTree* tree = (RBTree*)malloc(sizeof(RBTree));
	if (tree == NULL) return NULL;

	tree->nil = (Node*)malloc(sizeof(Node));
	if (tree->nil == NULL) {
		free(tree);
		return NULL;
	}

	strcpy(tree->nil->key, "");
	tree->nil->data = 0;
	tree->nil->color = Black;
	tree->nil->left = tree->nil;   // важно: указывает на себя
	tree->nil->right = tree->nil;  // важно: указывает на себя
	tree->nil->parent = tree->nil; // важно: указывает на себя

	tree->root = tree->nil;
	return tree;
}


Node* create_node(char* key, double data, Node* nil) {

	if (key == NULL) return NULL;

	Node* new_node = (Node*)malloc(sizeof(Node));
	if (new_node == NULL) return NULL;
	strncpy(new_node->key, key, sizeof(new_node->key) - 1);
	new_node->key[sizeof(new_node->key) - 1] = '\0';
	new_node->data = data;
	new_node->color = Red;
	new_node->left = nil;
	new_node->right = nil;
	new_node->parent = nil;

	return new_node;
}


void left_rotate(RBTree* tree, Node* node) {
	if (tree == NULL || node == NULL || node->right == tree->nil) return;

	Node* buff = node->right;
	node->right = buff->left;

	if (buff->left != tree->nil) {
		buff->left->parent = node;
	}

	buff->parent = node->parent;

	if (node->parent == tree->nil) {
		tree->root = buff;
	}
	else if (node == node->parent->left) {
		node->parent->left = buff;
	}
	else {
		node->parent->right = buff;
	}

	buff->left = node;
	node->parent = buff;
}


void right_rotate(RBTree* tree, Node* node) {
	if (tree == NULL || node == NULL || node->left == tree->nil) return;

	Node* buff = node->left;
	node->left = buff->right;

	if (buff->right != tree->nil) {
		buff->right->parent = node;
	}

	buff->parent = node->parent;

	if (node->parent == tree->nil) {
		tree->root = buff;
	}
	else if (node == node->parent->left) {
		node->parent->left = buff;
	}
	else {
		node->parent->right = buff;
	}

	buff->right = node;
	node->parent = buff;
}


void insert_balance(RBTree* tree, Node* z) {
	if (tree == NULL || z == NULL) return;

	while (z != tree->root && z->parent->color == Red) {
		if (z->parent == z->parent->parent->left) {
			Node* y = z->parent->parent->right;

			if (y != tree->nil && y->color == Red) {
				z->parent->color = Black;
				y->color = Black;
				z->parent->parent->color = Red;
				z = z->parent->parent;
			}
			else {
				if (z == z->parent->right) {
					z = z->parent;
					left_rotate(tree, z);
				}
				z->parent->color = Black;
				z->parent->parent->color = Red;
				right_rotate(tree, z->parent->parent);
				break;
			}
		}
		else {
			Node* y = z->parent->parent->left;

			if (y != tree->nil && y->color == Red) {
				z->parent->color = Black;
				y->color = Black;
				z->parent->parent->color = Red;
				z = z->parent->parent;
			}
			else {
				if (z == z->parent->left) {
					z = z->parent;
					right_rotate(tree, z);
				}
				z->parent->color = Black;
				z->parent->parent->color = Red;
				left_rotate(tree, z->parent->parent);
				break;
			}
		}
	}
	tree->root->color = Black;
}


void insert(RBTree* tree, char* key, double data) {
	if (tree == NULL || key == NULL) return;

	Node* new_node = create_node(key, data, tree->nil);
	if (new_node == NULL) return;

	Node* parent = tree->nil;
	Node* curent = tree->root;

	while (curent != tree->nil) {
		parent = curent;
		if (strcmp(key, curent->key) < 0) {
			curent = curent->left;
		}
		else {
			curent = curent->right;
		}
	}

	new_node->parent = parent;

	if (parent == tree->nil) {
		tree->root = new_node;
		new_node->color = Black;
		return;
	}
	else if (strcmp(key, parent->key) < 0) {
		parent->left = new_node;
	}
	else {
		parent->right = new_node;
	}

	insert_balance(tree, new_node);
}


Node* search(RBTree* tree, char* key) {
	if (tree == NULL || key == NULL || tree->root == tree->nil) return NULL;

	Node* result = tree->root;

	while (result != tree->nil) {
		int cmp = strcmp(key, result->key);
		if (cmp == 0) {
			return result;
		}
		else if (cmp < 0) {
			result = result->left;
		}
		else {
			result = result->right;
		}
	}

	return NULL;
}


Node* local_minimum(RBTree* tree, Node* node) {
	if (tree == NULL || node == NULL || node == tree->nil) return NULL;
	while (node->left != tree->nil) node = node->left;
	return node;
}


void replace_sub_tree(RBTree* tree, Node* u, Node* v) {
	if (u->parent == tree->nil)
		tree->root = v;
	else if (u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	v->parent = u->parent;
}	


void delete_balance(RBTree* tree, Node* x) {
	while (x != tree->root && x->color == Black) {
		if (x == x->parent->left) {
			Node* w = x->parent->right;
			if (w->color == Red) {
				w->color = Black;
				x->parent->color = Red;
				left_rotate(tree, x->parent);
				w = x->parent->right;
			}
			if (w->left->color == Black && w->right->color == Black) {
				w->color = Red;
				x = x->parent;
			}
			else {
				if (w->right->color == Black) {
					w->left->color = Black;
					w->color = Red;
					right_rotate(tree, w);
					w = x->parent->right;
				}
				w->color = x->parent->color;
				x->parent->color = Black;
				w->right->color = Black;
				left_rotate(tree, x->parent);
				x = tree->root;
			}
		}
		else {
			Node* w = x->parent->left;
			if (w->color == Red) {
				w->color = Black;
				x->parent->color = Red;
				right_rotate(tree, x->parent);
				w = x->parent->left;
			}
			if (w->right->color == Black && w->left->color == Black) {
				w->color = Red;
				x = x->parent;
			}
			else {
				if (w->left->color == Black) {
					w->right->color = Black;
					w->color = Red;
					left_rotate(tree, w);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = Black;
				w->left->color = Black;
				right_rotate(tree, x->parent);
				x = tree->root;
			}
		}
	}
	x->color = Black;
}


void delete_node(RBTree* tree, char* key) {
	Node* z = search(tree, key);
	if (z == NULL || z == tree->nil) return;

	Node* y = z;
	Node* x;
	Color y_original_color = y->color;

	if (z->left == tree->nil) {
		x = z->right;
		replace_sub_tree(tree, z, z->right);
	}
	else if (z->right == tree->nil) {	
		x = z->left;
		replace_sub_tree(tree, z, z->left);
	}
	else {
		y = local_minimum(tree, z->right);
		y_original_color = y->color;
		x = y->right;
		if (y->parent == z) {
			x->parent = y;
		}
		else {
			replace_sub_tree(tree, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}
		replace_sub_tree(tree, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	free(z);
	if (y_original_color == Black) {
		delete_balance(tree, x);
	}
}


void ShowNode(Node* node, Node* nil, int tab, FILE* output) {
	if (node == nil || output == NULL) return;

	for (int i = 0; i < tab; i++) {
		fprintf(output, "  ");
	}
	
	char parent_key[10];
	if (node->parent == nil || node->parent == NULL) {
		strcpy(parent_key, "NULL");
	}
	else {
		strcpy(parent_key, node->parent->key);
	}
	fprintf(output, "%s(%.4f,%s, %s)\n", node->key, node->data,
		node->color == Red ? "R" : "B", parent_key);
	ShowNode(node->left, nil, tab + 1, output);
	ShowNode(node->right, nil, tab + 1, output);
}


void ShowTree(RBTree* tree, FILE* output) {
	if (output == NULL) return;
	if (tree == NULL || tree->root == tree->nil) {
		fprintf(output, "Tree is empty\n");
		return;
	}
	ShowNode(tree->root, tree->nil, 0, output);
}


void free_subtree(Node* node, Node* nil) {
	if (node == NULL || node == nil) return;
	free_subtree(node->left, nil);
	free_subtree(node->right, nil);
	free(node);
}


void free_tree(RBTree* tree) {
	if (tree == NULL) return;

	free_subtree(tree->root, tree->nil);
	free(tree->nil);
	free(tree);
}


void RunFile(char* file, char* output_file) {
	FILE* f = fopen(file, "r");
	if (f == NULL) return;
	FILE* output = fopen(output_file, "w");
	if (output == NULL) {
		fclose(f);
		return;
	}

	RBTree* tree = create_tree();
	if (tree == NULL) {
		fclose(f);
		return;
	}

	char line[MAX_LEN];

	while (fgets(line, sizeof(line), f)) {
		char* token = strtok(line, "\t\n"), *token1;
		int choice = atoi(token);

		switch (choice) {
		case 1:
			token = strtok(NULL, "\t\n");
			token1 = strtok(NULL, "\t\n");
			double data = atof(token1);
			fprintf(output, "INSERTING ELEM: KEY: %s DATA: %.4lf\n", token, data);
			insert(tree, token, data);
			break;
		case 2:
			token = strtok(NULL, "\t\n");
			fprintf(output, "DELETING ELEM: KEY: %s\n", token);
			delete_node(tree, token);
			break;
		case 3:
			fprintf(output, "PRINT TREE\n");
			ShowTree(tree, output);
			break;
		case 4:
			token = strtok(NULL, "\t\n");
			fprintf(output, "SEARCHING ELEM: KEY: %s\n", token);
			Node* node = search(tree, token);
			if (node == NULL) {
				fprintf(output, "ELEM WITH KEY %s NON-DEFINED\n", token);
			}
			else {
				fprintf(output, "%s (%lf %s)\n", node->key, node->data, node->color == Red ? "R" : "B");
			}
			break;
		default:
			break;
		}
		fprintf(output, "\n\n");
	}

	free_tree(tree);
	fclose(f);
	fclose(output);
}


int main() {
	RunFile("test.txt", "output.txt");

	return 0;
}