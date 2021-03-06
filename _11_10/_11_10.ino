
 /* sim  khi chờ dùng dòng 10mA
  *  khi hoạt động từ 100mA-2a
  *  neo khi hoạt động dòng 45mA
  *  MPU hoat dong vs dong 3,6mA
  */
  
  // include các thư viện
#include <avr/interrupt.h>
#include "I2Cdev.h"
#include <Wire.h>
// cac loại led indication

#define ledint 13
#define ledtimer 10
#define ledinterrup 12
#define ledsms 11
//#include "TimerOne.h"
// các loại biến cho quá trình di chuyen
// biến hồng ngoại:
unsigned int irled =8; // output cua cam bien hong ngoai
unsigned int  state;
// BIEN FRS
int fsrPin = A0;     // noi force vs chan A0
int fsrReading;     // gia tri analog cua force
long fsrForce; // gia tri luc do duoc
// bien cho MPU

int gyro_x, gyro_y, gyro_z;
long acc_x, acc_y, acc_z, acc_total_vector;
int temperature;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
long loop_timer;
  
 unsigned long dtime;

volatile float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
boolean set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output;
// biến cho GPS
unsigned int updates;
unsigned int failedUpdates;
unsigned int pos;
unsigned int stringplace = 0;
unsigned int GPS=0;
int cuttran=5;
String timeUp;
String nmea[15];
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};
String user="01257266491";
// bien cho quá trình truyền thông tin
// Truyền qua GPRS
// biến lệnh AT
// bien cho leh AT
unsigned int AT=0;
unsigned int GPRS1=0;
unsigned int HTTP=0;
unsigned int GET=0;
unsigned int login=0;
unsigned int i;
// biến cho phương thức TCP/HTTP
 //String URL="http://lamhust.site88.net//GPS.php?";  
 String URL="http://phanhost.netau.net//GPS.php?";  
//String URL="http://unique-bk-server.herokuapp.com//accidents/new?";
 char *httppara;
// biến reset
int pinRest=4;
// biến số lần lặp 1 command
const unsigned int limit=2;
// bien truyền SMS
// bien cho CMT
char RcvdMsg[200] = "";
int RcvdCheck = 0;
int RcvdConf = 0;
int index = 0;
int RcvdEnd = 0;
char MsgMob[15];
char MsgTxt[50];
int MsgLength = 0;
unsigned int done=0; //0 la chua setup xong,1 la setup xong
// bien cho smstext
char number[50]="";
//char num[]="+841682740718";
char pass[]="GPS";
char text[]="nite nite";
char money[]="USD";
char account[]="VND";
String link="www.goolge.com/maps/place/";
char* httpparatext; 
// bien cho nut bam sos
 unsigned int fuck=0;
// bien cho bluetooth
volatile unsigned int call;
volatile unsigned int ignore;
volatile unsigned int off;
volatile int tt=0;
//bool i=0;
void setup() {
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  pinMode(ledint,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(cuttran,OUTPUT);
  pinMode(pinRest,OUTPUT);
  //digitalWrite(pinRest,HIGH);
  pinMode(irled, INPUT);
  pinMode(3, INPUT_PULLUP); // sử dụng điện trở kéo lên cho chân số 2, ngắt 0
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2),interrup,LOW); // gọi hàm tatled liên tục khi còn nhấn nút  1
  attachInterrupt(digitalPinToInterrupt(3),call_init,LOW); // gọi hàm tatled liên tục khi còn nhấn nút  1
  
//digitalWrite(cuttran,HIGH);

  Wire.begin();                                                        //Start I2C as master
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200); // cong giao tiep vs sim
  Serial3.begin(9600); // cong giao tiep vs GPS
 
  reset_on();
  setup_sim();
  delaymillis(500);
  setup_mpu_6050_registers();                                          //Setup the registers of the MPU-6050 (500dfs / +/-8g) and start the gyro

