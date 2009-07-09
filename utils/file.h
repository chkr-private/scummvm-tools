/* Scumm Tools
 * Copyright (C) 2004-2006  The ScummVM Team
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#ifndef COMMON_FILE_H
#define COMMON_FILE_H

#include "stream.h"

namespace Common {

class File : public SeekableReadStream {
protected:
	/** POSIX file handle to the actual file; 0 if no file is open. */
	FILE *_handle;

	/** If we own the internal handle or not (we don't if we're constructed from a ::File object */
	bool _owned;

	/** Status flag which tells about recent I/O failures. */
	bool _ioFailed;

private:
	// Disallow copying File objects. There is not strict reason for this,
	// except that so far we never had real need for such a feature, and
	// code that accidentally copied File objects tended to break in strange
	// ways.
	File(const File &f);
	File &operator  =(const File &f);

public:

	File(FILE *file);
	// Allow construction from a ::File object, note that we *do not* own the file pointer
	// in this scenario, we just act as a different wrapper for the file
	// FIXME: Actually -- why do we have *two* wrappers anyway???
	File(::File &file);
	virtual ~File();

	virtual void close();
	bool isOpen() const;
	bool ioFailed() const;
	void clearIOFailed();
	bool eos() const { return eof(); }
	bool eof() const;
	uint32 pos() const;
	uint32 size() const;
	void seek(int32 offs, int whence = SEEK_SET);
	uint32 read(void *dataPtr, uint32 dataSize);
};

} // End of namespace Common

#endif
