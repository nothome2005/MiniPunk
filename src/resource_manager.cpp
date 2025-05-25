#include "resource_manager.h"
#include <string>
#include <cstring>

Texture g_background = { 0 };
Texture coal_ = { 0 };
Texture iron_ = { 0 };
Texture tree_ = { 0 };
Texture generator_ = { 0 };
Texture ObjectCanva_ = { 0 };
Texture cursor_ = { 0 };
Font objectPanelFont = { 0 };
Texture player_idle_texture = { 0 };
Texture wolf_texture = { 0 };

void LoadResources()
{
    g_background = LoadTexture("background2.png");
    coal_ = LoadTexture("coal.png");
    iron_ = LoadTexture("iron.png");
    tree_ = LoadTexture("tree.png");
    generator_ = LoadTexture("generator3.png");
    ObjectCanva_ = LoadTexture("ObjectInfoCanva2.png");
    cursor_ = LoadTexture("cursor.png");
    player_idle_texture = LoadTexture("Player_IDLE.png");
    wolf_texture = LoadTexture("wolf.png");
    
    const char* chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
        "ÀÁÂÃ¥ÄÅªÆÇÈ²¯ÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÜŞßàáâã´äåºæçè³¿éêëìíîïğñòóôõö÷øùüşÿ"
        "0123456789.,:;!?()[]{}<>|/@#%$^&*-_=+~`'\" ";
    int codepointCount = 0;
    int* codepoints = LoadCodepoints(chars, &codepointCount);
    int fontSize = 28;
    objectPanelFont = LoadFontEx("Fifaks10Dev1.ttf", fontSize, codepoints, codepointCount);
    UnloadCodepoints(codepoints);
}

void UnloadResources()
{
    UnloadTexture(g_background);
    UnloadTexture(coal_);
    UnloadTexture(iron_);
    UnloadTexture(tree_);
    UnloadTexture(generator_);
    UnloadTexture(ObjectCanva_);
    UnloadTexture(cursor_);
    UnloadTexture(player_idle_texture);
    UnloadTexture(wolf_texture);
    UnloadFont(objectPanelFont);
}

void DrawPanelTest(int panelCenterX, int panelCenterY)
{
    DrawTextEx(objectPanelFont, "PANEL TEST", {(float)(panelCenterX), (float)(panelCenterY)}, 28, 2, RED);
    DrawText("PANEL TEST", panelCenterX, panelCenterY + 40, 28, RED);
}