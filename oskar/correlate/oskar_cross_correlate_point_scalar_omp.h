/*
 * Copyright (c) 2014-2015, The University of Oxford
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

#ifndef OSKAR_CROSS_CORRELATE_POINT_SCALAR_OMP_H_
#define OSKAR_CROSS_CORRELATE_POINT_SCALAR_OMP_H_

/**
 * @file oskar_cross_correlate_point_scalar_omp.h
 */

#include <oskar_global.h>
#include <utility/oskar_vector_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * Correlate function for point sources, scalar version (single precision).
 *
 * @details
 * Forms visibilities on all baselines by correlating Jones scalars for pairs
 * of stations and summing along the source dimension.
 *
 * @param[in] num_sources    Number of sources.
 * @param[in] num_stations   Number of stations.
 * @param[in] jones          Matrix of Jones scalars to correlate.
 * @param[in] source_I       Source Stokes I values, in Jy.
 * @param[in] source_l       Source l-direction cosines from phase centre.
 * @param[in] source_m       Source m-direction cosines from phase centre.
 * @param[in] source_n       Source n-direction cosines from phase centre.
 * @param[in] station_u      Station u-coordinates, in metres.
 * @param[in] station_v      Station v-coordinates, in metres.
 * @param[in] station_w      Station w-coordinates, in metres.
 * @param[in] uv_min_lambda  Minimum allowed UV length, in wavelengths.
 * @param[in] uv_max_lambda  Maximum allowed UV length, in wavelengths.
 * @param[in] inv_wavelength Inverse of the wavelength, in metres.
 * @param[in] frac_bandwidth Bandwidth divided by frequency.
 * @param[in,out] vis        Modified output complex visibilities.
 */
OSKAR_EXPORT
void oskar_cross_correlate_point_scalar_omp_f(int num_sources, int num_stations,
        const float2* jones, const float* source_I, const float* source_l,
        const float* source_m, const float* source_n, const float* station_u,
        const float* station_v, const float* station_w, float uv_min_lambda,
        float uv_max_lambda, float inv_wavelength, float frac_bandwidth,
        float2* vis);

/**
 * @brief
 * Correlate function for point sources, scalar version (double precision).
 *
 * @details
 * Forms visibilities on all baselines by correlating Jones scalars for pairs
 * of stations and summing along the source dimension.
 *
 * @param[in] num_sources    Number of sources.
 * @param[in] num_stations   Number of stations.
 * @param[in] jones          Matrix of Jones scalars to correlate.
 * @param[in] source_I       Source Stokes I values, in Jy.
 * @param[in] source_l       Source l-direction cosines from phase centre.
 * @param[in] source_m       Source m-direction cosines from phase centre.
 * @param[in] source_n       Source n-direction cosines from phase centre.
 * @param[in] station_u      Station u-coordinates, in metres.
 * @param[in] station_v      Station v-coordinates, in metres.
 * @param[in] station_w      Station w-coordinates, in metres.
 * @param[in] uv_min_lambda  Minimum allowed UV length, in wavelengths.
 * @param[in] uv_max_lambda  Maximum allowed UV length, in wavelengths.
 * @param[in] inv_wavelength Inverse of the wavelength, in metres.
 * @param[in] frac_bandwidth Bandwidth divided by frequency.
 * @param[in,out] vis        Modified output complex visibilities.
 */
OSKAR_EXPORT
void oskar_cross_correlate_point_scalar_omp_d(int num_sources, int num_stations,
        const double2* jones, const double* source_I, const double* source_l,
        const double* source_m, const double* source_n, const double* station_u,
        const double* station_v, const double* station_w, double uv_min_lambda,
        double uv_max_lambda, double inv_wavelength, double frac_bandwidth,
        double2* vis);

#ifdef __cplusplus
}
#endif

#endif /* OSKAR_CROSS_CORRELATE_POINT_SCALAR_OMP_H_ */
