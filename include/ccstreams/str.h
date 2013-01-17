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

#ifndef CCSTREAMS_STR_H
#define CCSTREAMS_STR_H 1

#include <stdio.h>

/* Create a FILE stream from a C string. The mode will be honored as per
 * fopen(...). Please remember to following nuances:
 *
 *  w  Truncates *str to 0 length.
 *  w+ Truncates *str to 0 length. Allocates space for *str if NULL.
 *
 *  a  Allocates space for *str if NULL. Stream is positioned at the end. All
 *     output will be to the end of the string (regardless of position).
 *  a+ Allocates space for *str if NULL. Stream is positioned at the
 *     beginning. All output will be to the end of the string (regardless of
 *     position).
 *
 * If *str is not NULL, then the existing data will be used (unless the mode
 * requires truncating it).
 *
 * The caller should free *str after closing the stream.
 *
 * The string will be grown/shrunk via calls to realloc(...). A trailing NULL
 * byte will be maintained.
 *
 * Explicitly writing a NULL byte will truncate the string.
 *
 * Output to the stream invalidates the contents of str (and *str) until after
 * a flush or close. This is due to the standard buffered IO. If this is not
 * desired, set the stream to use unbuffered IO instead.
 */
FILE *
ccstreams_fstropen(char **str, const char *mode);

#endif /* CCSTREAMS_STR_H */
