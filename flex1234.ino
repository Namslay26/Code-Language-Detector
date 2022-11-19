const int flexPin_pinky = A0;
const int flexPin_ring = A1; 
const int flexPin_middle = A2; 
const int flexPin_index = A3; 
const int flexPin_thumb = A4; 


int value_pinky;
int value_ring;
int value_middle;
int value_index;
int value_thumb;


#define STRAIGHT 0 	// Finger is straight
#define FOLDED 1	// Finger is folded
#define NUMFINGERS 5	// Not Hrithik roshan 
#define HISTORYLEN 3 //History's index 

/* calibration time periods */
#define BEGIN_WAIT 10	// waits for 10 seconds ...
#define STRAIGHT_WAIT 15  // calibrates finger holding straight for 15 mintues 
#define BUFFER_WAIT 25 	// in this 25 seconds, fold the fingers tight
#define FOLDED_TIME 30 // calibrates finger 


/* finger identifiers */
#define PINKY 0
#define RING 1
#define MIDDLE 2
#define INDEX 3
#define THUMB 4

/* finger data */
struct flex_val {
  int min_val;		// minval observed during calibration
  int max_val;		// maxval observed durign calibration 
  int cur_val;		// .. the current value 
//  int state;		// state of the finger determined comparing with calibration
//  int history[5];
};

struct flex_data {
	int			state;   // state of the finger ...
	int			curr_value; // current value 
	int 			history[HISTORYLEN];	// history 
};

int debug = 0; 
const int sleep_time = 200;

/* history counter */
int index=0;
/* this holds the calibration data for each finger, for straight and fold */
struct flex_val fv[NUMFINGERS][2];

/* this holds the current and history information and the state determined for each finger */
struct flex_data fd[NUMFINGERS];

char buffer[256];


void calibrate_fingers (int fold);
void updateFinger(int valuefinger, int finger, int fold);
int isFingerFolded (int finger, int valuefinger);
void dumpcalivalue();
void compute_state();
void generate_message();
int  translate_to_digit (int value);

/* setup routine configures the serial port and
 * performs the calibration of the flex sensors
 * This is run only once. 
 * The calibration has to be done everytime because
 * the flex sensor doesnt seem to be consistent 
 */
void setup()
{
	Serial.begin(9600);
	memset (fv, 0, sizeof (fv));
	memset (fd, 0, sizeof (fd));

	unsigned long myTime = millis();

	/* initial wait */
	while (myTime < BEGIN_WAIT * 1000)
	{
		myTime = millis();
	}
	Serial.println("Keep it straight");

	/* calibrate straight */
	while (myTime < STRAIGHT_WAIT * 1000)
	{
		myTime = millis();
		calibrate_fingers(0);
	}
	Serial.println("Relax");

	/* give time for user to fold fingers */
	while (myTime < BUFFER_WAIT * 1000)
	{
		myTime = millis();
	}
	Serial.println("Keep it folded");

	/* calibrate the folded position  */
	while (myTime < FOLDED_TIME * 1000)
	{
		myTime = millis();
		calibrate_fingers(1);
	}
	/* dump the calibration data for reference */
	dumpcalivalue();
}


void calibrate_fingers (int fold)
{
	value_pinky = analogRead(flexPin_pinky);
	value_ring = analogRead(flexPin_ring);
	value_middle = analogRead(flexPin_middle);
	value_index = analogRead(flexPin_index);
	value_thumb = analogRead(flexPin_thumb);

	updateFinger(value_pinky, PINKY, fold);
	updateFinger(value_ring, RING, fold);
	updateFinger(value_middle, MIDDLE, fold);
	updateFinger(value_index, INDEX, fold);
	updateFinger(value_thumb, THUMB, fold);

}


void updateFinger(int valuefinger, int finger, int fold)
{
	if (fv[finger][fold].cur_val==0)
	{
		fv[finger][fold].min_val = valuefinger;
	}
	if (valuefinger>fv[finger][fold].max_val)
	{
		fv[finger][fold].max_val=valuefinger;
	}
	else if(valuefinger<=fv[finger][fold].min_val)
	{ 
		fv[finger][fold].min_val=valuefinger;
	}
	fv[finger][fold].cur_val=valuefinger;
}


