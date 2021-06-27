#include "pch.h"
#include "VectorAnim.h"
#include "Choreography.h"
#include "TimeProvider.h"
#include "RemapTime.h"
#include "effects.h"

//VectorAnim *animTest;
Choreography* choreography;
RemapTime* remapTime;
Sequencer* sequencer;


TimeProvider *timeProvider;

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);       /* Establish viewing area to cover entire window. */
    glMatrixMode(GL_PROJECTION);  /* Start modifying the projection matrix. */
    glLoadIdentity();             /* Reset project matrix. */
    glOrtho(0, 4096, 4096, 0, -1, 1);   /* Map abstract coords directly to window coords. */
}

void display(void)
{
    double time = timeProvider->GetTime();
    time = remapTime->Convert(time);

    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    const std::vector<std::vector<vec2>> &frame = choreography->GetShapeFromTime(time);

    std::vector<std::vector<Vertex>> vertices = sequencer->Tick(time, frame);

    for (const std::vector<Vertex>& shape : vertices)
    {
        if (shape.size() <= 1)
            continue;
        for (int i = 0; i < (int)shape.size() - 1; i++)
        {
            glColor3f(shape[i].color.r, shape[i].color.g, shape[i].color.b);
            glVertex2f(shape[i].pos.x, shape[i].pos.y);
            glColor3f(shape[i + 1].color.r, shape[i + 1].color.g, shape[i + 1].color.b);
            glVertex2f(shape[i + 1].pos.x, shape[i + 1].pos.y);
        }
    }
    glEnd();
    glutSwapBuffers();
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    choreography = new Choreography("./data/split_map.json", "./data/Sequence.txt", "./data/");
    remapTime = new RemapTime("./data/remap.txt");
    std::vector<vec2> hand = choreography->GetShapeFromTime(double(2040 - 1) / 1000.0)[0];
    sequencer = new Sequencer({
        {0, new ColorEffect(vec3(0,0,1))},
        {2040, new MaskEffect(hand, vec3(0,1,0), vec3(0,0,1), 0.95f)},
        {7720, new ColorEffect(vec3(0,0,1))},
        {33990, new CircleEffect(1500.0f, vec3(0,1,0), vec3(0,0,1))},
        //{33990, new GhostEffect(vec3(0,1,0), vec3(0,0,1), 4)},
        //{33990, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        //{33990, new PlasmaEffect(1000)},
        });
    //animTest = new VectorAnim("./data/script0cbd82.json", true);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("single triangle");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    bool osc = true;
    if (osc)
    {
        timeProvider = new OscTimeProvider(8666);
    }
    else
    {
        timeProvider = new ClockTimeProvider();
    }
    glutMainLoop();
    delete choreography;
    delete timeProvider;
    delete remapTime;
    delete sequencer;
    return 0;
}