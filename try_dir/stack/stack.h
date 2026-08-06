#ifndef STACK_H_
#define STACK_H_

#define MAX_SIZE 100
#include <stdbool.h>


typedef struct
{
	int arr[MAX_SIZE];
	int top;
	int count;
}Stack;

void stackInit(Stack *s);

bool isStackFull(Stack *s);

bool isStackEmpty(Stack *s);

bool stackPush(Stack *s, int  a);

int stackPeek(Stack *s);

int stackPop(Stack *s);
void hellostack(void);

void stackPrint(const Stack s);

#endif
