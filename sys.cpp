/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001  Ludvig Strigeus
 * Copyright (C) 2001/2002 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#include "stdafx.h"
#include "scumm.h"

void *Scumm::fileOpen(const char *filename, int mode)
{
	_fileMode = mode;
	_whereInResToRead = 0;
	clearFileReadFailed(_fileHandle);

	if (mode == 1)
		return fopen(filename, "rb");

	if (mode == 2) {
		error("fileOpen: write not supported");
	}

	return NULL;
}

void Scumm::fileClose(void *file)
{
	if (_fileMode == 1 || _fileMode == 2)
		fclose((FILE *)file);
}

bool Scumm::fileReadFailed(void *file)
{
	return _fileReadFailed != 0;
}

void Scumm::clearFileReadFailed(void *file)
{
	_fileReadFailed = false;
}

bool Scumm::fileEof(void *file)
{
	return feof((FILE *)file) != 0;
}

uint32 Scumm::filePos(void *handle)
{
	return ftell((FILE *)handle);
}

void Scumm::fileSeek(void *file, long offs, int whence)
{
	switch (_fileMode) {
	case 1:
	case 2:
		if (fseek((FILE *)file, offs, whence) != 0)
			clearerr((FILE *)file);
		return;
	case 3:
		_whereInResToRead = offs;
		return;
	}
}

void Scumm::fileRead(void *file, void *ptr, uint32 size)
{
	byte *ptr2 = (byte *)ptr, *src;

	switch (_fileMode) {
	case 1:
		if (size == 0)
			return;

		if ((uint32)fread(ptr2, size, 1, (FILE *)file) != 1) {
			clearerr((FILE *)file);
			_fileReadFailed = true;
		}

		do {
			*ptr2++ ^= _encbyte;
		} while (--size);

		return;

	case 3:
		if (size == 0)
			return;

		src = getResourceAddress(rtTemp, 3) + _whereInResToRead;
		_whereInResToRead += size;
		do {
			*ptr2++ = *src++ ^ _encbyte;
		} while (--size);
		return;
	}
}

int Scumm::fileReadByte()
{
	byte b;
	byte *src;

	switch (_fileMode) {
	case 1:
		if (fread(&b, 1, 1, (FILE *)_fileHandle) != 1) {
			clearerr((FILE *)_fileHandle);
			_fileReadFailed = true;
		}
		return b ^ _encbyte;

	case 3:
		src = getResourceAddress(rtTemp, 3) + _whereInResToRead;
		_whereInResToRead++;
		return *src ^ _encbyte;
	}
	return 0;
}

uint Scumm::fileReadWordLE()
{
	uint a = fileReadByte();
	uint b = fileReadByte();
	return a | (b << 8);
}

uint32 Scumm::fileReadDwordLE()
{
	uint a = fileReadWordLE();
	uint b = fileReadWordLE();
	return (b << 16) | a;
}

uint Scumm::fileReadWordBE()
{
	uint b = fileReadByte();
	uint a = fileReadByte();
	return a | (b << 8);
}

uint32 Scumm::fileReadDwordBE()
{
	uint b = fileReadWordBE();
	uint a = fileReadWordBE();
	return (b << 16) | a;
}

/* Overloaded versions */
int Scumm::fileReadByte(void *handle)
{
	byte b;

	if (fread(&b, 1, 1, (FILE *)handle) != 1) {
		clearerr((FILE *)handle);
		_fileReadFailed = true;
	}
	return b ^ _encbyte;
}

uint Scumm::fileReadWordLE(void *handle)
{
	uint a = fileReadByte(handle);
	uint b = fileReadByte(handle);
	return a | (b << 8);
}

uint32 Scumm::fileReadDwordLE(void *handle)
{
	uint a = fileReadWordLE(handle);
	uint b = fileReadWordLE(handle);
	return (b << 16) | a;
}

uint Scumm::fileReadWordBE(void *handle)
{
	uint b = fileReadByte(handle);
	uint a = fileReadByte(handle);
	return a | (b << 8);
}

uint32 Scumm::fileReadDwordBE(void *handle)
{
	uint b = fileReadWordBE(handle);
	uint a = fileReadWordBE(handle);
	return (b << 16) | a;
}

bool Scumm::checkFixedDisk()
{
	return true;
}


#ifdef NEED_STRDUP
char *strdup(const char *s)
{
	if (s) {
		int len = strlen(s) + 1;
		char *d = (char *)malloc(len);
		if (d)
			memcpy(d, s, len);
		return d;
	}
	return NULL;
}
#endif /* NEED_STRDUP */


void *operator new(size_t size)
{
	return calloc(size, 1);
}

void operator delete(void *ptr)
{
	free(ptr);
}
