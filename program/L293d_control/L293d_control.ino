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

#define wheel_speed_pin 3

//contorl ball
#define motor_5_A     8
#define motor_5_B     9
#define motor_6_A     10
#define motor_6_A     11

#define STOP                0
#define FOWARD              1
#define FOWARD_RIGHT        2
#define RIGHT               3
#define BACKWARD_RIGHT      4
#define BACKWARD            5
#define BACKWARD_LEFT       6
#define LEFT                7
#define FOWARD_LEFT         8

int carsignal,motor_5_signal,motor_6_signal;
bool moveway[9]={ B00000000,
                  B10101010,
                  B00000000,
                  B00000000,
                  B00000000,
                  B00000000,
                  B00000000,
                  B00000000,
                  B00000000
                };

void signal_load_595(){
  digitalWrite(LATCH_595, LOW); 
  shiftOut(DATA_595, CLK_595, LSBFIRST, motor_5_signal*8+motor_6_signal*2);
  shiftOut(DATA_595, CLK_595, LSBFIRST, carsignal);
  digitalWrite(LATCH_595, HIGH);
}

void carmove_signal(int direction, int motor_speed){
  carsignal = moveway[direction];
  analogWrite(wheel_speed_pin ,motor_speed);
}

void setup(){
    Serial.begin(9600);
}
void loop(){
  carmove_signal(FOWARD,1023);
  signal_load_595();
}