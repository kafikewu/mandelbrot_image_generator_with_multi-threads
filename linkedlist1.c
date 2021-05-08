#include <stdio.h>
#include <stdlib.h>

struct bitmap{
    int i;
};

/*typedef struct {
    int info;
} DATA;*/


typedef struct node {
    //DATA data;
    int i, j, iter;
    struct bitmap* bm;
    struct node* next;
} NODE;

void init(NODE** head) {
    *head = NULL;
}

void print_list(NODE* head) {
    NODE * temp;
    for (temp = head; temp; temp = temp->next)
        printf("%d %d %d ", temp->i, temp->j, temp->bm->i);
}

NODE* add(NODE* node, int x, int y, struct temp* SOME, int iter) {
    NODE* temp = (NODE*) malloc(sizeof (NODE));
    if (temp == NULL) {
        exit(0); // no memory available
    }
    temp->i = x;
    temp->j = y;
    temp->iter = iter;
    temp->bm = SOME;
    temp->next = node;
    node = temp;
    return node;
}

/*void add_at(NODE* node, DATA data) {
    NODE* temp = (NODE*) malloc(sizeof (NODE));
    if (temp == NULL) {
        exit(EXIT_FAILURE); // no memory available
    }
    temp->data = data;
    temp->next = node->next;
    node->next = temp;
}*/

void remove_node(NODE* head) {
    NODE* temp = (NODE*) malloc(sizeof (NODE));
    if (temp == NULL) {
        exit(EXIT_FAILURE); // no memory available
    }
    temp = head->next;
    head->next = head->next->next;
    free(temp);
}

NODE * reverse_rec(NODE * ptr, NODE * previous) {
    NODE * temp;
    if (ptr->next == NULL) {
        ptr->next = previous;
        return ptr;
    } else {
        temp = reverse_rec(ptr->next, ptr);
        ptr->next = previous;
        return temp;
    }
}

NODE * reverse(NODE * node) {
    NODE * temp;
    NODE * previous = NULL;
    while (node != NULL) {
        temp = node->next;
        node->next = previous;
        previous = node;
        node = temp;
    }
    return previous;
}

NODE *free_list(NODE *head) {
    NODE *tmpPtr = head;
    NODE *followPtr;
    while (tmpPtr != NULL) {
        followPtr = tmpPtr;
        tmpPtr = tmpPtr->next;
        free(followPtr);
    }
    return NULL;
}

/*NODE *sort_list(NODE *head) {
    NODE *tmpPtr = head, *tmpNxt = head->next;
    DATA tmp;
    while (tmpNxt != NULL) {
        while (tmpNxt != tmpPtr) {
            if (tmpNxt->data.info < tmpPtr->data.info) {
                tmp = tmpPtr->data;
                tmpPtr->data = tmpNxt->data;
                tmpNxt->data = tmp;
            }
            tmpPtr = tmpPtr->next;
        }
        tmpPtr = head;
        tmpNxt = tmpNxt->next;
    }
    return tmpPtr;
}*/

int main() {
    int i;
    NODE* head;
    NODE* node;
    //DATA element;
    printf("Add Elements to List:\n");
    init(&head);
    for (i = 53; i <= 63; i++) {
        //element.info = i;
        printf("Add Element %2d To The List.\n", i);
        struct bitmap *some;
        some = (struct bitmap*)malloc(sizeof(struct bitmap));
        some->i = 12;
        head = add(head, i, i, some, i);
    }
    printf("\nPrint The List:\n");
    print_list(head);
    head = free_list(head);
    return (EXIT_SUCCESS);
}
