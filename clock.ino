#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266HTTPUpdateServer.h>
#define WEBSERVER_H
#include <WiFiUdp.h>
#include <NTPClient.h>               // Include NTPClient library
#include <Time.h>                 // Include Arduino time library

Adafruit_NeoPixel strip = Adafruit_NeoPixel(66, D2, NEO_GRB + NEO_KHZ800);

char active[] = "00000000000000000000000000001111111111000000000000000000000000000022222";
uint32_t colors[] = {0,0,0};

const char *ssid     = "XD_2.4Ghz";
const char *password = "Srvq4CTEm8Vrtu";
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "tempus1.gum.gov.pl", 3600 * 2, 60000);
BearSSL::ESP8266WebServerSecure https(443);
ESP8266HTTPUpdateServerSecure httpsUpdater;


const char html[] PROGMEM = R"(
<html>
    <head>
        <title>
            Color Changer
        </title>
        <meta charset='utf-8'>
        <script src=https://unpkg.com/vanilla-picker@2.11.2/dist/vanilla-picker.min.js integrity='sha384-Bmr8UHI3ZdrjltNzYImuM/badzP7XEDF62t5dGSuA3aKbImbSpEe16IMcBHVHfc7
        ' crossorigin='anonymous'></script>
        <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css' integrity='sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2' crossorigin='anonymous'>
        <script>
            var request = new XMLHttpRequest();
            var intStr = (x) => {
                s = x.toString(10);
                return s.length == 3 ? s : (s.length == 2 ? '0'+s:'00' + s);
            }
            let lastMsg = 0;
            let sendMsg = (msg) => {
                let now = new Date().getTime();
                if(now - lastMsg > 10){
                    lastMsg = now;
                    request.open('POST', 'api');
                    data.value = msg;
                    request.send(new FormData(form));
                }
            }
            var colorParser = (i) => (color) => {
                let msg = (intStr(color.rgba[0]) + intStr(color.rgba[1]) + intStr(color.rgba[2]) + i.toString())
                console.log(msg);
                sendMsg(msg);
            };
            var pickers = Array(4);
            var form, data;
            var genOpt = (htmlID, phyID) => { return {
                    parent:document.getElementById(htmlID),
                    alpha:false,
                    editor:false,
                    cancelButton:false,
                    onChange:colorParser(phyID)   
            }}
            var initColors= ()=>{
                pickers[0]= new Picker(genOpt('hours', 0));
                pickers[1]= new Picker(genOpt('spanel', 1));
                pickers[2]= new Picker(genOpt('minutes', 2));
                pickers[3]= new Picker(genOpt('all', 3));
                form = document.getElementById('req');
                data = document.getElementById('data')
            }
        </script>
    </head>
    <body onload='initColors()' class='bg-dark'>
        <nav class="nav justify-content-center">
            <a href='#' class='btn btn-dark' id='hours'>Set hours color</a>
            <a href='#' class='btn btn-dark' id='spanel'>Set side panel color</a>
            <a href='#' class='btn btn-dark' id='minutes'>Set minutes color</a>
            <a href='#' class='btn btn-dark' id='all'>Set all color</a>
        </nav>
        <nav class="nav justify-content-center">
            <a href='#' class='btn btn-dark' onclick='sendMsg(`0040000003`)'>Night</a>
            <a href='#' class='btn btn-dark' onclick='sendMsg(`0651240003`)'>Day</a>
            <a href='#' class='btn btn-dark' onclick='sendMsg(`2552552553`)'>Light</a>
        </nav>
        <form class="nav justify-content-center" id="req">
            <input type="password" class="form-control" id="secret" name="secret">
            <input type="hidden" id="data" name="data">
        </form>
</body>
</html>
)";

