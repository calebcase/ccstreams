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
#include <ec/ec.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <ccstreams/mem.h>

FILE *
ecx_ccstreams_fmemopen(char **ptr, size_t *size, const char *mode)
{
  FILE *stream = ccstream_fmemopen(ptr, size, mode);
  if (stream == NULL) {
    ec_throw_errno(errno, NULL) NULL;
  }

  return stream;
}
