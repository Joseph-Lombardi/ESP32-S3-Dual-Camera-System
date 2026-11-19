/* PI LOM - Arducam Mega dual SPI for ESP32-S3.
 * Camera 1: FSPI SCK12 MISO13 MOSI11 CS10
 * Camera 2: HSPI SCK16 MISO17 MOSI18 CS21
 * Routes transfers according to the last asserted CS.
 */
#include "ArducamSpi.h"
#include <Arduino.h>
#include <SPI.h>

#if !defined(CONFIG_IDF_TARGET_ESP32S3)
#error "Esta variante dual SPI requiere ESP32-S3"
#endif

static constexpr int CAM1_CS = 10;
static constexpr int CAM2_CS = 21;
static SPIClass spi1(FSPI);
static SPIClass spi2(HSPI);
static SPIClass *activeSpi = nullptr;
static bool started = false;

void arducamSpiBegin(void)
{
    if (started) return;
    pinMode(CAM1_CS, OUTPUT);
    pinMode(CAM2_CS, OUTPUT);
    digitalWrite(CAM1_CS, HIGH);
    digitalWrite(CAM2_CS, HIGH);
    spi1.begin(12, 13, 11, CAM1_CS);
    spi2.begin(16, 17, 18, CAM2_CS);
    started = true;
}

uint8_t arducamSpiTransfer(uint8_t data)
{
    return activeSpi ? activeSpi->transfer(data) : 0;
}

void arducamSpiTransferBlock(uint8_t *buff, uint16_t len)
{
    if (activeSpi && buff && len) activeSpi->transfer(buff, len);
}

void arducamSpiCsHigh(int pin)
{
    digitalWrite(pin, HIGH);
    if ((pin == CAM1_CS && activeSpi == &spi1) ||
        (pin == CAM2_CS && activeSpi == &spi2)) {
        activeSpi = nullptr;
    }
}

void arducamSpiCsLow(int pin)
{
    arducamSpiBegin();
    // Ensure the other device remains deselected.
    digitalWrite(CAM1_CS, HIGH);
    digitalWrite(CAM2_CS, HIGH);
    if (pin == CAM1_CS) activeSpi = &spi1;
    else if (pin == CAM2_CS) activeSpi = &spi2;
    else { activeSpi = nullptr; return; }
    digitalWrite(pin, LOW);
}

void arducamSpiCsOutputMode(int pin)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
}

void arducamDelayMs(uint16_t num) { delay(num); }
void arducamDelayUs(uint16_t num) { delayMicroseconds(num); }
