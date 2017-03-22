#ifndef ACTIONS
#define ACTIONS

/* Junction. */

void junction_prepare ()
{
	// Centerize.

	wait10Msec (1);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 30;

	delay (1000);

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

int junction_count ()
{
	// Prepare rotation.

	wait10Msec (1);

	resetGyro (gyroSensor);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 0;

	// Prepare edge detector.

	bool is_black = false;

	if (getColorName (colorSensor) == colorBlack)
		is_black = true;

	// Count black-white edge.

	int cnt = 0;

	while (getGyroDegrees (gyroSensor) < 359)
	{
		if (getColorName (colorSensor) != colorBlack)
		{
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

void junction_stop_at (int val)
{
	// Rotate 180 degree.

	wait10Msec (1);

	resetGyro (gyroSensor);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 0;

	while (getGyroDegrees (gyroSensor) < 170);

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;

	// Prepare detector.

	bool is_black = false;

	if (getColorName (colorSensor) == colorBlack)
		is_black = true;

	// Scan edges.

	int cnt = 0;

	wait10Msec (1);

	resetGyro (gyroSensor);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 0;

	while (getGyroDegrees (gyroSensor) < 360)
	{
		if (getColorName (colorSensor) != colorBlack)
		{
			if (is_black)
			{
				/* Edge is detected. */

				if (cnt++ >= val)
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

/* Path. */

void path_begin ()
{
	// Search for blue dot.

	motor[leftMotor]  = 30;
	motor[rightMotor] = 30;

	while (getColorName (colorSensor) != colorBlue);
}

TLegoColors path_walk ()
{
	bool left_dominant = true;

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

		delay (10);
	}

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

/* Main. */

/*task main ()
{
	path_begin ();

	junction_prepare ();
	junction_stop_at (0);

	path_walk ();

	junction_prepare ();
	junction_stop_at (1);

	path_walk ();

	junction_prepare ();
	junction_stop_at (0);

	path_walk ();

	junction_prepare ();
	junction_stop_at (1);

	path_walk ();
}
*/