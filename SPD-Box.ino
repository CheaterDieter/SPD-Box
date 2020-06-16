/* Programm für die SPD-Spendenbox
   in der Box verbauter Mikrocontroller: Arduino Nano, old bootloader
   entwickelt in der Arduino IDE 1.8.10
   im Frühjahr 2020

*/

#include "pitches.h"
#include "EEPROM.h"
#include "TimerOne.h"
#include <Adafruit_NeoPixel.h>

// notes in the melody:
int melody[] = {
  NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4,   NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,   NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4,    NOTE_E4, NOTE_D4, NOTE_D4,   NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4,   NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4,  NOTE_D4, NOTE_C4, NOTE_C4,    NOTE_D4, NOTE_D4, NOTE_E4, NOTE_C4,    NOTE_D4, NOTE_E4, NOTE_F4, NOTE_E4, NOTE_C4,    NOTE_D4, NOTE_E4, NOTE_F4, NOTE_E4, NOTE_D4,    NOTE_C4, NOTE_D4, NOTE_G3, NOTE_E4,    NOTE_E4, NOTE_F4, NOTE_G4,    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4,    NOTE_D4, NOTE_C4, NOTE_C4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 4, 4, 4,  4, 4, 4, 4, 4, 4, 4, 4,  3, 8, 2,   4, 4, 4, 4,    4, 4, 4, 4,    4, 4, 4, 4,    3, 8, 2,    4, 4, 4, 4,    4, 8, 8, 4, 4,    4, 8, 8, 4, 4, 4, 4, 4, 2,    4, 4, 4,    4, 4, 4, 4,    4, 4, 4, 4,    3, 8, 2

};
int noteNumber = 62; // number of notes in the Melody



/* ACHTUNG:
   Unterhalb dieses Absatzes müssen normalerweise keine Veränderungen vorgenommen werden
*/

int farbe = 0;
float farbeexpt = 0;

int auszeit = 9500; // Zeit in ms, die zwischen Anschaltungen im Extrastrom 2-Modus vergeht
int anzeit = 500; // Zeit in ms, die Anschaltung im Extrastrom 2-Modus dauert

// Konfiguration auslesen
byte modus = EEPROM.read(1);
// Modus 1 = Dauer an, keine Reaktion bei Geldeinwurf
// Modus 2 = An, bei Geldeinwurf blinkend aus + Ton
// Modus 3 = Aus, bei Geldeinwurf blinkend an + Ton
// Modus 4 = An, bei Geldeinwurf blinkend aus, ohne Ton
// Modus 5 = Aus, bei Geldeinwurf blinkend an, ohne Ton

byte hintergrundbeleuchtung = EEPROM.read(2);
// Hintergrundbeleuchtung 1 = Hellweiß
// Hintergrundbeleuchtung 2 = Regenbogen
// Hintergrundbeleuchtung 3 = Hellweiß wabernd

byte extrastrom = EEPROM.read(3);
// Extrastrom 1 = Kein zusätzlicher Stromverbrauch
// Extrastrom 2 = Dezenter zusätzlicher Stomverbrauch
// Extrastrom 3 = Aggresiver zusätzlicher Stromverbrauch

// Variablen definieren
int trigger = 6;
int echo = 7;
long dauer = 0; // Zeit, die eine Schallwelle bis zur Reflektion und zurück benötigt.
int entfernung = 0;
int state = 0;
int einwurf = 0;
int ledState = LOW;
int stromstatus = 0;
unsigned long previousMillis;
int einwurfreaktion = 0;
int pixel = 0;
int regenbogenfarbe = 1;
boolean dimmen = LOW;
int runde = 1;

#define LED_PIN     2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  15

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 255

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);



