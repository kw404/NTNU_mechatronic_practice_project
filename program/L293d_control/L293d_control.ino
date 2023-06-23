#define motor_control_num 6
//move system
#define CLK_595       7
#define LATCH_595     8
#define DATA_595      4

#define motor_LF_A    0
#define motor_LF_B    1
#define motor_RF_A    2
#define motor_RF_B    3
#define motor_LB_A    4
#define motor_LB_B    5
#define motor_RB_A    6
#define motor_RB_B    7

#define wheel_speed_pin     3
#define pingpong_speed_pin  10
#define motor6_speed_pin    11

//contorl ball
#define motor_5_A     8
#define motor_5_B     9
#define motor_6_A     10
#define motor_6_A     11

#define STOP                 0
#define FORWARD              1
#define FORWARD_RIGHT        2
#define RIGHT                3
#define BACKWARD_RIGHT       4
#define BACKWARD             5
#define BACKWARD_LEFT        6
#define LEFT                 7
#define FORWARD_LEFT         8
#define TRUN_RIGHT           9
#define TRUN_LEFT           10

#define clockwise         1
#define counterclockwise  2
#define off               0

#include <Wire.h>
#define ARDUINO_ADDR 0x8
char buf[128];

char val ;
int  count=0;

int carsignal,motor_5_signal,motor_6_signal;
int moveway[11]= { B00000000,
                  B10101010,
                  B10000010,
                  B10010110,
                  B00010100,
                  B01010101,
                  B01000001,
                  B01101001,
                  B01000001,
                  B10011001,
                  B01100110
                };
int pingpong_state[3]{  B10,
                        B01,
                        B00
                      };
int motor6_state[3]  {  B10,
                        B01,
                        B00
                      };

void signal_load_595(){
  digitalWrite(LATCH_595, LOW); 
  shiftOut(DATA_595, CLK_595, LSBFIRST, motor_5_signal*64+motor_6_signal*16);
  shiftOut(DATA_595, CLK_595, LSBFIRST, carsignal);
  digitalWrite(LATCH_595, HIGH);
  carsignal = 0;
}

void carmove_signal(int direction, int motor_speed){
  carsignal = moveway[direction];
  analogWrite(wheel_speed_pin ,motor_speed);
}

void pingpong_shit(int state, byte speed_1){
  motor_5_signal = pingpong_state[state];
  analogWrite(10,255);
}

void print_mainMenu(){
  char print_sim_joycontroler[3][3]={
    'q', 'w', 'e',
    'a', 'k', 'd',
    'z', 's', 'c'
  };
  Serial.println(" ");
  Serial.println("//////////////////////////////////");
  Serial.println("This is arduino car control mode, this is simulate joycontroler:");
  for(int i = 0; i<3; i++){
    for(int j = 0; j<3; j++){
      Serial.print(print_sim_joycontroler[i][j]);
      Serial.print("\t");
    }
    Serial.println(" ");
  }
  Serial.println(" ");
  Serial.println("input 'a' 'd' for trun right or left");
  Serial.println("input 't' for adhust PWM");
  Serial.println("//////////////////////////////////");
  Serial.print("Please enter your instruction: ");
}


/////////////////////////////////////i2c函式//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void receiveEvent(int numBytes){
  if(Wire.available()){       //判斷Wire.available()有沒有訊號
    char c = Wire.read();     //將傳入的訊號Wire.read()指定給字串C
    Serial.println(c);        //透過Serial印出字串C的內容
    val = c;
  }
}

void setup(){
  Wire.begin(ARDUINO_ADDR);
  //Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  Serial.println("START"); 
}
void loop(){

  print_mainMenu();
  while(1){
    if(Serial.available()!= 0){ val = Serial.read(); }
    
    if(Wire.available() != 0){ val = Wire.read(); }

    switch(val){
      case 'W' :
      case 'w' :
        carmove_signal(FORWARD,1023);
        break;
      case 'A' :
      case 'a' :
        carmove_signal(RIGHT,1023);
        break;
      case 'd':
      case 'D':
        carmove_signal(LEFT,1023);
        break;
      case 's':
      case 'S':
        carmove_signal(BACKWARD,1023);
        break;  
      case 'K':    
      case 'k':
        carmove_signal(STOP,1023);
        break;
      case 'E':    
      case 'e':
        carmove_signal(FORWARD_RIGHT,1023);
        break;
      case 'Q':    
      case 'q':
        carmove_signal(FORWARD_LEFT,1023);
        break;
      case 'c':
      case 'C':
        carmove_signal(BACKWARD_RIGHT,1023);
        break;        
      case 'Z':    
      case 'z':
        carmove_signal(BACKWARD_LEFT,1023);
        break;
      case 'l':    
      case 'L':
        carmove_signal(TRUN_RIGHT,1023);
        break;
      case 'J':
      case 'j':
        carmove_signal(TRUN_LEFT,1023);
        break;
      case 'i':
      case 'I':   
        pingpong_shit(clockwise, 255);
      case 'o':
      case 'O':   
        pingpong_shit(off, 255);
      default:
        break;
    }

    signal_load_595();

  }
}
