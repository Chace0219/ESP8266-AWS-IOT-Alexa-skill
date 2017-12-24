#include <Arduino.h>
#include <Stream.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

//AWS
#include "sha256.h"
#include "Utils.h"

//WEBSockets
#include <Hash.h>
#include <WebSocketsClient.h>

//MQTT PAHO
#include <SPI.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>

//AWS MQTT Websocket
#include "Client.h"
#include "AWSWebSocketClient.h"
#include "CircularByteBuffer.h"

// Arduino json 
#include <ArduinoJson.h>

//AWS IOT config, change these:, ah, sorry, we have to change ssid and pass. Sure
char wifi_ssid[] = "Alum-Home";
char wifi_password[] = "Paruco0898";
char aws_endpoint[] = "a3t6n0x4jpx9lo.iot.us-east-1.amazonaws.com";
char aws_key[] = "AKIAJGS75MVXOAX3R2UA";
char aws_secret[] = "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";
char aws_region[] = "us-east-1";

// format - $aws/things/SHADOWNAME/shadow/update, $aws/things/SHADOWNAME/shadow/update/delta
// for example, lets set tihng name as testdevice
const char* update_topic = "$aws/things/testdevice/shadow/update";
const char* delta_topic = "$aws/things/testdevice/shadow/update/delta";

int port = 443;
//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

ESP8266WiFiMulti WiFiMulti;

AWSWebSocketClient awsWSclient(1000);

IPStack ipstack(awsWSclient);
MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers> *client = NULL;

//# of connections
long connection = 0;

//generate random mqtt clientID
char* generateClientID() {
	char* cID = new char[23]();
	for (int i = 0; i<22; i += 1)
		cID[i] = (char)random(1, 256);
	return cID;
}

//count messages arrived
int arrivedcount = 0;


/* relay pin definitons */
const uint8_t RELAYPIN01 = 12;
const uint8_t RELAYPIN02 = 13;
const uint8_t RELAYPIN03 = 14;
const uint8_t RELAYPIN04 = 16;
const uint8_t RELAYPIN05 = 4;
const uint8_t RELAYPIN06 = 5;
const uint8_t RELAYPIN07 = 15;
const uint8_t RELAYPIN08 = 10;

/* relay commands */
bool relay01_Command = false;
bool relay02_Command = false;
bool relay03_Command = false;
bool relay04_Command = false;
bool relay05_Command = false;
bool relay06_Command = false;
bool relay07_Command = false;
bool relay08_Command = false;

//callback to handle mqtt messages
void messageArrived(MQTT::MessageData& md)
{
	MQTT::Message &message = md.message;

	Serial.print("Message ");
	Serial.print(++arrivedcount);
	Serial.print(" arrived: qos ");
	Serial.print(message.qos);
	Serial.print(", retained ");
	Serial.print(message.retained);
	Serial.print(", dup ");
	Serial.print(message.dup);
	Serial.print(", packetid ");
	Serial.println(message.id);
	Serial.print("Payload ");
	char* msg = new char[message.payloadlen + 1]();
	memcpy(msg, message.payload, message.payloadlen);
	Serial.println(msg);

	//
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(msg);

	// Test if parsing succeeds.
	if (root.success())
	{
		Serial.println("parse success");
		JsonObject& state = root["state"].asObject();

		StaticJsonBuffer<200> serBuff;
		JsonObject& serRoot = serBuff.createObject();
		JsonObject& serState = serRoot.createNestedObject("state");
		JsonObject& serReported = serState.createNestedObject("reported");

		if (state.containsKey("relay01"))
		{
			const char* command = state["relay01"];
			String strCommand = String(command);
			if (strCommand == "on")
			{
				relay01_Command = true;
				Serial.println("first relay command turned on");
				serReported["relay01"] = "on";
			}
			else
			{
				relay01_Command = false;
				Serial.println("first relay command turned off");
				serReported["relay01"] = "off";
			}
		}

		if (state.containsKey("relay02"))
		{
			const char* command = state["relay02"];
			String strCommand = String(command);
			if (strCommand == "on")
			{
				relay02_Command = true;
				Serial.println("second relay command turned on");
				serReported["relay02"] = "on";
			}
			else
			{
				relay02_Command = false;
				Serial.println("second relay command turned off");
				serReported["relay02"] = "off";
			}
		}

		if (state.containsKey("relay03"))
		{
			const char* command = state["relay03"];
			String strCommand = String(command);
			if (strCommand == "on")
			{
				relay03_Command = true;
				Serial.println("third relay command turned on");
				serReported["relay03"] = "on";
			}
			else
			{
				relay03_Command = false;
				Serial.println("third relay command turned off");
				serReported["relay03"] = "off";
			}
		}

		if (state.containsKey("relay04"))
		{
			const char* command = state["relay04"];
			String strCommand = String(command);
			if (strCommand == "on")
			{
				relay04_Command = true;
				Serial.println("fourth relay command turned on");
				serReported["relay04"] = "on";
			}
			else
			{
				relay04_Command = false;
				Serial.println("fourth relay command turned off");
				serReported["relay04"] = "off";
			}
		}
		if (state.containsKey("relay05"))
		{
			const char* command = state["relay05"];
			String strCommand = String(command);
			if (strCommand == "on")
			{
				relay05_Command = true;
				Serial.println("5th relay command turned on");
				serReported["relay05"] = "on";
			}
			else
			{
				relay05_Command = false;
				Serial.println("5th relay command turned off");
				serReported["relay05"] = "off";
			}
		}
		if (state.containsKey("relay06"))
		{
			const char* command = state["relay06"];
			String strCommand = String(command);
			if (strCommand == "on")
			{
				relay06_Command = true;
				Serial.println("6th relay command turned on");
				serReported["relay06"] = "on";
			}
			else
			{
				relay06_Command = false;
				Serial.println("6th relay command turned off");
				serReported["relay06"] = "off";
			}
		}
		if (state.containsKey("relay07"))
		{
			const char* command = state["relay07"];
			String strCommand = String(command);
			if (strCommand == "on")
			{
				relay07_Command = true;
				Serial.println("7th relay command turned on");
				serReported["relay07"] = "on";
			}
			else
			{
				relay07_Command = false;
				Serial.println("7th relay command turned off");
				serReported["relay07"] = "off";
			}
		}
		if (state.containsKey("relay08"))
		{
			const char* command = state["relay08"];
			String strCommand = String(command);
			if (strCommand == "on")
			{
				relay08_Command = true;
				Serial.println("8th relay command turned on");
				serReported["relay08"] = "on";
			}
			else
			{
				relay08_Command = false;
				Serial.println("8th relay command turned off");
				serReported["relay08"] = "off";
			}
		}
		//send a message
		MQTT::Message message;
		char buf[200];
		serRoot.printTo(buf);
		serRoot.printTo(Serial);
		message.qos = MQTT::QOS0;
		message.retained = false;
		message.dup = false;
		message.payload = (void*)buf;
		message.payloadlen = strlen(buf) + 1;
		int rc = client->publish(update_topic, message);
	}
	delete msg;
}

