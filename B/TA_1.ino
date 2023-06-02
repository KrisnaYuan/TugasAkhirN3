#include <HardwareSerial.h>
#include <DistanceSensor_A02YYUW.h>
unsigned char data1[4]={};
unsigned char data2[4]={};
float distance1,distance2;
float time_1, time_2, sped,selisih;
HardwareSerial serial1(1);
HardwareSerial serial2(2);


void setup() 
{
  pinMode(19, OUTPUT);
  //Serialx.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, 12, 13);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
}

void loop()
{
    DistanceSensor_A02YYUW distanceSensor(&Serial2);
    DistanceSensor_A02YYUW_MEASSUREMENT_STATUS meassurementStatus;
  do
    {
    meassurementStatus = distanceSensor.meassure();
  
    if (meassurementStatus == DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK) {
      Serial.print("Distance 2: ");
      distance2=distanceSensor.getDistance();
      distance2/=10;
      Serial.println(distance2);
    } else {
      Serial.print("Error:" );
      Serial.println(meassurementStatus);
    }
  } while (meassurementStatus != DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK);
  //Pengukuran
  time_2=millis();
  while(distance2<200)
  {
    DistanceSensor_A02YYUW distanceSensor(&Serial1);
    DistanceSensor_A02YYUW_MEASSUREMENT_STATUS meassurementStatus;
    do
      {
      meassurementStatus = distanceSensor.meassure();
  
      if (meassurementStatus == DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK) {
        Serial.print("Distance 1: ");
        distance1=distanceSensor.getDistance();
        distance1/=10;
        Serial.println(distance1);
      } else {
        Serial.print("Error:" );
        Serial.println(meassurementStatus);
      }
    } while (meassurementStatus != DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK);

    if (distance1<200)
    {
      time_1=millis();
      selisih= (time_1-time_2);
      selisih/=1000;
      sped=7.2/selisih;
      Serial.println(selisih);
      Serial.println(sped);
      Serial.print("Kecepatan adalah +");
      Serial.println(sped);
      if (sped>20)
      {
        digitalWrite(19, HIGH);
        delay(4000);
        digitalWrite(19,LOW);
      }
      else
      {
        delay(10000);
      }
      distance1=distance2=300;
    }
  }
  delay(10);
}
