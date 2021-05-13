#include <DHT.h>
#include <LiquidCrystal.h>
#include <DS1302.h>

//          ---Pinout definitions---

//LCD Keypad Shield pinout
#define LCD_RS A1
#define LCD_E A0
#define LCD_D4 A5
#define LCD_D5 A4
#define LCD_D6 A3
#define LCD_D7 A2
#define LCD_BTN A6
//LiquidCrystal lcd(RS, E,  D4, D5, D6, D7)
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7); // select the pins used on the LCD panel

//(RTC)Real Time Clock Module DS1302 pinout
#define RTC_CE 6
#define RTC_IO 7
#define RTC_SCLK 8
//     rtc(CE, IO, SCLK)
DS1302 rtc(RTC_CE, RTC_IO, RTC_SCLK); // select the pins used on the RTC

//(DHT)Humidity&Temperature Sensor DHT11 Module pinout
#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); // select the pins used on the DHT11

//RELAY pinout
#define RELAY1 5
#define RELAY2 4
#define RELAY3 3
#define RELAY4 2


//          ---Macro Constants & Global Variables definition/declaration---         

/*LCD Keypad Shield M.Constants & G.Variables
 *  define the values used by the LCD panel buttons
 */
int lcd_key     = 0; //Stores the btn Variable (if up, right, down e.t.c)
int adc_key_in  = 0; //Stores the keypress
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnNONE   4

/*(DHT)Humidity&Temperature Sensor DHT11 Module M.Constants & G.Variables
 * 
 */
#define DHT11_DELAY 1000 //Interval at which to read sensor (milliseconds)
//The Definition of the Main temperature range 
#define TEMPERATURE_MIN 37.5
#define TEMPERATURE_MAX 38.5
//The Definition of the Humidity range for the first 18 Days (MODE A)
#define HUMIDITY_MIN_A 48.0
#define HUMIDITY_MAX_A 53.0
//The Definition of the Humidity range for the last 3 Days (MODE B)
#define HUMIDITY_MIN_B 68.0
#define HUMIDITY_MAX_B 73.0

float tempSwap = TEMPERATURE_MAX; //temperature swap variable used for the flexible temperature range
float humiSwap_A = HUMIDITY_MIN_A; //humidity swap variable used for the flexible humidity range of (MODE A)
float humiSwap_B = HUMIDITY_MIN_B; //humidity swap variable used for the flexible humidity range of (MODE B)
unsigned long previousSensorMillis = 0; //will store last time sensor was updated
const long sensorInterval = DHT11_DELAY;//interval at which to read sensor (milliseconds)
float temperature = 0; //variable to store temperature
float humidity = 0;    //variable to store humidity

/*Global Variables
*/
//char my_time[16]; //Variable to Store time
//char my_date[16]; //Variable to Store Date
int mode = 0; //Mode selector initialization (Default: 0 == MODE A)

/*Function Declarations
*/
int read_LCD_buttons();
void read_keypress();
void days_passed(); //TODO
void check_temperature();
void read_sensor();
void print_statistics();

//          ---Initialisation---

void setup() {
   //Analog input Pin For Buttons
   pinMode(LCD_BTN, INPUT);

   //Output pins for Relays
   pinMode(RELAY1, OUTPUT);
   pinMode(RELAY2, OUTPUT);
   pinMode(RELAY3, OUTPUT);
   pinMode(RELAY4, OUTPUT);
   
   //Swich off the relays (Relays are active Low)
   digitalWrite(RELAY1, HIGH);
   digitalWrite(RELAY2, HIGH);
   digitalWrite(RELAY3, HIGH);
   digitalWrite(RELAY4, HIGH);
   
   dht.begin(); //Initializes DHT Sensor
   lcd.begin(16, 2);//Initializes LCD Screen
   
   //shows the initial mode on screen (Mode A)
   lcd.setCursor(0,1);
   lcd.print("A");
   
  //rtc.setTime(13,31,30); //used to set Time (use Once) //used to set time on RTC TODO:Create Menu to set Time
  //rtc.setDate(11,4,2021);//used to set Date (use Once) //used to set Date on RTC TODO:Create Menu to set Date

  //TODO: use to store variables on RTC module in case of power loss
  //DS1302_RAM firstDay 0 = 1;       
  //strcpy(my_time,rtc.getTimeStr());//used to copy time to a string 
}

