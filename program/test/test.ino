//system soruce include
#include "esp_camera.h"
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <sstream>
#include <ESP32Servo.h>
#include <Wire.h>

//stm32 sensor
#define LIGHT_PIN 4

//i2c
#define wire_channel 8
#define I2C_SDA 15 
#define I2C_SCL 13

//wifi command
#define wifi_FORWARD_LEFT      7
#define wifi_FORWARD           8
#define wifi_FROWARD_RIGHT     9
#define wifi_RIGHT             4
#define wifi_STOP              5
#define wifi_LEFT              6
#define wifi_BACKWARD_LEFT     1
#define wifi_BACKWARD          2
#define wifi_BACKEARD_RIGHT    3
#define wifi_TRUN_RIGHT        66
#define wifi_TRUN_LEFT         44
#define wifi_ADJUST            0

//i2c command
#define CAR_FORWARD_LEFT      'u'
#define CAR_FORWARD           'i'
#define CAR_FROWARD_RIGHT     'o'
#define CAR_RIGHT             'j'
#define CAR_STOP              'k'
#define CAR_LEFT              'l'
#define CAR_BACKWARD_LEFT     'm'
#define CAR_BACKWARD          ','
#define CAR_BACKEARD_RIGHT    '.'
#define CAR_TRUN_RIGHT        'a'
#define CAR_TRUN_LEFT         'd'
#define PWM_ADJUST            't'

//Camera related constants
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26      //SDA of cam
#define SIOC_GPIO_NUM     27      //SCL of cam
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

//param
const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;
const int PWMSpeedChannel = 2;
const int PWMLightChannel = 3;

//WIFI 設定
const char* ssid     = "pan0428";
const char* password = "04836920";

AsyncWebServer server(80);
AsyncWebSocket wsCamera("/Camera");
AsyncWebSocket wsCarInput("/CarInput");
uint32_t cameraClientId = 0;

