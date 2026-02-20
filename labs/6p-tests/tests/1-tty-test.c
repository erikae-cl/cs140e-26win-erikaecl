#include "libunix.h"

int main(void) {

    char *first = find_ttyusb_first();
    output("find_ttyusb_first returned: <%s>\n", first ? first : "NULL");


    char *last = find_ttyusb_last();
    output("find_ttyusb_last  returned: <%s>\n", last ? last : "NULL");


    output("Calling find_ttyusb()...\n");
    char *strict = find_ttyusb();
    output("find_ttyusb       returned: <%s>\n", strict);
    return 0;
}