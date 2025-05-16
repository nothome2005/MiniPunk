#include "resource_manager.h"
#include <string>
#include <cstring>

// Определение глобальных переменных
Texture g_background = { 0 };
Texture coal_ = { 0 };
Texture iron_ = { 0 };
Texture tree_ = { 0 };
Texture generator_ = { 0 };
Texture ObjectCanva_ = { 0 };
Texture cursor_ = { 0 }; // Глобальная переменная для курсора
Font objectPanelFont = { 0 }; // Глобальный шрифт для панели

void LoadResources()
{
    g_background = LoadTexture("background2.png");
    coal_ = LoadTexture("coal.png");
    iron_ = LoadTexture("iron.png");
    tree_ = LoadTexture("tree.png");
    generator_ = LoadTexture("generator3.png");
    ObjectCanva_ = LoadTexture("ObjectInfoCanva2.png");
    cursor_ = LoadTexture("cursor.png"); // Загрузка курсора
    // Расширенный набор символов (латиница, кириллица, цифры, знаки)
    const char* chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
        "АБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯабвгґдеєжзиіїйклмнопрстуфхцчшщьюя"
        "0123456789.,:;!?()[]{}<>|/@#%$^&*-_=+~`'\" ";
    int codepointCount = 0;
    int* codepoints = LoadCodepoints(chars, &codepointCount);
    int fontSize = 28; // Увеличен до 28 для совпадения с DrawTextEx
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
    UnloadTexture(cursor_); // Выгрузка курсора
    UnloadFont(objectPanelFont); // Выгрузка шрифта панели
}

void DrawPanelTest(int panelCenterX, int panelCenterY)
{
    // Нарисовать тестовый текст objectPanelFont прямо на панели, по центру
    DrawTextEx(objectPanelFont, "PANEL TEST", {(float)(panelCenterX), (float)(panelCenterY)}, 28, 2, RED);
    // Нарисовать стандартным шрифтом для сравнения
    DrawText("PANEL TEST", panelCenterX, panelCenterY + 40, 28, RED);
}