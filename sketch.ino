#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

// Configuración de la Pantalla LCD I2C (Dirección 0x27 o 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Definición de Pines - Sensor 1
const int TRIG_PIN_1 = 5;
const int ECHO_PIN_1 = 6;

// Definición de Pines - Sensor 2
const int TRIG_PIN_2 = 4;
const int ECHO_PIN_2 = 18;

// Definición de Pin - Servomotor
const int SERVO_PIN = 7;
Servo tapaServo;

// Variables de medición
long duracion1, duracion2;
int distancia1, distancia2;
int nivelPorcentaje1 = 0;

// Altura máxima del contenedor en centímetros (ejemplo: 20 cm)
const int ALTURA_CONTENEDOR = 20; 

void setup() {
  Serial.begin(115200);

  // Configuración de pines de los sensores
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);

  // Configuración del servomotor
  tapaServo.attach(SERVO_PIN);
  tapaServo.write(0); // Tapa abierta por defecto

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Urban Clean IoT");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

int medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duracion = pulseIn(echoPin, HIGH, 30000); // Timeout de 30ms
  if (duracion == 0) return 0;
  int distancia = duracion * 0.034 / 2;
  return distancia;
}

void loop() {
  // Medir distancias con ambos sensores
  distancia1 = medirDistancia(TRIG_PIN_1, ECHO_PIN_1);
  distancia2 = medirDistancia(TRIG_PIN_2, ECHO_PIN_2);

  // Calcular porcentaje de llenado basado en el Sensor 1
  if (distancia1 > 0 && distancia1 <= ALTURA_CONTENEDOR) {
    nivelPorcentaje1 = map(distancia1, ALTURA_CONTENEDOR, 0, 0, 100);
  } else if (distancia1 > ALTURA_CONTENEDOR) {
    nivelPorcentaje1 = 0;
  } else {
    nivelPorcentaje1 = 100; // Si está muy cerca o desbordado
  }

  // Mostrar en Monitor Serie
  Serial.print("Sensor 1 Nivel: "); Serial.print(nivelPorcentaje1); Serial.print("% | ");
  Serial.print("Sensor 2 Dist: "); Serial.print(distancia2); Serial.println(" cm");

  // Lógica de control y alerta del 85%
  lcd.setCursor(0, 0);
  lcd.print("Nivel S1: ");
  lcd.print(nivelPorcentaje1);
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  if (nivelPorcentaje1 >= 85) {
    lcd.print("ALERTA: LLENO!  ");
    tapaServo.write(90); // Bloquea o cierra la tapa (90 grados)
  } else {
    lcd.print("Estado: Normal  ");
    tapaServo.write(0);  // Tapa abierta o en posición normal (0 grados)
  }

  delay(1000);
}