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
        contact_fixed_header_t header;
        char resume[128] = "";
        size_t read_size = 0, resume_size = 0;

        read_size = fread(&header, 1, sizeof(contact_fixed_header_t), fd);
        if (read_size == 0)
           break;

        if(sizeof(contact_fixed_header_t) != read_size)
        {
            printf("[ERROR] broken db data read.\n");
            break;
        }

        resume_size = fread(resume, 1, header.resume_size, fd);
        if (resume_size != header.resume_size)
        {
            printf("[ERROR] resume data broken.\n");
            break;
        }

        printf("[%d]\t%s\t(%d)\t%s\t%s\t%s\n",
               header.id, header.name, header.age,
               header.phone, header.email, resume);
    }

    fclose(fd);
    return 0;
}

int db_del_contact_item(const char* db_filename, contact_item_t* item)
{
    return 0;
}

int db_find_contact_item(const char* db_filename, contact_item_t* item)
{
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
    }
    else
    {
        printf("unknown command: [%s]\n", c);
    }

    free(item);

    return retval;
}

