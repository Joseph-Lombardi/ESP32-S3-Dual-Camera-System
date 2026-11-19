#include <Arduino.h>
#include <Arducam_Mega.h>

// ============================================
// PI LOM - CAPTURA DE DOS CAMARAS POR USB
// Biblioteca Arducam Dual SPI modificada
// ============================================

// Camara 1: FSPI
// SCK 12, MISO 13, MOSI 11, CS 10
#define CAM1_CS 10

// Camara 2: HSPI
// SCK 16, MISO 17, MOSI 18, CS 21
#define CAM2_CS 21

#define BLOQUE 128
#define MAX_JPEG 2000000UL

Arducam_Mega camara1(CAM1_CS);
Arducam_Mega camara2(CAM2_CS);

bool lista1 = false;
bool lista2 = false;

// ============================================
// CAPTURAR Y TRANSMITIR UNA CAMARA
// ============================================

bool enviarFoto(Arducam_Mega &camara, int numero) {

  CamStatus estado = camara.takePicture(
    CAM_IMAGE_MODE_VGA,
    CAM_IMAGE_PIX_FMT_JPG
  );

  if (estado != CAM_ERR_SUCCESS) {
    Serial.print("ERROR CAPTURA_CAM");
    Serial.println(numero);
    return false;
  }

  uint32_t total = camara.getTotalLength();

  if (total == 0 || total > MAX_JPEG) {
    Serial.print("ERROR TAMANO_CAM");
    Serial.println(numero);
    return false;
  }

  // Protocolo: JPEG <camara> <bytes>
  Serial.print("JPEG ");
  Serial.print(numero);
  Serial.print(" ");
  Serial.println(total);
  Serial.flush();

  uint8_t buffer[BLOQUE];
  uint32_t enviados = 0;

  while (enviados < total) {

    uint8_t cantidad = (uint8_t)min(
      (uint32_t)BLOQUE,
      total - enviados
    );

    uint8_t leidos = camara.readBuff(
      buffer,
      cantidad
    );

    if (leidos == 0) {
      // La imagen quedo incompleta.
      // Python detectara el timeout.
      return false;
    }

    Serial.write(buffer, leidos);
    enviados += leidos;

    yield();
  }

  Serial.flush();

  // Aviso de que termino esta fotografia
  Serial.print("END ");
  Serial.println(numero);
  Serial.flush();

  return true;
}

// ============================================
// INICIALIZACION
// ============================================

void setup() {

  Serial.begin(115200);
  delay(2000);

  // Mantener ambos CS desactivados
  pinMode(CAM1_CS, OUTPUT);
  pinMode(CAM2_CS, OUTPUT);

  digitalWrite(CAM1_CS, HIGH);
  digitalWrite(CAM2_CS, HIGH);

  // La biblioteca Dual SPI inicializa
  // internamente los dos buses.

  CamStatus e1 = camara1.begin();
  lista1 = (e1 == CAM_ERR_SUCCESS);

  CamStatus e2 = camara2.begin();
  lista2 = (e2 == CAM_ERR_SUCCESS);

  if (lista1 && lista2) {
    Serial.println("READY");
  } else {
    Serial.println("ERROR INICIALIZACION");
  }
}

// ============================================
// COMANDOS USB
// ============================================

void loop() {

  if (!Serial.available()) {
    delay(10);
    return;
  }

  String comando = Serial.readStringUntil('\n');
  comando.trim();

  if (comando == "F") {

    if (!lista1 || !lista2) {
      Serial.println("ERROR CAMARAS_NO_LISTAS");
      return;
    }

    // Fotografia 1
    if (!enviarFoto(camara1, 1)) {
      return;
    }

    delay(500);

    // Fotografia 2
    if (!enviarFoto(camara2, 2)) {
      return;
    }

    Serial.println("DONE");
    Serial.flush();
  }
}
