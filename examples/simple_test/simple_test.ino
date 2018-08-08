#if !defined(XMC4700_Relax_Kit)
#error "Board might not be compatible"
#else  
char output_class[6][8] = {"Silence", "Unknown", "left", "right", "on", "off"};

#include "staticAudioInput.h"
#include "kws.h"
#include "audio_left/left0.h"

#define AUDIO_LENGTH 16000
int16_t audio_buffer[AUDIO_LENGTH] = WAV_LEFT;

void setup()
{
  Serial.begin(9600);
  Serial.println("Printing probabilities for silence, unknown, left, right, on and off");
  staticAudioInput audioInput(audio_buffer, AUDIO_LENGTH);

  KWS kws(&audioInput);
  kws.extract_features_static(); //extract mfcc features
  kws.classify();         //classify using dnn
  float val;
  for (int i = 0; i < 6; i++)
  {
    val = kws.averaged_output[i];
    Serial.print(val / 128);
    Serial.print("\t");
  }
  Serial.println();
  int max_ind = kws.get_top_class();
  Serial.print("Prediction: ");
  Serial.println(output_class[max_ind]);
}

void loop() {}
#endif