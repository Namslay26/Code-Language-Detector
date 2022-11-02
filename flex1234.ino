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

#define STRAIGHT 0
#define FOLDED 1
#define NUMFINGERS 5

#define BEGIN_WAIT 10
#define STRAIGHT_WAIT 15
#define BUFFER_WAIT 25
#define FOLDED_TIME 30

#define PINKY 0
#define RING 1
#define MIDDLE 2
#define INDEX 3
#define THUMB 4

struct flex_val {
  int min_val;
  int max_val;
  int cur_val;
};

struct flex_val fv[NUMFINGERS][2];


char buffer[256];

void setup()
{
  Serial.begin(9600);
  memset (fv, 0, sizeof (fv));
  unsigned long myTime = millis();
  while (myTime < BEGIN_WAIT * 1000)
  {
    myTime = millis();
  }
  Serial.println("Keep it straight");
  while (myTime < STRAIGHT_WAIT * 1000)
  {
    myTime = millis();
    calibrate_fingers(0);
  }
  Serial.println("Relax");
  while (myTime < BUFFER_WAIT * 1000)
  {
    myTime = millis();
  }
  Serial.println("Keep it folded");
  while (myTime < FOLDED_TIME * 1000)
  {
    myTime = millis();
    calibrate_fingers(1);
  }

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


void updateFinger(int valuefinger, int finger, int fold){
  if (fv[finger][fold].cur_val==0){
    fv[finger][fold].min_val = valuefinger;
    }
  if (valuefinger>fv[finger][fold].max_val){
  fv[finger][fold].max_val=valuefinger;
  }
  else if(valuefinger<=fv[finger][fold].min_val){ 
  fv[finger][fold].min_val=valuefinger;
  }
  fv[finger][fold].cur_val=valuefinger;

}

/*int isFingerFolded(int finger, int valuefinger){
 if ((fv[finger][0].min_val<=valuefinger) && (fv[finger][0].max_val>=valuefinger)){
    return 0;
 }
 else if ((fv[finger][1].min_val<=valuefinger)&&(fv[finger][1].min_val<=valuefinger)){
  return 1;
 }
 else{
  return 2;
 }
}*/
int isFingerFolded(int finger, int valuefinger){
 if ((fv[finger][1].min_val<=valuefinger)&&(fv[finger][1].min_val<=valuefinger)){
  return 1;
 }
 else{
  return 0;
 }
}

void dumpcalivalue(){

  for (int i =0;i<5;i++){
    sprintf(buffer, "STRAIGHT=[Finger:%d]:min-%d,max-%d,current-%d",i,fv[i][0].min_val,fv[i][0].max_val,fv[i][0].cur_val);
    Serial.println(buffer);
    sprintf(buffer, "FOLD=[Finger:%d]:min-%d,max-%d,current-%d",i,fv[i][1].min_val,fv[i][1].max_val,fv[i][1].cur_val);
    Serial.println(buffer);
    }
}
int giveCharacter(int isThumbFolded ,int isIndexFolded, int isMiddleFolded, int isRingFolded, int isPinkyFolded ){
    int gesture = 0;
    if ((isThumbFolded == 1)&&(isIndexFolded==1)&&(isMiddleFolded==1)&&(isRingFolded==1)&&(isPinkyFolded==1)){
      gesture = 0;
    }
  if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==1)&&(isRingFolded==1)&&(isPinkyFolded==1)){
      gesture = 1;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==1)&&(isPinkyFolded==1)){
      gesture = 2;
    }
    if ((isThumbFolded == 0)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==1)&&(isPinkyFolded==1)){
      gesture = 3;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==0)&&(isPinkyFolded==0)){
      gesture = 4;
    }
    if ((isThumbFolded == 0)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==0)&&(isPinkyFolded==0)){
      gesture = 5;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==0)&&(isPinkyFolded==1)){
      gesture = 6;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==1)&&(isPinkyFolded==0)){
      gesture = 7;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==1)&&(isRingFolded==0)&&(isPinkyFolded==0)){
      gesture = 8;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==1)&&(isMiddleFolded==0)&&(isRingFolded==0)&&(isPinkyFolded==0)){
      gesture = 9;
    }
}

void loop()
{
  
  //reading the values
  value_pinky = analogRead(flexPin_pinky);
  value_ring = analogRead(flexPin_ring);
  value_middle = analogRead(flexPin_middle);
  value_index = analogRead(flexPin_index);
  value_thumb = analogRead(flexPin_thumb);

  int isPinkyFolded = isFingerFolded(PINKY, value_pinky);
  int isRingFolded = isFingerFolded(RING, value_ring);
  int isMiddleFolded = isFingerFolded(MIDDLE, value_middle);
  int isIndexFolded = isFingerFolded(INDEX, value_index);
  int isThumbFolded = isFingerFolded(THUMB, value_thumb);

  int gesture = 0;
  
  
  sprintf (buffer, "Thumb [%d], Index [%d], Middle[%d], Ring[%d], Pinky [%d]",
      isThumbFolded,
      isIndexFolded,
      isMiddleFolded,
      isRingFolded,
      isPinkyFolded);
      
  
  Serial.println(buffer);

  //Recognising numbers 
  /*if ((isThumbFolded == 1)&&(isIndexFolded==1)&&(isMiddleFolded==1)&&(isRingFolded==1)&&(isPinkyFolded==1)){
      gesture = 0;
    }
  if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==1)&&(isRingFolded==1)&&(isPinkyFolded==1)){
      gesture = 1;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==1)&&(isPinkyFolded==1)){
      gesture = 2;
    }
    if ((isThumbFolded == 0)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==1)&&(isPinkyFolded==1)){
      gesture = 3;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==0)&&(isPinkyFolded==0)){
      gesture = 4;
    }
    if ((isThumbFolded == 0)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==0)&&(isPinkyFolded==0)){
      gesture = 5;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==0)&&(isPinkyFolded==1)){
      gesture = 6;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==0)&&(isRingFolded==1)&&(isPinkyFolded==0)){
      gesture = 7;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==0)&&(isMiddleFolded==1)&&(isRingFolded==0)&&(isPinkyFolded==0)){
      gesture = 8;
    }
    if ((isThumbFolded == 1)&&(isIndexFolded==1)&&(isMiddleFolded==0)&&(isRingFolded==0)&&(isPinkyFolded==0)){
      gesture = 9;
    }*/

  delay(100);
  
}
