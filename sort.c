/*
 * ============================================================================
 * Лабораторная работа №26
 * АТД: Линейный список (двунаправленный)
 * Процедура №6: поиск первого от начала элемента, который меньше своего
 *               непосредственного предшественника. Если найден — смещение
 *               его к началу, пока не станет первым или больше предшественника.
 * Метод сортировки: вариант метода вставки
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


// Тип ключа и значения
typedef int key_type;
typedef int value_type;

// Структура элемента данных
typedef struct {
    key_type key;
    value_type value;
} data_type;

// Узел двусвязного списка
typedef struct ListNode {
    data_type data;
    struct ListNode* next;
    struct ListNode* prev;
} ListNode;

// Структура списка (АТД)
typedef struct {
    ListNode* head;
    ListNode* tail;
    size_t size;
} list;


// Создание пустого списка
void list_create(list* l) {
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
}

// Проверка на пустоту
bool list_is_empty(const list* l) {
    return l->size == 0;
}

// Создание нового узла
static ListNode* create_node(data_type data) {
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    if (!node) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return NULL;
    }
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

// Добавление в конец
void list_push_back(list* l, data_type data) {
    ListNode* node = create_node(data);
    if (!node) return;
    
    if (list_is_empty(l)) {
        l->head = l->tail = node;
    } else {
        node->prev = l->tail;
        l->tail->next = node;
        l->tail = node;
    }
    l->size++;
}

// Добавление в начало
void list_push_front(list* l, data_type data) {
    ListNode* node = create_node(data);
    if (!node) return;
    
    if (list_is_empty(l)) {
        l->head = l->tail = node;
    } else {
        node->next = l->head;
        l->head->prev = node;
        l->head = node;
    }
    l->size++;
}

// Удаление из начала
void list_pop_front(list* l) {
    if (list_is_empty(l)) return;
    
    ListNode* temp = l->head;
    l->head = l->head->next;
    if (l->head) l->head->prev = NULL;
    else l->tail = NULL;
    
    free(temp);
    l->size--;
}

// Удаление из конца
void list_pop_back(list* l) {
    if (list_is_empty(l)) return;
    
    ListNode* temp = l->tail;
    l->tail = l->tail->prev;
    if (l->tail) l->tail->next = NULL;
    else l->head = NULL;
    
    free(temp);
    l->size--;
}

// Получение элемента по индексу
data_type* list_get(const list* l, size_t index) {
    if (index >= l->size) return NULL;
    
    ListNode* curr = l->head;
    for (size_t i = 0; i < index; i++) curr = curr->next;
    return &curr->data;
}

// Размер списка
size_t list_size(const list* l) {
    return l->size;
}

// Печать списка
void list_print(const list* l) {
    if (list_is_empty(l)) {
        printf("[]\n");
        return;
    }
    
    printf("[");
    ListNode* curr = l->head;
    while (curr) {
        printf("(%d,%d)", curr->data.key, curr->data.value);
        if (curr->next) printf(" <-> ");
        curr = curr->next;
    }
    printf("], size=%zu\n", l->size);
}

// Очистка списка
void list_clear(list* l) {
    while (!list_is_empty(l)) list_pop_front(l);
}

// Поиск узла по ключу
ListNode* list_find(const list* l, key_type key) {
    ListNode* curr = l->head;
    while (curr) {
        if (curr->data.key == key) return curr;
        curr = curr->next;
    }
    return NULL;
}

// Вставка элемента в упорядоченный список
void list_insert_sorted(list* l, data_type data) {
    if (list_is_empty(l) || data.key <= l->head->data.key) {
        list_push_front(l, data);
        return;
    }
    if (data.key >= l->tail->data.key) {
        list_push_back(l, data);
        return;
    }
    
    ListNode* curr = l->head;
    while (curr && curr->data.key < data.key) curr = curr->next;
    
    ListNode* node = create_node(data);
    if (!node) return;
    
    node->prev = curr->prev;
    node->next = curr;
    curr->prev->next = node;
    curr->prev = node;
    l->size++;
}

/*
 * Поиск первого от начала элемента, который меньше своего непосредственного
 * предшественника. Если найден — смещение его к началу до тех пор,
 * пока он не станет первым или больше предшественника.
 * Возвращает: true если элемент был перемещён, false если не найден
 */
