#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
#endif

// Узел дерева общего вида (первый сын, следующий брат)
typedef struct GeneralNode {
    int data;
    struct GeneralNode* firstChild;
    struct GeneralNode* nextSibling;
} GeneralNode;

// Структура для очереди (обход в ширину)
typedef struct QueueNode {
    GeneralNode* treeNode;
    int level;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
} Queue;

// Инициализация очереди
void initQueue(Queue* q) {
    q->front = q->rear = NULL;
}

// Проверка пустоты очереди
int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

// Добавление в очередь
void enqueue(Queue* q, GeneralNode* node, int level) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (!newNode) return;
    newNode->treeNode = node;
    newNode->level = level;
    newNode->next = NULL;
    
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// Извлечение из очереди
GeneralNode* dequeue(Queue* q, int* level) {
    if (isQueueEmpty(q)) return NULL;
    QueueNode* temp = q->front;
    GeneralNode* node = temp->treeNode;
    *level = temp->level;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    return node;
}

// Очистка очереди
void freeQueue(Queue* q) {
    int level;
    while (!isQueueEmpty(q)) {
        dequeue(q, &level);
    }
}

// Создание нового узла
GeneralNode* createNode(int value) {
    GeneralNode* newNode = (GeneralNode*)malloc(sizeof(GeneralNode));
    if (!newNode) return NULL;
    newNode->data = value;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;
    return newNode;
}

// Поиск узла по значению (первое вхождение)
GeneralNode* findNode(GeneralNode* root, int value) {
    if (root == NULL) return NULL;
    if (root->data == value) return root;
    
    GeneralNode* found = findNode(root->firstChild, value);
    if (found) return found;
    
    return findNode(root->nextSibling, value);
}

// Добавление узла (самым младшим сыном к указанному отцу)
int addNode(GeneralNode* root, int parentValue, int childValue) {
    GeneralNode* parent = findNode(root, parentValue);
    if (parent == NULL) {
        printf("Ошибка: узел со значением %d не найден!\n", parentValue);
        return 0;
    }
    
    GeneralNode* newNode = createNode(childValue);
    if (!newNode) return 0;
    
    if (parent->firstChild == NULL) {
        parent->firstChild = newNode;
    } else {
        GeneralNode* temp = parent->firstChild;
        while (temp->nextSibling != NULL) {
            temp = temp->nextSibling;
        }
        temp->nextSibling = newNode;
    }
    return 1;
}

// Удаление поддерева
void deleteSubtree(GeneralNode* node) {
    if (node == NULL) return;
    deleteSubtree(node->firstChild);
    deleteSubtree(node->nextSibling);
    free(node);
}

// Удаление узла и всего его поддерева
int deleteNode(GeneralNode** root, int value) {
    if (*root == NULL) return 0;
    
    // Если удаляем корень
    if ((*root)->data == value) {
        deleteSubtree(*root);
        *root = NULL;
        return 1;
    }
    
    // Поиск и удаление среди сыновей
    GeneralNode* parent = *root;
    GeneralNode* toDelete = NULL;
    GeneralNode* prev = NULL;
    
    // Рекурсивный поиск родителя
    GeneralNode* findParent(GeneralNode* node, int val, GeneralNode** par, GeneralNode** prevSib) {
        if (node == NULL) return NULL;
        
        GeneralNode* child = node->firstChild;
        *prevSib = NULL;
        while (child != NULL) {
            if (child->data == val) {
                *par = node;
                return child;
            }
            GeneralNode* found = findParent(child, val, par, prevSib);
            if (found) return found;
            *prevSib = child;
            child = child->nextSibling;
        }
        return NULL;
    }
    
    GeneralNode* par = NULL;
    GeneralNode* prevSib = NULL;
    toDelete = findParent(*root, value, &par, &prevSib);
    
    if (toDelete == NULL) return 0;
    
    if (prevSib == NULL) {
        par->firstChild = toDelete->nextSibling;
    } else {
        prevSib->nextSibling = toDelete->nextSibling;
    }
    
    deleteSubtree(toDelete);
    return 1;
}

// Текстовая визуализация
void printTree(GeneralNode* root, int level) {
    if (root == NULL) return;
    
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    printf("%d\n", root->data);
    
    printTree(root->firstChild, level + 1);
    printTree(root->nextSibling, level);
}

