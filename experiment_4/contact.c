#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SQL_STATEMENT_BUFFER_SIZE               4096
#define STR_STARTS_WITH(s, prefix)     (!strncmp(s, prefix, strlen(prefix)))

#define CONTACT_ITEM_FIXED_FIELD                \
    int id;                                     \
    int age;                                    \
    int resume_size;                            \
    int other;                                  \
    char name[32];                              \
    char phone[32];                             \
    char email[32];


#define CONTACT_FIELD_ID                        1
#define CONTACT_FIELD_AGE                       2
#define CONTACT_FIELD_NAME                      3
#define CONTACT_FIELD_PHONE                     4
#define CONTACT_FIELD_EMAIL                     5
#define CONTACT_FIELD_RESUME                    6

#define SQL_VERB_ERROR                          0
#define SQL_VERB_SELECT                         1
#define SQL_VERB_ADD                            2


#if 0
#define DEBUG
#endif

#ifdef DEBUG
#define DEBUG_PRINT(...)          printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif


#define SKIP_SPACE(c)                                           \
            do                                                  \
            {                                                   \
                while (isspace(*c))                             \
                    c++;                                        \
            } while (0)


struct dbctx_s
{
    const char* db_filename;
};

typedef struct dbctx_s dbctx_t;

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

struct contact_row_projection_s
{
    int id;
    int age;
    int name;
    int phone;
    int email;
    int resume;
};

typedef struct contact_row_projection_s contact_row_projection_t;

const char* parse(dbctx_t* ctx, const char* sql);

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

int db_show_contact(const char* db_filename, contact_row_projection_t* proj)
{
    FILE* fd = fopen(db_filename, "r");
    if (NULL == fd)
    {
        printf("[ERROR] can not open db file '%s'\n", db_filename);
        return 1;
    }

    if (NULL == proj || proj->id)
        printf("[id]\t");

    if (NULL == proj || proj->name)
        printf("name\t\t");

    if (NULL == proj || proj->age)
        printf("(age)\t");

    if (NULL == proj || proj->phone)
        printf("phone\t\t");

    if (NULL == proj || proj->email)
        printf("email\t\t");

    if (NULL == proj || proj->resume)
        printf("resume\t\t");

    printf("\n");

    while (!feof(fd))
    {
        contact_item_t item;

        if (db_read_item(fd, &item))
            break;

        /*
        printf("[%d]\t%s\t(%d)\t%s\t%s\t%s\n",
               item.id, item.name, item.age,
               item.phone, item.email, item.resume);
        */
        if (NULL == proj || proj->id)
            printf("[%d]\t", item.id);

        if (NULL == proj || proj->name)
            printf("%s\t", item.name);

        if (NULL == proj || proj->age)
            printf("(%d)\t", item.age);

        if (NULL == proj || proj->phone)
            printf("%s\t", item.phone);

        if (NULL == proj || proj->email)
            printf("%s\t", item.email);

        if (NULL == proj || proj->resume)
            printf("%s", item.resume);

        printf("\n");
    }

    fclose(fd);
    return 0;
}

int db_del_contact_item(const char* db_filename, contact_item_t* item)
{
    return 0;
}

int db_find_contact_item(const char* db_filename, contact_item_t* pattern, contact_row_projection_t* proj)
{
    FILE* fd = fopen(db_filename, "r");
    if (NULL == fd)
    {
        printf("[ERROR] can not open db file '%s'\n", db_filename);
        return 1;
    }

    if (NULL == proj || proj->id)
        printf("[id]\t");

    if (NULL == proj || proj->name)
        printf("name\t\t");

    if (NULL == proj || proj->age)
        printf("(age)\t");

    if (NULL == proj || proj->phone)
        printf("phone\t\t");

    if (NULL == proj || proj->email)
        printf("email\t\t");

    if (NULL == proj || proj->resume)
        printf("resume\t\t");

    printf("\n");

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

        if (NULL == proj || proj->id)
            printf("[%d]\t", item.id);

        if (NULL == proj || proj->name)
            printf("%s\t", item.name);

        if (NULL == proj || proj->age)
            printf("(%d)\t", item.age);

        if (NULL == proj || proj->phone)
            printf("%s\t", item.phone);

        if (NULL == proj || proj->email)
            printf("%s\t", item.email);

        if (NULL == proj || proj->resume)
            printf("%s", item.resume);

        printf("\n");
    }

    fclose(fd);
    return 0;
}

int strstart(const char* str, const char* head)
{
    size_t strg_len = strlen(str);
    size_t head_len = strlen(head);

    if (strg_len < head_len)
        return 0;

    else if (strg_len == head_len)
        return !strcmp(str, head);

    else
        return !strncmp(str, head, head_len);
}

