/*
 * libdatachannel streamer example
 * Copyright (c) 2020 Filip Klembara (in2core)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef fileparser_hpp
#define fileparser_hpp

#include <string>
#include <vector>
#include "stream.hpp"
#include <Processing.NDI.Advanced.h>


class FileParser: public StreamSource {
    std::string directory;
    std::string extension;
    uint32_t counter = -1;
    bool loop;
    uint64_t loopTimestampOffset = 0;
	NDIlib_recv_instance_t pNDI_recv = nullptr;

public:
    uint64_t sampleDuration_us;
    virtual void start();
    virtual void stop();
    FileParser(std::string directory, std::string extension, uint32_t samplesPerSecond, bool loop);
	~FileParser(); 
    virtual void loadNextSample();
};

#endif /* fileparser_hpp */
