#include <Servo.h>

Servo curservo;
Servo pushservo;
int SensorPin = A0;
int mfsr_r18 = 0;

int cnt=0;
int delay_time;
float C_amount, M_amount, Y_amount, K_amount, W_amount= 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  curservo.attach(9); // attaches the servo on pin 10 to the servo object
  pushservo.attach(10); // attaches the servo on pin 11 to the servo object

 curservo.write(88);
 pushservo.write(88);
}

int  input=0;
char*  buf;
int  buf_cnt;
float vC, vM, vY, vK;
int weight;

void loop() {
//  if(cnt == 0) {
//    
//    Serial.print("0");
// if(Serial.available()>0){
//    while((input = Serial.read())!='&')
//      buf[buf_cnt++]=input;   // 0.0000 0.0000 0.0000 0.0000 35
//    vC = atof(strtok(buf, " "));
//    vM = atof(strtok(NULL, " "));
//    vY = atof(strtok(NULL, " "));
//    vK = atof(strtok(NULL, " "));
//    weight = atoi(strtok(NULL, "\0"));
//  }
//  }
//    delay(4000);
//  Serial.print(String(cnt+1));
//  if(Serial.available()>0 && cnt+1 < 6)
//    while((input = Serial.read()) != '&');
  // put your main code here, to run repeatedly:
  cmyk(1, 0, 1, 0, 5);

  int SensorReading;
  

  
  switch(cnt) {
    case 0 : delay_time = C_amount; break;
    case 1 : delay_time = M_amount; break;
    case 2 : delay_time = Y_amount; break;
    case 3 : delay_time = K_amount; break;
    case 4 : delay_time = W_amount; break;
  }

if(delay_time==0)
pushservo.write(88);

  if(cnt==5)
  {
    curservo.write(88);
    pushservo.write(88);
  while(1);
  }
for(int i =0; i<89; i++)
{
  curservo.write(i);
  delay(3);
}
delay(1000);
int Sensor_cnt=0;
while(mfsr_r18 < 30) {
    SensorReading = analogRead(SensorPin);
    mfsr_r18 = map(SensorReading, 0, 1024, 0 ,255);

  Sensor_cnt++;
    pushservo.write(94);
    Serial.println(mfsr_r18);
  }
  mfsr_r18=0;
for(int i=149; i>89; i--)
{
  pushservo.write(i);
  delay(delay_time/90);
}
for(int i=28; i<88; i++)
{
  pushservo.write(i);
  delay(delay_time/90);
}
pushservo.write(82);
delay(Sensor_cnt);
  
  pushservo.write(88);
 delay(1000);

  cnt++;
 

}
void cmyk(float C_ratio, float M_ratio, float Y_ratio, float K_ratio, float amount)
{

  int param=600;
  int count=0;
if(C_ratio==0) C_amount = 0;
else count++;
if(M_ratio==0) M_amount = 0;
else count++;
if(Y_ratio==0) Y_amount = 0;
else count++;
if(K_ratio==0) K_amount = 0;
else count++;

C_amount=(amount/count)*C_ratio*param;
M_amount=(amount/count)*M_ratio*param;
Y_amount=(amount/count)*Y_ratio*param;
K_amount=(amount/count)*K_ratio*param;
W_amount=(amount/count)*(count-C_ratio-M_ratio-Y_ratio-K_ratio)*param;
return;
}

