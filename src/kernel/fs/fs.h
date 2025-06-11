#ifndef FS_H
#define FS_H

struct file {
    char name[32];
    char data[4096];
    unsigned int size;
    int used;
};

void fs_init(void);
struct file *fs_create(const char *name);
struct file *fs_open(const char *name);
int fs_write(struct file *f, const char *data, unsigned int len);
int fs_read(struct file *f, char *buf, unsigned int len);
int fs_list(struct file **list, unsigned int max);
int fs_delete(const char *name);
int fs_rename(const char *oldname, const char *newname);

#endif /* FS_H */