void moveCar(int inputValue)
{
  Serial.printf("Got value as %d\n", inputValue);  
  switch(inputValue)
  {    
    case wifi_FORWARD_LEFT:
      I2cArduinoUno_input(CAR_FORWARD_LEFT);                  
      break;
  
    case wifi_FORWARD:
      I2cArduinoUno_input(CAR_FORWARD);
      break;
  
    case wifi_FROWARD_RIGHT:
      I2cArduinoUno_input(CAR_FROWARD_RIGHT);
      break;
  
    case wifi_RIGHT:
      I2cArduinoUno_input(CAR_RIGHT);
      break;
 
    case wifi_STOP:
      I2cArduinoUno_input(CAR_STOP);
      break;

    case wifi_LEFT:
      I2cArduinoUno_input(CAR_LEFT);
      break;

    case wifi_BACKWARD_LEFT:
      I2cArduinoUno_input(CAR_BACKWARD_LEFT);
      break;

    case wifi_BACKWARD:
      I2cArduinoUno_input(CAR_BACKWARD);
      break;

    case wifi_BACKEARD_RIGHT:
      I2cArduinoUno_input(CAR_BACKEARD_RIGHT);
      break;
  
    default:
      I2cArduinoUno_input(CAR_STOP);
      break;
  }
}
void I2cArduinoUno_input(char val){
    Wire.beginTransmission(wire_channel);   //開始傳送,指定給從端8號 
    Wire.write(val);                        //寫入訊息 hello \n 
    Wire.endTransmission();                 //結束通訊 
}
//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////      網頁程式碼       //////////////////////////////

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(

<!DOCTYPE html>
<html>
    <head>
        <!-- 下面是表達網頁的編碼 -->
        <meta charset="UTF-8"/>
        <meta name="description"    content="這是潘鈴聲的ESP32 CAM 遙控網站"/>
        <meta name="author"         content="潘鈴聲"/>
        <meta name="keywords"       content="潘鈴聲, arduino, ESP32 CAM, 教學"/>
        <meta name="viewport"       content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no"/>
        <title>潘鈴聲的ESP32 CAM遙控網站</title> 
        <style>
        /* 小記 接下來會用到 箭頭(arrows) 按鈕(botton) 滑桿(slider) 防網頁誤觸 */
        /* 箭頭 */ 
        .arrows {
            font-size:30px;
            color:rgb(0, 255, 255);
        }
        /* 按鈕 */
        td.button {
            background-color:rgb(50, 41, 41);
            border-radius:25%;
            box-shadow: 5px 5px #666161;
        }
        /* 按鈕啟動後動畫 */
        td.button:active {
            transform: translate(5px,5px);
            box-shadow: none; 
        }
        /* 防止網頁被長按後跳出命令視窗 */
        .noselect { 
            -webkit-touch-callout:  none;   /* iOS Safari */
            -webkit-user-select:    none;   /* Safari */
            -khtml-user-select:     none;   /* Konqueror HTML */
            -moz-user-select:       none;   /* Firefox */
            -ms-user-select:        none;   /* Internet Explorer/Edge */
            user-select:            none;   /* Non-prefixed version, currently supported by Chrome and Opera */                                       
        }
        /* 滑桿寬度 */
        .slidecontainer {
            width: 100%;                    /* 設置滑桿容器的寬度為100% */
        }
        /* 滑桿顯示的各項數值 */
        .slider {
            -webkit-appearance: none;       /* 使用自定義的樣式,不使用瀏覽器預設樣式 */
            width: 100%;                    /* 設置滑桿的寬度為100% */
            height: 15px;                   /* 設置滑桿的高度為15px */
            border-radius: 5px;             /* 設置滑桿的邊框半徑為5px,呈現圓角的外觀 */
            background: #d3d3d3;          /* 設置滑桿的背景為淺灰色 */
            outline: none;                  /* 獲得焦點時不顯示外框線 */
            opacity: 0.7;                   /* 設置滑桿的透明度為0.7 */
            -webkit-transition: .2s;        /* 設置 WebKit 動畫的持續時間為0.2秒 */
            transition: opacity .2s;        /* 設置滑桿透明度的動畫持續時間為0.2秒 */
        }
        /*當滑鼠移動到滑桿上時,透明度變為1*/
        .slider:hover {
            opacity: 1;
        }
        /* 因為slider在不同瀏覽器有不同的預設模式,所以針對webkit(google), 和firefox做分別處理 */
        /* 設置 WebKit 樣式 */
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;       /* 不使用瀏覽器預設樣式 */
            appearance: none;               /* 不使用瀏覽器預設樣式 */
            width: 25px;                    /* 設置滑桿的寬度為25px */
            height: 25px;                   /* 設置滑桿的高度為25px */
            border-radius: 50%;             /* 設置滑桿的邊框半徑為50%,使其呈現圓形 */
            background: rgb(212, 0, 255);              /* 設置滑桿的背景顏色為紅色 */
            cursor: pointer;                /* 設置滑鼠游標顯示為指針,表示該元素可以被點擊或拖曳 */
        }
        /* 設置 Firefox 樣式 */
        .slider::-moz-range-thumb {
            width: 25px;                    /* 設置滑桿的寬度為25px */
            height: 25px;                   /* 設置滑桿的高度為25px */
            border-radius: 50%;             /* 設置滑桿的邊框半徑為50%,使其呈現圓形 */
            background: rgb(255, 0, 179);              /* 設置滑桿的背景顏色為紅色 */
            cursor: pointer;                /* 設置滑鼠游標顯示為指針,表示該元素可以被點擊或拖曳 */
        }
        </style>  
    </head>
    <!-- 整體架構排版 -->

    <!-- 設定基礎背景樣式 -->
    <body class="noselect" align="center" style="background-image: url('https://i.pximg.net/img-master/img/2019/06/26/12/23/03/75408210_p0_master1200.jpg'); background-size: cover; background-position: top center; ">    

        <!-- 主面板 -->
        <table  id="mainTable" style=" width:400px; margin:auto; table-layout:fixed; background-color: rgba(128, 128, 128, 0.5);"  CELLSPACING=10>
            <tr><!-- 鏡頭畫面 -->
                <img id="cameraImage" src="" style="width:400px;height:250px"></td>
            </tr> 
            <!-- 接下來是按鈕頁面 按鈕被按下和離手後會觸發 sandButtonInput函式 輸入值為( 字串, 數值 ) 數值在define中的 wifi_define 都設定好了 -->
            <tr><!-- 第一行按紐 -->
                <td class="button" ontouchstart='sendButtonInput("MoveCar","7")' ontouchend='sendButtonInput("MoveCar","5")'><span class="arrows" >&#8598;</span></td>
                <td class="button" ontouchstart='sendButtonInput("MoveCar","8")' ontouchend='sendButtonInput("MoveCar","5")'><span class="arrows" >&#8593;</span></td>
                <td class="button" ontouchstart='sendButtonInput("MoveCar","9")' ontouchend='sendButtonInput("MoveCar","5")'><span class="arrows" >&#8599;</span></td>
            </tr>
            <tr><!-- 第二行按紐 -->
                <td class="button" ontouchstart='sendButtonInput("MoveCar","4")' ontouchend='sendButtonInput("MoveCar","5")'><span class="arrows" >&#8592;</span></td>
                <td class="button"></td>   
                <td class="button" ontouchstart='sendButtonInput("MoveCar","6")' ontouchend='sendButtonInput("MoveCar","5")'><span class="arrows" >&#8594;</span></td>
            </tr>
            <tr><!-- 第三行按紐 -->
                <td class="button" ontouchstart='sendButtonInput("MoveCar","1")' ontouchend='sendButtonInput("MoveCar","5")'><span class="arrows" >&#8601;</span></td>
                <td class="button" ontouchstart='sendButtonInput("MoveCar","2")' ontouchend='sendButtonInput("MoveCar","5")'><span class="arrows" >&#8595;</span></td>
                <td class="button" ontouchstart='sendButtonInput("MoveCar","3")' ontouchend='sendButtonInput("MoveCar","5")'><span class="arrows" >&#8600;</span></td>
            </tr>
            <tr/><tr/>
            <!-- 接下來是滑桿頁面 會觸發 sandButtonInput函式 輸入值為( 字串, 數值 ) -->
            <tr><!-- 速度滑桿 -->
                <td style="text-align:left; color: rgb(255, 255, 255);"><b>Speed:</b></td>
                <td colspan=2>
                    <div class="slidecontainer">
                        <input type="range" min="0" max="255" value="150" class="slider" id="Speed" oninput='sendButtonInput("Speed",value)'>
                    </div>
                </td>
            </tr>        
            <tr><!-- LED燈滑桿 -->
                <td style="text-align:left; color: rgb(255, 255, 255);"><b>Light:</b></td>
                <td colspan=2>
                    <div class="slidecontainer">
                        <input type="range" min="0" max="255" value="0" class="slider" id="Light" oninput='sendButtonInput("Light",value)'>
                    </div>
                </td>   
            </tr>
            <tr><!-- 鏡頭底盤滑桿 -->
                <td style="text-align:left; color: rgb(255, 255, 255);"><b>Pan:</b></td>
                <td colspan=2>
                    <div class="slidecontainer">
                        <input type="range" min="0" max="180" value="90" class="slider" id="Pan" oninput='sendButtonInput("Pan",value)'>
                    </div>
                </td>
            </tr> 
            <tr><!-- 鏡頭上盤滑桿 -->
                <td style="text-align:left; color: rgb(255, 255, 255);"><b>Tilt:</b></td>
                <td colspan=2>
                    <div class="slidecontainer">
                        <input type="range" min="0" max="180" value="90" class="slider" id="Tilt" oninput='sendButtonInput("Tilt",value)'>
                    </div>
                </td>   
            </tr>      
        </table>

        <!-- 設定頁面採取的動作 -->
        <script>
            /* 建立websocket 溝通協定所需的變數 */
            var webSocketCameraUrl = "ws:\/\/" + window.location.hostname + "/Camera";          /* 建立鏡頭url */
            var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/CarInput";      /* 建立輸入指令的url */      
            var websocketCamera;        /* 儲存鏡頭畫面 */
            var websocketCarInput;      /* 儲存指令 */

            /* 啟動鏡頭圖像websocket傳輸 */
            function initCameraWebSocket() 
            {
                websocketCamera = new WebSocket(webSocketCameraUrl);            /* 將網路攝影機的畫面抓進畫面變數 */
                websocketCamera.binaryType = 'blob';                            /* 畫面變數資料型態變更為 blob */
                websocketCamera.onopen    = function(event){};                  /* 開啟協議的呼回 */
                websocketCamera.onclose   = function(event){setTimeout(initCameraWebSocket, 2000);};        /* 協議斷線的呼回 */
                websocketCamera.onmessage = function(event)                     /* 協議運作的呼回 */
                {
                    var imageId = document.getElementById("cameraImage");       /* 把網頁排板中的" cameraImage " 抓來賦予imageId的屬性 */
                    imageId.src = URL.createObjectURL(event.data);              /* 設定imageId中的src屬性為此傳輸協議的data 也就是圖像 */
                };
            }
            /* 啟動指令websocket傳輸 */
            function initCarInputWebSocket() 
            {
                websocketCarInput = new WebSocket(webSocketCarInputUrl);        /* 將指令抓進畫面變數 */
                websocketCarInput.onopen    = function(event)                   /* 開啟協議的呼回 */
                {
                    sendButtonInput("Speed", document.getElementById("Speed").value);       /* 開啟指令協議乎回時,以sendButtonInput為基礎開始傳輸數值 */
                    sendButtonInput("Light", document.getElementById("Light").value);
                    sendButtonInput("Pan",   document.getElementById("Pan").value);
                    sendButtonInput("Tilt",  document.getElementById("Tilt").value);                    
                };
                websocketCarInput.onclose   = function(event){setTimeout(initCarInputWebSocket, 2000);};        /* 協議斷線的呼回 */
                websocketCarInput.onmessage = function(event){};                /* 協議運作的呼回 */
            }
            /* 同時啟用上述兩個websocket溝通協定的函式 */
            function initWebSocket() 
            {
                initCameraWebSocket ();
                initCarInputWebSocket();
            }
             /* 設定按鈕傳輸函式 */
            function sendButtonInput(key, value) 
            {
                var data = key + "," + value;
                websocketCarInput.send(data);
            }                        
            window.onload = initWebSocket;      /* 畫面啟用時 同時啟用initWebSocket來啟動 websocket溝通協定的函式 */
            document.getElementById("mainTable").addEventListener("touchend", function(event){
                event.preventDefault()
            });      
        </script>

    </body> 
