#include <CPPM.h>
#include <ArduinoJson.h>

#define MANUAL_RC_ENABLED // Turns on Module
#define MANUAL_RC_SWITCH_STATE_UP_LIMIT 1000
#define MANUAL_RC_SWITCH_STATE_DOWN_LIMIT 2014
//#ifdef MANUAL_RC_ENABLED


struct RCControlModel
{
  int switch1Raw; 
  int switch1;
  int steering;
  int throttle;
  int brake;
};

RCControlModel RCController1;


void PPMUpdater()
{
  if(CPPM.synchronized()){
  RCController1.switch1Raw = CPPM.read_us(CPPM_GEAR);
  //RCController1.steering=map(CPPM.read_us(CPPM_AILE), 991, 2015, 0, 255);
  RCController1.steering=map(CPPM.read_us(CPPM_RUDD), 991, 2015, 0, 255);
  RCController1.throttle=map(CPPM.read_us(CPPM_THRO), 1006, 2014, 0, 255);
  RCController1.brake=map(CPPM.read_us(CPPM_ELEV), 991, 2015, 0, 255);
  

  if (RCController1.switch1Raw <= MANUAL_RC_SWITCH_STATE_UP_LIMIT)
  {
    RCController1.switch1 = 2;
  }
  else if (RCController1.switch1Raw > MANUAL_RC_SWITCH_STATE_UP_LIMIT && RCController1.switch1Raw < MANUAL_RC_SWITCH_STATE_DOWN_LIMIT)
  {
    RCController1.switch1 = 1;
  }
  else
  {
    RCController1.switch1 = 0;
  }
  }
 
}


//#endif // MANUAL_RC_ENABLED


StaticJsonDocument  <256> doc;
char json[] = "{\"Throttle\":\"100\",\"Steering\":125,\"Mode\":0,\"Stop\":0}";


void setup(){
  Serial.begin(9600);
  CPPM.begin();
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  auto error = deserializeJson(doc, json);
  if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
  }
  else{
    Serial.println("JSON deserialize SUCCESSFUL");
  }
}

void loop(){

 
  
  PPMUpdater();
 
  doc["Throttle"] = RCController1.throttle;  
  doc["Steering"] = RCController1.steering;
  doc["Mode"] = RCController1.switch1;
  doc["Braking"]=RCController1.brake;
  
  if(Serial.available()){ 
    serializeJson(doc, Serial);  
    Serial.println(""); 
   

  }
  Serial.flush();
  //delay(1); //10ms
  
  

   
}