//Set digital output 13 high to indicate startup
 // Serial.println("Wellcome to loop");
 for (int cal_int = 0; cal_int < 2000 ; cal_int ++) {                 //Run this code 2000 times
   if (cal_int % 125 == 0) led();// Serial.print(".");                       //Print a dot on the Serail every 125 readings
    //digitalWrite(13,HIGH);
    read_mpu_6050_data();                                              //Read the raw acc and gyro data from the MPU-6050
    gyro_x_cal += gyro_x;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
    gyro_y_cal += gyro_y;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
    gyro_z_cal += gyro_z;                                              //Add the gyro z-axis offset to the gyro_z_cal variable
    delay(3);                                                         //Delay 3us to simulate the 250Hz program loop
  }
  gyro_x_cal /= 2000;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
  gyro_y_cal /= 2000;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
  gyro_z_cal /= 2000;                                                  //Divide the gyro_z_cal variable by 2000 to get the avarage offset
  //PRR0|=(1<<PRTIM1);                                                 //Reset the loop timer
//Serial.println("Starting...");
digitalWrite(ledint,HIGH);
loop_timer = micros();
//timer_init(); 
 //Timer1.initialize(1000000); 
//Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt 
    //PRR0|=(1<<PRTIM1);
         
 
   
}
 


 

void loop() {
 read_mpu_6050_data();                                                //Read the raw acc and gyro data from the MPU-6050
   
  gyro_x -= gyro_x_cal;                                                //Subtract the offset calibration value from the raw gyro_x value
  gyro_y -= gyro_y_cal;                                                //Subtract the offset calibration value from the raw gyro_y value
  gyro_z -= gyro_z_cal;                                                //Subtract the offset calibration value from the raw gyro_z value
  
  //Gyro angle calculations
  //0.0000611 = 1 / (250Hz * 65.5)
  angle_pitch += gyro_x * 0.0000611;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_roll += gyro_y * 0.0000611;                                    //Calculate the traveled roll angle and add this to the angle_roll variable
  
  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  angle_pitch += angle_roll * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
  angle_roll -= angle_pitch * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the pitch angle to the roll angel
  
  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));  //Calculate the total accelerometer vector
  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;       //Calculate the roll angle
  
  //Place the MPU-6050 spirit level and note the values in the following two lines for calibration
  angle_pitch_acc -= 0.0;                                              //Accelerometer calibration value for pitch
  angle_roll_acc -= 0.0;                                               //Accelerometer calibration value for roll

  if(set_gyro_angles){                                                 //If the IMU is already started
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else{                                                                //At first start
    angle_pitch = angle_pitch_acc;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    angle_roll = angle_roll_acc;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = true;                                            //Set the IMU started flag
  }
  
  //To dampen the pitch and roll angles a complementary filter is used
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value
  while(micros() - loop_timer < 4000);                                 //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  loop_timer = micros();
  fsrReading = analogRead(fsrPin);
  forceCal(fsrReading);
// RecSMS();
  state=digitalRead(irled);
Serial.print(angle_pitch); Serial.print("\t"); // 
Serial.print(angle_roll); Serial.print("\t"); // 
Serial.print(fsrReading); Serial.print("\t"); // 
 Serial.print(fsrForce); Serial.print("\t"); //   
 Serial.print(state); Serial.print("\t"); // 
Serial.print("\n");

//Serial.flush();
 while(fuck==1){
 SOS();
 TCNT1=62500;
    fuck=0;
 }

while(call==1){
  unsigned int lop=0;// Serial1.begin(115200);
  unsigned int t=0;
  while(t==0){
   delaymillis(400);
   while(ovc("AT#CE","IG",1000)!=1&&lop<5){
    lop++;
   }
   delaymillis(400);
   if (lop<limit){
   lop=0;
   while(ovc("AT#CO","MC",1000)!=1&&lop<5){
    lop++; 
   }
   delaymillis(200);
   if(lop<limit){
    lop=0;
    t=1;
   }else{t=1;}
   }else{t=1;}
  }

  call=0;
  tt=1;
}
while(off==1){
  // Serial1.begin(115200);
    delaymillis(400);
    while(ovc("AT#CG","OK",1000)!=1); 
    delaymillis(400);
 // Serial1.end();
  off=0;
  tt=0;
}
while(ignore==1){
  // Serial1.begin(115200);
      delaymillis(400);
      while(ovc("AT#CF","IF",1000)!=1); 
      delaymillis(400);
  //Serial1.end();
  ignore=0;
  
}

if((angle_roll>50)&&(angle_roll<60)&&state==0){
     digitalWrite(ledtimer,HIGH);
     //digitalWrite(13,!digitalRead(13));
     PRR0|=(0<<PRTIM1); // bat timer
                             // tắt ngắt toàn cục
    
    /* Reset Timer/Counter1 */
    
    TCCR1A = 0;
    TCCR1B = 0;
    TIMSK1 = 0;
    cli();
    /* Setup Timer/Counter1 */
    TCCR1B |= (1 << CS12) ;
    TCCR1B |= (1 << CS10) ;
    TCNT1 = 62500;
    TIMSK1 = (1 << TOIE1);                  // Overflow interrupt enable 
    sei();                                  // cho phép ngắt toàn cục
 // Serial.println(" bat timer");
  
}
else{
    digitalWrite(ledtimer,LOW);
}

}

