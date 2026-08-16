#ifndef DIR_WALK_H
#define DIR_WALK_H

#include "../usb/usb.h"
#include "../mem_pool/mem_pool.h"
#include "../path_utils/path_utils.h"

void openDir(path_t *cwd, char *dir_to, usb_t *list, const uint32_t period, idxPool_t *p);


#endif
