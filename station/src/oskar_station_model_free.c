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

#include "station/oskar_station_model_free.h"
#include "station/oskar_element_model_free.h"
#include "utility/oskar_mem_free.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int oskar_station_model_free(oskar_StationModel* model)
{
    int error = 0;

    /* Check for sane inputs. */
    if (model == NULL)
        return OSKAR_ERR_INVALID_ARGUMENT;

    /* Free the element data. */
    error = oskar_mem_free(&model->x_signal);
    if (error) return error;
    error = oskar_mem_free(&model->y_signal);
    if (error) return error;
    error = oskar_mem_free(&model->z_signal);
    if (error) return error;
    error = oskar_mem_free(&model->x_weights);
    if (error) return error;
    error = oskar_mem_free(&model->y_weights);
    if (error) return error;
    error = oskar_mem_free(&model->z_weights);
    if (error) return error;
    error = oskar_mem_free(&model->weight);
    if (error) return error;
    error = oskar_mem_free(&model->amp_gain);
    if (error) return error;
    error = oskar_mem_free(&model->amp_gain_error);
    if (error) return error;
    error = oskar_mem_free(&model->phase_offset);
    if (error) return error;
    error = oskar_mem_free(&model->phase_error);
    if (error) return error;

    /* Free the receiver noise data. */
    error = oskar_mem_free(&model->total_receiver_noise);
    if (error) return error;

    /* Free the element pattern data if it exists. */
    if (model->element_pattern)
    {
        error = oskar_element_model_free(model->element_pattern);
        if (error) return error;
        model->element_pattern = NULL;
    }

    /* Recursively free the child stations. */
    if (model->child)
    {
        int i = 0;
        for (i = 0; i < model->num_elements; ++i)
        {
            error = oskar_station_model_free(&(model->child[i]));
            if (error) return error;
        }

        /* Free the array. */
        free(model->child);
        model->child = NULL;
    }

    /* Clear remaining parameters. */
    model->num_elements = 0;
    model->parent = NULL;
    model->longitude_rad = 0.0;
    model->latitude_rad = 0.0;
    model->altitude_metres = 0.0;
    model->ra0_rad = 0.0;
    model->dec0_rad = 0.0;
    model->single_element_model = 0;
    model->bit_depth = 0;

    return 0;
}

#ifdef __cplusplus
}
#endif
