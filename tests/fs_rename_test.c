#include <assert.h>
#include <string.h>
#include "../src/kernel/fs/fs.h"

int main(void) {
    fs_init();
    fs_create("old");
    assert(fs_rename("old", "new") == 0);
    assert(fs_open("old") == 0);
    struct file *f = fs_open("new");
    assert(f);
    assert(fs_rename("missing", "x") == -1);
    fs_create("taken");
    assert(fs_rename("new", "taken") == -1);
    return 0;
}
