#include <raylib.h>
#include <stdbool.h> // 确保包含这个头文件以使用bool类型

// 全局字体资源
typedef struct {
    unsigned char* fontData;
    unsigned int fileSize;
    Font font;
    bool loaded;
} FontResource;

// 初始化字体资源
FontResource LoadFontResource(const char* fontPath) {
    FontResource res = { 0 };
    res.fontData = LoadFileData(fontPath, &res.fileSize);
    if (res.fontData) {
        // 预加载常用ASCII字符和基本中文
        const char* preloadChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
            "!@#$%^&*()_+-=[]{};':\",./<>?\\|`~ 你好世界中文支持测试，！";
        int codepointCount;
        int* codepoints = LoadCodepoints(preloadChars, &codepointCount);
        res.font = LoadFontFromMemory(".ttf", res.fontData, res.fileSize, 32, codepoints, codepointCount);
        UnloadCodepoints(codepoints);
        res.loaded = (res.font.texture.id != 0);
    }
    return res;
}

// 释放字体资源
void UnloadFontResource(FontResource* res) {
    if (res->loaded) {
        UnloadFont(res->font);
        UnloadFileData(res->fontData);
        res->loaded = false;
    }
}

// 动态绘制文本（支持中英文混合）
void DrawDynamicText(FontResource* res, const char* text, Vector2 position, float fontSize, float spacing, Color color) {
    // 如果预加载字体可用，直接使用
    if (res->loaded) {
        float scale = fontSize / (float)res->font.baseSize;
        DrawTextEx(res->font, text, position, fontSize, spacing, color);
    }
    else {
        // 备用方案：动态加载（性能较低）
        int codepointCount;
        int* codepoints = LoadCodepoints(text, &codepointCount);
        Font tempFont = LoadFontFromMemory(".ttf", res->fontData, res->fileSize, fontSize, codepoints, codepointCount);
        DrawTextEx(tempFont, text, position, fontSize, spacing, color);
        UnloadFont(tempFont);
        UnloadCodepoints(codepoints);
    }
}

int main() {
    InitWindow(800, 600, "中文显示示例");

    // 加载字体资源（建议使用支持中文的字体）
    FontResource fontRes = LoadFontResource("font/font.ttf");
    if (!fontRes.loaded) {
        TraceLog(LOG_WARNING, "无法加载字体，尝试备用字体...");
        fontRes = LoadFontResource("font/DotGothic16-Regular.ttf");
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        // 绘制中文文本
        DrawDynamicText(&fontRes, "你好，世界！", (Vector2) { 50, 50 }, 40, 1, RED);
        DrawDynamicText(&fontRes, "Hello, 世界!", (Vector2) { 50, 100 }, 30, 1, BLUE);
        DrawDynamicText(&fontRes, "Raylib中文支持测试", (Vector2) { 50, 150 }, 25, 1, BLACK);

        EndDrawing();
    }

    // 释放资源
    UnloadFontResource(&fontRes);
    CloseWindow();

    return 0;
}