#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


typedef struct a
{
    int data;
    struct a** nexts;
    int size;
    int max_size;
} Node;

typedef struct
{
    Node* head;
} Tree;


Node* InitElem(int data) {
    Node* a = (Node*)malloc(sizeof(Node));
    if (a == NULL) {
        return NULL;
    }
    a->data = data;
    a->size = 0;
    a->max_size = 5;
    a->nexts = (Node**)malloc(sizeof(Node) * 5);
    if (a->nexts == NULL) {
        return NULL;
    }
    return a;
}

void Clear(Node* node) {
    if (node == NULL) {
        return;
    }
    if (node->size > 0) {
        for (int i = 0; i < node->size; i++) {
            Clear(node->nexts[i]);
        }
    }
    free(node);
    node = NULL;
}

Node* FindElem(Node* cur, int target) {
    if (cur == NULL) {
        return NULL;
    }
    if (cur->data == target) {
        return cur;
    }
    for (int i = 0; i < cur->size; ++i) {
        Node* a = FindElem(cur->nexts[i], target);
        if (a != NULL) {
            return a;
        }
    }
    return NULL;
}

Node* FindParent(Node* cur, int target, Node** parent) {
    if (cur == NULL) return NULL;

    for (int i = 0; i < cur->size; i++) {
        if (cur->nexts[i] != NULL && cur->nexts[i]->data == target) {
            *parent = cur;
            return cur->nexts[i];
        }
        Node* found = FindParent(cur->nexts[i], target, parent);
        if (found != NULL) return found;
    }
    return NULL;
}

void Add(Tree* tree, int parent, int son) {
    if (tree == NULL) {
        printf("ERROR: tree is not defined\n");
        return;
    }

    if (parent == NULL) {
        Node* a = InitElem(son);
        if (a == NULL) {
            printf("ERROR: not enough memory\n");
            return;
        }
        if (tree->head != NULL) {
            a->size = 1;
        }
        a->nexts[0] = tree->head;
        tree->head = a;
        return;
    }

    Node* father = FindElem(tree->head, parent);
    if (father == NULL) {
        printf("ERROR: element \"%d\" is not found\n", parent);
        return;
    }

    Node* elem = InitElem(son);
    if (elem == NULL) {
        printf("ERROR: not enough memory\n");
        return;
    }
    if (father->size > -father->max_size) {
        father->max_size *= 2;
        father->nexts = realloc(father->nexts, sizeof(Node*) * father->max_size);
    }
    father->nexts[father->size] = elem;
    father->size++;
}

void ShowNode(Node* node, int tab) {
    for (int i = 0; i < tab; i++) {
        printf("  ");
    }
    printf("%d\n", node->data);
    for (int i = 0; i < node->size; i++) {
        ShowNode(node->nexts[i], tab + 1);
    }
}

void ShowTree(Tree* tree) {
    if (tree == NULL || tree->head == NULL) {
        printf("Tree is empty\n");
        return;
    }
    ShowNode(tree->head, 0);
}

void Delete(Tree* tree, int target) {
    if (tree == NULL) {
        printf("ERROR: tree is not defined\n");
        return;
    }
    if (tree->head == NULL) {
        printf("ERROR: tree is empty\n");
        return;
    }

    if (tree->head->data == target) {
        Clear(tree->head);
        tree->head = NULL;
        printf("Root %d deleted\n", target);
        return;
    }

    Node* parent = NULL;
    Node* target_node = FindParent(tree->head, target, &parent);

    if (target_node == NULL) {
        printf("ERROR: element \"%d\" is not found\n", target);
        return;
    }

    for (int i = 0; i < parent->size; i++) {
        if (parent->nexts[i] == target_node) {
            for (int j = i; j < parent->size - 1; j++) {
                parent->nexts[j] = parent->nexts[j + 1];
            }
            parent->size--;
            break;
        }
    }

    Clear(target_node);
    printf("Element %d and its subtree deleted\n", target);
}


int CalcWeight(Node** nodes, int len, int max_len, int max) {
    if (max == -1) {
        return -1;
    }
    int cur_weight = 0;
    Node** new_nodes = (Node**)malloc(sizeof(Node*) * max_len);
    if (new_nodes == NULL) return -1;
    int cur = 0;
    for (int i = 0; i < len; i++) {
        cur_weight += nodes[i]->size;
        for (int j = 0; j < nodes[i]->size; j++) {
            if (cur == max_len) {
                max_len <<= 1;
                new_nodes = realloc(new_nodes, sizeof(Node*) * max_len);
            }
            new_nodes[cur] = nodes[i]->nexts[j];
            cur++;
        }
    }
    if (cur_weight > max) {
        max = cur_weight;
    }
    if (cur_weight > 0) {
        int t = CalcWeight(new_nodes, cur, max_len, max);
        if (t > max) {
            max = t;
        }
    }
    free(new_nodes);
    return max;
}

void Weight(Tree* tree) {
    if (tree == NULL || tree->head == NULL) {
        printf("0\n");
        return;
    }
    Node** nodes = (Node**)malloc(sizeof(Node*) * 10);
    if (nodes == NULL) {
        printf("ERROR: not enough memory\n");
        return;
    }
    nodes[0] = tree->head;
    int max = CalcWeight(nodes, 1, 10, 1);
    free(nodes);
    if (max == -1) {
        printf("ERROR: not enough memory\n");
        return;
    }
    printf("%d\n", max);
}

void menu() {
    int choice;
    Tree tree;
    tree.head = NULL;
    int parent = NULL, son, is_parent;

    do {
        printf("1. Add element\n");
        printf("2. Remove element\n");
        printf("3. Show tree\n");
        printf("4. Show weight of tree\n");
        printf("5. Exit\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            printf("Parent?: ");
            scanf("%d", &is_parent);
            if (is_parent) {
                printf("Parent: ");
                scanf("%d", &parent);
            }
            else {
                parent = NULL;
            }
            printf("Son: ");
            scanf("%d", &son);
            Add(&tree, parent, son);
            break;

        case 2:
            printf("Element: ");
            scanf("%d", &son);
            Delete(&tree, son);
            break;

        case 3:
            ShowTree(&tree);
            break;

        case 4:
            Weight(&tree);
            break;

        case 5:
            break;

        default:
            printf("ERROR: incorrect input/n");
            break;
        }
    } while (choice != 5);
    Clear(tree.head);
}

int main() {
    menu();
    return 0;
}