const char cert[] PROGMEM = R"(-----BEGIN CERTIFICATE-----
MIIC1jCCAj+gAwIBAgIUGm/GynwCP7pYMnzlmZ8/SKaTH5owDQYJKoZIhvcNAQEL
BQAwcDELMAkGA1UEBhMCUk8xCjAIBgNVBAgMAUIxDzANBgNVBAcMBlBvbGFuZDEY
MBYGA1UECgwPTWF0ZXVzelR5bCBbUk9dMRMwEQYDVQQLDApNYXRldXN6VHlsMRUw
EwYDVQQDDAwxOTIuMTY4LjAuOTkwHhcNMjEwNjAzMTY0MjIyWhcNMjIwNjAzMTY0
MjIyWjBwMQswCQYDVQQGEwJSTzEKMAgGA1UECAwBQjEPMA0GA1UEBwwGUG9sYW5k
MRgwFgYDVQQKDA9NYXRldXN6VHlsIFtST10xEzARBgNVBAsMCk1hdGV1c3pUeWwx
FTATBgNVBAMMDDE5Mi4xNjguMC45OTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkC
gYEAxu0GZwtHfCjhwGO+KUIdyf4t+N8lP1aG4m/r9SsbQsc27tb7zui68qrlOWoq
F3364yJt/Z01YPKqVcs1oH+oEoTN492hVXo1ayvss7hLb4VO091A5SeKoNPMo99E
meX6jtyE71zEP1R1yla/Iz0JzKMk3JoRU1N0tB+F1QWpcJsCAwEAAaNtMGswHQYD
VR0OBBYEFL3NtrrM+b/NpJWTSMDtNqJHk/YmMB8GA1UdIwQYMBaAFL3NtrrM+b/N
pJWTSMDtNqJHk/YmMA8GA1UdEwEB/wQFMAMBAf8wGAYDVR0RBBEwD4INZXNwODI2
Ni5sb2NhbDANBgkqhkiG9w0BAQsFAAOBgQC/kXJaQkixesBLI769LmvCnn6l8vrB
PUts5gKNAf05XKzZaWLG2DxPb+9mUOI2Xjv0+ku/mPjyTgA8Pjl+r0hjSubBK/Vs
BKPYoiorhZyiANfLydkrBFq3cOZJX/mmmOOjF0oIrU/+4x635a2DcNciDQiwvp1X
QhmRg1iiNN9/bA==
-----END CERTIFICATE-----)";

const char key[] PROGMEM = R"(-----BEGIN PRIVATE KEY-----
MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAMbtBmcLR3wo4cBj
vilCHcn+LfjfJT9WhuJv6/UrG0LHNu7W+87ouvKq5TlqKhd9+uMibf2dNWDyqlXL
NaB/qBKEzePdoVV6NWsr7LO4S2+FTtPdQOUniqDTzKPfRJnl+o7chO9cxD9UdcpW
vyM9CcyjJNyaEVNTdLQfhdUFqXCbAgMBAAECgYEAgRLzAJGVBL8SHF+b1/GKmrJO
6hkAdiSSyQKOPDqIKcBRqI/eO9dO1n6E1chJH40i6ZZBgDotlIZgd+KvHUE3sAdV
1TNhjLd9jirVlcWEqFQ9vMyYIahBBkPYtkQo6Zoear6bwK6N1RsYXpwI+zRqakYC
atXQTejd94RQYeMMSsECQQDrd8zmJUob6WqJCIMD3etMsSsKx88TIztMpioe/JOr
v0scLZV4NIyIvE4cPbcXp5hG+wgNewHljZw8s9OgNNZpAkEA2EWE4xJg5WYk2d5i
GW5S+Jju9GWXcBxGN8+nEL1DAzUYR6J2ahKaJ+VZhqy5SqaHYxgslg5WSVlmtj92
VbGWYwJAPiHNekkKphTQXLpXP9T1+rWhIuYYWPdp9U6/PI/vM1l1q0jH8QEEuCvf
/Im4X1CE2O3ycQQtOvJD8KyOwe5PSQJBANezXnprViD140M4vXbHxhET1yACQywZ
xx7bCLF1UmTci77Glnvs2bt4z/W2R4fg159J+Q4gdobpl7ulSAzn9ikCQHW9sUIK
nmeSeK0d2BrkLu3ADmmgy42OZmAvyW5AI03UK/7bbULFGz+seIXr6GsdhLp76ZzY
/v/FkXWrzooezXE=
-----END PRIVATE KEY-----)";

void setMany(char state, short offset, short howMany) {
  for (short i = offset; i < offset + howMany; i++)
    active[i] = state;
}

void setNumber(short offset, char number) {
  switch (number) {
    case '1':
      setMany('0', offset, 4);
      setMany('1', offset + 4, 2);
      setMany('0', offset + 6, 6);
      setMany('1', offset + 12, 2);
      break;
    case '2':
      setMany('0', offset, 1);
      setMany('1', offset + 1, 11);
      setMany('0', offset + 12, 2);
      break;
    case '3':
      setMany('0', offset, 1);
      setMany('1', offset + 1, 6);
      setMany('0', offset + 7, 2);
      setMany('1', offset + 9, 5);
      break;
    case '4':
      setMany('1', offset, 2);
      setMany('0', offset + 2, 2);
      setMany('1', offset + 4, 4);
      setMany('0', offset + 8, 4);
      setMany('1', offset + 12, 2);
      break;
    case '5':
      setMany('1', offset, 4);
      setMany('0', offset + 4, 2);
      setMany('1', offset + 6, 2);
      setMany('0', offset + 8, 1);
      setMany('1', offset + 9, 5);
      break;
    case '6':
      setMany('1', offset, 5);
      setMany('0', offset + 5, 1);
      setMany('1', offset + 6, 8);
      break;
    case '7':
      setMany('0', offset, 2);
      setMany('1', offset + 2, 4);
      setMany('0', offset + 6, 6);
      setMany('1', offset + 12, 2);
      break;
    case '8':
      setMany('1', offset, 14);
      break;
    case '9':
      setMany('1', offset, 8);
      setMany('0', offset + 8, 1);
      setMany('1', offset + 9, 5);
      break;
    case '0':
      setMany('1', offset, 6);
      setMany('0', offset + 6, 2);
      setMany('1', offset + 8, 6);
      break;

  }
}

