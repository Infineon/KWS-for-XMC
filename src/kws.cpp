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

/*
 * Description: Keyword spotting example code using MFCC feature extraction
 * and neural network. 
 */

#include "kws.h"

KWS::~KWS()
{
    delete mfcc_buffer;
}

KWS::KWS(SignalInput *signalInput, int numRecordingWindow)
{
    num_mfcc_features = NUM_MFCC_COEFFS;
    frame_len = FRAME_LEN;
    num_frames = NUM_FRAMES;
    frame_shift = FRAME_SHIFT;
    num_recording_win = numRecordingWindow;
    num_out_classes = OUT_DIM;
    
    mfcc = new MFCC(FRAME_LEN, num_mfcc_features);
    mfcc->input(signalInput);
    mfcc_buffer = new q7_t[num_frames * num_mfcc_features];

    dnn = new DNN();
    output = new q7_t[num_out_classes];
}

void KWS::extract_features()
{
    if (num_frames > num_recording_win)
    {
        //move old features left
        memmove(mfcc_buffer, mfcc_buffer + (num_recording_win * num_mfcc_features), (num_frames - num_recording_win) * num_mfcc_features);
    }
    //compute features only for the newly recorded audio
    int mfcc_buffer_head = (num_frames - num_recording_win) * num_mfcc_features;
    Serial.println(mfcc_buffer_head);
    for (int f = 0; f < 25; f++)
    {
        mfcc->getMfccFeatues(mfcc_buffer + mfcc_buffer_head);
        mfcc_buffer_head += num_mfcc_features;
    }
}

void KWS::classify(){
    dnn->run_nn(mfcc_buffer, output);
    arm_softmax_q7(output,num_out_classes,output);
}

void KWS::average_predictions()
{}