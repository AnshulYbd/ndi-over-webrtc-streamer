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

using NALU_TYPE = std::optional<std::vector<std::byte>>;

H264FileParser::H264FileParser(string directory, uint32_t fps, bool loop): FileParser(directory, ".h264", fps, loop) { }
struct offset {
	int begin = -1;
	int end = -1;
	offset(int s, int e) : begin(s), end(e) {}
	offset(){}
};

void H264FileParser::emplaceLastNLU(int beginPos, int endPos ) noexcept
{
	NALU_TYPE nl;
	nl = { sample.begin() + beginPos, sample.begin() + endPos };
	unitTypes.emplace_back(std::move(nl));
}

int H264FileParser::findNal(uint8_t *start, uint8_t *end ) noexcept
{
	uint8_t* message = start;
	int i = 0;
	uint8_t *p = start ;
	//std::cout << "\nNAL Type: ";
	int type = -1, prevType = -1, prevPos = -1;
	while (p +i <= end - 3) 
	{
		while ((p +i <= end - 3) && (p[i] || p[i + 1] || p[i+2] != 1) )
		{  
			++i; 
		}
		if (p + i > end - 3) 
		{
			auto len = end - start;
			assert(prevPos != -1);
			emplaceLastNLU(prevPos, len);
			return i;
		}
		type = (int)(p[i + 3] & 0x1F);
		//switch (type) {
		//case 7: {//Sequence Parameter Set (SPS) - 7
		//	std::cout << "7 - ";
		//} break;
		//case 8: {//Picture Parameter Set (PPS)- 8
		//	std::cout << "8 - ";
		//} break;
		//case 5: {//Instantaneous Decoder Refresh - 5
		//	std::cout << "5 - ";
		//} break;
		//case 6: {//Access Unit Delimiter (AUD) - 6
		//	std::cout << "6 - ";
		//} break;
		//case 1:
		//	std::cout << "1 - ";
		//	break;
		//default:
		//	std::cout << "X[" << type << "] ";
		//	break;
		//}

		switch (prevType) {
		case 7: {
			assert(prevPos != -1);
			emplaceLastNLU(prevPos, i );
			prevPos = -1;
		} break;
		case 8: {
			assert(prevPos != -1);
			emplaceLastNLU(prevPos, i );
			prevPos = -1;		
		} break;
		case 5: {
			assert(prevPos != -1);
			emplaceLastNLU(prevPos, i);
			prevPos = -1;
		} break;
		case 6: { // Access Unit Delimiter (AUD)
			assert(prevPos != -1);
			emplaceLastNLU(prevPos, i);
			prevPos = -1;
		} break;
		case 1:{
			assert(prevPos != -1);
			emplaceLastNLU(prevPos, i);
			prevPos = -1;
		}break;
		default:
			break;
		}
		prevType = type;
		prevPos = i + 4;
		++i; 
	}
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
				emplaceLastNLU(i, naluEndIndex);
                break;
            case 8:
				emplaceLastNLU(i, naluEndIndex);
                break;
            case 5:
				emplaceLastNLU(i, naluEndIndex);
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
	for (auto& unit : unitTypes){
		auto nalu = unit.value();
		units.insert(units.end(), nalu.begin(), nalu.end());
	}
	unitTypes.clear();
    return units;
}
