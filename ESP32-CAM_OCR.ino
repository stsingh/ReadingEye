//Sahej Singh (Massachusetts Academy of Mathematics and Science at WPI), credit to Author: ChungYi Fu(Kaohsiung, Taiwan), for much of web server HTML
//WIFI
const char* ssid     = "";   //Network Name
const char* password = "";   //Network Password

//AP
const char* apssid = "ESP32-CAM";
const char* appassword = "12345678";    //ap

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"         
#include "soc/soc.h"            
#include "soc/rtc_cntl_reg.h"   

String Feedback="";   
String Command="",cmd="",P1="",P2="",P3="",P4="",P5="",P6="",P7="",P8="",P9="";
//Bytes
byte ReceiveState=0,cmdState=1,strState=1,questionstate=0,equalstate=0,semicolonstate=0;

// Note: Requires PSRAM so use Wrover Module for ESP32

//Pin Definitions
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

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

WiFiServer server(80);

void ExecuteCommand()
{
  //Serial.println("");
  //Serial.println("Command: "+Command);
  if (cmd!="getstill") {
    Serial.println("cmd= "+cmd+" ,P1= "+P1+" ,P2= "+P2+" ,P3= "+P3+" ,P4= "+P4+" ,P5= "+P5+" ,P6= "+P6+" ,P7= "+P7+" ,P8= "+P8+" ,P9= "+P9);
    Serial.println("");
  }
  
  if (cmd=="your cmd") {
    // You can do anything.
    // Feedback="<font color=\"red\">Hello World</font>";
  }
  else if (cmd=="ip") {
    Feedback="AP IP: "+WiFi.softAPIP().toString();    
    Feedback+=", ";
    Feedback+="STA IP: "+WiFi.localIP().toString();
  }  
  else if (cmd=="mac") {
    Feedback="STA MAC: "+WiFi.macAddress();
  }  
  else if (cmd=="resetwifi") {  //WIFI
    WiFi.begin(P1.c_str(), P2.c_str());
    Serial.print("Connecting to ");
    Serial.println(P1);
    long int StartTime=millis();
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        if ((StartTime+5000) < millis()) break;
    } 
    Serial.println("");
    Serial.println("STAIP: "+WiFi.localIP().toString());
    Feedback="STAIP: "+WiFi.localIP().toString();
  }    
  else if (cmd=="restart") {
    ESP.restart();
  }
  else if (cmd=="digitalwrite") {
    ledcDetachPin(P1.toInt());
    pinMode(P1.toInt(), OUTPUT);
    digitalWrite(P1.toInt(), P2.toInt());
  }   
  else if (cmd=="analogwrite") {
    Serial.println(P2);
    if (P1="4") {
      ledcAttachPin(4, 4);  
      ledcSetup(4, 5000, 8);   
      ledcWrite(4,P2.toInt());  
    }
    else {
      ledcAttachPin(P1.toInt(), 5);
      ledcSetup(5, 5000, 8);
      ledcWrite(5,P2.toInt());
    }
  }  
  else if (cmd=="flash") {
    ledcAttachPin(4, 4);  
    ledcSetup(4, 5000, 8);   
     
    int val = P1.toInt();
    ledcWrite(4,val);  
  }  
  else if (cmd=="framesize") { 
    sensor_t * s = esp_camera_sensor_get();  
    if (P1=="QQVGA")
      s->set_framesize(s, FRAMESIZE_QQVGA);
    else if (P1=="HQVGA")
      s->set_framesize(s, FRAMESIZE_HQVGA);
    else if (P1=="QVGA")
      s->set_framesize(s, FRAMESIZE_QVGA);
    else if (P1=="CIF")
      s->set_framesize(s, FRAMESIZE_CIF);
    else if (P1=="VGA")
      s->set_framesize(s, FRAMESIZE_VGA);  
    else if (P1=="SVGA")
      s->set_framesize(s, FRAMESIZE_SVGA);
    else if (P1=="XGA")
      s->set_framesize(s, FRAMESIZE_XGA);
    else if (P1=="SXGA")
      s->set_framesize(s, FRAMESIZE_SXGA);
    else if (P1=="UXGA")
      s->set_framesize(s, FRAMESIZE_UXGA);           
    else 
      s->set_framesize(s, FRAMESIZE_QVGA);     
  }
  else if (cmd=="quality") { 
    sensor_t * s = esp_camera_sensor_get();
    int val = P1.toInt(); 
    s->set_quality(s, val);
  }
  else if (cmd=="contrast") {
    sensor_t * s = esp_camera_sensor_get();
    int val = P1.toInt(); 
    s->set_contrast(s, val);
  }
  else if (cmd=="brightness") {
    sensor_t * s = esp_camera_sensor_get();
    int val = P1.toInt();  
    s->set_brightness(s, val);  
  }
  else {
    Feedback="Command is not defined.";
  }
  if (Feedback=="") Feedback=Command;  
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  //drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);  //UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA

  ledcAttachPin(4, 4);  
  ledcSetup(4, 5000, 8);    
  
  WiFi.mode(WIFI_AP_STA);
  
  //Client
  //WiFi.config(IPAddress(192, 168, 201, 100), IPAddress(192, 168, 201, 2), IPAddress(255, 255, 255, 0));

  WiFi.begin(ssid, password);    //Connect

  delay(1000);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  long int StartTime=millis();
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      if ((StartTime+10000) < millis()) break;    //10
  } 

  if (WiFi.status() == WL_CONNECTED) {  
    WiFi.softAP((WiFi.localIP().toString()+"_"+(String)apssid).c_str(), appassword);   //IP Address        
    Serial.println("");
    Serial.println("STAIP address: ");
    Serial.println(WiFi.localIP());  

    for (int i=0;i<5;i++) {   //WIFI
      ledcWrite(4,10);
      delay(200);
      ledcWrite(4,0);
      delay(200);    
    }         
  }
  else {
    WiFi.softAP((WiFi.softAPIP().toString()+"_"+(String)apssid).c_str(), appassword);         

    for (int i=0;i<2;i++) {    //WIFI
      ledcWrite(4,10);
      delay(1000);
      ledcWrite(4,0);
      delay(1000);    
    }
  }     

  //AP and IP   
  //WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0)); 
  Serial.println("");
  Serial.println("APIP address: ");
  Serial.println(WiFi.softAPIP());    

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);  

  server.begin();          
}


