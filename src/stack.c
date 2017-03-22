#ifndef STACK
#define STACK

#define MAX_STACK_SIZE 50
#define STACK_EMPTY_VALUE 0

typedef struct {
	int data[MAX_STACK_SIZE];
	int size;
} Stack;

void Stack_init(Stack *s) {
	s->size = 0;
}

int Stack_size(Stack *s) {
	return s->size;
}

bool Stack_isFull(Stack *s) {
	return s->size >= MAX_STACK_SIZE;
}

bool Stack_isEmpty(Stack *s) {
	return s->size == 0;
}

int Stack_top(Stack *s) {
	if (s->size > 0) {
		return s->data[s->size - 1];
	} else {
		return STACK_EMPTY_VALUE;
	}
}

int Stack_get(Stack *s, int i) {
	if (i >= 0 && i < s->size) {
		return s->data[i];
	} else {
		return STACK_EMPTY_VALUE;
	}
}

void Stack_push(Stack *s, int value) {
	if (!Stack_isFull(q)) {
		s->data[s->size] = value;
		s->size++;
	}
}

int Stack_pop(Stack *s) {
	if (s->size > 0) {
		int res = s->data[s->size - 1];
		s->size--;
		return res;
	} else {
		return STACK_EMPTY_VALUE;
	}
}

#endif
