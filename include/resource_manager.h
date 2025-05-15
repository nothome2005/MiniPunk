#pragma once
#include "raylib.h"

// Глобальный доступ к текстурам (и другим ресурсам)
extern Texture g_background;

// Загрузка всех ресурсов
void LoadResources();

// Выгрузка всех ресурсов
void UnloadResources();