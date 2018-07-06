#ifndef __KWS_H__
#define __KWS_H__

#include <MFCC.h>
#include "dnn.h"

class KWS
{

public:
  ~KWS();
  KWS(SignalInput *signalInput, int numRecordingWindow = NUM_FRAMES);
  void extract_features();
  void classify();
  void average_predictions();

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
  MFCC *mfcc;
  // TODO: switch between cnn/dnn
  DNN *dnn;
  //DS_CNN ds_cnn;
  int num_recording_win;
  // number of predictions taken to compute the average
  int sliding_window_len;
};

#endif