//**********************************************************************************************************
//*    ESP32_iradio_srv-- Internet radio with control function by web server
//*
//**********************************************************************************************************
//**********************************************************************************************************
//*    audioI2S-- I2S audiodecoder for ESP32,                                                              *
//**********************************************************************************************************
//
// first release on 11/2018
// Version 2  , Aug.05/2019
//
//
// THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT.
// FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHOR
// OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
//
//  2020/5/6 modified by asmaro
//  2020/9/17 modified by asmaro
//  2021/2/2 modified by asmaro to support server
//
#include "Arduino.h"
#include "WiFiMulti.h"
#include "WebServer.h"
#include "Audio.h"
#include "Wire.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "U8g2lib.h"
#include "EEPROM.h"
#include "esp_deep_sleep.h"
//#include "U8x8lib.h"

// Digital I/O used
#define SD_CS         5   // unused 
#define SPI_MOSI      23  // unused
#define SPI_MISO      19  // unused
#define SPI_SCK       18  // unused
#define I2S_DOUT      25
#define I2S_BCLK      26
#define I2S_LRC       22
#define LED           2   // on board led
#define SLEEP         32  // sleep timer control button pin

Audio audio;
WiFiMulti wifiMulti;
//  Display, using I2C interface, not required
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 4, /* data=*/ 17, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/*clock*/4, /*data*/5 );
//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

String ssid =     "xxxxxxxx";      // WiFi 1
String password = "pppppppp";
String ssid2 =     "xxxxxxxx2";   // WiFi 2, uncomment wifiMulti.addAP() below
String password2 = "pppppppp2";   // if you want to specify it
// Pin for buttons
const int volPin1 = 14;
const int volPin2 = 12;
const int intPin = 13;    // change to next station
int volume;
int station;
int ct2,pt2,event;  // interval time 
int sleeptimer = 0;
bool sleepmode = false;
hw_timer_t * timer = NULL; //timer
// EEROM
struct st_eerom {
  int vol;           // saved volume value
  int stn;           // saved preset staion index 
  char stnname[48];  // saved staion name
  char stnurl[128];  // saved station url
};
st_eerom d_eerom;
// Deep sleep point
void IRAM_ATTR sleep_timer() {
  digitalWrite(LED,LOW);
  u8g2.clearDisplay();
  esp_deep_sleep_start();  // sleep forever
  //sleepmode=false;
}
// Preset internet station url
char* station_url[]={
  "http://igor.torontocast.com:1710/stream",  "http://igor.torontocast.com:1025/stream",
  "http://agnes.torontocast.com:8083/stream",  "string 4"
};
// J1GOLD https://jenny.torontocast.com:2000/stream/J1GOLD
char* station_name[]={
  "J-Pop Sakura",  "J-Pop hits",
  "JPOP Project",  "string 4"
};
char stnurl[128];  // current internet station url
char stnname[24];  // current internet station name
int max_station = 3;
WebServer server(80);  // port 80(default)
// Operation by server
int s_srv = 1;
int a_srv = 1;
int b_srv = 1;
char titlebuf[166];

