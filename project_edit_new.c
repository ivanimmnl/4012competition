#pragma config(Sensor, in1,    uppersensor,    sensorAnalog)
#pragma config(Sensor, in2,    lowersensor,    sensorAnalog)
#pragma config(Sensor, in3,    shortsensor,    sensorAnalog)
#pragma config(Sensor, in4,    color_FrontRight, sensorAnalog)
#pragma config(Sensor, in5,    color_FrontLeft, sensorAnalog)
#pragma config(Sensor, in6,    color_BackRight, sensorAnalog)
#pragma config(Sensor, in7,    color_BackLeft, sensorAnalog)
#pragma config(Sensor, in8,    rearsensor,     sensorAnalog)
#pragma config(Sensor, dgtl5,  masterswitch,   sensorDigitalIn)
#pragma config(Sensor, dgtl6,  westpin,        sensorDigitalIn)
#pragma config(Sensor, dgtl7,  southpin,       sensorDigitalIn)
#pragma config(Sensor, dgtl8,  eastpin,        sensorDigitalIn)
#pragma config(Sensor, dgtl9,  northpin,       sensorDigitalIn)
#pragma config(Sensor, dgtl11, bumperBL,       sensorDigitalIn)
#pragma config(Sensor, dgtl12, bumperBR,       sensorDigitalIn)
#pragma config(Motor,  port2,           rightmotor,    tmotorServoContinuousRotation, openLoop, reversed)
#pragma config(Motor,  port3,           leftmotor,     tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port6,           grabber,       tmotorServoStandard, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//start of variables declaration
bool isFrontLeftClear, isFrontRightClear, isBackLeftClear, isBackRightClear = true;
int lowervalue, uppervalue, shortvalue;
int FRval1, FLval1, BRval1, BLval1;
int FRval2, FLval2, BRval2, BLval2;

bool isBall = false;
bool isOpponent = false;
bool isBallIn = false;
bool isCollectionArea = false;
bool isScanning = false;
string state = "waiting to start";

string orientation = "NONE";
int orientationVal = 0;
//end of variables declaration

//start of tunable parameters
int SERVO_START_POS = -30;
int SERVO_CATCH_POS = 115;
int DELAY_CATCH = 1;
int DELAY_RELEASE = 6;

int UPPER_SENSOR_THRESHOLD = 1000;
int LOWER_SENSOR_THRESHOLD_LOW = 550;
int LOWER_SENSOR_THRESHOLD_HIGH = 2500;
int SHORT_SENSOR_THRESHOLD_LOW = 1500;
int SHORT_SENSOR_THRESHOLD_HIGH = 2500;
//end of tunable parameters

//start of functions
void read_compass() //read orientation
{
	orientationVal = 8*SensorValue(westpin)+4*SensorValue(southpin)+2*SensorValue(eastpin)+SensorValue(northpin);
	switch(orientationVal)
	{
	case 14:
		orientation = "NORTH";
		break;
	case 13:
		orientation = "EAST";
		break;
	case 11:
		orientation = "SOUTH";
		break;
	case 7:
		orientation = "WEST";
		break;
	case 12:
		orientation = "NORTHEAST";
		break;
	case 9:
		orientation = "SOUTHEAST";
		break;
	case 3:
		orientation = "SOUTHWEST";
		break;
	case 6:
		orientation = "NORTHWEST";
		break;
	default:
		orientation = "NONE";
	}
}


void move(int direction, int power) //move robot
{
	motor(leftmotor) = direction*power;
	motor(rightmotor) = direction*power;
}

void halt() //stop robot
{
	motor(leftmotor) = 0;
	motor(rightmotor) = 0;
}

void rotate(int direction, int power) //rotate robot, 1 is CW, -1 is CCW
{
	motor(leftmotor) = direction*power;
	motor(rightmotor) = -direction*power;
}

void align() //align to facing away the collection
{
	if(isBallIn == true){ //returning to collection
		if(SensorValue(eastpin) == 0){
			while(orientationVal != 11) //south
				rotate(1,30);
		}
		else if(SensorValue(westpin) == 0){
			while(orientationVal != 11) //south
				rotate(-1,30);
		}
		halt();
	}
	else{ //if explorer mode, stop rotating if ball detected, if line 176 is not needed, can delete
		if(SensorValue(eastpin) == 0){
			while(orientationVal != 11 && isBall == false) //south
				rotate(1,30);
		}
		else if(SensorValue(westpin) == 0){
			while(orientationVal != 11 && isBall == false) //south
				rotate(-1,30);
		}

		clearTimer(T4);
		while(time1[T4] < 2000 && isBall == false)
			move(-1,30);

		halt();
	}
}

