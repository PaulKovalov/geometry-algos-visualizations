#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

double PI = acos(-1);

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

struct PointsSort {
    sf::Vector2f base_vector;
    sf::Vector2f base_point;
    PointsSort(sf::Vector2f base_point) {
        base_vector.y = 0;
        base_vector.x = 1000 - base_point.x;
        this->base_point = base_point;
    };
    double angle(sf::Vector2f &p) {
        if (p.y == base_point.y) {
            return 0;
        }
        sf::Vector2f vec_p(p.x - base_point.x, p.y - base_point.y);
        double t = (vec_p.x * base_vector.x) / (sqrt(vec_p.x * vec_p.x + vec_p.y * vec_p.y) * sqrt(base_vector.x * base_vector.x));
        if (t < -1) {
            t = -1;
        } else if (t > 1) {
            t = 1;
        }
        t = acos(t) * (180.0 / PI);
        return t;
    }
    bool operator()(sf::Vector2f &a, sf::Vector2f &b) {
        double abp = angle(a);
        double bbp = angle(b);
        return abp < bbp;
    }
};

std::vector<sf::Vector2f> convex_hull(std::vector<sf::Vector2f> &points) {
    sf::Vector2f min_y_point(-1e6, 1e6);
    for (int i = 0; i < points.size(); ++i) {
        if (points[i].y < min_y_point.y) {
            min_y_point = points[i];
        } else if (points[i].y == min_y_point.y && points[i].x > min_y_point.x) {
            min_y_point = points[i];
        }
    }
    sort(points.begin(), points.end(), PointsSort(min_y_point));
    std::vector<sf::Vector2f> ch;
    ch.push_back(points[0]);
    ch.push_back(points[1]);
    for (int i = 2; i < points.size(); ++i) {
        sf::Vector2f next_to_top, top;
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