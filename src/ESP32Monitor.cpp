#include "ESP32Monitor.h"
#include <AsyncElegantOTA.h>

long lastMonitor = 0;
long monitorPeroid = 500;
AsyncWebServer server(80);
AsyncEventSource events("/events");     //- Create an Event Source on /events
//JSONVar readings;                       //- Json Variable to Hold Sensor Readings

String mName[64];
String dName[64];
String fName[64];
bool mData[64];
uint16_t dData[64];
float fData[64];
String diagData[64];

bool isSPIFFS;
uint8_t boardVersion;

String cmd;
char car;

//- Get Sensor Readings and return JSON object
String getValueReadings(){
  JSONVar readings;
  for(uint8_t i = 0; i < 64; i++){    //- Memory M
    //readings["m"+String(i)] = mData[i];     //random(2);
    readings[mName[i]] = mData[i];
  }
  for(uint8_t i = 0; i < 64; i++){    //- Memory D
    //readings["d"+String(i)] = dData[i];      //random(500);
    readings[dName[i]] = dData[i];
  }
  for(uint8_t i = 0; i < 64; i++){    //- Memory F
    /* float fv;
    fv = random(1000);
    fv /= 100; */
    //readings["f"+String(i)] = fData[i];    //fv;
    readings[fName[i]] = fData[i];
  }
  for(uint8_t i = 0 ;i < 8 ; i++){    //- Input
    readings["I"+String(i)] = readInput(i);
  }
  for(uint8_t i = 0 ;i < 8 ; i++){    //- Output
    readings["O"+String(i)] = getOutput(i);
  }

  uint8_t diagCount =0;
  while((diagData[diagCount] != "") && diagCount < 64){
    readings["Diagnostic "+String(diagCount)] = diagData[diagCount];
    diagCount += 1;
  }

  String jsonString = JSON.stringify(readings);
  //Serial.println(jsonString);
  return jsonString;
}

//- Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("--- > An error has occurred while mounting SPIFFS");
    isSPIFFS = false;
  }
  else{
    Serial.println("--- > SPIFFS mounted successfully");
    isSPIFFS = true;
  }
}

//- Set up web server 
void setupWebserver(uint8_t _boardversion){
  if (_boardversion == 0){
    server.on("/monitor", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/monitor10rs.html", "text/html");
    });
  }else if(_boardversion == 1){
    server.on("/monitor", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/monitor10rx.html", "text/html");
    });
  }else if(_boardversion == 2){
    server.on("/monitor", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/monitor32u.html", "text/html");
    });
  }else if(_boardversion == 3){
    server.on("/monitor", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/monitor10.html", "text/html");
    });
  }
  server.serveStatic("/", SPIFFS, "/");
  // Request for the latest sensor readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getValueReadings();
    request->send(200, "application/json", json);
    json = String();
  });
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      String msg = " Client reconnected! - message ID is: " + String(client->lastId());
      addDiagnostic(msg);
      String amsg = "--- >" + msg;
      Serial.println(amsg);
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  AsyncElegantOTA.begin(&server);

  server.begin();
  Serial.println("--- > ESP32 Monitor server started");
}

//- Update memory M
void defMName(bool setDiag){
  Serial.println("--- > Begin set MName");
  for(uint8_t i=0;i<64;i++){
    mName[i] = "m"+String(i);
  }
  Serial.println("--- > OK");
  if(setDiag){
    addDiagnostic("M Memory - defined");
  } 
  delay(100);
}
void setMName(uint8_t sAdd,String sn,bool setDiag){
  mName[sAdd] = sn;
  if(setDiag){
    addDiagnostic(" Set M Name" + String(sAdd) + " : " + String(sn));
  }
}
void updateMValue(uint8_t add, bool val,bool setDiag){
  mData[add] = val;
  if(setDiag){
    addDiagnostic(" Set M Value" + String(add) + " : " + String(val));
  }
}
void clearMValue(bool setDiag){
  for(uint8_t i=0;i<64;i++){
    mData[i] = false;
  }
  if(setDiag){
    addDiagnostic(" M Memory - has been cleared");
  }
}

//- Update memory D
void defDName(bool setDiag){
  Serial.println("--- > Begin set DName");
  for(uint8_t i=0;i<64;i++){
    dName[i] = "d"+String(i);
  }
  Serial.println("--- > OK");
  if(setDiag){
    addDiagnostic(" D Memory - defined");
  }
  delay(100);
}
void setDName(uint8_t sAdd,String sn,bool setDiag){
  dName[sAdd] = sn; 
  if(setDiag){
    addDiagnostic(" Set D Name" + String(sAdd) + " : " + String(sn));
  }
}
void updateDValue(uint8_t add, uint16_t val,bool setDiag){
  dData[add] = val;
  if(setDiag){
    addDiagnostic(" Set D Value" + String(add) + " : " + String(val));
  }
}
void clearDValue(bool setDiag){
  for(uint8_t i=0;i<64;i++){
    dData[i] = 0;
  }
  if(setDiag){
    addDiagnostic(" D Memory - has been cleared");
  }
}

