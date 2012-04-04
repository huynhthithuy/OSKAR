/*
 * Copyright (c) 2012, The University of Oxford
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

#include "math/cudak/oskar_cudak_dierckx_bispev_bicubic.h"
#include "math/cudak/oskar_cudaf_dierckx_fpbisp_single_bicubic.h"

// Single precision.

__global__
void oskar_cudak_dierckx_bispev_bicubic_f(const float* tx, const int nx,
        const float* ty, const int ny, const float* c, const int n,
        const float* x, const float* y, const int stride, float* z)
{
    // Get the output position (pixel) ID that this thread is working on.
    const int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i >= n) return;

    // Call device function to evaluate surface.
    oskar_cudaf_dierckx_fpbisp_single_bicubic_f(tx, nx, ty, ny, c,
            x[i], y[i], &z[i * stride]);
}

// Double precision.

__global__
void oskar_cudak_dierckx_bispev_bicubic_d(const double* tx, const int nx,
        const double* ty, const int ny, const double* c, const int n,
        const double* x, const double* y, const int stride, double* z)
{
    // Get the output position (pixel) ID that this thread is working on.
    const int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i >= n) return;

    // Call device function to evaluate surface.
    oskar_cudaf_dierckx_fpbisp_single_bicubic_d(tx, nx, ty, ny, c,
            x[i], y[i], &z[i * stride]);
}