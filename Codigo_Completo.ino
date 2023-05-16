#include "HX711.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define CLK A0
#define DOUT A1

unsigned long time;

// Variables de la bomba de agua
const short PinBomba2 = 9;
const short PinBomba1 = 10;
double Densidad = 1;
unsigned int Volumen_deseado;
float setPoint;

// Variables del motor Paso a Paso
const short DIR = 7;
const int STEP = 6;
const short ENABLE = 5;
short int Velocidad;

// Variables del encoder
const int Encoder_pin = 2;
int PulsosRotacion = 20;
int Encoder = 0;        // Value of the Output of the encoder
int EstadoEncoder = 1;  // Current state of the encoder to recognize the pulse changes
int Pulsos = 0;         // Counter of pulses
unsigned long InitialTime = 0;
unsigned long FinalTime;
float RPMs;

// Variables Sensor de temperatura
const int pintDatosDQ = 3;

// Variables de estado
short Estado = 8;
short Dosificando = 0;

//Lista de Estados
const short Estado_MedirBalanza = 0;
const short Estado_DosificarUnica = 1;
const short Estado_MotorUnico = 2;
const short Estado_Dosificar_Y_Motor = 3;
const short Estado_Temperatura = 4;

//Casos de entrada - Condiciones de transicion de estados
const char Tara = 'T';
const char CalibracionBalanza = 'C';
const char DosificarUnica = 'D';
const char MotorUnico = 'M';
const char CambioVelocidad = 'V';
const char STOP = 'S';
const char DosificarYMotor = 'B';
const char Temperatura = 'K';

//Intanciacion de objetos
HX711 balanza(DOUT, CLK);
OneWire oneWireObjeto(pintDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);

DeviceAddress insideThermometer;

void setup() {

  balanza.set_scale(-1284.87);
  balanza.set_offset(176995);

  pinMode(PinBomba1, OUTPUT);
  pinMode(PinBomba2, OUTPUT);

  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(ENABLE, OUTPUT);

  digitalWrite(ENABLE, HIGH);

  time = millis();

  Serial.begin(9600);

  sensorDS18B20.begin();
  if (!sensorDS18B20.getAddress(insideThermometer, 0)) 
    Serial.println("Unable to find address for Device 0"); 

  sensorDS18B20.setResolution(insideThermometer, 9);
  sensorDS18B20.setWaitForConversion(false);

}

void loop() {


  if (Estado == Estado_MedirBalanza) {
    estado_MedirBalanza();

  } else if (Estado == Estado_DosificarUnica) {
    estado_DosificarUnica();

  } else if (Estado == Estado_MotorUnico) {
    IniciarMotor();

  } else if (Estado == Estado_Dosificar_Y_Motor) {
    estado_Dosificar_Y_Motor();

  } else if (Estado == Estado_Temperatura) {
    estado_Temperatura();
  }

  if (Serial.available() > 0) {
    switch (Serial.read()) {

      case Tara:
        Serial.println("Tarando por favor espere");
        balanza.tare(50);
        Serial.println("Completado");
        break;

      case CalibracionBalanza:
        Calibracion();
        break;

      case DosificarUnica:
        Serial.print("¿Cuanto liquido desea echar? => ");  // Solicita al usuario ingresar el peso conocido
        delay(3000);
        Volumen_deseado = Serial.parseInt();
        Serial.println(Volumen_deseado);
        setPoint = balanza.get_units(5) + (Volumen_deseado / Densidad);
        Dosificando = 1;
        Estado = Estado_DosificarUnica;
        break;

      case MotorUnico:
        Serial.print("Con cual velocidad desea iniciar => ");
        delay(3000);
        Velocidad = Serial.parseInt();
        Serial.println(Velocidad);
        digitalWrite(ENABLE, LOW);
        Estado = Estado_MotorUnico;
        break;

      case CambioVelocidad:
        Velocidad = Serial.parseInt();
        break;

      case DosificarYMotor:  //Bomba mientras mezcla
        Volumen_deseado = Serial.parseInt();
        setPoint = balanza.get_units(1) + Volumen_deseado / Densidad;
        Dosificando = 1;
        Estado = Estado_Dosificar_Y_Motor;
        break;

      case Temperatura:
        Estado = Estado_Temperatura;
        break;

      case STOP:
        digitalWrite(ENABLE, HIGH);
        Estado = Estado_MedirBalanza;
        break;
    }
  }
}

void estado_MedirBalanza() {
  double pesoActual = balanza.get_units(5);
  Serial.print("Peso: ");
  Serial.print(pesoActual);
  Serial.println('g');
}

void estado_DosificarUnica() {
  if (Dosificando == 1) {
    DosificarB();
  } else {
    Estado = Estado_MedirBalanza;
  }
}

void estado_Dosificar_Y_Motor() {
  unsigned long actualTime = time;
  unsigned long interval = time - actualTime;
    estado_Temperatura();

  if (Serial.read() == 'V') {
    Velocidad = Serial.parseInt();
  }

  while (interval <= 2000) {
    IniciarMotor();
    time = millis();
    interval = time - actualTime;
  }
  if (Dosificando == 1) {
    DosificarB();
  } else {
    Estado = Estado_MotorUnico;
  }
}

void estado_Temperatura() {
  unsigned long actualTime = time;
  sensorDS18B20.requestTemperaturesByAddress(insideThermometer);
  float tempC = sensorDS18B20.getTempC(insideThermometer);
  Serial.print("Temperatura sensor 1: ");
  Serial.print(tempC);
  Serial.println(" C");
}

