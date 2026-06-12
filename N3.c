#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
#endif

// Типы узлов дерева выражения
typedef enum {
    NODE_NUMBER,      // число
    NODE_VARIABLE,    // переменная
    NODE_OPERATOR     // оператор (+, -, *, /, ^)
} NodeType;

// Структура узла дерева
typedef struct ExprNode {
    NodeType type;
    union {
        double number;      // для NODE_NUMBER
        char variable;      // для NODE_VARIABLE
        char operator;      // для NODE_OPERATOR (+, -, *, /, ^)
    } value;
    struct ExprNode* left;
    struct ExprNode* right;
} ExprNode;

// Создание узла-числа
ExprNode* createNumberNode(double num) {
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    node->type = NODE_NUMBER;
    node->value.number = num;
    node->left = node->right = NULL;
    return node;
}

// Создание узла-переменной
ExprNode* createVariableNode(char var) {
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    node->type = NODE_VARIABLE;
    node->value.variable = var;
    node->left = node->right = NULL;
    return node;
}

// Создание узла-оператора
ExprNode* createOperatorNode(char op, ExprNode* left, ExprNode* right) {
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    node->type = NODE_OPERATOR;
    node->value.operator = op;
    node->left = left;
    node->right = right;
    return node;
}

// Проверка, является ли узел числом
int isNumber(ExprNode* node) {
    return node != NULL && node->type == NODE_NUMBER;
}

// Проверка, является ли узел переменной
int isVariable(ExprNode* node) {
    return node != NULL && node->type == NODE_VARIABLE;
}

// Проверка, является ли узел оператором
int isOperator(ExprNode* node, char op) {
    return node != NULL && node->type == NODE_OPERATOR && node->value.operator == op;
}

// Копирование дерева
ExprNode* copyTree(ExprNode* root) {
    if (root == NULL) return NULL;
    
    ExprNode* newNode = (ExprNode*)malloc(sizeof(ExprNode));
    newNode->type = root->type;
    newNode->left = copyTree(root->left);
    newNode->right = copyTree(root->right);
    
    if (root->type == NODE_NUMBER) {
        newNode->value.number = root->value.number;
    } else if (root->type == NODE_VARIABLE) {
        newNode->value.variable = root->value.variable;
    } else {
        newNode->value.operator = root->value.operator;
    }
    
    return newNode;
}

// a * (b - c)  →  a * b - a * c
// Также работает для (b - c) * a
ExprNode* transformMulByDiff(ExprNode* node) {
    if (node == NULL) return NULL;
    
    // Сначала рекурсивно преобразуем поддеревья
    node->left = transformMulByDiff(node->left);
    node->right = transformMulByDiff(node->right);
    
    // Ищем умножение
    if (isOperator(node, '*')) {
        ExprNode* left = node->left;
        ExprNode* right = node->right;
        
        // Случай 1: a * (b - c)
        if (isOperator(right, '-')) {
            ExprNode* a = left;
            ExprNode* b = right->left;
            ExprNode* c = right->right;
            
            // Создаём a * b - a * c
            ExprNode* a_b = createOperatorNode('*', copyTree(a), copyTree(b));
            ExprNode* a_c = createOperatorNode('*', copyTree(a), copyTree(c));
            ExprNode* result = createOperatorNode('-', a_b, a_c);
            
            free(node);
            return result;
        }
        
        // Случай 2: (b - c) * a
        if (isOperator(left, '-')) {
            ExprNode* b = left->left;
            ExprNode* c = left->right;
            ExprNode* a = right;
            
            // Создаём b * a - c * a
            ExprNode* b_a = createOperatorNode('*', copyTree(b), copyTree(a));
            ExprNode* c_a = createOperatorNode('*', copyTree(c), copyTree(a));
            ExprNode* result = createOperatorNode('-', b_a, c_a);
            
            free(node);
            return result;
        }
    }
    
    return node;
}

// Глобальное преобразование (обрабатывает все вхождения)
ExprNode* applyTransformation(ExprNode* root) {
    return transformMulByDiff(root);
}

// Печать выражения (инфиксная запись с необходимыми скобками)
void printExpression(ExprNode* node, int parentPrecedence) {
    if (node == NULL) return;
    
    int currentPrecedence = 0;
    int needParentheses = 0;
    
    if (node->type == NODE_NUMBER) {
        if (node->value.number == (int)node->value.number) {
            printf("%d", (int)node->value.number);
        } else {
            printf("%g", node->value.number);
        }
        return;
    }
    
    if (node->type == NODE_VARIABLE) {
        printf("%c", node->value.variable);
        return;
    }
    
    // Определяем приоритет оператора
    switch (node->value.operator) {
        case '+': currentPrecedence = 1; break;
        case '-': currentPrecedence = 1; break;
        case '*': currentPrecedence = 2; break;
        case '/': currentPrecedence = 2; break;
        case '^': currentPrecedence = 3; break;
        default: currentPrecedence = 0;
    }
    
    needParentheses = (parentPrecedence > currentPrecedence) ||
                      (parentPrecedence == currentPrecedence && 
                       (node->value.operator == '-' || node->value.operator == '/'));
    
    if (needParentheses) printf("(");
    
    printExpression(node->left, currentPrecedence);
    printf(" %c ", node->value.operator);
    printExpression(node->right, currentPrecedence);
    
    if (needParentheses) printf(")");
}