</html>
)HTMLHOMEPAGE";


/////////////////////////////      網頁程式碼結束       //////////////////////////////
//////////////////////////////////////////////////////////////////////////////////



void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}
void onCarInputWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      moveCar(5);
      ledcWrite(PWMLightChannel, 0); 
      //panServo.write(90);
      //tiltServo.write(90);       
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str()); 
        int valueInt = atoi(value.c_str());     
        if (key == "MoveCar")
        {
          moveCar(valueInt);        
        }
        else if (key == "Speed")
        {
          ledcWrite(PWMSpeedChannel, valueInt);
        }
        else if (key == "Light")
        {
          ledcWrite(PWMLightChannel, valueInt);         
        }
        else if (key == "Pan")
        {
          //panServo.write(valueInt);
        }
        else if (key == "Tilt")
        {
          //tiltServo.write(valueInt);   
        }             
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}
void onCameraWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      cameraClientId = client->id();
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      cameraClientId = 0;
      break;
    case WS_EVT_DATA:
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}
void setupCamera()
{
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_4;
  config.ledc_timer = LEDC_TIMER_2;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  //config.pin_sscb_sda = SIOD_GPIO_NUM;
  //config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) 
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }  

  if (psramFound())
  {
    heap_caps_malloc_extmem_enable(20000);  
    Serial.printf("PSRAM initialized. malloc to take memory from psram above this size");    
  }  
}
void sendCameraPicture()
{
  if (cameraClientId == 0)
  {
    return;
  }
  unsigned long  startTime1 = millis();
  //capture a frame
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) 
  {
      Serial.println("Frame buffer could not be acquired");
      return;
  }

  unsigned long  startTime2 = millis();
  wsCamera.binary(cameraClientId, fb->buf, fb->len);
  esp_camera_fb_return(fb);
    
  //Wait for message to be delivered
  while (true)
  {
    AsyncWebSocketClient * clientPointer = wsCamera.client(cameraClientId);
    if (!clientPointer || !(clientPointer->queueIsFull()))
    {
      break;
    }
    delay(1);
  }
  
  unsigned long  startTime3 = millis();  
  Serial.printf("Time taken Total: %d|%d|%d\n",startTime3 - startTime1, startTime2 - startTime1, startTime3-startTime2 );
}
void setUpPinModes()
{
  //panServo.attach(PAN_PIN);
  //tiltServo.attach(TILT_PIN);

  //Set up PWM
  ledcSetup(PWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(PWMLightChannel, PWMFreq, PWMResolution);

  //Att Pin
  //ledcAttachPin(PWMtoArduino_PIN, PWMSpeedChannel); 
  ledcAttachPin(LIGHT_PIN,        PWMLightChannel);
  moveCar(wifi_STOP);     
 }


void setup() { 
  //i2c
  Wire.begin(I2C_SDA,I2C_SCL);
  Wire.setClock(400000);
  pinMode(LIGHT_PIN, OUTPUT);  
  
  setUpPinModes();
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
      
  wsCamera.onEvent(onCameraWebSocketEvent);
  server.addHandler(&wsCamera);

  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);

  server.begin();
  Serial.println("HTTP server started");

  setupCamera();
  
}

void loop() {
  wsCamera.cleanupClients(); 
  wsCarInput.cleanupClients(); 
  sendCameraPicture(); 
  Serial.printf("SPIRam Total heap %d, SPIRam Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram());
}
