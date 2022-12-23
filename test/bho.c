#include <stdio.h>
#include <stdbool.h>

#define N 10


// Define the directions
const int dx[4] = {-1, 0, 1, 0};
const int dy[4] = {0, 1, 0, -1};

// Define the labyrinth
char labyrinth[N][N] = {
    {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
    {'#', '?', ' ', '#', ' ', ' ', ' ', '#', ' ', '#'},
    {'#', ' ', '#', '#', ' ', '#', ' ', '#', ' ', '#'},
    {'#', ' ', '#', ' ', ' ', '#', ' ', '#', ' ', '#'},
    {'#', ' ', '#', ' ', '#', '#', ' ', '#', ' ', '#'},
    {'#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
    {'#', ' ', '#', '#', '#', '#', '#', '#', ' ', '#'},
    {'#', ' ', ' ', ' ', ' ', ' ', ' ', '_', ' ', '#'},
    {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
};

// Define the visited matrix
bool visited[N][N];
char path[N * N];
int index = 0;

// Define the start and end points
int start_x = 0, start_y = 1;
int end_x = 7, end_y = 8;

// Define the function to solve the labyrinth using DFS
bool solve(int x, int y, int i) {
    // Check if we have reached the end
    if (x == end_x && y == end_y) {
        path[index] = '\0';
        return true;
    }
    // Mark the current cell as visited
    visited[x][y] = true;
    // Try all possible directions
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        // Check if the new cell is valid and not visited
        if (nx >= 0 && nx < N && ny >= 0 && ny < N && labyrinth[nx][ny] != '#' && !visited[nx][ny]) {
            // Add the current direction to the path
            path[index] = (i == 0) ? 'w' : (i == 1) ? 'd' : (i == 2) ? 's' : 'a';
            // Recursively solve the labyrinth from the new cell
            if (solve(nx, ny, index++)) {
                return true;
            }
        }
    }
    // If no solution was found, remove the current direction
    return false;
}

int main() {
    // Solve the labyrinth
    if (solve(start_x, start_y, index)) {
        printf("Solution found!\n");
        for (size_t i = 1; i < index; i++) {
            printf("%c ", path[i]);
        }
        
    } else {
        printf("No solution found.\n");
    }
    return 0;
}