/// doi tin hieu doc duoc tu A0 thanh luc
long forceCal (int fsrReading)  
{
    int fsrVoltage;     // gia tri analog sang voltage
    unsigned long fsrResistance;  // gia tri vol sang dien tro
    unsigned long fsrConductance; // do dan
    
    fsrVoltage = map (fsrReading, 0, 1023, 0, 5000);
    ////tinh gia tri dien tro cua tam xop cam bien
    fsrResistance = 5000 - fsrVoltage;
    fsrResistance *= 5000;// 1k resistor
    fsrResistance /= fsrVoltage;
    ///// tinh gia tri dan dien 
    fsrConductance = 1000000; //// measure in microommhos
    fsrConductance /= fsrResistance;
    /// tinh luc
    if(fsrConductance <1000)
    {
      fsrForce = fsrConductance / 80; 
    }
    else
    {
      fsrForce = fsrConductance - 1000;
      fsrForce /= 30;
    }
    return fsrForce;
}




void read_mpu_6050_data() {                                            //Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68, 14);                                          //Request 14 bytes from the MPU-6050
  while (Wire.available() < 14);                                       //Wait until all the bytes are received
  acc_x = Wire.read() << 8 | Wire.read();                              //Add the low and high byte to the acc_x variable
  acc_y = Wire.read() << 8 | Wire.read();                              //Add the low and high byte to the acc_y variable
  acc_z = Wire.read() << 8 | Wire.read();                              //Add the low and high byte to the acc_z variable
  temperature = Wire.read() << 8 | Wire.read();                        //Add the low and high byte to the temperature variable
  gyro_x = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the gyro_x variable
  gyro_y = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the gyro_y variable
  gyro_z = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the gyro_z variable

}

