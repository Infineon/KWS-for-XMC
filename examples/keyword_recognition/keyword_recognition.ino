char output_class[6][8] = {"Silence", "Unknown", "left", "right", "on", "off"};

#include <MFCC.h>
#include <I2S.h>
#include "kws.h"
// currently only works with recording window length of 1
int recording_win = 4;
int averaging_window_len = 2;
KWS *kws;

#define NUM_FEATURES 10

// size of the FFT to compute
const int fftSize = 512;

// MFCC mfcc(fftSize, 10, 2);
int callbackCounter = 0;
void callback()
{
  callbackCounter++;
  kws->extract_features();
}

void setup()
{
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(115200);
  Serial.println("begins");
  I2S.begin(32000); // adafruit mic only supports 32khz to 64khz
  I2S.downScaleByTwo();
  kws = new KWS(&I2S, 1, averaging_window_len);
  I2S.configureInterrupt(callback);
}

void loop()
{

  if (callbackCounter == recording_win)
  {
    callbackCounter = 0;
    kws->classify();
    int max_ind = kws->get_top_class();
    int val = kws->averaged_output[max_ind];

    if (val > 80)
    {
      switch (max_ind)
      {
      case 2:
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, LOW);
        break;
      case 3:
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, LOW);
        break;
      case 5:
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        break;
      default:
        break;
      }
      delay(20);
    }
    // see detailed output
    for (int i = 0; i < 6; i++)
    {
      Serial.print(kws->averaged_output[i]);
      Serial.print("\t");
    }
    Serial.println();
  }
}
