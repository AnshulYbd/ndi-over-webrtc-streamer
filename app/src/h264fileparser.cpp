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

#include "h264fileparser.hpp"
#include "rtc/rtc.hpp"

#include <fstream>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
using namespace std;


H264FileParser::H264FileParser(string directory, uint32_t fps, bool loop): FileParser(directory, ".h264", fps, loop) { }
struct offset {
	int begin = -1;
	int end = -1;
	offset(int s, int e) : begin(s), end(e) {}
	offset(){}
};

int H264FileParser::findNal(uint8_t *start, uint8_t *end ) noexcept
{
	uint8_t* message = start;
	int i = 0;
	uint8_t *p = start ;

	offset of5, of6, of7, of8, of8_2;
	int type = 0, prevType = 0;
	while (p +i <= end - 3) 
	{
		while ((p +i <= end - 3) && (p[i] || p[i + 1] || p[i+2] != 1) ){
			++i;
		}

		if (p + i > end - 3) {
			if (of5.end == -1)//couldnt reach to the end, may be a split frame,
				of5.end = end - start;
			previousUnitType5 = {sample.begin() + of5.begin, sample.begin() + of5.end};
			previousUnitType6 = {sample.begin() + of6.begin, sample.begin() + of6.end};
			previousUnitType7 = {sample.begin() + of7.begin, sample.begin() + of7.end};
			previousUnitType8 = { sample.begin() + of8.begin, sample.begin() + of8.end };
			if (of8_2.end == -1)// new h264 format webrtc seems blind, but accomodate it here.
				of8_2.end = end - start;
			if(of8_2.begin != -1)
				previousUnitType8_2 = {sample.begin() + of8_2.begin, sample.begin() + of8_2.end};

			return i;
		}
		
		auto at = i - 1;

		type = (int)(p[at + 4] & 0x1F);
		switch (type) {
		case 7: {//Sequence Parameter Set (SPS) - 7
			of7.begin = at + 4;
		} break;
		case 8: {//Picture Parameter Set (PPS)- 8
			if (of8.begin == -1)
				of8.begin = at + 4;
			else
				of8_2.begin = at + 4;
		} break;
		case 5: {//Instantaneous Decoder Refresh - 5
			of5.begin = at + 4;
		} break;
		case 6: {//Access Unit Delimiter (AUD) - 6
			of6.begin = at + 4;
		} break;
		case 1:
			return 0;
		default:
			break;
		}
		switch (prevType) {
		case 7: {
			of7.end = at - 1;
		} break;
		case 8: {
			if (of8.end == -1)
				of8.end = at - 1;
			else
				of8_2.end = at - 1;
		} break;
		case 5: {
			of5.end = at - 1;
		} break;
		case 6: { // Access Unit Delimiter (AUD)
			of6.end = at - 1;
		} break;
		default:
			break;
		}
		prevType = type;
		++i; 
	}

	/* Include 8 bits leading zero */
	// if (p>start && *(p-1)==0)
	//    return (p-1);
	return i-1;
}
void H264FileParser::loadNextSample() {
    FileParser::loadNextSample();

#if defined LIVE_NDI_CAM_FEED && LIVE_NDI_CAM_FEED == 0
    size_t i = 0;
    while (i < sample.size()) {
        assert(i + 4 < sample.size());
		auto nextUnitPtr = sample.data() + i;
        auto lengthPtr = (uint32_t *) (sample.data() + i);//19 00 00 00 
        uint32_t length = ntohl(*lengthPtr);//00000019
        auto naluStartIndex = i + 4;
        auto naluEndIndex = naluStartIndex + length;
        assert(naluEndIndex <= sample.size());
        auto header = reinterpret_cast<rtc::NalUnitHeader *>(sample.data() + naluStartIndex);
        auto type = header->unitType();
		auto d = ((uint8_t)(*(nextUnitPtr + 4) )& 0x1F );

        switch (type) {
            case 7:
                previousUnitType7 = {sample.begin() + i, sample.begin() + naluEndIndex};
                break;
            case 8:
                previousUnitType8 = {sample.begin() + i, sample.begin() + naluEndIndex};;
                break;
            case 5:
                previousUnitType5 = {sample.begin() + i, sample.begin() + naluEndIndex};;
                break;
        }
        i = naluEndIndex;
    }

#else 
	auto start = sample.data();
	auto end = sample.data() + sample.size() -1;

	findNal((uint8_t *)start, (uint8_t*)end);

#endif
}

vector<byte> H264FileParser::initialNALUS() {
    vector<byte> units{};
    if (previousUnitType7.has_value()) {
        auto nalu = previousUnitType7.value();
        units.insert(units.end(), nalu.begin(), nalu.end());
    }
    
	if (previousUnitType8.has_value()) {
		auto nalu = previousUnitType8.value();
		units.insert(units.end(), nalu.begin(), nalu.end());
	}
	
	if (previousUnitType8_2.has_value()) {
		auto nalu = previousUnitType8_2.value();
		units.insert(units.end(), nalu.begin(), nalu.end());
	}

    if (previousUnitType5.has_value()) {
        auto nalu = previousUnitType5.value();
        units.insert(units.end(), nalu.begin(), nalu.end());
    }
	if (previousUnitType6.has_value()) {
		auto nalu = previousUnitType6.value();
		units.insert(units.end(), nalu.begin(), nalu.end());
	}
    return units;
}
