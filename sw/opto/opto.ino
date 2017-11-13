#include <Wire.h>
#include "Adafruit_MCP23017.h"


#include <SPI.h>          // needed for Arduino versions later than 0018
#include <Ethernet2.h>
#include <EthernetUdp2.h> // UDP library from: bjoern@cs.stanford.edu 12/30/2008

#define DEBUG   0           // debug flag to printout all pins and their value
#define DEBUG_T 1           // collects and prints timeing data
#define TX_FREQ 1000          // udp tx freq in Hz
#define D_BLINK 500         // number of cycles to hold led high
#define D_PRINT 9000        // number of cycles between printing of timeing debug

Adafruit_MCP23017 mcp;   // Adafruit libary for the MCP23017 

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = 
{
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(192, 168, 1, 89);     // local ip, gateway and subnet
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

unsigned int localPort = 8888;      // local port to listen on

IPAddress ip_r1(192, 168, 1, 118);    // remote ip addresses
IPAddress ip_r2(192, 168, 1, 22);     // remote ip addresses


#if defined(ESP32)
  #define WIZ_CS 33
#endif

// buffers for receiving and sending data
char packetBuffer[ 2 ]; //buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup() 
{  
  mcp.begin();      // use default address 0

  mcp.pinMode( 0, INPUT); // setup discrete pins
  mcp.pinMode( 1, INPUT);
  mcp.pinMode( 2, INPUT);
  mcp.pinMode( 3, INPUT);
  mcp.pinMode( 4, INPUT);
  mcp.pinMode( 5, INPUT);
  mcp.pinMode( 6, INPUT);
  mcp.pinMode( 7, INPUT);
  mcp.pinMode( 8, INPUT);
  mcp.pinMode( 9, INPUT);
  mcp.pinMode(10, INPUT);
  mcp.pinMode(11, INPUT);
  mcp.pinMode(12, INPUT);
  mcp.pinMode(13, INPUT);
  mcp.pinMode(14, INPUT);
  mcp.pinMode(15, INPUT);

  mcp.pullUp(0, HIGH);  // turn on 100K internal pullup for each pin
  mcp.pullUp(1, HIGH); 
  mcp.pullUp(2, HIGH); 
  mcp.pullUp(3, HIGH); 
  mcp.pullUp(4, HIGH); 
  mcp.pullUp(5, HIGH); 
  mcp.pullUp(6, HIGH); 
  mcp.pullUp(7, HIGH); 
  mcp.pullUp(8, HIGH); 
  mcp.pullUp(9, HIGH); 
  mcp.pullUp(10, HIGH); 
  mcp.pullUp(11, HIGH); 
  mcp.pullUp(12, HIGH); 
  mcp.pullUp(13, HIGH); 
  mcp.pullUp(14, HIGH); 
  mcp.pullUp(15, HIGH); 

  pinMode(13, OUTPUT);  // setup pint 13 led output


#if defined(WIZ_RESET)
  pinMode(WIZ_RESET, OUTPUT);
  digitalWrite(WIZ_RESET, HIGH);
  delay(100);
  digitalWrite(WIZ_RESET, LOW);
  delay(100);
  digitalWrite(WIZ_RESET, HIGH);
#endif

  Serial.begin(115200); // setup serial port
  delay(1000);
  
  Serial.println("\nDiscrete setup complete");
  Serial.println("\nStarting Network");

  Ethernet.init(WIZ_CS);
  
  // give the ethernet module time to boot up:
  delay(1000);

  // start the Ethernet and UDP:
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);

  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
}



