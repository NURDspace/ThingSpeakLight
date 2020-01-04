/*
    Cheerlight, retrieves the last tweeted color from cheerlight and displays it on a LED strip.
*/

#include <Arduino.h>

#include <ThingSpeak.h>
#include <WiFi.h>
#include <FastLED.h>

#include "secrets.h"

#define NUM_LEDS    60
#define LED_PIN     4

static WiFiClient client;
static CRGB leds[NUM_LEDS];

static const String colorName[] =
    { "none", "red", "pink", "green", "blue", "cyan", "white", "warmwhite", "oldlace", "purple",
    "magenta", "yellow", "orange"
};

// Map of RGB values for each of the Cheerlight color names
static const int colorRGB[][3] = { 0, 0, 0,   // "none"
    255, 0, 0,                  // "red"
    255, 192, 203,              // "pink"
    0, 255, 0,                  // "green"
    0, 0, 255,                  // "blue"
    0, 255, 255,                // "cyan",
    255, 50, 100,               // "white",
    255, 30, 10,                // "warmwhite",
    255, 30, 10,                // "oldlace",
    128, 0, 25,                 // "purple",
    255, 0, 50,                 // "magenta",
    255, 100, 0,                // "yellow",
    255, 10, 0,
};                              // "orange"

static void colorWipe(CRGB color, int wait)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
        FastLED.show();
        delay(wait);
    }
}

void setColor(String color)
{
    for (int iColor = 0; iColor <= 12; iColor++) {
        if (color == colorName[iColor]) {
            int r = colorRGB[iColor][0];
            int g = colorRGB[iColor][1];
            int b = colorRGB[iColor][2];
            colorWipe(CRGB(r, g, b), 50);
            return;
        }
    }
}

void setup(void)
{
    Serial.begin(115200);

    WiFi.begin(SECRET_SSID, SECRET_PASS);
    ThingSpeak.begin(client);

    FastLED.addLeds < WS2811, LED_PIN, BRG > (leds, NUM_LEDS);
    
    // startup animation
    for (int i = 0; i < 512; i++) {
        for (int l = 0; l < NUM_LEDS; l++) {
            leds[l] = CHSV(i + 4 * l, 255, 255);
        }
        FastLED.show();
        delay(5);
    }
}

void loop(void)
{
    String color = ThingSpeak.readStringField(1417, 1);

    // Check the status of the read operation to see if it was successful
    int statusCode = ThingSpeak.getLastReadStatus();
    if (statusCode == 200) {
        Serial.println("Counter: " + color);
        setColor(color);
    } else {
        Serial.println("Problem reading channel. HTTP error code " + String(statusCode));
    }

    delay(10000);
}


