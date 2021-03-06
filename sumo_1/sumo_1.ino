#define LED_A 13 //Arduino LED
#define LED_LEFT 5
#define LED_RIGHT 11
#define LED_BACK 2
#define BUTTON 12
#define DIP1 15
#define DIP2 14
#define DIP3 16
#define DIP4 17
#define SENS_LEFT A2
#define SENS_RIGHT A1
#define SENS_BACK A4
#define SHARP1 A5
#define SHARP2 A8
#define SHARP3 7
#define SHARP4 A3
#define SHARP5 A7
#define MOTOR_LEFT_PWM 3
#define MOTOR_LEFT_DIR 4
#define MOTOR_RIGHT_PWM 9
#define MOTOR_RIGHT_DIR 10
const int SHARP[5] = {SHARP1, SHARP2, SHARP3, SHARP4, SHARP5};
#define TICK 5
#define LOST_TIME 300 //also for death
#define MAX_SPEED 255
int NORM_SPEED = 150;
#define BLINK_RATE 100
#define EDGE_SPIN_TIME 300
int BACKING_TIME = 500;
#define BACK_SPIN_TIME 80
#define TRICK_TIME 800 //STARTING BEGINNING

void setup() {
	pinMode(LED_A, OUTPUT);
	pinMode(LED_LEFT, OUTPUT);
	pinMode(LED_RIGHT, OUTPUT);
	pinMode(LED_BACK, OUTPUT);
	pinMode(BUTTON,INPUT);
	digitalWrite(BUTTON,HIGH);  
	Serial.begin(9600);
	pinMode(DIP1,INPUT);
	digitalWrite(DIP1,HIGH); 
	pinMode(DIP2,INPUT);
	digitalWrite(DIP2,HIGH);
	pinMode(DIP3,INPUT);
	digitalWrite(DIP3,HIGH); 
	pinMode(DIP4,INPUT);
	digitalWrite(DIP4,HIGH);
	pinMode(SENS_LEFT,INPUT);
	//analogRead(A2)<75
	pinMode(SENS_RIGHT,INPUT);
	//analogRead(A1)<75
	pinMode(SENS_BACK,INPUT);
	//analogRead(A4)<75
	/*
	  pinMode(SHARP1,INPUT);
	  //#define SHARP1 !digitalRead(A5) 
	  pinMode(SHARP2,INPUT);
	  //#define SHARP2 !digitalRead(A3)
	  pinMode(SHARP3,INPUT);
	  //#define SHARP3 !digitalRead(7)
	  pinMode(SHARP4,INPUT);
	  //#define SHARP4 !digitalRead(A8)
	  pinMode(SHARP5,INPU
	  T);
	  //#define SHARP5 !digitalRead(A7)
	  */
	for (int i = 0; i < 5; ++i) {
		pinMode(SHARP[i], INPUT);
	}
	pinMode(MOTOR_LEFT_PWM,OUTPUT);//kreisais PWM
	pinMode(MOTOR_LEFT_DIR,OUTPUT);//kreisais Direction
	pinMode(MOTOR_RIGHT_PWM,OUTPUT);//labais PWM
	pinMode(MOTOR_RIGHT_DIR,OUTPUT);//labais Direction
}

void move (int leftSpeed, int rightSpeed) {
	if (leftSpeed < 0) {
		analogWrite(MOTOR_LEFT_PWM, -1 * leftSpeed);
		digitalWrite(MOTOR_LEFT_DIR, HIGH);
	}
	else {
		analogWrite(MOTOR_LEFT_PWM, leftSpeed);
		digitalWrite(MOTOR_LEFT_DIR, LOW);
	}
	if (rightSpeed < 0) {
		analogWrite(MOTOR_RIGHT_PWM, -1 * rightSpeed);
		digitalWrite(MOTOR_RIGHT_DIR, HIGH);
	}
	else {
		analogWrite(MOTOR_RIGHT_PWM, rightSpeed);
		digitalWrite(MOTOR_RIGHT_DIR, LOW);
	}
}

typedef enum {RIP, BEGINNING, WALL_FRONT, WALL_BACK, BACK_TURN, WALL_LEFT, WALL_LEFT_REV, WALL_RIGHT, WALL_RIGHT_REV, WALL_BACK_LEFT, WALL_BACK_RIGHT, SEARCHING, ATTACKING} stateType;

