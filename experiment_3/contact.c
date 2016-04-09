#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct contact_item_s
{
    int id;
    int age;
    int resume_size;
    char name[32];
    char phone[32];
    char email[32];
    char* resume;
};

typedef struct contact_item_s contact_item_t;

contact_item_t* new_contact_item(int id)
{
    return NULL;
}

void usage(const char* name)
{
    printf("%s dbfile [command] [args...]\n", name);
    exit(0);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
        usage(argv[0]);

    return 0;
}