int strstart_token(const char* str, const char* head, char** next_start)
{
    size_t strg_len = strlen(str);
    size_t head_len = strlen(head);
    char* next = NULL;

    if (strg_len < head_len)
        return 0;

    else if (strg_len == head_len)
    {
        if (NULL != next_start)
            *next_start = (char*) str + strg_len;
        return !strcmp(str, head);
    }

    next = (char*) str + head_len;
    if (isalnum(*next) || '_' == *next)
        return 0;

    if (NULL != next_start)
        *next_start = next;
    return !strncmp(str, head, head_len);
}

const char* parse_condition_statement(const char* sql, contact_item_t* item, char** next)
{
    char* c = (char*) sql;

    while (1)
    {
        int field = 0;
        char* vhead= NULL;
        char* vtail = NULL;
        char value[1024] = "";

        DEBUG_PRINT("c: %s<<<\n", c);

        SKIP_SPACE(c);
        if (strstart_token(c, "id", &vhead))
            field = CONTACT_FIELD_ID;

        else if (strstart_token(c, "age", &vhead))
            field = CONTACT_FIELD_AGE;

        else if (strstart_token(c, "name", &vhead))
            field = CONTACT_FIELD_NAME;

        else if (strstart_token(c, "phone", &vhead))
            field = CONTACT_FIELD_PHONE;

        else if (strstart_token(c, "email", &vhead))
           field = CONTACT_FIELD_EMAIL;

        else if (strstart_token(c, "resume", &vhead))
           field = CONTACT_FIELD_RESUME;

        else
        {
            char var[1024] = "";
            const char* x = c;
            while (isalnum(*x) || '_' == *x)
                x++;

            strncpy(var, c, x - c);
            printf("[ERROR] invalid field '%s'\n", var);
            return c;
        }

        DEBUG_PRINT("field: %d\n", field);

        SKIP_SPACE(vhead);
        if ('=' != *vhead)
        {
            printf("[ERROR] an asignment '=' expect\n");
            return vhead;
        }

        vhead++;
        SKIP_SPACE(vhead);
        vtail = vhead;
        while (';' != *vtail && ',' != *vtail && 0 != *vtail)
            vtail ++;

        strncpy(value, vhead, vtail - vhead);
        DEBUG_PRINT("Field '%d': %s<<<\n", field, value);
        switch (field)
        {
            case CONTACT_FIELD_ID:
                item->id = atoi(value);
                break;
            case CONTACT_FIELD_AGE:
                item->age = atoi(value);
                break;
            case CONTACT_FIELD_NAME:
                strncpy(item->name, value, 32);
                break;
            case CONTACT_FIELD_PHONE:
                strncpy(item->phone, value, 32);
                break;
            case CONTACT_FIELD_EMAIL:
                strncpy(item->email, value, 32);
                break;
            case CONTACT_FIELD_RESUME:
                item->resume_size = strlen(value);
                strncpy(item->resume, value, 128);
                break;
            default:
                break;
        }

        c = vtail;
        SKIP_SPACE(c);
        if (',' == *c)
        {
            c++;
            continue;
        }
        else if (';' == *c || 0 == *c)
        {
            break;
        }
    }

    *next = c;
    return NULL;
}

const char* parse_select_projection(const char* sql, contact_row_projection_t* p, char** next_start)
{
    char* c = (char*) sql;
    char* next = NULL;
    int have_column = 0;

    while (1)
    {
        SKIP_SPACE(c);
        if (strstart_token(c, "id", &next))
        {
            p->id = 1;
            have_column = 1;
        }

        else if (strstart_token(c, "age", &next))
        {
            p->age = 1;
            have_column = 1;
        }

        else if (strstart_token(c, "name", &next))
        {
            p->name = 1;
            have_column = 1;
        }

        else if (strstart_token(c, "phone", &next))
        {
            p->phone = 1;
            have_column = 1;
        }

        else if (strstart_token(c, "email", &next))
        {
            p->email = 1;
            have_column = 1;
        }

        else if (strstart_token(c, "resume", &next))
        {
            p->resume = 1;
            have_column = 1;
        }

        else if (strstart_token(c, "where", NULL))
        {
            if (0 == have_column)
            {
                printf("[ERROR] except column name.\n");
                return c;
            }

            next = c;
            break;
        }

        else if ('*' == *c)
        {
            p->id = 1;
            p->age= 1;
            p->name = 1;
            p->phone = 1;
            p->email = 1;
            p->resume = 1;
            next = c + 1;
            have_column = 1;
            break;
        }
        
        else if (0 == *c || ';' == *c)
        {
            if (0 == have_column)
            {
                printf("[ERROR] except column name.\n");
                return c;
            }

            next = c;
            break;
        }

        else
        {
            char var[1024] = "";
            char* tail = c;

            while (isalnum(*tail) || '_' == *tail)
                tail++;

            strncpy(var, c, tail - c);
            printf("[ERROR] invalid column '%s'\n", var);
            return c;
        }

        c = next;
        SKIP_SPACE(c);
        if (',' == *c)
            c++;
    }

    DEBUG_PRINT("PROJECTION: id=%d, age=%d, name=%d, phone=%d, email=%d, resume=%d\n",
            p->id, p->age, p->name, p->phone, p->email, p->resume);

    *next_start = next;
    return NULL;
}

