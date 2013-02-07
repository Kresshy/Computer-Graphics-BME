//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2012-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : VÁRADI SZABOLCS
// Neptun : IFGP0W
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
 
#include <math.h>
#include <stdlib.h>
 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// MsWindows-on ez is kell
#include <windows.h>     
#endif // Win32 platform
 
#include <GL/gl.h>
#include <GL/glu.h>
// A GLUT-ot le kell tolteni: http://www.opengl.org/resources/libraries/glut/
#include <GL/glut.h>     
 
 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...
 
//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
    float x, y, z;
 
    Vector( ) { 
        x = y = z = 0;
    }
    Vector(float x0, float y0, float z0 = 0) { 
        x = x0; y = y0; z = z0;
    }
    Vector operator*(float a) { 
        return Vector(x * a, y * a, z * a); 
    }
    Vector operator+(const Vector& v) {
        return Vector(x + v.x, y + v.y, z + v.z); 
    }
    Vector operator-(const Vector& v) {
        return Vector(x - v.x, y - v.y, z - v.z); 
    }
    float operator*(const Vector& v) {     // dot product
        return (x * v.x + y * v.y + z * v.z); 
    }
    Vector operator%(const Vector& v) {     // cross product
        return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
    float Length() { return sqrt(x * x + y * y + z * z); }
};
 
//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
    float r, g, b;
 
    Color( ) { 
        r = g = b = 0;
    }
    Color(float r0, float g0, float b0) { 
        r = r0; g = g0; b = b0;
    }
    Color operator*(float a) { 
        return Color(r * a, g * a, b * a); 
    }
    Color operator*(const Color& c) { 
        return Color(r * c.r, g * c.g, b * c.b); 
    }
    Color operator+(const Color& c) {
        return Color(r + c.r, g + c.g, b + c.b); 
    }
};
 
const int screenWidth = 600;    
const int screenHeight = 600;
const double PI = 3.14;
const int V = 100;
 
float hx = -2500, hy = 2500, gx = 1500, gy = 2000, vechX = -1000, vechY = 2000, vecgX = 2500, vecgY = 1100;
float tx = 0, ty = 0;
double visiblePercent = 0;
 
float rightX, rightY, leftX, leftY = 0;
 
long startTime, oldTime, visibleTime, deltaTime = 0;
 
float gradH, gradG;
float speedH = 0, speedG = 0;
bool visibleH = false, visibleG = false;
 
Color image[screenWidth*screenHeight];
 
// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
    glViewport(0, 0, screenWidth, screenHeight);
 
    float terrain;
 
    for(int Y = 0; Y < screenHeight; Y++)
        for(int X = 0; X < screenWidth; X++) {
 
            terrain = (sin(0.02*(X-300)+0.02*(Y-300)) * cos(0.02*(X-300)) + sin(0.02*(Y-300)) +2)/4;
 
 
            image[Y*screenWidth + X] = Color(terrain, terrain, terrain);
        }
 
 
        float normalizeVec = sqrt(pow(vechX,2) + pow(vechY,2));
        vechX /= normalizeVec;
        vechY /= normalizeVec;
 
        normalizeVec = sqrt(pow(vecgX,2) + pow(vecgY,2));
        vecgX /= normalizeVec;
        vecgY /= normalizeVec;
}
 
void drawGretel(float centerX, float centerY) {
 
    glColor3f(0.737255, 0.560784, 0.560784);
 
    centerX /= 5000;
    centerY /= 5000;
 
    glBegin(GL_LINE_STRIP);
 
    float x;
    float y;
    float Angle;
 
    for( int i=0; i<=100; i++ ) {
 
        Angle = i * (2.0*PI/100);
        x = cos (Angle) * 0.05;
        y = sin (Angle) * 0.05;
        glVertex2f(x + centerX, y + centerY);
 
    }
 
    glEnd( );
    glBegin(GL_LINE_STRIP);
 
    x = cos (75 * (2.0*PI/100)) * 0.05;
    y = sin (75 * (2.0*PI/100)) * 0.05;
    glVertex2f(x + centerX, y + centerY);
    glVertex2f(x + centerX, y- 0.10 + centerY);
 
    glEnd( );
    glBegin(GL_LINE_STRIP);
 
    x = cos (75 * (2.0*PI/100)) * 0.05;
    y = sin (75 * (2.0*PI/100)) * 0.05;
    glVertex2f(x- 0.05 + centerX, y - 0.05 + centerY);
    glVertex2f(x + 0.05 + centerX, y - 0.05 + centerY);
 
    glEnd( );
 
}
 