void setup_mpu_6050_registers() {
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    // truy xuat thanh ghi 6b thanh ghi quan ly nang luong
  Wire.write(0x00);                                                    //dua 8bit ve 0 
  Wire.endTransmission();                                              //End the transmission
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    // truy xuat thanh ghi 1C
  Wire.write(0x10);                                                    //truy xuat thanh Ghi AFS-SEl để lấy đâu ra cho acco la+-8g
  Wire.endTransmission();                                              //End the transmission
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    // tuong nhu nhu acceler
   Wire.write(0x08);    
  Wire.endTransmission();                                              //End the transmission
}



 
 void SOS(){
 digitalWrite(ledinterrup,HIGH);
 digitalWrite(ledtimer,!(digitalRead(ledtimer)));
 digitalWrite(ledint,!(digitalRead(ledint)));
unsigned long timecut;
 reset_on();
 digitalWrite(cuttran,HIGH);// bat GPS qua tran
//reset_on();
 //timecut=millis();
// while((unsigned long) (millis()- timecut)<15000 )
 delaymillis(15000);// thoi gian de thu GPS
 AT=0;
 while( AT==0){
    unsigned int more=0;
    while(sim900a("AT","OK",1000)!=1&&more<limit){
      more++;
    }
    if (more<limit){
      more=0;
      while(sim900a("AT+CREG?","+CREG: 0,1",8000)!=1&&more<limit)
  
      {
      more++;
      }
     if (more<limit){
      more=0;
      AT=1;
      GPRS1=0;
      
      // turn on GPRS
      while(GPRS1==0){
        more=0;
        while(sim900a("AT+CGATT=1","OK",10000)!=1&&more<limit){
          more++;
        
          }
           
          if(more<limit){         
           more=0;
           login=0;
            while(login==0){
            while(sim900a("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",2000)!=1);
           
           // while(sim900a("AT+SAPBR=3,1,\"APN\",\"v-internet\"","OK",2000)!=1);
         
            while(sim900a("AT+SAPBR=3,1,\"APN\",\"3m-word\"","OK",2000)!=1);
            
            while(sim900a("AT+SAPBR=3,1,\"USER\",\"mms\"","OK",2000)!=1);
         
            while(sim900a("AT+SAPBR=3,1,\"PWD\",\"mms\"","OK",2000)!=1);
           
            login=1;
         //   Serial.println("register succesfully");
          //  GET=1; 
            }
        
            more=0;
            delaymillis(200);
            while(sim900a("AT+SAPBR=1,1","OK",15000)!=1&&more<limit){
            more++;
     
            }
               
          
             if(more<limit){
              more=0;
              while(sim900a("AT+SAPBR=2,1","OK",2000)!=1&&more<limit){
                more++;
              }
              if(more<limit){
                more=0;
               GPRS1=1;
              HTTP=0;
             
                while(HTTP==0){
                 more=0;
                  while(sim900a("AT+HTTPINIT","OK",15000)!=1&&more<limit){
                    more++;
                   
                  }
                  
                  if(more<limit){
                    HTTP=1;// ko cho bat HHTP 
                    more=0;
                   while(sim900a("AT+HTTPPARA=\"CID\",1","OK",1000)!=1&&more<limit){
                    more++;
                   }
                  if (more<limit){
                    more=0;
                    i=0;
                    GET=0;
                 while(GET==0){
              
                 more=0;
                 if(more<limit){
                    i++;
                 GPS=0;
                 while(GPS==0){
                   
                 while ( Serial3.available() > 0)
                    {
                      
                    Serial3.read();
                    
                    }
                    if ( Serial3.find("$GPRMC,")) {
                    String tempMsg =  Serial3.readStringUntil('\n');
                    for (int i = 0; i < tempMsg.length(); i++) {
                    if (tempMsg.substring(i, i + 1) == ",") {
                    nmea[pos] = tempMsg.substring(stringplace, i);
                    stringplace = i + 1;
                    pos++;
                    }
                    if (i == tempMsg.length() - 1) {
                    nmea[pos] = tempMsg.substring(stringplace, i);
                    }
                    }
                    updates++;
                    nmea[2] = ConvertLat();
                    nmea[4] = ConvertLng();
                //  Serial.print(labels[i]);
                  //  Serial.print(nmea[i]);
                  // Serial.println("");
                    
                    
                    }
                    else {
                    
                    failedUpdates++;
                    
                    }
                    stringplace = 0;
                    pos = 0;
                    Serial3.flush();
                   
                    if(nmea[1]=="A"){
                     GPS=1;
                   
                      }
                   }
                  get_gpsdata(nmea[2],nmea[4],user);
                  if(more<limit){
                  more=0;
                 while(sim900a(httppara,"OK",1000)!=1&&more<limit){
                  more++;
                }
              
                
                   if (more<limit){
                     more=0;
                    while(sim900a("AT+HTTPACTION=0","+HTTPACTION:0,200",15000)!=1&&more<limit){
                      more++;
                    
                    }
                    if(more<limit){
                    more=0;
                    while(sim900a("AT+HTTPREAD=0,900\r\n","+HTTPREAD:",10000)!=1&&more<limit){
                        more++;
                      }
                          if(i<4){
                            AT=0;
                            GET=0;
                          }
                          if(i>4){
                            
                   TIMSK1 = (0 << TOIE1);                  // Overflow interrupt unnable 
                   digitalWrite(cuttran,LOW);//tat GPS
                   digitalWrite(12,!(digitalRead(12)));
                     GET=1;
                          }
                         
                     
                        } 
                        else{
                      GET=1;
                      reset_on();
                      AT=0;
                    }
                   
                   
                    }
                    else{
                      GET=1;
                      reset_on();
                      AT=0;
                    }
          
                   }
                  }
                
                }
     
                
              }
                else{
                      GET=1;
                      reset_on();
                      AT=0;
                    }
          
              } 
  
        }
    
      }
      else{
       //Serial.println(" lag vl");
        GPRS1=1;
        reset_on();
        AT=0;
        
      }
 
     }
   }

   
}


}
else{
  AT=1;
        reset_on();
        AT=0;
}
}
 }
 } 
  
 
 
 
 char sim900a(char* ATcommand, char*  expected_answer1, unsigned int timeout){

    unsigned char x=0,  answer=0;
    char response[100];
    unsigned long previous;
    memset(response, '\0', 100);    //
    delay(100);
     while( Serial2.available() > 0) Serial2.read();    // 
    Serial2.println(ATcommand);    // Send the AT command 
    Serial.println(ATcommand);   
    x = 0;
    previous = millis();

   
    do{
        if(Serial2.available() != 0){    
            response[x] = Serial2.read();
            x++;
         
            if (strstr(response, expected_answer1) != NULL)    
            {
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == 0) && (unsigned long)(millis() - previous) < timeout);
        Serial2.println(response);  
   Serial.println(response);
   
    return answer;
}

  
 void reset_on(){

  delaymillis(800);
  digitalWrite(pinRest,HIGH);
 delaymillis(600);
 digitalWrite(pinRest,LOW);
delaymillis(200);
 digitalWrite(pinRest,HIGH);
delaymillis(800);
 Serial.println(" Reset SIM");
 
 
  
 }

  char* get_gpsdata(String x,String y,String z){

  // xu ly chuoi trc khi chay at
  String para="AT+HTTPPARA=\"URL\",\"";
  para+=URL;
  para+="lati=";
  para+=x;
  para+="&";
  para+="longti=";
  para+=y;
  para+="&";
  para+="ID=";
  para+=z;
 // para+="&";
 // para+="date=";
//  para+=q;
  para+="\"";
   // conver para form String-> Char*
  const char* http = para.c_str();
   httppara = new char[strlen(http) + 20]; // creat chuoi http vs nhieu ki tu hon str_quest
  strcpy(httppara, http );
  //Serial.println(httppara);
  return httppara;
  
}


 String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";

  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}

