/* extract_mm_c64 - Extract data files from C64 version of Maniac Mansion
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

#include "util.h"
#include <stdarg.h>

void writeByteAlt(FILE *fp, uint8 b) {
	writeByte(fp, (uint8)(b ^ 0xFF));
}

void writeUint16LEAlt(FILE *fp, uint16 value) {
	writeUint16LE(fp, (uint16)(value ^ 0xFFFF));
}

#define writeByte writeByteAlt
#define writeUint16LE writeUint16LEAlt

#define NUM_ROOMS	55

unsigned char room_disks[NUM_ROOMS], room_tracks[NUM_ROOMS], room_sectors[NUM_ROOMS];
static const int SectorOffset[36] = {
	0,
	0, 21, 42, 63, 84, 105, 126, 147, 168, 189, 210, 231, 252, 273, 294, 315, 336,
	357, 376, 395, 414, 433, 452, 471,
	490, 508, 526, 544, 562, 580,
	598, 615, 632, 649, 666
};
static const int ResourcesPerFile[NUM_ROOMS] = {
	 0, 11,  1,  3,  9, 12,  1, 13, 10,  6,
	 4,  1,  7,  1,  1,  2,  7,  8, 19,  9,
	 6,  9,  2,  6,  8,  4, 16,  8,  3,  3,
	12, 12,  2,  8,  1,  1,  2,  1,  9,  1,
	 3,  7,  3,  3, 13,  5,  4,  3,  1,  1,
	 3, 10,  1,  0,  0
};

int main (int argc, char **argv) {
	FILE *input1, *input2, *output;
	char fname[1024];
	char inputPath[768];
	int i, j;
	unsigned short signature;

	if (argc < 3) {
		printf("\nUsage: %s <disk1.d64> <disk2.d64>\n", argv[0]);
		exit(2);
	}
	getPath(argv[argc - 1], inputPath);
	if (!(input1 = fopen(argv[1],"rb")))
		error("Unable to open file %s for input!",argv[1]);
	if (!(input2 = fopen(argv[2],"rb")))
		error("Unable to open file %s for input!",argv[2]);

	/* check signature */
	signature = readUint16LE(input1);
	if (signature != 0x0A31)
		error("Signature not found in disk 1!");
	signature = readUint16LE(input2);
	if (signature != 0x0132)
		error("Signature not found in disk 2!");

	sprintf(fname, "%s/00.LFL", inputPath);
	if (!(output = fopen(fname, "wb")))
		error("Unable to create index file!");
	notice("Creating 00.LFL...");

	/* write signature */
	writeUint16LE(output, signature);

	/* copy object flags */
	for (i = 0; i < 256; i++)
		writeByte(output, readByte(input1));
	/* copy room offsets */
	for (i = 0; i < NUM_ROOMS; i++) {
		room_disks[i] = readByte(input1);
		writeByte(output, room_disks[i]);
	}
	for (i = 0; i < NUM_ROOMS; i++) {
		room_sectors[i] = readByte(input1);
		writeByte(output, room_sectors[i]);
		room_tracks[i] = readByte(input1);
		writeByte(output, room_tracks[i]);
	}

	/* copy costume offsets */
	for (i = 0; i < 25; i++)
		writeByte(output, readByte(input1));
	for (i = 0; i < 25; i++)
		writeUint16LE(output, readUint16LE(input1));

	/* copy script offsets */
	for (i = 0; i < 160; i++)
		writeByte(output, readByte(input1));
	for (i = 0; i < 160; i++)
		writeUint16LE(output, readUint16LE(input1));

	/* copy sound offsets */
	for (i = 0; i < 70; i++)
		writeByte(output, readByte(input1));
	for (i = 0; i < 70; i++)
		writeUint16LE(output, readUint16LE(input1));
	fclose(output);

	for (i = 0; i < NUM_ROOMS; i++) {
		FILE *input;
		if (room_disks[i] == '1')
			input = input1;
		else if (room_disks[i] == '2')
			input = input2;
		else
			continue;

		sprintf(fname, "%s/%02i.LFL", inputPath, i);
		output = fopen(fname, "wb");

		if (output == NULL) {
			error("Unable to create %s!", fname);
		}

		notice("Creating %s...", fname);
		fseek(input, (SectorOffset[room_tracks[i]] + room_sectors[i]) * 256, SEEK_SET);

		for (j = 0; j < ResourcesPerFile[i]; j++) {
			unsigned short len = readUint16LE(input);
			writeUint16LE(output, len);

			for (len -= 2; len > 0; len--) {
				writeByte(output, readByte(input));
			}
		}

		rewind(input);
	}

	notice("All done!");

	return 0;
}