// Печать дерева (с отступами)
void printTree(ExprNode* node, int level) {
    if (node == NULL) {
        for (int i = 0; i < level; i++) printf("  ");
        printf("NULL\n");
        return;
    }
    
    printTree(node->right, level + 1);
    
    for (int i = 0; i < level; i++) printf("  ");
    if (node->type == NODE_NUMBER) {
        if (node->value.number == (int)node->value.number) {
            printf("%d\n", (int)node->value.number);
        } else {
            printf("%g\n", node->value.number);
        }
    } else if (node->type == NODE_VARIABLE) {
        printf("%c\n", node->value.variable);
    } else {
        printf("%c\n", node->value.operator);
    }
    
    printTree(node->left, level + 1);
}

// Освобождение памяти
void freeTree(ExprNode* node) {
    if (node == NULL) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

// Парсер простых выражений (поддерживает +, -, *, /, ^ и скобки)
// Упрощённая версия для демонстрации

const char* exprStr;
int exprPos;

void skipSpaces() {
    while (exprStr[exprPos] == ' ') exprPos++;
}

// Функции парсера
ExprNode* parseExpression();
ExprNode* parseTerm();
ExprNode* parseFactor();
ExprNode* parsePrimary();

ExprNode* parseExpression() {
    ExprNode* node = parseTerm();
    skipSpaces();
    while (exprStr[exprPos] == '+' || exprStr[exprPos] == '-') {
        char op = exprStr[exprPos];
        exprPos++;
        ExprNode* right = parseTerm();
        node = createOperatorNode(op, node, right);
        skipSpaces();
    }
    return node;
}

ExprNode* parseTerm() {
    ExprNode* node = parseFactor();
    skipSpaces();
    while (exprStr[exprPos] == '*' || exprStr[exprPos] == '/') {
        char op = exprStr[exprPos];
        exprPos++;
        ExprNode* right = parseFactor();
        node = createOperatorNode(op, node, right);
        skipSpaces();
    }
    return node;
}

ExprNode* parseFactor() {
    skipSpaces();
    ExprNode* node = parsePrimary();
    skipSpaces();
    if (exprStr[exprPos] == '^') {
        exprPos++;
        ExprNode* right = parseFactor();
        node = createOperatorNode('^', node, right);
    }
    return node;
}

ExprNode* parsePrimary() {
    skipSpaces();
    if (exprStr[exprPos] == '(') {
        exprPos++;
        ExprNode* node = parseExpression();
        skipSpaces();
        if (exprStr[exprPos] == ')') exprPos++;
        return node;
    }
    
    if (isdigit(exprStr[exprPos]) || exprStr[exprPos] == '.') {
        char* end;
        double num = strtod(exprStr + exprPos, &end);
        exprPos = end - exprStr;
        return createNumberNode(num);
    }
    
    if (isalpha(exprStr[exprPos])) {
        char var = exprStr[exprPos];
        exprPos++;
        return createVariableNode(var);
    }
    
    return NULL;
}

ExprNode* parse(const char* str) {
    exprStr = str;
    exprPos = 0;
    return parseExpression();
}

// Главная функция
int main() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    
    printf("=========================================================\n");
    printf("Задание №10: Умножение переменной на разность\n");
    printf("Преобразование: a * (b - c) → a * b - a * c\n");
    printf("=========================================================\n\n");
    
    // Тестовые выражения
    const char* testExpressions[] = {
        "a * (b - c)",
        "(x - y) * z",
        "2 * (a - b)",
        "(a - b) * 3",
        "x * (y - z) + 5",
        "a * (b - c) * d",
        "(a - b) * (c - d)",
        "a * (b - c) + d * (e - f)",
        "2 * a * (b - c)",
        "a + b * (c - d) - e",
        NULL
    };
    
    for (int i = 0; testExpressions[i] != NULL; i++) {
        printf("\n—————————————————————————————————————————————————————————\n");
        printf("Тест %d:\n", i + 1);
        printf("Исходное выражение: %s\n", testExpressions[i]);
        
        ExprNode* root = parse(testExpressions[i]);
        if (root == NULL) {
            printf("Ошибка парсинга!\n");
            continue;
        }
        
        printf("\nДерево исходного выражения:\n");
        printTree(root, 0);
        
        printf("\nИсходное выражение: ");
        printExpression(root, 0);
        printf("\n");
        
        ExprNode* transformed = applyTransformation(root);
        
        printf("\nДерево после преобразования:\n");
        printTree(transformed, 0);
        
        printf("\nРезультат преобразования: ");
        printExpression(transformed, 0);
        printf("\n");
        
        freeTree(transformed);
    }
    
    printf("\n=========================================================\n");
    printf("Интерактивный режим\n");
    printf("Введите выражение (или 'quit' для выхода):\n");
    
    char input[256];
    while (1) {
        printf("\n> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            break;
        }
        
        if (strlen(input) == 0) continue;
        
        ExprNode* root = parse(input);
        if (root == NULL) {
            printf("Ошибка парсинга! Поддерживаются: + - * / ^ ( ) и переменные a-z\n");
            continue;
        }
        
        printf("\nИсходное: ");
        printExpression(root, 0);
        printf("\n");
        
        ExprNode* transformed = applyTransformation(root);
        
        printf("Результат: ");
        printExpression(transformed, 0);
        printf("\n");
        
        freeTree(root);
        freeTree(transformed);
    }
    
    printf("До свидания!\n");
    return 0;
}
