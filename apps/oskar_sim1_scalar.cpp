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

#include "sky/oskar_SkyModel.h"
#include "sky/oskar_sky_model_load.h"
#include "sky/oskar_date_time_to_mjd.h"

#include "station/oskar_StationModel.h"

#include "interferometry/oskar_SimTime.h"
#include "interferometry/oskar_TelescopeModel.h"
#include "interferometry/oskar_interferometer1_scalar.h"
#include "interferometry/oskar_VisData.h"

#include "apps/lib/oskar_load_telescope.h"
#include "apps/lib/oskar_load_stations.h"
#include "apps/lib/oskar_Settings.h"

#ifndef OSKAR_NO_MS
#include "apps/lib/oskar_write_ms.h"
#endif

#include "utility/oskar_cuda_device_info.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <QtCore/QTime>

using namespace std;

int sim1_d(const oskar_Settings& settings);
int sim1_f(const oskar_Settings& settings);


int main(int argc, char** argv)
{
    // $> oskar_sim1_scalar settings_file.txt
    if (argc != 2)
    {
        fprintf(stderr, "ERROR: Missing command line arguments.\n");
        fprintf(stderr, "Usage:  $ oskar_sim1_scalar [settings file]\n");
        return EXIT_FAILURE;
    }

    oskar_Settings settings;
    if (!settings.load(QString(argv[1]))) return EXIT_FAILURE;
    settings.print();

    QTime timer;
    timer.start();

    // Double precision.
    if (settings.double_precision())
    {
        sim1_d(settings);
    }

    // Single precision.
    else
    {
        sim1_f(settings);
    }

    printf("= Completed simulation after %f seconds.\n", timer.elapsed() / 1.0e3);

    return EXIT_SUCCESS;
}



int sim1_d(const oskar_Settings& settings)
{
    // ============== Load input data =========================================
    oskar_SkyModelGlobal_d sky;
    oskar_sky_model_load_d(settings.sky_file().toLatin1().data(), &sky);
    printf("Sky model contains %d sources.\n", sky.num_sources);

    oskar_TelescopeModel_d telescope;
    oskar_load_telescope_d(settings.telescope_file().toLatin1().data(),
            settings.longitude_rad(), settings.latitude_rad(), &telescope);

    oskar_StationModel_d* stations;
    const char* station_dir = settings.station_dir().toLatin1().data();
    unsigned num_stations = oskar_load_stations_d(station_dir, &stations,
            &telescope.identical_stations);

    if (num_stations != telescope.num_antennas)
    {
        fprintf(stderr, "ERROR: Error loading telescope geometry.\n");
        return EXIT_FAILURE;
    }

    // ============== Simulation loop =========================================
    for (int i = 0; i < settings.obs().num_channels(); ++i)
    {
        double frequency = settings.obs().frequency(i);

        printf("- Frequency: %e Hz.\n", frequency);

        // Allocate visibility data.
        const oskar_SimTime* time = settings.obs().sim_time();
        oskar_VisData_d vis;
        int num_baselines = num_stations * (num_stations-1) / 2;
        oskar_allocate_vis_data_d(num_baselines * time->num_vis_dumps, &vis);

        oskar_interferometer1_scalar_d(telescope, stations, sky,
                settings.obs().ra0_rad(), settings.obs().dec0_rad(),
                time->obs_start_mjd_utc, time->obs_length_days,
                time->num_vis_dumps, time->num_vis_ave,
                time->num_fringe_ave, frequency,
                settings.obs().channel_bandwidth(),
                settings.disable_station_beam(), &vis);

        printf("= Number of visibility points generated: %i\n", vis.num_samples);

        // Write visibility binary file.
        if (!settings.obs().oskar_vis_filename().isEmpty())
        {
            QString vis_file = settings.obs().oskar_vis_filename() + "_channel_" + QString::number(i) + ".dat";
            printf("= Writing OSKAR visibility data file: %s\n",
                    vis_file.toLatin1().data());
            oskar_write_vis_data_d(vis_file.toLatin1().data(), &vis);
        }

        // Write MS.
#ifndef OSKAR_NO_MS
        if (!settings.obs().ms_filename().isEmpty())
        {
            QString ms_file = settings.obs().ms_filename() + "_channel_" + QString::number(i) + ".ms";
            printf("= Writing Measurement Set: %s\n", ms_file.toLatin1().data());
            oskar_write_ms_d(ms_file.toLatin1().data(), &settings, &vis, i, true);
        }
#endif

        oskar_free_vis_data_d(&vis);
    }

    // ============== Cleanup =================================================
    free(sky.RA);
    free(sky.Dec);
    free(sky.I);
    free(sky.Q);
    free(sky.U);
    free(sky.V);
    free(telescope.antenna_x);
    free(telescope.antenna_y);
    free(telescope.antenna_z);
    for (unsigned i = 0; i < num_stations; ++i)
    {
        free(stations[i].antenna_x);
        free(stations[i].antenna_y);
    }
    free(stations);

    return EXIT_SUCCESS;
}




