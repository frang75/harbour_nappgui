/* GTNAP Debugger Window */

#include <deblib/deblib.h>
#include <nappgui.h>

typedef struct _bufchar_t BufChar;
typedef struct _app_t App;

struct _bufchar_t
{
    char_t utf8[5];
    byte_t colorb;
    byte_t attrib;
};

struct _app_t
{
    Window *window;
    Font *font;
    TextView *text;
    View *view;
    bool_t alive;
    bool_t print_log;
    Mutex *mutex;
    Thread *protocol_thread;
    BufChar *text_buffer;
    vkey_t key_pressed;
    uint32_t key_modifiers;
    uint32_t ncols;
    uint32_t nrows;
    real32_t cell_width;
    real32_t cell_height;
    cursor_t cursor_type;
    uint32_t cursor_row;
    uint32_t cursor_col;
    bool_t cursor_draw;
    real64_t last_redraw;
};

/*---------------------------------------------------------------------------*/

static const real64_t i_BLINK_INTERVAL = 0.2;
static color_t i_COLORS[16];
static uint32_t i_DEFAULT_COLS = 80;
static uint32_t i_DEFAULT_ROWS = 25;
static char_t i_BOX_HORT_SINGLE[8];
static char_t i_BOX_VERT_SINGLE[8];
static char_t i_BOX_TOP_LEFT_SINGLE[8];
static char_t i_BOX_TOP_RIGHT_SINGLE[8];
static char_t i_BOX_DOWN_LEFT_SINGLE[8];
static char_t i_BOX_DOWN_RIGHT_SINGLE[8];

/*---------------------------------------------------------------------------*/

