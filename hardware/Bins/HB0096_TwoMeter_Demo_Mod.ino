//==============================================================================
// SA818-VHF Radio Demo from HackerBox 0096 - "Two Meter" - Modded with some various VHF frequencies. Ensure compliance and licensing before use.

//  https://hackerboxes.com/products/hackerbox-0096-two-meter
//
// Radio channels are specified in two arrays
//   (one for frequency and one for a text nickname)
// Button A cycles through channels which are received to the LINE OUT port
// Button C is Push to Talk (transmit) - Do not use this feature without
//   proper licensure and while making required call sign indentification
// 
// This code was adapted from an SA818-U demo written by IMSAI Guy:
//   https://youtu.be/73b-Q6AwoNA
//==============================================================================

#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();

#define SA818_AudioOn 17
#define SA818_AF_OUT  38  //Analog input to MCU
#define SA818_PTT     13
#define SA818_PD      27
#define SA818_HL      33
#define SA818_RXD     21
#define SA818_TXD     22
#define SA818_AF_IN 	25  //Analog output from MCU
#define Button_A      15
#define Button_B      26
#define Button_C      32

// All NOAA Weather Radio stations broadcast on one of seven frequencies in the VHF Public Service band:
// 162.400 MHz, 162.425 MHz, 162.450 MHz, 162.475 MHz, 162.500 MHz, 162.525 MHz, and 162.550 MHz.
//        https://www.weather.gov/phi/nwrfaq

// Number of channels
#define Channel_count 13 // Adjusted as per new channels

// Frequency array
double Frequency[Channel_count] = {
  146.2500, 162.4000, 162.4250, 162.4500, 162.4750, 162.5000, 162.5250, 162.5500,
  25.0000, 512.0000, 823.9875, 868.9875, 1300.0000
};

// Channel names
char *Channel_name[] = {
  "2m Test", "NOAA 1", "NOAA 2", "NOAA 3", "NOAA 4", "NOAA 5", "NOAA 6", "NOAA 7",
  "Custom 1", "Custom 2", "Custom 3", "Custom 4", "Custom 5"
};

int Channel = 0;            // current channel

void setup(void) {
  Serial2.begin(9600, SERIAL_8N1, SA818_TXD, SA818_RXD);

  pinMode(Button_A, INPUT_PULLDOWN);
  pinMode(Button_B, INPUT_PULLDOWN);
  pinMode(Button_C, INPUT_PULLDOWN);

  pinMode(SA818_AudioOn, INPUT);  //SA818 drives pin LOW to tell MCU that a signal is being received

  // SA818_AF_IN is an output from the MCU, but we set it as an INPUT when not in use
  pinMode(SA818_AF_IN, INPUT);

  //Output Power High/Low
  //  High Power: Float (MCU=input)
  //  Low Power: Drive pin LOW
  //  INPORTANT NOTE: NEVER Drive the pin HIGH (undefined state)
  pinMode(SA818_HL, OUTPUT);   
  digitalWrite(SA818_HL, LOW);   
  
  pinMode(SA818_PTT, OUTPUT);      //Push to Talk (Transmit)
  digitalWrite(SA818_PTT, HIGH);   //(0=TX, 1=RX) DO NOT Transmit Without License

  pinMode(SA818_PD, OUTPUT); 
  digitalWrite(SA818_PD, HIGH);    //Release Powerdown (1=Radio ON)

  tft.init();
  tft.setRotation(3);
  
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
  tft.setTextColor(TFT_RED); 
  tft.setCursor(0, 40, 4);
  tft.println("  SA818-VHF Demo");
  delay(3000);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.println("Radio Connect:");
  Serial2.println("AT+DMOCONNECT\r");  // connect communications with SA818
  tft.print(Serial2.readString());     // returns :0 if good
  tft.println("Radio Version:");
  Serial2.print("AT+VERSION\r\n");     // get version
  tft.print(Serial2.readString());     // returns: version number
  delay(2000);

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.println("Set Group:");           // group set
  setfreq();                           // set frequency
  tft.print(Serial2.readString());     // returns :0 if good
  
  tft.println("Set Volume: ");
  Serial2.print("AT+DMOSETVOLUME=8\r\n"); // set volume
  tft.print(Serial2.readString());        // returns :0 if good
  delay(2000);

  tft.fillScreen(TFT_BLACK);
  disp_channel();
}

void loop() {
  if (digitalRead(Button_A)){
    Channel += 1;                     // next channel
    if (Channel >= Channel_count) Channel=0;
    setfreq(); 
    tft.fillScreen(TFT_BLACK);
    disp_channel();
    delay(300);                       // debounce switch
  }

  if (digitalRead(Button_C)){
    tft.fillScreen(TFT_RED);          // RED Screen when transmitting
    disp_channel();
    digitalWrite(SA818_PTT,LOW);      // assert push to talk
    delay(300);                       // debounce switch
    while(digitalRead(Button_C));
    digitalWrite(SA818_PTT,HIGH);     // release push to talk
    tft.fillScreen(TFT_BLACK);        // BLACK Screen when not transmitting
    disp_channel();
    delay(1000);                      // wait for transmisson to end
  } 
}

void setfreq() {
  Serial2.print("AT+DMOSETGROUP=0,");
  Serial2.print(String(Frequency[Channel],4));
  Serial2.print(",");
  Serial2.print(String(Frequency[Channel],4));
  Serial2.print(",0000,1,0000\r\n");
}

void disp_channel() {
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_BLUE,TFT_BLACK);
  tft.println("SA818-VHF Demo");
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);
  tft.print("Freq: ");
  tft.println(String(Frequency[Channel],4));  
  tft.print("Name: ");
  tft.println(Channel_name[Channel]);
}
