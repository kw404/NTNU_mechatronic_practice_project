#include <Wire.h>
#define ARDUINO_ADDR 0x8
char buf[128];

#include <AFMotor.h>              //引用AFMotor
#include <Wire.h>                 //引用i2c

char val ;                        //車子動作指令
int  init_motor_speed = 200;      //初始車速pwm
int  count=0;                     //車子動作計數器

//馬達物件宣告
AF_DCMotor L_motor_one(1,MOTOR12_2KHZ); // 設定【L_motor】為左側馬達,並接在1號port
AF_DCMotor R_motor_one(2,MOTOR12_2KHZ); // 設定【R_motor】為右側馬達,並接在2號port
AF_DCMotor R_motor_two(3,MOTOR12_2KHZ); // 設定【R_motor】為右側馬達,並接在3號port
AF_DCMotor L_motor_two(4,MOTOR12_2KHZ); // 設定【L_motor】為左側馬達,並接在4號port

///////////////////////////////////車子動作函式////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void forword(){
  L_motor_one.run(FORWARD);  
  R_motor_one.run(FORWARD);
  L_motor_two.run(FORWARD);  
  R_motor_two.run(FORWARD);
}
void forword_right(){
  L_motor_one.run(RELEASE);  
  R_motor_one.run(FORWARD);
  L_motor_two.run(FORWARD);  
  R_motor_two.run(RELEASE);
}
void forword_left(){
  L_motor_one.run(FORWARD);  
  R_motor_one.run(RELEASE);
  L_motor_two.run(RELEASE);  
  R_motor_two.run(FORWARD);
}
void right(){
  L_motor_one.run(BACKWARD);  
  R_motor_one.run(FORWARD);
  L_motor_two.run(FORWARD);  
  R_motor_two.run(BACKWARD);
}
void backward(){
  L_motor_one.run(BACKWARD);  
  R_motor_one.run(BACKWARD);
  L_motor_two.run(BACKWARD);  
  R_motor_two.run(BACKWARD);
}
void backward_right(){
  L_motor_one.run(RELEASE);  
  R_motor_one.run(FORWARD);
  L_motor_two.run(BACKWARD);  
  R_motor_two.run(RELEASE);
}
void backward_left(){
  L_motor_one.run(BACKWARD);  
  R_motor_one.run(RELEASE);
  L_motor_two.run(RELEASE);  
  R_motor_two.run(BACKWARD);
}
void left(){
  L_motor_one.run(FORWARD);  
  R_motor_one.run(BACKWARD);
  L_motor_two.run(BACKWARD);  
  R_motor_two.run(FORWARD);
}
void rest(){
  L_motor_one.run(RELEASE);  
  R_motor_one.run(RELEASE);
  L_motor_two.run(RELEASE);  
  R_motor_two.run(RELEASE);
}
void trun_right(){
  L_motor_one.run(FORWARD);  
  R_motor_one.run(BACKWARD);
  L_motor_two.run(FORWARD);  
  R_motor_two.run(BACKWARD);
}
void trun_left(){
  L_motor_one.run(BACKWARD);  
  R_motor_one.run(FORWARD);
  L_motor_two.run(BACKWARD);  
  R_motor_two.run(FORWARD);
}
/*void PWM_adjust_byChannel(){
  int val = analogRead(PWMSpeedChannel);  
  L_motor_one.setSpeed(val);
  R_motor_one.setSpeed(val);
  L_motor_two.setSpeed(val);
  R_motor_two.setSpeed(val);
}*/
///////////////////////////////////monitor函式////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void print_mainMenu(){
  char print_sim_joycontroler[3][3]={
    'u', 'i', 'o',
    'j', 'k', 'l',
    'm', ',', '.'
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
void PWM_adjust(){
    char instruction = 'n';
    int i_w=1;
    int arg=0;
    int i=1;
    Serial.println(" ");
    Serial.println("//////////////////////////////////");        
    Serial.println("This is PWM output adjustment mode:");
    Serial.println("Current PWM output is %d:");
    Serial.println("If you want to change the arguments？(y/n)");    
    while(i){
      if(Serial.available()!= 0){instruction = Serial.read(); }          
      if(instruction == 'Y'||instruction == 'y'){     
         while(i_w){
           int j =1;
           while(j){if(Serial.available()!= 0){ arg = Serial.parseInt(); j=0; }}//debug 
           Serial.print("Input 0~255 for wheel speed:");
           j =1;
           while(j){
            if(Serial.available()!= 0){ arg = Serial.parseInt(); j=0; }
           }                    
           Serial.println(arg);
           if(0>arg || 255<arg){Serial.print("re-input again！(0~255)");}
           else{init_motor_speed = arg; i_w=0; i=0;}
         }                
      } 
      else if(instruction == 'N'||instruction == 'n'){i=0;}     
    }
  L_motor_one.setSpeed(init_motor_speed);   // 設定左側馬達的速度值200
  R_motor_one.setSpeed(init_motor_speed);   // 設定右側馬達的速度值200
  L_motor_two.setSpeed(init_motor_speed);   // 設定左側馬達的速度值200
  R_motor_two.setSpeed(init_motor_speed);   // 設定右側馬達的速度值200       
}
/*void receiveEvent(int nbyte)
{
    buf[nbyte] = 0;
    for (int i = 0; Wire.available(); i++){buf[i] = Wire.read();}
    Serial.println(buf);
}*/
void setup()
{
    Wire.begin(ARDUINO_ADDR);
    //Wire.onReceive(receiveEvent);
    Serial.begin(9600);
    Serial.println("START！");  
    L_motor_one.setSpeed(init_motor_speed);   //設定左側馬達的速度值200
    R_motor_one.setSpeed(init_motor_speed);   //設定右側馬達的速度值200
    L_motor_two.setSpeed(init_motor_speed);   //設定左側馬達的速度值200
    R_motor_two.setSpeed(init_motor_speed);   //設定右側馬達的速度值200
}

void loop()
{    
  print_mainMenu();
  //PWM_adjust_byChannel();  
  while(1){
    if(Serial.available()!= 0){ val = Serial.read(); }
    
    if(Wire.available() != 0){ val = Wire.read(); }

    switch(val){
      case 'I' :
      case 'i' :
        forword();
        count++;
        Serial.print("forword, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;
      case 'L' :
      case 'l' :
        right();
        count++;
        Serial.print("right, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;
      case 'J':
      case 'j':
        left();
        count++;
        Serial.print("left, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;
      case ',':
        backward();
        count++;
        Serial.print("backwork, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;  
      case 'K':    
      case 'k':
        rest();
        count=0;
        Serial.println("Reset all setp！");
        val = 'n';
        break;
      case 'O':    
      case 'o':
        forword_right();
        count++;
        Serial.print("forword_right, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;
      case 'U':    
      case 'u':
        forword_left();
        count++;
        Serial.print("forword_left, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;
      case '.':    
        backward_right();
        count++;
        Serial.print("backward_right, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;        
      case 'M':    
      case 'm':
        backward_left();
        count++;
        Serial.print("backward_left, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;
      case 'A':    
      case 'a':
        trun_left();
        count++;
        Serial.print("trun_left, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;
      case 'D':    
      case 'd':
        trun_right();
        count++;
        Serial.print("trun_right, this is step:");
        Serial.print("\t");
        Serial.print(count);
        Serial.println("");
        val = 'n';
        break;
        
      case 'T':    
      case 't':
        PWM_adjust();
        print_mainMenu();
        val = 'n';
        break;   
            
      default:
        break;
      }
    }   
}


