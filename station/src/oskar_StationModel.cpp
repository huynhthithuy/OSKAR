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


#include "station/oskar_StationModel.h"
#include "station/oskar_station_model_copy.h"
#include "station/oskar_station_model_free.h"
#include "station/oskar_station_model_init.h"
#include "station/oskar_station_model_load.h"
#include "station/oskar_station_model_location.h"
#include "station/oskar_station_model_multiply_by_wavenumber.h"
#include "station/oskar_station_model_resize.h"
#include "station/oskar_station_model_check_mem.h"
#include "station/oskar_station_model_type.h"

oskar_StationModel::oskar_StationModel(int type, int location, int n_elements)
: num_elements(n_elements),
  child(NULL),
  parent(NULL),
  element_pattern(NULL)
{
    if (oskar_station_model_init(this, type, location, n_elements))
        throw "Error in oskar_station_model_init";
}

oskar_StationModel::oskar_StationModel(const oskar_StationModel* other,
        int location)
: num_elements(other->num_elements),
  child(NULL),
  parent(NULL),
  element_pattern(NULL)
{
    if (oskar_station_model_init(this, other->x.type(), location,
            other->num_elements))
        throw "error in oskar_station_model_init";
    if (oskar_station_model_copy(this, other))
        throw "error in oskar_station_model_copy";
}

oskar_StationModel::~oskar_StationModel()
{
    oskar_station_model_free(this);
}

int oskar_StationModel::copy_to(oskar_StationModel* other)
{
    return oskar_station_model_copy(other, this); // Copy this to other.
}

int oskar_StationModel::load(const char* filename)
{
    return oskar_station_model_load(this, filename);
}

int oskar_StationModel::location() const
{
    return oskar_station_model_location(this);
}

int oskar_StationModel::resize(int n_elements)
{
    return oskar_station_model_resize(this, n_elements);
}

int oskar_StationModel::multiply_by_wavenumber(double frequency_hz)
{
    return oskar_station_model_multiply_by_wavenumber(this, frequency_hz);
}

int oskar_StationModel::type() const
{
    return oskar_station_model_type(this);
}

int oskar_StationModel::coord_type() const
{
    return oskar_station_model_coord_type(this);
}

int oskar_StationModel::coord_location() const
{
    return oskar_station_model_coord_location(this);
}