String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }

  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
      //Serial.println(lngsecond);

    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal); //float -> char
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
 } 
 

 char* sms_gps(String x,String y){

  // xu ly chuoi trc khi chay at

  link+=x;
  link+=",";
  link+=y;
 
   // conver para form String-> Char*
  const char* http = link.c_str();
  httpparatext = new char[strlen(http) + 1]; // creat chuoi http vs nhieu ki tu hon str_quest
  strcpy(httpparatext, http );
  //Serial.println(httppara);
return httpparatext;
}
void Send(char sms[]){
 unsigned int mes=0;
 while(mes==0){
 unsigned int more=0;
 while(sim900a("AT+CMGF=1","OK",1000)!=1&&more<limit){
  more++;
 }
 if(more<limit){
 more=0;
  snprintf(number,sizeof(number),"AT+CMGS=\"%s\"", MsgMob);
 while(sim900a(number,">",4000)!=1&&more<limit){
  more++;
}
 delaymillis(500);
 if (more<limit){
  more=0;
  Serial2.println(sms);
//   Serial.println(sms);
  //Serial2.print(" fuck off");
  delaymillis(500);
  Serial2.write(0x1A);
  Serial2.write(0x0D);
  Serial2.write(0x0A);
/*  Serial.print("fuck off");
  delay(500);
  Serial.write(0x1A); 
 Serial.write(0x0D);
 Serial.write(0x0A);
  delay(1000);
  */
while(sim900a("","OK",5000)!=1&&more<limit){
more++;
 }
  if (more<limit){
   more=0;
  //Serial.println( "sms da gui");
 mes=1;
   }
   else{
    reset_on();
    
   }
  }
  else{
    reset_on();
  }
  }
  else{
    reset_on();
  }
 }
}
  
 void setup_sim(){
 
  while(sim900a("AT","OK",2000)!=1);
  while(sim900a("AT+CMGF=1","OK",2000)!=1);
  while(sim900a("AT+CNMI=1,2,0,0,0","OK",4000)!=1);
  // while(ovc("AT#MF","",1000)!=1);
  
}
void Clear(){
  Serial2.flush();
 RcvdCheck = 0;
RcvdConf = 0;
index = 0;
RcvdEnd = 0;
MsgMob[15];
MsgTxt[50];
MsgLength = 0;
}
void interrup(){
fuck=1;
}
 

 
 
 ISR (TIMER1_OVF_vect) 
{  
    TCNT1=62500;
  if((angle_roll<20)&&state==0)// khi doi mu thi gia tri irled=0
    {   
      if(fsrForce>10){
        fuck=1;  
        TCNT1=62500;
               
    }
    
}
 
}

