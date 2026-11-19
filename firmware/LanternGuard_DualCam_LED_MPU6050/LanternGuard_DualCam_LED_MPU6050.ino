#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include <Arducam_Mega.h>
#include <Adafruit_NeoPixel.h>

// PI LOM - LANTERNGUARD
// Dos camaras en buses SPI independientes, LEDs y MPU6050
// Requiere biblioteca Arducam Dual SPI modificada.

// Camara 1 FSPI: SCK 12, MISO 13, MOSI 11, CS 10
#define CAM1_CS 10
// Camara 2 HSPI: SCK 16, MISO 17, MOSI 18, CS 21
#define CAM2_CS 21

#define LED_CAM1 4
#define LED_CAM2 5
#define NUM_LEDS 8
#define BRILLO 128

#define MPU_SDA 8
#define MPU_SCL 7
#define MPU_ADDR 0x68

#define BLOQUE 128
#define MAX_JPEG 2000000UL

Arducam_Mega camara1(CAM1_CS);
Arducam_Mega camara2(CAM2_CS);
Adafruit_NeoPixel anillo1(NUM_LEDS, LED_CAM1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel anillo2(NUM_LEDS, LED_CAM2, NEO_GRB + NEO_KHZ800);

bool lista1 = false;
bool lista2 = false;
bool mpuListo = false;

void apagarLEDs() {
  anillo1.clear();
  anillo2.clear();
  anillo1.show();
  anillo2.show();
}

void encenderLEDs() {
  anillo1.fill(anillo1.Color(255, 255, 255));
  anillo2.fill(anillo2.Color(255, 255, 255));
  anillo1.show();
  anillo2.show();
}

void iniciarLEDs() {
  anillo1.begin();
  anillo2.begin();
  anillo1.setBrightness(BRILLO);
  anillo2.setBrightness(BRILLO);
  apagarLEDs();
}

bool escribirMPU(uint8_t registro, uint8_t valor) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(registro);
  Wire.write(valor);
  return Wire.endTransmission() == 0;
}

bool iniciarMPU() {
  Wire.begin(MPU_SDA, MPU_SCL);
  Wire.setClock(100000);
  Wire.beginTransmission(MPU_ADDR);
  if (Wire.endTransmission() != 0) return false;
  if (!escribirMPU(0x6B, 0x00)) return false;
  if (!escribirMPU(0x1C, 0x00)) return false;
  delay(100);
  return true;
}

bool leerInclinacion(float &roll, float &pitch) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)6) != 6) return false;

  int16_t axRaw = (int16_t)((Wire.read() << 8) | Wire.read());
  int16_t ayRaw = (int16_t)((Wire.read() << 8) | Wire.read());
  int16_t azRaw = (int16_t)((Wire.read() << 8) | Wire.read());

  float ax = axRaw / 16384.0f;
  float ay = ayRaw / 16384.0f;
  float az = azRaw / 16384.0f;
  float magnitud = sqrtf(ax * ax + ay * ay + az * az);
  if (magnitud < 0.05f) return false;

  roll = atan2f(ay, az) * 180.0f / PI;
  pitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 180.0f / PI;
  return true;
}

bool enviarFoto(Arducam_Mega &camara, int numero,
                bool apagarDespuesDeCapturar = false) {
  CamStatus estado = camara.takePicture(
    CAM_IMAGE_MODE_VGA, CAM_IMAGE_PIX_FMT_JPG
  );
  if (apagarDespuesDeCapturar) apagarLEDs();
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

  Serial.print("JPEG ");
  Serial.print(numero);
  Serial.print(" ");
  Serial.println(total);
  Serial.flush();

  uint8_t buffer[BLOQUE];
  uint32_t enviados = 0;
  while (enviados < total) {
    uint8_t cantidad = (uint8_t)min((uint32_t)BLOQUE, total - enviados);
    uint8_t leidos = camara.readBuff(buffer, cantidad);
    if (leidos == 0) return false;
    Serial.write(buffer, leidos);
    enviados += leidos;
    yield();
  }

  Serial.flush();
  Serial.print("END ");
  Serial.println(numero);
  Serial.flush();
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  iniciarLEDs();

  pinMode(CAM1_CS, OUTPUT);
  pinMode(CAM2_CS, OUTPUT);
  digitalWrite(CAM1_CS, HIGH);
  digitalWrite(CAM2_CS, HIGH);

  // La biblioteca Dual SPI modificada inicializa los buses internamente.
  CamStatus e1 = camara1.begin();
  lista1 = (e1 == CAM_ERR_SUCCESS);
  CamStatus e2 = camara2.begin();
  lista2 = (e2 == CAM_ERR_SUCCESS);
  mpuListo = iniciarMPU();

  if (lista1 && lista2) Serial.println("READY");
  else Serial.println("ERROR INICIALIZACION");
  if (!mpuListo) Serial.println("ERROR MPU6050_NO_DETECTADO");
}

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

    float roll = 0.0f;
    float pitch = 0.0f;
    bool inclinacionValida = false;
    if (mpuListo) inclinacionValida = leerInclinacion(roll, pitch);

    encenderLEDs();
    delay(500);
    if (!enviarFoto(camara1, 1)) {
      apagarLEDs();
      return;
    }
    delay(500);
    if (!enviarFoto(camara2, 2, true)) {
      apagarLEDs();
      return;
    }
    apagarLEDs();

    if (inclinacionValida) {
      Serial.print("IMU ");
      Serial.print(roll, 2);
      Serial.print(" ");
      Serial.println(pitch, 2);
    } else {
      Serial.println("IMU ERROR");
    }
    Serial.println("DONE");
    Serial.flush();
  }
}
