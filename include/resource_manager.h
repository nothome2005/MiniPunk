#pragma once
#include "raylib.h"

// ���������� ������ � ��������� (� ������ ��������)
extern Texture g_background;
extern Texture coal_;
extern Texture iron_;
extern Texture tree_;
extern Texture generator_;
extern Texture ObjectCanva_;
extern Texture cursor_; // �������� ���������� ���������� ��� �������
extern Font objectPanelFont; // ����� ���������� ����� ��� ������

// �������� ���� ��������
void LoadResources();

// �������� ���� ��������
void UnloadResources();