static void i_update_text_buffer(App *app, const uint32_t nrows, const uint32_t ncols)
{
    cassert_no_null(app);
    if (app->text_buffer != NULL)
        heap_delete_n(&app->text_buffer, app->nrows * app->ncols, BufChar);

    app->nrows = nrows;
    app->ncols = ncols;
    app->text_buffer = heap_new_n0(app->nrows * app->ncols, BufChar);

    if (app->view != NULL)
        view_size(app->view, s2df(app->ncols * app->cell_width, app->nrows * app->cell_height));

    if (app->window != NULL)
        window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static void i_lock(App *app)
{
    cassert_no_null(app);
    bmutex_lock(app->mutex);
}

/*---------------------------------------------------------------------------*/

static void i_unlock(App *app)
{
    cassert_no_null(app);
    bmutex_unlock(app->mutex);
}

/*---------------------------------------------------------------------------*/

static void i_OnDraw(App *app, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    BufChar *bchar = NULL;
    uint32_t i, j;
    cassert_no_null(app);
    bchar = app->text_buffer;
    i_lock(app);
    draw_font(p->ctx, app->font);
    for (i = 0; i < app->nrows; ++i)
    {
        real32_t y = i * app->cell_height;
        for (j = 0; j < app->ncols; ++j, ++bchar)
        {
            real32_t x = j * app->cell_width;
            if (bchar->utf8[0] != 0)
            {
                byte_t fore = bchar->colorb & 0x0F;
                byte_t back = (bchar->colorb & 0xF0) >> 4;
                color_t cfore = i_COLORS[fore];
                color_t cback = i_COLORS[back];
                draw_text_color(p->ctx, cfore);
                draw_fill_color(p->ctx, cback);
                draw_rect(p->ctx, ekFILL, x + 2, y, app->cell_width + 1, app->cell_height + 1);
                draw_text(p->ctx, bchar->utf8, x, y);
            }
        }
    }

    if (app->cursor_draw == TRUE && app->cursor_type != ekCURSOR_NONE)
    {
        if (app->cursor_row < app->nrows && app->cursor_col < app->ncols)
        {
            real32_t x = app->cursor_col * app->cell_width;
            real32_t y = app->cursor_row * app->cell_height;
            color_t cfore = i_COLORS[COL_WHITE];
            color_t cback = i_COLORS[COL_BLACK];
            bchar = app->text_buffer + (app->cursor_row * app->ncols) + app->cursor_col;
            draw_fill_color(p->ctx, cback);
            draw_rect(p->ctx, ekFILL, x + 1, y, app->cell_width + 1, app->cell_height + 1);

            if (bchar->utf8[0] != 0)
            {
                draw_text_color(p->ctx, cfore);
                draw_text(p->ctx, bchar->utf8, x, y);
            }
        }
    }
    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_OnKeyDown(App *app, Event *e)
{
    const EvKey *p = event_params(e, EvKey);
    cassert_no_null(app);
    if (p->key == ekKEY_LSHIFT || p->key == ekKEY_RSHIFT)
        app->key_pressed = ENUM_MAX(vkey_t);
    else
        app->key_pressed = p->key;
    app->key_modifiers = p->modifiers;
}

/*---------------------------------------------------------------------------*/

static void i_OnKeyUp(App *app, Event *e)
{
    const EvKey *p = event_params(e, EvKey);
    cassert_no_null(app);
    app->key_modifiers = p->modifiers;
}

/*---------------------------------------------------------------------------*/

static Panel *i_panel(App *app)
{
    Panel *panel = panel_create();
    Layout *layout = layout_create(2, 1);
    View *view = view_create();
    TextView *text = textview_create();
    view_OnDraw(view, listener(app, i_OnDraw, App));
    view_OnKeyDown(view, listener(app, i_OnKeyDown, App));
    view_OnKeyUp(view, listener(app, i_OnKeyUp, App));
    view_allow_tab(view, TRUE);
    app->font = font_monospace(font_regular_size(), 0);
    app->text = text;
    app->view = view;
    font_extents(app->font, "OOOO", -1, &app->cell_width, &app->cell_height);
    app->cell_width /= 4;
    layout_view(layout, view, 0, 0);
    layout_textview(layout, text, 1, 0);
    layout_tabstop(layout, 0, 0, TRUE);
    layout_tabstop(layout, 1, 0, FALSE);
    layout_hsize(layout, 1, 400);
    layout_show_col(layout, 1, app->print_log);
    panel_layout(panel, layout);
    return panel;
}

/*---------------------------------------------------------------------------*/

static void i_OnClose(App *app, Event *e)
{
    bool_t *r = event_result(e, bool_t);
    *r = FALSE;
    unref(app);
}

/*---------------------------------------------------------------------------*/

static void i_log(App *app, String **str)
{
    cassert_no_null(app);
    cassert_no_null(str);
    if (app->print_log == TRUE)
    {
        log_printf("%s", tc(*str));
        /* i_lock(app);
         textview_writef(app->text, tc(*str));
         textview_writef(app->text, "\n");
         i_unlock(app); */
    }
    str_destroy(str);
}

/*---------------------------------------------------------------------------*/

static void i_set_size(App *app, const DebMsg *msg)
{
    cassert_no_null(app);
    cassert_no_null(msg);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s Rows: %d, Cols: %d", deblib_msg_str(msg->type), msg->row, msg->col);
        i_log(app, &log);
    }

    if (app->nrows != msg->row || app->ncols != msg->col)
    {
        i_update_text_buffer(app, msg->row, msg->col);
        view_update(app->view);
    }
}

/*---------------------------------------------------------------------------*/

static void i_show(App *app, const DebMsg *msg)
{
    cassert_no_null(app);
    cassert_no_null(msg);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s %d", deblib_msg_str(msg->type), msg->show);
        i_log(app, &log);
    }

    if (msg->show == TRUE)
        window_show(app->window);
    else
        window_hide(app->window);
}

/*---------------------------------------------------------------------------*/

