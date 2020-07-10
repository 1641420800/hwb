#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "aliyun_mqtt.h"

#define SENSOR_PIN 13

#define WIFI_SSID        "1641420800"         //替换自己的WIFI
#define WIFI_PASSWD      "15612774227"        //替换自己的WIFI

#define PRODUCT_KEY      "a15FrrRNdXl"        //替换自己的PRODUCT_KEY
#define DEVICE_NAME      "ceshi"              //替换自己的DEVICE_NAME
#define DEVICE_SECRET    "50f4c59aa6025119f20031e989dfbb93"       //替换自己的DEVICE_SECRET

#define DEV_VERSION       "S-TH-WIFI-v1.0-20200710"               //固件版本信息

#define ALINK_BODY_FORMAT         "{\"id\":\"111\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"
#define ALINK_TOPIC_PROP_POST     "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
#define ALINK_TOPIC_PROP_POSTRSP  "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post_reply"
#define ALINK_TOPIC_PROP_SET      "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
#define ALINK_METHOD_PROP_POST    "thing.event.property.post"
#define ALINK_TOPIC_DEV_INFO      "/ota/device/inform/" PRODUCT_KEY "/" DEVICE_NAME ""
#define ALINK_VERSION_FROMA       "{\"id\": 111,\"params\": {\"version\": \"%s\"}}"

struct ML {
  char ml[200] = {'\0'};
  struct ML *p = NULL;
};
struct SHUJU{
  
};

int LED_PIN = 2;
int LED = 0;
const char fg = ',';                     //分隔符
unsigned long lastMs = 0;
unsigned int WAIT_MS = 2000;

struct ML *p1 = new struct ML, *p2 = p1;    //队列出入口
char ml[200] = {'\0'};                      //通信数据缓存数组
int ml_i = 0;                               //缓存标识

WiFiClient   espClient;
PubSubClient mqttClient(espClient);

void init_wifi(const char *ssid, const char *password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("Wifi OK\r\n");
}
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
  sprintf(param, "{\"guangqiang\":%d,\"huoyan\":%d,\"kongqishidu\":%d,\"kongqizhiliang\":%d,\"turangshidu\":%d,\"wendu\":%d,\"yudi\":%d}", 77, 1, 33, 33, 22, 11, 11);
  sprintf(jsonBuf, ALINK_BODY_FORMAT, ALINK_METHOD_PROP_POST, param);
  mqttClient.publish(ALINK_TOPIC_PROP_POST, jsonBuf);
}
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
void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  init_wifi(WIFI_SSID, WIFI_PASSWD);
  mqttClient.setCallback(mqtt_callback);
}
void loop() {
  int i;
  struct ML *p = NULL;
  while (Serial.available()) {                          //处理串口
    ml[ml_i] = char(Serial.read());
    if (ml[ml_i] == '$' && ml_i != 0) {
      p2->p = new ML;
      for (i = 0; i < ml_i; i++) p2->ml[i] = ml[i];
      p2 = p2->p;
      ml_i = 0;
      ml[ml_i] = '$';
    }
    ml_i++;
  }
  while (p1->p) {
    if (mlcl(p1->ml, "gz")) {

    }

    p = p1;
    p1 = p1->p;
    delete p;
  }
  if (millis() - lastMs >= 20000)  {
    lastMs = millis();
    mqtt_check_connect();
    mqtt_interval_post();     //上交数据
    digitalWrite(LED_PIN, LED = !LED);
  }
  mqttClient.loop();
  delay(WAIT_MS); // ms
}