void setup() {
  if (modus < 1 || modus > 5) {
    modus = 1;
  }
  if (hintergrundbeleuchtung < 1 || hintergrundbeleuchtung > 3) {
    hintergrundbeleuchtung = 1;
  }
  if (extrastrom < 1 || extrastrom > 3) {
    extrastrom = 1;
  }
  Serial.begin(9600);
  Serial.println ("---KONFIGURATION---");
  Serial.print ("Modus: ");
  Serial.println (modus);
  Serial.print ("Hintergrundbeleuchtung: ");
  Serial.println (hintergrundbeleuchtung);
  Serial.print ("Zusätzlicher Stromverbrauch: ");
  Serial.println (extrastrom);

  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT); // Pin für zustäzlichen Stromverbrauch
  if (extrastrom == 3) {
    digitalWrite(9, HIGH); // aggresiver Stromverbrauch eingestellt
  } else {
    digitalWrite(9, LOW);
  }

  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(2, OUTPUT);


  delay(500);

  digitalWrite(trigger, LOW);
  delay(5);
  digitalWrite(trigger, HIGH);
  delay(10);
  digitalWrite(trigger, LOW);
  dauer = pulseIn(echo, HIGH);
  entfernung = (dauer / 2) * 0.03432;
  if (entfernung < 10) {
    Serial.println ("---KONFIGURATIONSMODUS GESTARTET---");
    // Konfigurationsmodus starten
    for (int i = 0; i < 10; i++) {
      tone(3, 200);
      delay(50);
      noTone(3);
      delay(50);
      tone(3, 300);
      delay(50);
      noTone(3);
      delay(50);
    }

    // Hier ggf Leuchtfunktion einfügen

    int tab = 1;
    while (1 == 1) {
      delay (500);
      for (int i = 0; i < tab; i++) {
        tone(3, 600);
        delay(100);
        noTone(3);
        delay(300);
      }
      if (tab == 1) {
        for (int i = 0; i < modus; i++) {
          tone(3, 1500);
          delay(100);
          noTone(3);
          delay(300);
        }
      }
      if (tab == 2) {
        for (int i = 0; i < hintergrundbeleuchtung; i++) {
          tone(3, 1500);
          delay(100);
          noTone(3);
          delay(300);
        }
      }
      if (tab == 3) {
        for (int i = 0; i < extrastrom; i++) {
          tone(3, 1500);
          delay(100);
          noTone(3);
          delay(300);
        }
      }

      //Messung beginnen
      digitalWrite(trigger, LOW);
      delay(5);
      digitalWrite(trigger, HIGH);
      delay(10);
      digitalWrite(trigger, LOW);
      dauer = pulseIn(echo, HIGH);
      entfernung = (dauer / 2) * 0.03432;
      if (entfernung < 10 || entfernung > 200)
      {
        for (int i = 0; i < 10; i++) {
          tone(3, 200);
          delay(50);
          noTone(3);
          delay(50);
          tone(3, 300);
          delay(50);
          noTone(3);
          delay(50);
        }

        digitalWrite(trigger, LOW);
        delay(5);
        digitalWrite(trigger, HIGH);
        delay(10);
        digitalWrite(trigger, LOW);
        dauer = pulseIn(echo, HIGH);
        entfernung = (dauer / 2) * 0.03432;
        if (entfernung < 10 || entfernung > 200)
        { // Tabwechsel
          tone(3, 300);
          delay(500);
          noTone(3);
          tab = tab + 1;
          if (tab == 4) {
            tab = 1;
          }
        } else {
          if (tab == 1) {
            modus = modus + 1;
            if (modus == 6) {
              modus = 1;
            }
            EEPROM.write(1, modus);
          }
          if (tab == 2) {
            hintergrundbeleuchtung = hintergrundbeleuchtung + 1;
            if (hintergrundbeleuchtung == 4) {
              hintergrundbeleuchtung = 1;
            }
            EEPROM.write(2, hintergrundbeleuchtung);
          }
          if (tab == 3) {
            extrastrom = extrastrom + 1;
            if (extrastrom == 4) {
              extrastrom = 1;
            }
            EEPROM.write(3, extrastrom);
          }
        }
      }
    }
  }
  if (modus == 1 || modus == 2 || modus == 4) {
    Serial.println ("HG Aktiv");
  } else {
    hintergrundbeleuchtung = 0;
    ledAnAus(LOW);
  }

  Serial.println ("---START---");

  if (extrastrom == 2) {
    // dezenter Stromverbrauch eingestellt
    digitalWrite(9, HIGH);
    stromstatus = 1;
    previousMillis = millis ();
    Serial.println ("ZUSATZVERBRAUCH AN");
  }
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
  ledAnAus (LOW);
 


  Timer1.initialize(50000); // µs 50000us = 0,05s = 20Hz
  Timer1.attachInterrupt(callback);


}

