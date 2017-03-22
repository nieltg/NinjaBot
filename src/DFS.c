#pragma config(StandardModel, "EV3_REMBOT")

#include "common/Actions.c"

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
		Actions_Junction_stopAt ((cnt > 1) ? 1 : 0);

		Actions_Path_walk ();
		DFS_do (false);
		Actions_Path_walk ();

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