//          ---The Loop---

void loop() {
  // put your main code here, to run repeatedly:
  read_sensor();
  check_temperature();
  check_humidity(mode);
  read_keypress();
  print_statistics();
}

//Check which button is pressed
int read_LCD_buttons(){
 adc_key_in = analogRead(LCD_BTN);      // read the value from the sensor
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons
 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 250)  return btnUP;
 if (adc_key_in < 450)  return btnDOWN;
 if (adc_key_in < 650)  return btnLEFT;

 return btnNONE;  // when all others fail, return this...
}

void read_keypress(){
 lcd.setCursor(0,1);            // move to the begining of the second line (Set the cursor to Print the curent Mode)
 lcd_key = read_LCD_buttons();  // read the buttons

 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     //Placeholder
     //lcd.print(">");
     break;
     }
   case btnLEFT:
     {
     //Placeholder
     //lcd.print("<");
     break;
     }
   case btnUP:
     {
     mode = 0;
     lcd.print("A");
     break;
     }
   case btnDOWN:
     {
     mode = 1;
     lcd.print("B");
     break;
     }
     case btnNONE:
     {
     //lcd.print("o");
     break;
     }
 }
}

void days_passed(){
  //TODO: after 18 days change to mode 2 
}

void check_temperature(){
  if(temperature < tempSwap){
    if(temperature < TEMPERATURE_MIN){
      tempSwap = TEMPERATURE_MAX;
    }
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    }else{
      if(temperature >= TEMPERATURE_MAX){
        tempSwap = TEMPERATURE_MIN;
      }
    digitalWrite(RELAY1, HIGH);
    digitalWrite(RELAY2, HIGH);
    }
}

void check_humidity(int mode){
  if(mode == 0){
    if(humidity > humiSwap_A){
      if(humidity > HUMIDITY_MAX_A){
        humiSwap_A = HUMIDITY_MIN_A;
      }
      digitalWrite(RELAY3, LOW);
    }else{
      if(humidity <= HUMIDITY_MIN_A){
        humiSwap_A = HUMIDITY_MAX_A;
      }
      digitalWrite(RELAY3, HIGH);
    }
  }else if(mode == 1){
    if(humidity > humiSwap_B){
      if(humidity > HUMIDITY_MAX_B){
        humiSwap_B = HUMIDITY_MIN_B;
      }
      digitalWrite(RELAY3, LOW);
    }else{
      if(humidity <= HUMIDITY_MIN_B){
        humiSwap_B = HUMIDITY_MAX_B;
      }
      digitalWrite(RELAY3, HIGH);
    }
  } 
}

void read_sensor(){
  // check to see if it's time to read the sensor; that is, if the difference
  // between the current time and last time you read the sensor is bigger than
  // the interval at which you want to read the sensor.
  unsigned long currentMillis = millis();

  if (currentMillis - previousSensorMillis >= sensorInterval) {
    // save the last time you read the sensor
    previousSensorMillis = currentMillis;

    //int chk = DHT.read11(DHT11_PIN);//read from DHT11 Module
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

  }
}


void print_statistics(){
  //lcd.clear(); //TODO: need to proper clear the screen (overwriting works well for now)  
  
  //First Line On Screen
  lcd.setCursor(0,0);//Set cursor to first line
  lcd.print(rtc.getTimeStr());//print Time on Screen 
  lcd.setCursor(10,0); //Set cursor to second half of first line
  //lcd.print((int)DHT.temperature);
  lcd.print(temperature);
  lcd.setCursor(14,0);
  lcd.print((char)223);
  lcd.print("C");
  
  //Second Line On Screen
  lcd.setCursor(2,1);//Set cursor to second line
  lcd.print(rtc.getDateStr());//print Date on Screen
  lcd.setCursor(13,1);
  lcd.print((int)humidity);
  lcd.print("%");
}
