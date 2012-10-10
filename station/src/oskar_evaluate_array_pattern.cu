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

#include "oskar_global.h"

#include "math/cudak/oskar_cudak_dftw_o2c_2d.h"
#include "math/cudak/oskar_cudak_dftw_o2c_3d.h"
#include "station/oskar_evaluate_array_pattern.h"
#include "station/oskar_evaluate_element_weights.h"
#include "station/oskar_station_model_location.h"
#include "station/oskar_station_model_type.h"
#include "station/oskar_StationModel.h"
#include "utility/oskar_Mem.h"
#include "utility/oskar_mem_element_size.h"
#include "utility/oskar_mem_type_check.h"
#include "utility/oskar_mem_realloc.h"

#include <cstdlib>
#include <cmath>

#ifdef __cplusplus
extern "C" {
#endif

int oskar_evaluate_array_pattern(oskar_Mem* beam,
        const oskar_StationModel* station, double l_beam, double m_beam,
        double n_beam, int num_points, const oskar_Mem* l, const oskar_Mem* m,
        const oskar_Mem* n, oskar_Mem* weights, oskar_Mem* weights_error,
        oskar_CurandState* curand_state)
{
    int error = 0, num_antennas;
    size_t element_size;

    // Sanity check on inputs.
    if (!beam || !station || !l || !m || !n || !weights || !weights_error ||
            !curand_state)
        return OSKAR_ERR_INVALID_ARGUMENT;

    // Check that all arrays are on the GPU.
    if (oskar_station_model_location(station) != OSKAR_LOCATION_GPU ||
            beam->location != OSKAR_LOCATION_GPU ||
            l->location != OSKAR_LOCATION_GPU ||
            m->location != OSKAR_LOCATION_GPU ||
            n->location != OSKAR_LOCATION_GPU ||
            weights->location != OSKAR_LOCATION_GPU ||
            weights_error->location != OSKAR_LOCATION_GPU)
        return OSKAR_ERR_BAD_LOCATION;

    // Check that the antenna coordinates have been scaled by the wavenumber.
    if (station->coord_units != OSKAR_RADIANS)
        return OSKAR_ERR_BAD_UNITS;

    // Check that the pattern array is a complex scalar.
    if (!oskar_mem_is_complex(beam->type) || oskar_mem_is_matrix(beam->type))
        return OSKAR_ERR_BAD_DATA_TYPE;

    // Get the dimensions.
    num_antennas = station->num_elements;
    element_size = oskar_mem_element_size(l->type);

    // Resize output array if required.
    if (beam->num_elements < num_points)
        oskar_mem_realloc(beam, num_points, &error);

    // Generate the beamforming weights.
    oskar_evaluate_element_weights(weights, weights_error, station,
            l_beam, m_beam, n_beam, curand_state, &error);

    // Check if safe to proceed.
    if (error) return error;

    // Double precision.
    if (oskar_station_model_type(station) == OSKAR_DOUBLE &&
            beam->type == OSKAR_DOUBLE_COMPLEX &&
            weights->type == OSKAR_DOUBLE_COMPLEX &&
            l->type == OSKAR_DOUBLE &&
            m->type == OSKAR_DOUBLE &&
            n->type == OSKAR_DOUBLE)
    {
        int num_threads = 256;
        int num_blocks = (num_antennas + num_threads - 1) / num_threads;

        // Evaluate beam using DFT.
        if (station->array_is_3d)
        {
            int antennas_per_chunk = 384;  // Should be multiple of 16.
            num_blocks = (num_points + num_threads - 1) / num_threads;
            size_t shared_mem_size = 5 * antennas_per_chunk * element_size;
            oskar_cudak_dftw_o2c_3d_d
            OSKAR_CUDAK_CONF(num_blocks, num_threads, shared_mem_size)
            (num_antennas, station->x_signal, station->y_signal,
                    station->z_signal, *weights, num_points, *l, *m, *n,
                    antennas_per_chunk, *beam);
        }
        else
        {
            int antennas_per_chunk = 448;  // Should be multiple of 16.
            num_blocks = (num_points + num_threads - 1) / num_threads;
            size_t shared_mem_size = 4 * antennas_per_chunk * element_size;
            oskar_cudak_dftw_o2c_2d_d
            OSKAR_CUDAK_CONF(num_blocks, num_threads, shared_mem_size)
            (num_antennas, station->x_signal, station->y_signal,
                    *weights, num_points, *l, *m, antennas_per_chunk, *beam);
        }
    }

    // Single precision.
    else if (oskar_station_model_type(station) == OSKAR_SINGLE &&
            beam->type == OSKAR_SINGLE_COMPLEX &&
            weights->type == OSKAR_SINGLE_COMPLEX &&
            l->type == OSKAR_SINGLE &&
            m->type == OSKAR_SINGLE &&
            n->type == OSKAR_SINGLE)
    {
        int num_threads = 256;
        int num_blocks = (num_antennas + num_threads - 1) / num_threads;

        // Evaluate beam using DFT.
        if (station->array_is_3d)
        {
            // Evaluate beam pattern for each source.
            int antennas_per_chunk = 800;  // Should be multiple of 16.
            num_blocks = (num_points + num_threads - 1) / num_threads;
            size_t shared_mem_size = 5 * antennas_per_chunk * element_size;
            oskar_cudak_dftw_o2c_3d_f
            OSKAR_CUDAK_CONF(num_blocks, num_threads, shared_mem_size)
            (num_antennas, station->x_signal, station->y_signal,
                    station->z_signal, *weights, num_points, *l, *m, *n,
                    antennas_per_chunk, *beam);
        }
        else
        {
            // Evaluate beam pattern for each source.
            int antennas_per_chunk = 896;  // Should be multiple of 16.
            num_blocks = (num_points + num_threads - 1) / num_threads;
            size_t shared_mem_size = 4 * antennas_per_chunk * element_size;
            oskar_cudak_dftw_o2c_2d_f
            OSKAR_CUDAK_CONF(num_blocks, num_threads, shared_mem_size)
            (num_antennas, station->x_signal, station->y_signal, *weights,
                    num_points, *l, *m, antennas_per_chunk, *beam);
        }
    }
    else
    {
        return OSKAR_ERR_BAD_DATA_TYPE;
    }

    // Return any CUDA error.
    cudaDeviceSynchronize();
    return cudaPeekAtLastError();
}

#ifdef __cplusplus
}
#endif