//connects to websocket layer and mqtt layer
bool connect()
{
	if (client == NULL)
	{
		client = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
	}
	else
	{
		if (client->isConnected())
			client->disconnect();
		delete client;
		client = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
	}

	// delay is not necessary... it just help us to get a "trustful" heap space value
	delay(1000);
	Serial.print(millis());
	Serial.print(" - conn: ");
	Serial.print(++connection);
	Serial.print(" - (");
	Serial.print(ESP.getFreeHeap());
	Serial.println(")");

	int rc = ipstack.connect(aws_endpoint, port);
	if (rc != 1)
	{
		Serial.println("error connection to the websocket server");
		return false;
	}
	else
		Serial.println("websocket layer connected");


	Serial.println("MQTT connecting");
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.MQTTVersion = 3;
	char* clientID = generateClientID();
	data.clientID.cstring = clientID;
	rc = client->connect(data);
	delete[] clientID;
	if (rc != 0)
	{
		Serial.print("error connection to MQTT server");
		Serial.println(rc);
		return false;
	}
	Serial.println("MQTT connected");
	return true;
}

//subscribe to a mqtt topic
void subscribe()
{
	//subscript to a topic
	int rc = client->subscribe(delta_topic, MQTT::QOS0, messageArrived);
	if (rc != 0)
	{
		Serial.print("rc from MQTT subscribe is ");
		Serial.println(rc);
		return;
	}
	Serial.println("MQTT subscribed");
}

//send a message to a mqtt topic
void sendmessage()
{
	//send a message
	MQTT::Message message;
	char buf[100];
	strcpy(buf, "{\"state\":{\"desired\":{\"firstrelay\": false}}}");
	message.qos = MQTT::QOS0;
	message.retained = false;
	message.dup = false;
	message.payload = (void*)buf;
	message.payloadlen = strlen(buf) + 1;
	int rc = client->publish(update_topic, message);
}

void setup()
{
	Serial.begin(115200);
	delay(2000);
	Serial.setDebugOutput(1);

	// pin mode 
	pinMode(RELAYPIN01, OUTPUT);
	digitalWrite(RELAYPIN01, LOW);

	pinMode(RELAYPIN02, OUTPUT);
	digitalWrite(RELAYPIN02, LOW);

	pinMode(RELAYPIN03, OUTPUT);
	digitalWrite(RELAYPIN03, LOW);

	pinMode(RELAYPIN04, OUTPUT);
	digitalWrite(RELAYPIN04, LOW);

	pinMode(RELAYPIN05, OUTPUT);
	digitalWrite(RELAYPIN05, LOW);

	pinMode(RELAYPIN06, OUTPUT);
	digitalWrite(RELAYPIN06, LOW);

	pinMode(RELAYPIN07, OUTPUT);
	digitalWrite(RELAYPIN07, LOW);

	pinMode(RELAYPIN08, OUTPUT);
	digitalWrite(RELAYPIN08, LOW);

	//fill with ssid and wifi password
	WiFiMulti.addAP(wifi_ssid, wifi_password);
	Serial.println("connecting to wifi");
	while (WiFiMulti.run() != WL_CONNECTED)
	{
		delay(100);
		Serial.print(".");
	}
	Serial.println("\nconnected");

	//fill AWS parameters    
	awsWSclient.setAWSRegion(aws_region);
	awsWSclient.setAWSDomain(aws_endpoint);
	awsWSclient.setAWSKeyID(aws_key);
	awsWSclient.setAWSSecretKey(aws_secret);
	awsWSclient.setUseSSL(true);

	if (connect())
	{
		subscribe();
		// sendmessage();
	}
}

void loop()
{
	//keep the mqtt up and running
	if (awsWSclient.connected())
	{
		client->yield();
	}
	else
	{
		//handle reconnection
		if (connect())
		{
			subscribe();
		}
	}

	// relay control
	digitalWrite(RELAYPIN01, relay01_Command);
	digitalWrite(RELAYPIN02, relay02_Command);
	digitalWrite(RELAYPIN03, relay03_Command);
	digitalWrite(RELAYPIN04, relay04_Command);
	digitalWrite(RELAYPIN05, relay05_Command);
	digitalWrite(RELAYPIN06, relay06_Command);
	digitalWrite(RELAYPIN07, relay07_Command);
	digitalWrite(RELAYPIN08, relay08_Command); // my mistake, lets upload sketch again
}