/*
*   Library for storing and retrieving multibple bits in one byte
*   Copyright (C) 2014-2017  S.Butzek
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BITSTORE_H
#define BITSTORE_H

#include "Arduino.h"

template<uint8_t bufSize>
class BitStore
{
public:
	/** Default constructor */
	BitStore(uint8_t bitlength);
	//~BitStore();
	void addValue(byte value);
	int8_t getValue(const uint16_t pos);
	bool moveLeft(const uint16_t begin);
	bool changeValue(const uint16_t pos, byte value);

	const uint16_t getSize();
	//unsigned char *datastore;  // Reserve 40 Bytes for our store. Should be edited to aa dynamic way
	unsigned char datastore[bufSize];
	void reset();
	unsigned char getByte(const uint8_t idx);
	uint8_t bytecount;  // Number of stored bytes
	uint16_t valcount;  // Number of total values stored

	int8_t operator[](const uint16_t pos) {
		getValue(pos);
		return _rval;
	}
	BitStore &operator+=(const int8_t value) {
		addValue(value);
		return *this;
	}



#ifndef UNITTEST
protected:

private:
#endif
	uint8_t valuelen;   // Number of bits for every value
	uint8_t bmask;
	uint8_t bcnt;
	const uint8_t buffsize;
	byte _rval;
};


/*
*   Library for storing and retrieving multibple bits in one byte
*   Copyright (C) 2014  S.Butzek
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

template<uint8_t bufSize>
BitStore<bufSize>::BitStore(uint8_t bitlength) :buffsize(bufSize)
{
	valuelen = bitlength; // How many bits shoudl be reserved for one value added ?
	bmask = 0;
	//buffsize = bufsize;
	//datastore= (unsigned char*) calloc(bufsize,sizeof(char)); // Speicher allokieren und 0 zuweisen
	reset();
	for (uint8_t x = 7; x>(7 - valuelen); x--)
	{
		bmask = bmask | (1 << x);
	}
}

/** @brief (one liner)
*
* (documentation goes here)
*/
/*template<uint8_t bufSize>
BitStore<bufSize>::~BitStore()
{
//free(datastore);
}
*/
template<uint8_t bufSize>
void BitStore<bufSize>::addValue(byte value)
{
	if (bytecount >= buffsize) Serial.println("OOB");
	if (bytecount >= buffsize) return; // Out of Buffer
	if (bcnt == 7 && valcount > 0)
	{
		bytecount++;
		datastore[bytecount] = 0;
	}
	//Serial.print("Adding value:");   Serial.print(value, DEC);
	//Serial.print(" (");   Serial.print(value, BIN); Serial.print(") ");
	//store[bytecount]=datastore[bytecount] | (value<<bcnt)
	value <<= (bcnt + 1 - valuelen);

	datastore[bytecount] = datastore[bytecount] | value;  // (valcount*valuelen%8)
														  /*
														  Serial.print(" at byte: ");   Serial.print(bytecount, DEC);
														  Serial.print(" at bitpos: ");   Serial.print(bcnt, DEC);
														  Serial.print("  datastore is (bin)");   Serial.print(datastore[bytecount], BIN);
														  Serial.print("  (dec)");   Serial.print(datastore[bytecount], DEC);
														  Serial.print(" : ");
														  Serial.print("  (valuelen)");   Serial.print(valuelen, DEC);
														  */
	valcount++;
	if (int8_t(bcnt - valuelen) >= 0)  // Soalnge nicht 8 Bit gepeichert wurden, erhoehen wir den counter zum verschieben
	{
		bcnt = bcnt - valuelen; //+valuelen
	}
	else {
		bcnt = 7;
	}

	//Serial.println("");
}


template<uint8_t bufSize>
bool BitStore<bufSize>::changeValue(const uint16_t pos, byte value)
{
	uint8_t bytepos = pos*valuelen / 8;
	/*
	Serial.print("Changing value:");   Serial.print(value, DEC);
	Serial.print(" (");   Serial.print(value, BIN); Serial.print(")@valpos:");
	Serial.print(pos, DEC);
	Serial.print(" @byte: ");   Serial.print(bytepos, DEC);
	Serial.print("  datastore is (bin)");   Serial.print(datastore[bytepos], BIN);
	Serial.print(" (dec)");   Serial.print(datastore[bytepos], DEC);
	*/
	if ((bytepos) >= buffsize) return false; // Out of Buffer
											 // Serial.print("Pos:");   Serial.print(pos, DEC);
											 // 01 11 11 00    10

	value <<= (8 - (pos*valuelen % 8) - valuelen);									// shift bits in value to new positon
																					//Serial.print(" new_value(bin)");   Serial.print(value, BIN);

	datastore[bytepos] = datastore[bytepos] & ~(bmask >> (pos*valuelen % 8));  // Clear bits to be changed 
	datastore[bytepos] = datastore[bytepos] | value;						    // Apply new bits

																				/*
																				Serial.print(" -> ");
																				Serial.print("  datastore to (bin)");   Serial.print(datastore[bytepos], BIN);
																				Serial.print(" (valuelen)");   Serial.println(valuelen, DEC);
																				*/

	return true;
}


