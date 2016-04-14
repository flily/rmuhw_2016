#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_STARTS_WITH(s, prefix)     (!strncmp(s, prefix, strlen(prefix)))

#define CONTACT_ITEM_FIXED_FIELD                \
    int id;                                     \
    int age;                                    \
    int resume_size;                            \
    int other;                                  \
    char name[32];                              \
    char phone[32];                             \
    char email[32];


struct contact_fixed_header_s
{
    CONTACT_ITEM_FIXED_FIELD
};

typedef struct contact_fixed_header_s contact_fixed_header_t;

struct contact_item_s
{
    CONTACT_ITEM_FIXED_FIELD
    char resume[128];
};

typedef struct contact_item_s contact_item_t;

int db_add_contact_item(const char* db_filename, contact_item_t* item)
{
    FILE* fd = NULL;
    
    if (NULL == item)
    {
        printf("[ERROR] no input data found\n");
        return 1;
    }
    
    if (NULL == (fd = fopen(db_filename, "a")))
    {
        printf("[ERROR] can not open db file '%s'\n", db_filename);
        return 1;
    }

    fwrite(item, sizeof(contact_fixed_header_t), 1, fd);
    fwrite(item->resume, 1, item->resume_size, fd);

    fflush(fd);
    fclose(fd);

    return 0;
}

int db_read_item(FILE* db, contact_item_t* item)
{
    size_t read_size = 0;
    if (NULL == item)
        return 1;

    memset(item, 0, sizeof(contact_item_t));
    read_size = fread(item, 1, sizeof(contact_fixed_header_t), db);
    if (read_size == 0)
        return 1;

    if (sizeof(contact_fixed_header_t) != read_size)
    {
        printf("[ERROR] broken db data read.\n");
        return 1;
    }

    read_size = fread(item->resume, 1, item->resume_size, db);
    if (read_size != item->resume_size)
    {
        printf("[ERROR] resume data broken.\n");
        return 1;
    }

    return 0;
}

int db_show_contact(const char* db_filename)
{
    FILE* fd = fopen(db_filename, "r");
    if (NULL == fd)
    {
        printf("[ERROR] can not open db file '%s'\n", db_filename);
        return 1;
    }

    printf("[%s]\t%s\t\t(%s)\t%s\t\t%s\t\t\t%s\n", "id", "name", "age", "phone", "email", "resume");
    while (!feof(fd))
    {
        contact_item_t item;

        if (db_read_item(fd, &item))
            break;

        printf("[%d]\t%s\t(%d)\t%s\t%s\t%s\n",
               item.id, item.name, item.age,
               item.phone, item.email, item.resume);
    }

    fclose(fd);
    return 0;
}

int db_del_contact_item(const char* db_filename, contact_item_t* item)
{
    return 0;
}

int db_find_contact_item(const char* db_filename, contact_item_t* pattern)
{
    FILE* fd = fopen(db_filename, "r");
    if (NULL == fd)
    {
        printf("[ERROR] can not open db file '%s'\n", db_filename);
        return 1;
    }

    printf("[%s]\t%s\t\t(%s)\t%s\t\t%s\t\t\t%s\n", "id", "name", "age", "phone", "email", "resume");
    while (!feof(fd))
    {
        contact_item_t item;

        if (db_read_item(fd, &item))
            break;

        if (pattern->id != 0 && pattern->id != item.id)
            continue;

        if (pattern->age != 0 && pattern->age != item.age)
            continue;

        if (pattern->name[0] != 0 && NULL == strstr(item.name, pattern->name))
            continue;

        if (pattern->phone[0] != 0 && NULL == strstr(item.phone, pattern->phone))
            continue;

        if (pattern->email[0] != 0 && NULL == strstr(item.email, pattern->email))
            continue;

        if (pattern->resume[0] != 0 && NULL == strstr(item.resume, pattern->resume))
            continue;

        printf("[%d]\t%s\t(%d)\t%s\t%s\t%s\n",
               item.id, item.name, item.age,
               item.phone, item.email, item.resume);
    }

    fclose(fd);
    return 0;
}

contact_item_t* parse_contact_item(int argc, char* argv[])
{
    int i = 0;
    size_t item_size = sizeof(contact_item_t);
    contact_item_t* item = (contact_item_t*) malloc(item_size);
    if (NULL == item)
        return NULL;

    memset(item, 0, item_size);
    for (i = 3; i < argc; i++)
    {
        if (STR_STARTS_WITH(argv[i], "--id="))
        {
            item->id = atoi(&argv[i][5]);
        }
        else if (STR_STARTS_WITH(argv[i], "--age="))
        {
            item->age = atoi(&argv[i][6]);
        }
        else if (STR_STARTS_WITH(argv[i], "--name="))
        {
            strncpy(item->name, &argv[i][7], 32);
        }
        else if (STR_STARTS_WITH(argv[i], "--phone="))
        {
            strncpy(item->phone, &argv[i][8], 32);
        }
        else if (STR_STARTS_WITH(argv[i], "--email="))
        {
            strncpy(item->email, &argv[i][8], 32);
        }
        else if (STR_STARTS_WITH(argv[i], "--resume="))
        {
            size_t resume_size = strlen(&argv[i][9]);
            item->resume_size = resume_size;
            strncpy(item->resume, &argv[i][9], 128);
        }
    }

#if 0
    printf("id: %d\n" "age: %d\n"
           "name: %s\n" "phone: %s\n"
           "email: %s\n" "resume: %s\n",
           item->id, item->age, item->name,
           item->phone, item->email, item->resume);
#endif

    return item;
}

void usage(const char* name)
{
    printf("%s dbfile [command] [args...]\n", name);
    exit(0);
}

int main(int argc, char* argv[])
{
    contact_item_t* item = NULL;
    char* db_filename = NULL;
    char* c = NULL;
    int retval = 0;

    if (argc < 3)
        usage(argv[0]);

    db_filename = argv[1];
    c = argv[2];

    item = parse_contact_item(argc, argv);
    if (NULL == item)
    {
        printf("parse failed.\n");
        return 1;
    }

    if (!strcmp("add", c) || !strcmp("a", c))
    {
        retval = db_add_contact_item(db_filename, item);
    }
    else if (!strcmp("show", c) || !strcmp("s", c))
    {
        retval = db_show_contact(db_filename);
    }
    else if (!strcmp("del", c) || !strcmp("d", c))
    {
    }
    else if (!strcmp("find", c) || !strcmp("f", c))
    {
        retval = db_find_contact_item(db_filename, item);
    }
    else
    {
        printf("unknown command: [%s]\n", c);
    }

    free(item);

    return retval;
}

