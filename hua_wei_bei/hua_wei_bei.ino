#include <dht11.h>          //包含dht11.h
//=========================================================
#define _bianhao  "1"
#define _gm       A6
#define _mq135    A3
#define _tr       A2
#define _dth11    A1
//=========================================================
enum
{
  GM = 2, MQ = 4, TR, DTH
} kg;
//=========================================================
struct ML {
  char ml[200] = {'\0'};
  struct ML *p = NULL;
};
struct KG {
  unsigned int gm : 1;
  unsigned int mq : 1;
  unsigned int tr : 1;
  unsigned int dth: 1;
};
struct CGQ {
  KG    kg      = {1, 1, 1, 1};
  float wsd[2]  = {0};
  int   gm      = 0;
  int   mq135   = 0;
  int   tr      = 0;
};
struct SJ {
  int shi;
  int fen;
  int miao;
};
struct FSJG { //时  分  秒
  SJ gm[2]  = {0 , 0 , 1};    //光敏传感器
  SJ mq[2]  = {0 , 0 , 20};   //MQ135
  SJ tr[2]  = {0 , 0 , 10};   //土壤湿度
  SJ dth[2] = {0 , 0 , 5};    //温湿度
};
//=========================================================
CGQ   cgq;
FSJG  jg;
SJ    sj = {0, 0, 0};
ML *p1 = new struct ML, *p2 = p1;
const char fg = ',';                     //分隔符
char hc[200] = {0};
int  hc_i    = 0;
unsigned int t = 0;
//=========================================================
dht11 DHT11;                //初始化dht11类
//=========================================================
void dth();      //dth11
void kaiguan();  //开关
int sjpd(FSJG *p);
int mlcl(char* s, const char* ml);
int cin(char* p, int x);
//=========================================================
void setup() {
  Serial.begin(9600);
  pinMode(GM, OUTPUT);
  digitalWrite(GM, HIGH);
  pinMode(MQ, OUTPUT);
  digitalWrite(MQ, HIGH);
  pinMode(TR, OUTPUT);
  digitalWrite(TR, HIGH);
  pinMode(DTH, OUTPUT);
  digitalWrite(DTH, HIGH);
  pinMode(13, HIGH);
  pinMode(_gm, INPUT);
  pinMode(_mq135, INPUT);
  pinMode(_tr, INPUT);
  pinMode(_dth11, INPUT);

}
//=========================================================
void loop() {
  unsigned int t1 = millis();
  ML *p = NULL;
  while (t1 - t >= 1000) {
    t += 1000;
    sj.miao++;
    if (sj.miao == 60) {
      sj.miao = 0;
      sj.fen++;
    }
    if (sj.fen == 60) {
      sj.fen = 0;
      sj.shi++;
    }
  }
  if (t > t1) t = 0;
  if (sj.shi < 0) sj.shi = 0;
  digitalWrite(13, !(sj.miao % 10));
  if (sjpd(jg.gm) && cgq.kg.gm) {
    cgq.gm = analogRead(_gm);
    Serial.print("gm:");
    Serial.print(_bianhao);
    Serial.print(",");
    Serial.print(cgq.gm);
    Serial.print("\r\n");
  }
  if (sjpd(jg.mq) && cgq.kg.mq) {
    cgq.mq135 = analogRead(_mq135);
    Serial.print("mq:");
    Serial.print(_bianhao);
    Serial.print(",");
    Serial.print(cgq.mq135);
    Serial.print("\r\n");
  }
  if (sjpd(jg.tr) && cgq.kg.tr) {
    cgq.tr = analogRead(_tr);
    Serial.print("tr:");
    Serial.print(_bianhao);
    Serial.print(",");
    Serial.print(cgq.tr);
    Serial.print("\r\n");
  }
  if (sjpd(jg.dth) && cgq.kg.dth) {
    dth();
    Serial.print("dth:");
    Serial.print(_bianhao);
    Serial.print(",");
    Serial.print(cgq.wsd[0]);
    Serial.print(",");
    Serial.print(cgq.wsd[1]);
    Serial.print("\r\n");
  }
  while (Serial.available()) {
    hc[hc_i] = Serial.read();
    if (hc[hc_i] == '\n' && hc_i != 0) {
      p2->p = new ML;
      for (int i = 0; i < hc_i; i++) p2->ml[i] = hc[i];
      p2 = p2->p;
      hc_i = -1;
    }
    hc_i++;
  }
  while (p1->p) {
    if (mlcl(p1->ml, "kg:")) {
      switch (cin(p1->ml, 0)) {
        case 0:
          cgq.kg.gm = cin(p1->ml, 1);
          break;
        case 1:
          cgq.kg.mq = cin(p1->ml, 1);
          break;
        case 2:
          cgq.kg.tr = cin(p1->ml, 1);
          break;
        case 3:
          cgq.kg.dth = cin(p1->ml, 1);
          break;
      }
    } else if (mlcl(p1->ml, "ds:")) {
      switch (cin(p1->ml, 0)) {
        case 0:
          jg.gm[0].shi  = cin(p1->ml, 1);
          jg.gm[0].fen  = cin(p1->ml, 2);
          jg.gm[0].miao = cin(p1->ml, 3);
          break;
        case 1:
          jg.mq[0].shi  = cin(p1->ml, 1);
          jg.mq[0].fen  = cin(p1->ml, 2);
          jg.mq[0].miao = cin(p1->ml, 3);
          break;
        case 2:
          jg.tr[0].shi  = cin(p1->ml, 1);
          jg.tr[0].fen  = cin(p1->ml, 2);
          jg.tr[0].miao = cin(p1->ml, 3);
          break;
        case 3:
          jg.dth[0].shi = cin(p1->ml, 1);
          jg.dth[0].fen = cin(p1->ml, 2);
          jg.dth[0].miao = cin(p1->ml, 3);
          break;
      }
    }

    p = p1;
    p1 = p1->p;
    delete p;
  }

  kaiguan();
}
//=========================================================
void dth() {      //dth11
  DHT11.read(_dth11); //初始化引脚并进行一次通信且返回通信结果
  cgq.wsd[0] = (float)DHT11.temperature;
  cgq.wsd[1] = (float)DHT11.humidity;
}
//=========================================================
void kaiguan() {
  digitalWrite(GM , cgq.kg.gm );
  digitalWrite(MQ , cgq.kg.mq );
  digitalWrite(TR , cgq.kg.tr );
  digitalWrite(DTH, cgq.kg.dth);
}
//=========================================================
int sjpd(SJ *p) {
  if (
    p[0].miao <= sj.miao - p[1].miao &&
    p[0].fen  <= sj.fen  - p[1].fen  &&
    p[0].shi  <= sj.shi  - p[1].shi
  ) {
    p[1].miao += p[0].miao;
    p[1].fen  += p[0].fen;
    p[1].shi  += p[0].shi;
    return 1;
  }
  if (sj.miao < p[1].miao) p[1].miao -= 60;
  if (sj.fen < p[1].fen)   p[1].fen  -= 60;
  if (sj.shi < p[1].shi)   p[1].shi   = 0;
  return 0;
}
//=========================================================
int cin(char* p, int x) //字符串转整数-p中第x个数字,数字间用","(分隔符)隔开
{
  int s = 0, fh = 1, i;
  for (i = 0; i < 200; i++) {
    if (!x) break;
    if (p[i] == fg) x--;
  }
  if (i == 199) return -1;
  p += i;
  if (*p == '-') {
    fh = -1;
    p++;
  }
  while (*p >= '0' && *p <= '9') {
    s *= 10;
    s += *p - '0';
    p++;
  }
  return s * fh;
}
//========================================================================
int mlcl(char* s, const char* ml)           //命令处理-判断s的开头是否为ml 是-将s中ml删掉
{
  const char* p1 = s, * p2 = ml;
  while (*p1 == *p2) {
    p1++, p2++;
    if (*p1 == '\0') break;
  }
  if (*p2 != '\0') return 0;
  else if (*p1 == '\0') return 1;
  else for (int i = 0; p1[i - 1] != '\0'; i++) s[i] = p1[i];
  return 2;
}
