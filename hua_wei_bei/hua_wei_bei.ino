#include <dht11.h>          //包含dht11.h
#include "Adafruit_SGP30.h"

#define bianhao  "1"
#define gm       A6
#define co2_scl  A5
#define co2_sda  A4
#define mq135    A3
#define tr       A2
#define dth11    A1
enum
{
  GM = 2, CO, MQ, TR, DTH
} kg;
struct KG {
  unsigned int gm : 1;
  unsigned int co : 1;
  unsigned int mq : 1;
  unsigned int tr : 1;
  unsigned int dth: 1;
};
struct CGQ {
  KG    kg      = {1, 1, 1, 1, 1};
  float wsd[2]  = {0};
  int   sgp[2]  = {0};
  int   gm      = 0;
  int   mq135   = 0;
  int   tr      = 0;
} cgq;

dht11 DHT11;                //初始化dht11类
Adafruit_SGP30 SGP;

void dth();      //dth11
void sgp();      //sgp30
void gkt();
void kaiguan();  //开关

void setup() {
  Serial.begin(9600);
  pinMode(GM, OUTPUT);
  digitalWrite(GM, HIGH);
  pinMode(CO, OUTPUT);
  digitalWrite(CO, HIGH);
  pinMode(MQ, OUTPUT);
  digitalWrite(MQ, HIGH);
  pinMode(TR, OUTPUT);
  digitalWrite(TR, HIGH);
  pinMode(DTH, OUTPUT);
  digitalWrite(DTH, HIGH);
  pinMode(gm, INPUT);
  pinMode(mq135, INPUT);
  pinMode(tr, INPUT);
  pinMode(dth11, INPUT);

  if (!SGP.begin()) {
    //while (1);
  }
  SGP.setIAQBaseline(0x8501, 0x8476);
}

void loop() {
  dth();
  sgp();
  gkt();
  Serial.print(bianhao);
  Serial.print(",");
  Serial.print(cgq.wsd[0]);
  Serial.print(",");
  Serial.print(cgq.wsd[1]);
  Serial.print(",");
  Serial.print(cgq.sgp[0]);
  Serial.print(",");
  Serial.print(cgq.gm);
  Serial.print(",");
  Serial.print(cgq.mq135);
  Serial.print(",");
  Serial.print(cgq.tr);
  Serial.print("\r\n");


  kaiguan();
}
void dth() {      //dth11
  DHT11.read(dth11); //初始化引脚并进行一次通信且返回通信结果
  cgq.wsd[0] = (float)DHT11.temperature;
  cgq.wsd[1] = (float)DHT11.humidity;
}
void sgp() {      //sgp30
  const float ab = 216.7f * ((cgq.wsd[1] / 100.0f)
                             * 6.112f * exp((17.62f * cgq.wsd[0])
                                 / (243.12f + cgq.wsd[0]))
                             / (273.15f + cgq.wsd[0]));
  const uint32_t absol = static_cast<uint32_t>(1000.0f * ab);
  SGP.setHumidity(absol);
  if (!SGP.IAQmeasure()) {
    return;
  }
  cgq.sgp[0] = SGP.eCO2;
  cgq.sgp[1] = SGP.TVOC;
}
void gkt() {
  cgq.gm = analogRead(gm);
  cgq.mq135 = analogRead(mq135);
  cgq.tr = analogRead(tr);
}
void kaiguan() {
  digitalWrite(GM , cgq.kg.gm );
  digitalWrite(CO , cgq.kg.co );
  digitalWrite(MQ , cgq.kg.mq );
  digitalWrite(TR , cgq.kg.tr );
  digitalWrite(DTH, cgq.kg.dth);
  if (!cgq.kg.dth) {
    pinMode(dth11, OUTPUT);
    digitalWrite(dth11, !cgq.kg.dth);
  }

}