int isFingerFolded (int finger, int valuefinger)
{
	if ((fv[finger][1].min_val<=valuefinger)&&(fv[finger][1].min_val<=valuefinger))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void dumpcalivalue()
{

	for (int i =0;i<NUMFINGERS;i++)
	{
		sprintf(buffer, "STRAIGHT=[Finger:%d]:min-%d,max-%d,current-%d",i,fv[i][0].min_val,fv[i][0].max_val,fv[i][0].cur_val);
		Serial.println(buffer);
		sprintf(buffer, "FOLD=[Finger:%d]:min-%d,max-%d,current-%d",i,fv[i][1].min_val,fv[i][1].max_val,fv[i][1].cur_val);
		Serial.println(buffer);
	}
}

void loop()
{

	int values[NUMFINGERS]; 
	values[PINKY] = analogRead(flexPin_pinky);
	values[RING] = analogRead(flexPin_ring);
	values[MIDDLE] = analogRead(flexPin_middle);
	values[INDEX] = analogRead(flexPin_index);
	values[THUMB] = analogRead(flexPin_thumb);

	for (i = 0;  i < NUMFINGERS; i++)
	{
		fd[i].curr_value = values[i];
		fd[i].history[index] = isFingerFolded (i, values[i]);
	}
	index = (index + 1) % HISTORYLEN;
	compute_state ();
	generate_message ();
	if (debug)
	{
		sprintf (buffer, "Index: %d Thumb [%d], Index [%d], Middle[%d], Ring[%d], Pinky [%d]",
				index, values[THUMB], values[INDEX], values[MIDDLE], values[RING], values[PINKY]);
		Serial.println(buffer);
	}
	delay(sleep_time);

}

void compute_state()
{
	int i,j = 0;
	int sum = 0;

	for (i = 0; i < NUMFINGERS; i ++) 
	{
		sum =  0;
		for (j = 0; j < HISTORYLEN; j ++) 
		{
			sum = sum + fd[i].history[j];
		}
		if (sum == HISTORYLEN)
		{
			fd[i].state = 1;
		}
		else if (sum == 0)
		{
			fd[i].state = 0;	
		}
		else
		{
			/* ... */
		}
	}
}
/*Here, we convert the values recieved into binary. After conversion, we use the translate_to_digit function to translate the binary values into digits.*/
void generate_message()
{
	int value = fd[THUMB].state<<4 + fd[INDEX].state<<3 + fd[MIDDLE].state<<2 + fd[RING].state<<1 + fd[PINKY].state;
	int answer = translate_to_digit(value);
	if (1)
	{
		sprintf(buffer, "Translated to %d", answer);
		Serial.println(buffer);
	}
}
/*this function takes binary value as input and returns the respective digit*/
int  translate_to_digit (int value)
{
    if(fd[THUMB].state==1 && fd[INDEX].state==1 && fd[MIDDLE].state==1 && fd[RING].state==1 && fd[PINKY].state==1){return 0;}
    if(fd[THUMB].state==1 && fd[INDEX].state==0 && fd[MIDDLE].state==1 && fd[RING].state==1 && fd[PINKY].state==1){return 1;}
    if(fd[THUMB].state==1 && fd[INDEX].state==0 && fd[MIDDLE].state==0 && fd[RING].state==1 && fd[PINKY].state==1){return 2;}
    if(fd[THUMB].state==0 && fd[INDEX].state==0 && fd[MIDDLE].state==0 && fd[RING].state==1 && fd[PINKY].state==1){return 3;}
    if(fd[THUMB].state==1 && fd[INDEX].state==0 && fd[MIDDLE].state==0 && fd[RING].state==0 && fd[PINKY].state==0){return 4;}
    if(fd[THUMB].state==0 && fd[INDEX].state==0 && fd[MIDDLE].state==0 && fd[RING].state==0 && fd[PINKY].state==0){return 5;}
    if(fd[THUMB].state==1 && fd[INDEX].state==0 && fd[MIDDLE].state==0 && fd[RING].state==0 && fd[PINKY].state==1){return 6;}
    if(fd[THUMB].state==1 && fd[INDEX].state==0 && fd[MIDDLE].state==0 && fd[RING].state==1 && fd[PINKY].state==0){return 7;}
    if(fd[THUMB].state==1 && fd[INDEX].state==0 && fd[MIDDLE].state==1 && fd[RING].state==0 && fd[PINKY].state==0){return 8;}
    if(fd[THUMB].state==1 && fd[INDEX].state==1 && fd[MIDDLE].state==0 && fd[RING].state==0 && fd[PINKY].state==0){return 9;}
    
    return -1;    
}