uint32_t getColor(String rgb) {
  uint8_t colorParts[3];
  colorParts[0] = rgb.substring(0,3).toInt();
  colorParts[1] = rgb.substring(3,6).toInt();
  colorParts[2] = rgb.substring(6,9).toInt();
  return strip.Color(colorParts[0], colorParts[1], colorParts[2]);
}


void setColors() {
  short int set = 0;
  for (int diode = 0; diode < 66; diode++) {
    set = (diode == 28 || diode == 38) ? set + 1 : set;
    switch (active[diode]) {
      case '1':
        strip.setPixelColor(diode, colors[set]);
        break;
      case '0':
        strip.setPixelColor(diode, 0);
        break;
    }
  }
  strip.show();
}
  
void handleData(String data) {
    //data[len] = 0;
    switch (data.charAt(9)) {
      case '0':
        colors[0] = getColor(data);
        break;
      case '1':
        colors[1] = getColor(data);
        break;
      case '2':
        colors[2] = getColor(data);
        break;
      case '3':
        colors[0] = getColor(data);
        colors[1] = getColor(data);
        colors[2] = getColor(data);
    }
    setColors();
}

void handleForm() {
  if (https.method() != HTTP_POST) {
    https.send(405, "text/plain", "Method Not Allowed");
  } else {
    String message = "POST form was:\n";
    String secret = "zxcvbnm";
    String sName = "secret";
    String dName = "data";
    char iData = -1;
    bool verified = false;
    for (uint8_t i = 0; i < https.args(); i++) {
      message += " " + https.argName(i) + ": " + https.arg(i) + "\n";
      if(dName.equals(https.argName(i)))iData = (char)i;
      if(sName.equals(https.argName(i)))verified = secret.equals(https.arg(i));
    }
    if(iData != -1 && verified){
      handleData(https.arg(iData));
      https.send(200, "text/plain", "SUCCESS");
      }
    else{
      https.send(403, "text/plain", "Invalid secret!");  
    }
  }
}

String decodeAndUpdate(String message) {
  setNumber(0, message.charAt(0));
  setNumber(14, message.charAt(1));
  setNumber(38, message.charAt(2));
  setNumber(52, message.charAt(3));

  for (short i = 0; i < 6; i++) {
    char c = active[38 + i];
    active[38 + i] = active[46 + i];
    active[46 + i] = c;
  }
  return message;
}

void minuteIntervalJobs(short h, short m){
     //ws.cleanupClients();
     if(h == 0 && m == 0)
     handleData("0040000003");
     if(h == 7 && m == 0)
     handleData("0651240003");
  }
  
void timeRefresh() {
  String message = "3301";
  static short lastminute = 64;
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();
  short minute_ = minute(unix_epoch);
  short hour_   = hour(unix_epoch);
  if(minute_ != lastminute){
    message[3]  = minute_ % 10 + 48;
    message[2]  = minute_ / 10 + 48;
    message[1]  = hour_   % 10 + 48;
    message[0]  = hour_   / 10 + 48;
    minuteIntervalJobs(hour_, minute_);
    decodeAndUpdate(message);
    setColors();
    lastminute = minute_;
  }
}


void setup() {
  strip.begin();
  strip.show();
  Serial.begin(9600);

  colors[0] = getColor("0900000");
  colors[1] = getColor("0900000");
  colors[2] = getColor("0900000");
  decodeAndUpdate("3301");
  WiFi.begin(ssid, password);

  Serial.print("Connecting.");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");


  https.getServer().setRSACert(new BearSSL::X509List(cert), new BearSSL::PrivateKey(key));
  https.on("/", HTTP_GET, [](){
    https.send_P(200, "text/html", html);
  });
  https.on("/api", handleForm);
  httpsUpdater.setup(&https, "/update", "admin", "admin");
  timeClient.begin();
  https.begin();
}

void loop() {
  timeRefresh();
  https.handleClient();
  //delay(50);
}
