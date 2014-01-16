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

#ifndef ECX_CCSTREAMS_COPY_H
#define ECX_CCSTREAMS_COPY_H 1

#include <ccstreams/copy.h>

/* Copy data from the input stream to the output stream. 
 *
 * bytes will be set to the total number of bytes written.
 */ 
void
ecx_ccstreams_copy(FILE *from, FILE *to, size_t *bytes);

/* Copy data from the input stream to the output stream in chunks of the given
 * size.
 *
 * bytes will be set to the total number of bytes written.
 */
void
ecx_ccstreams_copy_by(FILE *from, FILE *to, size_t *bytes, size_t chunk);


#endif /* ECX_CCSTREAMS_COPY_H */
