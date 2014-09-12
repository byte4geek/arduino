#include <EtherCard.h>

#define LED1PIN  2
#define LED2PIN  3

static byte mymac[] = {0x00,0x19,0xCB,0xF4,0x03,0x01};
byte Ethernet::buffer[700];

boolean led1Status;
boolean led2Status;

void setup () {
 
  Serial.begin(57600);
  Serial.println("2 WebRelays DHCP");
 
  if (!ether.begin(sizeof Ethernet::buffer, mymac, 10))
    Serial.println( "Accesso fallito all'Ethernet Shield");
 else
   Serial.println("Ethernet Shield initializzato");
 
  if (!ether.dhcpSetup())
    Serial.println("Impossibile configurare i parametri DHCP");
  else
    Serial.println("Parametri DHCP configurati");
 
  ether.printIp("IP Address:\t", ether.myip);
  ether.printIp("Netmask:\t", ether.netmask);
  ether.printIp("Gateway:\t", ether.gwip);
   Serial.println();
  
  pinMode(LED1PIN, OUTPUT);
  pinMode(LED2PIN, OUTPUT);
  
  digitalWrite(LED1PIN, LOW);
  digitalWrite(LED2PIN, LOW);
  
  led1Status = false;
  led2Status = false;  
}
  
void loop() {
 
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if(pos) {
    
      if(strstr((char *)Ethernet::buffer + pos, "GET /?RELAY1") != 0) {
        led1Status = !led1Status;
        digitalWrite(LED1PIN, led1Status);
        Serial.println("Ricevuto comando per Rele' 1");  
      }
      
      if(strstr((char *)Ethernet::buffer + pos, "GET /?RELAY2") != 0) {
        led2Status = !led2Status;
        digitalWrite(LED2PIN, led2Status);
        Serial.println("Ricevuto comando per Rele' 2");        
      }
      
      BufferFiller bfill = ether.tcpOffset();
      bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\n"
                        "Content-Type: text/html\r\nPragma: no-cache\r\n\r\n"
                        "<html><head><meta name='viewport' content='width=200px'/></head><body>"
                        "<div style='position:absolute;width:200px;height:200px;top:1%;left:50%;margin:5px 0 0 -100px'>"
                        "<div style='font:bold 18px verdana;text-align:center'>Web Rele'</div>"
                        "<br><div style='text-align:center'>"));

      if(led1Status) bfill.emit_p(PSTR("<a href=\"/?RELAY1\"><img src=\"http://www.byte4geek.com/images/arduino/butON.png\"></a><br>Stato Rele' 1 OFF<br>"));
      else bfill.emit_p(PSTR("<a href=\"/?RELAY1\"><img src=\"http://www.byte4geek.com/images/arduino/butOFF.png\"></a><br>Stato rele' 1 ON<br>"));
      
      if(led2Status) bfill.emit_p(PSTR("<br><a href=\"/?RELAY2\"><img src=\"http://www.byte4geek.com/images/arduino/butON.png\"></a><br>Stato Rele' 2 OFF"));
      else bfill.emit_p(PSTR("<br><a href=\"/?RELAY2\"><img src=\"http://www.byte4geek.com/images/arduino/butOFF.png\"></a><br>Stato rele' 2 ON"));      

      bfill.emit_p(PSTR("<br><a href=\"/\">Controlla lo stato dei Rele'</a></div></div></body></html>"));
      ether.httpServerReply(bfill.position());
  }
 }
