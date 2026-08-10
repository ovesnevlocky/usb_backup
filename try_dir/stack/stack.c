#include <stdio.h>

#include "stack.h"
#include <string.h>
#include <stdlib.h>


void hellostack(void)
{
	printf("helo from stack\n");
}

bool stackRealloc(Stack *s)
{
	//size_t oldC = s->capacity;

	s->capacity *= 2;
	void *tmp = realloc(s->arr, s->capacity * sizeof(int));
	if(!tmp)
	{
		return false;
	}
	
	//memset(s->arr + oldC, -1, sizeof(int) * (s->capacity - oldC));
	s->arr = tmp;
	return true;
}

void stackFree(Stack *s)
{
	free(s->arr);
	s->arr = NULL;
}

void stackInit(Stack *s)
{
	s->top = -1;
	s->count = 0;
	s->capacity = MAX_SIZE;
	s->arr = malloc(sizeof(int) * MAX_SIZE);
	memset(s->arr, -1, sizeof(int) * MAX_SIZE);
}


bool isStackFull(Stack *s)
{
	return s->count >= s->  capacity;
}

bool isStackEmpty(Stack *s)
{
	return s-> count <= 0;
}

bool stackPush(Stack *s, int a)
{
	if(isStackFull(s))
	{
		fprintf(stderr, "stack is full MAX:current %lu\n", s->count);
		return false;
	}
	
	if(a == -1)
	{
		printf("token is not inicialized\n");
		return false;
	}
	
	//has to be incremented first, as inicialized -1
	s->top += 1;
	s->count += 1;
	s->arr[s->top] = a;

	return true;
}
       
int stackPeek(Stack *s)
{
	int ret = -1;
	//if not a element is in the stack
	if(s-> top < 0)
	{
		return ret; 
	}
	 
	return s->arr[s->top];
}

int stackPop(Stack *s)
{

	int ret = -1;
	
	if(isStackEmpty(s))
		return ret;

	ret = s->arr[s->top];

	//set uninicialized to indicate non active element
	s->arr[s->top] = -1;
	s->top -= 1;
	s->count -= 1;
	
	return ret;
}

