#ifndef STACK_H_
#define STACK_H_

#define MAX_SIZE 2 
#include <stdbool.h>
#include <stdlib.h>


typedef struct
{
	int *arr;
	int top;
	size_t  count;
	size_t capacity;
}Stack;

void stackInit(Stack *s);

bool isStackFull(Stack *s);

bool isStackEmpty(Stack *s);

bool stackPush(Stack *s, int  a);

int stackPeek(Stack *s);

int stackPop(Stack *s);
void hellostack(void);

void stackPrint(const Stack s);

bool stackRealloc(Stack *s);

void stackFree(Stack *s);
#endif