void loop() {
  if (extrastrom == 2) {
    // dezenter Stromverbrauch eingestellt
    if (stromstatus == 0 && (millis () - previousMillis) > auszeit) {
      digitalWrite(9, HIGH);
      stromstatus = 1;
      previousMillis = millis ();
      Serial.println ("ZUSATZVERBRAUCH AN");
    }
    if (stromstatus == 1 && (millis () - previousMillis) > anzeit) {
      digitalWrite(9, LOW);
      stromstatus = 0;
      previousMillis = millis ();
      Serial.println ("ZUSATZVERBRAUCH AUS");
    }
  }
  Timer1.detachInterrupt();
  digitalWrite(trigger, LOW);
  delay(5);
  digitalWrite(trigger, HIGH);
  delay(10);
  digitalWrite(trigger, LOW);
  dauer = pulseIn(echo, HIGH);
  entfernung = (dauer / 2) * 0.03432;
  //Serial.println (entfernung);
  Timer1.attachInterrupt(callback);
  if (entfernung < 8) {
    Serial.println ("---EINSCHUB---");
    while (entfernung != 11) {

      if (extrastrom == 2) {
        // dezenter Stromverbrauch eingestellt
        if (stromstatus == 0 && (millis () - previousMillis) > auszeit) {
          digitalWrite(9, HIGH);
          stromstatus = 1;
          previousMillis = millis ();
          Serial.println ("ZUSATZVERBRAUCH AN");
        }
        if (stromstatus == 1 && (millis () - previousMillis) > anzeit) {
          digitalWrite(9, LOW);
          stromstatus = 0;
          previousMillis = millis ();
          Serial.println ("ZUSATZVERBRAUCH AUS");
        }
      }
      delay(100);
      Timer1.detachInterrupt();
      digitalWrite(trigger, LOW);
      delay(5);
      digitalWrite(trigger, HIGH);
      delay(10);
      digitalWrite(trigger, LOW);
      dauer = pulseIn(echo, HIGH);
      Timer1.attachInterrupt(callback);
      entfernung = (dauer / 2) * 0.03432;
      Serial.println (entfernung);
    }
    einwurf = 1;
  }

  if (einwurf == 1) {
    einwurf = 0;
    if (extrastrom == 2) {
      // dezenter Stromverbrauch eingestellt
      if (stromstatus == 1) {
        digitalWrite(9, LOW);
        stromstatus = 0;
        previousMillis = millis ();
        Serial.println ("ZUSATZVERBRAUCH AUS");
      }
    }

    if (modus == 2) {
      Timer1.detachInterrupt();


      for (int thisNote = 0; thisNote < noteNumber; thisNote++) {
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        ledAnAus(ledState);
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(3, melody[thisNote], noteDuration);
        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(3);
      }
      farbeexpt = 0;
      dimmen = LOW;
      Timer1.attachInterrupt(callback);
    }

    if (modus == 3) {
      Timer1.detachInterrupt();

      for (int thisNote = 0; thisNote < noteNumber; thisNote++) {
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        ledAnAus(ledState);
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(3, melody[thisNote], noteDuration);
        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(3);
      }
      ledAnAus(HIGH);
      delay(200);
      ledAnAus(LOW);
      Timer1.attachInterrupt(callback);
    }

    if (modus == 4) {
    
      Timer1.detachInterrupt();
      for (int i = 1; i <= 20; i++) {
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }              
        ledAnAus(ledState);
        delay(50);
        ledAnAus(ledState);
        delay(50);
    
      }
      Timer1.attachInterrupt(callback);
    }

    if (modus == 5) {
      Timer1.detachInterrupt();

      for (int i = 1; i <= 20; i++) {
        ledAnAus(HIGH);
        delay(50);
        ledAnAus(LOW);
        delay(50);
      }
      Timer1.attachInterrupt(callback);
    }
    else {
      einwurf = 0;
    }
    previousMillis = millis ();
  }
}

void callback() { //timer1 interrupt
  if (hintergrundbeleuchtung == 1) {
    ledAnAus(HIGH);
  }

  if (hintergrundbeleuchtung == 3) {
    farbeexpt = farbeexpt + 0.05;
    if (farbeexpt * farbeexpt * farbeexpt * farbeexpt > 255) {
      farbeexpt = 0;
      Serial.println ("Farbwert bei 0");
      if (dimmen == LOW) {
        dimmen = HIGH;
      } else {
        dimmen = LOW;
      }
    }

    int ergebnis = round (farbeexpt * farbeexpt * farbeexpt * farbeexpt);

    if (dimmen == HIGH) {
      ergebnis = 255 - ergebnis;
    }

    //Serial.println (ergebnis);

    strip.fill(strip.Color(strip.gamma8 (ergebnis), strip.gamma8 (ergebnis), strip.gamma8 (ergebnis), strip.gamma8 (ergebnis)));
    strip.show();
  }

  if (hintergrundbeleuchtung == 2) {
    if (runde == 1) {
      runde = 1;
    if (regenbogenfarbe == 1) {
      strip.setPixelColor(pixel, strip.Color(255, 0, 0, 0));
    }
    if (regenbogenfarbe == 2) {
      strip.setPixelColor(pixel, strip.Color(0, 255, 0, 0));
    }
    if (regenbogenfarbe == 3) {
      strip.setPixelColor(pixel, strip.Color(0, 0, 255, 0));
    }
    strip.show();                          //  Update strip to match
    pixel = pixel + 1;
    if (pixel == LED_COUNT) {
      pixel = 0;
      regenbogenfarbe = regenbogenfarbe + 1;
      if (regenbogenfarbe == 4) {
        regenbogenfarbe = 1;
      }
    }
    } else {
      runde = runde +1;
    }

  }


}

void ledAnAus (int state) {

  if (state == HIGH) {
    strip.fill(strip.Color(255, 255, 255, 255));
    strip.show();
    ledState = HIGH;
  }
  if (state == LOW) {
    strip.fill(strip.Color(0, 0, 0, 0));
    strip.show();
    ledState = LOW;
  }


}
