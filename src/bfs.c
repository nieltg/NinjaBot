#pragma config(StandardModel, "EV3_REMBOT")
#pragma DebuggerWindows("debugStream")
#pragma DebuggerWindows("ev3LCDScreen")

/*
	BFS
	Assumes no cycle is present.
*/

#include "stack.c"
#include "queue.c"
#include "junction.c"
#include "robot.c"

#define JUNCTION_TABLE_SIZE 100

#define TITLE_LINE 1
#define MAIN_INFO_LINE 3
#define QUEUE_LINE 4
#define ACTION_LINE 5
#define TRACE_DESCRIPTION_LINE 7
#define TRACE_VALUES_LINE 8
#define TURN_DESCRIPTION_LINE 9
#define TURN_VALUES_LINE 10

void goToJunction(int currentJunction, int nextJunction);
void displayStack(Stack *s, int line);
void displayQueue(Queue *q, int line);

Junction junctionTable[JUNCTION_TABLE_SIZE];
int junctionCount;
Queue bfsQueue;

task main() {

	displayCenteredTextLine(TITLE_LINE, "Stima - BFS");
	displayTextLine(MAIN_INFO_LINE, "Starting...");

	writeDebugStreamLine("Starting...");
	bool foundFire = false;
	Robot_begin();
	displayTextLine(MAIN_INFO_LINE, "Going to first junction...");
	Robot_prepare();
	Robot_followLineToJunction();

	Junction_set(&junctionTable[0], JUNCTION_START, 1, 0, 0);
	Junction_set(&junctionTable[0], JUNCTION_INTERSECTION, 0, 0, 0);
	junctionCount = 2;
	Queue_init(&bfsQueue);
	Queue_push(&bfsQueue, 1);

	while (!Queue_isEmpty(&bfsQueue)) {
		// Initial robot state at the beginning of the loop: color sensor touching junction (colored segment)

		int currentJunction = Queue_pop(&bfsQueue);
		writeDebugStreamLine("Current junction: %d.", currentJunction);

		displayTextLine(MAIN_INFO_LINE, "Current BFS junction: %d", currentJunction);
		displayQueue(&bfsQueue, QUEUE_LINE);

		TLegoColors junctionColor = Robot_getColor();

		if (junctionColor == colorYellow) {
			junctionTable[currentJunction].type = JUNCTION_DESTINATION;
			foundFire = true;
			writeDebugStreamLine("Found fire.");
			displayTextLine(ACTION_LINE, "Found fire!");
			Robot_putOutFire();
			while (!Queue_isEmpty(&bfsQueue)) {
				Queue_pop(&bfsQueue);
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
				Queue_push(&bfsQueue, junctionCount);
				writeDebugStreamLine("Pushed %d to queue.", junctionCount);
				displayQueue(&bfsQueue, QUEUE_LINE);
				junctionCount++;
			}

		} else {
			// Error: unknown junction type
			writeDebugStreamLine("Error: unknown junction type.");
		}

		// If there is still another unvisited junction, go there. Otherwise, go back to start.
		int nextJunction = Queue_isEmpty(&bfsQueue) ? 0 : Queue_front(&bfsQueue);
		writeDebugStreamLine("Go from %d to %d.", currentJunction, nextJunction);
		displayTextLine(ACTION_LINE, "Go from %d to %d.", currentJunction, nextJunction);
		goToJunction(currentJunction, nextJunction);
	}

	// TODO: Robot move backward until back at start box, or color sensor on blue segment.
	// Done! Robot is back at the starting point, ready for another run.
	writeDebugStreamLine("Finished!");
	displayTextLine(ACTION_LINE, "Finished!");
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
	displayTextLine(TRACE_DESCRIPTION_LINE, "Path from %d to start:", currentJunction);
	displayStack(&currentTrace, TRACE_VALUES_LINE);

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
		if (nextJunction == 0) {
			displayTextLine(TURN_DESCRIPTION_LINE, "Turns from %d to start:", currentJunction);
			displayStack(&returnInstructions, TURN_VALUES_LINE);
		}

		// Go back to lowest common ancestor junction
		Robot_prepare();
		Robot_uTurn();
		for (int i = 0; i < Stack_size(&returnInstructions); i++) {
			writeDebugStreamLine("Following line...");
			Robot_followLineToJunction();
			Robot_prepare();
			writeDebugStreamLine("Turning to path %d...", Stack_get(&returnInstructions, i));
			if (nextJunction != 0 || i < Stack_size(&returnInstructions) - 1) {
				Robot_turnToPath(Stack_get(&returnInstructions, i));
			}
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

void displayStack(Stack *s, int line) {
	int i;
	string disp, output;
	output = "";
	for (i = 0; i < Stack_size(s); i++) {
		disp = Stack_get(s, i);
		output += " ";
		output +=  disp;
	}
	displayTextLine(line, "Stack:%s", output);
}

void displayQueue(Queue *q, int line) {
	int i;
	string disp, output;
	output = "";
	for (i = 0; i < Queue_size(q); i++) {
		disp = Queue_get(q, i);
		output += " ";
		output +=  disp;
	}
	displayTextLine(line, "Queue:%s", output);
}
