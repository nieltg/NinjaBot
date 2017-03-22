#pragma config(StandardModel, "EV3_REMBOT")

/* Actions::Junction */

/**
 * @brief Prepare for more actions on junction.
 *
 * Implementation: Move robot forward a little.
 */
void Actions_Junction_begin ()
{
	// Centerize.

	wait10Msec (1);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 30;

	delay (1000);

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

/**
 * @brief Count available paths from a junction.
 * @returns  Count of available paths.
 *
 * %Actions_Junction_begin must be called before calling this function.
 * Implementation: Rotate 360 degrees and count for black-white edges.
 */
int Actions_Junction_count ()
{
	// Prepare edge detector.

	bool is_black = false;

	if (getColorName (colorSensor) == colorBlack)
		is_black = true;

	// Rotate 360 degrees.

	wait10Msec (1);

	resetGyro (gyroSensor);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 0;

	int cnt = 0;

	while (getGyroDegrees (gyroSensor) < 360)
	{
		// Detect black-white edge.

		if (getColorName (colorSensor) != colorBlack)
		{
			// Count edges.

			if (is_black)
				cnt++;

			is_black = false;
		}
		else
			is_black = true;
	}

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;

	return cnt;
}

/**
 * @brief Rotate and stop at specified way from a junction.
 * @param _id  Way number.
 *
 * %Actions_Junction_begin must be called before calling this function.
 * Implementation: Rotate 170 degrees then stop at specified black-white edges.
 */
void Actions_Junction_stopAt (int _id)
{
	// Rotate 170 degree.

	wait10Msec (1);

	resetGyro (gyroSensor);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 0;

	while (getGyroDegrees (gyroSensor) < 170) {}

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;

	// Prepare edge detector.

	bool is_black = false;

	if (getColorName (colorSensor) == colorBlack)
		is_black = true;

	// Rotate 360 degrees.

	int cnt = 0;

	wait10Msec (1);

	resetGyro (gyroSensor);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 0;

	while (getGyroDegrees (gyroSensor) < 360)
	{
		// Detect black-white edge.

		if (getColorName (colorSensor) != colorBlack)
		{
			if (is_black)
			{
				/* Stop if it is specified edge. */

				if (cnt++ >= _id)
					break;
			}

			is_black = false;
		}
		else
			is_black = true;
	}

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

/* Actions::Path */

/**
 * @brief Begin the journey.
 *
 * Implementation: Search for blue dot.
 */
void Actions_Path_begin ()
{
	// Walk straight.

	motor[leftMotor]  = 30;
	motor[rightMotor] = 30;

	// Stop at blue.

	while (getColorName (colorSensor) != colorBlue) {}
}

/**
 * @brief Walk through until the robot reaches a colored tile.
 * @return  Colored tile the robot discovered.
 *
 * Implementation: Walk through and stop if color sensor detect anycolor.
 */
TLegoColors Actions_Path_walk ()
{
	motor[leftMotor]  = 30;
	motor[rightMotor] = 20;

	while (true)
	{
		TLegoColors col = getColorName (colorSensor);

		if (col == colorBlack)
		{
			motor[leftMotor]  = 30;
			motor[rightMotor] = 10;
		}
		else
		if (col == colorWhite)
		{
			motor[leftMotor]  = 10;
			motor[rightMotor] = 30;
		}
		else
			return col;

		delay (1);
	}

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

/* Main. */

/*
task main ()
{
	Actions_Path_begin ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (0);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (1);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (0);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (1);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (1);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (0);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (1);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (0);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (1);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (1);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (0);

	Actions_Path_walk ();

	Actions_Junction_begin ();
	Actions_Junction_stopAt (1);

	Actions_Path_walk ();
}
*/
