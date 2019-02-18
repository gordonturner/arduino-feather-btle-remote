/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/*
  This is based on the Adafruit example project `hidcontrolkey`.

  The original project has been slightly changed to map buttons to the 
  following commands:
  
  PLAYPAUSE
  MEDIAPREVIOUS
  MEDIANEXT
  MUTE
  VOLUME+
  VOLUME-

  These buttons are on a remote control, speficially targeting iOS
  audio playback.
*/

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         0
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
/*=========================================================================*/


// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/*
 * Setup the input pins for the buttons
 */
// 
const int playPauseButtonPin =     A0;
const int mediaPreviousButtonPin = A1;
const int mediaNextButtonPin =     A2;
const int muteButtonPin =          A3;
const int volumnPlusButtonPin =    A4;
const int volumeMinusButtonPin =   A5;
 
/*
 * Initializing previous states
 */
int previousPlayPauseButtonState =     HIGH;
int previousMediaPreviousButtonState = HIGH;
int previousMediaNextButtonState =     HIGH;
int previousMuteButtonState =          HIGH;
int previousVolumnPlusButtonState =    HIGH;
int previousVolumeMinusButtonState =   HIGH;

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  while (!Serial);  // Required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit HID Control Key Example"));
  Serial.println(F("---------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Factory reset failed!"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  // This demo only works with firmware 0.6.6 and higher!
  // Request the Bluefruit firmware rev and check if it is valid
  if ( !ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    error(F("This sketch requires firmware version " MINIMUM_FIRMWARE_VERSION " or higher!"));
  }

  /* Enable HID Service */
  Serial.println(F("Enable HID Services (including Control Key): "));
  if (! ble.sendCommandCheckOK(F( "AT+BLEHIDEN=On"  ))) {
    error(F("Failed to enable HID (firmware >=0.6.6?)"));
  }

  /* Adding or removing services requires a reset */
  Serial.println(F("Performing a SW reset (service changes require a reset): "));
  if (! ble.reset() ) {
    error(F("Couldn't reset??"));
  }

  Serial.println();
  Serial.println(F("**********************************************************"));
  Serial.println(F("Go to your phone's Bluetooth settings to pair your device"));
  Serial.println(F("Some Control Key works system-wide: mute, brightness ..."));
  Serial.println(F("Some are application specific: Media play/pause"));
  Serial.println(F("**********************************************************"));

  Serial.println();
  Serial.println(F("**********************************************************"));
  Serial.println(F("Initializing the buttons"));
  Serial.println(F("**********************************************************"));
  pinMode(playPauseButtonPin, INPUT_PULLUP);
  pinMode(mediaPreviousButtonPin, INPUT_PULLUP);
  pinMode(mediaNextButtonPin, INPUT_PULLUP);
  pinMode(muteButtonPin, INPUT_PULLUP);
  pinMode(volumnPlusButtonPin, INPUT_PULLUP);
  pinMode(volumeMinusButtonPin, INPUT_PULLUP);

  Serial.println();
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  // PLAYPAUSE button
  int playPauseButtonState = digitalRead(playPauseButtonPin);
  if ((playPauseButtonState != previousPlayPauseButtonState) && (playPauseButtonState == HIGH)) 
  {
    Serial.println( F("PLAYPAUSE button pressed") );
    ble.println("AT+BleHidControlKey=PLAYPAUSE");
    if( ble.waitForOK() )
    {
      Serial.println( F("OK!") );
    }
    else
    {
      Serial.println( F("FAILED!") );
      // Failed, probably pairing is not complete yet
      Serial.println( F("Please make sure Bluefruit is paired and try again") );
    }
    delay(1);
  }
  // Save the current button state.
  previousPlayPauseButtonState = playPauseButtonState;
  

  // MEDIAPREVIOUS button
  int mediaPreviousButtonState = digitalRead(mediaPreviousButtonPin);
  if ((mediaPreviousButtonState != previousMediaPreviousButtonState) && (mediaPreviousButtonState == HIGH)) 
  {
    Serial.println( F("MEDIAPREVIOUS button pressed") );
    ble.println("AT+BleHidControlKey=MEDIAPREVIOUS");
    if( ble.waitForOK() )
    {
      Serial.println( F("OK!") );
    }
    else
    {
      Serial.println( F("FAILED!") );
      // Failed, probably pairing is not complete yet
      Serial.println( F("Please make sure Bluefruit is paired and try again") );
    }
    delay(1);
  }
  // Save the current button state.
  previousMediaPreviousButtonState = mediaPreviousButtonState;


  // MEDIANEXT button
  int mediaNextButtonState = digitalRead(mediaNextButtonPin);
  if ((mediaNextButtonState != previousMediaNextButtonState) && (previousMediaNextButtonState == HIGH)) 
  {
    Serial.println( F("MEDIANEXT button pressed") );
    ble.println("AT+BleHidControlKey=MEDIANEXT");
    if( ble.waitForOK() )
    {
      Serial.println( F("OK!") );
    }
    else
    {
      Serial.println( F("FAILED!") );
      // Failed, probably pairing is not complete yet
      Serial.println( F("Please make sure Bluefruit is paired and try again") );
    }
    delay(1);
  }
  // Save the current button state.
  previousMediaNextButtonState = mediaNextButtonState;

  
  // MUTE button
  int muteButtonState = digitalRead(muteButtonPin);
  if ((muteButtonState != previousMuteButtonState) && (previousMuteButtonState == HIGH)) 
  {
    Serial.println( F("MUTE button pressed") );
    ble.println("AT+BleHidControlKey=MUTE");
    if( ble.waitForOK() )
    {
      Serial.println( F("OK!") );
    }
    else
    {
      Serial.println( F("FAILED!") );
      // Failed, probably pairing is not complete yet
      Serial.println( F("Please make sure Bluefruit is paired and try again") );
    }
    delay(1);
  }
  // Save the current button state.
  previousMuteButtonState = muteButtonState;


  // VOLUMEPLUS button
  int volumnPlusButtonState = digitalRead(volumnPlusButtonPin);
  if ((volumnPlusButtonState != previousVolumnPlusButtonState) && (previousVolumnPlusButtonState == HIGH)) 
  {
    Serial.println( F("MUTE button pressed") );
    ble.println("AT+BleHidControlKey=VOLUME+");
    if( ble.waitForOK() )
    {
      Serial.println( F("OK!") );
    }
    else
    {
      Serial.println( F("FAILED!") );
      // Failed, probably pairing is not complete yet
      Serial.println( F("Please make sure Bluefruit is paired and try again") );
    }
    delay(1);
  }
  // Save the current button state.
  previousVolumnPlusButtonState = volumnPlusButtonState;


  // VOLUMEMINUS button
  int volumeMinusButtonState = digitalRead(volumeMinusButtonPin);
  if ((volumeMinusButtonState != previousVolumeMinusButtonState) && (previousVolumeMinusButtonState == HIGH)) 
  {
    Serial.println( F("MUTE button pressed") );
    ble.println("AT+BleHidControlKey=VOLUME-");
    if( ble.waitForOK() )
    {
      Serial.println( F("OK!") );
    }
    else
    {
      Serial.println( F("FAILED!") );
      // Failed, probably pairing is not complete yet
      Serial.println( F("Please make sure Bluefruit is paired and try again") );
    }
    delay(1);
  }
  // Save the current button state.
  previousVolumeMinusButtonState = volumeMinusButtonState;
  
}