bool list_fix_out_of_order(list* l) {
    if (list_is_empty(l) || l->size < 2) return false;
    
    ListNode* curr = l->head->next;
    
    // Поиск первого элемента, нарушающего порядок (меньше предшественника)
    while (curr) {
        if (curr->data.key < curr->prev->data.key) break;
        curr = curr->next;
    }
    
    if (!curr) return false;  // Не найден
    
    printf("  -> Найден нарушитель: ключ=%d (знач=%d) < предшественника=%d\n",
           curr->data.key, curr->data.value, curr->prev->data.key);
    
    data_type saved = curr->data;
    
    // Удаляем узел из текущей позиции
    if (curr->prev) curr->prev->next = curr->next;
    if (curr->next) curr->next->prev = curr->prev;
    if (curr == l->head) l->head = curr->next;
    if (curr == l->tail) l->tail = curr->prev;
    l->size--;
    free(curr);
    
    // Вставляем в правильную позицию (смещаем к началу)
    ListNode* pos = l->head;
    while (pos && pos->data.key < saved.key) {
        pos = pos->next;
    }
    
    ListNode* new_node = create_node(saved);
    if (!new_node) return false;
    
    if (!pos) {  // в конец
        if (l->tail) {
            l->tail->next = new_node;
            new_node->prev = l->tail;
            l->tail = new_node;
        } else {
            l->head = l->tail = new_node;
        }
    } else if (pos == l->head) {  // в начало
        new_node->next = l->head;
        l->head->prev = new_node;
        l->head = new_node;
    } else {  // в середину
        new_node->prev = pos->prev;
        new_node->next = pos;
        pos->prev->next = new_node;
        pos->prev = new_node;
    }
    l->size++;
    
    return true;
}


/*
 * Сортировка списка с использованием процедуры №6
 * Многократно применяем list_fix_out_of_order до полной упорядоченности
 */
void insertion_sort_variant(list* l) {
    if (list_is_empty(l) || l->size < 2) return;
    
    printf("\n=== Сортировка (вариант метода вставки) ===\n");
    printf("Исходный список: ");
    list_print(l);
    
    int iterations = 0;
    while (list_fix_out_of_order(l)) {
        iterations++;
        printf("  Итерация %d: ", iterations);
        list_print(l);
    }
    
    printf("Готово! %d итераций. Результат: ", iterations);
    list_print(l);
}


// Генерация случайного числа
static int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Создание тестового списка
void create_test_list(list* l, int count) {
    list_create(l);
    printf("Создание списка из %d элементов:\n", count);
    for (int i = 0; i < count; i++) {
        data_type d;
        d.key = rand_range(1, 99);
        d.value = d.key * 10;
        list_push_back(l, d);
        printf("  [%d] key=%d, val=%d\n", i, d.key, d.value);
    }
}

// Демонстрация процедуры №6
void demonstrate_procedure() {
    printf("\n============================================================\n");
    printf("ДЕМОНСТРАЦИЯ ПРОЦЕДУРЫ №6\n");
    printf("Задача: поиск элемента, нарушающего порядок, и его перемещение\n");
    printf("============================================================\n");
    
    list l;
    list_create(&l);
    
    data_type d;
    d.key = 10; d.value = 100; list_push_back(&l, d);
    d.key = 20; d.value = 200; list_push_back(&l, d);
    d.key = 30; d.value = 300; list_push_back(&l, d);
    d.key = 5;  d.value = 50;  list_push_back(&l, d);  // нарушитель!
    d.key = 40; d.value = 400; list_push_back(&l, d);
    d.key = 50; d.value = 500; list_push_back(&l, d);
    
    printf("\nИсходный список: ");
    list_print(&l);
    
    printf("\nПрименяем процедуру list_fix_out_of_order:\n");
    list_fix_out_of_order(&l);
    
    printf("\nРезультат: ");
    list_print(&l);
    
    list_clear(&l);
}