void  delaymillis(float wait){
  unsigned long pretime;
  pretime=millis();
  while((unsigned long)millis()-pretime<wait);
 }
 
 
 void serialEvent2() {
  while(Serial2.available()!=0)
{
char data = Serial2.read();
Serial2.setTimeout(3000);
if(data == '+'){RcvdCheck = 1;}
if((data == 'C') && (RcvdCheck == 1)){RcvdCheck = 2;}
if((data == 'M') && (RcvdCheck == 2)){RcvdCheck = 3;}
if((data == 'T') && (RcvdCheck == 3)){RcvdCheck = 4;}
if(RcvdCheck == 4){RcvdConf = 1; RcvdCheck = 0;}

if(RcvdConf == 1)
{
if(data == '\n'){RcvdEnd++;}
if(RcvdEnd == 3){RcvdEnd = 0;}
RcvdMsg[index] = data;

index++;
if(RcvdEnd == 2){RcvdConf = 0;MsgLength = index-2;index = 0;}
if(RcvdConf == 0)
{ 
Serial.println();
Serial.print("Mobile Number is: ");
for(int x = 4;x < 17;x++)
{
  MsgMob[x-4] = RcvdMsg[x];
  Serial.print(MsgMob[x-4]);
}
  Serial.println();
  Serial.print("Message Text: ");
for(int x = 46; x < MsgLength; x++)
{
  MsgTxt[x-46] = RcvdMsg[x];
 // Serial.print(MsgTxt[x-46]);
}
//if (Serial2.findUntil(MsgTxt,pass)){
if(strstr(MsgTxt,pass)!=NULL){
  digitalWrite(ledsms ,HIGH);
  digitalWrite(ledint,!(digitalRead(ledint)));
  digitalWrite(cuttran,HIGH);// bat GPS qua tran
delaymillis(15000);
  GPS=0;
 while(GPS==0){
  while (Serial3.available() > 0)
  {
    Serial3.read();

  }
  if (Serial3.find("$GPRMC,")) {
    String tempMsg = Serial3.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[pos] = tempMsg.substring(stringplace, i);
        stringplace = i + 1;
        pos++;
      }
      if (i == tempMsg.length() - 1) {
        nmea[pos] = tempMsg.substring(stringplace, i);
      }
    }
    updates++;
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
    for (int i = 0; i < 9; i++) {
   //  Serial.print(labels[i]);
    // Serial.print(nmea[i]);
    // Serial.println("");
    }

  }
  else {

    failedUpdates++;

  }
  stringplace = 0;
  pos = 0;
 Serial3.flush();
 if(nmea[1]=="A"){
// Serial.println("GET GPS succesfully");
   GPS=1;
  }

}
sms_gps(nmea[2],nmea[4]);
Send(httpparatext);
Clear();
digitalWrite(cuttran,LOW);
digitalWrite(ledsms ,!digitalRead(ledsms ));
  }

Clear();
}
}
}

}

 
 void led(){
  digitalWrite(13,HIGH);
  delaymillis(50);
  digitalWrite(13,LOW);
 }
 
 
 
void call_init (){
delay(50);

if(digitalRead(6)==0){
delay(30);

if (tt==0) {
   call=1;
   
}

if (tt==1) 
{  
  off=1;
 
}

}
if(!digitalRead(7)){
 ignore=1; 
}
 

}
 
  char ovc(char* ATcommand, char*  expected_answer1, unsigned int timeout){

    unsigned char x=0,  answer=0;
    char response[100];
    unsigned long previous;
    memset(response, '\0', 100);    //
    delay(100);
     while( Serial1.available() > 0) Serial1.read();    // 
   Serial1.println(ATcommand);    // Send the AT command 
    
    Serial.println(ATcommand);
    x = 0;
    previous = millis();

   
    do{
        if(Serial1.available() != 0){    
            response[x] = Serial1.read();
            x++;
         
            if (strstr(response, expected_answer1) != NULL)    
            {
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == 0) && (unsigned long)(millis() - previous) < timeout);
       
  Serial1.println(response);
  Serial.println(response);
 Serial1.flush();
  Serial.flush();
    return answer;
}

 

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

