
/**
 * @file list.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Jul    2 18:07:23 2021
 * @brief Lists.
 *        Modified 29/12/2024 by OUSSET Gaël
 * 
 * Lists.
 */

#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "assert.h"

list_t list_new(void) { 
    return NULL; 
}

int list_empty(list_t l) { 
    return l == list_new();
}

void* list_first(list_t l) { 

    assert(!list_empty(l), "List cannot be empty", NULL);
    
    return l->content; 
}

list_t list_next(list_t l) { 

    assert(!list_empty(l), "List cannot be empty", l);
    
    return l->next; 
}

list_t list_add_first(list_t l, void* object) { 
    struct link_t *new = malloc(sizeof(*new));
    assert(new, "Memory allocation failed", l); 

    new->content = object; 
    new->next        = l; 
    
    return new; 
}

list_t list_add_last(list_t l, void* object) {
    if (list_empty(l)) {
        l = list_add_first(l, object);
        return l;
    }

    struct link_t *new = malloc(sizeof(*new));
    assert(new, "Memory allocation failed", l);

    list_t first = l;

    for (; !list_empty(l->next); l = list_next(l)) {}

    l->next      = new;
    new->content = object;
    new->next    = list_new();

    return first;
}

list_t list_del_first(list_t l, action_t delete) { 
    list_t next;
    
    assert(!list_empty(l), "List cannot be empty", l); 

    next = l->next; 

    if (delete) delete(l->content);
    
    free(l);
    
    return next; 
}

size_t list_length(list_t l) { 
    size_t len = 0;
    
    for (; !list_empty(l) ; l = list_next(l), len++);
    
    return len; 
}

int list_print(list_t l, action_t print) { 
    int ret = printf("(%s", list_empty(l) ? "" : " ");

    for (; !list_empty(l); l = list_next(l)) { 
        ret += print(list_first(l));
        ret += printf(" ");
    }
    
    ret += printf(")");

    return ret; 
}

void list_delete(list_t l, action_t delete) {
    for (; !list_empty(l) ; l = list_del_first(l, delete));
}
