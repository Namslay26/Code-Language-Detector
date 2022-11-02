const int flexPin_pinky = A0;//the baseline is 783-790
const int flexPin_ring = A1; //the baseline is 801-812
const int flexPin_middle = A2; //the baseline is 693-703
const int flexPin_index = A3; //the baseline is 736-747
const int flexPin_thumb = A4; //the baseline is 745-759
const int xPin = A5; //the baseline is
const int yPin = A6; //the baseline is

int value_pinky;
int value_ring;
int value_middle;
int value_index;
int value_thumb;
int value_x;
int value_y;

char buffer[256];

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  //reading the values
  value_pinky = analogRead(flexPin_pinky);
  value_ring = analogRead(flexPin_ring);
  value_middle = analogRead(flexPin_middle);
  value_index = analogRead(flexPin_index);
  value_thumb = analogRead(flexPin_thumb);
  //value_x = analogRead(xPin);
  //value_y = analogRead(yPin);
  
  sprintf (buffer, "Thumb [%d], Index [%d], Middle[%d], Ring[%d], Pinky [%d]",
      value_thumb,
      value_index,
      value_middle,
      value_ring,
      value_pinky);
      
  Serial.println(buffer);
  value_pinky  = map(value_pinky, 700, 900, 0, 90);

  //if (value_thumb==741 && value_index==
  delay(100);
}
