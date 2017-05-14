//MOSTLY WORKING - only issue is LED always reset to high. Just swapped the toggle to default low, so maybe that'll change it.

#include <ESP8266WiFi.h>
#include <Time.h>
//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "soccerskilz"; //Password
WiFiServer server(80);
String html_string;

void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  String AP_NameString = "Soccer Controls"; //Set the wifi name
  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  Serial.begin(115200);
}
void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
}

String home_(String html_string){
  html_string += "<style>p{font-size:50pt;}button{height:50px;width:200px;font-size:20pt;}</style>\r\n";
  html_string += "<body><p>Welcome to ASSISTS.<br>Brought to you by DSGN 384.<br><br>\r\n";
  html_string += "Instructions:<br>\r\n";
  html_string += "&nbsp;1. Set up goals 20 feet from center<br>\r\n";
  html_string += "&nbsp;2. Have teammate ready with 10 balls<br>\r\n";
  html_string += "&nbsp;3. xxx<br><br>Press start when ready!</p>\r\n";
  html_string += "<button type=\"button\" onclick=\"location.href = '/session';\">Start</button>\r\n";
  html_string +=  "</body>\r\n</html>\n";
  return html_string;
}

String session(String html_string){
  html_string += "<style>p{font-size:50pt;}</style>\r\n";
  html_string += "<script>\r\n";
  html_string += "var s=-3;\r\n";
  html_string += "function add(){\r\n";
  html_string += "  s+=0.1;\r\n";
  html_string += "  document.getElementById('time').innerHTML = s.toFixed(1); \r\n";
  html_string += "  stopwatch();\r\n";
  html_string += "}\r\n";
  html_string += "function stopwatch() {\r\n";
  html_string += "  var t = setTimeout(add, 100);\r\n";
  html_string += "}\r\n";
  html_string += "</script>\r\n";
  html_string += "<body onload=\"stopwatch()\">\r\n";
  html_string += "<p style=\"font-size: 50pt\">Starting new session...</p>\r\n";
  html_string += "<p id=\"time\">0</p>\r\n";
  html_string += "<button type=\"button\" onclick=\"location.href = '/results';\" style=\"height:50px;width:200px\">See results</button>\r\n";
  html_string += "</body></html>\r\n";
  return html_string;
}

String results(String html_string){
  html_string += "<style>\r\n";
  html_string += "div{font-size:50pt;}\r\n";
  html_string += "table,th,td{border: 1px solid black; font-size:30pt;}\r\n";
  html_string += "button{height:50px;width:200px;font-size:30pt;}\r\n";
  html_string += "</style><script>\r\n";
  html_string += "function startTime() {\r\n";
  html_string += "  var today = new Date();\r\n";
  html_string += "  var dateString = today.toLocaleDateString();\r\n";
  html_string += "  var timeString = today.toLocaleTimeString();\r\n";
  html_string += "  document.getElementById('date').innerHTML = \"Date: \" + dateString;\r\n";
  html_string += "  document.getElementById('time').innerHTML = \"Time: \" + timeString;}\r\n";
  html_string += "</script>\r\n";
  html_string += "<body onload=\"startTime()\">\r\n";
  html_string += "<div id=\"session\">Session: X</div>\r\n";
  html_string += "<div id=\"date\"></div>\r\n";
  html_string += "<div id=\"time\"></div>\r\n";
  html_string += "<div id=\"score\">Score: </div>\r\n";
  html_string += "<div id=\"timeTaken\">Time Taken: </div>\r\n";
  html_string += "<table>\r\n";
  html_string += "<tr><th>Ball #</th><th>Target</th><th>Hit?</th><th>Time</th></tr>\r\n";
  html_string += "<tr><td>1</td><td>X</td><td>HIT</td><td>X</td></tr>\r\n";
  html_string += "<tr><td>2</td><td>X</td><td>HIT</td><td>X</td></tr>\r\n";
  html_string += "<tr><td>3</td><td>X</td><td>HIT</td><td>X</td></tr>\r\n";
  html_string += "<tr><td>4</td><td>X</td><td>HIT</td><td>X</td></tr>\r\n";
  html_string += "<tr><td>5</td><td>X</td><td>HIT</td><td>X</td></tr>\r\n";
  html_string += "<tr><td>6</td><td>X</td><td>HIT</td><td>X</td></tr>\r\n";
  html_string += "<tr><td>7</td><td>X</td><td>HIT</td><td>X</td></tr>\r\n";
  html_string += "<tr><td>8</td><td>X</td><td>HIT</td><td>X</td></tr>\r\n";
  html_string += "<tr><td>9</td><td>X</td><td>HIT</td><td>X</td></tr>\r\n";
  html_string += "<tr><td>10</td><td>X</td><td>HIT</td><td>X</td></tr></table><br>\r\n";
  html_string += "<button type=\"button\" onclick=\"location.href = '/';\">End</button>\r\n";
  html_string += "</body></html>\r\n";
  return html_string;
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
    html_string = results(html_string);
  }

  // session page
  else if (req.indexOf("/session") > 0) {
    html_string = session(html_string);
  }

  // home page
  else {
    html_string = home_(html_string);
  }
  
  // Send the response to the client
  client.print(html_string);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}


