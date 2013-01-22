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

#include <ccstreams/str.h>

struct str_cookie {
  char **str;
  size_t length;
  off_t offset;
  int append;
};

static
int
str_cookie_init(struct str_cookie *self, char **str, const int append)
{
  assert(str != NULL);
  assert(*str != NULL);

  int status = 0;

  self->str = str;
  self->length = strlen(*str);
  self->offset = 0;
  self->append = append;

  return 0;
}

static
void
str_cookie_fini(struct str_cookie *self)
{
  if (self == NULL) return;

  self->str = NULL;
  self->length = 0;
  self->offset = 0;
  self->append = 0;
}

static
ssize_t
str_read(void *cookie, char *buf, size_t size)
{
  struct str_cookie *str_cookie = cookie;

  char *str = *str_cookie->str + str_cookie->offset;
  size_t bytes_read = strnlen(str, size);

  memcpy(buf, str, bytes_read);
  str_cookie->offset += bytes_read;

  return bytes_read;
}

static
ssize_t
str_write(void *cookie, const char *buf, size_t size)
{
  int status = 0;
  struct str_cookie *str_cookie = cookie;

  size_t bytes_written = size;
  size_t truncate = strnlen(buf, size) < size ? 1 : 0;
  int append = str_cookie->append;

  char *str = *str_cookie->str;
  size_t length = str_cookie->length;
  size_t offset = (append ? length : str_cookie->offset) + size - truncate;

  if (truncate || length < offset) {
    length = offset;
  }

  str = realloc(str, length + 1);
  if (str == NULL) {
    status = -1;
    goto cleanup;
  }

  *str_cookie->str = str;

  str += append ? str_cookie->length : str_cookie->offset;
  memcpy(str, buf, bytes_written);
  (*str_cookie->str)[length] = '\0';

  str_cookie->length = length;
  str_cookie->offset = append ? str_cookie->offset : offset;

cleanup:
  if (status != 0) {
    return status;
  }

  return bytes_written;
}

static
int
str_seek(void *cookie, off64_t *offset, int whence)
{
  int status = 0;
  struct str_cookie *str_cookie = cookie;
  off_t new_offset = str_cookie->offset;

  switch (whence) {
    case SEEK_SET:
      new_offset = *offset;
      break;
    case SEEK_CUR:
      new_offset += *offset;
      break;
    case SEEK_END:
      new_offset = str_cookie->length + *offset;
      break;
  }

  if (str_cookie->length < new_offset) {
    status = -1;
    goto cleanup;
  }

cleanup:
  if (status == 0) {
    str_cookie->offset = new_offset;
    *offset = new_offset;
  }

  return status;
}

static
int
str_close(void *cookie)
{
  int status = 0;
  struct str_cookie *str_cookie = cookie;

  str_cookie_fini(str_cookie);
  free(str_cookie);

  return status;
}

FILE *
ccstreams_fstropen(char **str, const char *mode)
{
  assert(str != NULL);

  int status = 0;
  FILE *stream = NULL;
  struct str_cookie *cookie = NULL;
  cookie_io_functions_t str_io_funcs = {
    .read  = str_read,
    .write = str_write,
    .seek  = str_seek,
    .close = str_close,
  };
  size_t mode_length = strlen(mode);
  int extra = 0;
  int create = 0;
  int created = 0;
  int append = 0;
  int truncate = 0;
  int end = 0;
  
  if (mode_length > 1) {
    if (mode[1] == 'b') {
      if (mode_length > 2 && mode[2] == '+') {
        extra = 2;
      }
    }
    else if (mode[1] == '+') {
      extra = 1;
    }
  }

  switch (mode[0]) {
    case 'w':
      if (extra) {
        create = 1;
      }
      truncate = 1;
      break;
    case 'a':
      if (!extra) {
        end = 1;
      }
      create = 1;
      append = 1;
      break;
  }

  if (create && *str == NULL) {
    *str = malloc(1);
    if (*str == NULL) {
      status = -1;
      goto cleanup;
    }

    created = 1;
    *str[0] = '\0';
  }

  if (*str == NULL) {
    status = -1;
    errno = ENOENT;
    goto cleanup;
  }

  if (truncate & !created) {
    *str = realloc(*str, 1);
    if (*str == NULL) {
      status = -1;
      goto cleanup;
    }

    *str[0] = '\0';
  }

  cookie = malloc(sizeof(*cookie));
  if (cookie == NULL) {
    status = -1;
    goto cleanup;
  }

  status = str_cookie_init(cookie, str, append);
  if (status != 0) {
    status = -1;
    goto cleanup;
  }

  stream = fopencookie(cookie, mode, str_io_funcs);
  if (stream == NULL) {
    status = -1;
    goto cleanup;
  }

  if (end) {
    status = fseek(stream, 0, SEEK_END);
    if (status != 0) {
      status = -1;
      goto cleanup;
    }
  }

cleanup:
  if (status != 0) {
    str_cookie_fini(cookie);
    free(cookie);

    if (created) {
      free(*str);
      *str = NULL;
    }
  }

  return stream;
}
