#ifndef JUNCTION
#define JUNCTION

#define JUNCTION_INTERSECTION 'i'
#define JUNCTION_DEAD_END 'x'
#define JUNCTION_START 's'
#define JUNCTION_DESTINATION 'd'

typedef struct {
	char type; // The type of the junction
	int pathCount; // How many paths are connected to this junction
	int ancestor; // Index of the junction visited before this junction
	int ancestorPath; // From the previous junction, which path is taken to go to this junction
} Junction;

void Junction_set(Junction *j, char type, int pathCount, int ancestor, int ancestorPath) {
	j->type = type;
	j->pathCount = pathCount;
	j->ancestor = ancestor;
	j->ancestorPath = ancestorPath;
}

#endif
