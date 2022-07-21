//Coded By denyz427 on 22th April 2021 more projects at https://github.com/denyz427
//Board: NodeMCU

//Install CTBot Library: Sketch -> Include Library -> Manage Libraries -> Search "CTBot" -> Install
#include "CTBot.h"

#define Trig D7 //Trigger pin
#define Echo D8 //Echo pin

//Telegram Variables
CTBot myBot;
String ssid = "WiFi_Name";
String pass = "WiFi_Password";
String Gantissid;
String Gantipass;
String token = "Telegram_Bot_Token"; //How to get Telegram token: https://core.telegram.org/bots#6-botfather
int id;
TBMessage msg; //for Storing Telegram Chat Text
int a;

//Ultrasonic Sensor Variables
bool status = false;
double JarakMax = 0.5;
double PulseBefore = 5;
double PulseAfter;

void setup() {
  myBot.setMaxConnectionRetries(2); 
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);

  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
}

void loop() {
      
  //Check new message
  if(myBot.getNewMessage(msg)) 
  { 
    id=msg.sender.id;
    msg.text.trim();
    if(msg.text.equalsIgnoreCase("On")) 
    {                            
      status = true;                   
      myBot.sendMessage(id, "Smart Doorbell Activated"); 
      PulseBefore = 5;
    }
    else if (msg.text.equalsIgnoreCase("Off")) 
    {                                 
      status = false;
      myBot.sendMessage(id, "Smart Doorbell Deactivated");
    }
    else if(msg.text.indexOf("SSID=")>-1 && msg.text.indexOf(";PW=")>5)
    {
      a=msg.text.indexOf(";PW=");
      Gantissid = msg.text;
      Gantissid.remove(a, Gantissid.length()-a);
      Gantissid.remove(0, Gantissid.indexOf("SSID=")+5);    
      Gantipass = msg.text;
      Gantipass.remove(0, a+4);
      
      //connection check
      myBot.wifiConnect(Gantissid, Gantipass);
      delay(7000);
      if(myBot.testConnection())
      {
        myBot.sendMessage(id, "Connected to "+Gantissid);
        ssid=Gantissid;
        pass=Gantipass;
      }
      else
      {
        myBot.wifiConnect(ssid, pass);
        delay(7000);
        if(myBot.testConnection())
        {
          myBot.sendMessage(id, "SSID: "+Gantissid+"\nPassword: "+Gantipass+"\n\nFailed to connect to "+Gantissid+
          "\nReconnected to "+ssid);
        }
      }
    }
    else if (msg.text.toDouble()>=0.02 && msg.text.toDouble()<=4)
    {
      JarakMax = msg.text.toDouble();
      PulseBefore = 5;
      myBot.sendMessage(id, "Current Maximum detection distance: "+ String(JarakMax) + " meter");            
    }
    else
    {
      String condition;
      if(status)
      {
        condition="active";                                  
      }
      else
      {
        condition="inactive"; 
      }
      myBot.sendMessage(id, "Hi " +msg.sender.username+" \\(^_^)/ \n1. Send on to activate"+
      "\n2. Send off to deactivate"+
      "\n3. send any number (max 4) to set maximum detection distance"+
      "\n4. send SSID=(SSID);PW=(password) to switch WiFi SSID and password"
      "\n\nGood Luck!!\n"+
      "\nMax detection distance: "+String(JarakMax)+" meter"+
      "\nStatus: "+condition);
    }
  }


  if(status)
  {
     digitalWrite(Trig, HIGH);
     delayMicroseconds(10);
     digitalWrite(Trig, LOW);
     PulseAfter = pulseIn(Echo, HIGH,100000)*0.0001715; //pulseIn to return back and forth time in microseconds

     if(PulseAfter <= JarakMax && PulseBefore > JarakMax)
     {
       myBot.sendMessage(id, "There's someone at the door!"); 
     }
     Serial.println(PulseAfter);
     PulseBefore=PulseAfter;
     delay(1000);
  } 
}
