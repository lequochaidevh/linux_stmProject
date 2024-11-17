#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "OPPO A3s";
const char* password = "123321123";
const char* mqtt_server = "broker.emqx.io";
WiFiClient espClient;
PubSubClient client(espClient);

static char temperatureTemp[7] = "-1";
static char humidityTemp[7] = "-1";
static char pinTemp[7] = "-1";
static char sharpTemp[7] = "-1";
static char room_now_Temp[7] = "-1";
static char room_set_Temp[7] = "-1";
static char data_fb_Temp[7] = "-1";
String messageTemp_last = "";
long now = millis();
long lastMeasure = 0;

char c;
char char_array[50];
int i = 0;
int key = 0;
bool checkconnected = 0, check_sub = 0;
int flag = 0, flag_pub = 0, flag_loop = 0;
unsigned int  humi = 0, temp = 0, pin = 0, room_set = 0, room_now = 0;
float sharp = 0;
unsigned int data_feedback = 0;
void setup_wifi() {
  delay(10);
  //We start by connecting to a WiFi network
  /*Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);*/
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  /*Serial.println("");
    Serial.print("WiFi connected - ESP IP address: ");
    Serial.println(WiFi.localIP());*/
  digitalWrite(D4, HIGH);
  delay(1000);
  digitalWrite(D4, LOW);
}

void callback(String topic, byte* message, unsigned int length) {
  //  Serial.print("Message arrived on topic: ");
  //  Serial.print(topic);
  //  Serial.print(". Message: ");
  String messageTemp;
  //Serial.println();
  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  //  Serial.println();
  if (topic == "roomsetdevhh") {
    //    Serial.print("Changing Room lamp to ");
    if (messageTemp_last == messageTemp)
    {
      flag_loop ++;
    }
    else
    {
      flag_loop = 0;
    }
    if (flag_loop < 3)
    {
      messageTemp_last = messageTemp;
      if (messageTemp == "c1") {
      Serial.print("c1\n");
    }
    else if (messageTemp == "c2") {
      Serial.print("c2\n");
    }
    else if (messageTemp == "c3") {
      Serial.print("c3\n");
    }
    else if (messageTemp == "f1") {
      Serial.print("f1\n");
    }
    else if (messageTemp == "f2") {
      Serial.print("f2\n");
    }
    else if (messageTemp == "f3") {
      Serial.print("f3\n");
    }
    else {}
    }
  }

  else {}
}

