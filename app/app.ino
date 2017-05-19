#include <ESP8266WiFi.h>
#include <Time.h>
#include <Wire.h>

//////////////////////
// WiFi Definitions //
//////////////////////
String AP_NameString = "Soccer Controls"; //Wifi name
const char WiFiAPPSK[] = "soccerskilz";   //Password
WiFiServer server(80);

////////////////////////
// System Definitions //
////////////////////////
int sessionNumber;
const int totalBalls = 2;
const int intervalTime = 3;   //seconds
const float goTime = 0.5;     //seconds

// Arrays to hold session results
int target[totalBalls] = {0};
int hit[totalBalls] = {0};
int timeTaken[totalBalls] = {0};

// Overall results
int totalScore = 0;
int totalTimeTaken = 0;

// HTML request file to send to server
String html_string;
bool _reset = false;

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
  html_string += "&nbsp;1. Set up goals 20 feet from center<br>\r\n";
  html_string += "&nbsp;2. Have teammate ready with " + String(totalBalls) + " balls<br>\r\n";
  html_string += "&nbsp;3. xxx<br><br>Press start when ready!</p>\r\n";

  // Button to redirect to the session page
  html_string += "<button type=\"button\" onclick=\"location.href = '/session';\">Start</button>\r\n";
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
  html_string += "var intervalTime = " + String(intervalTime) + ";\r\n";
  html_string += "var goTime = " + String(goTime) + ";\r\n";
  
  html_string += "function add(){\r\n";
  html_string += "  intervalTime -= 0.1;\r\n";
  html_string += "  if(intervalTime < -0.1){\r\n";
  html_string += "    document.getElementById('time').style.display = 'none';\r\n";
  html_string += "    document.getElementById('go').style.display = 'block';}\r\n";
  html_string += "  else{\r\n";
  html_string += "    document.getElementById('go').style.display = 'none';\r\n";
  html_string += "    document.getElementById('time').innerHTML = intervalTime.toFixed(1);} \r\n";
  html_string += "  if(intervalTime < -goTime){\r\n";
  html_string += "    intervalTime = 3; currentBall +=1;\r\n";
  html_string += "    if (currentBall > totalBalls){\r\n";
  html_string += "      document.getElementById('button').style.display ='block';\r\n";
  html_string += "      clearTimeout(t);}\r\n";
  html_string += "    document.getElementById('time').style.display = 'block';\r\n";
  html_string += "    document.getElementById('numBalls').innerHTML = 'Ball: ' + currentBall;}\r\n";
  html_string += "  stopwatch();\r\n";
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
  html_string += String(totalScore) + "</div>\r\n";
  html_string += "<div id=\"timeTaken\">Time Taken: ";
  for(int i = 0; i < totalBalls; i++){
    totalTimeTaken += timeTaken[i]; 
  }
  html_string += String(totalTimeTaken) + "</div>\r\n";

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
    html_string += "<td>" + String(timeTaken[i-1]) + "</td></tr>\r\n";  //Time taken
  }
  html_string += "</table><br>\r\n";

  // Button to redirect to home page
  html_string += "<button type=\"button\" onclick=\"location.href = '/';\">End</button>\r\n";
  html_string += "</body></html>\r\n";
  
  return html_string;
}

void requestResults(){
  Wire.requestFrom(7, 3*totalBalls);
  for(int k=0; k<totalBalls; k++){
    target[k] = Wire.read();
  }
  for(int k=0; k<totalBalls; k++){
    hit[k] = Wire.read();
  }
  for(int k=0; k<totalBalls; k++){
    timeTaken[k] = Wire.read();
  }  
}

void resetStats(){
  target[totalBalls] = {0};
  hit[totalBalls] = {0};
  timeTaken[totalBalls] = {0};
  totalScore = 0;
  totalTimeTaken = 0;
  sessionNumber = sessionNumber + 1;
  _reset = true;
}

void setup(){
  Serial.begin(115200);
  Wire.begin(8);
  setupWiFi();
  server.begin();
  sessionNumber = 0;
}

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
  html_string = "HTTP/1.1 200 OK\r\n";
  html_string += "Content-Type: text/html\r\n\r\n";
  html_string += "<!DOCTYPE HTML>\r\n<html>\r\n";

  // results page
  if (req.indexOf("/results") > 0) {
    requestResults();
    html_string = results(html_string);
  }

  // session page
  else if (req.indexOf("/session") > 0) {
    Wire.beginTransmission(7);
    Wire.write(1);
    Wire.endTransmission();
    _reset = false;
    html_string = session(html_string);
  }

  // home page
  else {
    if(!_reset){
      resetStats();
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



