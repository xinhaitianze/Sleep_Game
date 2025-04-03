#include <raylib.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	unsigned char* fontData;
	unsigned int fileSize;
	Font font;
	bool loaded;
} FontResource;

FontResource LoadFontResource(const char* fontPath) {
	FontResource res = { 0 };
	res.fontData = LoadFileData(fontPath, &res.fileSize);
	if (res.fontData) {
		// 预加载常用ASCII字符
		const char* preloadChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
			"!@#$%^&*()_+-=[]{};':\",./<>?\\|`~ ，！";

		// 预加载基本汉字区的汉字
		int codepointCount = 0;
		int* codepoints = (int*)malloc(21000 * sizeof(int));  // 假设21000个汉字，当然这个数字是我瞎编的
		for (int i = 0x4E00; i <= 0x9FFF; i++) {
			codepoints[codepointCount++] = i;
		}

		// 合并预加载的字符和汉字
		int totalCodepointCount = codepointCount + strlen(preloadChars);
		int* totalCodepoints = (int*)malloc(totalCodepointCount * sizeof(int));
		for (int i = 0; i < codepointCount; i++) {
			totalCodepoints[i] = codepoints[i];
		}
		for (int i = 0; i < strlen(preloadChars); i++) {
			totalCodepoints[codepointCount + i] = preloadChars[i];
		}

		res.font = LoadFontFromMemory(".ttf", res.fontData, res.fileSize, 32, totalCodepoints, totalCodepointCount);
		free(codepoints);
		free(totalCodepoints);
		res.loaded = (res.font.texture.id != 0);
	}
	return res;
}

int main(void) {
	// 初始化窗口
	InitWindow(1300, 800, "Font Cache Demo");
	SetTargetFPS(60);

	// 加载基础字体
	FontResource fontRes = LoadFontResource("font/font.ttf");

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		// 使用缓存绘制文本
		DrawTextEx(fontRes.font, "SLEEP", (Vector2) { 515, 50 }, 100, 2, WHITE);
		DrawTextEx(fontRes.font, "===睡觉===", (Vector2) { 495, 140 }, 55, 2, WHITE);
		EndDrawing();
	}

	// 清理资源
	UnloadFont(fontRes.font);
	UnloadFileData(fontRes.fontData);
	CloseWindow();

	return 0;
}
