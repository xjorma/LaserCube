#include "pch.h"
#include "VectorAnim.h"
#include "Choreography.h"

//VectorAnim *animTest;
Choreography* choreography;

std::chrono::high_resolution_clock::time_point startTime;

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);       /* Establish viewing area to cover entire window. */
    glMatrixMode(GL_PROJECTION);  /* Start modifying the projection matrix. */
    glLoadIdentity();             /* Reset project matrix. */
    glOrtho(0, 4096, 4096, 0, -1, 1);   /* Map abstract coords directly to window coords. */
}

void display(void)
{
    std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = currentTime - startTime;
    double time = elapsed_seconds.count();
    //double curAnimTime = mod(time, animTest->duration());
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    //const std::vector<std::vector<vec2>> &frame = animTest->getShapeFromTime(curAnimTime);
    const std::vector<std::vector<vec2>> &frame = choreography->GetShapeFromTime(time);
    for (const std::vector<vec2>& shape : frame)
    {
        if (shape.size() <= 1)
            continue;
        for (int i = 0; i < (int)shape.size() - 1; i++)
        {
            glColor3f(1.0, 1.0, 1.0);
            glVertex2f(shape[i].x, shape[i].y);
            glColor3f(1.0, 1.0, 1.0);
            glVertex2f(shape[i + 1].x, shape[i + 1].y);
        }
        
        glColor3f(1.0, 1.0, 1.0);
        glVertex2f(shape[shape.size() - 1].x, shape[shape.size() - 1].y);
        glColor3f(1.0, 1.0, 1.0);
        glVertex2f(shape[0].x, shape[0].y);
    }
    glEnd();
    glutSwapBuffers();
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    choreography = new Choreography("./data/split_map.json", "./data/Sequence.txt", "./data/");
    //animTest = new VectorAnim("./data/script0cbd82.json", true);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("single triangle");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    startTime = std::chrono::high_resolution_clock::now();
    glutMainLoop();
    delete choreography;
    return 0;
}