
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
 
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
#define LEFT 2 //Left button
#define RIGHT 3  //Right button
#define TOP 4  //Top button
#define DOWN 5  //Down button


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

byte Time[8] = {0,12,9,1,2,11,8,21};  //Byte array to store Time {second, minute, hour, meridiem, day, date, month, year}
byte stopWatch[4] = {0,0,0,0}
byte control = 0; //control variable, used in setTime function to reset time
byte mode = 0; //mode variable for switching between modes
int i = 0; boolean leap = false; //iteration variable and boolean variable to check for leap year
char buffer[10]; //String buffer to store string padded with '0', incase of single digit time

void setup() 
{
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC)) //Do not delete this if block, for some reaason, OLED aint starting up without it
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();  // Clear the buffer
  display.setTextColor(WHITE);
  display.setCursor(1,16);
  display.setTextSize(2);
  display.print("chupakabra");
  display.display();
  delay(500);

  //Initializing buttons
  pinMode(LEFT, INPUT); 
  pinMode(RIGHT, INPUT);
  pinMode(TOP, INPUT);
  pinMode(DOWN, INPUT);

}

void screen()  //fuction used to display the time
{
  display.clearDisplay();
  display.setCursor(1,16);
  display.setTextSize(3);
  sprintf(buffer, "%02d:%02d", Time[2], Time[1]); //displays Hour n Minute
  //the %02d makes sure time is a 2 digit number, if not, pads 0 to the 10s place
  //String padding only works when done ;ike this, means "sprintf", cant be used directly on display.print()
  display.print(buffer);

  //display second
  display.setTextSize(2);
  display.setCursor(92,16);
  sprintf(buffer, "%02d", Time[0]);  
  display.print(buffer);

  //displays AM/PM
  display.setTextSize(1);
  if(Time[3] == 0)
    display.print("AM");
  if(Time[3] == 1)
    display.print("PM");

  //displays Day
  display.setCursor(1,1);  
  switch(Time[4])
  {
    case 0:
    display.print("Monday");
    break;

    case 1:
    display.print("Tuesday");
    break;
    
    case 2:
    display.print("Wednesday");
    break;

    case 3:
    display.print("Thursday");
    break;

    case 4:
    display.print("Friday");
    break;

    case 5:
    display.print("Saturday");
    break;

    case 6:
    display.print("Sunday");
    break;
  }

  //display date
  display.setCursor(65,1);
  sprintf(buffer, "%02d|%02d|20%02d", Time[5],Time[6],Time[7]);
  display.print(buffer);
  display.display();
}

void updateSecond() //function to increase time by 1 second
{
  Time[0]++;
 if(Time[0] > 59)
 {
  Time[0] = 0;
 }
}
void updateMinute()  //function to increase time by 1 minute
{
  Time[1]++;
 if(Time[1] > 59)
 {
  Time[1] = 0;
 }
}
void updateHour()  //function to increase time by 1 Hour
{
  Time[2]++;
 if(Time[2] > 12)
 {
  Time[2] = 1;
 }
}
void updateMeridiem()  //function to switch Meridiem
{
  Time[3]++;
  if(Time[3] > 1)
  {
   Time[3] = 0;
  }
}
void updateDay()  //function to increase Day of week by 1 Day
{
  Time[4]++;
  if(Time[4] > 6)
  {
   Time[4] = 0;
  }
}
void updateDate()  //function to increase date by 1 Day
{
  Time[5]++;
  //below statement check for the nummber of days for months with 31,30 and 28 days, including leap year
  if(((Time[5] > 31) && (Time[6] == 0 || Time[6] == 2 || Time[6] == 4 || Time[6] == 6 || Time[6] == 7 || Time[6] == 9 || Time[6] == 11)) || ((Time[5] > 30) && (Time[6] == 3 || Time[6] == 5 || Time[6] == 8 || Time[6] == 10)) || ((Time[5] == 29) && (Time[6] == 1)) || ((Time[5] > 29) && (Time[6] == 1) && leap))
    Time[5] = 1;
}
void updateMonth()  //function to increase date by 1 month
{
  Time[6]++;
  if(Time[6] > 12)
    Time[6] = 1;
}
void updateYear()  //function to increase date by 1 year
{
  Time[7]++;
  leap = Time[7]%4 == 0 ? true : false;
  if(Time[7] > 99)
    Time[7] = 0;
  
}