static const char PROGMEM INDEX_HTML[] = R"rawliteral(
  <!DOCTYPE html>
  <head>
  <title>Text recognition (tesseract.js)</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <script src="https:\/\/ajax.googleapis.com/ajax/libs/jquery/1.8.0/jquery.min.js"></script>
  <script src="https:\/\/unpkg.com/tesseract.js/dist/tesseract.min.js"></script>
  </head>
  <body>
  <img id="ShowImage" src="">
  <table>
  <tr>
    <td><input type="button" id="restart" value="Restart"></td> 
    <td colspan="2"><input type="button" id="getStill" value="Get Still"></td> 
  </tr>  
  <tr>
    <td>Flash</td>
    <td colspan="2"><input type="range" id="flash" min="0" max="255" value="0"></td>
  </tr>
  <tr>
    <td>Quality</td>
    <td colspan="2"><input type="range" id="quality" min="10" max="63" value="10"></td>
  </tr>
  <tr>
    <td>Brightness</td>
    <td colspan="2"><input type="range" id="brightness" min="-2" max="2" value="0"></td>
  </tr>
  <tr>
    <td>Contrast</td>
    <td colspan="2"><input type="range" id="contrast" min="-2" max="2" value="0"></td>
  </tr>
  <tr>
    <td>Resolution</td> 
    <td colspan="2">
    <select id="framesize">
        <option value="UXGA">UXGA(1600x1200)</option>
        <option value="SXGA">SXGA(1280x1024)</option>
        <option value="XGA">XGA(1024x768)</option>
        <option value="SVGA" selected="selected">SVGA(800x600)</option>
        <option value="VGA">VGA(640x480)</option>
        <option value="CIF">CIF(400x296)</option>
        <option value="QVGA">QVGA(320x240)</option>
        <option value="HQVGA">HQVGA(240x176)</option>
        <option value="QQVGA">QQVGA(160x120)</option>
    </select> 
    </td>
  </tr>      
  <tr>
    <td>Rotate</td>
    <td align="left" colspan="2">
        <select onchange="document.getElementById('canvas').style.transform='rotate('+this.value+')';">
          <option value="0deg">0deg</option>
          <option value="90deg">90deg</option>
          <option value="180deg">180deg</option>
          <option value="270deg">270deg</option>
        </select>
    </td>
  </tr>  
  </table>
  <br>
  <select id="lang">
    <option value='afr'>Afrikaans</option>
    <option value='amh'>Amharic</option>
    <option value='ara'>Arabic</option>
    <option value='asm'>Assamese</option>
    <option value='aze'>Azerbaijani</option>
    <option value='aze_cyrl'>Azerbaijani - Cyrillic</option>
    <option value='bel'>Belarusian</option>
    <option value='ben'>Bengali</option>
    <option value='bod'>Tibetan</option>
    <option value='bos'>Bosnian</option>
    <option value='bul'>Bulgarian</option>
    <option value='cat'>Catalan; Valencian</option>
    <option value='ceb'>Cebuano</option>
    <option value='ces'>Czech</option>
    <option value='chi_sim'>Chinese - Simplified</option>
    <option value='chi_tra'>Chinese - Traditional</option>
    <option value='chr'>Cherokee</option>
    <option value='cym'>Welsh</option>
    <option value='dan'>Danish</option>
    <option value='deu'>German</option>
    <option value='dzo'>Dzongkha</option>
    <option value='ell'>Greek, Modern (1453-)</option>
    <option value='eng' selected>English</option>
    <option value='enm'>English, Middle (1100-1500)</option>
    <option value='epo'>Esperanto</option>
    <option value='est'>Estonian</option>
    <option value='eus'>Basque</option>
    <option value='fas'>Persian</option>
    <option value='fin'>Finnish</option>
    <option value='fra'>French</option>
    <option value='frk'>German Fraktur</option>
    <option value='frm'>French, Middle (ca. 1400-1600)</option>
    <option value='gle'>Irish</option>
    <option value='glg'>Galician</option>
    <option value='grc'>Greek, Ancient (-1453)</option>
    <option value='guj'>Gujarati</option>
    <option value='hat'>Haitian; Haitian Creole</option>
    <option value='heb'>Hebrew</option>
    <option value='hin'>Hindi</option>
    <option value='hrv'>Croatian</option>
    <option value='hun'>Hungarian</option>
    <option value='iku'>Inuktitut</option>
    <option value='ind'>Indonesian</option>
    <option value='isl'>Icelandic</option>
    <option value='ita'>Italian</option>
    <option value='ita_old'>Italian - Old</option>
    <option value='jav'>Javanese</option>
    <option value='jpn'>Japanese</option>
    <option value='kan'>Kannada</option>
    <option value='kat'>Georgian</option>
    <option value='kat_old'>Georgian - Old</option>
    <option value='kaz'>Kazakh</option>
    <option value='khm'>Central Khmer</option>
    <option value='kir'>Kirghiz; Kyrgyz</option>
    <option value='kor'>Korean</option>
    <option value='kur'>Kurdish</option>
    <option value='lao'>Lao</option>
    <option value='lat'>Latin</option>
    <option value='lav'>Latvian</option>
    <option value='lit'>Lithuanian</option>
    <option value='mal'>Malayalam</option>
    <option value='mar'>Marathi</option>
    <option value='mkd'>Macedonian</option>
    <option value='mlt'>Maltese</option>
    <option value='msa'>Malay</option>
    <option value='mya'>Burmese</option>
    <option value='nep'>Nepali</option>
    <option value='nld'>Dutch; Flemish</option>
    <option value='nor'>Norwegian</option>
    <option value='ori'>Oriya</option>
    <option value='pan'>Panjabi; Punjabi</option>
    <option value='pol'>Polish</option>
    <option value='por'>Portuguese</option>
    <option value='pus'>Pushto; Pashto</option>
    <option value='ron'>Romanian; Moldavian; Moldovan</option>
    <option value='rus'>Russian</option>
    <option value='san'>Sanskrit</option>
    <option value='sin'>Sinhala; Sinhalese</option>
    <option value='slk'>Slovak</option>
    <option value='slv'>Slovenian</option>
    <option value='spa'>Spanish; Castilian</option>
    <option value='spa_old'>Spanish; Castilian - Old</option>
    <option value='sqi'>Albanian</option>
    <option value='srp'>Serbian</option>
    <option value='srp_latn'>Serbian - Latin</option>
    <option value='swa'>Swahili</option>
    <option value='swe'>Swedish</option>
    <option value='syr'>Syriac</option>
    <option value='tam'>Tamil</option>
    <option value='tel'>Telugu</option>
    <option value='tgk'>Tajik</option>
    <option value='tgl'>Tagalog</option>
    <option value='tha'>Thai</option>
    <option value='tir'>Tigrinya</option>
    <option value='tur'>Turkish</option>
    <option value='uig'>Uighur; Uyghur</option>
    <option value='ukr'>Ukrainian</option>
    <option value='urd'>Urdu</option>
    <option value='uzb'>Uzbek</option>
    <option value='uzb_cyrl'>Uzbek - Cyrillic</option>
    <option value='vie'>Vietnamese</option>
    <option value='yid'>Yiddish</option>
  </select><br>
  <button id="btnDetect" onclick="TextRecognition();" disabled>Text recognition</button>
  <br>
  <div id="result" style="width:320px;color:red"></div>
  
  <script>
    var getStill = document.getElementById('getStill');
    var ShowImage = document.getElementById('ShowImage');
    var result = document.getElementById('result');
    var flash = document.getElementById('flash');
    var btnDetect = document.getElementById('btnDetect');
    var myTimer;
    var restartCount=0;

    function all() {
      
      $( document ).ready(function() {
        $( "#getStill" ).trigger( "click" );
      });
      setTimeout(TextRecognition(), 10000);
    }

    getStill.onclick = function (event) {  
      clearInterval(myTimer);  
      myTimer = setInterval(function(){error_handle();},5000);
      ShowImage.src=location.origin+'/?getstill='+Math.random();      
    }

    function error_handle() {
      btnDetect.disabled =true;
      restartCount++;
      clearInterval(myTimer);
      if (restartCount<=2) {
        result.innerHTML = "Get still error. <br>Restart ESP32-CAM "+restartCount+" times.";
        myTimer = setInterval(function(){getStill.click();},10000);
      }
      else
        result.innerHTML = "Get still error. <br>Please close the page and check ESP32-CAM.";
    }  

    ShowImage.onload = function (event) {
      btnDetect.disabled =false;
      clearInterval(myTimer);
      restartCount=0; 
      getStill.click();
    }         
    
    restart.onclick = function (event) {
      fetch(location.origin+'?restart=stop');
    }    

    framesize.onclick = function (event) {
      fetch(document.location.origin+'?framesize='+this.value+';stop');
    }  

    flash.onchange = function (event) {
      fetch(location.origin+'?flash='+this.value+';stop');
    } 

    quality.onclick = function (event) {
      fetch(document.location.origin+'?quality='+this.value+';stop');
    } 

    brightness.onclick = function (event) {
      fetch(document.location.origin+'?brightness='+this.value+';stop');
    } 

    contrast.onclick = function (event) {
      fetch(document.location.origin+'?contrast='+this.value+';stop');
    }                             
    
    function getFeedback(target) {
      var data = $.ajax({
      type: "get",
      dataType: "text",
      url: target,
      success: function(response)
        {
          result.innerHTML = response;
        },
        error: function(exception)
        {
          result.innerHTML = 'fail';
        }
      });
    }   

    function TextRecognition() {
      result.innerHTML = "Recognize...";
      
      Tesseract.recognize(
        ShowImage,
        lang.value,
        { logger: m => console.log(m) }
      ).then(({ data: { text } }) => {
  
        result.innerHTML = text.replace(/\n/g, "<br>");
        text = result.innerHTML.replace(/<br>/g, "");
        tts(text);
        if (text=="a"||text=="A") {
          $.ajax({url: document.location.origin+'?analogwrite=4;10', async: false});
          //$.ajax({url: document.location.origin+'?digitalwrite=2;1', async: false});
        }
        else if (text=="b"||text=="B") {
          $.ajax({url: document.location.origin+'?analogwrite=4;0', async: false});
          //$.ajax({url: document.location.origin+'?digitalwrite=2;0', async: false});
        }
      })
    }     
    window.onload = all;
    
    function tts(text) {
      if ('speechSynthesis' in window) {
        var synthesis = window.speechSynthesis;
        
        // Get the first `en` language voice in the list
        var voice = synthesis.getVoices().filter(function(voice) {
          return voice.lang === 'en';
        })[30];

        //Get utterance object
        var utterance = new SpeechSynthesisUtterance(text);

        // Set utterance properties
        utterance.voice = voice;
        utterance.pitch = 1;
        utterance.rate = 1;
        utterance.volume = 0.8;

        // Speak the utterance
        synthesis.speak(utterance);

        
      } else {
        console.log('Text-to-speech not supported.');
      }
    }
  </script>
  </body>
  </html> 
)rawliteral";



