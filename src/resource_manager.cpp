#include "resource_manager.h"

// ����������� ���������� ����������
Texture g_background = { 0 };

void LoadResources()
{
    g_background = LoadTexture("background.png");
    // ����� ����� �������� �������� ������ ��������
}

void UnloadResources()
{
    UnloadTexture(g_background);
    // ����� ����� �������� �������� ������ ��������
}