int sim1_f(const oskar_Settings& settings)
{
    // ============== Load input data =========================================
    oskar_SkyModelGlobal_f sky;
    oskar_sky_model_load_f(settings.sky_file().toLatin1().data(), &sky);
    printf("Sky model contains %d sources.\n", sky.num_sources);

    oskar_TelescopeModel_f telescope;
    oskar_load_telescope_f(settings.telescope_file().toLatin1().data(),
            settings.longitude_rad(), settings.latitude_rad(), &telescope);

    oskar_StationModel_f* stations;
    const char* station_dir = settings.station_dir().toLatin1().data();
    unsigned num_stations = oskar_load_stations_f(station_dir, &stations,
            &telescope.identical_stations);

    if (num_stations != telescope.num_antennas)
    {
        fprintf(stderr, "ERROR: Error loading telescope geometry.\n");
        return EXIT_FAILURE;
    }

    // ============== Simulation loop =========================================
    for (int i = 0; i < settings.obs().num_channels(); ++i)
    {
        float frequency = settings.obs().frequency(i);

        printf("- Frequency: %e Hz.\n", frequency);

        // Allocate visibility data.
        const oskar_SimTime* time = settings.obs().sim_time();
        oskar_VisData_f vis;
        int num_baselines = num_stations * (num_stations-1) / 2;
        oskar_allocate_vis_data_f(num_baselines * time->num_vis_dumps, &vis);

        oskar_interferometer1_scalar_f(telescope, stations, sky,
                settings.obs().ra0_rad(), settings.obs().dec0_rad(),
                time->obs_start_mjd_utc, time->obs_length_days,
                time->num_vis_dumps, time->num_vis_ave,
                time->num_fringe_ave, frequency,
                settings.obs().channel_bandwidth(),
                settings.disable_station_beam(), &vis);

        printf("= Number of visibility points generated: %i\n", vis.num_samples);

        // Write visibility binary file.
        if (!settings.obs().oskar_vis_filename().isEmpty())
        {
            QString vis_file = settings.obs().oskar_vis_filename() +
                    "_channel_" + QString::number(i) + ".dat";
            printf("= Writing OSKAR visibility data file: %s\n",
                    vis_file.toLatin1().data());
            oskar_write_vis_data_f(vis_file.toLatin1().data(), &vis);
        }

        // Write MS.
#ifndef OSKAR_NO_MS
        if (!settings.obs().ms_filename().isEmpty())
        {
            QString ms_file = settings.obs().ms_filename() + "_channel_" + QString::number(i) + ".ms";
            printf("= Writing Measurement Set: %s\n", ms_file.toLatin1().data());
            oskar_write_ms_f(ms_file.toLatin1().data(), &settings, &vis, i, true);
        }
#endif

        oskar_free_vis_data_f(&vis);
    }

    // ============== Cleanup =================================================
    free(sky.RA);
    free(sky.Dec);
    free(sky.I);
    free(sky.Q);
    free(sky.U);
    free(sky.V);
    free(sky.reference_freq);
    free(sky.spectral_index);
    free(telescope.antenna_x);
    free(telescope.antenna_y);
    free(telescope.antenna_z);
    for (unsigned i = 0; i < num_stations; ++i)
    {
        free(stations[i].antenna_x);
        free(stations[i].antenna_y);
    }
    free(stations);

    return EXIT_SUCCESS;
}

