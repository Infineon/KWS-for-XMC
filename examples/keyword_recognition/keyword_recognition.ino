#if !defined(XMC4700_Relax_Kit)
#error "Board might not be compatible"
#else  
  
#include <I2S.h>
#include "kws.h"
#define NUM_RECORDING_WINDOW 4
#define AVG_WINDOW_LEN 2
#define KWS_THRESHOLD 80

char output_class[6][8] = {"Silence", "Unknown", "left", "right", "on", "off"};
KWS *kws;

int recording_window_no = 0;
void i2s_buffer_full_intr()
{
  recording_window_no++;
  kws->extract_features();
}

void setup()
{
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
/*   Serial.begin(115200);
  Serial.println("begins"); */
  I2S.begin(32000); // adafruit mic only supports 32khz to 64khz
  I2S.downScaleByTwo();
  kws = new KWS(&I2S, AVG_WINDOW_LEN);
  I2S.configureInterrupt(i2s_buffer_full_intr);
}

void loop()
{
  if (recording_window_no == NUM_RECORDING_WINDOW)
  {
    recording_window_no = 0;
    kws->classify();
    int max_ind = kws->get_top_class();
    int val = kws->averaged_output[max_ind];

    if (val > KWS_THRESHOLD)
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
      case 4:
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, HIGH);
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
/*     for (int i = 0; i < 6; i++)
    {
      Serial.print(kws->averaged_output[i]);
      Serial.print("\t");
    }
    Serial.println(); */
  }
}
#endif