void lapse()  //loop to update time after each second
{
  updateSecond();
  if(Time[0] == 0)
  {
    updateMinute();
    if(Time[1] == 0)
    {
      updateHour();
      if(Time[2] == 1)
      {
        updateMeridiem();
        if(Time[3] == 0)
        {
          updateDay();
          updateDate();
          if(Time[5] == 1)
          {
            updateMonth();
            if(Time[6] == 1)
            {
              updateYear();
            }
          }
        }
      }
    }
  }
}

 void setTime()  //function for user to set time
 //Left button: to change the component of time being reset
 //Right button: to enter/exit reset mode
 //Top button: to increase the selected component of time by 1
 {
    Time[0] = 0;  //the second is reset to zero while resetting the watch
    display.setCursor(10,50);
    display.setTextSize(1);
    display.print("Entering Reset mode");  //displays mode change
    display.display();
    delay(1000);  //entry n exit to ths function is through the same button, this delay function stop instant exit from the function
    control = 0;  // has values from 0 to 7, each assigned to one component of the time
    for(;;)       //infinite loop, commmand does not exit this loop unless the user wants to
    {
     if(digitalRead(LEFT) == LOW)  //when the left button is pressed the command moves to next time component
     {
      control++;
      delay(500);  //delay gives the user to take his finger off the button
     }
     
     switch(control)
     {
      case 0:
        display.setCursor(1,16);
        display.setTextSize(3);
        display.print((char) 218);  //enters a full blocks
        display.print((char) 218);  //entes a second block
        display.display();
        delay(150);  //the two delay functions and screen function, create a blinking effect; so tht the user know which component of time is in reset mode
        screen();
        delay(150);

        if(digitalRead(TOP) == LOW) //if the user enters the change button while on the rset mode for tht component of time, the corresponding update function gets triggered
          updateHour();
      break;
      
      case 1:
        display.setCursor(56,16);
        display.setTextSize(3);
        display.print((char) 218);
        display.print((char) 218);
        display.display();
        delay(150);
        screen();
        delay(150);

        if(digitalRead(TOP) == LOW)
          updateMinute();
      break;

    //uncommment the below block and add case_ID to activate seconds reset 
    //case <case_ID>:
    //  display.setCursor(1,16);
    //  display.setTextSize(3);
    //  display.print("     ");
    //  display.setTextSize(2);
    //  display.print((char) 218);
    //  display.print((char) 218);
    //  display.display();
    //  delay(150);
    //  screen();
    //  delay(150);
    //
    //  if(digitalRead(TOP) == LOW)
    //    updateSecond();
    //  break;

      case 2:
        display.setCursor(116,16);
        display.setTextSize(1);
        display.print((char) 218);
        display.print((char) 218);
        display.display();
        delay(150);
        screen();
        delay(150);

        if(digitalRead(TOP) == LOW)
          updateMeridiem();
      break;

      case 3:
        display.setCursor(1,1);
        display.setTextSize(1);
        for(i=0; i <10; i++)
        display.print((char) 218);
        display.display();
        delay(150);
        screen();
        delay(150);

        if(digitalRead(TOP) == LOW)
          updateDay();
      break;
      
      case 4:
        display.setCursor(65,1);
        display.setTextSize(1);
        for(i=0; i <2; i++)
        display.print((char) 218);
        display.display();
        delay(150);
        screen();
        delay(150);

        if(digitalRead(TOP) == LOW)
          updateDate();
      break;

      case 5:
        display.setCursor(65,1);
        display.setTextSize(1);
        display.print("   ");
        for(i=0; i <2; i++)
        display.print((char) 218);
        display.display();
        delay(150);
        screen();
        delay(150);

        if(digitalRead(TOP) == LOW)
          updateMonth();
      break;

      case 6:
        display.setCursor(65,1);
        display.setTextSize(1);
        display.print("        ");
        for(i=0; i <2; i++)
        display.print((char) 218);
        display.display();
        delay(150);
        screen();
        delay(150);

        if(digitalRead(TOP) == LOW)
          updateYear();
      break;
     }
   if(control>6)
    control= 0;
   if(digitalRead(RIGHT) == LOW)
    stopWatch();
  }
}

void stopWatch()
{
  
}

void loop() 
{
 delay(1000); //lapse 1 second
 lapse();
 screen();
 if(digitalRead(RIGHT) == LOW)
  setTime();
}
 
