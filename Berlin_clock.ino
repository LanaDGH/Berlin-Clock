// This is an Arduino program that drives the display of the Berlin clock on an 8x8 LED matrix
// https://en.wikipedia.org/wiki/Mengenlehreuhr

// The matrix should look like this at 23:59, when all LEDs used by the clock are ON:

// 10001001
// 00101010
// 01010110
// 10101000
// 01011001
// 00100010
// 01100100
// 10001000

#include <virtuabotixRTC.h>

virtuabotixRTC myRTC(11, 12, 13);

#define ROW_1 2
#define ROW_2 3
#define ROW_3 4
#define ROW_4 5
#define ROW_5 6
#define ROW_6 7
#define ROW_7 8
#define ROW_8 9

#define COL_1 10
#define COL_2 1 //11
#define COL_3 A5 //12
#define COL_4 A4 //13
#define COL_5 A0
#define COL_6 A1
#define COL_7 A2
#define COL_8 A3

const byte rows[] = {
    ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8
};
const byte col[] = {
  COL_1,COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8
};

// The display buffer
byte MATRIX[]={B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000};

void setup() 
{   
  //Uncomment next line if you want to set up the time
  //myRTC.setDS1302Time(00, 57, 16, 7, 24, 7, 2022);
    
    // Set all used pins to OUTPUT
    // This is very important! If the pins are set to input
    // the display will be very dim.
    for (byte i = 0; i <= 13; i++)
        pinMode(i, OUTPUT);
    
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    pinMode(A4, OUTPUT);
    pinMode(A5, OUTPUT);
}

void loop() {
int bseconds, bhours5, bhours1, bminutes5, bminutes1;
int *berlinTime;

// No delay will make it appear brighter
delay(5);

clearDisplay();

myRTC.updateTime();

int seconds = myRTC.seconds;
int minutes = myRTC.minutes;
int hours = myRTC.hours;

berlinTime = getBerlinTime(seconds, minutes, hours);
bseconds = *(berlinTime + 0);
bhours5 = *(berlinTime + 1);
bhours1 = *(berlinTime + 2);
bminutes5 = *(berlinTime + 3);
bminutes1 = *(berlinTime + 4);

makeMatrix(bseconds, bhours5, bhours1, bminutes5, bminutes1);

displayScreen(MATRIX);
}

void clearDisplay(){
  for(int i=0; i<8; i++){
    MATRIX[i]=B00000000;  
  }
}

int * getBerlinTime(int seconds, int minutes, int hours) {
  static int b[5];
  //Switch "normal" time to "Berlin" time - compatible with the Berlin clock
  //Value of a LED diode turned ON can be as following:
 
  //Value of a LED diode is one second
  b[0] = seconds%2; //can be 0 or 1 - first LED on the clock is ON or OFF
  //Value of a LED diode is five hours
  b[1] = hours/5; //0 to 4 - number of LED diodes of the same value which are ON (applies to the following lines)
  //Value of a LED diode is one hour
  b[2] = hours%5; //0 to 4
  //Value of a LED diode is five minutes
  b[3] = minutes/5; //0 to 11 - number of LED diodes in the diagonal + three LED diodes below
  //Value of a LED diode is one minute
  b[4] = minutes%5; //0 to 4

  return b;
}

void makeMatrix(int bseconds, int bhours5, int bhours1, int bminutes5, int bminutes1){
  if(bseconds==1){
    MATRIX[0]=MATRIX[0]|B10000000;
  }

  for(int i=0; i<bhours5; i++){
    MATRIX[3-i]=MATRIX[3-i]|(B10000000>>i);  
  }

  for(int i=0; i<bhours1; i++){
    MATRIX[4-i]=MATRIX[4-i]|(B01000000>>i);  
  }

  //Turning on the LEDs in the third row
  //every third LED in this row is below the diagonal
  //minutes are grouped by fifteen, which is the value of three consecutive LEDs
  //on the original Berlin clock, every third light in the third row is marked with a different color
  //and on this watch, every third light in the third row is lowered below the diagonal
  
  //first we light the LEDs in groups of three LEDs
  for(int i=0; i<bminutes5/3; i++){
    MATRIX[7-2*i]=MATRIX[7-2*i]|(B10000000>>(2*i));
    MATRIX[6-2*i]=MATRIX[6-2*i]|(B01000000>>(2*i));
    MATRIX[6-2*i]=MATRIX[6-2*i]|(B00100000>>(2*i));
  }
  //then we turn on the rest of the LEDs diagonally - one or two
  int p=bminutes5/3;
  if(bminutes5%3==1)
    MATRIX[7-p*2]=MATRIX[7-p*2]|(B10000000>>p*2);
  else if(bminutes5%3==2){
    MATRIX[7-p*2]=MATRIX[7-p*2]|(B10000000>>p*2);
    MATRIX[6-p*2]=MATRIX[6-p*2]|(B01000000>>p*2);
  }
  
  for(int i=0; i<bminutes1; i++){
    MATRIX[7-i]=MATRIX[7-i]|(B00001000>>i);  
  }
}

void displayScreen(byte znak[]){
  for(int i=0; i<8; i++){
    digitalWrite(rows[i], HIGH); 
    for(int j=0; j<8; j++){
      int n = ~znak[i]<<j & 10000000;
        digitalWrite(col[j], n);
        delayMicroseconds(100);
        digitalWrite(col[j], HIGH);
    }
    digitalWrite(rows[i], LOW);
  }
}