void loop() { //mūžīgais cikls
	
	long time = millis();
	move(0,0);//Izsaucam funkciju motoru apstādināšanai
	delay(5);
	if (!digitalRead(DIP4)) {
		NORM_SPEED = 200;
		BACKING_TIME = 800;
		digitalWrite(LED_A, HIGH);
	}
	else
		digitalWrite(LED_A, LOW);
	if (!digitalRead(DIP1))
		digitalWrite(LED_BACK, HIGH);
	else
		digitalWrite(LED_BACK, LOW);
	if (!digitalRead(BUTTON))//robots sāk darboties ar Button1 nospiešanu
	{
		int blinkTime = millis();
		digitalWrite(LED_RIGHT, HIGH);
		digitalWrite(LED_LEFT, LOW);
		while (!digitalRead(BUTTON)){
			time = millis();
			if (time - blinkTime > 2 * BLINK_RATE) {
				blinkTime += 2 * BLINK_RATE;
				digitalWrite(LED_RIGHT, HIGH);
				digitalWrite(LED_LEFT, LOW);
			} else if (time - blinkTime > BLINK_RATE) {
				digitalWrite(LED_LEFT, HIGH);
				digitalWrite(LED_RIGHT, LOW);
			}
			delay(TICK);
		} //Gaidām, līdz atlaiž pogu
		//5-sec delay, kā noteikumos
		digitalWrite(LED_LEFT, LOW);
		digitalWrite(LED_RIGHT, HIGH);
		delay(1000);
		digitalWrite(LED_BACK, HIGH);
		delay(1000);
		digitalWrite(LED_LEFT, HIGH);
		delay(1000);
		digitalWrite(LED_LEFT, LOW);
		delay(1000);
		digitalWrite(LED_BACK, LOW);
		delay(1000);
		digitalWrite(LED_RIGHT, LOW);

		int lastSeen = 4;
		long deadTime = 0;
		long startTime = 0;
		long lost = millis();
		long stepTime = time;
		stateType state = BEGINNING;
		
		while (digitalRead(BUTTON) and state != RIP) // robots apstājas līdz ar button1 nospiešanu
		{
			time = millis();
			int bottom = 0; //sensor states
			if (analogRead(SENS_LEFT) < 75) {
				digitalWrite(LED_LEFT, HIGH);
				bottom += 1;
			} else
				digitalWrite(LED_LEFT, LOW);
			if (analogRead(SENS_RIGHT) < 75) {
				digitalWrite(LED_RIGHT, HIGH);
				bottom += 2;
			} else
				digitalWrite(LED_RIGHT, LOW);
			if (analogRead(SENS_BACK) < 75) {
				digitalWrite(LED_BACK, HIGH);
				bottom += 4;
			} else
				digitalWrite(LED_BACK, LOW);
			bool sharp[5];
			for (int i = 0; i < 5; ++i) {
				sharp[i] = !digitalRead(SHARP[i]);
			}
			boolean located = (sharp[0] || sharp[1] || sharp[2] || sharp[3] || sharp[4]);
			if (sharp[1] && sharp[2] && sharp[3])
				lastSeen = 4;
			else if (sharp[1] && sharp[2])
				lastSeen = 3;
			else if (sharp[1])
				lastSeen = 2;
			else if (sharp[3] && sharp[2])
				lastSeen = 5;
			else if (sharp[3])
				lastSeen = 6;
			else if (sharp[0])
				lastSeen = 1;
			else if (sharp[4])
				lastSeen = 7;
			 
			if (located)
				lost = time;
			else if (time - lost > LOST_TIME) {
				if (lastSeen < 5)
					lastSeen = 0;
				else
					lastSeen = 10;
			}
	  
			switch (bottom) {
			case 1:
				if (state != WALL_LEFT_REV && (lastSeen == 0 || lastSeen == 10)) {
					move(MAX_SPEED, -1 * NORM_SPEED / 5);
					state = WALL_LEFT;
					stepTime = time;
				}
				else {
					move(-1 * MAX_SPEED, NORM_SPEED / 5);
					state = WALL_LEFT_REV;
					stepTime = time;
				}
				break;
			case 2:
				stepTime = time;
				if (state != WALL_RIGHT_REV && (lastSeen == 0 || lastSeen == 10)) {
					state = WALL_RIGHT;
					move(-1 * NORM_SPEED / 5, MAX_SPEED);
				} else {
					state = WALL_RIGHT_REV;
					move (NORM_SPEED / 5, -1 * MAX_SPEED);
				}
				break;
			case 3:
				state = WALL_FRONT;
				move (-1 * MAX_SPEED, -1 * MAX_SPEED);
				stepTime = time;
				break;
			case 4:
				state = WALL_BACK;
				move (MAX_SPEED, MAX_SPEED);
				stepTime = time;
				break;
			case 5:
				state = WALL_BACK_LEFT;
				move(MAX_SPEED, MAX_SPEED * 2 / 5);
				stepTime = time;
				break;
			case 6:
				state = WALL_BACK_RIGHT;
				move(MAX_SPEED * 2 / 5, MAX_SPEED);
				stepTime = time;
				break;
			case 7:
				stepTime = time;
				if (deadTime == 0)
					deadTime = time;
				else if (deadTime + LOST_TIME < time) {
					move(0, 0);
					state = RIP;
				}
				break;
			}

			switch (state) {
			case BEGINNING:
				Serial.println("tralala");
				if (located)
					state = ATTACKING;
				else {
					if (digitalRead(DIP1))
						state = SEARCHING;
					else {
						if (startTime == 0)
							startTime = time;
						move(MAX_SPEED, MAX_SPEED);
						if (startTime + LOST_TIME < time)
							state = SEARCHING;
					}
				}
				break;
			case WALL_FRONT:
				if (time - stepTime > BACKING_TIME * 2 / 3 && located)
					state = ATTACKING;
				else if (time - stepTime > BACKING_TIME) {
					stepTime = time;
					state = BACK_TURN;
					if (lastSeen < 4) {
						move(-1 * MAX_SPEED, -1 * MAX_SPEED / 5);
					}
					else {
						move(-1 * MAX_SPEED / 5, -1 * MAX_SPEED);
					}
				}
				break;
			case WALL_BACK:
				if (time - stepTime > BACKING_TIME / 2 && located)
					state = ATTACKING;
				else if (time - stepTime > BACKING_TIME)
					state = SEARCHING;
				break;
			case BACK_TURN:
				if (time - stepTime > BACK_SPIN_TIME)
					state = SEARCHING;
				else if (located)
					state = ATTACKING;
				break;
			case WALL_LEFT:
				if (time - stepTime > 2 * EDGE_SPIN_TIME)
					state = SEARCHING;
				else if (time - stepTime > EDGE_SPIN_TIME) {
					move(NORM_SPEED, NORM_SPEED);
					if (located)
						state = ATTACKING;
				}
				break;
			case WALL_LEFT_REV:
				if (time - stepTime > 2 * EDGE_SPIN_TIME)
					state = SEARCHING;
				else if (time - stepTime > EDGE_SPIN_TIME) {
					move (-1 * NORM_SPEED, NORM_SPEED);
					if (located)
						state = ATTACKING;
				}
				break;
			case WALL_RIGHT:
				if (time - stepTime > 2 * EDGE_SPIN_TIME)
					state = SEARCHING;
				else if (time - stepTime > EDGE_SPIN_TIME) {
					move(NORM_SPEED, NORM_SPEED);
					if (located)
						state = ATTACKING;
				}
				break;
			case WALL_RIGHT_REV:
				if (time - stepTime > 2 * EDGE_SPIN_TIME)
					state = SEARCHING;
				else if (time - stepTime > EDGE_SPIN_TIME) {
					move(NORM_SPEED, -1 * NORM_SPEED);
					if (located)
						state = ATTACKING;
				}
				break;
			case WALL_BACK_LEFT:
				if (time - stepTime > 1.5 * EDGE_SPIN_TIME)
					state = SEARCHING;
				if (located)
					state = ATTACKING;
				break;
			case WALL_BACK_RIGHT:
				if (time - stepTime > 1.5 * EDGE_SPIN_TIME)
					state = SEARCHING;
				if (located)
					state = ATTACKING;
				break;
			case SEARCHING:
				if (located)
					state = ATTACKING;
				else {
					switch (lastSeen) {
					case 0:
						move(-0.5f * NORM_SPEED, NORM_SPEED);
						break;
					case 8:
						move(NORM_SPEED, -0.5f * NORM_SPEED);
						break;
					case 1:
						move (-2 * NORM_SPEED / 3, NORM_SPEED);
						break;
					case 7:
						move(NORM_SPEED, -2 * NORM_SPEED / 3);
						break;
					case 2:
						move (NORM_SPEED / 3, NORM_SPEED);
						break;
					case 6:
						move(NORM_SPEED, NORM_SPEED / 3);
						break;
					case 3:
						move(NORM_SPEED * 2 / 3, NORM_SPEED);
						break;
					case 5:
						move(NORM_SPEED, NORM_SPEED * 2 / 3);
						break;
					case 4:
						move(NORM_SPEED, NORM_SPEED);
					}
				}
				break;
			case ATTACKING:
				switch (lastSeen) {
				case 1:
					move(-0.5f * MAX_SPEED, MAX_SPEED);
					break;
				case 7:
					move (MAX_SPEED, -0.5f * MAX_SPEED);
					break;
				case 2:
					move(0, MAX_SPEED);
					break;
				case 6:
					move(MAX_SPEED, 0);
					break;
				case 3:
					move(MAX_SPEED / 2, MAX_SPEED);
					break;
				case 5:
					move (MAX_SPEED, MAX_SPEED / 2);
					break;
				case 4:
					move(MAX_SPEED, MAX_SPEED);
					break;
				}
				break;
			}
			delay(TICK);
			//Serial.println(!digitalRead(DIP2));
		}
		digitalWrite(LED_LEFT, LOW);
		digitalWrite(LED_RIGHT, LOW);
		digitalWrite(LED_BACK, LOW);
		while (!digitalRead(BUTTON)){}
	}
}  