void loop() {
  Feedback="";Command="";cmd="";P1="";P2="";P3="";P4="";P5="";P6="";P7="";P8="";P9="";
  ReceiveState=0,cmdState=1,strState=1,questionstate=0,equalstate=0,semicolonstate=0;
  
   WiFiClient client = server.available();

  if (client) { 
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();             
        
        getCommand(c);   //Get Command
                
        if (c == '\n') {
          if (currentLine.length() == 0) {    
            
            if (cmd=="getstill") {
              //JPEG
              camera_fb_t * fb = NULL;
              fb = esp_camera_fb_get();  
              if(!fb) {
                Serial.println("Camera capture failed");
                delay(1000);
                ESP.restart();
              }
  
              client.println("HTTP/1.1 200 OK");
              client.println("Access-Control-Allow-Origin: *");              
              client.println("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
              client.println("Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS");
              client.println("Content-Type: image/jpeg");
              client.println("Content-Disposition: form-data; name=\"imageFile\"; filename=\"picture.jpg\""); 
              client.println("Content-Length: " + String(fb->len));             
              client.println("Connection: close");
              client.println();
              
              uint8_t *fbBuf = fb->buf;
              size_t fbLen = fb->len;
              for (size_t n=0;n<fbLen;n=n+1024) {
                if (n+1024<fbLen) {
                  client.write(fbBuf, 1024);
                  fbBuf += 1024;
                }
                else if (fbLen%1024>0) {
                  size_t remainder = fbLen%1024;
                  client.write(fbBuf, remainder);
                }
              }  
              
              esp_camera_fb_return(fb);
            
              pinMode(4, OUTPUT);
              digitalWrite(4, LOW);               
            }
            else {
              //HTML Feedback
              client.println("HTTP/1.1 200 OK");
              client.println("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
              client.println("Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS");
              client.println("Content-Type: text/html; charset=utf-8");
              client.println("Access-Control-Allow-Origin: *");
              client.println("Connection: close");
              client.println();
              
              String Data="";
              if (cmd!="")
                Data = Feedback;
              else {
                Data = String((const char *)INDEX_HTML);
              }
              int Index;
              for (Index = 0; Index < Data.length(); Index = Index+1000) {
                client.print(Data.substring(Index, Index+1000));
              }           
              
              client.println();
            }
                        
            Feedback="";
            break;
          } else {
            currentLine = "";
          }
        } 
        else if (c != '\r') {
          currentLine += c;
        }

        if ((currentLine.indexOf("/?")!=-1)&&(currentLine.indexOf(" HTTP")!=-1)) {
          if (Command.indexOf("stop")!=-1) {  //stop -> http://192.168.xxx.xxx/?cmd=aaa;bbb;ccc;stop
            client.println();
            client.println();
            client.stop();
          }
          currentLine="";
          Feedback="";
          ExecuteCommand();
        }
      }
    }
    delay(1);
    client.stop();
  }
}

