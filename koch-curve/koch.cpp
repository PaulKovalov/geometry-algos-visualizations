#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace sf;
#define PI 3.14159265
Clock cl;
class Line
{   
    Vertex line[2];
    Vector2f p1, p2;
    float x1, x2, y1, y2;
    float angle;
    float l;

  public:
  
    Line(Vector2f v, float l, float angle)
    {
        this->angle = angle;
        float dx, dy;
        dy = sin(angle * PI / 180) * l;
        dx = cos(angle * PI / 180) * l;
        this->x1 = v.x;
        this->y1 = v.y;
        this->x2 = v.x + dx;
        this->y2 = v.y + dy;
        this->l = l;
        p1 = Vector2f(x1, y1);
        p2 = Vector2f(x2, y2);
        line[0] = Vertex(Vector2f(x1, y1), Color::Black);
        line[1] = Vertex(Vector2f(x2, y2), Color::Black);
    }
    float getAngle()
    {
        return angle;
    }
    Vector2f get_p1()
    {
        return p1;
    }
    Vector2f get_p2()
    {
        return p2;
    }
    Vector2f getCoordsOfThird()
    {
        float dx, dy;
        dx = (x1 + 0.5 * x2) / 1.5;
        dy = (y1 + 0.5 * y2) / 1.5;
        return Vector2f(dx, dy);
    }
    float getLength()
    {
        return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    }
    void draw(RenderWindow &window)
    {
        window.draw(line, 2, sf::Lines);
    }
};
vector<Line> koch(vector<Line> &lines, int p, double &area, RenderWindow &window)
{
    if (p == 0)
        return lines;
    vector<Line> tmp;
    area += ((pow(lines[0].getLength()/3,2)*sqrt(3))*lines.size())/4;
    while(cl.getElapsedTime().asMilliseconds() < 500){}
    cl.restart();
    window.clear(Color::White);
    for (int i = 0; i < lines.size(); i++){
        lines[i].draw(window);
        Line l0(lines[i].get_p1(), lines[i].getLength() / 3, lines[i].getAngle());
        Line l1(l0.get_p2(), lines[i].getLength() / 3, lines[i].getAngle() + 300);
        Line l2(l1.get_p2(), lines[i].getLength() / 3, lines[i].getAngle() + 60);
        Line l3(l2.get_p2(), lines[i].getLength() / 3, lines[i].getAngle());
        tmp.push_back(l0);
        tmp.push_back(l1);
        tmp.push_back(l2);
        tmp.push_back(l3);
    }
    window.display();
    p--;
    return koch(tmp, p, area, window);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Invalid input! Type ./koch _amount of steps_ _length of initial line_" << endl;
        return 0;
    }
    int n = atoi(argv[1]);
    int l = atoi(argv[2]);
    double area = (l*l*sqrt(3))/4;
    Line line(Vector2f(500, 100), l, 60);
    Line line1(line.get_p2(), l, line.getAngle() + 120);
    Line line2(line1.get_p2(), l, line1.getAngle() + 120);
    RenderWindow window(VideoMode(1000, 700), "Koch");
    window.setFramerateLimit(60);
    vector<Line> t;
    t.push_back(line);
    t.push_back(line1);
    t.push_back(line2);
    vector<Line> lines = koch(t, n, area, window);
    stringstream ss;
    ss << fixed<<setprecision(3)<<area;
    string s = ss.str();
    Text text;
    Font font;
    font.loadFromFile("Vertigo.ttf");
    text.setFont(font);
    text.setColor(Color::Red);
    text.setCharacterSize(30);
    text.setPosition(50,50);
    text.setString("S: "+s);
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
                return 0;
            }
            if (event.type == Event::Closed)
            {
                window.close();
                return 0;
            }
        }
        window.clear(Color::White);
        for (int i = 0; i < lines.size(); i++)
        {
            lines[i].draw(window);
        }
        window.draw(text);
        window.display();
    }
    return 0;
}