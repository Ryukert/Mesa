#include <WiFi.h>
#include <AsyncTCP.h>
#include <esp_task_wdt.h>
#include <ESPAsyncWebServer.h>

int arr[] = {21,25,28,32,35,39,42,45,48,51,54,57,61,64,67,70,74,76,79,82,85,88,91,94,97,100,103,106,108,111,113,116,118,121,123,126,128,131,133,136,138,141,145};
int frec[] = {1, 181, 363, 726, 1453};
 
const int derA = 18; 
const int derB = 19;
const int derA1 = 2;
const int derB1 = 4; 
int i,j;

// Replace with your network credentials
const char* ssid = "Lab Electronic";
const char* password = "!2345678";

const int output = 2;

String sliderValue = "0";
String sliderValue2 = "0";
String sliderValue3 = "0";
String sliderValue4 = "0";
String sliderValue5 = "0";

int motor1 = 0;
int motor2 = 0;

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int ledChanne2 = 1;
const int resolution = 8;

const char* PARAM_INPUT = "value";
const char* PARAM_INPUT2 = "value2";
const char* PARAM_INPUT3 = "value3";
const char* PARAM_INPUT4 = "value4";
const char* PARAM_INPUT5 = "value5";

float rpm;
float conv;
int pwm;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>MesaFI</title>
  <style>
    
  html {
  font-family: Arial; display: inline-block; text-align: center;
  font-size: 2.5em; /* 40px/16=2.5em */
  width: 100%;
  height:100%;
  background: linear-gradient(-45deg, #C8C7AD, #DEDA76);
}


form {
  width: 300px;
  margin: 0 auto;
  text-align: center;
  padding-top: 50px;
}

.value-button {
  display: inline-block;
  border: 1px solid #ddd;
  margin: 10px;
  width: 40px;
  height: 20px;
  text-align: center;
  vertical-align: middle;
  padding: 11px 0;
  color: white;
  background: #000;
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  -khtml-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
}

.value-button:hover {
  cursor: pointer;
}

form #decrease {
  margin-right: -4px;
  border-radius: 8px 0 0 8px;
}

form #increase {
  margin-left: -4px;
  border-radius: 0 8px 8px 0;
}

form #input-wrap {
  margin: 0px;
  padding: 0px;
}

input#number {
  font-family: Arial; 
   font-size: 1.5em; 
  text-align: center;
  border: none;
  border-bottom: 1px solid #ddd;
  margin: 0px;
  width: 200px;
  height: 120px;
}

input[type=number]::-webkit-inner-spin-button,
input[type=number]::-webkit-outer-spin-button {
    -webkit-appearance: none;
    margin: 0;
}

.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

