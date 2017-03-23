#ifndef ROBOT
#define ROBOT

void Robot_putOutFire() {
	// TODO
	delay(3000);
}

void Robot_reversePrepare() {
	wait10Msec (1);

	motor[leftMotor]  = -30;
	motor[rightMotor] = -30;

	delay (600);

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

void Robot_uTurn() {
	wait10Msec (1);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 0;

	delay (2950);

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

/**
 * @brief Prepare for more actions on junction.
 *
 * Implementation: Move robot forward a little.
 */
void Robot_prepare ()
{
	// Centerize.

	wait10Msec (1);

	motor[leftMotor]  = 30;
	motor[rightMotor] = 30;

	delay (800);

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

/**
 * @brief Count available paths from a junction.
 * @returns Count of available paths.
 *
 * %Robot_prepare must be called before calling this function.
 * Implementation: Rotate 360 degrees and count for black-white edges.
 */
int Robot_countPath ()
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
 * %Robot_prepare must be called before calling this function.
 * Implementation: Rotate 170 degrees then stop at specified black-white edges.
 */
void Robot_turnToPath(int path)
{
	Robot_uTurn();
	if (path <= 0) return;

	motor[leftMotor]  = 30;
	motor[rightMotor] = 0;

	// Clear 0th path (black -> white)
	while (getColorName (colorSensor) != colorBlack);
	delay (50);
	while (getColorName (colorSensor) != colorWhite);
	writeDebugStreamLine("Passed 0th path...");

	int risingEdgeCount = 0;
	resetGyro(gyroSensor);
	while (getGyroDegrees(gyroSensor) < 360 && risingEdgeCount < path) {
		delay (50);
		// white -> black
		while (getColorName (colorSensor) != colorWhite);
		delay (50);
		while (getColorName (colorSensor) != colorBlack);
		risingEdgeCount++;
		writeDebugStreamLine("Passing path...");
	}

	motor[leftMotor]  = 30;
	motor[rightMotor] = 0;
}

/* Actions::Path */

/**
 * @brief Begin the journey.
 *
 * Implementation: Search for blue dot.
 */
void Robot_begin ()
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
TLegoColors Robot_followLineToJunction()
{
	motor[leftMotor]  = 20;
	motor[rightMotor] = 30;

	while (true)
	{
		TLegoColors col = getColorName (colorSensor);

		if (col == colorBlack)
		{
			motor[leftMotor]  = 10;
			motor[rightMotor] = 30;
		}
		else
		if (col == colorWhite)
		{
			motor[leftMotor]  = 30;
			motor[rightMotor] = 10;
		}
		else
			return col;

		delay (1);
	}

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

TLegoColors Robot_getColor() {
	return getColorName(colorSensor);
}

/* Main. */

/*
task main ()
{
	Robot_prepare ();

	Robot_prepare ();
	Robot_turnToPath (0);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (1);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (0);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (1);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (1);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (0);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (1);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (0);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (1);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (1);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (0);

	Robot_followLineToJunction ();

	Robot_prepare ();
	Robot_turnToPath (1);

	Robot_followLineToJunction ();
}
*/

#endif