void loop() 
{

#if DEBUG_T
  unsigned long m_t1, m_t2, m_t3, m_t4;
#endif

  static unsigned long t_last    = 0;
  static unsigned long t_current = 0;
  static unsigned long t_sum     = 0;
  static unsigned long td_last   = 0;
  
  static union                       // union holds discrete data
  {
    unsigned short value;            // allows access to all discrete data at once.
    struct 
    {
      unsigned char d9  : 1; // J3
      unsigned char d10 : 1;
      unsigned char d11 : 1;
      unsigned char d12 : 1;
      
      unsigned char d13 : 1;  // J4
      unsigned char d14 : 1;
      unsigned char d15 : 1;
      unsigned char d16 : 1;
      
      unsigned char d1  : 1; // J1
      unsigned char d2  : 1;
      unsigned char d3  : 1;
      unsigned char d4  : 1;
      
      unsigned char d5  : 1; //J2
      unsigned char d6  : 1;
      unsigned char d7  : 1;
      unsigned char d8  : 1;
    } bits;
  } var;

  static unsigned short last;           // holds the last state of the discretes
  static int            change_count;   // counter used to blink led on discrete change
  static int            print_delay = D_PRINT;
  static int            tx_count    = 0;
  static int            tx_adj      = 0;
  
  const double          freq_ms  = ( 1.0 / TX_FREQ ) * 1000; // period in ms 
  const unsigned int    tx_limit = freq_ms * 1000;           // used to set the the transmit frequencey of the data


  if( change_count > 0 )
  {
    change_count --;          // decrement count each cycle
  }
  else if( change_count == 0 )
  {
    digitalWrite(13, LOW);    // turn the LED off
    change_count = -1;
  }

  unsigned int t_temp =  micros();
  if( ( t_temp - t_last - tx_adj ) >= tx_limit )
  {   


#if DEBUG_T
    m_t1=micros();
#endif

    var.value =  ~ mcp.readGPIOAB();
  
#if DEBUG_T
    m_t2=micros();
#endif
   
    if( var.value != last)
    {
      Serial.print( "Change : " );
      Serial.println( (0x10000 | var.value), BIN);
      digitalWrite(13, HIGH);        // turn the LED on
      change_count = D_BLINK;        // number of cycles to hold high
      
#if DEBUG
      Serial.print( "\n" );
      Serial.print( "\nJ1\n" );
      Serial.print( " d1  : " ); Serial.println(var.bits.d1);
      Serial.print( " d2  : " ); Serial.println(var.bits.d2);
      Serial.print( " d3  : " ); Serial.println(var.bits.d3);
      Serial.print( " d4  : " ); Serial.println(var.bits.d4);
      
      Serial.print( "\nJ2\n" );
      Serial.print( " d5  : " ); Serial.println(var.bits.d5);
      Serial.print( " d6  : " ); Serial.println(var.bits.d6);
      Serial.print( " d7  : " ); Serial.println(var.bits.d7);
      Serial.print( " d8  : " ); Serial.println(var.bits.d8);
  
      Serial.print( "\nJ3\n" );
      Serial.print( " d9  : " ); Serial.println(var.bits.d9);
      Serial.print( " d10 : " ); Serial.println(var.bits.d10);
      Serial.print( " d11 : " ); Serial.println(var.bits.d11);
      Serial.print( " d12 : " ); Serial.println(var.bits.d12);
  
      Serial.print( "\nJ4\n" );
      Serial.print( " d13 : " ); Serial.println(var.bits.d13);
      Serial.print( " d14 : " ); Serial.println(var.bits.d14);
      Serial.print( " d15 : " ); Serial.println(var.bits.d15);
      Serial.print( " d16 : " ); Serial.println(var.bits.d16);
#endif
    }
  
    last = var.value;
    
#if DEBUG_T
    m_t3=micros();
#endif
    
    // send data to remote 1
    Udp.beginPacket(ip_r1, 55000);
    Udp.write( (char *) &var.value, 2);
    Udp.endPacket();
  
    // send data to remote 2
    //Udp.beginPacket(ip_r2, 55001);
    //Udp.write( (char *) &var.value, 2);
    //Udp.endPacket();

    int temp_tx_adj = ( int(tx_limit) - int(td_last) );

    tx_adj = tx_adj + (temp_tx_adj * 0.1);
  
    t_current = micros();
    td_last   = t_current - t_last;
    t_sum    += td_last;
    t_last    = t_current;
    
    tx_count ++;

#if DEBUG_T
    m_t4=micros();
#endif


#if DEBUG_T
      
    if( print_delay <= 0 )
    {
      unsigned long t_delta = t_sum / tx_count;
      
      t_sum    = 0;
      tx_count = 0;
  
      
      Serial.print("\n TX_FREQ: ");
      Serial.print(TX_FREQ);
  
      Serial.print("  freq_ms: ");
      Serial.print(freq_ms);
  
      Serial.print("  tx_limit: ");
      Serial.print(tx_limit);

      Serial.print(" us  t_delta: ");
      Serial.print( t_delta );
      
      Serial.print(" us  td_last: ");
      Serial.print( td_last );
      Serial.print(" us\n");
      
      Serial.print("\n Delta send : ");
      Serial.print( t_delta );
      Serial.print(" us   Est Freq  : ");
      Serial.print( 1.0 / ( t_delta / 1000.0 / 1000.0 ) );   
  
      Serial.print("\n  Delta I/O : ");
      Serial.print( (m_t2 - m_t1) );
    
      Serial.print(" us     Delta UDP : ");
      Serial.print( (m_t4 - m_t3) );
    
      Serial.print(" us  Total Delta : ");
      Serial.print( (m_t4 - m_t1) / 1000.0 );
      Serial.print(" ms");
    
      Serial.print("\n\n");
  
      print_delay = D_PRINT;
    }
  
  }

  print_delay--;
  
#endif

}




