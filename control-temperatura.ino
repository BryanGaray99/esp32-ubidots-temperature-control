#include "UbidotsEsp32Mqtt.h"
#include "DHT.h"

/****************************************
Define Constants
****************************************/

#define WIFISSID ""         // Wi-Fi SSID
#define PASSWORD ""         // Wi-Fi password
#define TOKEN ""             // Ubidots Token
#define DEVICE_LABEL ""   // Device label in Ubidots

#define DHTTYPE DHT11       // Type of DHT sensor
#define TEMPERATURE "temperature"              // Variable label for temperature in Ubidots
#define HUMIDITY "humidity"                    // Variable label for humidity in Ubidots
#define MANUAL_MODE "manual-mode"              // Variable label for manual mode in Ubidots
#define MOTOR "motor"                          // Variable label for motor in Ubidots
#define LIGHT_BULD "light-bulb"                // Variable label for light bulb in Ubidots

#define DHTPIN 23             // Pin for DHT sensor
#define ENABLE1_PIN 19        // Pin for motor control
#define MOTOR1_PIN1 18        // Motor control pin 1
#define MOTOR1_PIN2 5         // Motor control pin 2
#define LIGHT_PIN 26           // Pin for light bulb control

const uint8_t NUMBER_OF_VARIABLES = 3;  // Number of variables to subscribe to
char *variable_labels[NUMBER_OF_VARIABLES] = {"manual-mode", "motor", "light-bulb"}; // Labels of variables to subscribe to

float TEMPERATURE_VAR;      // Variable to store temperature
float HUMIDITY_VAR;         // Variable to store humidity
int MOTOR_VAR;              // Variable to store motor state
int LIGHT_BULD_VAR;         // Variable to store light bulb state

float value;                // To store incoming value
uint8_t variable;           // To keep track of the state machine and use the switch case

const int ERROR_VALUE = 65535;   // Error value

/* Set the publication frequency to 5 seconds */
const int PUBLISH_FREQUENCY = 5000; 

/* Define extra variables for the library */
unsigned long timer;
uint8_t analogPin = 34; 

// Define temperature limits
const float TEMPERATURE_MIN = 28.0;
const float TEMPERATURE_MAX = 30.0;
// Variable for manual/automatic mode
int MANUAL_MODE_VAR = 0;

/****************************************
Initialize constructors for objects
****************************************/
Ubidots ubidots(TOKEN);
DHT dht(DHTPIN, DHTTYPE);

/****************************************
Auxiliary Functions
****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
    char *variable_label = (char *)malloc(sizeof(char) * 30);
    get_variable_label_topic(topic, variable_label);
    Serial.println(variable_label);
    value = btof(payload, length);
    set_state(variable_label);
    execute_cases();
    free(variable_label);
}

// Parse topic to extract the variable label which changed value
void get_variable_label_topic(char *topic, char *variable_label)
{
    Serial.print("topic:");
    Serial.println(topic);
    sprintf(variable_label, "");
    for (int i = 0; i < NUMBER_OF_VARIABLES; i++)
    {
        char *result_lv = strstr(topic, variable_labels[i]);
        if (result_lv != NULL)
        {
            uint8_t len = strlen(result_lv);
            char result[100];
            uint8_t i = 0;
            for (i = 0; i < len - 3; i++)
            {
                result[i] = result_lv[i];
            }
            result[i] = '\0';
            Serial.print("Label is: ");
            Serial.println(result);
            sprintf(variable_label, "%s", result);
            break;
        }
    }
}

// Cast from an array of chars to float value
float btof(byte *payload, unsigned int length)
{
    char *demo_ = (char *)malloc(sizeof(char) * 10);
    for (int i = 0; i < length; i++)
    {
        demo_[i] = payload[i];
    }
    return atof(demo_);
}

// State machine to use switch case
void set_state(char *variable_label)
{
    variable = 0;
    for (uint8_t i = 0; i < NUMBER_OF_VARIABLES; i++)
    {
        if (strcmp(variable_label, variable_labels[i]) == 0)
        {
            break;
        }
        variable++;
    }
    if (variable >= NUMBER_OF_VARIABLES)
        variable = ERROR_VALUE; // Not valid
}

// Function with switch case to determine which variable changed and assign the value accordingly to the code variable
void execute_cases()
{
    switch (variable)
    {
    case 0:
        MANUAL_MODE_VAR = static_cast<int>(value); // Convert the value to an integer
        Serial.print("MANUAL_MODE_VAR: ");
        Serial.println(MANUAL_MODE_VAR);
        Serial.println();
        break;
    case 1:
        MOTOR_VAR = value;
        if (value == 1.0)
        {
          Serial.println("Received value: 1.0 - Turning on the motor");
          turnOnMotor();
        }
        else
        {
          Serial.println("Received value: 0.0 - Turning off the motor");
          turnOffMotor();
        }
        Serial.print("MOTOR_VAR: ");
        Serial.println(MOTOR_VAR);
        Serial.println();
        break;
    case 2:
        LIGHT_BULD_VAR = value;
        if (value == 1.0)
        {
          Serial.println("Received value: 1.0 - Turning on the light bulb");
          turnOnLight();
        }
        else
        {
          Serial.println("Received value: 0.0 - Turning off the light bulb");
          turnOffLight();
        }
        Serial.print("LIGHT_BULD_VAR: ");
        Serial.println(LIGHT_BULD_VAR);
        Serial.println();
        break;
    case ERROR_VALUE:
        Serial.println("Error");
        Serial.println();
        break;
    default:
        Serial.println("Default");
        Serial.println();
    }
}

/****************************************
Main Functions
****************************************/

