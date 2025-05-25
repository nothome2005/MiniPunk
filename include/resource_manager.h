#pragma once
#include "raylib.h"

// Глобальный доступ к текстурам (и другим ресурсам)
extern Texture g_background;
extern Texture coal_;
extern Texture iron_;
extern Texture tree_;
extern Texture generator_;
extern Texture ObjectCanva_;
extern Texture cursor_; // Добавить глобальную переменную для курсора
extern Texture player_idle_texture; // Добавлен extern Texture для текстуры игрока
extern Texture wolf_texture; // Добавлен extern Texture для спрайта волка
extern Font objectPanelFont; // Новый глобальный шрифт для панели

// Загрузка всех ресурсов
void LoadResources();

// Выгрузка всех ресурсов
void UnloadResources();