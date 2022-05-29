#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "piuio-kmod.h"

#define PIUIO_KMOD_DEV_PATH "/dev/piuio0"

bool piuio_kmod_available()
{
  int fd = open(PIUIO_KMOD_DEV_PATH, O_RDONLY);

  if (fd < 0) {
    return false;
  }

  struct stat st;

  if (fstat(fd, &st) == -1) {
    close(fd);
    return false;
  }

  close(fd);

  if (!S_ISCHR(st.st_mode)) {
    return false;
  }

  return true;
}

result_t piuio_kmod_open(int *fd)
{
  int fd_tmp;

  assert(fd != NULL);

  fd_tmp = open("/dev/piuio0", O_RDONLY);

  if (fd_tmp < 0) {
    return errno;
  }

  *fd = fd_tmp;

  return RESULT_SUCCESS;
}

result_t piuio_kmod_poll(int fd, union piuio_kmod_paket *paket)
{
  assert(fd >= 0);
  assert(paket != NULL);

  int result;

  // Raw input field covers the entire buffer for writing to the kernel and
  // reading back
  result = read(fd, paket->raw, sizeof(paket->raw));

  if (result != sizeof(paket->raw)) {
    if (errno > 0) {
      return errno;
    } else {
      return EIO;
    }
  } else {
    // Invert pull ups
    for (uint8_t i = 0; i < sizeof(paket->raw); i++) {
      paket->raw[i] ^= 0xFF;
    }

    return RESULT_SUCCESS;
  }
}

void piuio_kmod_close(int fd)
{
  assert(fd >= 0);

  close(fd);
}