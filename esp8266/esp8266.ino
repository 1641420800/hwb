#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "aliyun_mqtt.h"
//=======================================================================================================
#define SENSOR_PIN 13
//=======================================================================================================
#define WIFI_SSID        "1641420800"         //替换自己的WIFI
#define WIFI_PASSWD      "15612774227"        //替换自己的WIFI
//=======================================================================================================
#define PRODUCT_KEY      "a15FrrRNdXl"        //替换自己的PRODUCT_KEY
#define DEVICE_NAME      "ceshi"              //替换自己的DEVICE_NAME
#define DEVICE_SECRET    "50f4c59aa6025119f20031e989dfbb93"       //替换自己的DEVICE_SECRET
//=======================================================================================================
#define DEV_VERSION       "S-TH-WIFI-v1.0-20200710"               //固件版本信息
//=======================================================================================================
#define ALINK_BODY_FORMAT         "{\"id\":\"111\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"
#define ALINK_TOPIC_PROP_POST     "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
#define ALINK_TOPIC_SJBH_POST     "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/shujushangbao/post"
#define ALINK_TOPIC_PROP_POSTRSP  "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post_reply"
#define ALINK_TOPIC_PROP_SET      "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
#define ALINK_METHOD_PROP_POST    "thing.event.property.post"
#define ALINK_TOPIC_DEV_INFO      "/ota/device/inform/" PRODUCT_KEY "/" DEVICE_NAME ""
#define ALINK_VERSION_FROMA       "{\"id\": 111,\"params\": {\"version\": \"%s\"}}"
//=======================================================================================================
struct ML {
  char ml[200] = {'\0'};
  struct ML *p = NULL;
};
struct SHUJU {
  int   gz      = 0;    //光照强度
  int   hy      = 0;    //火焰
  int   mq      = 0;    //空气质量
  float dth[2]  = {0};  //温湿度
  int   yd      = 0;    //雨滴
  int   tr      = 0;    //土壤
};
struct BIAO {
  int bh = 0;
  struct SHUJU *p = NULL;
};
//=======================================================================================================
const int LED_PIN = 2;
int LED = 0;
const char fg = ',';                     //分隔符
unsigned long lastMs = 0;
unsigned int WAIT_MS = 2000;

struct ML *p1 = new struct ML, *p2 = p1;    //队列出入口
struct BIAO *biao = new struct BIAO[50];    //表
int yicun = 0;                              //已存
struct SHUJU pjsj;                          //平均数据
char ml[200] = {'\0'};                      //通信数据缓存数组
int ml_i = 0;                               //缓存标识
WiFiClient   espClient;
PubSubClient mqttClient(espClient);
//=======================================================================================================
int cch(char* s, int j, char* p2, int n, char a = ',');
int cin(char* p, int x);
int mlcl(char* s, char* ml);
void dch(double d, int bl, char *p);
void init_wifi(const char *ssid, const char *password);
void mqtt_callback(char *topic, byte *payload, unsigned int length);
void mqtt_version_post();
void mqtt_sjbh_post(int wz);
void mqtt_check_connect();
void mqtt_interval_post();
struct SHUJU* bcdz(int bh);
void pingjun();

