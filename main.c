#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/rctl.h>

// TODO: make this dynamic
#define RACCT_OUT_SIZE 4096
#define PCPU_SEARCH "pcpu="

long get_pcpu(const char *filter) {
  char rbuf[RACCT_OUT_SIZE];
  int ret = rctl_get_racct(filter, strlen(filter) + 1,
    rbuf, RACCT_OUT_SIZE);
  if (ret != 0) {
    perror("rctl_get_racct");
    return -1;
  }

  const char *c = strstr(rbuf, PCPU_SEARCH);
  if (c == NULL) {
    fprintf(stderr, "No pcpu found\n");
    return -1;
  }
  c += strlen(PCPU_SEARCH);

  int pcpu;
  int scanned = sscanf(c, "%d", &pcpu);
  if (scanned != 1) {
    fprintf(stderr, "Failed to parse pcpu\n");
    return -1;
  }
  
  return pcpu;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return -2;
  }
  const char *filter = argv[1];

  int size = 2;
  int sum = 0;
  int count = 0; 
  int *values = calloc(size, sizeof(int));

  for (long i = 0; true; i = (i + 1) % size) {
    int pcpu = get_pcpu(filter);
    if (pcpu == -1) {
      return -1;
    }

    sum += pcpu - values[i];
    values[i] = pcpu;
    if (count < size) {
      ++count;
    }

    int avg = sum / count;
    printf("%d\n", avg);

    usleep(1000 * 100);
  }

  return 0;
}
