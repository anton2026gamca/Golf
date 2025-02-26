#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>

#define BETTER_RAYLIB_IMPLEMENTATION
#include "better_raylib.h"

using namespace std;

// Define screen width and height
int screenWidth = 1296;
int screenHeight = 720;

#define AIM_MAX_DISTANCE 300
#define AIM_RECT_WIDTH 15
#define AIM_RECT_NORMAL_COLOR ORANGE
#define AIM_RECT_MAX_COLOR RED
Color aim_rect_color = AIM_RECT_NORMAL_COLOR;
float aim_distance = 0;

#define BALL_BOUNCIENESS 1.03
#define BALL_FRICTION 1.03
#define BALL_FORCE_MULTIPLYER 10
#define BUMPER_BOUNCE_VELOCITY 300 * BALL_FORCE_MULTIPLYER

Sprite ball;
Vector2 ball_velocity = {0, 0};
Sprite hole;

const char *player_data_filename = "resources/p.dat";

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

struct Bumper
{
    int grid_x;
    int grid_y;
    double remaining_seconds;
};
Levels levels;
int num_rects = -1;
Rectangle *rects;
int num_bumpers = -1;
Bumper *bumpers = nullptr;
Sprite bumper_up;
Sprite bumper_down;
int shots_left = 0;

int level_num = 0;
uint8_t highest_level = 0;
const std::string levels_hash = "6f62bd6174767352643662a98068095a0ac7d6698e5d89cbdc239f74c6ae3b7a";
int lvlslc_page = 0;
int game_stage = 0; // 0 = Main Menu, 1 = Playing, 2 = Pause menu; 3 = All Levels Completed; 4 = Level Selection

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