//Get Cmd
void getCommand(char c)
{
  if (c=='?') ReceiveState=1;
  if ((c==' ')||(c=='\r')||(c=='\n')) ReceiveState=0;
  
  if (ReceiveState==1)
  {
    Command=Command+String(c);
    
    if (c=='=') cmdState=0;
    if (c==';') strState++;
  
    if ((cmdState==1)&&((c!='?')||(questionstate==1))) cmd=cmd+String(c);
    if ((cmdState==0)&&(strState==1)&&((c!='=')||(equalstate==1))) P1=P1+String(c);
    if ((cmdState==0)&&(strState==2)&&(c!=';')) P2=P2+String(c);
    if ((cmdState==0)&&(strState==3)&&(c!=';')) P3=P3+String(c);
    if ((cmdState==0)&&(strState==4)&&(c!=';')) P4=P4+String(c);
    if ((cmdState==0)&&(strState==5)&&(c!=';')) P5=P5+String(c);
    if ((cmdState==0)&&(strState==6)&&(c!=';')) P6=P6+String(c);
    if ((cmdState==0)&&(strState==7)&&(c!=';')) P7=P7+String(c);
    if ((cmdState==0)&&(strState==8)&&(c!=';')) P8=P8+String(c);
    if ((cmdState==0)&&(strState>=9)&&((c!=';')||(semicolonstate==1))) P9=P9+String(c);
    
    if (c=='?') questionstate=1;
    if (c=='=') equalstate=1;
    if ((strState>=9)&&(c==';')) semicolonstate=1;
  }
}