void drawHansel(float centerX, float centerY){
 
    glColor3f(0.196078,0.6,0.8);
 
    centerX /= 5000;
    centerY /= 5000;
 
    float x;
    float y;
    float Angle;
 
    glBegin(GL_LINE_STRIP);
 
    for( int i=0; i<=100; i++ ) {
 
        Angle = i * (2.0*PI/100);
        x = cos (Angle) * 0.05;
        y = sin (Angle) * 0.05;
        glVertex2f(x + centerX, y + centerY);
 
    }    
    glEnd( );
    glBegin(GL_LINE_STRIP);
 
    Angle = 45 * (2.0*PI/360);
    x = cos (Angle) * 0.05;
    y = sin (Angle) * 0.05;
    glVertex2f(x + centerX, y + centerY);
    x = cos (Angle) * 0.05 + 0.05;
    y = sin (Angle) * 0.05 + 0.05;
    glVertex2f(x + centerX, y + centerY);
 
    glEnd( );
    glBegin(GL_LINE_STRIP);
 
    Angle = 45 * (2.0*PI/360);
    x = cos (Angle) * 0.05;
    y = sin (Angle) * 0.05 + 0.05;
    glVertex2f(x + centerX, y + centerY);
    x = cos (Angle) * 0.05 + 0.05;
    y = sin (Angle) * 0.05 + 0.05;
    glVertex2f(x + centerX, y + centerY);
    x = cos (Angle) * 0.05 + 0.05;
    y = sin (Angle) * 0.05;
    glVertex2f(x + centerX, y + centerY);
 
    glEnd( );
}
 
void drawTower(float centerX, float centerY) {
 
    centerX /= 5000;
    centerY /= 5000;
 
    glColor3f(1, 1, 0);
    glBegin(GL_TRIANGLES);    
 
    glVertex2f(0.0 + centerX, 0.05 + centerY);
    glVertex2f(0.05 + centerX, -0.05 + centerY);
    glVertex2f(-0.05 + centerX, -0.05 + centerY);
 
    glEnd();
 
}
 
void drawProgressBar(double percent) {
 
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(-0.98, 0.98);
    glVertex2f(-0.98, 0.92);
    glVertex2f(0.98, 0.98);
    glVertex2f(0.98, 0.92);
    glEnd();
 
    float length = (-0.98) + (0.0196*percent);
 
    glColor3f(1, 0, 1);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(-0.98, 0.97);
    glVertex2f(-0.98, 0.93);
    glVertex2f(length, 0.97);
    glVertex2f(length, 0.93);
    glEnd();
 
}
 
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
 
    x1 /= 5000;
    y1 /= 5000;
    x2 /= 5000;
    y2 /= 5000;
    x3 /= 5000;
    y3 /= 5000;
 
    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1,y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
 
}
void calculateGradient(float hX, float hY, float gX, float gY) {
 
    float dhX,dhY,dgX,dgY;
 
    dhX = 3.82*cos(0.02*hX)*cos(0.02*hX+0.02*hY)-3.82*sin(0.02*hX)*sin(0.02*hX+0.02*hY);
    dhY = 3.82*cos(0.02*hX)*cos(0.02*hX+0.02*hY)+3.82*cos(0.02*hY);
    dgX = 3.82*cos(0.02*gX)*cos(0.02*gX+0.02*gY)-3.82*sin(0.02*gX)*sin(0.02*gX+0.02*gY);
    dgY = 3.82*cos(0.02*gX)*cos(0.02*gX+0.02*gY)+3.82*cos(0.02*gY);
 
    gradH = dhX * vechX + dhY * vechY;
    gradG = dgX * vecgX + dgY * vecgY;
 
}
 
float calculateHeight(float x, float y) {
    return    ((sin(0.02*(x)+0.02*(y)) * cos(0.02*(x)) + sin(0.02*(y)))*191) + 632;
}
 
