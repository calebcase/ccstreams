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

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <ccstreams/copy.h>

int
ccstreams_copy_by(FILE *from, FILE *to, size_t *bytes, size_t chunk)
{
  assert(from != NULL);
  assert(to != NULL);
  assert(bytes != NULL);
  assert(chunk != 0);

  size_t bytes_read = 0;
  size_t bytes_written = 0;

  int status = 0;
  char buffer[chunk];

  do {
    bytes_read = fread(buffer, 1, chunk, from);
    bytes_written = fwrite(buffer, 1, bytes_read, to);
    *bytes += bytes_written;

    if (bytes_read < chunk) {
      if (ferror(from)) {
        status = -1;
        break;
      }
    }

    if (bytes_written < bytes_read) {
      if (ferror(to)) {
        status = -1;
        break;
      }
    }
  } while (!feof(from));

  return status;
}

int
ccstreams_copy(FILE *from, FILE *to, size_t *bytes)
{
  return ccstreams_copy_by(from, to, bytes, 4096);
}
