#include <SDL2/SDL.h>
#include <memory>
#include <unistd.h>
#include <vector>
#include <iostream>

using namespace std;

SDL_Window *window = nullptr;
SDL_Surface *surface = nullptr;

// Width and height of your cell in pixels
unsigned int CELL_SIZE = 2;

unsigned int CELLMAP_WIDTH = 480,
        CELLMAP_HEIGHT = 480;

unsigned int SCREEN_WIDTH = CELLMAP_WIDTH * CELL_SIZE,
        SCREEN_HEIGHT = CELLMAP_HEIGHT * CELL_SIZE;

void DrawCell(unsigned int x, unsigned int y, unsigned int colour);

class Cell final {
public:
    Cell();

    ~Cell();

    bool GetState();

    void SetOn();

    void SetOff();

private:
    bool state;
};

Cell::Cell() : state(false) {}

Cell::~Cell() = default;

bool Cell::GetState() {
    return state;
}

void Cell::SetOn() {
    state = true;
}

void Cell::SetOff() {
    state = false;
}

class CellMap final {
public:
    CellMap(unsigned int width, unsigned int height);

    ~CellMap();

    void SetCell(unsigned int x, unsigned int y);

    void ClearCell(unsigned int x, unsigned int y);

    void Init();

    void InitBlinker(unsigned int x, unsigned int y);

    void InitGlider(unsigned int x, unsigned int y);
    
    unsigned int GetNeighbours(unsigned int x, unsigned int y);

    void NextGen();


private:
    vector<vector<Cell>> cells, temp_cells;
    unsigned int w, h;
    unsigned int length;
};

CellMap::CellMap(unsigned int width, unsigned int height) : w(width), h(height) {
    length = w * h;
    cells.resize(w, vector<Cell>(h));
}

CellMap::~CellMap() = default;

void CellMap::Init() {
    auto seed = (unsigned) time(nullptr);
    srand(seed);

    unsigned int x, y;
    for (int i = 0; i < length * 0.7; i++) {
        x = rand() % (w - 1);
        y = rand() % (h - 1);

        if (!cells[x][y].GetState()) {
            SetCell(x, y);
        }
    }
}

void CellMap::InitBlinker(unsigned int x, unsigned int y) {
    for (int i = -1; i <= 1; ++i) {
        SetCell(x + i, y);
    }
}

void CellMap::InitGlider(unsigned int x, unsigned int y) {

    SetCell(x, y);
    SetCell(x + 1, y - 1);
    SetCell(x + 1, y - 2);
    SetCell(x, y - 2);
    SetCell(x - 1, y - 2);

}


unsigned int CellMap::GetNeighbours(unsigned int x, unsigned int y) {
    int n_neighbours = 0,
            neighbourX, neighbourY;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0)
                continue;


            if (x + i < 0 || x + i >= w)
                i > 0 ? neighbourX = 0 : neighbourX = w - 1;
            else
                neighbourX = x + i;

            if (y + j < 0 || y + j >= h)
                j > 0 ? neighbourY = 0 : neighbourY = h - 1;
            else
                neighbourY = y + j;

            n_neighbours += temp_cells[neighbourX][neighbourY].GetState();
        }
    }
    return n_neighbours;
}

void CellMap::NextGen() {
    int n_neighbours;
    temp_cells = cells;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            n_neighbours = GetNeighbours(x, y);
            if (temp_cells[x][y].GetState()) {
                if ((n_neighbours != 2) && (n_neighbours != 3)) {
                    ClearCell(x, y);
                    DrawCell(x, y, 0x00);
                }
            } else {
                if (n_neighbours == 3) {
                    SetCell(x, y);
                    DrawCell(x, y, 0xFF);
                }
            }
        }
    }

}

void CellMap::SetCell(unsigned int x, unsigned int y) {
    cells[x][y].SetOn();
}

void CellMap::ClearCell(unsigned int x, unsigned int y) {
    cells[x][y].SetOff();
}

void DrawCell(unsigned int x, unsigned int y, unsigned int colour) {
    Uint8 *pixel_ptr = (Uint8 *) surface->pixels + (y * CELL_SIZE * CELLMAP_WIDTH + x * CELL_SIZE) * 4;
    for (unsigned i = 0; i < CELL_SIZE; i++) {
        for (unsigned j = 0; j < CELL_SIZE; j++) {
            for (unsigned k = 0; k < 3; k++) {
                *(pixel_ptr + j * 4 + k) = colour;
            }
        }
        pixel_ptr += SCREEN_WIDTH * 4;
    }
}


int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    surface = SDL_GetWindowSurface(window);

    // Event Handler
    SDL_Event e;

    CellMap map = CellMap(CELLMAP_WIDTH, CELLMAP_HEIGHT);
    map.Init();


    // Rendering loop
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0)
            if (e.type == SDL_QUIT)
                quit = true;

        map.NextGen();

//        usleep(100000);


        SDL_UpdateWindowSurface(window);
    }


    SDL_DestroyWindow(window);
    SDL_Quit();
}