void handleRoot(void)
{
    String html;
    char stnno[4];
    html = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>I-Radio</title>";
    //html += "<html>";
    //html += "<head>";
    //html += "<meta charset=\"utf-8\">";
    //html += "<title>Internet Radio</title>";
    html += "</head><body><p><a href=\"/oper\">Operation</a></p><p>Now playing:</p>";
    //html += "<body>";
    //html += "<p><a href=\"/oper\">Operation</a></p>";
    //html += "<p>Now playing:</p>";
    html += "<p>";
    html += stnurl;
    html += "</p>";
    html +=  titlebuf;
    html += "<p>Volume: ";
    sprintf(stnno,"%d",volume);
    html += stnno;
    html += "</p>";
    html += "<p>Preset Station List: ";
    sprintf(stnno,"%d",station+1);
    html += stnno;
    html += "</p>";
    html += "<ol>";
    html += "<li>";
    html += station_url[0];
    html += "</li>";
    html += "<li>";
    html += station_url[1];
    html += "</li>";
    html += "<li>";
    html += station_url[2];
    html += "</li>";
    html += "</ol>";
    html += "</body>";
    html += "</html>";
    server.send(200, "text/html", html);
}
void handleOper(void)  // Hadle Operation for button and station setting
{
    String html;
    char stn[5];
    stn[0]=0;
    if (server.method() == HTTP_POST) {
       String val1 = server.arg("slp");
       String val2 = server.arg("vol");
       String val3 = server.arg("stn");
       String val4 = server.arg("stnset");
       if (val1 == "sleep") {
         Serial.println("Sleep_onoff");
         s_srv=0;
       }
       if (val2 == "down") {
         Serial.println("Volume_down");
         a_srv=0;
       }
       if (val2 == "up") {
         Serial.println("Volume_up");
         b_srv=0;
       }
       if (val3 == "next") {
         Serial.println("Next_staion");
         station_setting();
       }
       if (val4 == "set") {
         Serial.print("Set_staion:1 ");
         // Get input station setting
         String s_stnurl = server.arg("stnurl");
         String s_stnname = server.arg("stnname");
         Serial.print(s_stnname); Serial.print(" ");
         Serial.println(s_stnurl);
         // Set current
         strcpy(stnurl,s_stnurl.c_str());
         strcpy(stnname,s_stnname.c_str());
         bool conn_ok;
         conn_ok=audio.connecttohost(s_stnurl.c_str()); // change station
         if (conn_ok) {
             Serial.println("Set new staion OK");
         } else {
             Serial.println("Set new staion failed");
             strcpy(stn,"Err!");
         }
       }
       if (val4 == "save") {
         Serial.print("Save_staion: ");
         Serial.println(stnurl);
         strcpy(d_eerom.stnurl,stnurl); // save current
         strcpy(d_eerom.stnname,stnname);
         EEPROM.put<st_eerom>(0, d_eerom);
         EEPROM.commit();
       }
    }
    html = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>I-Radio Oper</title>";
    //html += "<html>";
    //html += "<head>";
    //html += "<meta charset=\"utf-8\">";
    //html += "<title>Internet Radio Operation</title>";
    html += "</head><body><p>Operation</p><form method=\"post\" action=\"?\">";
    //html += "<body>";
    //html += "<p>Operation</p>";
    //html += "<form method=\"post\" action=\"?\">";
    html += "<table style=\”border:none;\”><tr>";
    //html += "<tr>";
    html += "<td><button type=\"submit\" formaction=\"/oper\" name=\"slp\" value=\"sleep\">SLEEP</button></td>";
    html += "<td><button type=\"submit\" formaction=\"/oper\" name=\"vol\" value=\"down\">VOL-</button></td>";
    html += "<td><button type=\"submit\" formaction=\"/oper\" name=\"vol\" value=\"up\">VOL+</button></td>";
    html += "<td><button type=\"submit\" formaction=\"/oper\" name=\"stn\" value=\"next\">STATION</button></td>";
    html += "</tr><tr><td><p>Station URL:</p></td>";
    //html += "<tr>";
    //html += "<td><p>Station URL:</p></td>";
    if (strlen(stn)==0) {
      html += "<td></td>";
    } else {
      html += "<td><font color=\"#ff4500\">Err!</font></td>";
    }
    html += "<td><input type=\"text\" name=\"stnname\" size=\"18\" maxlength=\"23\" value=\"";
    html += stnname;
    html += "\"></td><td>";
    //html += "<td>";
    html += "<input type=\"text\" name=\"stnurl\" size=\"64\" maxlength=\"127\" value=\"";
    html += stnurl;
    html += "\">";
    html += "<button type=\"submit\" formaction=\"/oper\" name=\"stnset\" value=\"set\">SET</button>";
    html += "<button type=\"submit\" formaction=\"/oper\" name=\"stnset\" value=\"save\">SAVE</button>";
    html += "</td></tr></table></form><p><a href=\"/\">Return</a></p></body></html>";
    //html += "</tr>";
    //html += "</table>";
    //html += "</form><p><a href=\"/\">Return</a></p></body></html>";
    //html += "<p><a href=\"/\">Return</a></p>";
    //html += "</body>";
    //html += "</html>";   
    server.send(200, "text/html", html);   
}
void handleNotFound(void)
{
    server.send(404, "text/plain", "Not Found.");
}

