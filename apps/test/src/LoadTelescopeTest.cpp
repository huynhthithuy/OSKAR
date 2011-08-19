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

#include "apps/test/LoadTelescopeTest.h"
#include "apps/oskar_load_telescope.h"
#include "interferometry/oskar_TelescopeModel.h"

#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <cstdio>

void LoadTelescopeTest::test_load()
{
    const char* filename = "temp_telescope.dat";
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    int num_stations = 10;
    for (int i = 0; i < num_stations; ++i)
    {
        out << (float)i + i/10.0 << "," << (float)i - i/10.0 << endl;
    }
    file.close();

    oskar_TelescopeModel telescope;
    oskar_load_telescope(filename, &telescope);

    // Check the data loaded correctly.
    CPPUNIT_ASSERT_EQUAL(num_stations, (int)telescope.num_antennas);
    for (int i = 0; i < num_stations; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL((float)i + i / 10.0, telescope.antenna_x[i], 1.0e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL((float)i - i / 10.0, telescope.antenna_y[i], 1.0e-6);
    }

    QFile::remove(filename);
}