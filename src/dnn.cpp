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
 * Description: Keyword spotting DNN model example code
 */

/* Network Structure 

  10x25 input features
    |
   IP1 : Innerproduct (weights: 250x32)
    |
   IP2 : Innerproduct (weights: 32x32)
    |
   IP3 : Innerproduct (weights: 32x32)
    |
   IP4 : Innerproduct (weights: 32x6)
    |
   12 outputs

*/

#include "dnn.h"

const q7_t DNN::ip1_wt[IP1_WT_DIM]=fc1_W_0;
const q7_t DNN::ip1_bias[IP1_OUT_DIM]=fc1_b_0;
const q7_t DNN::ip2_wt[IP2_WT_DIM]=fc2_W_0;
const q7_t DNN::ip2_bias[IP2_OUT_DIM]=fc2_b_0;
const q7_t DNN::ip3_wt[IP3_WT_DIM]=fc3_W_0;
const q7_t DNN::ip3_bias[IP3_OUT_DIM]=fc3_b_0;
const q7_t DNN::ip4_wt[IP4_WT_DIM]=final_fc_0;
const q7_t DNN::ip4_bias[OUT_DIM]=Variable_0;

DNN::DNN()
{
  scratch_pad = new q7_t[SCRATCH_BUFFER_SIZE];
  ip1_out = scratch_pad;
  ip2_out = ip1_out+IP1_OUT_DIM;
  ip3_out = ip1_out;
  vec_buffer = (q15_t*)(ip1_out+IP1_OUT_DIM+IP2_OUT_DIM);
  frame_len = FRAME_LEN;
  frame_shift = FRAME_SHIFT;
  num_mfcc_features = NUM_MFCC_COEFFS;
  num_frames = NUM_FRAMES;
  num_out_classes = OUT_DIM;
  in_dec_bits = MFCC_DEC_BITS;
}

DNN::~DNN()
{
  delete scratch_pad;
}

// python quant_test.py --model_architecture dnn --model_size_info 48 48 48 --dct_coefficient_count 10 --window_size_ms 32 --window_stride_ms 32 --wanted_words='left,right,on,off' --checkpoint work/DNN/DNN_48/training/best/dnn_8893.ckpt-14000 --data_url= --data_dir=audio/ --act_max 32 0 0 0 0
// INFO:tensorflow:Restoring parameters from work/DNN/DNN_48/training/best/dnn_8893.ckpt-14000
// fc1_W_0 number of wts/bias: (310, 48) dec bits: 8 max: (0.48046875,0.48061433) min: (-0.4609375,-0.46078154)
// fc1_b_0 number of wts/bias: (48,) dec bits: 7 max: (0.6171875,0.61578125) min: (-0.359375,-0.35907987)
// Q5.2 * Q0.8 = Qx.10, Q0.7 <<3 = Q0.10

// fc2_W_0 number of wts/bias: (48, 48) dec bits: 8 max: (0.44921875,0.44886285) min: (-0.37890625,-0.379192)
// fc2_b_0 number of wts/bias: (48,) dec bits: 8 max: (0.47265625,0.47408545) min: (-0.33984375,-0.34163097)
// 
// fc3_W_0 number of wts/bias: (48, 48) dec bits: 8 max: (0.37109375,0.37228742) min: (-0.3984375,-0.39983305)
// fc3_b_0 number of wts/bias: (48,) dec bits: 7 max: (0.515625,0.5190745) min: (-0.28125,-0.28058767)
// 
// final_fc_0 number of wts/bias: (48, 6) dec bits: 7 max: (0.484375,0.4828463) min: (-0.53125,-0.52936935)
// Variable_0 number of wts/bias: (6,) dec bits: 7 max: (0.5390625,0.5386016) min: (-0.109375,-0.11157851)
void DNN::run_nn(q7_t* in_data, q7_t* out_data)
{
	// Run all layers
	
	// IP1 
	arm_fully_connected_q7(in_data, ip1_wt, IN_DIM, IP1_OUT_DIM, 3, 7, ip1_bias, ip1_out, vec_buffer);
        // RELU1
	arm_relu_q7(ip1_out, IP1_OUT_DIM);

	// IP2 
	arm_fully_connected_q7(ip1_out, ip2_wt, IP1_OUT_DIM, IP2_OUT_DIM, 2, 8, ip2_bias, ip2_out, vec_buffer);
        // RELU2
	arm_relu_q7(ip2_out, IP2_OUT_DIM);

	// IP3 
	arm_fully_connected_q7(ip2_out, ip3_wt, IP2_OUT_DIM, IP3_OUT_DIM, 3, 7, ip3_bias, ip3_out, vec_buffer);
        // RELU3
	arm_relu_q7(ip3_out, IP3_OUT_DIM);

	// IP4 
	arm_fully_connected_q7(ip3_out, ip4_wt, IP3_OUT_DIM, OUT_DIM, 2, 7, ip4_bias, out_data, vec_buffer);

}


