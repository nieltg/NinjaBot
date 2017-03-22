#ifndef QUEUE
#define QUEUE

#define MAX_QUEUE_SIZE 100
#define EMPTY_QUEUE_VALUE 0

typedef struct {
	int data[MAX_QUEUE_SIZE];
	int head;
	int tail;
} Queue;

void Queue_init(Queue *q) {
	q->head = 0;
	q->tail = 0;
}

int Queue_size(Queue *q) {
	int size = q->tail - q->head;
	if (size < 0) {
		size += MAX_QUEUE_SIZE;
	}
	return size;
}

bool Queue_isFull(Queue *q) {
	return Queue_size(q) >= MAX_QUEUE_SIZE-1;
}

bool Queue_isEmpty(Queue *q) {
	return q->head == q->tail;
}

int Queue_front(Queue *q) {
	if (q->head != q->tail) {
		return q->data[q->head];
	} else {
		return EMPTY_QUEUE_VALUE;
	}
}

void Queue_push(Queue *q, int value) {
	if (!Queue_isFull(q)) {
		q->data[q->tail] = value;
		q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
	}
}

int Queue_pop(Queue *q) {
	if (q->head != q->tail) {
		int res = q->data[q->head];
		q->head = (q->head + 1) % MAX_QUEUE_SIZE;
		return res;
	} else {
		return EMPTY_QUEUE_VALUE;
	}
}

#endif
