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

#include <check.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <ccstreams/mem.h>

char *ptr = NULL;
size_t size = 0;
FILE *stream = NULL;

#define MEM_RW_INITIAL "Hello World!"

void
mem_rw_setup(void)
{
  int status = 0;
  char initial_ptr[] = MEM_RW_INITIAL;

  ptr = malloc(sizeof(initial_ptr));
  fail_unless(ptr != NULL, NULL);

  strcpy(ptr, initial_ptr);
  size = strlen(ptr) + 1;

  stream = ccstreams_fmemopen(&ptr, &size, "r+");
  fail_unless(stream != NULL, NULL);
}

void
mem_rw_teardown(void)
{
  if (stream != NULL) {
    fclose(stream);
    stream = NULL;
  }

  free(ptr);
  ptr = NULL;
}

START_TEST(mem_rw_read)
{
  char buf[1024];
  size_t bytes_read = 0; 

  bytes_read = fread(buf, sizeof(buf[0]), sizeof(buf), stream);
  fail_unless(ferror(stream) == 0, strerror(errno));
  fail_unless(bytes_read == sizeof(MEM_RW_INITIAL));
  fail_unless(strncmp(buf, MEM_RW_INITIAL, sizeof(MEM_RW_INITIAL) - 1) == 0);
}
END_TEST

START_TEST(mem_rw_write)
{
  char msg[] = "How are you?";
  size_t bytes_written = 0;

  bytes_written = fwrite(msg, sizeof(msg[0]), sizeof(msg) - 1, stream);
  fail_unless(ferror(stream) == 0, strerror(errno));
  fail_unless(fflush(stream) == 0, strerror(errno));
  fail_unless(bytes_written == sizeof(msg) - 1);
  fail_unless(strncmp(ptr, msg, sizeof(msg) - 1) == 0);
}
END_TEST

START_TEST(mem_rw_tell)
{
  long pos = ftell(stream);
  fail_unless(pos == 0, strerror(errno));
}
END_TEST

START_TEST(mem_rw_seek)
{
  int status = 0;
  long pos = 0;
  char buf[1024];
  size_t bytes_read = 0; 
  
  status = fseek(stream, 5, SEEK_SET);
  fail_unless(status == 0, strerror(errno));

  pos = ftell(stream);
  fail_unless(pos == 5, "Offset wasn't updated properly.");

  bytes_read = fread(buf, sizeof(buf[0]), sizeof(buf), stream);
  fail_unless(ferror(stream) == 0, strerror(errno));
  fail_unless(bytes_read == sizeof(MEM_RW_INITIAL) - 5);
  fail_unless(strncmp(buf, MEM_RW_INITIAL + 5, sizeof(MEM_RW_INITIAL) - 5) == 0);
}
END_TEST

START_TEST(mem_rw_write_growing)
{
  int status = 0;
  char msg[] = " How are you?";
  size_t bytes_written = 0;

  status = fseek(stream, 0, SEEK_END);
  fail_unless(status == 0, strerror(errno));

  bytes_written = fwrite(msg, sizeof(msg[0]), sizeof(msg), stream);
  fail_unless(ferror(stream) == 0, strerror(errno));
  fail_unless(fflush(stream) == 0, strerror(errno));
  fail_unless(bytes_written == sizeof(msg));
  fail_unless(size == sizeof(msg) + sizeof(MEM_RW_INITIAL), "Expecting a size of %zu but got %zu.", sizeof(msg) + sizeof(MEM_RW_INITIAL), size);
  fail_unless(strcmp(ptr, MEM_RW_INITIAL) == 0, "ptr: '%s' initial: '" MEM_RW_INITIAL "'", ptr);
  fail_unless(strcmp(ptr + sizeof(MEM_RW_INITIAL), msg) == 0, "ptr: '%s' msg: '%s'", ptr, msg);
}
END_TEST

Suite *
mem_suite(void)
{
  Suite *suite = suite_create("mem");

  TCase *tc_mem_rw = tcase_create("mem rw");

  tcase_add_checked_fixture(tc_mem_rw, mem_rw_setup, mem_rw_teardown);
  tcase_add_test(tc_mem_rw, mem_rw_read);

  tcase_add_checked_fixture(tc_mem_rw, mem_rw_setup, mem_rw_teardown);
  tcase_add_test(tc_mem_rw, mem_rw_write);

  tcase_add_checked_fixture(tc_mem_rw, mem_rw_setup, mem_rw_teardown);
  tcase_add_test(tc_mem_rw, mem_rw_tell);

  tcase_add_checked_fixture(tc_mem_rw, mem_rw_setup, mem_rw_teardown);
  tcase_add_test(tc_mem_rw, mem_rw_seek);

  tcase_add_checked_fixture(tc_mem_rw, mem_rw_setup, mem_rw_teardown);
  tcase_add_test(tc_mem_rw, mem_rw_write_growing);

  suite_add_tcase(suite, tc_mem_rw);

  return suite;
}

int
main(void)
{
  int failed = 0;

  SRunner *sr = srunner_create(mem_suite());

  srunner_run_all(sr, CK_NORMAL);
  failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
