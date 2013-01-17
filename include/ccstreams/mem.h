/* Copyright 2013 Caleb Case
 *
 * This file is part of the CCStreams Library.
 *
 * The CCStreams Library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The CCStreams Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the CCStreams Library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CCSTREAMS_MEM_H
#define CCSTREAMS_MEM_H 1

#include <stdio.h>

/* Create a stream from a memory buffer. If *ptr is NULL, then an empty (zero
 * sized) buffer will be allocated, otherwise the existing data is used. The
 * caller should free the buffer after the stream is closed. The buffer will
 * be grown via calls to realloc(...) when writing past the end of the buffer.
 * Output to the stream will invalidate the contents of ptr and size (as well
 * as *ptr and *size) until after a flush or close.
 *
 * This stream operates similarly to the stream return from the POSIX
 * functions fmemopen and open_memstream. There are some very important
 * differences however.
 *
 * Unlike fmemopen:
 *
 * - This stream does not store a null byte at the end of the buffer.
 * - This stream will automatically increase the size an existing buffer.
 * - No special consideration is given to the mode (e.g. no specialized
 *   behavior depending on the "binary" mode).
 *
 * Unlike open_memstream:
 *
 * - This stream does not store a null byte after the end of the buffer (and
 *   therefor does not allocate extra space for the buffer).
 * - This stream will use the existing data in the buffer.
 * - This stream does not permit seeking past the end of the buffer.
 */
FILE *
ccstreams_fmemopen(char **ptr, size_t *size, const char *mode);

#endif /* CCSTREAMS_MEM_H */
