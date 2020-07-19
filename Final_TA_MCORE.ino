#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h> 
#include <MeColorSensor.h>
#include <MeMCore.h>

MeColorSensor colorsensor_2(2);
MeColorSensor colorsensor_4(4);

uint8_t detectedColor(MeColorSensor colorSensor, uint8_t colorType){
  if(colorType == colorSensor.Returnresult()){
    return 1;
  }
  return 0;
}


MeUltrasonicSensor ultrasonic_3(3);
MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

void move(int direction, int speed)
{
      int leftSpeed = 0;
      int rightSpeed = 0;
      if(direction == 1){
          leftSpeed = speed;
          rightSpeed = speed;
      }else if(direction == 2){
          leftSpeed = -speed;
          rightSpeed = -speed;
      }else if(direction == 3){
          leftSpeed = -speed;
          rightSpeed = speed;
      }else if(direction == 4){
          leftSpeed = speed;
          rightSpeed = -speed;
      }
      motor_9.run((9)==M1?-(leftSpeed):(leftSpeed));
      motor_10.run((10)==M1?-(rightSpeed):(rightSpeed));
}
double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;
double currentTime = 0;
double lastTime = 0;
MeRGBLed rgbled_7(7, 7==7?2:4);
MeLEDMatrix ledMtx_1(1);  
int data;
int activeColor=0; //no color selected


void setup(){
    Serial.begin(115200); 
    ledMtx_1.setColorIndex(1);
    ledMtx_1.setBrightness(6);
    lastTime = millis()/1000.0;
    rgbled_7.setColor(0,0,0,0);
    rgbled_7.show();

    rgbled_7.setColor(0, 51,249,255);
    rgbled_7.show();
    _delay(1);
    rgbled_7.setColor(0, 0, 0, 0);
    rgbled_7.show();
  
    ledMtx_1.setColorIndex(1);
    ledMtx_1.setBrightness(6);
    ledMtx_1.showNum(2048, 3);


    
    colorsensor_2.SensorInit();
  colorsensor_4.SensorInit();
    delay(1000);
 }

 void Data(){
  data= Serial.read();
  if(data== 210 || data== 211 || data== 212 || data== 213){
    
    activeColor = data;
    ledMtx_1.drawStr(0,0+7,"ht");
    motor_9.run((9)==M1?-(0):(0));
    motor_10.run((10)==M1?-(0):(0));
    //delay(5000);
            
    } 
 }
 
void Uturn(){
  
            ledMtx_1.drawStr(0,0+7,"8");
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(500);
            
            motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(-120):(-120));
            delay(600);

                      
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(400);
            
            motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(-120):(-120));
            delay(500);
            
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0)); 
            
             delay(1000);
   
  }

void avoider(){
      ledMtx_1.drawStr(0,0+7,"st");
           
           //kanan
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(500); 
            motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(-120):(-120));
            delay(500);
            
            //lurus
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(500);

            motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(120):(120));
            delay(1000);
          
            //kiri 
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(500);
             motor_9.run((9)==M1?-(-120):(-120));
            motor_10.run((10)==M1?-(120):(120));
            delay(550); 
            
          //lurus
               motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(500);

             motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(120):(120));
            delay(1400);

            //kiri 
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(500);
             motor_9.run((9)==M1?-(-120):(-120));
            motor_10.run((10)==M1?-(120):(120));
            delay(500); 

            //lurus
             motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(800); 
             motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(120):(120));
            delay(1150);


           //kanan
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(500); 
            motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(-120):(-120));
            delay(500);
            
//           stop
            
        
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(1000);
            
  
  }
