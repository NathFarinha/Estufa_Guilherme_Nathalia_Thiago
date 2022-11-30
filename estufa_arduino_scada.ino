#include <SimpleModbusSlave.h>

// Projeto Estufa - Laboratório Sistemas de Supervisão e Controle
// include the library code:
#include <LiquidCrystal.h>
#include "DHT.h"
#define DHTPIN A1 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);


//////////////// registers of your slave ///////////////////
enum 
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
    
  SENSOR_LUM, // luminosidade
  TEMPERATURA_DHT11, 
  SENSOR_UMID_SOLO,
  HOLDING_REGS_SIZE // leave this one
  // total number of registers for function 3 and 16 share the same register array
  // i.e. the same address space
};
unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array


int Ilum = 6;
int Aquec = 7; 
int Irrig = 8; 
int Vent = 13; 
int ldrPin = 0; // LDR no pino analógico 0
int ldrValue = 0; // Valor lido do LDR
int flag = 0;
float Celsius = 0;
float Umidade = 0; // Percentual de umidade
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
 Serial.begin(115200);
 //Serial.flush();
 pinMode(A0,INPUT); // LDR
 pinMode(A1,INPUT); // Temperatura
 pinMode(A5,INPUT); // Umidade do solo
 pinMode(Ilum,OUTPUT); 
 pinMode(Aquec,OUTPUT);
 pinMode(Irrig,OUTPUT);
 pinMode(Vent,OUTPUT);
 lcd.begin(16, 2); // Declara o lcd 16x2
 
 
 dht.begin();
 modbus_configure(&Serial, 115200, SERIAL_8N1, 1, 2, HOLDING_REGS_SIZE, holdingRegs);

  // modbus_update_comms(baud, byteFormat, id) is not needed but allows for easy update of the
  // port variables and slave id dynamically in any function.
 modbus_update_comms(115200, SERIAL_8N1, 10);
   
}

void loop() {

  modbus_update();
                                                  
  holdingRegs[TEMPERATURA_DHT11] = dht.readTemperature(); 
  holdingRegs[SENSOR_LUM] = analogRead(A0);
  holdingRegs[SENSOR_UMID_SOLO] = analogRead(A0);
  
 
ldrValue = analogRead(ldrPin);
//ldrValue = holdingRegs[SENSOR_LUM];
//Serial.flush();
  
if(ldrValue>500)
{
  digitalWrite(Ilum, HIGH);
}
  
 else
 {
digitalWrite(Ilum,LOW);
 }
  
delay(300);
  
//Celsius = dht.readTemperature();
 Celsius = holdingRegs[TEMPERATURA_DHT11];
 Umidade = analogRead(A5);
  if(Celsius<25 && flag ==0)
{
  flag = 1;
  digitalWrite(Aquec,HIGH);
  digitalWrite(Vent,LOW);
}
  
  else if(Celsius>35)
  {
  digitalWrite(Aquec,LOW);
  digitalWrite(Vent,HIGH);
  }
  
  else
  {
  digitalWrite(Aquec,LOW);
  digitalWrite(Vent,LOW);
  }
  
  if(flag==1)
{
  digitalWrite(Aquec,HIGH);
  digitalWrite(Vent,LOW);
    
    if(Celsius >=28)
    {
      flag = 0;
    }
}
  
  
  
  Umidade = (Umidade/876)*100;
  
if(Umidade <70)
  {
   digitalWrite(Irrig,HIGH);
  }
  
  else
  {
  digitalWrite(Irrig,LOW);
  }
  //Serial.println(Umidade);

 lcd.clear(); // Limpa o LCD
  lcd.setCursor(0, 1);
  lcd.print("Umidade=");
  lcd.setCursor(9, 1);
  lcd.print(Umidade);
  lcd.setCursor(14, 1);
  lcd.print("%");
  
  lcd.setCursor(0, 0);
  lcd.print("T=");
  lcd.print(Celsius);
  lcd.setCursor(7, 0);
  lcd.print("C");
  
  lcd.setCursor(9, 0);
  lcd.print("I=");
  lcd.print(ldrValue);
 
  Serial.print("Ilum = ");
  Serial.print(ldrValue);
  Serial.print(", Temp = ");
  Serial.print(Celsius);
  Serial.print(", Umidade = ");
  Serial.println(Umidade);
  
 }
