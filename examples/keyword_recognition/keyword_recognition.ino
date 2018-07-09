char output_class[6][8] = {"Silence", "Unknown", "left", "right", "on", "off"};


#include <MFCC.h>
#include <I2S.h>
#include "kws.h"
// currently only works with recording window length of 1
int recording_win = 1;
int averaging_window_len = 6;
KWS *kws;

#define NUM_FEATURES 10

// size of the FFT to compute
const int fftSize = 512;

// int8_t features[NUM_FEATURES];

// MFCC mfcc(fftSize, 10, 2);
int callbackCounter = 0;
void callback()
{
  callbackCounter++;
  kws->extract_features();kws->classify();
  for (int i = 0; i < 6; i++)
  {
    Serial.print(kws->averaged_output[i]);
    Serial.print("\t");
  }
  Serial.println();


  // mfcc.getMfccFeatues(features);
  // delay(10);
  // for (int i = 0; i < NUM_FEATURES; i++)
  // {
  //   Serial.print(features[i]);
  //   Serial.print("\t");
  // }
  // Serial.println();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("begins");
  I2S.downScaleByTwo();
  I2S.begin(32000); // adafruit mic only supports 32khz to 64khz
  // mfcc.input(&I2S);
  kws = new KWS(&I2S, recording_win, averaging_window_len);
  I2S.configureInterrupt(callback);
}

void loop()
{
  // Serial.print(I2S._debugCounter);
  // Serial.print("\t");
  // Serial.println(callbackCounter);

}
