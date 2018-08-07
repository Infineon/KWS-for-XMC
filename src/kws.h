/*
 * Copyright (C) 2018 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __KWS_H__
#define __KWS_H__

#include <MFCC.h>
#include "dnn/dnn.h"

#define SINGLE_RECORDING_WINDOW 1
class KWS
{
public:
  KWS();
  ~KWS();
  // if default variables unmodified it is assumed that the input signal is static
  KWS(SignalInput *signalInput, int slidingWindowLen = 1);
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

  // (num_frames * num_mfcc_features) mfcc values are used as nn input
  int num_frames; 
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