void maneuver() //LIST ALL SCENARIOS, INCLUDE BUMPERS ETC, include scenario when opponent detected
{
	if(isBallIn == false){ //explorer mode
		//if(isOpponent == true)
		//{
		//	move(-1, 30);
		//	wait1Msec(1500); //tune
		//	align();
		//}
		if(isFrontLeftClear == false){
			state = "maneuvering";
			move(-1, 45);
			wait1Msec(1000);

			clearTimer(T3);
			while(time1(T3) < 500 && isBall == false) //facing right, tune timer
				rotate(1, 45);

			isScanning = false; //to stop the scanning if it's scanning
			halt();
		}
		else if(isFrontRightClear == false){
			state = "maneuvering";
			move(-1, 45);
			wait1Msec(1000);
			clearTimer(T3);
			while(time1(T3) < 500 && isBall == false) //facing left, tune timer
				rotate(-1, 45);

			isScanning = false;
			halt();
		}

		//if robot facing collection area, rotate back, might not be necessary
		if(SensorValue(northpin) == 0 && isScanning == false && isBall == false){ //if robot facing north/northeast/northwest -> facing collection area
			align();
			move(-1,30);
			wait1Msec(3000); //move back 3s
		}
	}
	else if(isBallIn == true && isCollectionArea == false && SensorValue(rearsensor) < 1000){ //if rear sensor doesn't detect anything, function as usual
		//if(isOpponent == true)
		//{
		//	move(1, 30);
		//	wait1Msec(2000);
		//  align();
		//}
		if(isBackLeftClear == false){
			//rotate(-1, 45);
			//wait1Msec(1000);
			move(1, 60); //move forward 2s
			wait1Msec(2000);
			align();
		}
		else if(isBackRightClear == false){
			//rotate(1, 45);
			//wait1Msec(1000);
			move(1, 60); //move forward 2s
			wait1Msec(2000);
			align();
		}
	}
}

void scan() //rotate robot to scan
{
	isScanning = true;
	state = "scanning";
	clearTimer(T2);
	while(isScanning == true && time1[T2] < 800 && isBall == false){ //rotate left
		rotate(-1,30);
		maneuver(); //if boundary detected, isScanning will be updated to false and the scan function will exit
	}

	clearTimer(T2);
	while(isScanning == true && time1[T2] < 1600 && isBall == false){ //rotate right
		rotate(1,30);
		maneuver();
	}

	clearTimer(T2);
	while(isScanning == true && time1[T2] < 900 && orientationVal != 11 && isBall == false){ //rotate left-> robot is back to facing south, fine tune timer
		rotate(-1,30);
		maneuver();
	}

	isScanning = false;
	halt();
}

void grab_ball() //ball catching mechanism
{
	for(int count = SERVO_START_POS; count <= SERVO_CATCH_POS; count++){ //swing down
		setServo(grabber, count);
		wait1Msec(DELAY_CATCH);
	}
	wait1Msec(2000);
}

void release_ball()
{
	for(int count = SERVO_CATCH_POS; count >= SERVO_START_POS; count--){ //swing up
		setServo(grabber, count);
		wait1Msec(DELAY_RELEASE);
	}
	wait1Msec(2000); //wait 2s for ball to roll down
}

void initialize() //initialize all values
{
	isBall = false;
	isOpponent = false;
	isBallIn = false;
	isCollectionArea = false;
	isFrontLeftClear = true;
	isFrontRightClear = true;
	isBackLeftClear = true;
	isBackRightClear = true;
	setServo(grabber, SERVO_START_POS);
}
//end of functions

//start of side tasks
task detect_ball_opponent()
{
	while(true){
		wait1Msec(50);
		lowervalue = (SensorValue(lowersensor)+SensorValue(lowersensor)+SensorValue(lowersensor)+SensorValue(lowersensor))/4;
		uppervalue = (SensorValue(uppersensor)+SensorValue(uppersensor)+SensorValue(uppersensor)+SensorValue(uppersensor))/4;
		shortvalue = (SensorValue(shortsensor)+SensorValue(shortsensor)+SensorValue(shortsensor)+SensorValue(shortsensor))/4;

		if(isBallIn == false){
			if((lowervalue > LOWER_SENSOR_THRESHOLD_LOW && lowervalue < LOWER_SENSOR_THRESHOLD_HIGH) || (shortvalue > SHORT_SENSOR_THRESHOLD_LOW && shortvalue < SHORT_SENSOR_THRESHOLD_HIGH)){
				if(uppervalue > UPPER_SENSOR_THRESHOLD){ //uppersensor detects something, tune
					//if(SensorValue(lowersensor) >= SensorValue(uppersensor) + 700) //TUNE -> TO DIFFERENTIATE BALL/OPPONENT
					//{
					//	isBall = true;
					//	isOpponent = true;
					//}
					//else
					//{
					//	isBall = false;
					//	isOpponent = true;
					//}
					isBall = false;
					isOpponent = true;
				}
				else{
					isBall = true; //WHAT IF IT DETECTS COLLECTION PLACE? -> MAKE SURE ROBOT ALWAYS FAR FROM COLLECTION/ FACING FORWARD?
					isOpponent = false;
				}
			}
			else{
				isBall = false;
				isOpponent = false;
			}
		}
		else if(isBallIn == true){ //returning to collection, bumper 0 means pressed
			if(SensorValue(bumperBR) == 0 && SensorValue(rearsensor) > 1000)// && SensorValue(bumperBL) == 0/* && SensorValue(rearsensor) < 500*/) //rear sensor doesn't detect anything
				isCollectionArea = true;
			//else if(SensorValue(rearsensor) < 2500 && SensorValue(rearsensor) > 500) //PLACE REAR SENSOR HIGHER THAN COLLECTION AREA
			//	isOpponent = true;
		}
	}
}