void checkColor(){
  if(((detectedColor(colorsensor_2, 2))  &&  (detectedColor(colorsensor_4, 2))) && activeColor==210 ){
     
    rgbled_7.setColor(0, 255,0,0);
    rgbled_7.show();
  
    motor_9.run((9)==M1?-(0):(0));
    motor_10.run((10)==M1?-(0):(0));
    activeColor=0;
    delay(5000); 
  
    rgbled_7.setColor(0, 0, 0, 0);
    rgbled_7.show();
  }

  else if(((detectedColor(colorsensor_2, 0))  &&  (detectedColor(colorsensor_4, 0))) && activeColor==211 ){
    rgbled_7.setColor(0, 255,255,0);
    rgbled_7.show();
     
    
    motor_9.run((9)==M1?-(0):(0));
    motor_10.run((10)==M1?-(0):(0));
    activeColor=0;
    delay(5000); 
  
    rgbled_7.setColor(0, 0, 0, 0);
    rgbled_7.show();
  }

  else if(((detectedColor(colorsensor_2, 5))  &&  (detectedColor(colorsensor_4, 5))) && activeColor==212 ){
    rgbled_7.setColor(0, 0,255,0);
    rgbled_7.show();
     
    
    ledMtx_1.drawStr(0,0+7,"HG");
    motor_9.run((9)==M1?-(0):(0));
    motor_10.run((10)==M1?-(0):(0));
    activeColor=0;
    delay(5000); 
    
    rgbled_7.setColor(0, 0, 0, 0);
    rgbled_7.show();
  }

  else if(((detectedColor(colorsensor_2, 7))  &&  (detectedColor(colorsensor_4, 7))) && activeColor==213 ){

   rgbled_7.setColor(0, 0,0,255);
    rgbled_7.show();

    
    ledMtx_1.drawStr(0,0+7,"Hb");
    motor_9.run((9)==M1?-(0):(0));
    motor_10.run((10)==M1?-(0):(0));
    activeColor=0;
    delay(5000); 
    
    rgbled_7.setColor(0, 0, 0, 0);
    rgbled_7.show();
  }
  
}
  
void loop(){        
    Data();
    checkColor();
                      
        if(ultrasonic_3.distanceCm() < 16 && ( data ==1 || data ==2 || data ==5)){
            avoider();
      
        }
        else if(ultrasonic_3.distanceCm() < 16 && ( data !=1 || data !=2 || data !=5)){ 
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0)); 
          }
        
        if(data ==1){ //forward
            ledMtx_1.drawStr(0,0+7,"1");
            motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(120):(120));
            
        }  
        else if(data ==2){//left3
            ledMtx_1.drawStr(0,0+7,"2");
            motor_9.run((9)==M1?-(95):(95));
            motor_10.run((10)==M1?-(120):(120));
            
             }
        else if(data ==3){//left2
            ledMtx_1.drawStr(0,0+7,"3");
            motor_9.run((9)==M1?-(85):(85));
            motor_10.run((10)==M1?-(120):(120));
             }
        else if(data ==4){//left1
            ledMtx_1.drawStr(0,0+7,"4");
            motor_9.run((9)==M1?-(75):(75));
            motor_10.run((10)==M1?-(120):(120));
             }
        else if(data ==41){//left0
            ledMtx_1.drawStr(0,0+7,"4");
            motor_9.run((9)==M1?-(75):(75));
            motor_10.run((10)==M1?-(130):(130));
             }
        else if(data ==5){//right3 
            ledMtx_1.drawStr(0,0+7,"5");
            motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(95):(95)); 
             }
        else if(data ==6){//right2
            ledMtx_1.drawStr(0,0+7,"6");
            motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(85):(85));
             }
        else if(data ==7){//right1
            ledMtx_1.drawStr(0,0+7,"7");
            motor_9.run((9)==M1?-(125):(125));
            motor_10.run((10)==M1?-(75):(75));
          } 
        else if(data ==71){//right0
            ledMtx_1.drawStr(0,0+7,"7");
            motor_9.run((9)==M1?-(140):(140));
            motor_10.run((10)==M1?-(75):(75));
          } 
      
        else if(data ==8){//lane turn
          Uturn();
           }

        else if(data ==9){//stop
            ledMtx_1.drawStr(0,0+7,"9");
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(4000);
            
            ledMtx_1.drawStr(0,0+7,"1");
            motor_9.run((9)==M1?-(120):(120));
            motor_10.run((10)==M1?-(120):(120));
            delay(2000);
        } 
        
        else if(data ==10){//traffic
            ledMtx_1.drawStr(0,0+7,"10");
            motor_9.run((9)==M1?-(0):(0));
            motor_10.run((10)==M1?-(0):(0));
            delay(550); 
        } 
    _loop(); 
}
 
void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime)_loop();
}

void _loop(){
}