template<uint8_t bufSize>
const uint16_t BitStore<bufSize>::getSize()
{
	return valcount - 1;
}

template<uint8_t bufSize>
bool BitStore<bufSize>::moveLeft(const uint16_t begin)
{
	if (begin == 0) return false;
	if (begin > valcount) return false;

	uint8_t startbyte = begin*valuelen / 8;

	//Serial.print("moveleft startbyte:"); Serial.print(startbyte, DEC); Serial.print("@valpos");  Serial.print(begin, DEC);
	//Serial.print(" bytecount:"); Serial.print(bytecount, DEC);

	if (begin % (8 / valuelen) != 0) {
		uint8_t shift_left = (begin % (8 / valuelen))*valuelen;
		uint8_t shift_right = 8 - shift_left;
		//Serial.print(" sleft ");   Serial.print(shift_left, DEC);  Serial.print(" sright"); Serial.print(shift_right, DEC);

		valcount = valcount - (shift_left / valuelen);
		uint8_t i = startbyte;
		uint8_t z = 0;
		for (; i < bytecount; ++i, ++z)
		{
			datastore[z] = (datastore[i] << shift_left) | (datastore[i + 1] >> shift_right);;
			/*
			Serial.println("");
			Serial.print("    ");  Serial.print(i, DEC); Serial.print(":");
			Serial.print("z="); Serial.print(datastore[z], BIN);
			Serial.print(" ileft="); Serial.print(datastore[i] << shift_left, BIN);
			Serial.print(" iright="); Serial.print(datastore[i + 1] >> shift_right, BIN);
			*/
		}
		datastore[z] = datastore[i] << shift_left;
		bcnt = 7 - shift_left;
		bytecount = bytecount - startbyte;
		bcnt = bcnt - valuelen;

	}
	else {
		bytecount = bytecount - startbyte;
		//Serial.print(" memmove ");   Serial.print(bytecount, DEC);  Serial.print(" bytes");
		memmove(datastore, datastore + startbyte, sizeof(datastore[bytecount]) * bytecount);
		bcnt = 7;
	}
	valcount = valcount - (8 / valuelen*startbyte);

	//Serial.print(" valcount: ");   Serial.print(valcount, DEC);
	//Serial.print(" bytecount: ");   Serial.print(bytecount, DEC);
	//Serial.println(" ");
	return true;

}

template<uint8_t bufSize>
int8_t BitStore<bufSize>::getValue(const uint16_t pos)
{
	int16_t bytepos = pos*valuelen / 8;
	if ((bytepos) >= buffsize) return -1; // Out of Buffer
										  //Serial.print("getValue Pos:");   Serial.print(pos, DEC);

	uint8_t mask; // Local modified bitmask
				  //ret= (datastore[pos*valuelen/8]>>(pos*valuelen%8))&bmask;
	mask = bmask >> (pos*valuelen % 8);            //Mask the position we want to retrieve
												   //Serial.print(" Bitmask:");   Serial.print(mask, BIN);

	_rval = datastore[bytepos] & mask;         // Combine the mask with our store to extract the bit

	byte scnt = 8 - (valuelen*(pos) % 8) - valuelen;// ((pos + 1)*valuelen % 8);         //      pos*valuelen    0*1/1*1/2*1/../7*1    7/6/5/0    8-1  8-2  8-3 8-8   
													//byte scnt = ((pos+1)*valuelen % 8);								  //                       0*4/1*4              
													//Serial.print(" datastore[bytepos]: ("); Serial.print(_rval, DEC); Serial.print(") ");  Serial.println(_rval, BIN);
													//Serial.print(" shift:");   Serial.print(scnt, DEC);

													//_rval = _rval >> abs(8 - (pos + 1)*valuelen) % 8;//(pos + 1)*valuelen % 8;
	_rval = _rval >> scnt;
	//Serial.print(" return: ("); Serial.print(_rval, DEC); Serial.print(") ");  Serial.println(_rval, BIN);

	return _rval;
}

template<uint8_t bufSize>
unsigned char BitStore<bufSize>::getByte(const uint8_t idx)
{
	if (idx >= buffsize) return -1; // Out of buffer range
	return datastore[idx];
}

template<uint8_t bufSize>
void BitStore<bufSize>::reset()
{
	for (uint8_t i = 0; i<buffsize; i++)
	{
		datastore[i] = 0;
	}
	bytecount = 0;
	valcount = 0;
	bcnt = 7;
}


#endif // BITSTORE_H
