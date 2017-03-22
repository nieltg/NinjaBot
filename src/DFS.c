#pragma config(StandardModel, "EV3_REMBOT")

#include "common/Actions.c"

/**
 * @brief Explore the map using DFS technique.
 *
 * I.S.: Robot is about to enter a junction.
 * F.S.: Robot is about to leave a junction.
 */
void DFS_do (bool _first)
{
	// Enter junction.

	Actions_Junction_begin ();

	// Count paths.

	int cnt = Actions_Junction_count ();

	if (!_first)
		cnt = cnt - 1;

	for (int i = 0; i < cnt; i++)
	{
		// Workaround: Stop at 0 if there's only a path.

		Actions_Junction_stopAt ((cnt > 1) ? 1 : 0);

		// Move robot to junction and redo DFS.

		Actions_Path_walk ();
		DFS_do (false);
		Actions_Path_walk ();

		// Prepare for next junction operation.

		Actions_Junction_begin ();
	}

	if (!_first)
		Actions_Junction_stopAt ((cnt > 1) ? 1 : 0);
}

task main ()
{
	Actions_Path_begin ();

	DFS_do (true);
}