void reconnect() {
  // Loop until we're reconnected
  checkconnected = client.connected();
  while (!checkconnected) {
    //Serial.println("Attempting MQTT connection...");
    checkconnected = client.connect("ESP8266Client");
    if (checkconnected) {
      //Serial.println("connected");
      //Subscribe or resubscribe to a topic
      //You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("roomsetdevhh");
      delay(80);
      break;
    }
    else {
      digitalWrite(D4, HIGH);
      delay(1000);
      digitalWrite(D4, LOW);
      delay(1000);
      digitalWrite(D4, HIGH);
      delay(1000);
      digitalWrite(D4, LOW);
      delay(1000);
      digitalWrite(D4, HIGH);
      delay(1000);
      digitalWrite(D4, LOW);
      //Wait 5 seconds before retrying

    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(D4, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP8266Client");

  now = millis();
  if (flag_pub == 1)
  {
    if (now - lastMeasure > 200) {
    lastMeasure = now;
    key ++;
    if ( key == 1)      client.publish("sharpdevh", sharpTemp);
    else if (key == 2)  client.publish("humiditydevh", humidityTemp);
    else if (key == 3)  client.publish("temperaturedevh", temperatureTemp);
    else if (key == 4)  client.publish("pindevh", pinTemp);
    else if (key == 5)  client.publish("roomnowdevh", room_now_Temp);
    else if (key == 6)  client.publish("datafeedbackdevh", data_fb_Temp);
    //else if (key == 7)
    //else if (key == 7)  client.subscribe("roomsetdevhh");
    else  key = 0;
    //    Serial.print("sharpTemp: ");  Serial.println(sharpTemp);
    //    Serial.print("humidityTemp: ");  Serial.println(temperatureTemp);
    //    Serial.print("temperatureTemp: ");  Serial.println(temperatureTemp);
    //    Serial.print("pinTemp: ");  Serial.println(pinTemp);
    //    Serial.print("room_now_Temp: ");  Serial.println(room_now_Temp);
  }
  }
  

  if ( now > (5000 * 60) ) ESP.reset();

  if (Serial.available() > 0)
  {
    char_array[i] = Serial.read();
    //Serial.write(char_array[i]);

    if (char_array[i] == '\0')
    {
      if (flag == 0)
      {
        if (char_array[0]  == 's')
        {
          flag = 1;
          char_array[50] = NULL;
          i = 0;
        }
        else if (char_array[0]  == 'h')
        {
          flag = 2;
          char_array[50] = NULL;
          i = 0;
        }
        else if (char_array[0]  == 't')
        {
          flag = 3;
          char_array[50] = NULL;
          i = 0;
        }
        else if (char_array[0]  == 'p')
        {
          flag = 4;
          char_array[50] = NULL;
          i = 0;
        }
        else if (char_array[0]  == 'r')
        {
          flag = 5;
          char_array[50] = NULL;
          i = 0;
        }
        else if (char_array[0]  == 'b')
        {
          flag = 6;
          char_array[50] = NULL;
          i = 0;
        }
        else {}
      }

      else if ( flag == 1)
      {

        sharp = float((char_array[i - 3] - 48) * 10 + (char_array[i - 2] - 48)) / 100;
        if ( sharp >= 0 && sharp <= 100)
        {
          /*sharpTemp[0] = (char)(sharp / 10 + 48);
            sharpTemp[1] = (char)(sharp % 10 + 48);
            sharpTemp[2] = '\0';*/
          dtostrf(sharp, 4, 2, sharpTemp);
        }
        else {}
        //Serial.println(sharpTemp);
        digitalWrite(D4, HIGH);
        //        delay(300);
        //        digitalWrite(D4, LOW);
        flag = 0;
        char_array[50] = NULL;
        i = 0;
      }
      else if ( flag == 2)
      {

        humi = int((char_array[i - 3] - 48) * 10 + (char_array[i - 2] - 48));
        if ( humi >= 0 && humi <= 100)
        {
          /*humidityTemp[0] = (char)(humi / 10 + 48);
            humidityTemp[1] = (char)(humi % 10 + 48);
            humidityTemp[2] = '\0';*/
          dtostrf(humi, 4, 0, humidityTemp);
        }
        else {}
        //        digitalWrite(D4, HIGH);
        //        delay(300);
        digitalWrite(D4, LOW);
        //Serial.println(humidityTemp);
        //client.publish("room/humidity", humidityTemp);

        flag = 0;
        char_array[50] = NULL;
        i = 0;
      }
      else if ( flag == 3)
      {

        temp = int((char_array[i - 3] - 48) * 10 + (char_array[i - 2] - 48));
        if ( temp >= 0 && temp <= 100)
        {
          /*temperatureTemp[0] = (char)(temp / 10 + 48);
            temperatureTemp[1] = (char)(temp % 10 + 48);
            temperatureTemp[2] = '\0';*/
          dtostrf(temp, 4, 0, temperatureTemp);
        }
        else {}
        //Serial.println(temperatureTemp);
        //client.publish("room/temperature", temperatureTemp);
        digitalWrite(D4, HIGH);
        //        delay(300);
        //        digitalWrite(D4, LOW);
        flag = 0;
        char_array[50] = NULL;
        i = 0;
      }
      else if ( flag == 4)
      {

        pin = int((char_array[i - 3] - 48) * 10 + (char_array[i - 2] - 48)) + 2;
        if ( pin > 100 )
        {
          pin = 100;
        }
        else if (pin < 6)
        {
          pin = 0;
        }
        else {}
        if ( pin >= 0 && pin <= 100)
        {
          /*pinTemp[0] = (char)(temp / 100 - 48);
            pinTemp[1] = (char)((temp % 100) / 10 + 48);
            pinTemp[2] = (char)((temp % 100) % 10 + 48);
            pinTemp[3] = '\0';*/
          dtostrf(pin, 4, 0, pinTemp);
        }
        else {}
        //Serial.println(pinTemp);
        //client.publish("room/pin", pinTemp);
        //        digitalWrite(D4, HIGH);
        //        delay(300);
        digitalWrite(D4, LOW);
        flag = 0;
        char_array[50] = NULL;
        i = 0;
      }
      else if ( flag == 5)
      {

        room_now = int((char_array[i - 3] - 48) * 10 + (char_array[i - 2] - 48));
        if ( room_now >= 0 && room_now <= 5)
        {
          /*room_now_Temp[0] = (char)(room_set / 10 - 48);
            room_now_Temp[1] = (char)(room_set % 10 - 48);
            room_now_Temp[2] = '\0';*/
          dtostrf(room_now, 4, 0, room_now_Temp);
        }
        else {}
        digitalWrite(D4, HIGH);
        flag = 0;
        flag_pub = 1;
        char_array[50] = NULL;
        i = 0;
      }
      else if ( flag == 6)
      {

        data_feedback = int((char_array[i - 2] - 48));
        if ( data_feedback >= 0 && data_feedback <= 10)
        {
          dtostrf(data_feedback, 4, 0, data_fb_Temp);
        }
        else {}
        digitalWrite(D4, LOW);
        flag = 0;
        char_array[50] = NULL;
        i = 0;
      }

      else {}

    }
    else
    {
      i++;
    }
  }
  else {}


}
