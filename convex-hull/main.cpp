#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <queue>
#include <cmath>

using namespace std;
using namespace sf;

int K = 5, R = 20;
double PI = acos(-1);

// the code from the previous lab that generates points of the puasson disk

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

vector<Vector2f> generate_points(int width, int height) {
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
    vector<Vector2f> result;
    for (int i = 0; i < points.size(); ++i) {
        points[i].erase(remove_if(points[i].begin(), points[i].end(), [](Vector2f p) { return p.x <= 0 || p.y <= 0; }), points[i].end());
        result.insert(result.end(), points[i].begin(), points[i].end());
    }
    return result;
}
// ------------
// New code for generating the convex hull

enum turn_type { left_turn = 1,
                 right_turn = -1,
                 collinear = 0 };

double cross(const Vector2f &a, const Vector2f b) {
    return a.x * b.y - b.x * a.y;
}

turn_type turn(const Vector2f &a, const Vector2f &b, const Vector2f &c) {
    double det = cross(b - a, c - a);
    if (det > 0) {
        return left_turn;
    } else if (det < 0) {
        return right_turn;
    }
    return collinear;
}

struct PointsSort {
    Vector2f base_vector;
    Vector2f base_point;
    PointsSort(Vector2f base_point) {
        base_vector.y = 0;
        base_vector.x = 1000 - base_point.x;
        this->base_point = base_point;
    };
    double angle(Vector2f &p) {
        if (p.y == base_point.y) {
            return 0;
        }
        Vector2f vec_p(p.x - base_point.x, p.y - base_point.y);
        double t = (vec_p.x * base_vector.x) / (sqrt(vec_p.x * vec_p.x + vec_p.y * vec_p.y) * sqrt(base_vector.x * base_vector.x));
        if (t < -1) {
            t = -1;
        } else if (t > 1) {
            t = 1;
        }
        t = acos(t) * (180.0 / PI);
        return t;
    }
    bool operator()(Vector2f &a, Vector2f &b) {
        double abp = angle(a);
        double bbp = angle(b);
        return abp < bbp;
    }
};

vector<Vector2f> convex_hull(vector<Vector2f> &points) {
    Vector2f min_y_point(-1e6, 1e6);
    for (int i = 0; i < points.size(); ++i) {
        if (points[i].y < min_y_point.y) {
            min_y_point = points[i];
        } else if (points[i].y == min_y_point.y && points[i].x > min_y_point.x) {
            min_y_point = points[i];
        }
    }
    sort(points.begin(), points.end(), PointsSort(min_y_point));
    vector<Vector2f> ch;
    ch.push_back(points[0]);
    ch.push_back(points[1]);
    for (int i = 2; i < points.size(); ++i) {
        Vector2f next_to_top, top;
        top = ch[ch.size() - 1];
        next_to_top = ch[ch.size() - 2];
        while (turn(next_to_top, top, points[i]) != left_turn) {
            ch.pop_back();
            top = ch[ch.size() - 1];
            next_to_top = ch[ch.size() - 2];
        }
        ch.push_back(points[i]);
    }
    return ch;
}

int main() {
    srand(time(NULL));
    int screen_width = 700, screen_height = 700;
    int ch_width = 500, ch_height = 500;
    int offset_x = (screen_width - ch_width) / 2;
    int offset_y = (screen_height - ch_height) / 2;
    RenderWindow window(VideoMode(screen_width, screen_height), "Convex hull");
    window.setFramerateLimit(30);
    int total_count = 0;

    vector<Vector2f> points = generate_points(ch_width, ch_height);
    for (int i = 0; i < points.size(); ++i) {
        points[i].x += offset_x;
        points[i].y += offset_y;
    }
    vector<Vector2f> ch_points = convex_hull(points);
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (Keyboard::isKeyPressed(Keyboard::Escape) || Keyboard::isKeyPressed(Keyboard::Q)) {
                window.close();
                return 0;
            }
        }
        window.clear();
        for (int i = 0; i < points.size(); ++i) {
            CircleShape circle(2);
            circle.setPosition(points[i].x, points[i].y);
            window.draw(circle);
        }
        for (int i = 0; i < ch_points.size(); ++i) {
            CircleShape circle(2);
            circle.setFillColor(Color::Red);
            circle.setPosition(ch_points[i].x, ch_points[i].y);
            window.draw(circle);
            Vertex line[] = {
                Vertex(ch_points[i]), Vertex(ch_points[(i + 1) % ch_points.size()])
            };
            line[0].color = Color::Red;
            line[0].position.x += 2;
            line[0].position.y += 2;
            line[1].position.x += 2;
            line[1].position.y += 2;
            line[1].color = Color::Red;
            window.draw(line, 2, Lines);
        }
        window.display();
    }

    return 0;
}
