//PROGRAM START
//NOTE: the temperature calculation from the voltage is the same as the equation given, but is distributed into multiple steps and redefined variables. 
//For example, the coefficients of the Steinhart-Hart equation are stored as separate variables.


int ThermistorPin = 0;

//Calibration code for producing new coefficients during the calibration phase of the competition.
double lower_resist=29498.07;
double mid_resist=12052.51;
double higher_resist=2521.42;
double lower_temp=2;
double mid_temp=20.0;
double higher_temp=62;

double J=log(lower_resist);
double K=log(mid_resist);
double L=log(higher_resist);
double X=1.0/(lower_temp+273.15);
double Y=1.0/(mid_temp+273.15);
double Z=1.0/(higher_temp+273.15);

double I=(Y-X)/(K-J);
double O=(Z-X)/(L-J);

double C=((O-I)/(L-K))/(J+K+L);
double B=I-C*(J*J+J*K+K*K);
double A=X-J*(B+C*J*J);

//Utility variables and constants for the circuitry and live calculations.
int RED = 13;
int BLUE = 12;
int GRE = 11;

double T;

double Vout=0;
double totalVout=0;
int cycle=1;
int refresh = 1000;

const int buttonPin2=3;
const int power_the_button2=1;
int buttonState2=0;

bool combine = false;

void setup()
{
    pinMode(RED, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(GRE, OUTPUT);
    pinMode(buttonPin2, INPUT);
    pinMode(power_the_button2, OUTPUT);
    Serial.begin(9600);
}

void loop() 
{
    
    /*
    Initial Coefficients
    A=0.00112448806063313;
    B=0.00023485824059245;
    C=0.00000008505219328914266;*/
    
    /*
    1st Modification
    A=0.001361497751201684;
    B=0.0001843656859543257;
    C=0.0000003745534071063977;*/
    
    /*Second Modification
    A=0.0005303297498075823;
    B=0.00033120482187551733;*/
    
    /*Third Modification
    A=0.000454292473560131;
    B=0.00034410714722585273;
    C=-0.00000034099798605503665;*/
    
    /*Fourth modification is active
    A=0.001362685921;
    B=0.0001790878631;
    C=0.0000004284658997;  */ 
    
    
    // read the input on analog pin 0:
    int sensorValue = analogRead(ThermistorPin);
    
    
    //Button code
    //Read button's state; if pressed, then reverse the value of a designated boolean.
    digitalWrite(power_the_button2, HIGH);
    buttonState2=digitalRead(buttonPin2);    
    if(buttonState2!=LOW)
    {
        combine = !combine;
        delay(1000);
    }    

    //****CALCULATIONS****

    //Conditional statement to complement the previous button press detection
    if(combine) 
    {      
        //Average Voltage over time if the combine boolean is true
        Serial.println("Using averaged voltage");
        totalVout += double(analogRead(ThermistorPin));
        Vout=double(totalVout)/cycle++;
    }
    else
    {
        //Use normal, real-time voltage if the combine boolean is false
        Serial.println("Using real time voltage");
        totalVout=0;
        cycle=1;
        Vout=double(analogRead(ThermistorPin));
    }

    //Resistance
    double R = 10000 * (1023.0 / (float)Vout - 1.0);
                                                      //double R=(10000.0*Vout)/(5.0-Vout);
    char mes[] = {'R', ':', ' '};
    print_to_monitor(mes, R);
    printVoltage(Vout);
    
    //Temperature
    double logR=log(double(R));
    T=1.0/(A+B*logR+C*logR*logR*logR);
    T-=273.15;
    T=T*9.0/5 + 32;
    printTemps(1);
    T=(T-32.000000)*5.0000/9.00000;
    

    //LED FOR EACH TEMPERATURE
    
    if (T >= 0 && T <= 25.0)
    {
        digitalWrite(BLUE, HIGH);
    }
    else if (T > 25.0 && T <= 50.0)
    {
        digitalWrite(GRE, HIGH);
    }
    else if (T > 50.0 && T <= 75.0)
    {
        digitalWrite(RED, HIGH);
    }  
    delay(refresh);
    off_all();    
}

//***FUNCTIONS***

//Turn off all LEDs
void off_all()
{
    digitalWrite(RED, LOW);
    digitalWrite(GRE, LOW);
    digitalWrite(BLUE, LOW);
}

//Send the voltage value to the serial monitor
void printVoltage(double Vout)
{
    Serial.print("Voltage: ");
    Serial.print(Vout*5.0000000000000/1023.0000000000, 5);
}

//Send the temperature value(s) to the serial monitor
void printTemps(bool f_too)
{
    Serial.print(" || Temperature: ");
    if(f_too)
    {
        Serial.print(T, 4);
        Serial.print(" F; ");
    }
    float Tc=(T-32)*5/9;
    Serial.print(Tc, 4);
    Serial.println(" C\n");
}

//Send a argument-specified message to the serial monitor
void print_to_monitor(char messagey[], double outnum)
{
    Serial.print(messagey);
    Serial.println(outnum);
}
