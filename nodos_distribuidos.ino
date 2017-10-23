/*



 * ************************************ENVIO ESTADO BOTON**********************************/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// PINES PARA RELES 4 5 16 0
#define LED_PIN1 14
#define LED_PIN2 4
#define LED_PIN3 5
#define LED_PIN4 16
// PINES PARA BOTONES 12 13 15
int BUTTON_PIN0 = 12; //button is connected to GPIO pin D1
int BUTTON_PIN1 = 13;
int BUTTON_PIN2 = 15;
//int BUTTON_PIN3 = 4;
// Update these with values suitable for your network.
const char* ssid = "DIEGO";
const char* password = "20083066";
const char* mqtt_server = "broker.mqtt-dashboard.com";
//const char* mqtt_server = "iot.eclipse.org";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
long fertilizar;
//bool boolFertilizar=false;
bool pressed0=true;
bool pressed1=true;
bool pressed2=true;
//bool pressed3=true;
void setup_wifi() {
  int intentos=0;
   delay(1000);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && intentos<=25) 
    {
      delay(500);
      Serial.print(".");
      intentos++;
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
      Serial.print("Command from MQTT broker is : [");
      Serial.print(topic);
      int p =(char)payload[0]-'0';
  
   //ILUMINAR////////////////////////////////
      if(p==0) 
      {
         digitalWrite(LED_PIN1, HIGH); 
        Serial.println(" ILUMINAR OFF " );
      }
      if(p==1)
      {
         digitalWrite(LED_PIN1, LOW); 
        Serial.println(" ILUMINAR ON " );
      }
  //OXIGENAR////////////////////////////////     
      if(p==2) 
      {
         digitalWrite(LED_PIN2, HIGH); 
        Serial.println(" OXIGENAR OFF " );
      } 
      if(p==3)
      {
         digitalWrite(LED_PIN2, LOW); 
        Serial.println(" OXIGENAR ON " );
      }
  //FERTILIZAR////////////////////////////////
       if(p==4) 
      {
        digitalWrite(LED_PIN3, HIGH); 
        Serial.println(" FERTILIZAR A OFF " );
        digitalWrite(LED_PIN4, HIGH); 
        Serial.println(" FERTILIZAR B OFF " );
      } 
      if(p==5)
      {
        fertilizar_modulo();
      }
      Serial.println();
} //end callback

void reconnect() {
  // Loop until we're reconnected
  int intentos=0;
  while (!client.connected()&&intentos<=2) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("mod11");
      client.subscribe("mod12");
      client.subscribe("mod13");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    intentos++;
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //declaracion pines
  pinMode(BUTTON_PIN0,INPUT);
  pinMode(BUTTON_PIN1,INPUT);
  pinMode(BUTTON_PIN2,INPUT);
  //pinMode(BUTTON_PIN3,INPUT);
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  pinMode(LED_PIN4, OUTPUT);
  digitalWrite(LED_PIN1, HIGH);
  digitalWrite(LED_PIN2, HIGH); 
  digitalWrite(LED_PIN3, HIGH);
  digitalWrite(LED_PIN4, HIGH);  
}

void loop() {
  if (!client.connected()&&WiFi.localIP()!=0) {
    reconnect();
  }
  client.loop();
  long now = millis();
  int status0;
  int status1;
  int status2;
  //每隔0.5s发送一次数据
  if (now - lastMsg > 50) {
     lastMsg = now;
     status0=digitalRead(BUTTON_PIN0);
     status1=digitalRead(BUTTON_PIN1);
     status2=digitalRead(BUTTON_PIN2);
     String msg="";
     Serial.print("STATUS 0: " );
     Serial.println(status0 );
     Serial.print("STATUS 1: " );
     Serial.println(status1 );
     Serial.print("STATUS 2: " );
     Serial.println(status2 );
     Serial.print("estado rele: " );
     Serial.println(digitalRead(LED_PIN1) );
     ///STATUS 0
     if(status0==HIGH){
      if(pressed0){
        if(digitalRead(LED_PIN1)==LOW){
           msg= msg+ "0";
           char message[58];
           msg.toCharArray(message,58);
           Serial.println(message);
           digitalWrite(LED_PIN1, HIGH); 
           Serial.println(" ILUMINACION OFF " );
           //publish sensor data to MQTT broker
           client.publish("mod13", message);
        }else{
           msg= msg+ "1";
           char message[58];
           msg.toCharArray(message,58);
           Serial.println(message);
           digitalWrite(LED_PIN1, LOW); 
           Serial.println(" ILUMINACION ON " );
           //publish sensor data to MQTT broker
           client.publish("mod13", message);
        }
        pressed0=false;
      }
     }
     else
     {
        pressed0=true;
     }
     //STATUS 1
     if(status1==HIGH){
      if(pressed1){
        if(digitalRead(LED_PIN2)==LOW){
           msg= msg+ "2";
           char message[58];
           msg.toCharArray(message,58);
           Serial.println(message);
           digitalWrite(LED_PIN2, HIGH); 
           Serial.println(" OXIGENAR OFF" );
           //publish sensor data to MQTT broker
           client.publish("mod12", message);
        }else{
           msg= msg+ "3";
           char message[58];
           msg.toCharArray(message,58);
           Serial.println(message);
           digitalWrite(LED_PIN2, LOW);
           Serial.println(" OXIGENAR ON" ); 
           //publish sensor data to MQTT broker
           client.publish("mod12", message);
        }
        pressed1=false;
      }
     }
     else
     {
        pressed1=true;
     }
     //STATUS 2
     if(status2==HIGH){
      if(pressed2){
        if(digitalRead(LED_PIN3)==LOW){
           digitalWrite(LED_PIN3, HIGH);
           digitalWrite(LED_PIN4, HIGH);  
           Serial.println(" FERTILIZAR OFF " );
           //publish sensor data to MQTT broker
        }else{
           fertilizar_modulo();
        }
        pressed2=false;
      }
     }
     else
     {
        pressed2=true;
     }
    }
    
    /*if (now - fertilizar > 6000) {
       //PARA QUE ENVIE EL ESTADO CADA 6 SEGUNDOS
       //Serial.println(" PASARON % SEGUNDOS!!!!!!!!!!!!!!!" );
       if(digitalRead(LED_PIN3)==LOW){
          Serial.println(" FERTILIZAR A ON ON" );
          digitalWrite(LED_PIN3, HIGH);
          digitalWrite(LED_PIN4, LOW);
       }
    }
    if (now - fertilizar > 12000) {
       if(digitalRead(LED_PIN4)==LOW){
          Serial.println(" FERTILIZAR B ON ON" );
          digitalWrite(LED_PIN4, HIGH);
          fertilizar = now;
       }
    }*/
}

void fertilizar_modulo(){
  Serial.println(" FERTILIZAR ON " );
    digitalWrite(LED_PIN3, LOW);
    delay(5000);
    digitalWrite(LED_PIN3, HIGH);
    digitalWrite(LED_PIN4, LOW);
    delay(5000);
    digitalWrite(LED_PIN4, HIGH);
  Serial.println(" FERTILIZAR OFF " ); 
}

