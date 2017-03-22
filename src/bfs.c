#pragma config(StandardModel, "EV3_REMBOT")

#include "stack.c"
#include "queue.c"
#include "junction.c"
#include "actions.c"

#define JUNCTION_TABLE_SIZE 100

void goToJunction(int currentJunction, int nextJunction);

Junction junctionTable[JUNCTION_TABLE_SIZE];
int junctionCount;
Queue bfsQueue;

task main() {

	bool foundFire = false;
	Robot_goToStartJunction();

	Junction_set(&junctionTable[0], JUNCTION_START, 1, 0, 0);
	junctionCount = 1;
	Queue_init(&bfsQueue);
	Queue_push(&bfsQueue, 0);

	while (!Queue_isEmpty(&bfsQueue)) {
		// Initial robot state at the beginning of the loop: color sensor touching junction (colored segment)

		int currentJunction = Queue_pop(&bfsQueue);
		TLegoColors junctionColor = Robot_getColor();
		junctionTable[currentJunction].pathCount = Robot_countJunction();

		if (junctionColor == colorYellow) {
			junctionTable[currentJunction].junctionType = JUNCTION_FIRE;
			foundFire = true;
			Robot_putOutFire();
			while (!Queue_isEmpty(&bfsQueue)) {
				Queue_pop(&bfsQueue);
			}

		} else if (junctionColor == colorRed || junctionColor == colorBlue) {
			junctionTable[currentJunction].junctionType = JUNCTION_DEAD_END;

		} else if (junctionColor == colorGreen) {
			junctionTable[currentJunction].junctionType = JUNCTION_INTERSECTION;

			for (int direction = 1; direction < junctionTable[currentJunction].pathCount; direction++) {
				junctionTable[junctionCount].ancestor = currentJunction;
				junctionTable[junctionCount].ancestorPath = direction;
				Queue_push(&bfsQueue, junctionCount);
				junctionCount++;
			}

		} else {
			// Error: unknown junction type
		}

		// If there is still another unvisited junction, go there. Otherwise, go back to start.
		int nextJunction = Queue_isEmpty(&bfsQueue) ? 0 : Queue_front(&bfsQueue);
		goToJunction(currentJunction, nextJunction);
	}

	// TODO: Robot move backward until back at start box, or color sensor on blue segment.
	// Done! Robot is back at the starting point, ready for another run.
}

void goToJunction(int currentJunction, int nextJunction) {
	Stack currentTrace;
	Stack nextTrace;
	Stack_init(&currentTrace);
	Stack_init(&nextTrace);
	int traceJunction;

	// Trace junctions visited from current junction to start junction
	traceJunction = currentJunction;
	while (traceJunction != 0) {
		Stack_push(&currentTrace, traceJunction);
		traceJunction = junctionTable[traceJunction].ancestor;
	}
	Stack_push(&currentTrace, 0);

	// Trace junctions visited from next junction to start junction
	traceJunction = nextJunction;
	while (traceJunction != 0) {
		Stack_push(&nextTrace, traceJunction);
		traceJunction = junctionTable[traceJunction].ancestor;
	}
	Stack_push(&nextTrace, 0);

	// Find lowest common ancestor junction, remove all common junctions from traces
	int lcaJunction;
	while (Stack_top(&currentTrace) == Stack_top(&nextTrace)) {
		lcaJunction = currentTrace.pop();
		nextTrace.pop();
	}

	if (lcaJunction != currentJunction) {
		// Compute return instructions sequence (stack bottom is first instruction, stack top is last)
		Stack returnInstructions;
		Stack_init(&returnInstructions);
		Stack_push(&currentTrace, lcaJunction);
		for (int i = 2; i < Stack_size(&currentTrace)-1; i++) {
			int pathCount = junctionTable[Stack_get(&currentTrace, i)].pathCount;
			int ancestorPath = junctionTable[Stack_get(&currentTrace, i-1)].ancestorPath;
			Stack_push(&returnInstructions, pathCount - ancestorPath);
		}

		// Go back to lowest common ancestor junction
		Robot_prepare();
		Robot_uTurn();
		Robot_followLineToJunction();
		for (int i = 0; i < Stack_size(&returnInstructions); i++) {
			Robot_turnToPath(Stack_get(&returnInstructions, i));
			Robot_followLineToJunction();
		}
		Robot_prepare();
		Robot_uTurn();

	} else {
		Robot_prepare();
	}
	// State: robot is facing forward over the lowest common ancestor junction

	// Go from lowest common ancestor junction to the next junction
	if (Stack_isEmpty(&nextTrace)) {
		// The lowest common ancestor junction is an ancestor of the current junction
		Robot_reversePrepare();
	} else {
		Stack_pop(&nextTrace);
		while (!Stack_isEmpty(&nextTrace)) {
			Robot_turnToPath(junctionTable[Stack_pop(&nextTrace)].ancestorPath);
			Robot_followLineToJunction();
		}
	}
	// State: robot is at next junction, with color sensor on colored segment
}

/*
void Robot_goToStartJunction();
void Robot_prepare();
void Robot_reversePrepare();
TLegoColors Robot_getColor();
int Robot_countJunction();
void Robot_turnToPath(int pathDirection);
void Robot_putOutFire();
void Robot_displayText(string text);

Assumes no cycle
 */
