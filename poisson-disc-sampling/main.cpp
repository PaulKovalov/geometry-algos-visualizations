#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <queue>
#include <vector>

using namespace sf;
using namespace std;

int K, R = 20;
double PI = acos(-1);

double random_coord(double l, double h) {
    double r = (double)rand() / RAND_MAX;
    return l + r * (h - l);
}

bool check_adjacents(int x, int y, Vector2f point, vector<vector<Vector2f>> &points) {
    x = max(0, x - 2);
    y = max(0, y - 2);
    for (int i = y; i < min((int)points.size(), y + 5); ++i) {
        for (int j = 0; j < min((int)points[0].size(), x + 5); ++j) {
            Vector2f curr_point = points[i][j];
            double f1 = curr_point.x - point.x;
            double f2 = curr_point.y - point.y;
            double d = sqrt(f1 * f1 + f2 * f2);
            if (d < R && d != 0) {
                return false;
            }
        }
    }
    return true;
}

vector<vector<Vector2f>> generate_points(int width, int height) {
    int r_over_sq = R / sqrt(2);  // the size of the grid cell
    int grid_width = width / r_over_sq, grid_height = height / r_over_sq;
    queue<Vector2f> q;
    vector<vector<Vector2f>> points(grid_height, vector<Vector2f>(grid_width, Vector2f(-100, -100)));
    q.push(Vector2f(random_coord(0, width), random_coord(0, height)));
    while (!q.empty()) {
        Vector2f current_point = q.front();
        q.pop();
        for (int k = 0; k < K; ++k) {
            double phi = random_coord(0, 2 * PI);
            double d = random_coord(R, 2 * R);
            Vector2f new_point(current_point.x + d * cos(phi), current_point.y + d * sin(phi));
            int grid_x = new_point.x / r_over_sq, grid_y = new_point.y / r_over_sq;
            if (grid_x >= grid_width || grid_y >= grid_height || grid_x < 0 || grid_y < 0) {
                continue;
            }
            if (points[grid_y][grid_x].x != -100)
                continue;
            if (check_adjacents(grid_x, grid_y, new_point, points)) {
                points[grid_y][grid_x] = new_point;
                q.push(new_point);
            }
        }
    }
    return points;
}

int main() {
    srand(time(NULL));
    int width = 600, height = 600;
    
    cin >> K;
    RenderWindow window(VideoMode(width, height), "Disk");
    window.setFramerateLimit(30);
    int total_count = 0;
    
    vector<vector<Vector2f>> points = generate_points(width, height);

    while (window.isOpen()) {
        Event event;
        while(window.pollEvent(event)) {
            if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
                return 0;
            }
        }
        window.clear();
        for (int i = 0; i < points.size(); ++i) {
            for (int j = 0; j < points[i].size(); ++j) {
                CircleShape circle(2);
                circle.setPosition(points[i][j].x, points[i][j].y);
                window.draw(circle);
            }
        }
        window.display();
    }
    return 0;
}