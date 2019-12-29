#include <stdio.h>
#include "simple_logger.h"
#include "gfc_types.h"
#include "gf2d_windows.h"
#include "gf2d_elements.h"
#include "gf2d_mouse.h"
#include "windows_common.h"
#include "scene.h"

extern void exitGame();
extern void exitCheck();

typedef struct
{
    TextLine    filename;
    Scene      *scene;
}EditorData;

int editor_window_draw(Window *win)
{
    EditorData *data;
    if (!win->data)return 0;
    data = win->data;
    scene_draw(data->scene);
    return 0;
}

int editor_window_free(Window *win)
{
    EditorData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    free(data);
    return 0;
}

int editor_window_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    Vector2D mouse;
    if (!win)return 0;
    if (!updateList)return 0;
    mouse = gf2d_mouse_get_position();
    if (mouse.y < 8)
    {
        win->dimensions.y = 0;
    }
    else if (!gf2d_window_mouse_in(win))
    {
        win->dimensions.y = -win->dimensions.h;
    }
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 56:
                exitCheck();
                return 1;
        }
    }
    return 0;
}

Window *editor_window(Scene * scene)
{
    Window *win;
    EditorData *data;
    win = gf2d_window_load("config/editor_window.json");
    if (!win)
    {
        slog("failed to load editor window");
        return NULL;
    }
    win->update = editor_window_update;
    win->free_data = editor_window_free;
    win->draw = editor_window_draw;
    data = (EditorData*)gfc_allocate_array(sizeof(EditorData),1);
    win->data = data;
    data->scene = scene;
    return win;
}

/*
 * 
 * Intro menu for the editor
 * 
 */

typedef struct
{
    TextLine filename;
    Window *editorMenu;
}EditorMenuData;

int editor_menu_free(Window *win)
{
    EditorMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    free(data);

    return 0;
}

void onFileNameCancel(void *Data)
{
    EditorMenuData* data;
    if (!Data)return;
    data = Data;
    gfc_line_cpy(data->filename,"config/");
    return;
}

void onFileNameOk(void *Data)
{
    EditorMenuData* data;
    Scene *scene;
    if (!Data)return;
    data = Data;
    
    scene = scene_load(data->filename);
    if (!scene)
    {
        window_alert("File not found", data->filename,NULL,NULL);
        onFileNameCancel(Data);
        return;
    }
    editor_window(scene);
    
    gf2d_window_free(data->editorMenu);
    return;
}

int editor_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    EditorMenuData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (EditorMenuData*)win->data;
    if (!data)return 0;
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                window_text_entry("Enter Scene to Load", data->filename, win->data, GFCLINELEN, onFileNameOk,onFileNameCancel);
                return 1;
            case 52:
                editor_window(scene_new());
                gf2d_window_free(win);
                return 1;
            case 53:
                exitGame();
                gf2d_window_free(win);
                return 1;
        }
    }
    return 0;
}


Window *editor_menu()
{
    Window *win;
    EditorMenuData* data;
    win = gf2d_window_load("config/editor_menu.json");
    if (!win)
    {
        slog("failed to load editor menu");
        return NULL;
    }
    win->update = editor_menu_update;
    win->free_data = editor_menu_free;
    data = (EditorMenuData*)gfc_allocate_array(sizeof(EditorMenuData),1);
    data->editorMenu = win;
    gfc_line_cpy(data->filename,"config/");
    win->data = data;
    return win;
}


/*eol@eof*/