void setup() {
    //pinMode(SD_CS, OUTPUT);
    //digitalWrite(SD_CS, HIGH);
    Serial.begin(115200);
    pinMode(LED,OUTPUT);
    pinMode(volPin1, INPUT_PULLUP);
    pinMode(volPin2, INPUT_PULLUP);
    pinMode(intPin, INPUT_PULLUP);
    pinMode(SLEEP, INPUT_PULLUP); // sleep after about 60 min when button was pressed
    digitalWrite(LED,LOW);   // sleep mode indicator
    attachInterrupt(intPin,station_setting, FALLING); // pin 13 to change station
    EEPROM.begin(sizeof(st_eerom));
    EEPROM.get<st_eerom>(0,d_eerom);
    Serial.print("vol: ");    Serial.println(d_eerom.vol);
    if (d_eerom.vol<=0 || d_eerom.vol>=21) {
      volume = 8;  // default
      d_eerom.vol = volume;
    } else volume = d_eerom.vol; // restore
    
    Serial.print("stn: ");    Serial.println(d_eerom.stn);
    if (d_eerom.stn<=0 || d_eerom.stn>=max_station) {
      station = 0;  // default
      d_eerom.stn = station;
    } else station = d_eerom.stn; // restore
    if (d_eerom.stnurl[0]==0 || d_eerom.stnurl[0]==255) {
      strcpy(stnurl,station_url[station]); // use preset
      strcpy(stnname,station_name[station]);
    } else {
      strcpy(stnurl,d_eerom.stnurl); // use saved
      strcpy(stnname,d_eerom.stnname);
    }
    // save current
    EEPROM.put<st_eerom>(0, d_eerom);
    EEPROM.commit();
    event = 0;
    pt2=millis();
    //SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    //SPI.setFrequency(1000000);
    //Wire.begin();
    u8g2.begin();
    //u8x8.setFont(u8x8_font_artossans8_r);
    //SD.begin(SD_CS);
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(ssid.c_str(), password.c_str());  
    //wifiMulti.addAP(ssid2.c_str(), password2.c_str());
    wifiMulti.run();   // It may be connected to strong one
    //u8x8.clear();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    //u8g2.setFont(u8g2_font_mozart_nbp_tr);
    //u8x8.drawString(0,2,"Internet Radio");
    /*if(WiFi.status() != WL_CONNECTED){
        Serial.println("WiFi Error");
        WiFi.disconnect(true);
        wifiMulti.run();
    }*/
    while (true) {
      if(WiFi.status() == WL_CONNECTED){ break; }  // WiFi connect OK then next step
      Serial.println("WiFi Err");
      u8g2.drawStr(0,20,"WiFi Err");
      u8g2.sendBuffer();  
      WiFi.disconnect(true);
      delay(5000);
      wifiMulti.run();
      delay(1000*300);  // Wait for Wifi ready
    }
    //u8g2.drawStr(0,20,"WiFi OK");
    //u8g2.sendBuffer();
    u8g2.drawStr(0,10,"Internet Radio"); 
    u8g2.sendBuffer();  
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(volume); // 0...20
//    audio.connecttoSD("/320k_test.mp3");
//    audio.connecttohost("http://www.wdr.de/wdrlive/media/einslive.m3u");
    bool conn_ok;
    conn_ok = audio.connecttohost(stnurl);  // use current 
//    audio.connecttohost("http://macslons-irish-pub-radio.com/media.asx");
//    audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac"); //  128k aac
//    audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3"); //  128k mp3
//    audio.connecttospeech("Wenn die Hunde schlafen, kann der Wolf gut Schafe stehlen.", "de");
    if (!conn_ok) { // conect failure
      Serial.println("Fail to connect");
      d_eerom.stnname[0] = 0;  // next time use preset 
      d_eerom.stnurl[0] = 0;
      EEPROM.put<st_eerom>(0, d_eerom);
      EEPROM.commit();
      strcpy(stnname,"Not");
    }
    char buf[64];
    sprintf(buf,"%s Connected",stnname);
    u8g2.clearBuffer();
    u8g2.drawStr(0,10,"Internet Radio"); 
    u8g2.drawStr(0,20,buf); 
    u8g2.sendBuffer();  
    //u8x8.drawString(0,2,"Connect: Sakura");
    server.on("/", handleRoot);
    server.on("/oper", handleOper);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.print("IP = ");
    Serial.println(WiFi.localIP());
    titlebuf[0] = 0;
    delay(200);
}

