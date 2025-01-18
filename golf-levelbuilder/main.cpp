#include "raylib.h"
#include "raymath.h"

// Define screen width and height
#define SCREEN_WIDTH 1296
#define SCREEN_HEIGHT 720

int windowScale = 2;
#define VIRTUAL_WIDTH (int) SCREEN_WIDTH / windowScale
#define VIRTUAL_HEIGHT (int) SCREEN_HEIGHT / windowScale

// Macro to replace GetMousePosition with a scaled version
#define GetMousePosition() (Vector2){ \
    GetMousePosition().x * (float)VIRTUAL_WIDTH / (float)SCREEN_WIDTH, \
    GetMousePosition().y * (float)VIRTUAL_HEIGHT / (float)SCREEN_HEIGHT \
}

#define GetScreenWidth() (int){ \
    GetScreenWidth() * VIRTUAL_WIDTH / SCREEN_WIDTH \
}

#define GetScreenHeight() (int){ \
    GetScreenHeight() * VIRTUAL_WIDTH / SCREEN_WIDTH \
}

#include <iostream>
#include <fstream>

#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>

using namespace std;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

#define BETTER_RAYLIB_IMPLEMENTATION
#include "better_raylib.h"

struct Level {
    Vector2 ball_position;
    Vector2 hole_position;
    uint8_t shots;
    uint8_t **grid;
};

struct Levels {
    uint8_t grid_size;
    uint8_t grid_width;
    uint8_t grid_height;
    uint8_t count;
    Level *all;
};

Sprite ball;
Sprite hole;
Sprite bumper;

Levels levels;
//Level level;

int stage = 0; // 0 - selecting level; 1 - editing level

void DrawBumper(int grid_x, int grid_y)
{
    bumper.position = {(float)grid_x * levels.grid_size, (float)grid_y * levels.grid_size};
    bumper.Draw();
}

