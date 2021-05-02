

/*VCC - 5V
GND - GND
SDA - A4
SCL - A5*/

#include <Wire.h>
#include <SoftwareSerial.h>
SoftwareSerial I2CBT(3,2);
// El TX pin 3
// El RX pin 2
byte serialA;
 
int gyro_x, gyro_y, gyro_z;
long acc_x, acc_y, acc_z, acc_total_vector;
int temperature;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
long loop_timer;
int lcd_loop_counter;
float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
boolean set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output;
int pulso; 
int limite = 500;


void setup() {
Wire.begin();
I2CBT.begin(9600);  
Serial.begin(9600); 


setup_mpu_6050_registers(); //Iniciar registros

gyro_x_cal /= 2000; //dividir en 2000
gyro_y_cal /= 2000; 
gyro_z_cal /= 2000; 


loop_timer = micros(); //Reiniciar timer
}

void loop(){
pulso = analogRead(A0);
read_mpu_6050_data(); //Lectura del sensor

gyro_x -= gyro_x_cal; //restar valores de la calibracion
gyro_y -= gyro_y_cal; 
gyro_z -= gyro_z_cal; 

//Calculos

angle_pitch += gyro_x * 0.0000611; //Sumar angulo 
angle_roll += gyro_y * 0.0000611; 


angle_pitch += angle_roll * sin(gyro_z * 0.000001066); //transferir angulo de inclinacion
angle_roll -= angle_pitch * sin(gyro_z * 0.000001066); 

//Calculos angulos del acelerometro
acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z)); //Calcular total

angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296; //Calculo x
angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296; //Calculo y


angle_pitch_acc -= 0.0; //Calibracion
angle_roll_acc -= 0.0; l

if(set_gyro_angles){ //iniciar
angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004; //Correjir inclinacion del acelerometro
angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004; 
}
else{ 
angle_pitch = angle_pitch_acc; //reiniciar 
angle_roll = angle_roll_acc; 
set_gyro_angles = true; //reiniciar 
}


angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;
angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1; //Tomar el 90% y el 10 suelto



while(micros() - loop_timer < 4000); //Tiempo de espera
loop_timer = micros(); //Reiniciar timer

byte Datos[2];
serialA=I2CBT.read();
Datos[0]='a';
Datos[1]=angle_pitch_output;
Datos[2]=angle_roll_output;
Datos[3]=pulso;
Datos[4]=gyro_z;

// Envío de datos BT
if (serialA == 49){
      I2CBT.write(Datos[0]);
      I2CBT.write(Datos[1]);
      I2CBT.write(Datos[2]);
      I2CBT.write(Datos[3]);
      I2CBT.write(Datos[4]);
      serialA=0; 
} 
Serial.println(angle_pitch_output);
Serial.println(angle_roll_output);
Serial.println(pulso);

}

  //Transmisiones del acelerometro
void read_mpu_6050_data(){ 
Wire.beginTransmission(0x68); //Comenzar comunicacion
Wire.write(0x3B); //ENviar solicitud de registro
Wire.endTransmission(); //Fin de transmision
Wire.requestFrom(0x68,14); //Solicitud de 14 bytes
while(Wire.available() < 14); //Espera de bytes
acc_x = Wire.read()<<8|Wire.read(); //añadir a acc_x
acc_y = Wire.read()<<8|Wire.read(); //añadir a acc_y
acc_z = Wire.read()<<8|Wire.read(); //añadir a acc_z
temperature = Wire.read()<<8|Wire.read(); //añadir a temperature
gyro_x = Wire.read()<<8|Wire.read(); //añadir a gyro_x
gyro_y = Wire.read()<<8|Wire.read(); //añadir a gyro_y
gyro_z = Wire.read()<<8|Wire.read(); //añadir a gyro_z
}

void setup_mpu_6050_registers(){
    //Segunda transmision
Wire.beginTransmission(0x68); //Comenzar comunicacion
Wire.write(0x6B); //Enviar solicitud de registro
Wire.write(0x00); 
Wire.endTransmission(); //Terminar transmision
//Configurar acelerometro (+/-8g)
Wire.beginTransmission(0x68); //Comenzar comunicacion
Wire.write(0x1C); //Enviar solicitud de registro
Wire.write(0x10); //Reiniciar solicitud
Wire.endTransmission(); //Terminar transmision
//Configurar gyroscopio (500dps full scale)
Wire.beginTransmission(0x68); //Start communicating with the MPU-6050
Wire.write(0x1B); //Enviar solicitud de registro
Wire.write(0x08); 
Wire.endTransmission(); //Terminar transmision
}
