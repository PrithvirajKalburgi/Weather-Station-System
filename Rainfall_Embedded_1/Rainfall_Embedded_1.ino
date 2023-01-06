#include <LiquidCrystal.h> // include LCD library
// include Ethernet libarty W5500 library
#include <Ethernet.h> // incluide Ethernet library W5100
#include <PubSubClient.h> // include MQTT library
#include <TimerOne.h> // include timer library
#define rain 18
int pulses_counter=0;
int maxValue;
boolean is_rain = false;
LiquidCrystal lcd(37, 36, 35, 34, 33, 32); // LCD pin wiring settings for MEGA
EthernetClient ethClient;
static uint8_t mymac[6] = { 0x44,0x76,0x58,0x10,0x00,0x73 };
unsigned int Port = 1883; // MQTT port number
byte server[] = { 10,6,0,21 }; // TAMK IP
char* deviceId = "API-2022-RAIN1"; // * set your device id (will be the MQTT client username)
char* clientId = "223API"; // * set a random string (max 23 chars, will be the MQTT client id)
char* deviceSecret = "tamk";
void callback(char* topic, byte* payload, unsigned int length); // subscription callback for received MQTTT messages
PubSubClient client(server, Port, callback, ethClient); // mqtt client
#define inTopic "ICT4_in_2020" // * MQTT channel where data are received
#define outTopic "ICT4_out_2020"
void setup() {
pinMode(rain, INPUT);
attachInterrupt(digitalPinToInterrupt(rain), isr_rain, CHANGE);
Serial.begin(9600);
lcd.begin(20,4);
lcd.setCursor(0,0);
lcd.print("30.3.2020 Alyk jatk ");
Serial.println("Start 11.3.2022");
delay(500);
Timer1.initialize(10000000);
Timer1.attachInterrupt(Timer_isr);
fetch_IP();
Connect_MQTT_server();
}
void loop(){
while(true)
{
lcd.setCursor(0,3);
lcd.print("Send message: ");
lcd.print(pulses_counter);
send_MQTT_message(pulses_counter);
delay(10000);
}
}
void fetch_IP(void)
{
byte rev=1;
lcd.setCursor(0,0);
lcd.print(" Waiting IP ");
rev=Ethernet.begin( mymac);
Serial.print( F("\nW5100 Revision ") );
if ( rev == 0){
Serial.println( F( "Failed to access Ethernet controller" ) );
lcd.setCursor(0,0); lcd.print(" Ethernet failed ");
}
Serial.println( F( "Setting up DHCP" ));
Serial.print("Connected with IP: ");
Serial.println(Ethernet.localIP());
lcd.setCursor(0,0);
lcd.print(" ");
lcd.setCursor(0,0);
lcd.print("myIP=");
lcd.print(Ethernet.localIP());
delay(1500);
}
void send_MQTT_message(int num){
char bufa[100];
sprintf(bufa,"IOTJS={\"S_name\":\"%s\",\"S_value\":%d}",deviceId,num);
Serial.println( bufa );
if (client.connected()) {
client.publish(outTopic,bufa);
}
else {
delay(500);
lcd.setCursor(0,1);
lcd.print(" RE Connecting ");
Serial.println(" RE Connecting" );
client.connect(clientId, deviceId, deviceSecret);
delay(1000);
}
}
void isr_rain() {
pulses_counter++;
is_rain = !is_rain;
}
void Timer_isr() {
pulses_counter = 0;
}
void Connect_MQTT_server() // MQTT server connection
{
Serial.println(" Connecting to MQTT" );
Serial.print(server[0]); Serial.print("."); // Print MQTT server IP number to Serial monitor
Serial.print(server[1]); Serial.print(".");
Serial.print(server[2]); Serial.print(".");
Serial.println(server[3]);
lcd.setCursor(0,1);
// 012345678901234567890
lcd.print(" ");
lcd.setCursor(0,1);
lcd.print("MQTT=");
lcd.print(server[0]);lcd.print("."); // Print MQTT server IP number to LCD
lcd.print(server[1]);lcd.print(".");
lcd.print(server[2]);lcd.print(".");
lcd.print(server[3]);
delay(500);
if (!client.connected()) // check if allready connected
{
// connection to MQTT server
if (client.connect(clientId, deviceId, deviceSecret))
{
lcd.setCursor(0,1);
lcd.print("Conn"); // Connection is OK
Serial.println(" Connected OK " );
client.subscribe(inTopic); // subscript to in topic
}
else
{
lcd.setCursor(0,1);
// 01234567890123456789
lcd.print(" MQTT Error "); // error in connection
Serial.println(" MQTT Connection ERROR " );
}
}
}
// Receive incoming MQTT message
void callback(char* topic, byte* payload, unsigned int length)
{
// copu the payload content into a char*
char* receiv_string;
receiv_string = (char*) malloc(length + 1);
memcpy(receiv_string, payload, length); // copy received message to receiv_string
receiv_string[length] = '\0';
lcd.setCursor(0,0);
// 01234567890123456789
lcd.print("Mess= ");
lcd.setCursor(5,0);
lcd.print(receiv_string); // print reveived message to LCD
Serial.println( receiv_string );
free(receiv_string);
}