void calculateVisibility(float tX, float tY, float hX, float hY, float gX, float gY) {
 
    const float scale = 300.0/5000.0;
 
    tX *= scale;
    tY *= scale;
    hX *= scale;
    hY *= scale;
    gX *= scale;
    gY *= scale;
 
    float vectorhX, vectorhY, vectorhZ, vectorgX, vectorgY, vectorgZ;
 
    float tZ = ((sin(0.02*(tX)+0.02*(tY)) * cos(0.02*(tX)) + sin(0.02*(tY)))*191) + 632 + 20;
    float hZ = ((sin(0.02*(hX)+0.02*(hY)) * cos(0.02*(hX)) + sin(0.02*(hY)))*191) + 632;
    float gZ = ((sin(0.02*(gX)+0.02*(gY)) * cos(0.02*(gX)) + sin(0.02*(gY)))*191) + 632;
 
    vectorhX = tX - hX;
    vectorhY = tY - hY;
    vectorhZ = tZ - hZ;
 
    vectorgX = tX - gX;
    vectorgY = tY - gY;
    vectorgZ = tZ - gZ;
 
    float normalize = sqrt(pow(vectorhX,2)+pow(vectorhY,2)+pow(vectorhZ,2));
 
    vectorhX /= normalize;
    vectorhY /= normalize;
    vectorhZ /= normalize;
 
    normalize = sqrt(pow(vectorgX,2)+pow(vectorgY,2)+pow(vectorgZ,2));
 
    vectorgX /= normalize;
    vectorgY /= normalize;
    vectorgZ /= normalize;
 
    float distanceH = sqrt(pow(tX - hX, 2) + pow(tY - hY, 2) + pow(tZ - hZ, 2));
    float distanceG = sqrt(pow(tX - gX, 2) + pow(tY - gY, 2) + pow(tZ - gZ, 2));
 
    float mhX = hX, mhY = hY, mhZ = hZ, mgX = gX, mgY = gY, mgZ = gZ;
 
    float step = 100 * scale;
 
    for(int i = 0; i < distanceH; i+=step) {
        if ( calculateHeight(mhX, mhY) > mhZ) { visibleH = false; return; }
        mhX += vectorhX*step;
        mhY += vectorhY*step;
        mhZ += vectorhZ*step;
    }
 
    for(int i = 0; i < distanceG; i+=step) {
        if ( calculateHeight(mgX, mgY) > mgZ) { visibleG = false; return; }
        mgX += vectorgX*step;
        mgY += vectorgY*step;
        mgZ += vectorgZ*step;
    }
 
    visibleH = true;
    visibleG = true;
 
}
 
 
// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
 
    drawHansel(hx, hy);
    drawGretel(gx, gy);
    drawTower(tx, ty);
    if ( visibleG && visibleH ) drawTriangle(hx, hy, gx, gy, tx, ty);
    drawProgressBar(visiblePercent);
 
 
    glutSwapBuffers();                 
 
}
 
 
// Billentyuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
 
    if (key =='t') {
        tx = (2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f)*5000.0;
        ty = (2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f)*5000.0;
        glutPostRedisplay();
    }
 
}
 
// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
 
        leftX = (float)(x - 300) / 300; leftY=(float)(300 - y) / 300;
        leftX *= 5000; leftY *= 5000;
 
        vechX = (leftX - hx);
        vechY = (leftY - hy);
 
        float normalizeVec = sqrt(pow(vechX,2) + pow(vechY,2));
        vechX /= normalizeVec;
        vechY /= normalizeVec;
 
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
 
        rightX = (float)(x - 300) / 300; rightY =(float)(300 - y) / 300;
        rightX *= 5000; rightY *= 5000;
 
        vecgX = (rightX - gx);
        vecgY = (rightY - gy);
 
        float normalizeVec = sqrt(pow(vecgX,2) + pow(vecgY,2));
        vecgX /= normalizeVec;
        vecgY /= normalizeVec;
    }
}
 
 
// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
 
    startTime = glutGet(GLUT_ELAPSED_TIME);    
    deltaTime = startTime - oldTime;
    oldTime = startTime;
 
    if ( deltaTime >= 100 ) {
 
        for ( int i = 100; i <= deltaTime; i+=100) {
 
            calculateGradient((hx*(300.0/5000.0)),(hy*(300.0/5000.0)),(gx*(300.0/5000.0)),(gy*(300.0/5000.0)));
 
            float MH = 180.0/PI * atan(gradH);
            float MG = 180.0/PI * atan(gradG);
 
            speedH = V * ( 1 - MH/90) * cos(atan(gradH));
            speedG = V * ( 1 - MG/90) * cos(atan(gradG));
 
            if (hx + vechX * speedH >= 5000) { vechX *= -1; }
            if (hx + vechX * speedH <= -5000) { vechX *= -1; }
            if (hy + vechY * speedH >= 5000) { vechY *= -1; }
            if (hy + vechY * speedH <= -5000) { vechY *= -1; }
 
            if (gx  + vecgX * speedG >= 5000) { vecgX *= -1; }
            if (gx  + vecgX * speedG <= -5000) { vecgX *= -1; }
            if (gy  + vecgY * speedG >= 5000) { vecgY *= -1; }
            if (gy  + vecgY * speedG <= -5000) { vecgY *= -1; }
 
            hx += vechX * speedH;
            hy += vechY * speedH;
 
            gx += vecgX * speedG;
            gy += vecgY * speedG;
 
            calculateVisibility(tx, ty, hx, hy, gx, gy);
 
            if (visibleG && visibleH) visibleTime += deltaTime;
 
            visiblePercent = (double)visibleTime/(double)startTime * 100;
 
            deltaTime -= 100;
            glutPostRedisplay();
 
        } 
    } 
 
    if (deltaTime < 100 ) {
 
        for ( int i = 1; i <= deltaTime; i+=deltaTime) {
 
            calculateGradient((hx*(300.0/5000.0)),(hy*(300.0/5000.0)),(gx*(300.0/5000.0)),(gy*(300.0/5000.0)));
 
            float MH = 180.0/PI * atan(gradH);
            float MG = 180.0/PI * atan(gradG);
 
            speedH = V * ( 1 - MH/90) * deltaTime/100 * cos(atan(gradH));
            speedG = V * ( 1 - MG/90) * deltaTime/100 * cos(atan(gradG));
 
            if (hx + vechX * speedH >= 5000) { vechX *= -1; }
            if (hx + vechX * speedH <= -5000) { vechX *= -1; }
            if (hy + vechY * speedH >= 5000) { vechY *= -1; }
            if (hy + vechY * speedH <= -5000) { vechY *= -1; }
 
            hx += vechX * speedH;
            hy += vechY * speedH;
 
            if (gx  + vecgX * speedG >= 5000) { vecgX *= -1; }
            if (gx  + vecgX * speedG <= -5000) { vecgX *= -1; }
            if (gy  + vecgY * speedG >= 5000) { vecgY *= -1; }
            if (gy  + vecgY * speedG <= -5000) { vecgY *= -1; }
 
            gx += vecgX * speedG;
            gy += vecgY * speedG;
 
            calculateVisibility(tx, ty, hx, hy, gx, gy);
 
            if (visibleG && visibleH) visibleTime += deltaTime;
 
            visiblePercent = (double)visibleTime/(double)startTime * 100;
 
            deltaTime -= deltaTime;
            glutPostRedisplay();
 
        }
    }
}
 
// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv);                 // GLUT inicializalasa
    glutInitWindowSize(600, 600);            // Alkalmazas ablak kezdeti merete 600x600 pixel 
    glutInitWindowPosition(100, 100);            // Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);    // 8 bites R,G,B,A + dupla buffer + melyseg buffer
 
    glutCreateWindow("Grafika hazi feladat");        // Alkalmazas ablak megszuletik es megjelenik a kepernyon
 
    glMatrixMode(GL_MODELVIEW);                // A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);            // A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
 
    onInitialization();                    // Az altalad irt inicializalast lefuttatjuk
 
    glutDisplayFunc(onDisplay);                // Esemenykezelok regisztralasa
    glutMouseFunc(onMouse); 
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
 
    glutMainLoop();                    // Esemenykezelo hurok
 
    return 0;
}