/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-slider-pwm/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.


*********
    Modifications to use a NeoPixel on the Adafruit ESP32-S2 Feather TFT - Ben Dash 2023
*********/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 1

const char* ssid = "XXXXXX";
const char* password = "XXXXXXXX";

String redValue = "0";
String greenValue = "0";
String blueValue = "0";

const char* PARAM_INPUT = "value";

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>NeoPixel RGB Controller</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; } 
    .red { color: red; }
    .green { color: green; }
    .blue { color: blue; }
  </style>
</head>
<body>
  <h2>NeoPixel <span style="color:red">R</span><span style="color:green">G</span><span style="color:blue">B</span> Controller</h2>
  <p>Red: <span id="textRedValue" class="red">%REDVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this, 'red')" id="redSlider" min="0" max="255" value="%REDVALUE%" step="1" class="slider"></p>
  <p>Green: <span id="textGreenValue" class="green">%GREENVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this, 'green')" id="greenSlider" min="0" max="255" value="%GREENVALUE%" step="1" class="slider"></p>
  <p>Blue: <span id="textBlueValue" class="blue">%BLUEVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this, 'blue')" id="blueSlider" min="0" max="255" value="%BLUEVALUE%" step="1" class="slider"></p>

<script>
function updateSliderPWM(element, color) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?color=" + color + "&value=" + element.value, true);
  xhr.send();
  document.getElementById("text" + color + "Value").innerHTML = element.value;
  console.log(element.value);
}
</script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  pixels.begin();
  pixels.setBrightness(50);
  pixels.clear();
  pixels.show();

  delay(10);

  tft.init(135, 240);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);    
  tft.drawRoundRect(1, 1, 239, 134, 10, ST77XX_RED);
  tft.drawRoundRect(6, 6, 229, 124, 10, ST77XX_GREEN);
  tft.drawRoundRect(11, 11, 219, 114, 10, ST77XX_BLUE);

  // start WiFi
  tft.println();
  tft.println();
  tft.println("  NeoPixel Control");
  tft.println();
  tft.println();
  tft.print("  IP");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    tft.print(".");
  }
  // Print ESP Local IP Address
  tft.println(WiFi.localIP());

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = String(index_html);
    html.replace("%REDVALUE%", redValue);
    html.replace("%GREENVALUE%", greenValue);
    html.replace("%BLUEVALUE%", blueValue);
    request->send(200, "text/html", html);
  });

server.on("/slider", HTTP_GET, [](AsyncWebServerRequest *request){
  String inputMessage;
  String redMessage;
  String greenMessage;
  String blueMessage;
  String outputMessage;
  String colorParam;  // define colorParam variable
    // GET input1 value on <ESP_IP>/slider?color=red&value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      colorParam = request->getParam("color")->value();  // set colorParam to the value of the color parameter
        if (colorParam == "red") {
          redValue = inputMessage;
          redMessage = "Red: " + redValue;
          pixels.setPixelColor(0, pixels.Color(redValue.toInt(), greenValue.toInt(), blueValue.toInt()));
        }
        if (colorParam == "green") {
          greenValue = inputMessage;
          greenMessage = "Green: " + greenValue;
          pixels.setPixelColor(0, pixels.Color(redValue.toInt(), greenValue.toInt(), blueValue.toInt()));
        }
        if (colorParam == "blue") {
          blueValue = inputMessage;
          blueMessage = "Blue: " + blueValue;
          pixels.setPixelColor(0, pixels.Color(redValue.toInt(), greenValue.toInt(), blueValue.toInt()));
        }
        pixels.show();
      }
      // Send the response
      outputMessage = redMessage + "<br>" + greenMessage + "<br>" + blueMessage + "<br>";
      request->send(200, "text/html", outputMessage);
  });

  server.begin();
}

void loop() {

}
