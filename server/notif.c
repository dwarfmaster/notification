
#include "notif.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct _word {
    char* word;
    uint32_t length;
    xcb_query_text_extents_cookie_t cookie;
};

static xcb_char2b_t* to_char2b(char* text)
{
    xcb_char2b_t* ret;
    uint32_t i;

    ret = malloc(strlen(text) * sizeof(xcb_char2b_t));
    for(i = 0; i < strlen(text); ++i) {
        ret[i].byte1 = text[i];
        ret[i].byte2 = 0;
    }

    return ret;
}

static struct _word* cut_in_words(xcb_connection_t* c, char* text, xcb_font_t font)
{
    struct _word* words = malloc(10 * sizeof(struct _word));
    uint32_t size = 10;
    uint32_t i = 0;
    char* word;
    xcb_char2b_t* xcbword;
    xcb_query_text_extents_reply_t* reply;

    word = strtok(text, " ");
    while(word) {
        words[i].word   = word;
        words[i].length = 0;
        xcbword = to_char2b(word);
        words[i].cookie = xcb_query_text_extents(c, font, strlen(word), xcbword);
        free(xcbword);

        word = strtok(NULL, " ");
        ++i;
        if(i >= size) {
            size += 10;
            words = realloc(words, sizeof(struct _word) * size);
        }
    }
    words[i].word = NULL;

    i = 0;
    while(words[i].word) {
        reply = xcb_query_text_extents_reply(c, words[i].cookie, NULL);
        words[i].length = reply->overall_width;
        /* TODO Debug output, to delete */
        printf("Word \"%s\" has length %u.\n", words[i].word, words[i].length);
        free(reply);
        ++i;
    }

    return words;
}

static uint32_t space_length(xcb_connection_t* c, xcb_font_t font)
{
    xcb_query_text_extents_reply_t* reply;
    xcb_query_text_extents_cookie_t cookie;
    uint32_t space;
    xcb_char2b_t sp;
    
    sp.byte1 = ' ';
    sp.byte1 = 0;
    cookie = xcb_query_text_extents(c, font, 1, &sp);
    reply  = xcb_query_text_extents_reply(c, cookie, NULL);
    space = reply->overall_width;
    free(reply);

    /* TODO remove, for debug only. */
    printf("Space length is %u.\n", space);

    return space;
}

static void add_line(srv_notif_t* notif, struct _word* words, uint32_t beg, uint32_t end, uint32_t line)
{
    uint32_t length, j;
    char* aline;

    length = 0;
    for(j = beg; j < end; ++j)
        length += strlen(words[j].word) + 1;
    notif->lines[line] = malloc(length);
    aline = notif->lines[line];

    for(j = beg; j < end; ++j) {
        strcpy(aline, words[j].word);
        aline += strlen(words[j].word);
        aline[0] = ' ';
        ++aline;
    }
    --aline;
    aline[0] = '\0';
    /* TODO remove, for debug only */
    printf("Line added : \"%s\"\n", notif->lines[line]);
}

srv_notif_t* create_notif(xcb_connection_t* c, srv_screen_t* scr, uint32_t y, const char* name, const char* text)
{
    struct _word* words;
    char* string;
    char buffer[256];
    uint32_t width, height, x;
    srv_gcontext_t gc;
    uint32_t i, size, last_i, length, line;
    uint32_t space;
    srv_notif_t* notif;

    if(!get_gcontext(name, &gc))
        return NULL;

    notif = malloc(sizeof(srv_notif_t));
    notif->gc = gc;

    string = malloc(strlen(text) + 1);
    strcpy(string, text);
    words = cut_in_words(c, string, gc.font);
    space = space_length(c, gc.font);

    snprintf(buffer, 256, "%s.width", name);
    width = 200;
    if(has_entry(buffer))
        width = get_int(buffer);
    else if(has_entry("global.width"))
        width = get_int("global.width");
    width -= gc.width * 2;

    size = 10;
    notif->lines = malloc(sizeof(char*) * size);

    last_i = length = 0;
    line = 0;
    for(i = 0; words[i].word;) {
        length += words[i].length;
        if(length >= width) {
            add_line(notif, words, last_i, i, line);
            last_i = i;
            ++line;
            length = 0;
        }
        length += space;

        ++i;
        if(line >= size) {
            size += 10;
            notif->lines = realloc(notif->lines, size);
        }
    }
    add_line(notif, words, last_i, i, line);
    ++line;
    notif->lines[line] = NULL;
    
    height = line * gc.font_height + gc.width * 2;
    width += gc.width * 2;
    x = scr->w - width - 50;
    notif->win = open_window(c, scr->xcbscr, x, y, width, height, name);

    free(string);
    free(words);
    return notif;
}

void draw_notif(xcb_connection_t* c, srv_notif_t* notif)
{
    display_notif(c, &notif->win, notif->gc, (const char**)notif->lines);
}

void free_notif(xcb_connection_t* c, srv_notif_t* notif)
{
    uint32_t i;
    for(i = 0; notif->lines[i]; ++i)
        free(notif->lines[i]);
    free(notif->lines);
    close_window(c, notif->win);
    free(notif);
}