static void i_scroll(App *app, const DebMsg *msg)
{
    uint32_t i, j;
    cassert_no_null(app);
    cassert_no_null(msg);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s Top: %d, Left: %d, Bottom: %d, Right: %d, Char: '%s', Color: %d", deblib_msg_str(msg->type), msg->top, msg->left, msg->bottom, msg->right, msg->utf8, msg->colorb);
        i_log(app, &log);
    }

    i_lock(app);
    for (i = msg->top; i <= msg->bottom; ++i)
    {
        for (j = msg->left; j <= msg->right; ++j)
        {
            BufChar *bchar = app->text_buffer + (i * app->ncols + j);
            cassert(i < app->nrows);
            cassert(j < app->ncols);
            str_copy_c(bchar->utf8, sizeof(bchar->utf8), msg->utf8);
            bchar->colorb = msg->colorb;
            bchar->attrib = 0;
        }
    }
    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_box(App *app, const DebMsg *msg)
{
    uint32_t i, j;
    cassert_no_null(app);
    cassert_no_null(msg);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s Top: %d, Left: %d, Bottom: %d, Right: %d, Color: %d", deblib_msg_str(msg->type), msg->top, msg->left, msg->bottom, msg->right, msg->colorb);
        i_log(app, &log);
    }

    i_lock(app);
    for (i = msg->top + 1; i <= msg->bottom - 1; ++i)
    {
        /* Left edge */
        {
            BufChar *bchar = app->text_buffer + (i * app->ncols + msg->left);
            cassert(i < app->nrows);
            str_copy_c(bchar->utf8, sizeof(bchar->utf8), i_BOX_VERT_SINGLE);
            bchar->colorb = msg->colorb;
            bchar->attrib = 0;
        }

        /* Right edge */
        {
            BufChar *bchar = app->text_buffer + (i * app->ncols + msg->right);
            cassert(i < app->nrows);
            str_copy_c(bchar->utf8, sizeof(bchar->utf8), i_BOX_VERT_SINGLE);
            bchar->colorb = msg->colorb;
            bchar->attrib = 0;
        }
    }

    for (j = msg->left + 1; j <= msg->right - 1; ++j)
    {
        /* Top edge */
        {
            BufChar *bchar = app->text_buffer + (msg->top * app->ncols + j);
            cassert(j < app->ncols);
            str_copy_c(bchar->utf8, sizeof(bchar->utf8), i_BOX_HORT_SINGLE);
            bchar->colorb = msg->colorb;
            bchar->attrib = 0;
        }

        /* Bottom edge */
        {
            BufChar *bchar = app->text_buffer + (msg->bottom * app->ncols + j);
            cassert(j < app->ncols);
            str_copy_c(bchar->utf8, sizeof(bchar->utf8), i_BOX_HORT_SINGLE);
            bchar->colorb = msg->colorb;
            bchar->attrib = 0;
        }
    }

    /* Top-left corner */
    {
        BufChar *bchar = app->text_buffer + (msg->top * app->ncols + msg->left);
        str_copy_c(bchar->utf8, sizeof(bchar->utf8), i_BOX_TOP_LEFT_SINGLE);
        bchar->colorb = msg->colorb;
        bchar->attrib = 0;
    }

    /* Top-right corner */
    {
        BufChar *bchar = app->text_buffer + (msg->top * app->ncols + msg->right);
        str_copy_c(bchar->utf8, sizeof(bchar->utf8), i_BOX_TOP_RIGHT_SINGLE);
        bchar->colorb = msg->colorb;
        bchar->attrib = 0;
    }

    /* Bottom-left corner */
    {
        BufChar *bchar = app->text_buffer + (msg->bottom * app->ncols + msg->left);
        str_copy_c(bchar->utf8, sizeof(bchar->utf8), i_BOX_DOWN_LEFT_SINGLE);
        bchar->colorb = msg->colorb;
        bchar->attrib = 0;
    }

    /* Bottom-right corner */
    {
        BufChar *bchar = app->text_buffer + (msg->bottom * app->ncols + msg->right);
        str_copy_c(bchar->utf8, sizeof(bchar->utf8), i_BOX_DOWN_RIGHT_SINGLE);
        bchar->colorb = msg->colorb;
        bchar->attrib = 0;
    }

    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_cursor(App *app, const DebMsg *msg)
{
    cursor_t cursor = ENUM_MAX(cursor_t);
    cassert_no_null(app);
    cassert_no_null(msg);

    cursor = (cursor_t)msg->attrib;

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s Type: %s", deblib_msg_str(msg->type), deblib_cursor_str(cursor));
        i_log(app, &log);
    }

    i_lock(app);
    app->cursor_type = cursor;
    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_set_pos(App *app, const DebMsg *msg)
{
    cassert_no_null(app);
    cassert_no_null(msg);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s (%d, %d)", deblib_msg_str(msg->type), msg->row, msg->col);
        i_log(app, &log);
    }

    i_lock(app);
    app->cursor_row = msg->row;
    app->cursor_col = msg->col;
    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_get_pos(App *app, DebMsg *msg)
{
    cassert_no_null(app);
    cassert_no_null(msg);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s (%d, %d)", deblib_msg_str(msg->type), msg->row, msg->col);
        i_log(app, &log);
    }

    i_lock(app);
    msg->row = app->cursor_row;
    msg->col = app->cursor_col;
    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_putchar(App *app, const DebMsg *msg)
{
    BufChar *bchar = NULL;
    cassert_no_null(app);
    cassert_no_null(msg);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s Row: %d, Col: %d, Char: '%s', Color: %d, Attrib: %d", deblib_msg_str(msg->type), msg->row, msg->col, msg->utf8, msg->colorb, msg->attrib);
        i_log(app, &log);
    }

    cassert(msg->row < app->nrows);
    cassert(msg->col < app->ncols);
    i_lock(app);
    bchar = app->text_buffer + (msg->row * app->ncols + msg->col);
    str_copy_c(bchar->utf8, sizeof(bchar->utf8), msg->utf8);
    bchar->colorb = msg->colorb;
    bchar->attrib = msg->attrib;
    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_puttext(App *app, const DebMsg *msg)
{
    BufChar *bchar = NULL;
    const char_t *text = NULL;
    uint32_t codepoint, nbytes, col;
    cassert_no_null(app);
    cassert_no_null(msg);
    cassert(msg->row < app->nrows);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s Row: %d, Col: %d, Color: %d, Text: '%s'", deblib_msg_str(msg->type), msg->row, msg->col, msg->colorb, msg->utf8);
        i_log(app, &log);
    }

    col = msg->col;
    text = msg->utf8;
    i_lock(app);
    bchar = app->text_buffer + (msg->row * app->ncols + msg->col);
    codepoint = unicode_to_u32b(text, ekUTF8, &nbytes);

    while (codepoint != 0)
    {
        uint32_t nb = unicode_to_char(codepoint, bchar->utf8, ekUTF8);
        cassert_unref(nb == nbytes, nbytes);
        cassert_unref(col < app->ncols, col);
        bchar->utf8[nb] = 0;
        bchar->colorb = msg->colorb;
        bchar->attrib = 0;
        text += nb;
        bchar += 1;
        col += 1;
        codepoint = unicode_to_u32b(text, ekUTF8, &nbytes);
    }
    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_save(App *app, const DebMsg *msg, Stream *stm)
{
    uint32_t i, j;
    cassert_no_null(app);
    cassert_no_null(msg);
    cassert(msg->row < app->nrows);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s Top: %d, Left: %d, Bottom: %d, Right: %d", deblib_msg_str(msg->type), msg->top, msg->left, msg->bottom, msg->right);
        i_log(app, &log);
    }

    i_lock(app);

    for (i = msg->top; i <= msg->bottom; ++i)
    {
        const BufChar *bchar = app->text_buffer + i * app->ncols + msg->left;
        cassert(i < app->nrows);
        for (j = msg->left; j <= msg->right; ++j, ++bchar)
        {
            uint32_t codepoint = unicode_to_u32(bchar->utf8, ekUTF8);
            uint8_t color = (uint8_t)bchar->colorb;
            cassert(j < app->ncols);
            if (codepoint > 0xFFFF)
                codepoint = '?';
            stm_write_u16(stm, (uint16_t)codepoint);
            stm_write_u8(stm, color);
        }
    }

    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_rest(App *app, const DebMsg *msg, Stream *stm)
{
    uint32_t i, j;
    cassert_no_null(app);
    cassert_no_null(msg);
    cassert(msg->row < app->nrows);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("%s Top: %d, Left: %d, Bottom: %d, Right: %d", deblib_msg_str(msg->type), msg->top, msg->left, msg->bottom, msg->right);
        i_log(app, &log);
    }

    i_lock(app);

    for (i = msg->top; i <= msg->bottom; ++i)
    {
        BufChar *bchar = app->text_buffer + i * app->ncols + msg->left;
        cassert(i < app->nrows);
        for (j = msg->left; j <= msg->right; ++j, ++bchar)
        {
            uint32_t codepoint = stm_read_u16(stm);
            uint8_t color = stm_read_u8(stm);
            uint32_t nb = 0;
            cassert(j < app->ncols);
            nb = unicode_to_char(codepoint, bchar->utf8, ekUTF8);
            bchar->utf8[nb] = 0;
            bchar->colorb = color;
        }
    }

    i_unlock(app);
}

