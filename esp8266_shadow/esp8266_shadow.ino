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
#include "FiniteStateMachine.h"

/* relay pin definitons */
const uint8_t MOTOR1OPEN = 12;
const uint8_t MOTOR1CLOSE = 13;
const uint8_t MOTOR2OPEN = 14;
const uint8_t MOTOR2CLOSE = 16;
const uint8_t MOTOR3OPEN = 4;
const uint8_t MOTOR3CLOSE = 5;
const uint8_t MOTOR4OPEN = 15;
const uint8_t MOTOR4CLOSE = 10;

//
const uint16_t FULLCYCLE = 20000;

void statusChanged();

/* */
State idle01(statusChanged, NULL, NULL);
State forward01(statusChanged, NULL, NULL);
State backward01(statusChanged, NULL, NULL);
FiniteStateMachine liner01Controller(idle01);
uint8_t motor1_position = 0;
uint16_t motor1_ActionTime = 0;

State idle02(statusChanged, NULL, NULL);
State forward02(statusChanged, NULL, NULL);
State backward02(statusChanged, NULL, NULL);
FiniteStateMachine liner02Controller(idle02);
uint8_t motor2_position = 0;
uint16_t motor2_ActionTime = 0;

State idle03(statusChanged, NULL, NULL);
State forward03(statusChanged, NULL, NULL);
State backward03(statusChanged, NULL, NULL);
FiniteStateMachine liner03Controller(idle03);
uint8_t motor3_position = 0;
uint16_t motor3_ActionTime = 0;

State idle04(statusChanged, NULL, NULL);
State forward04(statusChanged, NULL, NULL);
State backward04(statusChanged, NULL, NULL);
FiniteStateMachine liner04Controller(idle04);
uint8_t motor4_position = 0;
uint16_t motor4_ActionTime = 0;



