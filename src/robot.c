#ifndef ROBOT
#define ROBOT

TLegoColors Robot_getColor()
{
	TLegoColors col = getColorName (colorSensor);

	switch (col)
	{
	case colorBlack:
	case colorWhite:
		return col;
	}

	int h = getColorHue (colorSensor);

	if ((h >= 331) || (h <= 20))
		return colorRed;
	else
	if ((h >= 166) && (h <= 260))
		return colorBlue;
	else
	if ((h >= 71) && (h <= 165))
		return colorGreen;
	else
	if ((h >= 21) && (h <= 70))
		return colorYellow;

	writeDebugStreamLine ("Unknown! Hue: %d", h);

	return colorNone;
}

void Robot_putOutFire() {
	// TODO
	writeDebugStreamLine("Putting out fire...");
	delay(3000);
	writeDebugStreamLine("Fire extinguished!");
}

void Robot_reversePrepare() {
	wait10Msec (1);

	motor[leftMotor]  = -30;
	motor[rightMotor] = -30;

	delay (650);

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

	delay (850);

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

	if (Robot_getColor() == colorBlack)
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

		if (Robot_getColor() != colorBlack)
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
	while (Robot_getColor() != colorBlack);
	delay (50);
	while (Robot_getColor() != colorWhite);
	writeDebugStreamLine("Passed 0th path...");

	int risingEdgeCount = 0;
	resetGyro(gyroSensor);
	while (getGyroDegrees(gyroSensor) < 360 && risingEdgeCount < path) {
		delay (50);
		// white -> black
		while (Robot_getColor() != colorWhite);
		delay (50);
		while (Robot_getColor() != colorBlack);
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

	while (Robot_getColor() != colorBlue) {}
}

/**
 * @brief Follow the left edge of a line until the robot reaches a colored tile.
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
		TLegoColors col = Robot_getColor();

		if (col == colorBlack)
		{
			motor[leftMotor]  = -20;
			motor[rightMotor] = 30;
		}
		else
		if (col == colorWhite)
		{
			motor[leftMotor]  = 30;
			motor[rightMotor] = 10;
		}
		else {
			motor[leftMotor]  = 0;
			motor[rightMotor] = 0;
			writeDebugStreamLine("Arrived at end of line, color: %d.", col);
			return col;
		}

		delay (1);
	}

	motor[leftMotor]  = 0;
	motor[rightMotor] = 0;
}

#endif