// Rotate right function
constexpr uint32_t ROTR(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

// SHA-256 constants
const uint32_t K[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// SHA-256 context
struct SHA256Context {
    uint32_t h[8];
    uint64_t bitlen;
    std::vector<uint8_t> data;
};

// Initialize SHA-256 context
void sha256Init(SHA256Context& ctx) {
    ctx.h[0] = 0x6a09e667;
    ctx.h[1] = 0xbb67ae85;
    ctx.h[2] = 0x3c6ef372;
    ctx.h[3] = 0xa54ff53a;
    ctx.h[4] = 0x510e527f;
    ctx.h[5] = 0x9b05688c;
    ctx.h[6] = 0x1f83d9ab;
    ctx.h[7] = 0x5be0cd19;
    ctx.bitlen = 0;
    ctx.data.clear();
}

// Process a 512-bit chunk
void sha256Transform(SHA256Context& ctx, const uint8_t* chunk) {
    uint32_t w[64];
    for (int i = 0; i < 16; i++) {
        w[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) | chunk[i * 4 + 3];
    }
    for (int i = 16; i < 64; i++) {
        uint32_t s0 = ROTR(w[i - 15], 7) ^ ROTR(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = ROTR(w[i - 2], 17) ^ ROTR(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = ctx.h[0];
    uint32_t b = ctx.h[1];
    uint32_t c = ctx.h[2];
    uint32_t d = ctx.h[3];
    uint32_t e = ctx.h[4];
    uint32_t f = ctx.h[5];
    uint32_t g = ctx.h[6];
    uint32_t h = ctx.h[7];

    for (int i = 0; i < 64; i++) {
        uint32_t S1 = ROTR(e, 6) ^ ROTR(e, 11) ^ ROTR(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t temp1 = h + S1 + ch + K[i] + w[i];
        uint32_t S0 = ROTR(a, 2) ^ ROTR(a, 13) ^ ROTR(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    ctx.h[0] += a;
    ctx.h[1] += b;
    ctx.h[2] += c;
    ctx.h[3] += d;
    ctx.h[4] += e;
    ctx.h[5] += f;
    ctx.h[6] += g;
    ctx.h[7] += h;
}

// Finalize and compute the hash
void sha256Final(SHA256Context& ctx, std::vector<uint8_t>& digest) {
    size_t dataSize = ctx.data.size();
    ctx.data.push_back(0x80);

    while ((ctx.data.size() + 8) % 64 != 0) {
        ctx.data.push_back(0x00);
    }

    uint64_t bitlen = ctx.bitlen + dataSize * 8;
    for (int i = 7; i >= 0; i--) {
        ctx.data.push_back((bitlen >> (i * 8)) & 0xFF);
    }

    for (size_t i = 0; i < ctx.data.size(); i += 64) {
        sha256Transform(ctx, ctx.data.data() + i);
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 3; j >= 0; j--) {
            digest.push_back((ctx.h[i] >> (j * 8)) & 0xFF);
        }
    }
}

// Compute the SHA-256 hash of a file
bool sha256File(const std::string& filename, std::vector<uint8_t>& digest) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file.\n";
        return false;
    }

    SHA256Context ctx;
    sha256Init(ctx);

    std::vector<uint8_t> buffer(4096);
    while (file.good()) {
        file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        std::streamsize bytesRead = file.gcount();
        ctx.data.insert(ctx.data.end(), buffer.begin(), buffer.begin() + bytesRead);
        ctx.bitlen += bytesRead * 8;
    }

    sha256Final(ctx, digest);
    return true;
}

// Convert digest to a hexadecimal string
std::string toHexString(const std::vector<uint8_t>& digest) {
    std::ostringstream oss;
    for (uint8_t byte : digest) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

bool IsFileEmpty(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos fileSize = file.tellg();
        file.close();
        return fileSize == 0;
    } else {
        std::cerr << "Unable to open file: " << fileName << std::endl;
        return false;
    }
}

uint8_t **CreateGrid(int width, int height) {
    uint8_t **grid = new uint8_t*[width];
    for (int i = 0; i < width; ++i) {
        grid[i] = new uint8_t[height]();
    }
    return grid;
}

void FreeGrid(uint8_t **grid, int width) {
    for (int i = 0; i < width; ++i) {
        delete[] grid[i];
    }
    delete[] grid;
}

Level *CreateLevels(int count, int grid_width, int grid_height)
{
    Level *levels = new Level[count];
    for (int i = 0; i < count; i++)
    {
        levels[i] = Level();
        levels[i].grid = CreateGrid(grid_width, grid_height);
    }
    return levels;
}

void FreeLevels(Level *levels, int count, int grid_width)
{
    for (int i = 0; i < count; i++)
    {
        FreeGrid(levels[i].grid, grid_width);
    }
    delete[] levels;
}

void Save(char *filename, Levels *levels)
{
    ofstream ofstream(filename, ios::out | ios::binary);
    ofstream.write((char*) &levels->grid_size, sizeof(uint8_t));
    ofstream.write((char*) &levels->grid_width, sizeof(uint8_t));
    ofstream.write((char*) &levels->grid_height, sizeof(uint8_t));
    ofstream.write((char*) &levels->count, sizeof(uint8_t));
    for (int i = 0; i < levels->count; i++)
    {
        ofstream.write((char*) &levels->all[i].ball_position, sizeof(Vector2));
        ofstream.write((char*) &levels->all[i].hole_position, sizeof(Vector2));
        ofstream.write((char*) &levels->all[i].shots, sizeof(uint8_t));
        for (int y = 0; y < levels->grid_height; y++)
        {
            for (int x = 0; x < levels->grid_width; x++)
            {
                ofstream.write((char*) &levels->all[i].grid[x][y], sizeof(uint8_t));
            }
        }
    }
    ofstream.close();

    std::vector<uint8_t> digest;
    if (sha256File(filename, digest)) {
        std::cout << "SHA-256 hash: " << toHexString(digest) << std::endl;
    } else {
        std::cerr << "Failed to compute hash.\n";
    }
}

void Load(char *filename, Levels *levels)
{
    if (IsFileEmpty(filename))
    {
        FreeLevels(levels->all, levels->count, levels->grid_width);
        levels->grid_width = 36;
        levels->grid_height = 20;
        levels->grid_size = 36;
        levels->count = 0;
        levels->all = new Level[0];
    }
    else
    {
        ifstream ifstream(filename, ios::in | ios::binary);
        if (!ifstream.is_open())
            return;
        ifstream.read((char*) &levels->grid_size, sizeof(uint8_t));
        ifstream.read((char*) &levels->grid_width, sizeof(uint8_t));
        ifstream.read((char*) &levels->grid_height, sizeof(uint8_t));
        if (levels->count > 0)
            FreeLevels(levels->all, levels->count, levels->grid_width);
        ifstream.read((char*) &levels->count, sizeof(uint8_t));
        levels->all = CreateLevels(levels->count, levels->grid_width, levels->grid_height);
        for (int i = 0; i < levels->count; i++)
        {
            ifstream.read((char*) &levels->all[i].ball_position, sizeof(Vector2));
            ifstream.read((char*) &levels->all[i].hole_position, sizeof(Vector2));
            ifstream.read((char*) &levels->all[i].shots, sizeof(uint8_t));
            for (int y = 0; y < levels->grid_height; y++)
            {
                for (int x = 0; x < levels->grid_width; x++)
                {
                    ifstream.read((char*) &levels->all[i].grid[x][y], sizeof(uint8_t));
                }
            }
        }
        ifstream.close();
    }
}

void LoadLevels(char *filename, Levels *levels)
{
    Load(filename, levels);
    ball.scale = (float)levels->grid_size / ball.GetSourceRect().width;
    hole.scale = (float)levels->grid_size / hole.GetSourceRect().width;
    bumper.scale = (float)levels->grid_size / bumper.GetSourceRect().width;
}

void AddLevel(Levels *levels)
{
    Levels new_levels = Levels();
    new_levels.grid_width = levels->grid_width;
    new_levels.grid_height = levels->grid_height;
    new_levels.grid_size = levels->grid_size;
    new_levels.count = levels->count + 1;

    new_levels.all = new Level[new_levels.count];
    for (int i = 0; i < levels->count; i++)
    {
        new_levels.all[i] = levels->all[i];
    }
    new_levels.all[new_levels.count - 1].grid = CreateGrid(new_levels.grid_width, new_levels.grid_height);
    new_levels.all[new_levels.count - 1].ball_position = {floor(SCREEN_WIDTH * (1.0f / 10.0f) / new_levels.grid_size + 0.5f) * new_levels.grid_size, floor(SCREEN_HEIGHT / 2 / new_levels.grid_size + 0.5f) * new_levels.grid_size};
    new_levels.all[new_levels.count - 1].hole_position = {floor(SCREEN_WIDTH * (9.0f / 10.0f) / new_levels.grid_size + 0.5f) * new_levels.grid_size, floor(SCREEN_HEIGHT / 2 / new_levels.grid_size + 0.5f) * new_levels.grid_size};
    new_levels.all[new_levels.count - 1].shots = 1;

    delete[] levels->all;
    levels->all = new_levels.all;
    levels->count = new_levels.count;
}

void RemoveLevel(Levels *levels, int level_num)
{
    int count = levels->count - 1;
    Level *new_levels = new Level[count];
    for (int i = 0; i < levels->count; i++)
    {
        if (i < level_num)
            new_levels[i] = levels->all[i];
        else if (i > level_num)
            new_levels[i - 1] = levels->all[i];
    }

    delete[] levels->all;
    levels->all = new_levels;
    levels->count = count;
}

int main()
{
    // Create Window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Golf! - Level Editor");
    // Set Max FPS
    SetTargetFPS(60);

    // Set the exit key to none
    SetExitKey(KEY_NULL); // default is ESC

    levels = Levels();
    int current_level = 0;
    Color grid_color = GRAY;
    int grid_paint_value = 1;

    ball = LoadSprite("resources/ball.png", {0.5, 0.5}, {0, 0}, 2);
    hole = LoadSprite("resources/hole.png", {0.5, 0.5}, {0, 0}, 2);
    bumper = LoadSprite("resources/bumper.png", {0, 0}, {0, 0}, 2);

    bool dragging_ball = false;
    bool dragging_hole = false;

    /* Stage 0 */
    Rectangle loadfile_rect = {5, 5, 59, 20};
    Rectangle save_rect = {5, 27, 59, 20};
    Rectangle scrollPanelRect = {loadfile_rect.width + 10, 5, (float)GetScreenWidth() - (loadfile_rect.width + 15), (float)GetScreenHeight() - 8};
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory(), 440, 310, "Open File");
    char filename[512] = "";
    char *fileload_errmsg = "";
    //LoadLevels(filename, &levels);
    int scrollPanelHeight = 0;
    int ls_level_width = 36 * 5;
    int ls_level_height = 20 * 5;
    int ls_levels_per_line = 3;

    /* Stage 1 */
    int bar_x = 0;
    Rectangle back_rect = {10, 10, 90, 40};
    bar_x += back_rect.width + 10;
    Rectangle shots_rect = {bar_x + 10.0f, 10, (float)MeasureText("Shots:", 30) + 125, 40};
    Rectangle shots_less = {bar_x + (float)MeasureText("Shots:", 30) + 20, 15, 30, 30};
    Rectangle shots_num = {bar_x + (float)MeasureText("Shots:", 30) + 55, 15, 40, 30};
    Rectangle shots_more = {bar_x + (float)MeasureText("Shots:", 30) + 100, 15, 30, 30};
    bar_x += shots_rect.width + 10;
    Rectangle paint_options_rect = {bar_x + 10.0f, 10, 65 + (float)MeasureText("Paint:", 30) + (float)MeasureText("Clear", 30) + (float)MeasureText("Block", 30) + (float)MeasureText("Bumper", 30), 40};
    bar_x += paint_options_rect.width + 10;

    const int ui_rects_count = 4;
    Rectangle ui_rects[ui_rects_count] = {back_rect, shots_rect, paint_options_rect};

    RenderTexture2D frame = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

    Rectangle scrollPanelView;
    Vector2 scrollPanelScroll = {0, 0};

    // Main Loop
    while (!WindowShouldClose())
    {
        if (stage == 0 && windowScale != 2)
        {
            windowScale = 2;
            UnloadRenderTexture(frame);
            frame = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
        }
        else if (stage == 1 && windowScale != 1)
        {
            windowScale = 1;
            UnloadRenderTexture(frame);
            frame = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
        }

        BeginTextureMode(frame);
        ClearBackground(DARKGREEN);
        {
            if (stage == 0)
            {
                ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

                // Left panel buttons
                if (GuiButton(loadfile_rect, GuiIconText(ICON_FILE_OPEN, "Load")))
                {
                    fileDialogState.windowActive = true;
                    fileDialogState.windowBounds.x = (GetScreenWidth() - fileDialogState.windowBounds.width) / 2;
                    fileDialogState.windowBounds.y = (GetScreenHeight() - fileDialogState.windowBounds.height) / 2;
                }
                if (GuiButton(save_rect, GuiIconText(ICON_FILE_SAVE, "Save")))
                {
                    Save(filename, &levels);
                }

                // File dialog
                if (fileDialogState.SelectFilePressed)
                {
                    if (IsFileExtension(fileDialogState.fileNameText, ".dat"))
                    {
                        strcpy(filename, TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                        fileload_errmsg = "";
                        LoadLevels(filename, &levels);
                        fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory(), 440, 310, "Open File"); // Reset window
                    }
                    else
                    {
                        fileload_errmsg = (char*)"Please select a .dat file";
                        FreeLevels(levels.all, levels.count, levels.grid_width);
                        levels.count = 0;
                        levels.grid_width = 0;
                        levels.grid_height = 0;
                        levels.grid_size = 0;
                        strcpy(filename, "");
                    }

                    fileDialogState.SelectFilePressed = false;
                }
                if (fileDialogState.windowActive)
                    GuiLock();

                // Levels + scroll panel
                bool sliderDragging = guiControlExclusiveMode;
                int contentHeight = ceil((levels.count + 3) / 3) * (ls_level_height + 6) + 3;
                scrollPanelHeight = max(contentHeight, (int)scrollPanelRect.height - 27);
                GuiScrollPanel(scrollPanelRect, filename, {loadfile_rect.width + 10, 5, scrollPanelRect.width - 15, (float)scrollPanelHeight}, &scrollPanelScroll, &scrollPanelView);
                BeginScissorMode(scrollPanelView.x + 1, scrollPanelView.y + 1, scrollPanelView.width - 2, scrollPanelView.height - 2); // Don't draw outside scroll area
                {
                    // Draw error when loading file (if any)
                    DrawText(fileload_errmsg, scrollPanelView.x + (scrollPanelView.width - 2) / 2, scrollPanelView.y + (scrollPanelView.height - 2) / 2 - 10, 20, 0.5, RED);

                    bool no_add = false;
                    for (int i = 0; i < levels.count; i++)
                    {
                        Rectangle lvl_rect = {scrollPanelView.x + scrollPanelScroll.x + 4 + (int)floor(i % ls_levels_per_line) * (ls_level_width + 6), scrollPanelView.y + scrollPanelScroll.y + 4 + (int)floor(i / ls_levels_per_line) * (ls_level_height + 6), (float)ls_level_width, (float)ls_level_height};
                        bool is_hovered = !guiLocked && CheckCollisionPointRec(GetMousePosition(), lvl_rect) && CheckCollisionPointRec(GetMousePosition(), scrollPanelView);

                        // Draw level
                        DrawRectangle(lvl_rect.x, lvl_rect.y, lvl_rect.width, lvl_rect.height, DARKGREEN);
                        int grid_size = lvl_rect.width / levels.grid_width;
                        for (int y = 0; y < levels.grid_height; y++)
                        {
                            for (int x = 0; x < levels.grid_width; x++)
                            {
                                if (levels.all[i].grid[x][y] == 1)
                                    DrawRectangle(lvl_rect.x + x * grid_size, lvl_rect.y + y * grid_size, grid_size, grid_size, DARKGRAY);
                                else if (levels.all[i].grid[x][y] == 2)
                                    DrawRectangle(lvl_rect.x + x * grid_size, lvl_rect.y + y * grid_size, grid_size, grid_size, {0, 255, 0, 255});
                            }
                        }
                        Vector2 ball_pos = {lvl_rect.x + (levels.all[i].ball_position.x / (levels.grid_width * levels.grid_size)) * lvl_rect.width, lvl_rect.y + (levels.all[i].ball_position.y / (levels.grid_height * levels.grid_size)) * lvl_rect.height};
                        DrawCircle(ball_pos.x, ball_pos.y, ceil(grid_size / 2), WHITE);
                        Vector2 hole_pos = {lvl_rect.x + (levels.all[i].hole_position.x / (levels.grid_width * levels.grid_size)) * lvl_rect.width, lvl_rect.y + (levels.all[i].hole_position.y / (levels.grid_height * levels.grid_size)) * lvl_rect.height};
                        DrawCircle(hole_pos.x, hole_pos.y, ceil(grid_size / 2), BLACK);

                        // Draw buttons
                        int size = 13;
                        Rectangle dlt_btn = {lvl_rect.x + lvl_rect.width - size - 2, lvl_rect.y + 2, (float)size, (float)size};
                        int width = size * 2 + 15;
                        Rectangle mv_rect = {dlt_btn.x - width - 2, dlt_btn.y, (float)width, (float)size};
                        bool hover_buttons = CheckCollisionPointRec(GetMousePosition(), mv_rect) || CheckCollisionPointRec(GetMousePosition(), dlt_btn);

                        if (is_hovered && !sliderDragging)
                        {
                            if (Button(dlt_btn, "x", 10, {165, 50, 50, 255}, RED, RED))
                            {
                                RemoveLevel(&levels, i);
                                no_add = true;
                            }
                            if (CheckCollisionPointRec(GetMousePosition(), dlt_btn))
                                DrawRectangleLinesEx(dlt_btn, 2, {130, 0, 0, 255});
                            else
                                DrawRectangleLinesEx(dlt_btn, 2, {110, 50, 50, 255});
                            DrawRectangle(mv_rect.x, mv_rect.y, mv_rect.width, mv_rect.height, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
                            if (GuiButton({mv_rect.x, mv_rect.y, 13, 13}, "+") && i < levels.count - 1)
                            {
                                Vector2 ball = levels.all[i].ball_position;
                                Vector2 hole = levels.all[i].hole_position;
                                int shots = levels.all[i].shots;
                                uint8_t **grid = levels.all[i].grid;

                                levels.all[i].ball_position = levels.all[i + 1].ball_position;
                                levels.all[i].hole_position = levels.all[i + 1].hole_position;
                                levels.all[i].shots = levels.all[i + 1].shots;
                                levels.all[i].grid = levels.all[i + 1].grid;

                                levels.all[i + 1].ball_position = ball;
                                levels.all[i + 1].hole_position = hole;
                                levels.all[i + 1].shots = shots;
                                levels.all[i + 1].grid = grid;
                            }
                            if (GuiButton({mv_rect.x + mv_rect.width - 13, mv_rect.y, 13, 13}, "-") && i > 0)
                            {
                                Vector2 ball = levels.all[i].ball_position;
                                Vector2 hole = levels.all[i].hole_position;
                                int shots = levels.all[i].shots;
                                uint8_t **grid = levels.all[i].grid;

                                levels.all[i].ball_position = levels.all[i - 1].ball_position;
                                levels.all[i].hole_position = levels.all[i - 1].hole_position;
                                levels.all[i].shots = levels.all[i - 1].shots;
                                levels.all[i].grid = levels.all[i - 1].grid;

                                levels.all[i - 1].ball_position = ball;
                                levels.all[i - 1].hole_position = hole;
                                levels.all[i - 1].shots = shots;
                                levels.all[i - 1].grid = grid;
                            }
                            char text[25];
                            sprintf(text, "%d", i + 1);
                            DrawText(text, mv_rect.x + mv_rect.width / 2, mv_rect.y + 2, 10, 0.5, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
                            
                            if (!hover_buttons && IsMouseButtonReleased(0) && !CheckCollisionPointRec(GetMousePosition(), mv_rect))
                            {
                                stage = 1;
                                current_level = i;
                                ball.position = levels.all[i].ball_position;
                                hole.position = levels.all[i].hole_position;
                            }
                        }
                        
                        // Draw outline around level
                        Color outline_color = is_hovered && !sliderDragging ? ((IsMouseButtonDown(0) && !hover_buttons) ? GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_PRESSED))
                                                                                                                        : GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED)))
                                                                            : GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
                        DrawRectangleLinesEx({lvl_rect.x - 2, lvl_rect.y -2, lvl_rect.width + 4, lvl_rect.height + 4}, 2, outline_color);
                    }
                    // Draw add level button
                    Rectangle add_rect = {scrollPanelView.x + scrollPanelScroll.x + 2 + (int)floor(levels.count % ls_levels_per_line) * (ls_level_width + 6), scrollPanelView.y + scrollPanelScroll.y + 2 + (int)floor(levels.count / ls_levels_per_line) * (ls_level_height + 6), (float)ls_level_width + 4, (float)ls_level_height + 4};
                    if (filename[0] != '\0' && GuiButton(add_rect, "+") && !no_add && !sliderDragging)
                    {
                        AddLevel(&levels);
                    }
                }
                EndScissorMode();

                GuiUnlock();
                GuiWindowFileDialog(&fileDialogState);
            }
            else if (stage == 1)
            {
                Vector2 mouse_pos = GetMousePosition();
                bool is_mouse_on_ui = false;
                for (int i = 0; i < ui_rects_count; i++)
                    if (CheckCollisionPointRec(mouse_pos, ui_rects[i]))
                        is_mouse_on_ui = true;

                if (IsMouseButtonPressed(0) && ball.GetCollisionPoint(mouse_pos))
                    dragging_ball = true;
                else if (IsMouseButtonReleased(0))
                    dragging_ball = false;
                if (dragging_ball)
                {
                    ball.position.x = floor(mouse_pos.x / levels.grid_size + 0.5f) * levels.grid_size;
                    ball.position.y = floor(mouse_pos.y / levels.grid_size + 0.5f) * levels.grid_size;
                }

                if (IsMouseButtonPressed(0) && hole.GetCollisionPoint(mouse_pos))
                    dragging_hole = true;
                else if (IsMouseButtonReleased(0))
                    dragging_hole = false;
                if (dragging_hole)
                {
                    hole.position.x = floor(mouse_pos.x / levels.grid_size + 0.5f) * levels.grid_size;
                    hole.position.y = floor(mouse_pos.y / levels.grid_size + 0.5f) * levels.grid_size;
                }

                Vector2 ball_grid_pos = {floor(ball.position.x / levels.grid_size), floor(ball.position.y / levels.grid_size)};
                Vector2 hole_grid_pos = {floor(hole.position.x / levels.grid_size), floor(hole.position.y / levels.grid_size)};

                for (int y = 0; y < levels.grid_height; y++)
                {
                    for (int x = 0; x < levels.grid_width; x++)
                    {
                        bool coord_is_in_ball = (ball_grid_pos.x == x || ball_grid_pos.x - 1 == x) && (ball_grid_pos.y == y || ball_grid_pos.y - 1 == y);
                        bool coord_is_in_hole = (hole_grid_pos.x == x || hole_grid_pos.x - 1 == x) && (hole_grid_pos.y == y || hole_grid_pos.y - 1 == y);
                        if (!coord_is_in_ball && !coord_is_in_hole)
                        {
                            if (levels.all[current_level].grid[x][y] == 1)
                                DrawRectangle(x * levels.grid_size, y * levels.grid_size, levels.grid_size, levels.grid_size, DARKGRAY);
                            else if (levels.all[current_level].grid[x][y] == 2)
                                DrawBumper(x, y);
                        }
                    }
                }

                if (!is_mouse_on_ui && !dragging_ball && !dragging_hole && mouse_pos.x >= 0 && mouse_pos.x < SCREEN_WIDTH && mouse_pos.y >= 0 && mouse_pos.y < SCREEN_HEIGHT)
                {
                    int x = floor(mouse_pos.x / levels.grid_size);
                    int y = floor(mouse_pos.y / levels.grid_size);
                    bool coord_is_in_ball = (ball_grid_pos.x == x || ball_grid_pos.x - 1 == x) && (ball_grid_pos.y == y || ball_grid_pos.y - 1 == y);
                    bool coord_is_in_hole = (hole_grid_pos.x == x || hole_grid_pos.x - 1 == x) && (hole_grid_pos.y == y || hole_grid_pos.y - 1 == y);
                    if (!coord_is_in_ball && !coord_is_in_hole)
                    {
                        if (IsMouseButtonDown(0))
                            levels.all[current_level].grid[x][y] = grid_paint_value;
                        if (grid_paint_value == 0)
                            DrawRectangle(x * levels.grid_size, y * levels.grid_size, levels.grid_size, levels.grid_size, DARKGREEN);
                        else if (grid_paint_value == 1)
                            DrawRectangle(x * levels.grid_size, y * levels.grid_size, levels.grid_size, levels.grid_size, DARKGRAY);
                        else if (grid_paint_value == 2)
                            DrawBumper(x, y);
                    }
                }

                for (int y = 0; y < SCREEN_HEIGHT; y += levels.grid_size)
                {
                    DrawLine(0, y, SCREEN_WIDTH, y, grid_color);
                }
                for(int x = 0; x < SCREEN_WIDTH; x += levels.grid_size)
                {
                    DrawLine(x, 0, x, SCREEN_HEIGHT, grid_color);
                }

                hole.Draw();
                ball.Draw();

                if (Button(back_rect, "Back", 30, GRAY, BLUE, RED))
                {
                   stage = 0;
                }
                DrawRectangle(shots_rect.x, shots_rect.y, shots_rect.width, shots_rect.height, DARKGRAY);
                DrawText("Shots:", shots_rect.x + 5, shots_rect.y + 7, 30, RAYWHITE);
                if (Button(shots_less, "-", 30, GRAY, BLUE, DARKBLUE))
                {
                    levels.all[current_level].shots--;
                }
                DrawRectangle(shots_num.x, shots_num.y, shots_num.width, shots_num.height, GRAY);
                char shots_text[25];
                sprintf(shots_text, "%d", levels.all[current_level].shots);
                DrawText(shots_text, shots_num.x + shots_num.width / 2, shots_num.y + 1, 30, 0.5f, RAYWHITE);
                if (Button(shots_more, "+", 30, GRAY, BLUE, DARKBLUE))
                {
                    levels.all[current_level].shots++;
                }

                DrawRectangle(paint_options_rect.x, paint_options_rect.y, paint_options_rect.width, paint_options_rect.height, DARKGRAY);
                DrawText("Paint:", paint_options_rect.x + 10, paint_options_rect.y + 7, 30, RAYWHITE);
                int text_width = MeasureText("Paint:", 30);
                if (Button({paint_options_rect.x + text_width + 20, paint_options_rect.y + 5, (float)MeasureText("Clear", 30) + 10, 30}, "Clear", 30, GRAY, BLUE, RED))
                {
                    grid_paint_value = 0;
                }
                else if (Button({paint_options_rect.x + text_width + (float)MeasureText("Clear", 30) + 35, paint_options_rect.y + 5, (float)MeasureText("Block", 30) + 10, 30}, "Block", 30, GRAY, BLUE, RED))
                {
                    grid_paint_value = 1;
                }
                else if (Button({paint_options_rect.x + text_width + (float)MeasureText("Clear", 30) + (float)MeasureText("Block", 30) + 50, paint_options_rect.y + 5, (float)MeasureText("Bumper", 30) + 10, 30}, "Bumper", 30, GRAY, BLUE, RED))
                {
                    grid_paint_value = 2;
                }

                levels.all[current_level].ball_position = ball.position;
                levels.all[current_level].hole_position = hole.position;
            }
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        {
            DrawTexturePro(
                frame.texture, 
                (Rectangle){0, 0, (float)frame.texture.width, (float)-frame.texture.height},
                (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                (Vector2){0, 0}, 0, WHITE
            );
        }
        EndDrawing();
    }

    UnloadRenderTexture(frame);
    ball.Delete();
    hole.Delete();
    CloseWindow();
    return 0;
}