// wifi and aws connection information for device
// char wifi_ssid[] = "Alum-Home";
// char wifi_password[] = "Paruco0898";
char wifi_ssid[] = "TP-LINK_64FB80";
char wifi_password[] = "chunxing151201";
char aws_endpoint[] = "a3t6n0x4jpx9lo.iot.us-east-1.amazonaws.com";
char aws_key[] = "AKIAJELWOIVPZ65JS5YQ";
char aws_secret[] = "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";
char aws_region[] = "us-east-1";

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

	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(msg);
	// Test if parsing succeeds.
	if (root.success())
	{
		Serial.println("parse success");
		JsonObject& state = root["state"].asObject();
		if (state.containsKey("motor1"))
		{
			JsonObject& motor = state["motor1"].asObject();
			uint8_t newPos = motor["position"];
			if (newPos > motor1_position)
			{
				motor1_ActionTime = map(newPos - motor1_position, 0, 100, 0, FULLCYCLE);
				Serial.print(F("motor1 forward time :"));
				Serial.println(motor1_ActionTime);
				liner01Controller.transitionTo(forward01);
			}
			else if(newPos < motor1_position)
			{
				motor1_ActionTime = map(motor1_position - newPos, 0, 100, 0, FULLCYCLE);
				Serial.print(F("motor1 backward time is "));
				Serial.println(motor1_ActionTime);
				liner01Controller.transitionTo(backward01);
			}
			motor1_position = newPos;
		}
		else if (state.containsKey("motor2"))
		{
			JsonObject& motor = state["motor2"].asObject();
			uint8_t newPos = motor["position"];
			if (newPos > motor2_position)
			{
				motor2_ActionTime = map(newPos - motor2_position, 0, 100, 0, FULLCYCLE);
				Serial.print(F("motor2 forward time is "));
				Serial.println(motor2_ActionTime);
				liner02Controller.transitionTo(forward02);
			}
			else if (newPos < motor2_position)
			{
				motor2_ActionTime = map(motor2_position - newPos, 0, 100, 0, FULLCYCLE);
				Serial.print(F("motor2 backward time is "));
				Serial.println(motor2_ActionTime);
				liner02Controller.transitionTo(backward02);
			}
			motor2_position = newPos;
		}
		else if (state.containsKey("motor3"))
		{
			JsonObject& motor = state["motor3"].asObject();
			uint8_t newPos = motor["position"];
			if (newPos > motor3_position)
			{
				motor3_ActionTime = map(newPos - motor3_position, 0, 100, 0, FULLCYCLE);
				Serial.print(F("motor3 forward time is "));
				Serial.println(motor3_ActionTime);
				liner03Controller.transitionTo(forward03);
			}
			else if (newPos < motor3_position)
			{
				motor3_ActionTime = map(motor3_position - newPos, 0, 100, 0, FULLCYCLE);
				Serial.print(F("motor3 backward time is "));
				Serial.println(motor3_ActionTime);
				liner03Controller.transitionTo(backward03);
			}
			motor3_position = newPos;
		}
		else if (state.containsKey("motor4"))
		{
			JsonObject& motor = state["motor4"].asObject();
			uint8_t newPos = motor["position"];
			if (newPos > motor4_position)
			{
				motor4_ActionTime = map(newPos - motor4_position, 0, 100, 0, FULLCYCLE);
				Serial.print(F("motor4 forward time is "));
				Serial.println(motor4_ActionTime);
				liner04Controller.transitionTo(forward04);
			}
			else if (newPos < motor4_position)
			{
				motor4_ActionTime = map(motor4_position - newPos, 0, 100, 0, FULLCYCLE);
				Serial.print(F("motor4 backward time is "));
				Serial.println(motor4_ActionTime);
				liner04Controller.transitionTo(backward04);
			}
			motor4_position = newPos;
		}
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
void updateShadow()
{
	StaticJsonBuffer<1024> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	JsonObject& state = root.createNestedObject("state");
	JsonObject& reported = state.createNestedObject("reported");
	JsonObject& desired = state.createNestedObject("desired");

	JsonObject& reportedMotor01 = reported.createNestedObject("motor1");
	JsonObject& desiredMotor01 = desired.createNestedObject("motor1");
	reportedMotor01["position"] = motor1_position;
	desiredMotor01["position"] = motor1_position;
	if (liner01Controller.isInState(idle01))
	{
		reportedMotor01["action"] = "idle";
		desiredMotor01["action"] = "idle";
	}
	else if (liner01Controller.isInState(forward01))
	{
		reportedMotor01["action"] = "forward";
		desiredMotor01["action"] = "forward";
	}
	else if (liner01Controller.isInState(backward01))
	{
		reportedMotor01["action"] = "backward";
		desiredMotor01["action"] = "backward";
	}

	JsonObject& reportedMotor02 = reported.createNestedObject("motor2");
	JsonObject& desiredMotor02 = desired.createNestedObject("motor2");
	reportedMotor02["position"] = motor2_position;
	desiredMotor02["position"] = motor2_position;
	if (liner02Controller.isInState(idle02))
	{
		reportedMotor02["action"] = "idle";
		desiredMotor02["action"] = "idle";
	}
	else if (liner02Controller.isInState(forward02))
	{
		reportedMotor02["action"] = "forward";
		desiredMotor02["action"] = "forward";
	}
	else if (liner02Controller.isInState(backward02))
	{
		reportedMotor02["action"] = "backward";
		desiredMotor02["action"] = "backward";
	}

	JsonObject& reportedMotor03 = reported.createNestedObject("motor3");
	JsonObject& desiredMotor03 = desired.createNestedObject("motor3");
	reportedMotor03["position"] = motor3_position;
	desiredMotor03["position"] = motor3_position;
	if (liner03Controller.isInState(idle03))
	{
		reportedMotor03["action"] = "idle";
		desiredMotor03["action"] = "idle";
	}
	else if (liner03Controller.isInState(forward03))
	{
		reportedMotor03["action"] = "forward";
		desiredMotor03["action"] = "forward";
	}
	else if (liner03Controller.isInState(backward03))
	{
		reportedMotor03["action"] = "backward";
		desiredMotor03["action"] = "backward";
	}

	JsonObject& reportedMotor04 = reported.createNestedObject("motor4");
	JsonObject& desiredMotor04 = desired.createNestedObject("motor4");
	reportedMotor04["position"] = motor4_position;
	desiredMotor04["position"] = motor4_position;
	if (liner04Controller.isInState(idle04))
	{
		reportedMotor04["action"] = "idle";
		desiredMotor04["action"] = "idle";
	}
	else if (liner04Controller.isInState(forward04))
	{
		reportedMotor04["action"] = "forward";
		desiredMotor04["action"] = "forward";
	}
	else if (liner04Controller.isInState(backward04))
	{
		reportedMotor04["action"] = "backward";
		desiredMotor04["action"] = "backward";
	}

	MQTT::Message message;
	char buf[1024];
	root.printTo(buf);
	Serial.println(buf);
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
	pinMode(MOTOR1OPEN, OUTPUT);
	digitalWrite(MOTOR1OPEN, LOW);
	pinMode(MOTOR1CLOSE, OUTPUT);
	digitalWrite(MOTOR1CLOSE, LOW);

	pinMode(MOTOR2OPEN, OUTPUT);
	digitalWrite(MOTOR2OPEN, LOW);
	pinMode(MOTOR2CLOSE, OUTPUT);
	digitalWrite(MOTOR2CLOSE, LOW);

	pinMode(MOTOR3OPEN, OUTPUT);
	digitalWrite(MOTOR3OPEN, LOW);
	pinMode(MOTOR3CLOSE, OUTPUT);
	digitalWrite(MOTOR3CLOSE, LOW);
	pinMode(MOTOR4OPEN, OUTPUT);
	digitalWrite(MOTOR4OPEN, LOW);
	pinMode(MOTOR4CLOSE, OUTPUT);
	digitalWrite(MOTOR4CLOSE, LOW);

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
		updateShadow();
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
			updateShadow();
		}
	}

	liner01Controller.update();
	liner02Controller.update();
	liner03Controller.update();
	liner04Controller.update();

	if (liner01Controller.isInState(idle01))
	{
		digitalWrite(MOTOR1OPEN, LOW);
		digitalWrite(MOTOR1CLOSE, LOW);
	}
	else if (liner01Controller.isInState(forward01))
	{
		if (liner01Controller.timeInCurrentState() >= motor1_ActionTime)
			liner01Controller.transitionTo(idle01);
		else
		{
			digitalWrite(MOTOR1OPEN, HIGH);
			digitalWrite(MOTOR1CLOSE, LOW);
		}
	}
	else if (liner01Controller.isInState(backward01))
	{
		if (liner01Controller.timeInCurrentState() >= motor1_ActionTime)
			liner01Controller.transitionTo(idle01);
		else
		{
			digitalWrite(MOTOR1OPEN, LOW);
			digitalWrite(MOTOR1CLOSE, HIGH);
		}
	}

	if (liner02Controller.isInState(idle02))
	{
		digitalWrite(MOTOR2OPEN, LOW);
		digitalWrite(MOTOR2CLOSE, LOW);
	}

	else if (liner02Controller.isInState(forward02))
	{
		if (liner02Controller.timeInCurrentState() >= motor2_ActionTime)
			liner02Controller.transitionTo(idle02);
		else
		{
			digitalWrite(MOTOR2OPEN, HIGH);
			digitalWrite(MOTOR2CLOSE, LOW);
		}
	}
	else if (liner02Controller.isInState(backward02))
	{
		if (liner02Controller.timeInCurrentState() >= motor2_ActionTime)
			liner02Controller.transitionTo(idle02);
		else
		{
			digitalWrite(MOTOR2OPEN, LOW);
			digitalWrite(MOTOR2CLOSE, HIGH);
		}
	}

	if (liner03Controller.isInState(idle03))
	{
		digitalWrite(MOTOR3OPEN, LOW);
		digitalWrite(MOTOR3CLOSE, LOW);
	}
	else if (liner03Controller.isInState(forward03))
	{
		if (liner03Controller.timeInCurrentState() >= motor3_ActionTime)
			liner03Controller.transitionTo(idle03);
		else
		{
			digitalWrite(MOTOR3OPEN, HIGH);
			digitalWrite(MOTOR3CLOSE, LOW);
		}
	}
	else if (liner03Controller.isInState(backward03))
	{
		if (liner03Controller.timeInCurrentState() >= motor3_ActionTime)
			liner03Controller.transitionTo(idle03);
		else
		{
			digitalWrite(MOTOR3OPEN, LOW);
			digitalWrite(MOTOR3CLOSE, HIGH);
		}
	}

	if (liner04Controller.isInState(idle04))
	{
		digitalWrite(MOTOR4OPEN, LOW);
		digitalWrite(MOTOR4CLOSE, LOW);
	}
	else if (liner04Controller.isInState(forward04))
	{
		if (liner04Controller.timeInCurrentState() >= motor4_ActionTime)
			liner04Controller.transitionTo(idle04);
		else
		{
			digitalWrite(MOTOR4OPEN, HIGH);
			digitalWrite(MOTOR4CLOSE, LOW);
		}
	}
	else if (liner04Controller.isInState(backward04))
	{
		if (liner04Controller.timeInCurrentState() >= motor4_ActionTime)
			liner04Controller.transitionTo(idle04);
		else
		{
			digitalWrite(LED_BUILTIN, LOW);
			digitalWrite(MOTOR4OPEN, LOW);
			digitalWrite(MOTOR4CLOSE, HIGH);
		}
	}
}

void statusChanged()
{
	updateShadow();
}