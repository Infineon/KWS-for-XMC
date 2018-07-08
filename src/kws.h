#ifndef __KWS_H__
#define __KWS_H__

#include <MFCC.h>
#include "dnn.h"

class KWS
{

public:
  KWS();
  ~KWS();
  // if default variables unmodified it is assumed that the input signal is static
  KWS(SignalInput *signalInput, int numRecordingWindow = NUM_FRAMES, int slidingWindowLen = 1);
  void extract_features();
  void classify();
  int get_top_class();

  int audio_block_size;
  int audio_buffer_size;
  q7_t *averaged_output;

protected:
  MFCC *mfcc;
  // TODO: switch between cnn/dnn
  DNN *dnn;
  //DS_CNN ds_cnn;

  q7_t *mfcc_buffer;
  q7_t *output;
  q7_t *predictions;

  int num_frames; // num_frames * num_mfcc_features values are used as nn input
  int num_mfcc_features;
  int frame_len;
  // no overlapping windows implemented yet
  // int frame_shift;
  int num_out_classes;
  int num_recording_win;
  // number of predictions taken to compute the average
  int sliding_window_len;
};

#endif