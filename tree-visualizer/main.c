#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define YELLOW "\e[0;33m"
#define BLUE "\e[4;1;34m"
#define RESET "\e[0m"
void print_file(const char *name, int is_dir, int indent) {
  printf("%*c", indent * 4, ' ');
  if (is_dir) {
    printf("%s%s%s\n", BLUE, name, RESET);
  } else {
    printf("%s%s%s\n", YELLOW, name, RESET);
  }
}

int tree(const char *path, int indent) {
  DIR *dir = opendir(path);
  if (!dir) {
    perror("opendir error\n");
    return 1;
  }

  struct dirent *dp;
  while ((dp = readdir(dir)) != NULL) {
    const char *name = dp->d_name;

    // ignoring . and .. otherwise infinite loop because it will keep on going
    // in the current directory
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
      continue;
    }
    // if it's directory then showing with blue color
    // else if it's a file then red
    if (dp->d_type == DT_DIR) {
      print_file(name, 1, indent);

      size_t len = strlen(path) + strlen(name) + 2;
      char sub_path[len];

      snprintf(sub_path, len, "%s/%s", path, name);
      tree(sub_path, indent + 1);
    } else {
      print_file(name, 0, indent);
    }
  }

  closedir(dir);
  return 0;
}

// char *argv[] and char **argv are the same thing
// *argv[] automatically behind the scene gets converted to **argv
// because we can't pass the whole array(under the hood c can't understand it),
// as it decays the array into a pointer pointing towards the first element
int main(int argc, char **argv) {
  // default path is the current directory
  const char *path = ".";

  if (argc > 1) {
    path = argv[1];
  }
  int err = tree(path, 0);
  return err;
}
