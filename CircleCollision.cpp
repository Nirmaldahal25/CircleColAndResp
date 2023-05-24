#include <iostream>
#include <cmath>
#include <array>
#include <GLFW/glfw3.h>

const int HEIGHT = 480;
const int WIDTH = 640;

class Circle
{
public:
    void setMass(float m)
    {
        mass = m;
        radius = m;
    }
    void setPosition(float x, float y, float rad = 0)
    {
        this->centerx = x;
        this->centery = y;
        this->radius = rad;
    }

    void setVelocity(float vx, float vy)
    {
        velocityy = vy;
        velocityx = vx;
    }

    void setColor(float r, float g, float b)
    {
        color[0] = r / 255;
        color[1] = g / 255;
        color[2] = b / 255;
    }

    void resolve()
    {
        // velocityx = velocityx / (std::abs(velocityx) + std::abs(velocityy));
        // velocityy = velocityy / (std::abs(velocityx) + std::abs(velocityy));
        // if (std::sqrt(velocityx * velocityx + velocityy * velocityy) > (radius / 2.0f))
        // {
        //     velocityx = velocityx / (2 * radius + std::abs(velocityx));
        //     velocityy = velocityy / (2 * radius + std::abs(velocityy));
        // }

        centerx += velocityx / 10;
        centery += velocityy / 10;

        if ((centerx - radius) < 0)
        {
            centerx = radius;
            velocityx = -velocityx;
        }
        else if ((centerx + radius) > WIDTH)
        {
            centerx = WIDTH - radius;
            velocityx = -velocityx;
        }
        if ((centery - radius) < 0)
        {
            centery = radius;
            velocityy = -velocityy;
        }
        else if ((centery + radius) > HEIGHT)
        {
            centery = HEIGHT - radius;
            velocityy = -velocityy;
        }
    }

    void drawcircle()
    {
        const int num_segments = 100;
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(color[0], color[1], color[2]);
        for (int ii = 0; ii < num_segments; ii++)
        {
            float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);          // get the current angle
            float x = this->radius * cosf(theta);                                       // calculate the x component
            float y = this->radius * sinf(theta);                                       // calculate the y component
            glVertex2f(this->get_x(x + this->centerx), this->get_y(y + this->centery)); // output vertex
        }
        glEnd();
    }

    friend bool collision(Circle &c1, Circle &c2);

protected:
    float get_y(float y)
    {
        return (y / HEIGHT * 2.0f - 1.0f);
    };

    float get_x(float x)
    {
        return (x / WIDTH * 2.0f - 1.0f);
    }

private:
    float radius;
    float centerx;
    float centery;
    std::array<float, 3> color;

    float mass;
    float velocityx;
    float velocityy;
};

