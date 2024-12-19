#include <iostream>
#include <fstream>

#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>

#include "Sprite.h"
#include "Shapes.h"
#include "Helpers.h"
#include "raymath.h"
#include "raylib.h"

using namespace std;

// Define screen width and height
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct Level {
    Vector2 ball_position;
    Vector2 hole_position;
    unsigned __int8 shots;
    unsigned __int8 grid_size;
    unsigned __int8 grid_width;
    unsigned __int8 grid_height;
    unsigned __int8 **grid;
};

Sprite ball;
Sprite hole;
Sprite bumper;

Level level;

void DrawBumper(int grid_x, int grid_y)
{
    bumper.position = {(float)grid_x * level.grid_size, (float)grid_y * level.grid_size};
    bumper.Draw();
}

#pragma region FILE_HASH
// Rotate right function
constexpr unsigned __int32 ROTR(unsigned __int32 x, unsigned __int32 n) {
    return (x >> n) | (x << (32 - n));
}

// SHA-256 constants
const unsigned __int32 K[] = {
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
    unsigned __int32 h[8];
    unsigned __int64 bitlen;
    std::vector<unsigned __int8> data;
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
void sha256Transform(SHA256Context& ctx, const unsigned __int8* chunk) {
    unsigned __int32 w[64];
    for (int i = 0; i < 16; i++) {
        w[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) | chunk[i * 4 + 3];
    }
    for (int i = 16; i < 64; i++) {
        unsigned __int32 s0 = ROTR(w[i - 15], 7) ^ ROTR(w[i - 15], 18) ^ (w[i - 15] >> 3);
        unsigned __int32 s1 = ROTR(w[i - 2], 17) ^ ROTR(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    unsigned __int32 a = ctx.h[0];
    unsigned __int32 b = ctx.h[1];
    unsigned __int32 c = ctx.h[2];
    unsigned __int32 d = ctx.h[3];
    unsigned __int32 e = ctx.h[4];
    unsigned __int32 f = ctx.h[5];
    unsigned __int32 g = ctx.h[6];
    unsigned __int32 h = ctx.h[7];

    for (int i = 0; i < 64; i++) {
        unsigned __int32 S1 = ROTR(e, 6) ^ ROTR(e, 11) ^ ROTR(e, 25);
        unsigned __int32 ch = (e & f) ^ (~e & g);
        unsigned __int32 temp1 = h + S1 + ch + K[i] + w[i];
        unsigned __int32 S0 = ROTR(a, 2) ^ ROTR(a, 13) ^ ROTR(a, 22);
        unsigned __int32 maj = (a & b) ^ (a & c) ^ (b & c);
        unsigned __int32 temp2 = S0 + maj;

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
void sha256Final(SHA256Context& ctx, std::vector<unsigned __int8>& digest) {
    size_t dataSize = ctx.data.size();
    ctx.data.push_back(0x80);

    while ((ctx.data.size() + 8) % 64 != 0) {
        ctx.data.push_back(0x00);
    }

    unsigned __int64 bitlen = ctx.bitlen + dataSize * 8;
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
bool sha256File(const std::string& filename, std::vector<unsigned __int8>& digest) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file.\n";
        return false;
    }

    SHA256Context ctx;
    sha256Init(ctx);

    std::vector<unsigned __int8> buffer(4096);
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
std::string toHexString(const std::vector<unsigned __int8>& digest) {
    std::ostringstream oss;
    for (unsigned __int8 byte : digest) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}
#pragma endregion
#pragma region LOAD-SAVE SYSTEM
unsigned __int8 **CreateGrid(int width, int height) {
    unsigned __int8 **grid = new unsigned __int8*[width];
    for (int i = 0; i < width; ++i) {
        grid[i] = new unsigned __int8[height];
    }
    return grid;
}

void FreeGrid(unsigned __int8 **grid, int width) {
    for (int i = 0; i < width; ++i) {
        delete[] grid[i];
    }
    delete[] grid;
}

void Save(char *filename, Level *level)
{
    ofstream ofstream(filename, ios::out | ios::binary);
    ofstream.write((char*) &level->ball_position, sizeof(Vector2));
    ofstream.write((char*) &level->hole_position, sizeof(Vector2));
    ofstream.write((char*) &level->shots, sizeof(unsigned __int8));
    ofstream.write((char*) &level->grid_size, sizeof(unsigned __int8));
    ofstream.write((char*) &level->grid_width, sizeof(unsigned __int8));
    ofstream.write((char*) &level->grid_height, sizeof(unsigned __int8));
    for (int y = 0; y < level->grid_height; y++)
    {
        for (int x = 0; x < level->grid_width; x++)
        {
            ofstream.write((char*) &level->grid[x][y], sizeof(unsigned __int8));
        }
    }
    ofstream.close();
}

void Load(char *filename, Level *level)
{
    ifstream ifstream(filename, ios::in | ios::binary);
    ifstream.read((char*) &level->ball_position, sizeof(Vector2));
    ifstream.read((char*) &level->hole_position, sizeof(Vector2));
    ifstream.read((char*) &level->shots, sizeof(unsigned __int8));
    ifstream.read((char*) &level->grid_size, sizeof(unsigned __int8));
    ifstream.read((char*) &level->grid_width, sizeof(unsigned __int8));
    ifstream.read((char*) &level->grid_height, sizeof(unsigned __int8));
    for (int y = 0; y < level->grid_height; y++)
    {
        for (int x = 0; x < level->grid_width; x++)
        {
            ifstream.read((char*) &level->grid[x][y], sizeof(unsigned __int8));
        }
    }
    ifstream.close();
}
#pragma endregion

int main()
{
    // Create Window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Golf! - Level Editor");
    // Set Max FPS
    SetTargetFPS(60);

    // Set the exit key to none
    SetExitKey(KEY_NULL); // default is ESC

    level = Level();
    level.grid_size = 36;
    level.grid_width = (int)ceil(SCREEN_WIDTH / level.grid_size) + 1;
    level.grid_height = (int)ceil(SCREEN_HEIGHT / level.grid_size) + 1;
    level.grid = CreateGrid(level.grid_width, level.grid_height);
    for (int y = 0; y < level.grid_height; y++)
        for (int x = 0; x < level.grid_width; x++)
            level.grid[x][y] = 0;
    if (level.shots < 1)
        level.shots == 1;
    Color grid_color = GRAY;
    int grid_paint_value = 1;

    ball = LoadSprite("resources/ball.png", {0.5, 0.5}, {floor(SCREEN_WIDTH * (1.0f / 10.0f) / level.grid_size + 0.5f) * level.grid_size, floor(SCREEN_HEIGHT / 2 / level.grid_size + 0.5f) * level.grid_size}, (float)level.grid_size / 128);
    hole = LoadSprite("resources/hole.png", {0.5, 0.5}, {floor(SCREEN_WIDTH * (9.0f / 10.0f) / level.grid_size + 0.5f) * level.grid_size, floor(SCREEN_HEIGHT / 2 / level.grid_size + 0.5f) * level.grid_size}, (float)level.grid_size / 128);
    bumper = LoadSprite("resources/bumper.png", {0, 0}, {0, 0}, (float)level.grid_size / 128);

    bool dragging_ball = false;
    bool dragging_hole = false;

    int bar_x = 0;
    Rectangle save_rect = {10, 10, 90, 40};
    bar_x += save_rect.width + 10;
    Rectangle load_rect = {bar_x + 10.0f, 10, 90, 40};
    bar_x += load_rect.width + 10;
    Rectangle shots_rect = {bar_x + 10.0f, 10, (float)MeasureText("Shots:", 30) + 125, 40};
    Rectangle shots_less = {bar_x + (float)MeasureText("Shots:", 30) + 20, 15, 30, 30};
    Rectangle shots_num = {bar_x + (float)MeasureText("Shots:", 30) + 55, 15, 40, 30};
    Rectangle shots_more = {bar_x + (float)MeasureText("Shots:", 30) + 100, 15, 30, 30};
    bar_x += shots_rect.width + 10;
    Rectangle paint_options_rect = {bar_x + 10.0f, 10, 65 + (float)MeasureText("Paint:", 30) + (float)MeasureText("Clear", 30) + (float)MeasureText("Block", 30) + (float)MeasureText("Bumper", 30), 40};
    bar_x += paint_options_rect.width + 10;

    const int ui_rects_count = 4;
    Rectangle ui_rects[ui_rects_count] = {save_rect, load_rect, shots_rect, paint_options_rect};

    // Main Loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGREEN);
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
                ball.position.x = floor(mouse_pos.x / level.grid_size + 0.5f) * level.grid_size;
                ball.position.y = floor(mouse_pos.y / level.grid_size + 0.5f) * level.grid_size;
            }

            if (IsMouseButtonPressed(0) && hole.GetCollisionPoint(mouse_pos))
                dragging_hole = true;
            else if (IsMouseButtonReleased(0))
                dragging_hole = false;
            if (dragging_hole)
            {
                hole.position.x = floor(mouse_pos.x / level.grid_size + 0.5f) * level.grid_size;
                hole.position.y = floor(mouse_pos.y / level.grid_size + 0.5f) * level.grid_size;
            }

            Vector2 ball_grid_pos = {floor(ball.position.x / level.grid_size), floor(ball.position.y / level.grid_size)};
            Vector2 hole_grid_pos = {floor(hole.position.x / level.grid_size), floor(hole.position.y / level.grid_size)};

            for (int y = 0; y < level.grid_height; y++)
            {
                for (int x = 0; x < level.grid_width; x++)
                {
                    bool coord_is_in_ball = (ball_grid_pos.x == x || ball_grid_pos.x - 1 == x) && (ball_grid_pos.y == y || ball_grid_pos.y - 1 == y);
                    bool coord_is_in_hole = (hole_grid_pos.x == x || hole_grid_pos.x - 1 == x) && (hole_grid_pos.y == y || hole_grid_pos.y - 1 == y);
                    if (!coord_is_in_ball && !coord_is_in_hole)
                    {
                        if (level.grid[x][y] == 1)
                            DrawRectangle(x * level.grid_size, y * level.grid_size, level.grid_size, level.grid_size, DARKGRAY);
                        else if (level.grid[x][y] == 2)
                            DrawBumper(x, y);
                    }
                }
            }

            if (!is_mouse_on_ui && !dragging_ball && !dragging_hole && mouse_pos.x >= 0 && mouse_pos.x < SCREEN_WIDTH && mouse_pos.y >= 0 && mouse_pos.y < SCREEN_HEIGHT)
            {
                int x = floor(mouse_pos.x / level.grid_size);
                int y = floor(mouse_pos.y / level.grid_size);
                bool coord_is_in_ball = (ball_grid_pos.x == x || ball_grid_pos.x - 1 == x) && (ball_grid_pos.y == y || ball_grid_pos.y - 1 == y);
                bool coord_is_in_hole = (hole_grid_pos.x == x || hole_grid_pos.x - 1 == x) && (hole_grid_pos.y == y || hole_grid_pos.y - 1 == y);
                if (!coord_is_in_ball && !coord_is_in_hole)
                {
                    if (IsMouseButtonDown(0))
                        level.grid[x][y] = grid_paint_value;
                    if (grid_paint_value == 0)
                        DrawRectangle(x * level.grid_size, y * level.grid_size, level.grid_size, level.grid_size, DARKGREEN);
                    else if (grid_paint_value == 1)
                        DrawRectangle(x * level.grid_size, y * level.grid_size, level.grid_size, level.grid_size, DARKGRAY);
                    else if (grid_paint_value == 2)
                        DrawBumper(x, y);
                }
            }

            for (int y = 0; y < SCREEN_HEIGHT; y += level.grid_size)
            {
                DrawLine(0, y, SCREEN_WIDTH, y, grid_color);
            }
            for(int x = 0; x < SCREEN_WIDTH; x += level.grid_size)
            {
                DrawLine(x, 0, x, SCREEN_HEIGHT, grid_color);
            }

            hole.Draw();
            ball.Draw();

            if (Button(save_rect, "Save", 30, GRAY, BLUE, RED))
            {
                Save((char*)"level.dat", &level);

                std::string filename = "level.dat";
                std::vector<unsigned __int8> digest;

                if (sha256File(filename, digest)) {
                    std::cout << "SHA-256 hash: " << toHexString(digest) << std::endl;
                } else {
                    std::cerr << "Failed to compute hash.\n";
                }
            }

            if (Button(load_rect, "Load", 30, GRAY, BLUE, RED))
            {
                Load((char*)"level.dat", &level);
                ball.position = level.ball_position;
                hole.position = level.hole_position;
            }

            DrawRectangle(shots_rect.x, shots_rect.y, shots_rect.width, shots_rect.height, DARKGRAY);
            DrawText("Shots:", shots_rect.x + 5, shots_rect.y + 7, 30, RAYWHITE);
            if (Button(shots_less, "-", 30, GRAY, BLUE, DARKBLUE))
            {
                level.shots--;
            }
            DrawRectangle(shots_num.x, shots_num.y, shots_num.width, shots_num.height, GRAY);
            char shots_text[25];
            sprintf(shots_text, "%d", level.shots);
            DrawText(shots_text, shots_num.x + shots_num.width / 2, shots_num.y + 1, 30, 0.5f, RAYWHITE);
            if (Button(shots_more, "+", 30, GRAY, BLUE, DARKBLUE))
            {
                level.shots++;
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

            level.ball_position = ball.position;
            level.hole_position = hole.position;
        }
        EndDrawing();
    }

    FreeGrid(level.grid, level.grid_width);
    ball.Delete();
    hole.Delete();
    CloseWindow();
    return 0;
}