.switch input { 
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider {
  background-color: #2196F3;
}

input:focus + .slider {
  box-shadow: 0 0 1px #2196F3;
}

input:checked + .slider:before {
  -webkit-transform: translateX(26px);
  -ms-transform: translateX(26px);
  transform: translateX(26px);
}

/* Rounded sliders */
.slider.round {
  border-radius: 34px;
}

.slider.round:before {
  border-radius: 50%;
}

.resultado {
    font-size: 1rem;
    font-family: sans-serif;
    padding: 1rem 0 2rem 2rem;
    color: white;
}

.button {
  background-color: black; /* Green */
  border: none;
  color: white;
  padding: 10px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 2px;
  cursor: pointer;
}

.button1 {border-radius: 4px;}

  </style>
</head>
<body>
  <h2>Mesa Vibradora FI-UAGro</h2>

    <div id="resultado" class="resultado">
            Modo <span id="status">Automatico</span>
          </div>

    <label class="switch">
      <input id="toggleswitch" type="checkbox">
      <span class="slider round"></span>
    </label>
          
    <!-- Sección para mover motores en modo AUTOMATICO -->
      <div>
        
      <!-- switch de primer motor -->
          <form>
            <select id="mySelect">
              <option value="1">128 mHz</option>
              <option value="2">64 mHz</option>
              <option value="3">32 mHz</option>
              <option value="4">16 mHz</option>
            </select>
          </form>

          <button class="button button1" id="boton1" onclick="boton()">Inicia</button>
          
      <!-- switch de segundo motor -->
          <form>
            <select id="mySelect2">
              <option value="1">128 mHz</option>
              <option value="2">64 mHz</option>
              <option value="3">32 mHz</option>
              <option value="4">16 mHz</option>
            </select>
          </form>
          
          <button class="button button1" id="boton2" onclick="boton2()">Inicia</button>
      
    </div>

     <!-- Sección para mover motores en modo MANUAL -->
    <div>
      <label>

    <!-- Incrementar valores de primer motor -->
         <form>
            <div class="value-button" id="decrease" style="display:none" onclick="decreaseValue()" value="Decrease Value">-</div>
              <input type="number" id="textSliderValue" style="display:none" min="21" max="146" value="21" disabled >
            <div class="value-button" id="increase" style="display:none" onclick="increaseValue()" value="Increase Value">+</div>
        </form>
        
    <!-- Incrementar valores de segundo motor -->        
        <form>
            <div class="value-button" id="decrease2" style="display:none" onclick="decreaseValue2()" value="Decrease Value">-</div>
              <input type="number" id="textSliderValue2" style="display:none" min="7" max="50" value="7" disabled >
            <div class="value-button" id="increase2"  style="display:none" onclick="increaseValue2()" value="Increase Value">+</div>
        </form>

    </div>
    
    <script>
    var input = document.getElementById('toggleswitch');
    var outputtext = document.getElementById('status');

    var input2 = document.getElementById('toggleswitch2');
    var outputtext2 = document.getElementById('status2');

    var xhr = new XMLHttpRequest();

    
    input.addEventListener('change',function(){
        if(this.checked) {
            outputtext.innerHTML = "Manual";
            
            document.getElementById("increase").style.display = '';
            document.getElementById("decrease").style.display = '';
            document.getElementById("increase2").style.display = '';
            document.getElementById("decrease2").style.display = '';
            document.getElementById("textSliderValue").style.display = '';
            document.getElementById("textSliderValue2").style.display = '';
            
            document.getElementById("boton1").style.display = 'none';
            document.getElementById("boton2").style.display = 'none';
            document.getElementById("mySelect2").style.display = 'none';
            document.getElementById("mySelect").style.display = 'none';

            
        } else {
            outputtext.innerHTML = "Automatico";

            document.getElementById("boton1").style.display = '';
            document.getElementById("boton2").style.display = '';
            document.getElementById("mySelect").style.display = '';
            document.getElementById("mySelect2").style.display = '';
            
            document.getElementById("increase").style.display = 'none';
            document.getElementById("decrease").style.display = 'none';
            document.getElementById("increase2").style.display = 'none';
            document.getElementById("decrease2").style.display = 'none';
            document.getElementById("textSliderValue").style.display = 'none';
            document.getElementById("textSliderValue2").style.display = 'none';
            
        }

        xhr.open("GET", "/slider?value5="+0, true);
        xhr.send();
        
    });
    
    function boton(){
      var x = document.getElementById("mySelect").value;
      updateAuto1(x);
    }

    function boton2(){
      var x = document.getElementById("mySelect2").value;
      updateAuto2(x);
    }
    
    function updatePWM(val) {
      xhr.open("GET", "/slider?value="+val, true);
      xhr.send();
    }

    function updatePWM2(val) {
      xhr.open("GET", "/slider?value2="+val, true);
      xhr.send();
    }

    function updateAuto1(val){
      xhr.open("GET", "/slider?value3="+val, true);
      xhr.send();
    }
    
    function updateAuto2(val){
      xhr.open("GET", "/slider?value4="+val, true);
      xhr.send(); 
 
    }
    
     function increaseValue() {
      var value = parseInt(document.getElementById('textSliderValue').value, 10);
     if(value<255){
        value=value+1;
      }
      document.getElementById('textSliderValue').value = value;
      updatePWM(value);
    }
    
    function decreaseValue() {
      var value = parseInt(document.getElementById('textSliderValue').value, 10);
      if(value==0){
        value=0;
      }else if(value>0){
        value=value-1;
      }
      document.getElementById('textSliderValue').value = value;
      updatePWM(value);
    }

    function increaseValue2() {
      var value = parseInt(document.getElementById('textSliderValue2').value, 10);
     if(value<255){
        value=value+1;
      }
      document.getElementById('textSliderValue2').value = value;
      updatePWM2(value);
    }
    
    function decreaseValue2() {
      var value = parseInt(document.getElementById('textSliderValue2').value, 10);
      if(value==0){
        value=0;
      }else if(value>0){
        value=value-1;
      }
      document.getElementById('textSliderValue2').value = value;
      updatePWM2(value);
    }
    
    </script>
    
  </body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if (var == "SLIDERVALUE"){
    return sliderValue;
  }
  return String();
}

