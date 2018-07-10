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
KWS::KWS() {}

KWS::KWS(SignalInput *signalInput, int numRecordingWindow, int slidingWindowLen)
    : num_recording_win(numRecordingWindow), sliding_window_len(slidingWindowLen)
{
    // these values must correspond to the network
    num_mfcc_features = NUM_MFCC_COEFFS;
    frame_len = FRAME_LEN;
    num_frames = NUM_FRAMES;
    // frame_shift = FRAME_SHIFT;
    num_out_classes = OUT_DIM;

    mfcc = new MFCC(frame_len, num_mfcc_features, MFCC_DEC_BITS);
    mfcc->input(signalInput, SAMP_FREQ);
    mfcc_buffer = new q7_t[num_frames * num_mfcc_features];

    dnn = new DNN();
    output = new q7_t[num_out_classes];
    averaged_output = new q7_t[num_out_classes];
    predictions = new q7_t[sliding_window_len * num_out_classes];
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
    for (int f = 0; f < num_recording_win; f++)
    {
        mfcc->getMfccFeatues(mfcc_buffer + mfcc_buffer_head);
        mfcc_buffer_head += num_mfcc_features;
    }
}

void KWS::classify()
{
    dnn->run_nn(mfcc_buffer, output);
    arm_softmax_q7(output, num_out_classes, output);

    //shift right old predictions
    arm_copy_q7((q7_t *)predictions, (q7_t *)(predictions + num_out_classes), (sliding_window_len - 1) * num_out_classes);
    //add new predictions
    arm_copy_q7((q7_t *)output, (q7_t *)predictions, num_out_classes);
    //compute averages
    int sum;
    for (int j = 0; j < num_out_classes; j++)
    {
        sum = 0;
        for (int i = 0; i < sliding_window_len; i++)
            sum += predictions[i * num_out_classes + j];
        averaged_output[j] = (q7_t)(sum / sliding_window_len);
    }
}

int KWS::get_top_class()
{
    int max_ind = 0;
    int max_val = -128;
    for (int i = 0; i < num_out_classes; i++)
    {
        if (max_val < averaged_output[i])
        {
            max_val = averaged_output[i];
            max_ind = i;
        }
    }
    return max_ind;
}