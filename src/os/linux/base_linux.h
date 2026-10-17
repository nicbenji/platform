#ifndef BASE_LINUX_H_
#define BASE_LINUX_H_

#define _FILE_OFFSET_BITS 64 // TODO: keep this or just read in <32bit batches

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

#endif  // BASE_LINUX_H_
