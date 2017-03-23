#pragma config(StandardModel, "EV3_REMBOT")

#include "stack.c"
#include "junction.c"
#include "robot.c"

#define JUNCTION_TABLE_SIZE 100

void goToJunction(int currentJunction, int nextJunction);

Junction junctionTable[JUNCTION_TABLE_SIZE];
int junctionCount;
Stack dfsStack;

task main() {

	bool foundFire = false;
	Robot_begin();
	Robot_prepare();
	Robot_followLineToJunction();

	Junction_set(&junctionTable[0], JUNCTION_START, 1, 0, 0);
	Junction_set(&junctionTable[0], JUNCTION_INTERSECTION, 0, 0, 0);
	junctionCount = 2;
	Stack_init(&dfsStack);
	Stack_push(&dfsStack, 1);

	while (!Stack_isEmpty(&dfsStack)) {
		// Initial robot state at the beginning of the loop: color sensor touching junction (colored segment)

		int currentJunction = Stack_pop(&dfsStack);
		writeDebugStreamLine("Current junction: %d.", currentJunction);

		TLegoColors junctionColor = robot_getColor();

		if (junctionColor == colorYellow) {
			junctionTable[currentJunction].type = JUNCTION_DESTINATION;
			foundFire = true;
			writeDebugStreamLine("Found fire.");
			Robot_putOutFire();
			while (!Stack_isEmpty(&dfsStack)) {
				Stack_pop(&dfsStack);
			}

		} else if (junctionColor == colorRed || junctionColor == colorBlue) {
			writeDebugStreamLine("Dead end.");
			junctionTable[currentJunction].type = JUNCTION_DEAD_END;

		} else if (junctionColor == colorGreen) {
			writeDebugStreamLine("Intersection.");
			junctionTable[currentJunction].type = JUNCTION_INTERSECTION;

			Robot_prepare();
			junctionTable[currentJunction].pathCount = Robot_countPath();
			writeDebugStreamLine("Path count: %d.", junctionTable[currentJunction].pathCount);
			Robot_reversePrepare();

			for (int direction = 1; direction < junctionTable[currentJunction].pathCount; direction++) {
				junctionTable[junctionCount].ancestor = currentJunction;
				junctionTable[junctionCount].ancestorPath = direction;
				Stack_push(&dfsStack, junctionCount);
				writeDebugStreamLine("Pushed %d to queue.", junctionCount);
				junctionCount++;
			}

		} else {
			// Error: unknown junction type
			writeDebugStreamLine("Error: unknown junction type.");
		}

		// If there is still another unvisited junction, go there. Otherwise, go back to start.
		int nextJunction = Stack_isEmpty(&dfsStack) ? 0 : Stack_top(&dfsStack);
		writeDebugStreamLine("Go from %d to %d.", currentJunction, nextJunction);
		goToJunction(currentJunction, nextJunction);
	}

	// TODO: Robot move backward until back at start box, or color sensor on blue segment.
	// Done! Robot is back at the starting point, ready for another run.
	writeDebugStreamLine("Finished!");
}

void goToJunction(int currentJunction, int nextJunction) {
	Stack currentTrace;
	Stack nextTrace;
	Stack_init(&currentTrace);
	Stack_init(&nextTrace);
	int traceJunction;

	if (currentJunction == nextJunction) {
		return;
	}

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
	int lcaJunction = -1;
	while (!Stack_isEmpty(&currentTrace) && !Stack_isEmpty(&nextTrace) && Stack_top(&currentTrace) == Stack_top(&nextTrace)) {
		lcaJunction = Stack_pop(&currentTrace);
		Stack_pop(&nextTrace);
	}
	writeDebugStreamLine("LCA junction: %d", lcaJunction);

	if (lcaJunction != currentJunction) {
		// Compute return instructions sequence (stack bottom is first instruction, stack top is last)
		Stack returnInstructions;
		Stack_init(&returnInstructions);
		Stack_push(&currentTrace, lcaJunction);
		for (int i = 1; i < Stack_size(&currentTrace); i++) {
			int currentTraceJunction = Stack_get(&currentTrace, i);
			int nextTraceJunction = Stack_get(&currentTrace, i-1);
			int pathCount = junctionTable[currentTraceJunction].pathCount;
			int ancestorPath = junctionTable[nextTraceJunction].ancestorPath;
			Stack_push(&returnInstructions, pathCount - ancestorPath);
		}

		// Go back to lowest common ancestor junction
		Robot_prepare();
		Robot_uTurn();
		for (int i = 0; i < Stack_size(&returnInstructions); i++) {
			writeDebugStreamLine("Following line...");
			Robot_followLineToJunction();
			Robot_prepare();
			writeDebugStreamLine("Turning to path %d...", Stack_get(&returnInstructions, i));
			Robot_turnToPath(Stack_get(&returnInstructions, i));
		}
		Robot_uTurn();

	} else {
		Robot_prepare();
	}
	// State: robot is facing forward over the lowest common ancestor junction

	writeDebugStreamLine("Arrived at LCA: %d.", lcaJunction);

	// Go from lowest common ancestor junction to the next junction
	if (Stack_isEmpty(&nextTrace)) {
		// The lowest common ancestor junction is an ancestor of the current junction
		Robot_reversePrepare();
	} else {
		while (!Stack_isEmpty(&nextTrace)) {
			int next = Stack_pop(&nextTrace);
			writeDebugStreamLine("Next: %d, turnToPath: %d.", next, junctionTable[next].ancestorPath);
			Robot_turnToPath(junctionTable[next].ancestorPath);
			Robot_followLineToJunction();
			if (!Stack_isEmpty(&nextTrace)) {
				Robot_prepare();
			}
		}
	}
	// State: robot is at next junction, with color sensor on colored segment
}

/*
void Robot_begin();
void Robot_prepare();
void Robot_reversePrepare();
TLegoColors Robot_getColor();
int Robot_countPath();
void Robot_turnToPath(int pathDirection);
void Robot_uTurn();
void Robot_putOutFire();
void Robot_displayText(string text);

Assumes no cycle
 */