uint8_t **CreateGrid(int width, int height) {
    uint8_t **grid = new uint8_t*[width];
    for (int i = 0; i < width; ++i) {
        grid[i] = new uint8_t[height];
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

int LoadLevels(const char *filename, Levels *levels)
{
    ifstream ifstream(filename, ios::in | ios::binary);
    if (!ifstream.is_open())
        return 1;
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

    return 0;
}

void FreeLevel()
{
    if (num_rects >= 0)
    {
        delete[] rects;
        rects = nullptr;
    }
    if (num_bumpers >= 0)
    {
        delete[] bumpers;
        bumpers = nullptr;
    }
}

int LoadLevel(int level_num)
{
    FreeLevel();
    num_rects = 0;
    for (int y = 0; y < levels.grid_height; y++)
        for (int x = 0; x < levels.grid_width; x++)
            if (levels.all[level_num].grid[x][y] == 1)
                num_rects++;
    rects = new Rectangle[num_rects];
    int i = 0;
    for (int y = 0; y < levels.grid_height; y++)
        for (int x = 0; x < levels.grid_width; x++)
            if (levels.all[level_num].grid[x][y] == 1)
                rects[i++] = {(float)x * levels.grid_size, (float)y * levels.grid_size, (float)levels.grid_size, (float)levels.grid_size};

    num_bumpers = 0;
    for (int y = 0; y < levels.grid_height; y++)
        for (int x = 0; x < levels.grid_width; x++)
            if (levels.all[level_num].grid[x][y] == 2)
                num_bumpers++;
    bumpers = new Bumper[num_bumpers]();
    i = 0;
    for (int y = 0; y < levels.grid_height; y++)
    {
        for (int x = 0; x < levels.grid_width; x++)
        {
            if (levels.all[level_num].grid[x][y] == 2)
            {
                bumpers[i].grid_x = x;
                bumpers[i].grid_y = y;
                bumpers[i].remaining_seconds = 0;
                i++;
            }
        }
    }
    ball.position = levels.all[level_num].ball_position;
    hole.position = levels.all[level_num].hole_position;
    shots_left = (int)levels.all[level_num].shots;
    ball_velocity = {0, 0};
    return 0;
}

void DrawLevel()
{
    hole.Draw();
    ball.Draw();
    for (int i = 0; i < num_rects; i++)
        DrawRectanglePro(rects[i], {0, 0}, 0, DARKGRAY);
    
    for (int i = 0; i < num_bumpers; i++)
    {
        if (bumpers[i].remaining_seconds <= 0)
        {
            bumper_up.position = {(float)bumpers[i].grid_x * levels.grid_size, (float)bumpers[i].grid_y * levels.grid_size};
            bumper_up.Draw();
        }
        else
        {
            bumper_down.position = {(float)bumpers[i].grid_x * levels.grid_size, (float)bumpers[i].grid_y * levels.grid_size};
            bumper_down.Draw();
            bumpers[i].remaining_seconds -= GetFrameTime();
        }
    }
}

void SavePlayerData()
{
    ofstream ofstream(player_data_filename, ios::out | ios::binary);
    ofstream.write((char*) &highest_level, sizeof(uint8_t));
    ofstream.close();
}

int LoadPlayerData()
{
    ifstream ifstream(player_data_filename, ios::in | ios::binary);
    if (!ifstream.is_open())
    {
        SavePlayerData();
        return 1;
    }
    ifstream.read((char*) &highest_level, sizeof(uint8_t));
    ifstream.close();

    return 0;
}

int main()
{
    bool exit = false;

    // Create Window
    InitWindow(screenWidth, screenHeight, "Golf!");
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    // Set Max FPS
    SetTargetFPS(60);

    // Set the exit key to none
    SetExitKey(KEY_NULL); // default is ESC

    ball = LoadSprite("resources/ball.png", {0.5, 0.5}, {(float)screenWidth / 2, (float)screenHeight / 2}, 1.0f / 4.0f);
    hole = LoadSprite("resources/hole.png", {0.5, 0.5}, {(float)screenWidth / 2, (float)screenHeight / 2}, 1.0f / 4.0f);
    bumper_up = LoadSprite("resources/bumper_up.png", {0, 0}, {0, 0}, 1);
    bumper_down = LoadSprite("resources/bumper_down.png", {0, 0}, {0, 0}, 1);

    LoadPlayerData();
    const char *levels_filename = "resources/l.dat";
    if (LoadLevels(levels_filename, &levels) > 0)
    {
        cout << "ERROR: File \"" << levels_filename << "\" not found!" << endl;
        return 1;
    }
    LoadLevel(level_num);
#ifndef DONT_CHECK_HASH
    std::vector<uint8_t> digest;
    if (sha256File(levels_filename, digest)) {
        std::string hash = toHexString(digest);
        if (hash == levels_hash)
        {
            std::cout << "INFO: HASH: Matches [Levels]: " << hash << std::endl;
        }
        else
        {
            std::cerr << "ERROR: HASH: Does not mach [levels]: " << hash << std::endl;
            exit = true;
        }
    } else {
        std::cerr << "ERROR: HASH: Failed to compute hash.\n";
        exit = true;
    }
#endif

    const int btn_width = 450;
    const int num_buttons = 2;
    int button_index = 0;
    Rectangle play_btn_rect = {(float)screenWidth / 2 - btn_width / 2, (float)screenHeight / 2 + (button_index - (float)num_buttons / 2) * 35, btn_width, 30};
    button_index++;
    Rectangle quit_btn_rect = {(float)screenWidth / 2 - btn_width / 2, (float)screenHeight / 2 + (button_index - (float)num_buttons / 2) * 35, btn_width, 30};

    Rectangle pause_btn_rect = {(float)screenWidth / 2, 10, (float)MeasureText("Pause", 30) + 10, 30};
    pause_btn_rect.x -= pause_btn_rect.width / 2;

    const int ui_rects_count = 1;
    Rectangle ui_rects[ui_rects_count] = {pause_btn_rect};

    // Main Loop
    while (!WindowShouldClose() && !exit)
    {
        BeginDrawing();
        ClearBackground(DARKGREEN);
        {
            float delta_time = GetFrameTime();
            Vector2 mouse_pos = GetMousePosition();

            bool is_mouse_on_ui = false;
            for (int i = 0; i < ui_rects_count; i++)
                if (CheckCollisionPointRec(mouse_pos, ui_rects[i]))
                    is_mouse_on_ui = true;

            if (game_stage == 0) // Main Menu
            {
                DrawLevel();

                DrawText("GOLF!", screenWidth / 2, 100, 60, 0.5, RAYWHITE);

                if (Button(play_btn_rect, "PLAY", 30, DARKGRAY, BLUE, DARKBLUE))
                {
                    game_stage = 4;
                }
                if (Button(quit_btn_rect, "QUIT", 30, DARKGRAY, RED, RED))
                {
                    exit = true;
                }
            }
            else if (game_stage == 1) // Playing
            {
                bool is_aiming = IsMouseButtonDown(0) && Vector2Distance({0, 0}, ball_velocity) < 10 && !is_mouse_on_ui;
                if (is_aiming)
                {
                    ball.rotation = Vector2Angle({0, 1}, Vector2Subtract(mouse_pos, ball.position)) * RAD2DEG;
                    aim_distance = Vector2Distance(ball.position, mouse_pos);
                    if (aim_distance > AIM_MAX_DISTANCE)
                    {
                        aim_distance = AIM_MAX_DISTANCE;
                        aim_rect_color = AIM_RECT_MAX_COLOR;
                    }
                    else
                    {
                        aim_rect_color = AIM_RECT_NORMAL_COLOR;
                    }
                    DrawRectanglePro({ball.position.x, ball.position.y, aim_distance, AIM_RECT_WIDTH}, {0, AIM_RECT_WIDTH / 2}, Vector2Angle({1, 0}, Vector2Subtract(mouse_pos, ball.position)) * RAD2DEG, aim_rect_color);
                }
                if (is_mouse_on_ui)
                    aim_distance = 0;
                if (aim_distance != 0 && !is_aiming && !is_mouse_on_ui)
                {
                    ball_velocity = {sin(ball.rotation * DEG2RAD) * aim_distance * BALL_FORCE_MULTIPLYER, -cos(ball.rotation * DEG2RAD) * aim_distance * BALL_FORCE_MULTIPLYER};
                    aim_distance = 0;
                    shots_left--;
                }

                ball_velocity.x /= BALL_FRICTION;
                ball_velocity.y /= BALL_FRICTION;

                float rotation = ball.rotation;
                ball.rotation = 0;
                
                Vector2 move_by = {0, 0};
                Vector2 ball_old_position = ball.position;
                ball.position.x += ball_velocity.x * delta_time;
                bool collision_x = false;
                for (int i = 0; i < num_rects; i++)
                    if (ball.GetCollisionRect(rects[i], {0, 0}, 0))
                        collision_x = true;
                for (int i = 0; i < num_bumpers; i++)
                {
                    Rectangle bumper_rect = {(float)bumpers[i].grid_x * levels.grid_size, (float)bumpers[i].grid_y * levels.grid_size, (float)levels.grid_size, (float)levels.grid_size};
                    if (ball.GetCollisionRect(bumper_rect, {0, 0}, 0))
                    {
                        collision_x = true;
                        if (bumpers[i].remaining_seconds <= 0)
                        {
                            ball_velocity = Vector2Multiply(Vector2Normalize(ball_velocity), {BUMPER_BOUNCE_VELOCITY, BUMPER_BOUNCE_VELOCITY});
                            bumpers[i].remaining_seconds = 2;
                        }
                    }
                }
                if (collision_x || ball.position.x - ball.GetWidth() / 2 < 0 || ball.position.x + ball.GetWidth() / 2 > screenWidth)
                    ball_velocity.x = -ball_velocity.x * BALL_BOUNCIENESS;
                else
                    move_by.x = ball_velocity.x * delta_time;
                ball.position.x = ball_old_position.x;

                ball.position.y += ball_velocity.y * delta_time;
                bool collision_y = false;
                for (int i = 0; i < num_rects; i++)
                    if (ball.GetCollisionRect(rects[i], {0, 0}, 0))
                        collision_y = true;
                for (int i = 0; i < num_bumpers; i++)
                {
                    Rectangle bumper_rect = {(float)bumpers[i].grid_x * levels.grid_size, (float)bumpers[i].grid_y * levels.grid_size, (float)levels.grid_size, (float)levels.grid_size};
                    if (ball.GetCollisionRect(bumper_rect, {0, 0}, 0))
                    {
                        collision_y = true;
                        if (bumpers[i].remaining_seconds <= 0)
                        {
                            ball_velocity = Vector2Multiply(Vector2Normalize(ball_velocity), {BUMPER_BOUNCE_VELOCITY, BUMPER_BOUNCE_VELOCITY});
                            bumpers[i].remaining_seconds = 2;
                        }
                    }
                }
                if (collision_y || ball.position.y - ball.GetHeight() / 2 < 0 || ball.position.y + ball.GetHeight() / 2 > screenHeight)
                    ball_velocity.y = -ball_velocity.y * BALL_BOUNCIENESS;
                else
                    move_by.y = ball_velocity.y * delta_time;
                ball.position.y = ball_old_position.y;

                ball.rotation = rotation;
                if (abs(move_by.x) > 0.2 || abs(move_by.y) > 0.2)
                    ball.rotation = Vector2Angle({0, -1}, move_by) * RAD2DEG;
                
                ball.position.x += move_by.x;
                ball.position.y += move_by.y;

                if (CheckCollisionCircles(ball.position, ball.GetWidth() / 2, hole.position, hole.GetWidth() / 3))
                {
                    if (level_num + 1 > highest_level)
                        highest_level = level_num + 1;
                    level_num++;
                    if (level_num >= levels.count)
                    {
                        level_num = 0;
                        game_stage = 3;
                    }
                    LoadLevel(level_num);
                }
                else if (shots_left <= 0 && Vector2Distance({0, 0}, ball_velocity) < 10)
                {
                    LoadLevel(level_num);
                }

                DrawLevel();

                char text[25];
                sprintf(text, "Level: %d", level_num + 1);
                DrawText(text, 10, 10, 30, RAYWHITE);

                if (Button(pause_btn_rect, "Pause", 30, DARKGRAY, BLUE, RED))
                {
                    game_stage = 2;
                }

                sprintf(text, "Shots: %d", shots_left);
                DrawText(text, screenWidth - 10, 10, 30, 1, RAYWHITE);
            }
            else if (game_stage == 2) // Pause Menu
            {
                DrawLevel();
                char text[25];
                sprintf(text, "Level: %d", level_num);
                DrawText(text, 10, 10, 30, RAYWHITE);
                sprintf(text, "Shots: %d", shots_left);
                DrawText(text, screenWidth - 10, 10, 30, 1, RAYWHITE);

                const int btn_count = 4;
                int btn_index = 0;
                Rectangle background = {(screenWidth - (btn_width + 10)) / 2.0f, screenHeight / 2.0f - 70 - 100, btn_width + 10, 300};
                Rectangle resume_btn = {(screenWidth - btn_width) / 2.0f, screenHeight / 2 + (btn_index - btn_count / 2.0f) * 35 + 60, btn_width, 30};
                btn_index++;
                Rectangle restart_btn = {(screenWidth - btn_width) / 2.0f, screenHeight / 2 + (btn_index - btn_count / 2.0f) * 35 + 60, btn_width, 30};
                btn_index++;
                Rectangle main_menu_btn = {(screenWidth - btn_width) / 2.0f, screenHeight / 2 + (btn_index - btn_count / 2.0f) * 35 + 60, btn_width, 30};
                btn_index++;
                Rectangle quit_btn = {(screenWidth - btn_width) / 2.0f, screenHeight / 2 + (btn_index - btn_count / 2.0f) * 35 + 60, btn_width, 30};

                DrawRectangle(background.x, background.y, background.width, background.height, DARKGRAY);
                DrawText("Paused", screenWidth / 2, background.y + 20, 60, 0.5f, RAYWHITE);
                if (Button(resume_btn, "Resume", 30, GRAY, BLUE, DARKBLUE))
                {
                    game_stage = 1;
                }
                if (Button(restart_btn, "Restart Level", 30, GRAY, BLUE, DARKBLUE))
                {
                    LoadLevel(level_num);
                    game_stage = 1;
                }
                if (Button(main_menu_btn, "Main Menu", 30, GRAY, BLUE, DARKBLUE))
                {
                    ball_velocity = {0, 0};
                    level_num = 0;
                    LoadLevel(level_num);
                    game_stage = 0;
                }
                if (Button(quit_btn, "Quit Game", 30, GRAY, RED, RED))
                {
                    exit = true;
                }
            }
            else if (game_stage == 3) // Completed all levels
            {
                DrawLevel();

                const int btn_count = 2;
                int btn_index = 0;
                Rectangle background = {(screenWidth - (btn_width + 10)) / 2.0f, screenHeight / 2.0f - 70 - 110, btn_width + 10, 275};
                Rectangle main_menu_btn = {(screenWidth - btn_width) / 2.0f, screenHeight / 2 + (btn_index - btn_count / 2.0f) * 35 + 60, btn_width, 30};
                btn_index++;
                Rectangle quit_btn = {(screenWidth - btn_width) / 2.0f, screenHeight / 2 + (btn_index - btn_count / 2.0f) * 35 + 60, btn_width, 30};

                DrawRectangle(background.x, background.y, background.width, background.height, DARKGRAY);
                DrawText("YOU WIN!", screenWidth / 2, background.y + 20, 60, 0.5f, RAYWHITE);
                DrawText("You completed all levels", screenWidth / 2, background.y + 100, 30, 0.5f, RAYWHITE);
                DrawText("...", screenWidth / 2, background.y + 130, 30, 0.5f, RAYWHITE);
                if (Button(main_menu_btn, "Main Menu", 30, GRAY, BLUE, DARKBLUE))
                {
                    ball_velocity = {0, 0};
                    level_num = 0;
                    LoadLevel(level_num);
                    game_stage = 0;
                }
                if (Button(quit_btn, "Quit Game", 30, GRAY, RED, RED))
                {
                    exit = true;
                }
            }
            else if (game_stage == 4) // Level selection
            {
                DrawLevel();

                const Color btn_color = {95, 95, 95, 255};
                const int levels_per_page = 15;
                const int pages_count = (int)ceil((float)levels.count / levels_per_page);
                int levels_last_page = levels_per_page;
                if (levels.count % levels_per_page != 0)
                    levels_last_page = levels.count % levels_per_page;

                DrawRectangle(screenWidth / 2 - 275, screenHeight / 2 - 240, 550, 450, DARKGRAY);
                if (Button({(float)screenWidth / 2 + 215, (float)screenHeight / 2 - 230, 50, 30}, "x", 30, GRAY, BLUE, DARKBLUE))
                    game_stage = 0;
                DrawText("Choose a Level", screenWidth / 2, screenHeight / 2 - 220, 45, 0.5f, RAYWHITE);
                char page_text[25];
                sprintf(page_text, "Page %d of %d", lvlslc_page + 1, pages_count);
                DrawText(page_text, screenWidth / 2, screenHeight / 2 - 120, 30, 0.5f, RAYWHITE);
                const int levels_this_page = (lvlslc_page == pages_count - 1) ? levels_last_page : levels_per_page;
                for (int i = 0; i < levels_this_page; i++)
                {
                    int lvl_num = lvlslc_page * levels_per_page + i;
                    char text[3];
                    sprintf(text, "%d", lvl_num + 1);
                    if (lvl_num > highest_level)
                    {
                        Button({(float)screenWidth / 2 - 235 + (i % 5 * 95), (float)screenHeight / 2 - 80 + (i / 5 * 95), 90, 90}, text, 30, btn_color, btn_color, btn_color);
                    }
                    else if (Button({(float)screenWidth / 2 - 235 + (i % 5 * 95), (float)screenHeight / 2 - 80 + (i / 5 * 95), 90, 90}, text, 30, GRAY, BLUE, DARKBLUE))
                    {
                        level_num = lvl_num;
                        LoadLevel(level_num);
                        game_stage = 1;
                    }
                }

                if (lvlslc_page == 0)
                {
                    Button({(float)screenWidth / 2 - 265, (float)screenHeight / 2 - 80, 20, 280}, "<", 30, btn_color, btn_color, btn_color);
                }
                else if (Button({(float)screenWidth / 2 - 265, (float)screenHeight / 2 - 80, 20, 280}, "<", 30, GRAY, BLUE, DARKBLUE))
                {
                    lvlslc_page--;
                }

                if (lvlslc_page == pages_count - 1)
                {
                    Button({(float)screenWidth / 2 + 245, (float)screenHeight / 2 - 80, 20, 280}, ">", 30, btn_color, btn_color, btn_color);
                }
                else if (Button({(float)screenWidth / 2 + 245, (float)screenHeight / 2 - 80, 20, 280}, ">", 30, GRAY, BLUE, DARKBLUE))
                {
                    lvlslc_page++;
                }
            }
        }
        EndDrawing();
    }

    SavePlayerData();

    FreeLevel();
    FreeLevels(levels.all, levels.count, levels.grid_width);
    ball.Delete();
    hole.Delete();
    CloseWindow();
    return 0;
}