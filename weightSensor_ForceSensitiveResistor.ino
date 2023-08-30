//PROGRAM START

//To be modified. These are the bounds of the range of values that each LED corresponds to.
double redLedLow = 0;
double redLedHigh = 300;
double greenLedLow = 300;
double greenLedHigh = 600;
double blueLedLow = 600;
double blueLedHigh = 1000;


//Voltage divider reader
int ProbePin = 2;


//Utility variables and constants for the circuitry and live calculations.
int RED = 13;
int BLUE = 12;
int GRE = 11;

//Will contain the mass value
double T;

double Vout = 0;
unsigned long totalVout = 0;
unsigned long curVout = 0;
unsigned long cycle = 0;
unsigned long curCycle = 0;
int refresh = 250;
int sensorValue;
int minV;

const int buttonPin2 = 3;
const int power_the_button2 = 1;
int buttonState2 = 0;

const int buttonPin3 = 5;
int buttonState3 = 0;
unsigned long milliStart = 0;
unsigned long milliStartWait = 0;
bool counting = false;
bool waiting = false;

//Setup the program and ports.
void setup()
{
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GRE, OUTPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(power_the_button2, OUTPUT);
  Serial.begin(9600);
}

//The main code that repeats indefinitely
void loop()
{
  //Read voltage at the voltage divider with the force sensitive resistor
  sensorValue = analogRead(ProbePin);

  //Check if a button is pressed
  digitalWrite(power_the_button2, HIGH);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);
  //If red button pressed, print the immediate current voltage value and mass value.
  if (buttonState2 != LOW)
  {
    Serial.print("********************PRESSED ");
    printVoltage(sensorValue);
    Serial.println(getMass(sensorValue*5.000/1023.000));
  }
  //If blue button pressed, start the voltage read process. This entails waiting for 20 seconds then taking a 5 second voltage measurement
  if (buttonState3 != LOW)
  {
    //If a current voltage read process is in progress, cancel it and restart the process.
    if (waiting || counting)
    {
      Serial.println("=================Restarted measurement process.");
      waiting = false;
      counting = false;
      totalVout = 0;
      cycle = 0;
    }

    //Start the 20 second wait.
    Serial.println("================================STARTED WAITING");
    waiting = true;
    milliStartWait = millis() % 10000;
    off_all();
    delay(1000);
  }

  //Once the waiting period is over, start reading the continuous flow of voltage values for 5 seconds
  if(waiting && millis()%10000 == milliStartWait)
  {
    waiting = false;
    milliStartWait = 0;
    Serial.println("================================DONE WAITING, STARTED READING");
    counting = true;
    milliStart = millis() % 5000;
    delay(100);
  }

  //If in the voltage read process, add current voltage to a total and increment a counter.
  if(counting)
  {
    cycle+=1;
    totalVout += sensorValue;
  }

  //After 5 seconds of reading voltage, stop.
  if(counting && millis() % 5000 == milliStart)
  {
    Serial.print("FINAL VOLTAGE: ");

    //Print the average voltage.
    printVoltage(totalVout / double(cycle));

    //Calculate mass using the equation, which is found in getMass function below
    T = getMass((totalVout / double(cycle))*5.0/1023.0);
    Serial.print("MASS: ");
    Serial.println(T);

    //Reset utility variables
    counting = false;
    totalVout = 0;
    cycle = 0;

    //Activate correct led using the boundary values found at the top of this code. LEDs aren't mutually exclusive according to rules.
    if (T >= blueLedLow && T <= blueLedHigh)
    {
      digitalWrite(BLUE, HIGH);
    }
    if (T >= greenLedLow && T <= greenLedHigh)
    {
      digitalWrite(GRE, HIGH);
    }
    if (T >= redLedLow && T <= redLedHigh)
    {
      digitalWrite(RED, HIGH);
    }
  }
    
}

//***FUNCTIONS***

//Turn off all LEDs
void off_all()
{
  digitalWrite(RED, LOW);
  digitalWrite(GRE, LOW);
  digitalWrite(BLUE, LOW);
}

float e=2.71828;
//Contains equation for Voltage -> Mass
double getMass(double x)
{
  //return -1484077 + 1925261*x - 997933.4*pow(x, 2) + 258300.5*pow(x, 3) - 33383.97*pow(x, 4) + 1723.884*pow(x, 5);
  //A previous version of the equation, kept in a comment for utility/miscellaneous/emergency purposes:
  //return -181269 + 289373.7*x - 180781.1*pow(x, 2) + 55427.19*pow(x, 3) - 8361.297*pow(x, 4) + 497.7512*pow(x, 5);
  return 2*pow(10.0, -5)*pow(e, 5.0218*x);
  //y = 2E-05e5.0218x

}

//Send the voltage value to the serial monitor
void printVoltage(double Vout)
{
  Serial.print("Voltage: ");
  Serial.println(Vout * 5.0000000000000 / 1023.0000000000, 5);
}
