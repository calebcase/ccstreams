#include <ccstreams/str.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main()
{
  int status = 0;

  /* Initialize an empty string. */
  char *str = NULL;
  FILE *stream = ccstreams_fstropen(&str, "w+");
  if (stream == NULL) {
    perror(NULL);
    return 1;
  }

  /* Say hello. */
#define MSG "Hello World!"
  status = fprintf(stream, MSG);
  if (status < 0 || status != sizeof(MSG) - 1) {
    perror(NULL);
    return 1;
  }
  /* Say it again! */
  status = fprintf(stream, " %s", MSG);
  if (status < 0 || status != sizeof(MSG)) {
    perror(NULL);
    return 1;
  }
  status = fflush(stream);
  if (status != 0) {
    perror(NULL);
    return 1;
  }
  printf("Sizeof %zu \tLength %zu \tOffset %li \tMessage '%s'\n", sizeof(MSG), strlen(str), ftell(stream), str);
#undef MSG

  /* Rewind the stream and fill with a shorter message. */
#define MSG "Shorter."
  rewind(stream);
  status = fprintf(stream, MSG); /* Note: By itself this will not work. fprintf doesn't emit a null byte! */
  if (status < 0 || status != sizeof(MSG) - 1) {
    perror(NULL);
    return 1;
  }
  status = fputc('\0', stream);
  if (status == EOF) {
    perror(NULL);
  }
  status = fflush(stream);
  if (status != 0) {
    perror(NULL);
    return 1;
  }
  printf("Sizeof %zu \tLength %zu \tOffset %li \tMessage '%s'\n", sizeof(MSG), strlen(str), ftell(stream), str);
#undef MSG

  /* Rewind the stream and fill with an even shorter message. */
#define MSG "Shorter"
  rewind(stream);
  status = fwrite(MSG, 1, sizeof(MSG), stream);
  if (status < 0 || status != sizeof(MSG)) {
    perror(NULL);
    return 1;
  }
  status = fflush(stream);
  if (status != 0) {
    perror(NULL);
    return 1;
  }
  printf("Sizeof %zu \tLength %zu \tOffset %li \tMessage '%s'\n", sizeof(MSG), strlen(str), ftell(stream), str);
#undef MSG

  /* Write an longer message. */
#define MSG " than the original. Longer than the original. Can't make up your mind?"
  status = fwrite(MSG, 1, sizeof(MSG), stream);
  if (status < 0 || status != sizeof(MSG)) {
    perror(NULL);
    return 1;
  }
  status = fflush(stream);
  if (status != 0) {
    perror(NULL);
    return 1;
  }
  printf("Sizeof %zu \tLength %zu \tOffset %li \tMessage '%s'\n", sizeof(MSG), strlen(str), ftell(stream), str);
#undef MSG

  /* Close and reopen it. */
  fclose(stream);
  stream = ccstreams_fstropen(&str, "a+");
  if (stream == NULL) {
    perror(NULL);
    return 1;
  }

  /* Append a messsage. */
#define MSG " Back again so soon?"
  status = fwrite(MSG, 1, sizeof(MSG), stream);
  if (status < 0 || status != sizeof(MSG)) {
    perror(NULL);
    return 1;
  }
  status = fflush(stream);
  if (status != 0) {
    perror(NULL);
    return 1;
  }
  printf("Sizeof %zu \tLength %zu \tOffset %li \tMessage '%s'\n", sizeof(MSG), strlen(str), ftell(stream), str);
#undef MSG

  /* Finally close and free the memory. */
  fclose(stream);
  free(str);

  return 0;
}
