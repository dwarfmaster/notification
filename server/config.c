
#include "config.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

struct _entry {
    char* name;
    char* value;
    struct _entry* child;
    struct _entry* next;
};
static struct _entry* _root = NULL;

static char* config_path()
{
    char* home = getenv("XDG_CONFIG_HOME");
    char* path = NULL;
    size_t len = 0;
    struct stat buffer;
    int xdg = 1;

    if(!home
            || stat(home, &buffer) < 0
            || !S_ISDIR(buffer.st_mode)) {
        home = getenv("HOME");
        xdg = 0;

        if(!home
                || stat(home, &buffer) < 0
                || !S_ISDIR(buffer.st_mode))
            return NULL;
    }

    len = strlen(home);
    len += (xdg ? 2 : 3); /* /\0 ou /.\0 */
    len += strlen("notify_config");
    path = malloc(len);

    if(xdg)
        snprintf(path, len, "%s/notify_config", home);
    else
        snprintf(path, len, "%s/.notify_config", home);
    return path;
}

static struct _entry* find_token_in(const char* token, struct _entry* level)
{
    struct _entry* act = level;

    while(act) {
        if(strcmp(act->name, token) == 0)
            return act;
        else
            act = act->next;
    }

    return NULL;
}

static struct _entry* create_child_cond(const char* name, struct _entry* parent)
{
    struct _entry* ent;

    if(parent) {
        ent = find_token_in(name, parent->child);
        if(ent)
            return ent;
    } else if(_root) {
        ent = find_token_in(name, _root);
        if(ent)
            return ent;
    }

    ent = malloc(sizeof(struct _entry));
    ent->name = malloc(strlen(name) + 1);
    strcpy(ent->name, name);
    ent->value = NULL;
    ent->child = NULL;
    ent->next = NULL;

    if(parent) {
        ent->next  = parent->child;
        parent->child = ent;
    }
    else if(_root) {
        ent->next = _root->next;
        _root->next = ent;
    }
    else
        _root = ent;

    return ent;
}

static char* chomp_string(char* val)
{
    int len = strlen(val);
    char* str = val;
    int i = 0;

    while(i < len && 
            (val[i] == ' ' || val[i] == '\t' || val[i] == '\n'))
        ++i;
    str += i;

    i = len - 1;
    while(i >= 0 &&
            (val[i] == ' ' || val[i] == '\t' || val[i] == '\n'))
        --i;
    val[i+1] = '\0';

    return str;
}

static void parse_line(char* line)
{
    struct _entry* ent = NULL;
    char token[256];
    char value[256];
    char* str;
    unsigned int id = 0, secid = 0;
    int intok = 1;

    for(id = 0; id < strlen(line); ++id) {
        if(line[id] == '#')
            break;
        else if(intok && (line[id] == '.' || line[id] == '/')) {
            token[secid] = '\0';
            ent = create_child_cond(chomp_string(token), ent);
            secid = 0;
        }
        else if(intok && (line[id] == ':' || line[id] == '=')) {
            token[secid] = '\0';
            ent = create_child_cond(chomp_string(token), ent);
            secid = 0;
            intok = 0;
        }
        else if(intok) {
            token[secid++] = line[id];
            if(secid >= 256)
                break;
        }
        else {
            value[secid++] = line[id];
            if(secid >= 256)
                break;
        }
    }

    if(intok)
        secid = 0;
    value[secid] = '\0';

    if(ent) {
        str = chomp_string(value);
        ent->value = malloc(strlen(str) + 1);
        strcpy(ent->value, str);
    }
}

int load_config()
{
    char* path = config_path();
    char line[256];
    FILE* file;

    if(!path)
        return 0;
    file = fopen(path, "r");
    free(path);
    if(!file)
        return 0;

    while(fgets(line, 256, file))
        parse_line(line);

    fclose(file);
    return 1;
}

static void free_entry(struct _entry* ent)
{
    free_entry(ent->child);
    free_entry(ent->next);
    if(ent->name)
        free(ent->name);
    if(ent->value)
        free(ent->value);
    free(ent);
}

void free_config()
{
    free_entry(_root);
    _root = NULL;
}

static struct _entry* name_to_entry(const char* name)
{
    char* buffer = malloc(strlen(name));
    char* tok = strtok(buffer, "./");
    struct _entry* act = _root;

    while(tok) {
        act = find_token_in(tok, act);
        if(!act) {
            free(buffer);
            return NULL;
        }
        tok = strtok(NULL, "./");
    }

    free(buffer);
    return act;
}

int has_entry(const char* name)
{
    return name_to_entry(name) != NULL;
}

const char* get_string(const char* name)
{
    struct _entry* ent = name_to_entry(name);
    if(!ent)
        return "";
    else
        return ent->value;
}

int32_t get_int(const char* name)
{
    const char* str = get_string(name);
    int32_t ret = 0;
    sscanf(str, "%i", &ret);
    return ret;
}

int get_bool(const char* name)
{
    const char* str = get_string(name);
    int ret = 0;
    if(strcmp(str, "true") == 0
            || strcmp(str, "True") == 0
            || strcmp(str, "1") == 0)
        ret = 1;
    return ret;
}

static void dump_entry(struct _entry* ent, int ident)
{
    struct _entry* act = ent;
    int i;

    while(act) {
        for(i = 0; i < ident; ++i)
            printf("+");
        if(act->value)
            printf(" %s : \"%s\"\n", act->name, act->value);
        else
            printf(" %s\n", act->name);
        dump_entry(act->child, ident + 1);
        act = act->next;
    }
}

void dump_to_stdout()
{
    dump_entry(_root, 0);
}

