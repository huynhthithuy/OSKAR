/*
 * Copyright (c) 2011, The University of Oxford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Oxford nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OSKAR_CUDAF_SINC_H_
#define OSKAR_CUDAF_SINC_H_

/**
 * @file oskar_cudaf_sinc.h
 */

#include "utility/oskar_cuda_eclipse.h"

/**
 * @brief
 * CUDA function to evaluate sinc(x) (single precision).
 *
 * @details
 * This inline CUDA device function evaluates sinc(x) = sin(x) / x.
 *
 * @param[in] x Function argument.
 */
__device__ __forceinline__ float oskar_cudaf_sinc_f(float a)
{
    return (a == 0.0f) ? 1.0f : sinf(a) / a;
}

/**
 * @brief
 * CUDA function to evaluate sinc(x) (double precision).
 *
 * @details
 * This inline CUDA device function evaluates sinc(x) = sin(x) / x.
 *
 * @param[in] x Function argument.
 */
__device__ __forceinline__ float oskar_cudaf_sinc_d(float a)
{
    return (a == 0.0) ? 1.0 : sin(a) / a;
}

#endif // OSKAR_CUDAF_SINC_H_