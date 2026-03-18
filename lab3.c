#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum { NUM, VAR, OP } NodeType;

typedef struct Node {
    NodeType type;
    char op;
    int value;
    char var;
    struct Node* left;
    struct Node* right;
} Node;

// Создание узла - число
Node* createNum(double val) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->type = NUM;
    n->value = val;
    n->left = NULL;
    n->right = NULL;
    return n;
}

//Создание узла - переменная
Node* createVar(char c) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->type = VAR;
    n->var = c;
    n->left = NULL;
    n->right = NULL;
    return n;
}

//Создание узла - оператор
Node* createOp(char op, Node* l, Node* r) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->type = OP;
    n->op = op;
    n->left = l;
    n->right = r;
    return n;
}

//Печать выражения по дереву
void printExpr(Node* root) {
    if (!root) return;
    if (root->type == NUM) {
        printf("%d", root->value);
    }
    else if (root->type == VAR) {
        printf("%c", root->var);
    }
    else {
        printf("(");
        printExpr(root->left);
        printf(" %c ", root->op);
        printExpr(root->right);
        printf(")");
    }
}

//Освобождение дерева
void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

//Проверка, является ли узел нулевой константой
int isZero(Node* n) {
    return n && n->type == NUM && n->value == 0;
}

//Упрощение: убираем слагаемые-нули
Node* removeZeroTerms(Node* root) {
    if (!root) return NULL;

    root->left = removeZeroTerms(root->left);
    root->right = removeZeroTerms(root->right);

    if (root->type == OP && root->op == '+') {
        if (isZero(root->left)) {
            Node* tmp = root->right;
            free(root->left);
            free(root);
            return tmp;
        }
        if (isZero(root->right)) {
            Node* tmp = root->left;
            free(root->right);
            free(root);
            return tmp;
        }
    }
    return root;
}

//Парсер
const char* input;
char peek() { 
    while (*input == ' ') input++;
    return *input; 
}
char next() { 
    while (*input == ' ') input++;
    return *input++; 
}

Node* parseExpr();

//Парсинг элементов вида "число", "переменная", "скобки"
Node* parseFactor() {
    if (isdigit(peek())) {
        double val = 0;
        while (isdigit(peek())) {
            val = val * 10 + (next() - '0');
        }
        return createNum(val);
    }
    else if (isalpha(peek())) {
        return createVar(next());
    }
    else if (peek() == '(') {
        next();
        Node* n = parseExpr();
        next(); // ')'
        return n;
    }
    return NULL;
}

//Парсинг множителей/делителей
Node* parseTerm() {
    Node* left = parseFactor();
    while (peek() == '*' || peek() == '/') {
        char op = next();
        Node* right = parseFactor();
        left = createOp(op, left, right);
    }
    return left;
}

//Парсинг слагаемых/вычитаемых
Node* parseExpr() {
    Node* left = parseTerm();
    while (peek() == '+' || peek() == '-') {
        char op = next();
        Node* right = parseTerm();
        left = createOp(op, left, right);
    }
    return left;
}

int main() {
    // Тестовые примеры
    const char* tests[] = {
        "0+a",
        "a+0",
        "a + 12 + 0",
        "0+(b+0)",
        "(a-765)+0",
        "0+0+0",
        "12+0",
        "45-g+5*0/2",
        "56*3-5/8*(0+11+v/x-(a*0))",
        "0+12+0+a+765+ 0-y *y/2"
    };
    int n_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n_tests; i++) {
        printf("INPUT:  %s\n", tests[i]);
        input = tests[i];
        Node* expr = parseExpr();

        expr = removeZeroTerms(expr);

        printf("OUTPUT: ");
        printExpr(expr);
        printf("\n\n");
        freeTree(expr);
    }

    return 0;
}