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

#include <ccstreams/mem.h>

struct mem_cookie {
  char **ptr;
  size_t *size;
  off_t offset;
  int append;
};

static
int
mem_cookie_init(struct mem_cookie *self, char **ptr, size_t *size, const int append)
{
  assert(ptr != NULL);
  assert(*ptr != NULL);
  assert(size != NULL);

  int status = 0;

  self->ptr = ptr;
  self->size = size;
  self->offset = 0;
  self->append = append;

  return 0;
}

static
void
mem_cookie_fini(struct mem_cookie *self)
{
  if (self == NULL) return;

  self->ptr = NULL;
  self->size = NULL;
  self->offset = 0;
  self->append = 0;
}

static
ssize_t
mem_read(void *cookie, char *buf, size_t size)
{
  struct mem_cookie *mem_cookie = cookie;

  char *ptr = *mem_cookie->ptr + mem_cookie->offset;
  size_t bytes_read = 0;

  if (mem_cookie->offset + size > *mem_cookie->size) {
    bytes_read = *mem_cookie->size - mem_cookie->offset;
  }
  else {
    bytes_read = size;
  }

  memcpy(buf, ptr, bytes_read);
  mem_cookie->offset += bytes_read;

  return bytes_read;
}

static
ssize_t
mem_write(void *cookie, const char *buf, size_t size)
{
  int status = 0;
  struct mem_cookie *mem_cookie = cookie;

  size_t bytes_written = size;
  int append = mem_cookie->append;

  char *ptr = *mem_cookie->ptr;
  size_t new_size = *mem_cookie->size;
  size_t offset = (append ? new_size : mem_cookie->offset) + size;

  if (new_size < offset) {
    new_size = offset;
  }

  ptr = realloc(ptr, new_size);
  if (ptr == NULL) {
    status = -1;
    goto cleanup;
  }

  *mem_cookie->ptr = ptr;

  ptr += append ? *mem_cookie->size : mem_cookie->offset;
  memcpy(ptr, buf, bytes_written);

  *mem_cookie->size = new_size;
  mem_cookie->offset = append ? mem_cookie->offset : offset;

cleanup:
  if (status != 0) {
    return status;
  }

  return bytes_written;
}

static
int
mem_seek(void *cookie, off64_t *offset, int whence)
{
  int status = 0;
  struct mem_cookie *mem_cookie = cookie;
  off_t new_offset = mem_cookie->offset;

  switch (whence) {
    case SEEK_SET:
      new_offset = *offset;
      break;
    case SEEK_CUR:
      new_offset += *offset;
      break;
    case SEEK_END:
      new_offset = *mem_cookie->size + *offset;
      break;
  }

  if (*mem_cookie->size < new_offset) {
    status = -1;
    goto cleanup;
  }

cleanup:
  if (status == 0) {
    mem_cookie->offset = new_offset;
    *offset = new_offset;
  }

  return status;
}

static
int
mem_close(void *cookie)
{
  int status = 0;
  struct mem_cookie *mem_cookie = cookie;

  mem_cookie_fini(mem_cookie);
  free(mem_cookie);

  return status;
}

FILE *
ccstreams_fmemopen(char **ptr, size_t *size, const char *mode)
{
  assert(ptr != NULL);
  assert(size != NULL);

  int status = 0;
  FILE *stream = NULL;
  struct mem_cookie *cookie = NULL;
  cookie_io_functions_t mem_io_funcs = {
    .read  = mem_read,
    .write = mem_write,
    .seek  = mem_seek,
    .close = mem_close,
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

  if (create && *ptr == NULL) {
    *ptr = malloc(0);
    if (*ptr == NULL) {
      status = -1;
      goto cleanup;
    }

    created = 1;
    *size = 0;
  }

  if (*ptr == NULL) {
    status = -1;
    errno = ENOENT;
    goto cleanup;
  }

  if (truncate & !created) {
    free(*ptr);
    *ptr = malloc(0);
    if (*ptr == NULL) {
      status = -1;
      goto cleanup;
    }
  }

  cookie = malloc(sizeof(*cookie));
  if (cookie == NULL) {
    status = -1;
    goto cleanup;
  }

  status = mem_cookie_init(cookie, ptr, size, append);
  if (status != 0) {
    status = -1;
    goto cleanup;
  }

  stream = fopencookie(cookie, mode, mem_io_funcs);
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
    mem_cookie_fini(cookie);
    free(cookie);

    if (created) {
      free(*ptr);
      *ptr = NULL;
    }
  }

  return stream;
}
