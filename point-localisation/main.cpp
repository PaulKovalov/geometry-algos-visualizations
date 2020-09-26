#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <cassert>

using namespace std;
using namespace sf;

int GRID_SIZE = 100;

enum turn_type { left_turn = 1,
                 right_turn = -1,
                 collinear = 0 };

double cross(const sf::Vector2f &a, const sf::Vector2f b) {
    return a.x * b.y - b.x * a.y;
}

turn_type turn(const sf::Vector2f &a, const sf::Vector2f &b, const sf::Vector2f &c) {
    double det = cross(b - a, c - a);
    if (det > 0) {
        return left_turn;
    } else if (det < 0) {
        return right_turn;
    }
    return collinear;
}

vector<Vector2f> generate_polygon(int v) {
    vector<Vector2f> polygon;
    vector<vector<int> > grid(GRID_SIZE, vector<int>(GRID_SIZE, 0));
    for (int i = 0; i < v; ++i) {
        Vector2f vertex = Vector2f(rand() % GRID_SIZE, rand() % GRID_SIZE);
        int attempt_limit = 10;
        while (grid[vertex.x][vertex.y] && attempt_limit) {
            --attempt_limit;
            vertex = Vector2f(rand() % GRID_SIZE, rand() % GRID_SIZE);
        }
        grid[vertex.x][vertex.y] = 1;
        polygon.push_back(Vector2f(vertex.x, vertex.y));
    }
    Vector2f leftmost(1e9, 1e9), rightmost(-1e9, -1e9);
    for (int i = 0; i < polygon.size(); ++i) {
        if (polygon[i].x < leftmost.x) {
            leftmost = polygon[i];
        }
        if (polygon[i].x > rightmost.x) {
            rightmost = polygon[i];
        }
    }
    vector<Vector2f> top_points, bottom_points;
    for (int i = 0; i < polygon.size(); ++i) {
        turn_type t = turn(rightmost, leftmost, polygon[i]);
        if (t == left_turn) {
            top_points.push_back(polygon[i]);
        } else {
            bottom_points.push_back(polygon[i]);
        }
    }
    top_points.erase(remove_if(top_points.begin(), top_points.end(), [leftmost, rightmost](Vector2f &p){return p == rightmost || p == leftmost;}), top_points.end());
    bottom_points.erase(remove_if(bottom_points.begin(), bottom_points.end(), [leftmost, rightmost](Vector2f &p){return p == rightmost || p == leftmost;}), bottom_points.end());
    sort(top_points.begin(), top_points.end(), [](Vector2f& a, Vector2f& b){return a.x < b.x;});
    sort(bottom_points.begin(), bottom_points.end(), [](Vector2f& a, Vector2f& b){return a.x > b.x;});
    polygon.clear();
    polygon.push_back(leftmost);
    polygon.insert(polygon.end(), top_points.begin(), top_points.end());
    polygon.push_back(rightmost);
    polygon.insert(polygon.end(), bottom_points.begin(), bottom_points.end());
    assert(polygon[0] == leftmost);
    for (int i = 1; i <= top_points.size(); ++i) {
        assert(polygon[i] == top_points[i - 1]);
    }
    assert(polygon[top_points.size() + 1] == rightmost);
    for (int i = top_points.size() + 2, j = 0; i < polygon.size(), j < bottom_points.size(); ++i, ++j) {
        assert(polygon[i] == bottom_points[j]);
    }
    return polygon;
}

Vector2f generate_random_point(int width, int height) {
    int x = rand() % width;
    int y = rand() % height;
    return Vector2f(x, y);
}

int ccw(Vector2f a,Vector2f b, Vector2f c) {
    return (c.y-a.y) * (b.x-a.x) > (b.y-a.y) * (c.x-a.x);
}

bool intersect(Vector2f a, Vector2f b, Vector2f c, Vector2f d) {
    return (ccw(a, c, d) != ccw(b, c, d)) && (ccw(a, b, c) != ccw(a, b, d));
}

int main() {
    srand(time(NULL));
    int screen_width = 700, screen_height = 700;
    int ch_width = 500, ch_height = 500;
    int offset_x = (screen_width - ch_width) / 2;
    int offset_y = (screen_height - ch_height) / 2;
    RenderWindow window(VideoMode(screen_width, screen_height), "Point localization");
    window.setFramerateLimit(30);
    vector<Vector2f> polygon_points = generate_polygon(20);
    // generate a random point
    Vector2f point = generate_random_point(screen_width, screen_height);
    Vector2f ray = Vector2f(1e5, point.y);
    // count number of intersections
    int count = 0;
    for (int i = 0; i < polygon_points.size(); ++i) {
        Vector2f p1 = Vector2f(offset_x + polygon_points[i].x * 5, offset_y + polygon_points[i].y * 5);
        Vector2f p2 = Vector2f(offset_x + polygon_points[(i + 1) % polygon_points.size()].x * 5, offset_y + polygon_points[(i + 1) % polygon_points.size()].y * 5);
        if (intersect(p1, p2, point, Vector2f(0,0))) {
            ++count;
        }
    }
    cout << "Count: " << count << endl;
    if (count % 2) {
        cout << "Point is inside the polygon" << endl;
    } else {
        cout << "Point is outside the polygon" << endl;
    }
    // count intersections
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (Keyboard::isKeyPressed(Keyboard::Escape) || Keyboard::isKeyPressed(Keyboard::Q)) {
                window.close();
                return 0;
            }
        }
        window.clear();
        CircleShape circle(2);
        circle.setPosition(point);
        circle.setFillColor(Color::Red);
        window.draw(circle);
        for (int i = 0; i < polygon_points.size(); ++i) {
            Vertex line[] = {
                Vertex(polygon_points[i]),
                Vertex(polygon_points[(i + 1) % polygon_points.size()]),
            };
            line[0].position.x *= 5;
            line[0].position.x += offset_x;
            line[0].position.y *= 5;
            line[0].position.y += offset_y;
            line[1].position.x *= 5;
            line[1].position.x += offset_x;
            line[1].position.y *= 5;
            line[1].position.y += offset_y;
            line[0].color = Color::White;
            line[1].color = Color::White;
            window.draw(line, 2, Lines);
        }
        window.display();
    }
    return 0;
}