bool collision(Circle &c1, Circle &c2)
{
    float distance = std::sqrt(std::powf(c2.centerx - c1.centerx, 2) + std::powf(c2.centery - c1.centery, 2));
    const float r_distance = c1.radius + c2.radius;
    if (distance <= r_distance)
    {
        // static collision resolution
        // Calculate displacement required
        float fOverlap = 0.5f * (distance - c1.radius - c2.radius);

        const float c1x = c1.centerx;
        const float c1y = c1.centery;
        // Displace Current Ball away from collision
        c1.centerx -= fOverlap * (c1.centerx - c2.centerx) / distance;
        c1.centery -= fOverlap * (c1.centery - c2.centery) / distance;

        // Displace Target Ball away from collision
        c2.centerx += fOverlap * (c1x - c2.centerx) / distance;
        c2.centery += fOverlap * (c1y - c2.centery) / distance;

        // dynamic circle collision resolution
        distance = std::sqrt(std::powf(c2.centerx - c1.centerx, 2) + std::powf(c2.centery - c1.centery, 2));

        const float nx = (c2.centerx - c1.centerx) / distance;
        const float ny = (c2.centery - c1.centery) / distance;

        const float v1d = std::atan2f(c1.velocityy, c1.velocityx);
        const float v2d = std::atan2f(c2.velocityy, c2.velocityx);
        float angleofcontact = std::atan2f(ny, nx);

        const float v1 = std::sqrt(c1.velocityx * c1.velocityx + c1.velocityy * c1.velocityy);
        const float v2 = std::sqrt(c2.velocityx * c2.velocityx + c2.velocityy * c2.velocityy);
        // elastic collision
        const float mmd = c1.mass - c2.mass;
        const float mmt = c1.mass + c2.mass;
        const float v1s = v1 * std::sinf(v1d - angleofcontact);

        const float cp = std::cosf(angleofcontact);
        const float sp = std::sinf(angleofcontact);

        float cdp1 = v1 * std::cosf(v1d - angleofcontact);
        float cdp2 = v2 * std::cosf(v2d - angleofcontact);
        const float cpp = std::cosf(angleofcontact + 3.14f / 2.0f);
        const float spp = std::sinf(angleofcontact + 3.14f / 2.0f);

        float t = (cdp1 * mmd + 2 * c2.mass * cdp2) / mmt;
        c1.velocityx = t * cp + v1s * cpp;
        c1.velocityy = t * sp + v1s * spp;

        angleofcontact += 3.14f;

        const float v2s = v2 * std::sinf(v2d - angleofcontact);
        cdp1 = v1 * std::cosf(v1d - angleofcontact);
        cdp2 = v2 * std::sinf(v2d - angleofcontact);
        t = ((cdp2 * (-mmd)) + 2.0f * c1.mass * cdp1) / mmt;
        c2.velocityx = t * (-cp) + v2s * (-cpp);
        c2.velocityy = t * (-sp) + v2s * (-spp);

        {
            const float vx = std::abs(c1.velocityx) + std::abs(c2.velocityx);
            const float vy = std::abs(c1.velocityy) + std::abs(c2.velocityy);

            c1.velocityx = c1.velocityx / vx;
            c1.velocityy = c1.velocityy / vy;

            c2.velocityx = c2.velocityx / vx;
            c2.velocityy = c2.velocityy / vy;
        }
    }
    return false; // circles collided
}

int main()
{
    glfwInit(); // init glfw libraryc
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Circle Collision", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create opengl window" << std::endl;
    }
    glfwMakeContextCurrent(window);
    std::array<Circle, 10> circles;

    const auto getCircle = [](const std::array<float, 3> &colors, const std::array<float, 2> &velocity, const std::array<float, 2> &positions, float mass) -> Circle
    {
        Circle circle = Circle();
        circle.setColor(colors[0], colors[1], colors[2]);
        circle.setVelocity(velocity[0], velocity[1]);
        circle.setPosition(positions[0], positions[1]);
        circle.setMass(mass);
        return circle;
    };

    circles[0] = getCircle({0.0f, 255.0f, 0.0f}, {1.0f, 1.0f}, {100, 400}, 40.0f);
    circles[1] = getCircle({255.0f, 255.0f, 0.0f}, {1.0f, 1.0f}, {30, 20}, 20.0f);
    circles[2] = getCircle({0.0f, 255.0f, 255.0f}, {1.0f, 1.0f}, {200, 300}, 30.0f);
    circles[3] = getCircle({120.0f, 255.0f, 120.0f}, {1.0f, 1.0f}, {90, 90}, 40.0f);
    int length = 4;
    while (!glfwWindowShouldClose(window))
    {
        // display();
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        for (int i = 0; i < length; i++)
        {
            for (int j = 0; j < length; j++)
            {
                if (i != j)
                {
                    collision(circles[i], circles[j]);
                }
            }
        }

        for (int i = 0; i < length; i++)
        {
            circles[i].resolve();
            circles[i].drawcircle();
        }

        glfwSwapBuffers(window); // redraws contents
        glfwPollEvents();        // listen incoming messages from OS
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}