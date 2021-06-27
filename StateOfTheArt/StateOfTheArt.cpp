#include "pch.h"
#include "VectorAnim.h"
#include "Choreography.h"
#include "TimeProvider.h"
#include "RemapTime.h"
#include "effects.h"
#include "AudioCapture.h"

//VectorAnim *animTest;
Choreography* choreography;
RemapTime* remapTime;
Sequencer* sequencer;
AudioCapture *audioCapture;


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
    glBlendFunc(GL_ONE, GL_ONE);
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
    audioCapture = new AudioCapture();
    choreography = new Choreography("./data/split_map.json", "./data/Sequence.txt", "./data/");
    remapTime = new RemapTime("./data/remap.txt");
    std::vector<vec2> hand = choreography->GetShapeFromTime(double(2040 - 1) / 1000.0)[0];
    sequencer = new Sequencer({
        {0, new ColorEffect(vec3(0,0,1))},                                                          // James Bond Begin
        {2040, new MaskEffect(hand, vec3(0,1,0), vec3(0,0,1), 0.95f)},                              // Girl in hand
        {7720, new ColorEffect(vec3(0,0,1))},                                                       // James Bond End

        {19570, new TextEffect(std::string("HOLOSAPIENS"), vec3(1))},
        {19570 + 900, new TextEffect(std::string("STATE"), vec3(1))},
        {19570 + 900 * 2, new TextEffect(std::string("OF"), vec3(1))},
        {19570 + 900 * 3, new TextEffect(std::string("LASER"), vec3(1))},

        {33990, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},

        {43430 + 200 * 1, new TextEffect(std::string("AWESOME"), vec3(1), 0.5)},
        {43430 + 200 * 2, new TextEffect(std::string("CODE"), vec3(1), 0.5)},
        {43430 + 200 * 3, new TextEffect(std::string("BY"), vec3(1), 0.5)},
        {43430 + 200 * 4, new TextEffect(std::string("MAJOR-X"), vec3(1), 0.5)},
        {43430 + 200 * 5, new TextEffect(std::string("KRAZY"), vec3(1), 0.5)},
        {43430 + 200 * 6, new TextEffect(std::string("MUSIC"), vec3(1), 0.5)},
        {43430 + 200 * 7, new TextEffect(std::string("BY"), vec3(1), 0.5)},
        {43430 + 200 * 8, new TextEffect(std::string("TRAVOLTA"), vec3(1), 0.5)},
        {43430 + 200 * 9, new TextEffect(std::string("BY"), vec3(1), 0.5)},
        {43430 + 200 * 10, new TextEffect(std::string("HOLOSAPIENS"), vec3(1), 0.5)},
        {43430 + 200 * 11, new TextEffect(std::string("LOVES"), vec3(1), 0.5)},
        {43430 + 200 * 12, new TextEffect(std::string("SPACEBALLS"), vec3(1), 0.5)},
        {43430 + 200 * 10, new TextEffect(std::string("SHERBROOKE"), vec3(1), 0.5)},
        {43430 + 200 * 11, new TextEffect(std::string("CANADA"), vec3(1), 0.5)},
        {43430 + 200 * 12, new TextEffect(std::string("LASER"), vec3(1), 0.5)},
        {43430 + 200 * 13, new TextEffect(std::string("CUBE"), vec3(1), 0.5)},
        {43430 + 200 * 14, new TextEffect(std::string("RULEZ"), vec3(1), 0.5)},
        {43430 + 200 * 15, new TextEffect(std::string("VOTE"), vec3(1), 0.5)},
        {43430 + 200 * 16, new TextEffect(std::string("YEAH"), vec3(1), 0.5)},

        {46830, new GhostEffect(vec3(0,1,0), vec3(0,0,1), 4)},
        {58030, new PlasmaEffect(1000)},
        {66670, new CircleEffect(1500.0f, vec3(0,1,0), vec3(0,0,1), audioCapture)},

        {73670 + 200 * 1, new TextEffect(std::string("THANKX"), vec3(1), 0.5)},
        {73670 + 200 * 2, new TextEffect(std::string("ENITALP"), vec3(1), 0.5)},
        {73670 + 200 * 3, new TextEffect(std::string("NICHOLAS"), vec3(1), 0.5)},
        {73670 + 200 * 4, new TextEffect(std::string("VECTOR"), vec3(1), 0.5)},
        {73670 + 200 * 5, new TextEffect(std::string("BEAM"), vec3(1), 0.5)},
        {73670 + 200 * 6, new TextEffect(std::string("DANCE"), vec3(1), 0.5)},
        {73670 + 200 * 7, new TextEffect(std::string("LOUD"), vec3(1), 0.5)},
        {73670 + 200 * 8, new TextEffect(std::string("REAL"), vec3(1), 0.5)},
        {73670 + 200 * 9, new TextEffect(std::string("TIME"), vec3(1), 0.5)},
        {73670 + 200 * 10, new TextEffect(std::string("GOGO"), vec3(1), 0.5)},
        {73670 + 200 * 11, new TextEffect(std::string("GADGET"), vec3(1), 0.5)},
        {73670 + 200 * 12, new TextEffect(std::string("SUPER"), vec3(1), 0.5)},
        {73670 + 200 * 10, new TextEffect(std::string("OPTIMIZE"), vec3(1), 0.5)},
        {73670 + 200 * 11, new TextEffect(std::string("LIGHT"), vec3(1), 0.5)},
        {73670 + 200 * 12, new TextEffect(std::string("SPEED"), vec3(1), 0.5)},
        {73670 + 200 * 13, new TextEffect(std::string("ZERO"), vec3(1), 0.5)},
        {73670 + 200 * 14, new TextEffect(std::string("LATENCY"), vec3(1), 0.5)},


        {76670, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {77030, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {77470, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {77910, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {78350, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {78790, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {79230, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {79670, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {80110, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {80550, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {80990, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {81430, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {81870, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {82310, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {82750, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {83190, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {83630, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {84070, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {84470, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {84910, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {85350, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {85830, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {86270, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {86710, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {87150, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {87590, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {88030, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {88470, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {88910, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {89350, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {89750, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {90190, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {90630, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {91070, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},


        {91510, new GhostEffect(vec3(0,1,0), vec3(0,0.3,0.7f), 4)},                         // Jump
        {105550, new DisturbEffect(200.0f, vec3(0.5,1,0.5), audioCapture)},                 // Glichy
        {138070, new PlasmaEffect(1000)},                                                   // Tubuloc
        {154750, new GhostEffect(vec3(0,1,0), vec3(0.2f,0.2f,0.5f), 4)},
        {175590, new PlasmaEffect(1000)},                                                   // outline

        {203310, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {203750, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {204190, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {204630, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {205070, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {205510, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {205950, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {206390, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {206830, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {207270, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {207710, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {208150, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {208590, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {209030, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {209470, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {209910, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {210350, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {210790, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {211230, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {211670, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {212110, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {212550, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {212990, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {213430, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {213870, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {214310, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {214750, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {215190, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {215630, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {216070, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},
        {216510, new InnerCircleEffect(vec3(0,0,1), vec3(1,0,0), 64.f, 1000, -8.0f)},
        {216870, new InnerCircleEffect(vec3(1,1,0), vec3(0,1,1), 64.f, 1000, -8.0f)},

        {217630, new LineEffect(0.1f, vec3(1), vec3(0.25f), vec3(0,0,1), audioCapture)},

     });

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
    delete audioCapture;
    return 0;
}