#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/rctl.h>

#define RACCT_OUT_SIZE 4096

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return 2;
  }
  const char *rctl_spec = argv[1];

  char rbuf[RACCT_OUT_SIZE];
  int ret = rctl_get_racct(rctl_spec, strlen(rctl_spec) + 1,
    rbuf, RACCT_OUT_SIZE);
  if (ret != 0) {
    perror("rctl_get_racct");
    return 1;
  }

  const char *c = rbuf;
  bool done = false;
  while (!done) {
    const char *key = c;
    c = strchr(key, '=');
    if (c == NULL) {
      fprintf(stderr, "No equals found\n");
      return 1;
    }
    int keysz = c - key;

    const char *val = c + 1;
    int valsz;
    c = strchr(val, ',');
    if (c == NULL) {
      done = true;
      valsz = strlen(val);
    } else {
      valsz = c - val;
    }
    c += 1;

    printf("%.*s = %.*s\n", keysz, key, valsz, val);
  }

  return 0;
}