const char* parse_select(dbctx_t* ctx, const char* sql)
{
    char* c = (char*) sql;
    char* next = NULL;
    const char* err = NULL;
    contact_row_projection_t projection;
    contact_item_t item;

    DEBUG_PRINT("SELECT: %s\n", sql);

    memset(&projection, 0, sizeof(contact_row_projection_t));
    memset(&item, 0, sizeof(contact_item_t));

    err = parse_select_projection(c, &projection, &next);
    if (NULL != err)
        return err;

    c = next;
    SKIP_SPACE(c);

    if (0 == *c)
    {
        db_show_contact(ctx->db_filename, &projection);
        return NULL;
    }

    if (';' == *c)
    {
        db_show_contact(ctx->db_filename, &projection);
        return parse(ctx, c + 1);
    }

    DEBUG_PRINT("PARSE WHERE: %s<<<\n", c);
    if (!strstart_token(c, "where", &next))
    {
        printf("[ERROR] expect where statement\n");
        return c;
    }

    c = next;
    err = parse_condition_statement(c, &item, &next);
    if (NULL != err)
        return err;

    db_find_contact_item(ctx->db_filename, &item, &projection);
    if (';' == *next)
        return parse(ctx, next + 1);

    return NULL;
}

const char* parse_insert_values(dbctx_t* ctx, const char* sql)
{
    contact_item_t item;
    const char* err = NULL;
    char* next = NULL;

    DEBUG_PRINT("INSERT VALUES: %s\n", sql);
    memset(&item, 0, sizeof(contact_item_t));

    err = parse_condition_statement(sql, &item, &next);
    if (NULL != err)
        return err;
    
    db_add_contact_item(ctx->db_filename, &item);
    if (';' == *next)
        return parse(ctx, next + 1);
    
    return 0;
}

const char* parse_insert(dbctx_t* ctx, const char* sql)
{
    char* c = (char*) sql;
    char* f = NULL;

    while (isspace(*c))
        c++;

    DEBUG_PRINT("INSERT: %s\n", c);
    if (strstart_token(c, "value", &f))
        return parse_insert_values(ctx, f);

    else if (strstart_token(c, "values", &f))
        return parse_insert_values(ctx, f);

    else if (0 == *c || strstart(c, ";"))
    {
        printf("[WARNING] insert nothing.\n");
        return NULL;
    }

    else
    {
        printf("[ERROR] expect keyword 'value' or 'values' here, or ';' to terminate statement\n");
        return c;
    }
}

const char* parse(dbctx_t* ctx, const char* sql)
{
    const char* c = sql;

    while (isspace(*c))
        c++;

    if (0 == *c)
        return NULL;
    
    if (strstart(c, "select"))
        return parse_select(ctx, c + 6);

    else if (strstart(c, "insert"))
        return parse_insert(ctx, c + 6);

    else
    {
        printf("Unknow verb: %s\n", sql);
        return sql;
    }
}

void usage(const char* name)
{
    printf("%s dbfile \n", name);
    exit(0);
}

int main(int argc, char* argv[])
{
    int retval = 0;
    char sql[SQL_STATEMENT_BUFFER_SIZE + 1] = "";
    dbctx_t ctx;

    if (argc <= 1)
    {
        printf("no db file specified.\n");
        usage(argv[0]);
        return 0;
    }

    ctx.db_filename = argv[1];

    while (1)
    {
        char* read_str = NULL;
        const char* err_chr = NULL;
        printf("SQL> ");
        fflush(stdout);

        read_str = fgets(sql, SQL_STATEMENT_BUFFER_SIZE, stdin);
        if (NULL == read_str)
        {
            printf("EOF\n");
            break;
        }

        /* remove the last '\n' */
        sql[strlen(sql) - 1] = 0;

        if (NULL != (err_chr = parse(&ctx, sql)))
        {
            char* c = sql;
            printf("syntax error:\n  '%s'\n", sql);

            printf("   ");
            for (c = sql; c != err_chr; c++)
                putchar(' ');
            printf("^\n");

            printf("example:\n"
                   "  select *\n"
                   "  insert values id = 1, age = 20, name = Lily, phone = 12345, email = lily@example.com, resume = hi;\n"
                  );
            continue;
        }
    }

    return retval;
}