//=======================================================================================================
void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  init_wifi(WIFI_SSID, WIFI_PASSWD);
  mqttClient.setCallback(mqtt_callback);
}
void loop() {
  int i;
  struct ML *p = NULL;
  struct SHUJU * sj;
  char lss[200] = { 0 };
  while (Serial.available()) {                          //处理串口
    ml[ml_i] = char(Serial.read());
    if (ml[ml_i] == '\n' && ml_i != 0) {
      p2->p = new ML;
      for (i = 0; i < ml_i; i++) p2->ml[i] = ml[i];
      p2 = p2->p;
      ml_i = -1;
    }
    ml_i++;
  }
  while (p1->p) {
    if (mlcl(p1->ml, "gm:")) {         //光照
      sj = bcdz( cin(p1->ml, 0));
      sj -> gz = cin(p1->ml, 1);
      Serial.print("OK\r\n");
    }
    if (mlcl(p1->ml, "hy:")) {         //火焰
      sj = bcdz( cin(p1->ml, 0));
      sj -> hy = cin(p1->ml, 1);
      Serial.print("OK\r\n");
    }
    if (mlcl(p1->ml, "mq:")) {         //空气质量
      sj = bcdz( cin(p1->ml, 0));
      sj -> mq = cin(p1->ml, 1);
      Serial.print("OK\r\n");
    }
    if (mlcl(p1->ml, "dth:")) {        //温湿度
      sj = bcdz( cin(p1->ml, 0));
      sj -> dth[0] = cdo(p1->ml, 1);
      sj -> dth[1] = cdo(p1->ml, 2);
      Serial.print("OK\r\n");
    }
    if (mlcl(p1->ml, "yd:")) {         //雨滴
      sj = bcdz( cin(p1->ml, 0));
      sj -> yd = cin(p1->ml, 1);
      Serial.print("OK\r\n");
    }
    if (mlcl(p1->ml, "tr:")) {         //土壤
      sj = bcdz( cin(p1->ml, 0));
      sj -> tr = cin(p1->ml, 1);
      Serial.print("OK\r\n");
    }


    p = p1;
    p1 = p1->p;
    delete p;
  }
  if (millis() - lastMs >= 20000)  {
    lastMs = millis();
    pingjun();
    mqtt_check_connect();
    mqtt_interval_post();     //上交数据
    for (int i = 0; i < yicun; i++)mqtt_sjbh_post(i);
    digitalWrite(LED_PIN, LED = !LED);
  }
  mqttClient.loop();
  delay(WAIT_MS); // ms
  Serial.println(ESP.getFreeHeap());
}
//======================================================================================================= WIFI连接
void init_wifi(const char *ssid, const char *password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("Wifi OK\r\n");
}
//======================================================================================================= 阿里云物联网支持部分
void mqtt_callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  payload[length] = '\0';
  Serial.println((char *)payload);
  if (strstr(topic, ALINK_TOPIC_PROP_SET))  {
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(payload);
    if (!root.success())    {
      Serial.println("parseObject() failed");
      return;
    }
  }
}
void mqtt_version_post() {
  char param[512];
  char jsonBuf[1024];
  //sprintf(param, "{\"MotionAlarmState\":%d}", digitalRead(13));
  sprintf(param, "{\"id\": 111,\"params\": {\"version\": \"%s\"}}", DEV_VERSION);
  // sprintf(jsonBuf, ALINK_BODY_FORMAT, ALINK_METHOD_PROP_POST, param);
  Serial.println(param);
  mqttClient.publish(ALINK_TOPIC_DEV_INFO, param);
}
void mqtt_check_connect() {
  while (!mqttClient.connected()) {
    while (connect_aliyun_mqtt(mqttClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET))    {
      Serial.println("MQTT connect succeed!");
      //client.subscribe(ALINK_TOPIC_PROP_POSTRSP);
      mqttClient.subscribe(ALINK_TOPIC_PROP_SET);
      Serial.println("subscribe done");
      mqtt_version_post();
    }
  }
}
void mqtt_interval_post() {
  char param[512];
  char jsonBuf[1024];
  char s1[20], s2[20];
  dch((double)pjsj.dth[0], 2, s1);
  dch((double)pjsj.dth[1], 2, s2);
  sprintf(param, "{\"guangqiang\":%d,\"huoyan\":%d,\"kongqishidu\":%s,\"kongqizhiliang\":%d,\"turangshidu\":%d,\"wendu\":%s,\"yudi\":%d,\"shuliang\":%d}"
          , pjsj.gz, pjsj.hy, s1 , pjsj.mq, pjsj.tr, s2, pjsj.yd,yicun);
  sprintf(jsonBuf, ALINK_BODY_FORMAT, ALINK_METHOD_PROP_POST, param);
  Serial.println(jsonBuf);
  mqttClient.publish(ALINK_TOPIC_PROP_POST, jsonBuf);
}
void mqtt_sjbh_post(int wz) {
  char param[512];
  char jsonBuf[1024];
  char s1[20], s2[20];
  struct SHUJU *pp = biao[wz].p;
  dch((double)pp->dth[0], 2, s1);
  dch((double)pp->dth[1], 2, s2);
  sprintf(param, "{\"bianhao\":%d,\"guangqiang\":%d,\"huoyan\":%d,\"kongqishidu\":%s,\"kongqizhiliang\":%d,\"turangshidu\":%d,\"wendu\":%s,\"yudi\":%d}"
          , biao[wz].bh, biao[wz].p->gz, biao[wz].p->hy, s1 , biao[wz].p->mq, biao[wz].p->tr, s2, biao[wz].p->yd);
  sprintf(jsonBuf, ALINK_BODY_FORMAT, ALINK_METHOD_PROP_POST, param);
  Serial.println(jsonBuf);
  mqttClient.publish(ALINK_TOPIC_SJBH_POST, jsonBuf);
}
//======================================================================================================= 串口通信支持部分
int mlcl(char* s, char* ml)           //命令处理-判断s的开头是否为ml 是-将s中ml删掉
{
  char* p1 = s, * p2 = ml;
  while (*p1 == *p2) {
    p1++, p2++;
    if (*p1 == '\0') break;
  }
  if (*p2 != '\0') return 0;
  else if (*p1 == '\0') return 1;
  else for (int i = 0; p1[i - 1] != '\0'; i++) s[i] = p1[i];
  return 2;
}
int cin(char* p, int x)        //字符串转整数-p中第x个数字,数字间用","(分隔符)隔开
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
int cch(char* s, int j, char* p2, int n, char a) {
  char* p1 = s;
  int bz = 0, i, k = 0;
  while (n && j) {
    if (*p1 == a) n--;
    p1++;
    j--;
  }
  for (i = 0; i < j; i++) {
    if (p1[i] == '\"') bz++;
    else {
      p2[k] = p1[i];
      k++;
    }
    if (bz == 2) {
      p2[k] = '\0';
      return i - 1;
    }
  }
  return -1;
}
double cdo(char* p, int n) {
  int a = cin(p, n), i;
  for (i = 0; i < 200; i++) {
    if (!n) break;
    if (p[i] == fg) n--;
  }
  if (i == 199) return -1;
  p += i;
  while (*p != '.') p++;
  p++;
  double b = (double)cin(p, n);
  while (b > 1) b /= 10;
  b = a + b;
  return b;
}
void dch(double d, int bl, char *p) {
  int ws = 0, ls = 0;
  if (d < 1 && d > -1) ls = 1;
  while (d > 1 || d < -1) {
    d /= 10;
    ws++;
  }
  if (d < 0) {
    *p = '-';
    p++;
    d *= -1;
  }
  if (ls) {
    *p = '0';
    p++;
  }
  while (d != 0) {
    if (!ws) {
      *p = '.';
      p++;
    }
    if (ws == -1 * bl) break;
    ls = (int)(d * 10);
    d = d * 10 - ls;
    *p = '0' + ls % 10;
    p++;
    ws--;
  }
  *p = '\0';
}
//======================================================================================================= 数据储存支持部分
struct SHUJU* bcdz(int bh) {
  int i;
  struct SHUJU* p = NULL;
  for (i = 0; i < yicun; i++) {
    if (biao[i].bh == bh) return biao[i].p;
  }
  if (yicun < 50) {
    biao[yicun].bh = bh;
    p = new struct SHUJU;
    biao[yicun].p = p;
    yicun++;
    return p;
  }
  return NULL;
}
void pingjun() {
  int i;
  if(!yicun) return;
  pjsj.hy = 0;
  pjsj.gz = 0;
  pjsj.mq = 0;
  pjsj.yd = 0;
  pjsj.tr = 0;
  pjsj.dth[0] = 0;
  pjsj.dth[1] = 0;
  for (i = 0; i < yicun; i++) {
    if (biao[i].p == NULL) continue;
    pjsj.hy += biao[i].p->hy;
    pjsj.gz += biao[i].p->gz;
    pjsj.mq += biao[i].p->mq;
    pjsj.tr += biao[i].p->tr;
    pjsj.dth[0] += biao[i].p->dth[0];
    pjsj.dth[1] += biao[i].p->dth[1];
  }
  if (pjsj.hy) pjsj.hy = 1;
  pjsj.gz = pjsj.gz / yicun;
  pjsj.mq = pjsj.mq / yicun;
  pjsj.tr = pjsj.tr / yicun;
  pjsj.dth[0] = pjsj.dth[0] / yicun;
  pjsj.dth[1] = pjsj.dth[1] / yicun;
}