task detect()
{
	while(true){
		//start of boundary detection
		FLval1 = SensorValue(color_FrontLeft);
		FRval1 = SensorValue(color_FrontRight);
		BLval1 = SensorValue(color_BackLeft);
		BRval1 = SensorValue(color_BackRight);

		wait1Msec(50); //tune

		FLval2 = SensorValue(color_FrontLeft);
		FRval2 = SensorValue(color_FrontRight);
		BLval2 = SensorValue(color_BackLeft);
		BRval2 = SensorValue(color_BackRight);

		if(FLval1 - FLval2  > 25) //tune
			isFrontLeftClear = false;
		else
			isFrontLeftClear = true;

		if(FRval1 - FRval2 > 25) //tune
			isFrontRightClear = false;
		else
			isFrontRightClear = true;

		if(BLval1 - BLval2 > 25) //tune
			isBackLeftClear = false;
		else
			isBackLeftClear = true;

		if(BRval1 - BRval2 > 25) //tune
			isBackRightClear = false;
		else
			isBackRightClear = true;
		//end of boundary detection

		read_compass();
	}
}
//end of side tasks


task main()
{
	startTask(detect);
	startTask(detect_ball_opponent);
	setServo(grabber, SERVO_START_POS);

	while(SensorValue(masterswitch) == 1) {}

	while(true){
		initialize();

		//to chiong until halfway, tune timer
		clearTimer(T1);
		while(time1[T1] < 2000){
			state = "moving forward";
			maneuver(); //only if boundary is detected, see function
			move(1, 60); //move forward
		}

		while(isBall == false){ //move and search until ball detected
			clearTimer(T1);
			while(time1[T1] < 2000 && isBall == false){ //move 2s or until ball is detected
				state = "moving forward";
				maneuver(); //only if boundary is detected, see function
				move(1, 45); //move forward
			}

			halt();

			if(isBall == false) //if after moving still doesn't detect the ball
				scan(); //rotate to scan
		}

		while(isBall == true && isBallIn == false) //approach ball
		{
			while(isBall == true && SensorValue(lowersensor) < 2100){ //approach until ball is within range (FINE TUNE)
				state = "approaching ball";
				move(1,30); //if suddenly ball gone, it will stop moving
				maneuver(); //do we need to care abt boundary when chasing ball? what if ball is at the corner?

				////if ball too close
				//if(SensorValue(lowersensor) < 1700 && SensorValue(lowersensor) > 1400 && SensorValue(shortsensor) < 2500 && SensorValue(shortsensor) > 1500){
				//	move(-1, 30);
				//	wait1Msec(1000); //move back 1s
				//}
			}

			halt();

			if(isBall == true && SensorValue(lowersensor) > 1900 && SensorValue(lowersensor) < 2200){ //to check whether ball is within capturing range FINE TUNE if ball suddenly gone, skip this function
				state = "catching ball";
				grab_ball();
				isBallIn = true; //ONLY FOR TESTING
			}
			else
				scan();

			//in case ball gone while approaching, the code will loop from beginning (search ball)

			//if(/*SensorValue(shortsensor) > 1200*/ SensorValue(lowersensor) < 2500) //FINE TUNE, if after grab_ball is done, ball is in
			//	isBallIn = true;
			//else //rotate until find ball, ball should be nearby (logically)
			//	scan();
		}

		while(isBallIn == true && isCollectionArea == false){ //if ball is grabbed, return to collection, MAKE SURE CANNOT LOSE HOLD OF BALL
			align();

			while(isCollectionArea == false){ //need to add codes to task "detection" to check for collection area
				state = "returning home";
				maneuver();
				move(-1, 45); //move backwards
			}
			halt();

			release_ball(); //release ball to collection
			break;
			//after releasing, loop back to top
		}
	}
}
