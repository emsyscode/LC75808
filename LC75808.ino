/**********************************************************************/
/*This code is not clean and OFFCOURSE will be possible do it better! */ 
/*this is only a group of functions to be used as independent code,   */ 
/*and by this way will be possible to any person can do the changes   */ 
/*of code and see what happen.                                        */
/*The code don't depend of any external library or extenal functions  */
/*complicated.                                                        */
/*I'm let the values to sent as binnary, this allow swap bit by bit   */
/* to is possible test segment by segment without convert to HEX      */
/**********************************************************************/
#define VFD_in 8  // This is the pin number 7 on Arduino UNO
#define VFD_clk 9 // This is the pin number 8 on Arduino UNO
#define VFD_ce 10 // This is the pin number 9 on Arduino UNO

byte Aa, Ab, Ac, Ad, Ae, Af, Ag, Ah, Ai, Aj, Ak, Al, Am, An, Ao, Ap;
byte blockBit = 0x00;

#define BUTTON_PIN 2 //Att check wich pins accept interrupts... Uno is 2 & 3
volatile byte buttonReleased = false;

//ATT: On the Uno and other ATMEGA based boards, unsigned ints (unsigned integers) are the same as ints in that they store a 2 byte value.
//Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647.

//*************************************************//
void setup() {
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_ce, OUTPUT);
  //
  pinMode(13, OUTPUT);
  Serial.begin(115200); // only to debug
  //
  pinMode(BUTTON_PIN, INPUT);
  //Next line is the attach of interruption to pin 2
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
                  buttonReleasedInterrupt,
                  FALLING);
  //
  sendControl_LC75808();
}
void send_char(unsigned char a)
{
 //
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  // the validation of data happen when clk go from LOW to HIGH.
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
  digitalWrite(VFD_ce, LOW); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  digitalWrite(VFD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
  for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(VFD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
    //
    digitalWrite(VFD_ce, HIGH); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  }
}
/*********************************************************************/
// I h've created 3 functions to send bit's, one with strobe, other without strobe and one with first byte with strobe followed by remaing bits.
void send_char_without(unsigned char a)
{
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
  }
}
/*********************************************************************/
void send_char_8bit_stb(unsigned char a)
{
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
 int i = -1;
  data=a;
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
  for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
   i++;
   digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// 
    delayMicroseconds(1);
    if (i==7){
    //Serial.println(i);
    digitalWrite(VFD_ce, HIGH);
    delayMicroseconds(1);
    }  
  }
}
/*******************************************************************/
void sendControl_LC75808(){
//send total of 8 + 24 bits.
/*
• CCB address: ....42H
• D1 to D480: ........ Display data
• KC1 to KC6: ........ Key scan output state setting data
• PC1 to PC4: ........ General-purpose output port state setting data
• CT0 to CT3, CTC: Display contrast setting data
• SC: ...................... Segment on/off control data
• SP: ...................... Normal mode/sleep mode control data
• DT1, DT2:............ Display technique setting data
 */
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); // firts 8 bits is CCB(is 0x4B) address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
//Structure is as follow: KC1,KC2,KC3,KC4,KC5,KC6,PC1,PC2,  PC3,PC4,CT0,CT1,CT2,CT3,CTC,SC,   SP,DT1,DT2,0,0,1,0,0. The last 3 bits is mark as control
//Importante: SP normal mode, SC segment on/off, PC1-PC4 general porpuse,CT0-CT3 & CTC contrast display
send_char_without(0B00000000);//   8:1  KC1,KC2,KC3,KC4,KC5,KC6,PC1,PC2,
send_char_without(0B00000000);//  16:9  PC3,PC4,CT0,CT1,CT2,CT3,CTC,SC,
send_char_without(0B00100000); // 24:17 SP,DT1,DT2,0,0,1,0,0.   The last 3 bits: 100, define it as cmd status dsp 
//               (0B0000xxxx) x adjust contrast, (0Bxxxx0000) General Purpose
//
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void ledGreen(){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); // firts 8 bits is CCB(is 0x4B) address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
//Structure is as follow: KC1,KC2,KC3,KC4,KC5,KC6,PC1,PC2,  PC3,PC4,CT0,CT1,CT2,CT3,CTC,SC,   SP,DT1,DT2,0,0,1,0,0. The last 3 bits is mark as control
//Importante: SP normal mode, SC segment on/off, PC1-PC4 general porpuse,CT0-CT3 & CTC contrast display
send_char_without(0B00000000);//   8:1  KC1,KC2,KC3,KC4,KC5,KC6,PC1,PC2,
send_char_without(0B00000001);//  16:9  PC3,PC4,CT0,CT1,CT2,CT3,CTC,SC,
send_char_without(0B00100000); // 24:17 SP,DT1,DT2,0,0,1,0,0.   The last 3 bits: 100, define it as cmd status dsp 
//               (0B0000xxxx) x adjust contrast, (0Bxxxx0000) General Purpose
//
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void ledRed(){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); // firts 8 bits is CCB(is 0x4B) address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
//Structure is as follow: KC1,KC2,KC3,KC4,KC5,KC6,PC1,PC2,  PC3,PC4,CT0,CT1,CT2,CT3,CTC,SC,   SP,DT1,DT2,0,0,1,0,0. The last 3 bits is mark as control
//Importante: SP normal mode, SC segment on/off, PC1-PC4 general porpuse,CT0-CT3 & CTC contrast display
send_char_without(0B10000000);//   8:1  KC1,KC2,KC3,KC4,KC5,KC6,PC1,PC2,
send_char_without(0B00000000);//  16:9  PC3,PC4,CT0,CT1,CT2,CT3,CTC,SC,
send_char_without(0B00100000); // 24:17 SP,DT1,DT2,0,0,1,0,0.   The last 3 bits: 100, define it as cmd status dsp 
//               (0B0000xxxx) x adjust contrast, (0Bxxxx0000) General Purpose
//
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void ledRedGreen(){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); // firts 8 bits is CCB(is 0x4B) address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
//Structure is as follow: KC1,KC2,KC3,KC4,KC5,KC6,PC1,PC2,  PC3,PC4,CT0,CT1,CT2,CT3,CTC,SC,   SP,DT1,DT2,0,0,1,0,0. The last 3 bits is mark as control
//Importante: SP normal mode, SC segment on/off, PC1-PC4 general porpuse,CT0-CT3 & CTC contrast display
send_char_without(0B10000000);//   8:1  KC1,KC2,KC3,KC4,KC5,KC6,PC1,PC2,
send_char_without(0B00000001);//  16:9  PC3,PC4,CT0,CT1,CT2,CT3,CTC,SC,
send_char_without(0B00100000); // 24:17 SP,DT1,DT2,0,0,1,0,0.   The last 3 bits: 100, define it as cmd status dsp 
//               (0B0000xxxx) x adjust contrast, (0Bxxxx0000) General Purpose
//
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
/*******************************************************************/
void allON_LC75808(){
unsigned char group=0;
//send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
//The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
for(int i=0; i<4;i++){   // Dx until 600 bits
      group=i;
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75808 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    
    send_char_without(0B11111111);  send_char_without(0B11111111); //   1:8       
    send_char_without(0B11111111);  send_char_without(0B11111111); //  17:24    
    send_char_without(0B11111111);  send_char_without(0B11111111); //  33:40     
    send_char_without(0B11111111);  send_char_without(0B11111111); //  49:56    
    send_char_without(0B11111111);  send_char_without(0B11111111); //  65:72    
    send_char_without(0B11111111);  send_char_without(0B11111111); //  81:88    
    send_char_without(0B11111111);  send_char_without(0B11111111); //  96:103   
    send_char_without(0B11111111);   // 112:119  
        switch (group){ //-120:127//Last 3 bits is "DD" data direction, and is used
          case 0: send_char_without(0B00000000); break;
          case 1: send_char_without(0B10000000); break;
          case 2: send_char_without(0B01000000); break;
          case 3: send_char_without(0B11000000); break;
        }
    
    // to mark the 4 groups of 120 bits, 00, 01, 10, 11.
    delayMicroseconds(1);
    digitalWrite(VFD_ce, LOW); // 
    delayMicroseconds(1);
    }
}
/*******************************************************************/
void allOFF_LC75808(){
unsigned char group=0;
//send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
//The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
for(int i=0; i<4;i++){ // Dx until 600 bits
      group=i;
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75878 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    
    send_char_without(0B00000000);  send_char_without(0B00000000); //   1:8      -9:16// 
    send_char_without(0B00000000);  send_char_without(0B00000000); //  17:24    -25:32// 
    send_char_without(0B00000000);  send_char_without(0B00000000); //  33:40    -41:48//  
    send_char_without(0B00000000);  send_char_without(0B00000000); //  49:56    -57:64// 
    send_char_without(0B00000000);  send_char_without(0B00000000); //  65:72    -73:80//  
    send_char_without(0B00000000);  send_char_without(0B00000000); //  81:88    -89:96// 
    send_char_without(0B00000000);  send_char_without(0B00000000); //  96:103  -104:111//  
    send_char_without(0B00000000);   // 112:119  
        switch (group){ //120:127// Last 3 bits is "DD" data direction, and is used
          case 0: send_char_without(0B00000000); break;
          case 1: send_char_without(0B10000000); break;
          case 2: send_char_without(0B01000000); break;
          case 3: send_char_without(0B11000000); break;
        }
   
// to mark the 4 groups of 120 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
 }
}
/*******************************************************************/
void LC75808_00_msg5(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75808 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk0
    send_char_without(0B00000000);  send_char_without(0B00000000); // 0-1           
    send_char_without(0B00000000);  send_char_without(0B00000000); // 2-3      
    send_char_without(0B00000000);  send_char_without(0B11111110); // 4-5      H 
    send_char_without(0B00010000);  send_char_without(0B00010000); // 6-7      
    send_char_without(0B00010000);  send_char_without(0B11111110); // 8-9      
    send_char_without(0B11111110);  send_char_without(0B10010010); // A-B      E 
    send_char_without(0B10010010);  send_char_without(0B10010010); // C-D    
    send_char_without(0B10000010);  send_char_without(0B00000000); // E-F 
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void LC75808_10_msg5(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75808 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk1
    send_char_without(0B11111110);  send_char_without(0B10000000); // 0-1     L 
    send_char_without(0B10000000);  send_char_without(0B10000000); // 2-3     
    send_char_without(0B10000000);  send_char_without(0B11111110); // 4-5     L
    send_char_without(0B10000000);  send_char_without(0B10000000); // 6-7      
    send_char_without(0B10000000);  send_char_without(0B10000000); // 8-9     
    send_char_without(0B11111110);  send_char_without(0B10000010); // A-B     O   
    send_char_without(0B10000010);  send_char_without(0B10000010); // C-D     
    send_char_without(0B11111110);  send_char_without(0B10000000); // E-F   
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void LC75808_01_msg5(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75808 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk2
    send_char_without(0B00000000);  send_char_without(0B00000000); // 0-1         
    send_char_without(0B00000000);  send_char_without(0B00000000); // 2-3       
    send_char_without(0B00000000);  send_char_without(0B00000000); // 4-5       
    send_char_without(0B00000000);  send_char_without(0B00000000); // 6-7      
    send_char_without(0B00000000);  send_char_without(0B00000000); // 8-9       
    send_char_without(0B00000000);  send_char_without(0B00000000); // A-B       
    send_char_without(0B00000000);  send_char_without(0B00000000); // C-D     
    send_char_without(0B00000000);  send_char_without(0B01000000); // E-F   
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void LC75808_11_msg5(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75808 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk3
    send_char_without(0B00000000);  send_char_without(0B00000000); // 0-1      
    send_char_without(0B00000000);  send_char_without(0B00000000); // 2-3      
    send_char_without(0B00000000);  send_char_without(0B00000000); // 4-5      
    send_char_without(0B00000000);  send_char_without(0B00000000); // 6-7    
    send_char_without(0B00000000);  send_char_without(0B00000000); // 8-9      
    send_char_without(0B00000000);  send_char_without(0B00000000); // A-B     
    send_char_without(0B00000000);  send_char_without(0B00000000); // C-D     
    send_char_without(0B00000000);  send_char_without(0B11000000); // E-F   
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
//
void LC75808_00_msgWheel(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75808 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk0
    send_char_without(0B00000000);  send_char_without(0B00000000); // 0-1   8:1   -  16:9     
    send_char_without(0B00000000);  send_char_without(0B00000000); // 2-3  24:17  -  32:25    
    send_char_without(0B00000000);  send_char_without(0B00000000); // 4-5  33:40  -  48:41  
    send_char_without(0B00000000);  send_char_without(0B00000000); // 6-7  56:49  -  64:57    
    send_char_without(0B00000000);  send_char_without(0B00000000); // 8-9  72:65  -  80:73   
    send_char_without(0B00000000);  send_char_without(0B00000001); // A-B  88:81  -  96:89  
    send_char_without(0B00000001);  send_char_without(0B00000001); // C-D 104:97  - 112:105
    send_char_without(0B00000000);  send_char_without(0B00000000); // E-F 120:113 - 128:121
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void LC75808_10_msgWheel(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75808 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk1
    send_char_without(0B00000001);  send_char_without(0B00000001); // 128:121  136-129     
    send_char_without(0B00000001);  send_char_without(0B00000000); // 144:137  152-145    
    send_char_without(0B00000000);  send_char_without(0B00000001); // 160:153  168-161
    send_char_without(0B00000001);  send_char_without(0B00000001); // 176:169  184-177    
    send_char_without(0B00000000);  send_char_without(0B00000000); // 192:185  200-193   
    send_char_without(0B00000000);  send_char_without(0B00000000); // 208:201  216:209   
    send_char_without(0B00000000);  send_char_without(0B00000000); // 217:210  225-218    
    send_char_without(0B00000000);  send_char_without(0B10000000); // 233:226   
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void LC75808_01_msgWheel(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75808 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk2                                                            
    send_char_without(0B00000000);  send_char_without(0B00000000); // 241-234   249:242    
    send_char_without(0B00000000);  send_char_without(0B00000000); //     
    send_char_without(0B00000000);  send_char_without(0B00000000); //   
    send_char_without(0B00000000);  send_char_without(0B00000000); //    
    send_char_without(0B00000000);  send_char_without(0B00000000); //    
    send_char_without(0B00000000);  send_char_without(0B00000000); //   
    send_char_without(0B00000000);  send_char_without(0B00000000); //  
    send_char_without(0B00000000);  send_char_without(0B01000000); //  
delayMicroseconds(1);                                                 
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void LC75808_11_msgWheel(void){
  digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75878 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk0
    send_char_without(0B00000000);  send_char_without(0B00000000); // 0-1    1:8     
    send_char_without(0B00000000);  send_char_without(0B00000000); // 2-3  17:24     
    send_char_without(0B00000000);  send_char_without(0B00000000); // 4-5  33:40     
    send_char_without(0B00000000);  send_char_without(0B00000000); // 6-7  49:56    
    send_char_without(0B00000000);  send_char_without(0B00000000); // 8-9  65:72     
    send_char_without(0B00000000);  send_char_without(0B00000000); // A-B  81:88   
    send_char_without(0B00000000);  send_char_without(0B00000000); // C-D  96:103 
    send_char_without(0B00000000);  send_char_without(0B11000000); // E-F 112:119
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
//
void LC75808_00_msg1(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75878 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk0
    send_char_without(0B00000000);  send_char_without(0B00000000); // 0-1   "Space"... empty    
    send_char_without(0B00000000);  send_char_without(0B00000000); // 2-3     
    send_char_without(0B00000000);  send_char_without(0B11111110); // 4-5   F 
    send_char_without(0B00010010);  send_char_without(0B00010010); // 6-7      
    send_char_without(0B00010010);  send_char_without(0B00010010); // 8-9      
    send_char_without(0B11111110);  send_char_without(0B10000010); // A-B   O    
    send_char_without(0B10000010);  send_char_without(0B10000010); // C-D     
    send_char_without(0B11111110);  send_char_without(0B00000000); // E-F 
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void LC75808_10_msg1(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75878 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk1
    send_char_without(0B11111110);  send_char_without(0B10000000); // 0-1     L
    send_char_without(0B10000000);  send_char_without(0B10000000); // 2-3     
    send_char_without(0B10000000);  send_char_without(0B11111110); // 4-5     K
    send_char_without(0B00010000);  send_char_without(0B00101000); // 6-7     
    send_char_without(0B01000100);  send_char_without(0B10000010); // 8-9       
    send_char_without(0B10011110);  send_char_without(0B10010010); // A-B     S
    send_char_without(0B10010010);  send_char_without(0B10010010); // C-D     
    send_char_without(0B11110010);  send_char_without(0B10000000); // E-F 
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void LC75808_01_msg1(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75878 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk2
    send_char_without(0B00000000);  send_char_without(0B00000000); // 0-1   
    send_char_without(0B00000000);  send_char_without(0B00000000); // 2-3      
    send_char_without(0B00000000);  send_char_without(0B00000000); // 4-5      
    send_char_without(0B00000000);  send_char_without(0B00000000); // 6-7      
    send_char_without(0B00000000);  send_char_without(0B00000000); // 8-9       
    send_char_without(0B00000000);  send_char_without(0B00000000); // A-B       
    send_char_without(0B00000000);  send_char_without(0B00000000); // C-D     
    send_char_without(0B00000000);  send_char_without(0B01000000); // E-F 
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
void LC75808_11_msg1(void){
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
delayMicroseconds(1);
// On the 75878 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
    //blk3
    send_char_without(0B00000000);  send_char_without(0B00000000); // 0-1        
    send_char_without(0B00000000);  send_char_without(0B00000000); // 2-3      
    send_char_without(0B00000000);  send_char_without(0B00000000); // 4-5     
    send_char_without(0B00000000);  send_char_without(0B00000000); // 6-7      
    send_char_without(0B00000000);  send_char_without(0B00000000); // 8-9       
    send_char_without(0B00000000);  send_char_without(0B00000000); // A-B      
    send_char_without(0B00000000);  send_char_without(0B00000000); // C-D     
    send_char_without(0B00000000);  send_char_without(0B11000000); // E-F 
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
//
void wheel_0(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00};
uint8_t anime1[16] = {0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};

char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
void wheel_1(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00};
uint8_t anime1[16] = {0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};
  
char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
void wheel_2(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
uint8_t anime1[16] = {0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};
  
char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
void wheel_3(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00};
uint8_t anime1[16] = {0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};

char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
void wheel_4(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t anime1[16] = {0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};
char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
void wheel_5(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t anime1[16] = {0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};

char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
void wheel_6(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t anime1[16] = {0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};

char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
void wheel_7(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t anime1[16] = {0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};

char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
void wheel_8(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t anime1[16] = {0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};

char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
void wheel_9(){
uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t anime1[16] = {0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};

//uint8_t anime0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00};
//uint8_t anime1[16] = {0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
//uint8_t anime2[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40};
//uint8_t anime3[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0};
char c = 0x00;
unsigned char mask = 1; //our bitmask
uint8_t count=0x00;

// On the 75808 the message have first 120+8 (16*8) bits
    for (unsigned int arr=0; arr<4; arr++){
    count=0x00;
    digitalWrite(VFD_ce, LOW); //
    delayMicroseconds(1);
    send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
    delayMicroseconds(1);
        for( uint8_t pos = 0; pos <16; pos++){
          if (pos==15){
              switch  (arr){
                case 0: send_char_without(anime0[15]); break;
                case 1: send_char_without(anime1[15]); break;
                case 2: send_char_without(anime2[15]); break;
                case 3: send_char_without(anime3[15]); break;
              }
              Serial.print(mask, BIN);Serial.print(" B "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
           }
            else{
                    switch  (arr){
                      case 0: send_char_without(anime0[pos]); break;
                      case 1: send_char_without(anime1[pos]); break;
                      case 2: send_char_without(anime2[pos]); break;
                      case 3: send_char_without(anime3[pos]); break;
                    }
              }  
          count++;
          Serial.print(mask, BIN);Serial.print(" : "); Serial.print(pos, DEC); Serial.print(" - ");Serial.println( count, DEC);
          delay(1);      
        }
        delayMicroseconds(1);
        digitalWrite(VFD_ce, LOW); // 
        delayMicroseconds(1);
        delay(1);
      }
}
//
void searchOfSegments() {
  int group = 0x00;
  byte nBit = 0x00;
  byte nMask = 0b00000001;
  unsigned int block = 0;
  unsigned int nSeg = 0x00;

  Serial.println();
  Serial.println("We start the test of segments!");
  Serial.println("Make OFF of all segments!");
  //
  for (block = 0; block < 4; block++) { //This is the last 1 bit's
    for ( group = 0; group < 15; group++) { // Do until n bits 15*8 bits (The last byte is fixed data and define the burst block and is inserted at: segments();)
      //for(int nBit=0; nBit<8; nBit++){
      for (nMask = 0b00000001; nMask > 0; nMask <<= 1) {
        Aa = 0x00; Ab = 0x00; Ac = 0x00; Ad = 0x00; Ae = 0x00; Af = 0x00; Ag = 0x00; Ah = 0x00; Ai = 0x00; Aj = 0x00; Ak = 0x00; Al = 0x00; Am = 0x00; An = 0x00; Ao = 0x00; Ap = 0x00;
        switch (group) {
          case  0: Aa = nMask; break;
          case  1: Ab = nMask; break;
          case  2: Ac = nMask; break;
          case  3: Ad = nMask; break;
          case  4: Ae = nMask; break;
          case  5: Af = nMask; break;
          case  6: Ag = nMask; break;
          case  7: Ah = nMask; break;
          case  8: Ai = nMask; break;
          case  9: Aj = nMask; break;
          case 10: Ak = nMask; break;
          case 11: Al = nMask; break;
          case 12: Am = nMask; break;
          case 13: An = nMask; break;
          case 14: Ao = nMask; break;
          case 15: Ap = nMask; break;//This "Ap" are only used on the function: segments();, because this is 16º byte!
        }

        nSeg++;
        if ((nSeg >= 0) && (nSeg < 120)) {
          blockBit = 0;
        }
        if ((nSeg >= 121) && (nSeg < 240)) {
          blockBit = 1;
        }
        if ((nSeg >= 241) && (nSeg < 360)) {
          blockBit = 2;
        }
        if ((nSeg >= 361) && (nSeg < 480)) {
          blockBit = 3;
        }
        //This start the control of button to allow continue teste!
        while (1) {
          if (!buttonReleased) {
            delay(200);
          }
          else {
            delay(15);
            buttonReleased = false;
            break;
          }
        }
        delay(50);
        segments();
        Serial.print(nSeg, DEC); Serial.print(", group: "); Serial.print(group, DEC); Serial.print(", BlockBit: "); Serial.print(blockBit, HEX); Serial.print(", nMask: "); Serial.print(nMask, BIN); Serial.print("   \t");
        Serial.print(Ap, HEX); Serial.print(", ");
        Serial.print(Ao, HEX); Serial.print(", "); Serial.print(An, HEX); Serial.print(", "); Serial.print(Am, HEX); Serial.print(", "); Serial.print(Al, HEX); Serial.print(", ");
        Serial.print(Ak, HEX); Serial.print(", "); Serial.print(Aj, HEX); Serial.print(", "); Serial.print(Ai, HEX); Serial.print(", "); Serial.print(Ah, HEX); Serial.print(", ");
        Serial.print(Ag, HEX); Serial.print(", "); Serial.print(Af, HEX); Serial.print(", "); Serial.print(Ae, HEX); Serial.print(", "); Serial.print(Ad, HEX); Serial.print(", ");
        Serial.print(Ac, HEX); Serial.print(", "); Serial.print(Ab, HEX); Serial.print(", "); Serial.print(Aa, HEX); Serial.print("; ");

        Serial.println();
        delay (250);
      }
    }
  }
}
void segments() {
  //Bit function:
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(1);
  send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address!
  delayMicroseconds(1);
  //
  send_char_without(~Aa & 0xFF);  send_char_without(~Ab & 0xFF);  //   8:1     -16:9//
  send_char_without(~Ac & 0xFF);  send_char_without(~Ad & 0xFF);  //  24:17    -32:25//
  send_char_without(~Ae & 0xFF);  send_char_without(~Af & 0xFF);  //  40:33    -48:41//
  send_char_without(~Ag & 0xFF);  send_char_without(~Ah & 0xFF);  //  56:49    -64:57//
  send_char_without(~Ai & 0xFF);  send_char_without(~Aj & 0xFF);  //  72:65    -80:73//
  send_char_without(~Ak & 0xFF);  send_char_without(~Al & 0xFF);  //  88:81    -96:89//
  send_char_without(~Am & 0xFF);  send_char_without(~An & 0xFF);  // 104:97   -112:105//
  send_char_without(~Ao & 0xFF); //120:113 
  
  //The next switch finalize the burst of bits -52:49//
  switch (blockBit) { //Last bit is data direction, and is used to mark the 2 groups of 53 bits, 0, 1.
    //Bits os control: DP, DQ, and Last bit to define group.
    case 0: send_char_without(0B00000000 | Ap); break; //Block 0
    case 1: send_char_without(0B10000000 | Ap); break; //Block 1
    case 2: send_char_without(0B01000000 | Ap); break; //Block 2
    case 3: send_char_without(0B11000000 | Ap); break; //Block 3
  }
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); //
}
void wheel(){
  unsigned int t = 10;
  for (unsigned int n=0; n<2;n++){
    wheel_1();
    delay(t);
    wheel_2();
    delay(t);
    wheel_3();
    delay(t);
    wheel_4();
    delay(t);
    wheel_5();
    delay(t);
    wheel_6();
    delay(t);
    wheel_7();
    delay(t);
    wheel_8();
    delay(t);
    wheel_9();
    delay(t);
      }
 wheel_0();
delay(t);
}
void loop() {
sendControl_LC75808();
delay(1); 
LC75808_00_msgWheel();
LC75808_10_msgWheel();
LC75808_01_msgWheel();
LC75808_11_msgWheel(); 
ledGreen();
delay(2000);
ledRed();
delay(1000);
ledRedGreen(); //This line active the both LED's, Green and Red, but the red is more bright and make shadown over green!
delay(1000);
//
allON_LC75808(); // All on
delay(2000);
allOFF_LC75808(); // All off
delay(2000);
//
LC75808_00_msg5();
LC75808_10_msg5();
LC75808_01_msg5();
LC75808_11_msg5();
delay(2000);
//
LC75808_00_msg1(); 
LC75808_10_msg1(); 
LC75808_01_msg1(); 
LC75808_11_msg1(); 
delay(1000);
//
for(unsigned int s=0; s<8; s++){
  wheel();
}
//
allON_LC75808(); // All on
searchOfSegments(); //Uncomment this line if you want run to find segments
}
void buttonReleasedInterrupt() {
  buttonReleased = true; // This is the line of interrupt button to advance one step on the search of segments!
}