/*---------------------------------------------------------------------------*/

static void i_read_key(App *app, DebMsg *msg)
{
    cassert_no_null(app);
    cassert_no_null(msg);
    msg->key = app->key_pressed;
    msg->modifiers = app->key_modifiers;
    app->key_pressed = ENUM_MAX(vkey_t);

    if (app->print_log == TRUE)
    {
        if (msg->key != ENUM_MAX(vkey_t))
        {
            String *log = str_printf("%s Key: %d, Modif: %d", deblib_msg_str(msg->type), (int)msg->key, msg->modifiers);
            i_log(app, &log);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_unknown(App *app, const DebMsg *msg)
{
    cassert_no_null(app);
    cassert_no_null(msg);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("Unknown msg: %d", msg->type);
        i_log(app, &log);
    }
}

/*---------------------------------------------------------------------------*/

static uint32_t i_protocol_thread(App *app)
{
    Socket *server_sock = NULL;
    heap_start_mt();

    server_sock = bsocket_server(kDEBLIB_SERVER_PORT, 32, NULL);

    if (app->print_log == TRUE)
    {
        String *log = str_printf("Created server socket:%p %d", (void *)server_sock, kDEBLIB_SERVER_PORT);
        i_log(app, &log);
    }

    if (server_sock != NULL)
    {
        Socket *income_sock = bsocket_accept(server_sock, 0, NULL);

        if (app->print_log == TRUE)
        {
            String *log = str_printf("Created income socket:%p", (void *)income_sock);
            i_log(app, &log);
        }

        if (income_sock != NULL)
        {
            Stream *stm = stm_socket(income_sock);
            if (stm != NULL)
            {
                uint32_t ip;
                uint16_t port;
                bsocket_remote_ip(income_sock, &ip, &port);
                if (app->print_log == TRUE)
                {
                    String *log = str_printf("Incomming connect from: %s:%d", bsocket_ip_str(ip), port);
                    i_log(app, &log);
                }

                for (;;)
                {
                    if (app->alive == TRUE)
                    {
                        DebMsg msg;
                        deblib_recv_message(stm, &msg);

                        if (app->print_log == TRUE)
                        {
                            if (msg.type != ekMSG_READ_KEY)
                            {
                                String *log = str_printf("RECV Socket: %s", deblib_msg_str(msg.type));
                                i_log(app, &log);
                            }
                        }

                        switch (msg.type)
                        {
                        case ekMSG_CONNECT:
                            stm_write_u32(stm, kDEBLIB_HANDSHAKE);
                            break;

                        case ekMSG_SET_SIZE:
                            i_set_size(app, &msg);
                            break;

                        case ekMSG_SHOW:
                            i_show(app, &msg);
                            break;

                        case ekMSG_SCROLL:
                            i_scroll(app, &msg);
                            break;

                        case ekMSG_BOX:
                            i_box(app, &msg);
                            break;

                        case ekMSG_CURSOR:
                            i_cursor(app, &msg);
                            break;

                        case ekMSG_SET_POS:
                            i_set_pos(app, &msg);
                            break;

                        case ekMSG_GET_POS:
                            i_get_pos(app, &msg);
                            stm_write_u32(stm, msg.row);
                            stm_write_u32(stm, msg.col);
                            break;

                        case ekMSG_PUTCHAR:
                            i_putchar(app, &msg);
                            break;

                        case ekMSG_PUTTEXT:
                            i_puttext(app, &msg);
                            break;

                        case ekMSG_SAVE:
                            i_save(app, &msg, stm);
                            break;

                        case ekMSG_REST:
                            i_rest(app, &msg, stm);
                            break;

                        case ekMSG_READ_KEY:
                            i_read_key(app, &msg);
                            stm_write_enum(stm, msg.key, vkey_t);
                            stm_write_u32(stm, msg.modifiers);
                            break;

                        case ekMSG_CLOSE:
                            app->alive = FALSE;
                            break;

                        default:
                            i_unknown(app, &msg);
                            app->alive = FALSE;
                        }

                        if (app->print_log == TRUE)
                        {
                            if (msg.type != ekMSG_READ_KEY)
                            {
                                String *log = str_printf("END Socket: %s", deblib_msg_str(msg.type));
                                i_log(app, &log);
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                stm_close(&stm);
            }
        }

        bsocket_close(&server_sock);
    }

    heap_end_mt();
    return 0;
}

/*---------------------------------------------------------------------------*/

static App *i_app(void)
{
    App *app = heap_new0(App);
    uint32_t nrows = UINT32_MAX, ncols = UINT32_MAX;
    uint32_t argc = osapp_argc();
    app->print_log = FALSE;

    if (app->print_log == TRUE)
    {
#if defined(__LINUX__)
        log_file("/home/fran/Desktop/debugger_log.txt");
#elif defined(__WINDOWS__)
        log_file("C:\\Users\\Fran\\Desktop\\debugger_log.txt");
#elif defined(__MACOS__)
        log_file("/Users/fran/Desktop/debugger_log.txt");
#endif
    }

    if (app->print_log == TRUE)
        log_printf("argc: %d", argc);

    /* Parse arguments */
    if (argc == 3)
    {
        char_t argv[128];
        bool_t err1, err2;
        osapp_argv(1, argv, sizeof(argv));
        if (app->print_log == TRUE)
            log_printf("argv[1]: %s", argv);

        nrows = str_to_u32(argv, 10, &err1);
        osapp_argv(2, argv, sizeof(argv));
        if (app->print_log == TRUE)
            log_printf("argv[2]: %s", argv);

        ncols = str_to_u32(argv, 10, &err2);
        if (err1 == TRUE || err2 == TRUE)
        {
            nrows = UINT32_MAX;
            ncols = UINT32_MAX;
        }
    }

    if (app->print_log == TRUE)
        log_printf("Starting debugger: nrows: %d ncols: %d", nrows, ncols);

    if (ncols == UINT32_MAX || nrows == UINT32_MAX)
    {
        nrows = i_DEFAULT_ROWS;
        ncols = i_DEFAULT_COLS;
    }

    i_update_text_buffer(app, nrows, ncols);
    app->key_pressed = ENUM_MAX(vkey_t);
    app->key_modifiers = 0;
    app->alive = TRUE;
    app->cursor_type = ekCURSOR_NONE;
    app->cursor_row = UINT32_MAX;
    app->cursor_col = UINT32_MAX;
    app->cursor_draw = FALSE;
    app->last_redraw = -1;

    /* Set Unicode characters for box drawing */
    unicode_to_char(0x2501, i_BOX_HORT_SINGLE, ekUTF8);
    unicode_to_char(0x2503, i_BOX_VERT_SINGLE, ekUTF8);
    unicode_to_char(0x250F, i_BOX_TOP_LEFT_SINGLE, ekUTF8);
    unicode_to_char(0x2513, i_BOX_TOP_RIGHT_SINGLE, ekUTF8);
    unicode_to_char(0x2517, i_BOX_DOWN_LEFT_SINGLE, ekUTF8);
    unicode_to_char(0x251B, i_BOX_DOWN_RIGHT_SINGLE, ekUTF8);

    return app;
}

/*---------------------------------------------------------------------------*/

static void i_assert(void *item, const uint32_t group, const char_t *caption, const char_t *detail, const char_t *file, const uint32_t line)
{
    unref(item);
    log_printf("cassert group: %d, caption: %s, detail: %s, file: %s, line: %d", group, caption, detail, file, line);
}

/*---------------------------------------------------------------------------*/

static App *i_create(void)
{
    App *app = i_app();
    Panel *panel = i_panel(app);
    cassert_set_func((void *)app, i_assert);
    deblib_init_colors(i_COLORS);
    view_size(app->view, s2df(app->ncols * app->cell_width, app->nrows * app->cell_height));
    app->window = window_create(ekWINDOW_STD);
    window_panel(app->window, panel);
    window_title(app->window, "GTNap Debugger");
    window_origin(app->window, v2df(500, 200));
    window_OnClose(app->window, listener(app, i_OnClose, App));
    window_show(app->window);
    app->mutex = bmutex_create();
    app->protocol_thread = bthread_create(i_protocol_thread, app, App);
    return app;
}

/*---------------------------------------------------------------------------*/

static void i_update(App *app, const real64_t prtime, const real64_t ctime)
{
    cassert_no_null(app);
    unref(prtime);
    unref(ctime);
    if (app->alive == TRUE)
    {
        app->cursor_draw = !app->cursor_draw;
        view_update(app->view);
    }
    else
    {
        osapp_finish();
    }
}

/*---------------------------------------------------------------------------*/

static void i_destroy(App **app)
{
    cassert_no_null(app);
    cassert_no_null(*app);
    bthread_wait((*app)->protocol_thread);
    bthread_close(&(*app)->protocol_thread);
    bmutex_close(&(*app)->mutex);
    font_destroy(&(*app)->font);
    heap_delete_n(&(*app)->text_buffer, (*app)->nrows * (*app)->ncols, BufChar);
    window_destroy(&(*app)->window);
    heap_delete(app, App);
}

/*---------------------------------------------------------------------------*/

#include <osapp/osmain.h>
osmain_sync(i_BLINK_INTERVAL, i_create, i_destroy, i_update, "", App)