void loop()
{
    server.handleClient();
    int b = digitalRead(volPin2); // digitalread 12
    int a = digitalRead(volPin1); // digitalread 14
    int s = digitalRead(SLEEP);   // digitalread 32
    if (a==0 || b==0) {
      updatevolume(a, b);
      a=1;
      b=1;
    }
    if (a_srv==0 || b_srv==0) { // by server operation
      updatevolume(a_srv, b_srv);
      a_srv=1;
      b_srv=1;
    }
    if (s==0 || s_srv==0) { // button or by server operation
      if (sleepmode) {
        sleepmode=false;
      } else {
        sleepmode =true;
      }
      if (sleepmode) {
        digitalWrite(LED,HIGH);
        timer = timerBegin(0, 40000, true); //timer=500us
        timerAttachInterrupt(timer, &sleep_timer, true);
        timerAlarmWrite(timer, 7200000, false); // one time   100000:50sec 7200000:3600sec
        timerAlarmEnable(timer);
        Serial.println("Sleep timer ON");
      } else {
        digitalWrite(LED,LOW);
        timerAlarmDisable(timer);
        Serial.println("Sleep timer OFF");
      }
      s=1;
      s_srv=1;
      delay(200);  // prevent chattering
    }
    audio.loop();
    if(Serial.available()){ // put streamURL in serial monitor
        audio.stopSong();
        String r=Serial.readString(); r.trim();
        if(r.length()>5) audio.connecttohost(r);
        log_i("free heap=%i", ESP.getFreeHeap());
    }
    if (event==1) { // interrupt of change station request ?
        event = 0; // clear 
        audio.connecttohost(station_url[station]); // change station
        strcpy(stnurl,station_url[station]); // use preset
        strcpy(stnname,station_name[station]);
        EEPROM.get<st_eerom>(0,d_eerom);
        d_eerom.stn = station;
        Serial.println("Clear saved station");
        d_eerom.stnname[0] = 0;  // next time use preset 
        d_eerom.stnurl[0] = 0;
        EEPROM.put<st_eerom>(0, d_eerom);
        EEPROM.commit();
    }
}
// change volume
void updatevolume(uint8_t a, uint8_t b)
{
  if (b==0) { // push vol up
    volume ++;
    if (volume>=21) volume=20;
  } else if (a==0) { // push vol down
    volume --;
    if (volume<=0) volume=0;
  }
  Serial.print("vl:");
  Serial.println(volume);
  audio.setVolume(volume);
  EEPROM.get<st_eerom>(0,d_eerom);
  d_eerom.vol = volume;
  EEPROM.put<st_eerom>(0, d_eerom);
  EEPROM.commit();
  delay(200);  // prevent chattering
}
// int routine
void station_setting(){
  Serial.print("st:");
  Serial.println(station);
  ct2=millis();
  delay(10);  // no effect here
  if ((ct2-pt2)>250) {
    station=station+1;
  }
  pt2=ct2;
  if(station>=max_station){
    station=0;
  }
  event=1;
} 
// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreaminfo(const char *info){
    Serial.print("streaminfo  ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    char buf[64];
    char buf1[64];
    Serial.print("streamtitle ");Serial.println(info);
    sprintf(buf,"%s Connected",stnname);
    sprintf(titlebuf,"%s",info);  // server data
    sprintf(buf1,"Vol:%d Internet Radio",volume);
    u8g2.clearBuffer();
    u8g2.drawStr(0,10,buf1);
    u8g2.drawStr(0,20,buf);
    u8g2.drawStr(0,30,info); 
    u8g2.sendBuffer();  
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}