//- Update memory F
void defFName(bool setDiag){
  Serial.println("--- > Begin set FName");
  for(uint8_t i=0;i<64;i++){
    fName[i] = "f"+String(i);
  }
  Serial.println("--- > OK");
  if(setDiag){
    addDiagnostic(" F Memory - defined");
  }
  delay(100);
}
void setFName(uint8_t sAdd,String sn,bool setDiag){
  fName[sAdd] = sn;
  if(setDiag){
    addDiagnostic(" Set F Name" + String(sAdd) + " : " + String(sn));
  }
}
void updateFValue(uint8_t add, float val,bool setDiag){
  fData[add] = val;
  if(setDiag){
    addDiagnostic(" Set F Value" + String(add) + " : " + String(val));
  }
}
void clearFValue(bool setDiag){
  for(uint8_t i=0;i<64;i++){
    fData[i] = 0.0;
  }
  if(setDiag){
    addDiagnostic(" F Memory - has been cleared");
  }
}

void promptMsg(){
  Serial.print("");
}

void addDiagnostic(String _diag){
  uint8_t msgNum = 0;
  while((diagData[msgNum].length() > 0)&&(msgNum < 64)){    //- Find empty message pos
    msgNum += 1;
  }
  Serial.print("--- > DiagMsg: ");
  if(msgNum < 64){      //- Assign diagnostic data 
    diagData[msgNum] = _diag;
    Serial.println(diagData[msgNum]);
    promptMsg();
  }else{                //- Clear diagnostic data
    Serial.println("--- > Clear DiagMsg");
    promptMsg();
    for(uint8_t i=0;i<64;i++){
      diagData[i] = "";
    }
  }
}

//- Memory definitions
void defMemory(bool setDiag){
  if(setDiag){
    addDiagnostic("Begin define Memory");
  }
  defMName();
  defDName();
  defFName();
}

//- Init Web monitor
void initESP32Monitor(uint8_t _version){
  Serial.println("---------------------------------------------------------");
  Serial.println("                   Begin ESP32 Monitor");
  Serial.println("---------------------------------------------------------");
  Serial.println("--- > Begin init SPI FAT FILE SYSTEM");
  initSPIFFS();  //- Init SPIFFS for web storage
  if(isSPIFFS){
    Serial.println("--- > OK");
  }else{
    Serial.println("--- > Failed");
  }
  Serial.println();
  delay(500);
  Serial.println("--- > Begin init Device IO");
  if(_version == 0){
    initIO();
    Serial.println("--- > Use ESP32 Control v1.0RS");
  }else if(_version == 1){
    initIO(1);
    Serial.println("--- > Use ESP32 Control v1.0RX");
  }else if(_version == 2){
    initIO(2);
    Serial.println("--- > Use MINIPLC RLX 32U");
  }
  else if(_version == 3){
    initIO(3);
    Serial.println("--- > Use Other lagacy board");
  }

  Serial.println();
  delay(500);
  Serial.println("--- > Begin init Web monitor server");
  setupWebserver(_version);
  Serial.println("--- > OK");
  Serial.println();
  delay(500);
  Serial.println("--- > Begin Define memory");
  defMemory(true);
  Serial.println("--- > OK");
  Serial.println();
  delay(500);
}

void processCMD(){
  int index;
  if((index = cmd.indexOf("LOG")) > -1) {
    String msg = cmd.substring(index + 4, cmd.length() -2);
    Serial.println("LOG >>" + msg + "<<");
  } else if((index = cmd.indexOf("mname")) > -1) {    //- Mname
    uint8_t pIndex = cmd.indexOf(" : ");
    String msg = cmd.substring(index + 6, index + 8);
    String msgVal = cmd.substring(index + 11, cmd.length() -2);
    String cmdMSG = "--- > MName" + msg + " to : " + msgVal;
    Serial.println(cmdMSG);
    setMName(msg.toInt(),msgVal,true);
  } else if((index = cmd.indexOf("dname")) > -1) {    //- Dname
    uint8_t pIndex = cmd.indexOf(" : ");
    String msg = cmd.substring(index + 6, index + 8);
    String msgVal = cmd.substring(index + 11, cmd.length() -2);
    String cmdMSG = "--- > DName" + msg + " to : " + msgVal;
    Serial.println(cmdMSG);
    setDName(msg.toInt(),msgVal,true);
  } else if((index = cmd.indexOf("fname")) > -1) {    //- Fname
    uint8_t pIndex = cmd.indexOf(" : ");
    String msg = cmd.substring(index + 6, index + 8);
    String msgVal = cmd.substring(index + 11, cmd.length() -2);
    String cmdMSG = "--- > FName" + msg + " to : " + msgVal;
    Serial.println(cmdMSG);
    setFName(msg.toInt(),msgVal,true);
    
  } else if((index = cmd.indexOf("reboot")) > -1) {
    Serial.println("--- > Reboot ...");
    ESP.restart();
  }
}

void fetchCMD(){
  if (Serial.available()) {
    car = Serial.read();
    cmd += String(car);
    Serial.print(car);
  }
  if (cmd.indexOf('\n') > -1) {
    processCMD();
    cmd = "";
    promptMsg();
  }
}

//- Main Web monitor task
void webMonTask(){
  if ((millis() - lastMonitor) > monitorPeroid) {   //-For Status monitor
    //- Send Events to the client with the Sensor Readings Every 10 seconds
    events.send("ping",NULL,millis());
    events.send(getValueReadings().c_str(),"new_readings" ,millis());
    lastMonitor = millis();
  }
  toggleLed();    //- Toggle LED Status
  fetchCMD();     //- Fecth commad line
}