#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

const int pinRele = 8;
bool releActivo = false;
unsigned long tiempoActivacion = 0;
unsigned long duracionRiego = 0;

void setup() {
  Serial.begin(9600);
  if (!rtc.begin()) {
    Serial.println("No se encontró el RTC");
    while (1);
  }

  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, HIGH); // Rele apagado por defecto (invertido)
}

void loop() {
  DateTime ahora = rtc.now();

  // Mostrar hora actual
  Serial.print("Hora actual: ");
  Serial.print(ahora.hour());
  Serial.print(":");
  Serial.print(ahora.minute());
  Serial.print(":");
  Serial.println(ahora.second());

  // Activaciones programadas (solo mañana y noche)
  if (!releActivo) {
    if ((ahora.hour() == 10 || ahora.hour() == 22) && ahora.minute() == 0 && ahora.second() == 0) {
      digitalWrite(pinRele, LOW); // Encender bomba
      Serial.println(">> Bomba ACTIVADA - Riego de 5 segundos");
      tiempoActivacion = millis();
      duracionRiego = 5000;
      releActivo = true;
    }
  }

  // Control de apagado
  if (releActivo) {
    unsigned long tiempoTranscurrido = millis() - tiempoActivacion;

    if (tiempoTranscurrido >= duracionRiego) {
      digitalWrite(pinRele, HIGH); // Apagar bomba
      Serial.println(">> Bomba DESACTIVADA");
      releActivo = false;
    } else {
      Serial.print(">> Bomba ENCENDIDA - faltan ");
      Serial.print((duracionRiego - tiempoTranscurrido) / 1000);
      Serial.println(" seg para apagarse");
    }
  }

  delay(1000); // Refrescar cada segundo
}
