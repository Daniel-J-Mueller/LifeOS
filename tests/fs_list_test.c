#include <assert.h>
#include <string.h>
#include "../src/kernel/fs/fs.h"

int main(void) {
    fs_init();
    fs_create("alpha");
    fs_create("beta");

    struct file *list[2];
    int count = fs_list(list, 2);
    assert(count == 2);
    assert(strcmp(list[0]->name, "alpha") == 0);
    assert(strcmp(list[1]->name, "beta") == 0);
    return 0;
}
