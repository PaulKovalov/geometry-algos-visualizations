#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

double size_of_matrix;
double size_of_screen;
struct Point
{
    Vector2f v;
    Point(double x, double y)
    {
        double unit = size_of_screen / size_of_matrix;
        v.x=(size_of_screen / (2 * size_of_matrix)) + (x * unit);
        v.y=(size_of_screen / (2 * size_of_matrix)) + (y * unit);
        //cout<<v.x<<" "<<v.y<<endl;
    }
};
class Matrix
{
  private:
    int **matrix;
    int size = 0;
    vector<Point> points;
    RenderWindow *window;
    Clock clock;
  public:
    Matrix(int size, RenderWindow &window)
    {
        this->window = &window;
        this->size = size;
        matrix = new int *[size];
        for (int i = 0; i < size; i++)
        {
            matrix[i] = new int[size];
        }
        matrix[0][0] = 2;
        matrix[0][1] = 3;
        matrix[1][0] = 1;
        matrix[1][1] = 4;
    }
    void print()
    {

        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                cout << matrix[i][j] << (matrix[i][j] < 100 ? "  " : " ");
                cout << (matrix[i][j] < 10 ? " " : "");
            }
            cout << endl;
        }
        cout << endl;
    }
    void rotate(int direction)
    {
        int **matrix_tmp = new int *[size];
        for (int i = 0; i < size; i++)
        {
            matrix_tmp[i] = new int[size];
        }
        if (direction > 0)
        {
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    matrix_tmp[j][size - i - 1] = matrix[i][j];
                }
            }
        }
        else
        {
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    matrix_tmp[i][j] = matrix[j][size - i - 1];
                }
            }
        }
        for (int i = 0; i < size; i++)
            delete[] matrix[i];
        delete[] matrix;
        matrix = matrix_tmp;
    }
    ~Matrix()
    {
        for (int i = 0; i < size; i++)
            delete[] matrix[i];
        delete[] matrix;
    }
    void mirror_matrix()
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size / 2; j++)
            {
                swap(matrix[i][j], matrix[i][size - j - 1]);
            }
        }
    }
    //c - is the number of step
    void create_peano_matrix(int c)
    {
        if (c == 0)
        {
            return;
        }
        int n_size = size * 2, k, t;
        size_of_matrix = n_size;
        k = t = pow(4, log(size * size) / log(4));
        int **matrix_tmp = new int *[n_size];
        for (int i = 0; i < n_size; i++)
        {
            matrix_tmp[i] = new int[n_size];
        }
        //upper left
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                matrix_tmp[i][j] = matrix[i][j] + k;
            }
        }
        k = t * 2;
        //upper right
        for (int i = 0; i < size; i++)
        {
            for (int j = size, prev_j = 0; j < n_size; j++, prev_j++)
            {
                matrix_tmp[i][j] = matrix[i][prev_j] + k;
            }
        }
        k = t * 3;
        //lower right
        mirror_matrix();
        rotate(-1);
        for (int i = size, prev_i = 0; i < n_size; i++, prev_i++)
        {
            for (int j = size, prev_j = 0; j < n_size; j++, prev_j++)
            {
                matrix_tmp[i][j] = matrix[prev_i][prev_j] + k;
            }
        }
        //lower left
        rotate(1);
        rotate(1);
        for (int i = size, prev_i = 0; i < n_size; i++, prev_i++)
        {
            for (int j = 0; j < size; j++)
            {
                matrix_tmp[i][j] = matrix[prev_i][j];
            }
        }

        for (int i = 0; i < size; i++)
            delete[] matrix[i];
        delete[] matrix;
        this->size = n_size;
        matrix = matrix_tmp;
        --c;
        create_points();
        draw_line();
        clock.restart();
        create_peano_matrix(c);
    }
    void create_points()
    {
        vector<Point>().swap(points);
        for (int k = 1; k <= size * size; k++)
        {
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    if(matrix[i][j] == k){
                        points.push_back(Point(j,i));
                    }
                }
            }
        }
    }
    void draw_line(){
        window->clear(Color::White);
        for(int i = 0; i< points.size()-1; i++){
            Vertex line[] = {
                Vertex(points[i].v,Color::Black),
                Vertex(points[i+1].v, Color::Black)
            };
            //cout<<points[i].v.x<<" "<<points[i].v.y<<endl;
            window->draw(line, 2, sf::Lines);
        }
        window->display();
        while(clock.getElapsedTime().asMilliseconds() < 1000);
    }
};
int main()
{

    RenderWindow window(VideoMode(600, 600), "Hilbert Curve");
    window.setFramerateLimit(60);
    size_of_screen = 600;
    Matrix m(2, window);
    //m.print();
    m.create_peano_matrix(5);
    //m.print();
    m.create_points();
    
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                return 0;
            }
        }
        window.clear(Color::White);
        m.draw_line();
        window.display();
    }
    return 0;
}