// Демонстрация сортировки
void demonstrate_sort() {
    printf("\n============================================================\n");
    printf("ДЕМОНСТРАЦИЯ СОРТИРОВКИ (ВАРИАНТ МЕТОДА ВСТАВКИ)\n");
    printf("============================================================\n");
    
    list l;
    create_test_list(&l, 8);
    insertion_sort_variant(&l);
    list_clear(&l);
}


void interactive_mode() {
    list l;
    list_create(&l);
    
    int choice;
    key_type k;
    value_type v;
    
    printf("\n============================================================\n");
    printf("============================================================\n");
    
    while (1) {
        printf("\n┌────────────────────────────────────────────────────────┐\n");
        printf("│ МЕНЮ                                                   │\n");
        printf("├────────────────────────────────────────────────────────┤\n");
        printf("│ 1. Добавить элемент в конец                           │\n");
        printf("│ 2. Добавить элемент в начало                          │\n");
        printf("│ 3. Удалить из начала                                  │\n");
        printf("│ 4. Удалить из конца                                   │\n");
        printf("│ 5. Показать список                                    │\n");
        printf("│ 6. Применить процедуру №6 (fix out of order)          │\n");
        printf("│ 7. Сортировка (вариант метода вставки)                │\n");
        printf("│ 0. Выход                                              │\n");
        printf("└────────────────────────────────────────────────────────┘\n");
        printf("Выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Ошибка ввода!\n");
            continue;
        }
        
        switch (choice) {
            case 1:
                printf("Ключ: ");
                scanf("%d", &k);
                printf("Значение: ");
                scanf("%d", &v);
                {
                    data_type d = {k, v};
                    list_push_back(&l, d);
                    printf("Добавлен (%d,%d) в конец\n", k, v);
                }
                break;
                
            case 2:
                printf("Ключ: ");
                scanf("%d", &k);
                printf("Значение: ");
                scanf("%d", &v);
                {
                    data_type d = {k, v};
                    list_push_front(&l, d);
                    printf("Добавлен (%d,%d) в начало\n", k, v);
                }
                break;
                
            case 3:
                if (!list_is_empty(&l)) {
                    data_type* first = list_get(&l, 0);
                    printf("Удалён (%d,%d) из начала\n", first->key, first->value);
                    list_pop_front(&l);
                } else printf("Список пуст!\n");
                break;
                
            case 4:
                if (!list_is_empty(&l)) {
                    data_type* last = list_get(&l, list_size(&l) - 1);
                    printf("Удалён (%d,%d) из конца\n", last->key, last->value);
                    list_pop_back(&l);
                } else printf("Список пуст!\n");
                break;
                
            case 5:
                printf("Список: ");
                list_print(&l);
                break;
                
            case 6:
                if (list_fix_out_of_order(&l)) {
                    printf("Процедура выполнена!\n");
                } else {
                    printf("Нарушающих порядок элементов не найдено\n");
                }
                break;
                
            case 7:
                insertion_sort_variant(&l);
                break;
                
            case 0:
                list_clear(&l);
                printf("До свидания!\n");
                return;
                
            default:
                printf("Неверный выбор!\n");
        }
    }
}


int main() {
    srand(time(NULL));
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║     Лабораторная работа №26                               ║\n");
    printf("║     АТД: Линейный список (двусвязный)                     ║\n");
    printf("║     Процедура №6: поиск и перемещение нарушителя          ║\n");
    printf("║     Метод сортировки: вариант метода вставки              ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    demonstrate_procedure();
    demonstrate_sort();
    
    interactive_mode();
    
    return 0;
}