// Функция 10: Проверить монотонность возрастания ширины уровня
// Возвращает: 1 - монотонно возрастает, 0 - не возрастает
int isWidthMonotonicIncreasing(GeneralNode* root) {
    if (root == NULL) {
        printf("Дерево пусто\n");
        return 0;
    }
    
    Queue q;
    initQueue(&q);
    enqueue(&q, root, 0);
    
    int currentLevel = 0;
    int currentWidth = 0;
    int prevWidth = -1;
    int levelCounts[1000] = {0};  // Максимум 1000 уровней
    int maxLevel = 0;
    
    // Обход в ширину для подсчета ширины каждого уровня
    while (!isQueueEmpty(&q)) {
        int level;
        GeneralNode* node = dequeue(&q, &level);
        
        if (level > maxLevel) maxLevel = level;
        levelCounts[level]++;
        
        // Добавляем всех сыновей в очередь
        GeneralNode* child = node->firstChild;
        while (child != NULL) {
            enqueue(&q, child, level + 1);
            child = child->nextSibling;
        }
    }
    
    // Проверка монотонного возрастания
    printf("\nШирина по уровням:\n");
    for (int i = 0; i <= maxLevel; i++) {
        printf("Уровень %d: %d узлов\n", i, levelCounts[i]);
        
        if (prevWidth != -1) {
            if (levelCounts[i] <= prevWidth) {
                printf("Нарушение: ширина уровня %d (%d) <= ширины уровня %d (%d)\n",
                       i, levelCounts[i], i-1, prevWidth);
                freeQueue(&q);
                return 0;
            }
        }
        prevWidth = levelCounts[i];
    }
    
    freeQueue(&q);
    return 1;
}

// Освобождение всего дерева
void freeTree(GeneralNode* root) {
    if (root == NULL) return;
    freeTree(root->firstChild);
    freeTree(root->nextSibling);
    free(root);
}

// Вспомогательная функция для ручного ввода дерева
GeneralNode* createTreeManually() {
    int rootVal;
    printf("Введите значение корня: ");
    scanf("%d", &rootVal);
    
    GeneralNode* root = createNode(rootVal);
    if (!root) return NULL;
    
    int choice, parentVal, childVal;
    while (1) {
        printf("\n1 - Добавить узел\n");
        printf("2 - Закончить создание\n");
        printf("Выбор: ");
        scanf("%d", &choice);
        
        if (choice == 2) break;
        if (choice == 1) {
            printf("Введите значение родителя: ");
            scanf("%d", &parentVal);
            printf("Введите значение нового узла: ");
            scanf("%d", &childVal);
            addNode(root, parentVal, childVal);
        }
    }
    return root;
}

// Главное меню
int main() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    
    GeneralNode* root = NULL;
    int choice, val, parentVal;
    
    printf("=== Программа для работы с деревом общего вида ===\n");
    printf("Узлы содержат целые числа.\n");
    printf("Функция №10: Проверка монотонности возрастания ширины уровня.\n\n");
    
    // Создание начального дерева
    printf("Создание дерева:\n");
    root = createTreeManually();
    
    if (root == NULL) {
        printf("Ошибка создания дерева!\n");
        return 1;
    }
    
    while (1) {
        printf("\n==================== МЕНЮ ====================\n");
        printf("1. Добавить новый узел (самым младшим сыном)\n");
        printf("2. Показать дерево (текстовая визуализация)\n");
        printf("3. Удалить узел (вместе с поддеревом)\n");
        printf("4. Проверить монотонность возрастания ширины уровня (Функция №10)\n");
        printf("5. Завершить работу\n");
        printf("==============================================\n");
        printf("Ваш выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода!\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1:  // Добавление узла
                printf("Введите значение родительского узла: ");
                scanf("%d", &parentVal);
                printf("Введите значение нового узла: ");
                scanf("%d", &val);
                if (addNode(root, parentVal, val)) {
                    printf("Узел %d добавлен как сын узла %d\n", val, parentVal);
                }
                break;
                
            case 2:  // Визуализация
                printf("\nДерево:\n");
                printTree(root, 0);
                break;
                
            case 3:  // Удаление узла
                printf("Введите значение удаляемого узла: ");
                scanf("%d", &val);
                if (deleteNode(&root, val)) {
                    printf("Узел %d и его поддерево удалены\n", val);
                    if (root == NULL) {
                        printf("Дерево стало пустым!\n");
                    }
                } else {
                    printf("Узел %d не найден!\n", val);
                }
                break;
                
            case 4:  // Функция №10
                if (root == NULL) {
                    printf("Дерево пусто!\n");
                } else {
                    if (isWidthMonotonicIncreasing(root)) {
                        printf("\nРЕЗУЛЬТАТ: Ширина уровней МОНОТОННО ВОЗРАСТАЕТ.\n");
                    } else {
                        printf("\nРЕЗУЛЬТАТ: Ширина уровней НЕ ВОЗРАСТАЕТ монотонно.\n");
                    }
                }
                break;
                
            case 5:  // Выход
                freeTree(root);
                printf("До свидания!\n");
                return 0;
                
            default:
                printf("Неверный выбор! Введите число от 1 до 5.\n");
        }
    }
    
    return 0;
}
