#pragma once
#include "raylib.h"

// ���������� ������ � ��������� (� ������ ��������)
extern Texture g_background;
extern Texture coal_;
extern Texture iron_;
extern Texture tree_;
extern Texture generator_;
extern Texture cursor_;
extern Texture player_idle_texture;
extern Texture wolf_texture;

// �������� ���� ��������
void LoadResources();

// �������� ���� ��������
void UnloadResources();