void setup()
{
  Serial.begin(115200);
  pinMode(ENABLE1_PIN, OUTPUT);
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);

  // ubidots.setDebug(true);  // Uncomment this to make debug messages available
  ubidots.connectToWifi(WIFISSID, PASSWORD);
  ubidots.setCallback(callback);

  // DHT11
  Serial.println("DHT11 turned on");
  dht.begin();
  delay(1000);

  ubidots.setup();
  ubidots.reconnect();
  for (uint8_t i = 0; i < NUMBER_OF_VARIABLES; i++)
  {
      ubidots.subscribeLastValue(DEVICE_LABEL, variable_labels[i]); // Insert the device and Variable's Labels
      delay(100);
  }
  timer = millis();
}

void loop()
{
    if (!ubidots.connected())
    {
        ubidots.reconnect();
        for (uint8_t i = 0; i < NUMBER_OF_VARIABLES; i++)
        {
            ubidots.subscribeLastValue(DEVICE_LABEL, variable_labels[i]); // Insert the device and Variable's Labels
            delay(100);
        }
    }

    // Read the current state of the manual mode
    int currentManualMode = MANUAL_MODE_VAR;

    if (labs(static_cast<long>(millis()) - static_cast<long>(timer)) > PUBLISH_FREQUENCY)
    {
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        ubidots.add(TEMPERATURE, t);
        ubidots.add(HUMIDITY, h);
        ubidots.publish(DEVICE_LABEL);

        Serial.println("Sending data to Ubidots:");
        Serial.println("Temperature: " + String(t));
        Serial.println("Humidity: " + String(h));
        Serial.println("-----------------------------------------");
        timer = millis();

        // Control turning on/off the light bulb and motor based on temperature and manual mode
        if (t < TEMPERATURE_MIN && currentManualMode == 0)
        {
            // Turn on the light bulb
            turnOnLight();
            Serial.println("Temperature is below the minimum threshold. Turning on light.");
        }
        else if (t > TEMPERATURE_MAX && currentManualMode == 0)
        {
            // Turn on the motor
            turnOnMotor();
            Serial.println("Temperature is above the maximum threshold. Turning on motor.");
        }
        else if (currentManualMode == 0)
        {
            // Turn off both the light bulb and the motor
            turnOffLight();
            turnOffMotor();
            Serial.println("Temperature is within the acceptable range. Turning off light and motor.");
        }
    }

    ubidots.loop();
}

void turnOnMotor()
{
  Serial.println("Turning on motor");
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, HIGH);
  analogWrite(ENABLE1_PIN, 255);
}

void turnOffMotor()
{
  Serial.println("Turning off motor");
  analogWrite(ENABLE1_PIN, 0);
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, LOW);
}

void turnOnLight()
{
  Serial.println("Turning on light");
  digitalWrite(LIGHT_PIN, LOW);
}

void turnOffLight()
{
  Serial.println("Turning off light");
  digitalWrite(LIGHT_PIN, HIGH);
}
