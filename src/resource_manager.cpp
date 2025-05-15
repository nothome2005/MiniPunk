#include "resource_manager.h"

// Определение глобальных переменных
Texture g_background = { 0 };

void LoadResources()
{
    g_background = LoadTexture("background.png");
    // Здесь можно добавить загрузку других ресурсов
}

void UnloadResources()
{
    UnloadTexture(g_background);
    // Здесь можно добавить выгрузку других ресурсов
}