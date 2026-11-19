#include <Arduino.h>
#include <Arducam_Mega.h>

// La biblioteca PI LOM inicializa ambos buses en la primera transaccion.
Arducam_Mega camara1(10);
Arducam_Mega camara2(21);

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("=== PI LOM / DUAL SPI ===");
  Serial.println("[CAM1] Antes de begin()");
  Serial.flush();
  CamStatus r1 = camara1.begin();
  Serial.print("[CAM1] Resultado: ");
  Serial.println((int)r1);
  Serial.println("[CAM2] Antes de begin()");
  Serial.flush();
  CamStatus r2 = camara2.begin();
  Serial.print("[CAM2] Resultado: ");
  Serial.println((int)r2);
  Serial.println("[FIN] Prueba completada");
}
void loop() { delay(1000); }
