#include <assert.h>
#include <string.h>
#include "../src/kernel/fs/fs.h"

int main(void) {
    fs_init();
    struct file *f = fs_create("test");
    assert(f);
    assert(fs_write(f, "hello", 5) == 5);
    char buf[6] = {0};
    assert(fs_read(f, buf, 5) == 5);
    assert(strcmp(buf, "hello") == 0);
    struct file *f2 = fs_open("test");
    assert(f2 == f);
    return 0;
}
