#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s PATH [PATH ...]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int fd = inotify_init();
  if (fd == -1) {
    perror("inotify_init");
    exit(EXIT_FAILURE);
  }

  inotify_add_watch(fd, argv[1], IN_CLOSE_WRITE | IN_DELETE);

  char buffer[4096];

  while (1) {
    ssize_t size = read(fd, buffer, sizeof(buffer));

    if (size < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size;) {
      const struct inotify_event *event =
          (const struct inotify_event *)&buffer[i];
      printf("%s: %s\n",
             (IN_CLOSE_WRITE & event->mask) == 0 ? "Deleted" : "Created",
             event->name);
      i += sizeof(struct inotify_event) + event->len;
    }
  }
  close(fd);
  return 0;
}
