#include <ESP8266WiFi.h>
#include <Time.h>
#include <Wire.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const String AP_NameString = "Assists"; //Wifi name
const char WiFiAPPSK[] = "wildcats";   //Password
WiFiServer server(80);

////////////////////////
// System Definitions //
////////////////////////
const int numGoals = 3;
const int totalBalls = 10;
const int ballFeedRate = 3;   //seconds (excluding goTime)
const float goTime = 0.5;     //seconds (time GO! flashed on screen)

// Arrays to hold session results
byte sessionNumber = 0;
byte target[totalBalls] = {0};
byte hit[totalBalls] = {0};
int timeTaken[totalBalls] = {0};
byte temp1;
byte temp2;
byte errorByte = 0;

// Overall results
int totalScore = 0;
int totalTimeTaken = 0;

// HTML request file to send to server
String html_string;
bool _reset = false;
bool _start = false;
bool _ready = false;

// Wire communication
const byte startCommand = 1;
const byte readyCommand = 2;
byte targetStatus[numGoals];

void setupWiFi(){
  WiFi.mode(WIFI_AP);

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

String home_(String html_string){
  // CSS styles
  html_string += "<style>p{font-size:50pt;}button{height:50px;width:200px;font-size:20pt;}</style>\r\n";

  // HTML welcome message and instructions
  html_string += "<body><p>Welcome to ASSISTS.<br>Brought to you by DSGN 384.<br><br>\r\n";
  html_string += "Instructions:<br>\r\n";
  html_string += "&nbsp;1. Set up targets 20 feet from center<br>\r\n";
  html_string += "&nbsp;2. Have teammate ready with " + String(totalBalls) + " balls<br>\r\n";
  html_string += "&nbsp;3. Press ready to check for targets!</p>\r\n";

  // Button to redirect to the ready page
  html_string += "<button type=\"button\" onclick=\"location.href = '/ready" + String(sessionNumber+1) + "';\">Ready</button>\r\n";
  html_string +=  "</body>\r\n</html>\n";
  
  return html_string;
}

String ready_html(String html_string){
  // CSS styles
  html_string += "<style>p{font-size:50pt;}button{height:50px;width:200px;font-size:20pt;}</style>\r\n";

  // Target status check
  html_string += "<body><p>Target status:<br><br>\r\n";
  for (int i=0; i<numGoals; i++){
    html_string += "Target " + String(i+1) + ": ";
    if(targetStatus[i] == 1){
      html_string += "OK!<br>\r\n";
    }
    else{
      html_string += "BAD.<br>\r\n";
    }
  }
  html_string += "<br></p>\r\n";
  
  // Button to redirect to the session page
  html_string += "<button type=\"button\" onclick=\"location.href = '/session" + String(sessionNumber+1) + "';\">Start</button>\r\n";
  html_string +=  "</body>\r\n</html>\n";
  
  return html_string;
}


String session(String html_string){
  // CSS styles
  html_string += "<style>p, button{font-size:50pt;}</style>\r\n";

  // Javascript functions
  html_string += "<script>\r\n";
  html_string += "var totalBalls=" + String(totalBalls) + ";\r\n";
  html_string += "var currentBall=1;\r\n";
  html_string += "var ballFeedRate = " + String(ballFeedRate) + ";\r\n";
  html_string += "var goTime = " + String(goTime) + ";\r\n";
  
  html_string += "function add(){\r\n";
  html_string += "  ballFeedRate -= 0.1;\r\n";
  html_string += "  if(ballFeedRate < -0.1){\r\n";
  html_string += "    document.getElementById('time').style.display = 'none';\r\n";
  html_string += "    document.getElementById('go').style.display = 'block';}\r\n";
  html_string += "  else{\r\n";
  html_string += "    document.getElementById('go').style.display = 'none';\r\n";
  html_string += "    document.getElementById('time').innerHTML = ballFeedRate.toFixed(1);} \r\n";
  html_string += "  if(ballFeedRate < -goTime){\r\n";
  html_string += "    ballFeedRate = 3; currentBall +=1;\r\n";
  html_string += "    if (currentBall > totalBalls){\r\n";
  html_string += "      setTimeout(displayResultsButton, 3000);\r\n";
  html_string += "      clearTimeout(t);}\r\n";
  html_string += "    document.getElementById('time').style.display = 'block';\r\n";
  html_string += "    document.getElementById('numBalls').innerHTML = 'Ball: ' + currentBall;}\r\n";
  html_string += "  stopwatch();\r\n";
  html_string += "}\r\n";

  html_string += "function displayResultsButton() {\r\n";
  html_string += "  document.getElementById('button').style.display ='block';\r\n";
  html_string += "}\r\n";
  
  html_string += "function stopwatch() {\r\n";
  html_string += "  var t = setTimeout(add, 100);\r\n";
  html_string += "}\r\n";
  html_string += "</script>\r\n";

  // HTML body
  html_string += "<body onload=\"stopwatch()\">\r\n";
  html_string += "<p ir=\"title\">Starting new session...</p>\r\n";
  html_string += "<p id=\"numBalls\">Ball: 1</p>\r\n";
  html_string += "<p id=\"time\" style=\"font-size: 200pt; text-align: center;\">0</p>\r\n";
  html_string += "<p id=\"go\" style=\"font-size: 200pt; background-color: rgb(0, 255, 0); text-align: center; display: none;\">GO!</p>\r\n";

  // Button redirects to results page
  html_string += "<button id=\"button\" type=\"button\" onclick=\"location.href = '/results';\" style=\"height:100px;width:500px; margin: 0 auto; display: none;\">See results</button>\r\n";
  html_string += "</body></html>\r\n";
  
  return html_string;
}

String results(String html_string){
  // CSS styles
  html_string += "<style>\r\n";
  html_string += "div{font-size:50pt;}\r\n";
  html_string += "table,th,td{border: 1px solid black; font-size:30pt;}\r\n";
  html_string += "button{height:50px;width:200px;font-size:30pt;}\r\n";
  html_string += "</style><script>\r\n";

  // Javascript functions
  html_string += "function startTime() {\r\n";
  html_string += "  var today = new Date();\r\n";
  html_string += "  var dateString = today.toLocaleDateString();\r\n";
  html_string += "  var timeString = today.toLocaleTimeString();\r\n";
  html_string += "  document.getElementById('date').innerHTML = \"Date: \" + dateString;\r\n";
  html_string += "  document.getElementById('time').innerHTML = \"Time: \" + timeString;}\r\n";
  html_string += "</script>\r\n";

  // Overall statistics
  html_string += "<body onload=\"startTime()\">\r\n";
  html_string += "<div id=\"session\">Session: " + String(sessionNumber) + "</div>\r\n";
  html_string += "<div id=\"date\"></div>\r\n";
  html_string += "<div id=\"time\"></div>\r\n";
  
  html_string += "<div id=\"score\">Score: ";
  for(int i = 0; i < totalBalls; i++){
    if(hit[i] == 1){
      totalScore += 1; 
    }
  }
  html_string += String(totalScore) + "/" + String(totalBalls) + "</div>\r\n";
  
  html_string += "<div id=\"timeTaken\">Time Taken: ";
  for(int i = 0; i < totalBalls; i++){
    totalTimeTaken += timeTaken[i]; 
  }
  html_string += String((float)totalTimeTaken/1000.0) + "s</div>\r\n";

  // Draw table for statistics on individual targets
  html_string += "<table>\r\n";
  html_string += "<tr><th>Ball #</th><th>Target</th><th>Hit?</th><th>Time</th></tr>\r\n";
  for(int i = 1; i <= totalBalls; i++){
    html_string += "<tr><td>" + String(i) + "</td>";                    //Ball number
    html_string += "<td>" + String(target[i-1]) + "</td>";              //Target
    if (hit[i-1] == 1){                                                 //HIT
      html_string += "<td>HIT</td>";
    }
    else{                                                               //MISS
      html_string += "<td>MISS</td>";
    }
    html_string += "<td>" + String((float)timeTaken[i-1]/1000.0) + "s</td></tr>\r\n";  //Time taken
  }
  html_string += "</table><br>\r\n";

  // Display error messages is any
  if(errorByte > 0){
    html_string += "<div>WARNING!\r\n";
    if(errorByte == 100){
      html_string += "Fatal error: More than one target offline. Please contact administrator.";
    }
    else{
      html_string += "Error with target " + String(errorByte) + ": No communication.";
    }
    html_string += "</div>\r\n";
  }

  // Button to redirect to home page
  html_string += "<button type=\"button\" onclick=\"location.href = '/';\">End</button>\r\n";
  html_string += "</body></html>\r\n";
  
  return html_string;
}

void requestResults(){
  // because I2C buffer only hold 32 bytes, we request the data in two parts
  // request first half of data
  Wire.requestFrom(7, 2*totalBalls + 2);
  sessionNumber = Wire.read();
  errorByte = Wire.read();
  
  for(int k=0; k<totalBalls; k++){
    target[k] = Wire.read();
  }
  for(int k=0; k<totalBalls; k++){
    hit[k] = Wire.read();
  }
  while(Wire.available()){
    byte dump = Wire.read();
  }

  // request second half of data
  Wire.requestFrom(7, 2*totalBalls);
  
  for(int k=0; k<totalBalls; k++){
    temp1 = Wire.read();
    temp2 = Wire.read();
    timeTaken[k] = temp1;
    timeTaken[k] = (timeTaken[k] << 8)|temp2; 
  }
  while(Wire.available()){
    byte dump = Wire.read();
  }
}

void resetStats(){
  totalScore = 0;
  totalTimeTaken = 0;
  _reset = true;
}








// ----------Setup-------------//

void setup(){
  Serial.begin(115200);
  Wire.begin();
  setupWiFi();
  server.begin();
}


// ----------Loop-------------//
void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  client.flush();

  // reset html_string
  html_string  = "HTTP/1.1 200 OK\r\n";
  html_string += "Content-Type: text/html\r\n\r\n";
  html_string += "<!DOCTYPE HTML>\r\n<html>\r\n";

  // results page
  if (req.indexOf("/results") > 0) {
    requestResults();
    html_string = results(html_string);
    _start = false;
  }

  // session page
  else if (req.indexOf("/session") > 0) {
    if(!_start){
      Wire.beginTransmission(7);
      Wire.write(startCommand);
      Wire.endTransmission();
      _start = true;
      _reset = false;
    }
    html_string = session(html_string);
  }

  // ready page
  else if (req.indexOf("/ready") > 0) {
    if(!_ready){
      Wire.beginTransmission(7);
      Wire.write(readyCommand);
      Wire.endTransmission();
      delay(50);

      while(Wire.available()){
        byte dump = Wire.read();
      }

      delay(3500*(numGoals + 1));
      Wire.requestFrom(7, numGoals);
      
      
      for(int i=0; i<numGoals; i++){
        targetStatus[i] = Wire.read();
      }
      
      while(Wire.available()){
        byte dump = Wire.read();
      }
      
      _ready = true;
    }
    
    html_string = ready_html(html_string);
  }

  // home page
  else {
    if(!_reset){
      resetStats();
      _ready = false;
    }
    html_string = home_(html_string);
  }
  
  // Send the response to the client
  client.print(html_string);
  delay(1);
  Serial.println("Client disonnected");
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}



