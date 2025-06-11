#include "fs.h"
#include "../../lib/string.h"

#define MAX_FILES 16

static struct file files[MAX_FILES];

void fs_init(void) {
    for (int i = 0; i < MAX_FILES; i++)
        files[i].used = 0;
}

static void copy_name(char *dst, const char *src) {
    int i = 0;
    while (src[i] && i < 31) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

struct file *fs_create(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            copy_name(files[i].name, name);
            files[i].size = 0;
            files[i].used = 1;
            return &files[i];
        }
    }
    return 0;
}

struct file *fs_open(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0)
            return &files[i];
    }
    return 0;
}

int fs_write(struct file *f, const char *data, unsigned int len) {
    if (!f || !f->used)
        return -1;
    if (len > sizeof(f->data))
        len = sizeof(f->data);
    for (unsigned int i = 0; i < len; i++)
        f->data[i] = data[i];
    f->size = len;
    return (int)len;
}

int fs_read(struct file *f, char *buf, unsigned int len) {
    if (!f || !f->used)
        return -1;
    if (len > f->size)
        len = f->size;
    for (unsigned int i = 0; i < len; i++)
        buf[i] = f->data[i];
    return (int)len;
}

int fs_list(struct file **list, unsigned int max) {
    unsigned int count = 0;
    for (int i = 0; i < MAX_FILES && count < max; i++) {
        if (files[i].used) {
            list[count++] = &files[i];
        }
    }
    return (int)count;
}
