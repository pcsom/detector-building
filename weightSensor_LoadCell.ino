//PROGRAM START
#include <ArduinoSort.h>
#include <Adafruit_NAU7802.h>

Adafruit_NAU7802 nau;

//To be modified. These are the bounds of the range of values that each LED corresponds to.
double redLedLow = 0;
double redLedHigh = 350;
double greenLedLow = 0;
double greenLedHigh = 350;
double blueLedLow = 351;
double blueLedHigh = 1000;


//Voltage divider reader
int ProbePin = 2;


//Utility variables and constants for the circuitry and live calculations.
int RED = 13;
int BLUE = 12;
int GRE = 11;

//Will contain the mass value
double T;
//A counter
unsigned long cycle = 0;

const int power_the_button2 = 1;
int buttonState2 = 0;

const int buttonPin3 = 5;
int buttonState3 = 0;
unsigned long milliStart = 0;
bool counting = false;

int32_t diffs[100];
int32_t val;
int32_t prevVal;
int32_t baseVal;

//Setup the program and ports.
void setup()
{
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GRE, OUTPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(power_the_button2, OUTPUT);
  Serial.begin(9600);

  //Set parameters within NAU7802 DIP ADC chip. This code does not perform any calibration or data analysis;
  //It only sets parameters such as excess voltage regulation, voltage resolution gain, measurement speed, and serial data and clock setup.
  Serial.println("NAU7802");
  if (! nau.begin()) {
    Serial.println("Failed to find NAU7802");
  }
  Serial.println("Found NAU7802");

  nau.setLDO(NAU7802_3V3);
  Serial.print("LDO voltage set to ");
  switch (nau.getLDO()) {
    case NAU7802_4V5:  Serial.println("4.5V"); break;
    case NAU7802_4V2:  Serial.println("4.2V"); break;
    case NAU7802_3V9:  Serial.println("3.9V"); break;
    case NAU7802_3V6:  Serial.println("3.6V"); break;
    case NAU7802_3V3:  Serial.println("3.3V"); break;
    case NAU7802_3V0:  Serial.println("3.0V"); break;
    case NAU7802_2V7:  Serial.println("2.7V"); break;
    case NAU7802_2V4:  Serial.println("2.4V"); break;
    case NAU7802_EXTERNAL:  Serial.println("External"); break;
  }

  nau.setGain(NAU7802_GAIN_128);
  Serial.print("Gain set to ");
  switch (nau.getGain()) {
    case NAU7802_GAIN_1:  Serial.println("1x"); break;
    case NAU7802_GAIN_2:  Serial.println("2x"); break;
    case NAU7802_GAIN_4:  Serial.println("4x"); break;
    case NAU7802_GAIN_8:  Serial.println("8x"); break;
    case NAU7802_GAIN_16:  Serial.println("16x"); break;
    case NAU7802_GAIN_32:  Serial.println("32x"); break;
    case NAU7802_GAIN_64:  Serial.println("64x"); break;
    case NAU7802_GAIN_128:  Serial.println("128x"); break;
  }

  nau.setRate(NAU7802_RATE_10SPS);
  Serial.print("Conversion rate set to ");
  switch (nau.getRate()) {
    case NAU7802_RATE_10SPS:  Serial.println("10 SPS"); break;
    case NAU7802_RATE_20SPS:  Serial.println("20 SPS"); break;
    case NAU7802_RATE_40SPS:  Serial.println("40 SPS"); break;
    case NAU7802_RATE_80SPS:  Serial.println("80 SPS"); break;
    case NAU7802_RATE_320SPS:  Serial.println("320 SPS"); break;
  }

  // Take 10 readings to flush out readings
  for (uint8_t i=0; i<10; i++) {
    while (! nau.available()) delay(1);
    nau.read();
  }

  while (! nau.calibrate(NAU7802_CALMOD_INTERNAL)) {
    Serial.println("Failed to load internal offset, retrying!");
    delay(1000);
  }
  Serial.println("Loaded internal offset");

  while (! nau.calibrate(NAU7802_CALMOD_OFFSET)) {
    Serial.println("Failed to load system offset, retrying!");
    delay(1000);
  }
  Serial.println("Loaded system offset");
}

//The main code that repeats indefinitely
void loop()
{
  //Read voltage given by ADC 
  while (! nau.available()) {
    delay(1);
  }
  prevVal = val;
  val = nau.read();

  //Check if button is pressed
  digitalWrite(power_the_button2, HIGH);
  buttonState3 = digitalRead(buttonPin3);

  //If blue button pressed, start the voltage read process. This entails taking a 10 second voltage measurement
  if (buttonState3 != LOW)
  {
    //Start the 20 second wait.
    Serial.println("================================STARTED RECORDING");
    counting = true;
    cycle = 0;
    //Save the base voltage when the button is pressed.
    baseVal = val;
    off_all();
    delay(500);
  }


  //If in the voltage read process, add [current voltage - base voltage] to an array.
  //The subtraction accounts for noise and uncontrollable drift in the load cell.
  if(counting)
  {
    diffs[cycle] = val - baseVal;
    cycle += 1;
  }

  //After 10 seconds of reading voltage, stop.
  if(counting && cycle == 100)
  {
    //Sort the array of voltages and take an average of the 50 greatest values.
    sortArray(diffs, 100);
    double mV = 0;
    for (int i = 0; i < 50; i++)
    {
      mV += 3.3*diffs[i]/(double)16777.216/60.0;
    }

    //Account for circuit setup causing inverted voltage return value
    mV = abs(mV);

    Serial.println(getMass(mV));
    
    //Print voltage
    Serial.print("FINAL VOLTAGE: ");
    Serial.print(mV, 6);
    Serial.println(" mV"); 

    //Calculate mass using the equation, which is found in getMass function below
    T = getMass(mV);
    Serial.print("MASS: ");
    Serial.println(T);

    //Activate correct led using the boundary values found at the top of this code. LEDs aren't mutually exclusive according to rules.
    if (T >= blueLedLow && T <= blueLedHigh)
    {
      digitalWrite(BLUE, HIGH);
    }
    if (T >= greenLedLow && T <= greenLedHigh)
    {
      digitalWrite(GRE, HIGH);
    }
    if (T >= redLedLow && T <= redLedHigh || (T <= 1000 && T >= 601))
    {
      digitalWrite(RED, HIGH);
    }

    //Reset utility variables
    counting = false;
    cycle = 0;
  }
  delay(100);
    
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
  //The line above is a previous version of the equation, kept in a comment for utility/miscellaneous/emergency purposes. Below is the true equation.
  return -6.417911 + 53.01505*x + 6.281152*pow(x, 2) - 1.09542*pow(x, 3) + 0.05946595*pow(x, 4);
}
