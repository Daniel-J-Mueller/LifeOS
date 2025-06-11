#include <assert.h>
#include <string.h>
#include "../src/kernel/fs/fs.h"

int main(void) {
    fs_init();
    fs_create("temp");
    assert(fs_delete("temp") == 0);
    assert(fs_open("temp") == 0);
    assert(fs_delete("temp") == -1);
    return 0;
}