void setup(){
  esp_task_wdt_init();
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(derA, OUTPUT);
  pinMode(derB, OUTPUT);  
  pinMode(derA1, OUTPUT);
  pinMode(derB1, OUTPUT);
  
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(derA, ledChannel);
  
  ledcWrite(ledChannel, motor1);

  ledcAttachPin(derA1, ledChanne2);
  
  ledcWrite(ledChanne2, motor2);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = inputMessage;
      conv= ((sliderValue.toInt()-0.67)/0.59);
      pwm = (int)conv;
      motor1 = pwm;
      Serial.println("\n-------\nPWM");
      Serial.println(pwm);
      Serial.println("RPM");
      ledcWrite(ledChannel, motor1);
      
    }else if (request->hasParam(PARAM_INPUT2)) {
      inputMessage = request->getParam(PARAM_INPUT2)->value();
      sliderValue2 = inputMessage;
      
      conv =  ((sliderValue2.toInt()+3.47)/0.21);
      pwm = (int)conv;
      motor2=pwm;
      Serial.println("\n-------\nPWM2");
      Serial.println(pwm);
      Serial.println("RPM2");
      ledcWrite(ledChanne2, motor2);
      
    }else if (request->hasParam(PARAM_INPUT3)) {
      inputMessage = request->getParam(PARAM_INPUT3)->value();
      sliderValue3 = inputMessage;

      
      if(sliderValue3.toInt() == 0){
        ledcWrite(ledChannel, 0);
        Serial.println("ZERO_1");
      }else if(sliderValue3.toInt() != 0){
      
          Serial.println("UNO_1");
          j=42;
          i=0;
          
          while(i<=j){
            conv= ((arr[i]-0.69)/0.57);
            pwm = (int)conv; 
            motor1=pwm;
            Serial.println(motor1);
            ledcWrite(ledChannel, motor1);
            delay(frec[sliderValue3.toInt()]);
            i=i+1;
         };
         
         while(j>=0){
            conv= ((arr[j]-0.69)/0.57);
            pwm = (int)conv; 
            motor1=pwm;
            Serial.println(motor1);
            ledcWrite(ledChannel, motor1);
            delay(frec[sliderValue3.toInt()]);
            j=j-1;
         };
         ledcWrite(ledChannel, 0);
      }
      
    }else if (request->hasParam(PARAM_INPUT4)) {
      inputMessage = request->getParam(PARAM_INPUT4)->value();
      sliderValue4 = inputMessage;
      
      if(sliderValue4.toInt() == 0){
        ledcWrite(ledChanne2, 0);
        Serial.println("ZERO_2");
      }else if(sliderValue4.toInt() != 0){
        
        Serial.println("UNO_2");
        j=50;
        i=7;
        do{
          conv= ((i+3.47)/0.21);
          pwm = (int)conv; 
          motor2=pwm;
          Serial.println(motor2);
          ledcWrite(ledChanne2, motor2);
          delay(frec[sliderValue4.toInt()]);
          i=i+1;
       }while(i<44);
       
       do{
            conv= ((j+3.47)/0.21);
            pwm = (int)conv; 
            motor2=pwm;
            Serial.println(motor2);
            ledcWrite(ledChanne2, motor2);
            delay(frec[sliderValue4.toInt()]);
            j=j-1;
         }while(j>=0);
         
      }
      ledcWrite(ledChanne2, 0);
    }else if (request->hasParam(PARAM_INPUT5)) {
      inputMessage = request->getParam(PARAM_INPUT5)->value();
      sliderValue5 = inputMessage;
      
      ledcWrite(ledChannel, sliderValue5.toInt());
      ledcWrite(ledChanne2, sliderValue5.toInt());
      
    }else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  
  // Start server
  server.begin();
}
  
void loop() { }
