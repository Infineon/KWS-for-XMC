#ifndef __KWS_H__
#define __KWS_H__

#include <MFCC.h>
#include <I2S.h>
#include "nn.h"

class KWS{

public:
  ~KWS();
  void extract_features();
  void classify();
  void average_predictions();
  int get_top_class(q7_t* prediction);
  int16_t* audio_buffer;
  q7_t *mfcc_buffer;
  q7_t *output;
  q7_t *predictions;
  q7_t *averaged_output;
  int num_frames;
  int num_mfcc_features;
  int frame_len;
  int frame_shift;
  int num_out_classes;
  int audio_block_size;
  int audio_buffer_size;

protected:
  KWS();
  void init_kws();
  MFCC *mfcc;
  NN *nn;
  int mfcc_buffer_size;
  int recording_win;
  int sliding_window_len;
  
};

#endif