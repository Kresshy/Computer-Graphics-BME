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
 
const int MAX_SIZE = 102;
 
float Xmax = 0 , Xmin = 0, Ymax = 0, Ymin = 0;
float leftX, leftY, zoomX, zoomY;
 
float coordLeft = 100;
float coordRight = 500;
float coordBottom = 100;
float coordTop = 500;
 
int right_button_pressed = 0;
Vector right_first, right_second;
 
int n = 0;
 
bool kkSplineSelected = false;
bool crSplineSelected = false;
 
int calculateCodes(Vector p) {
 
    int code = 0;
 
    if (p.y > Ymax)
        code += 8;
    else if (p.y < Ymin)
        code += 2; 
 
    if (p.x > Xmax)
        code += 4; 
    else if (p.x < Xmin)
        code += 1; 
 
    return code;
}
 
bool cohenClipper(Vector point0, Vector point1) {
 
    int code0, code1;
 
    for(;;) {
 
        code0 = calculateCodes(point0);
        code1 = calculateCodes(point1);
 
        if ((code0 & code1) != 0) return false;
        if ((code0 == 0) && (code1 == 0)) return true;
 
        if ((code0 & 8) != 0) {
            point0.x += (point1.x - point0.x) * (Ymax - point0.y) / (point1.y - point0.y);
            point0.y = Ymax;
        } else  if ((code0 & 4) != 0) {
            point0.y += (point1.y - point0.y) * (Xmax - point0.x) / (point1.x - point0.x);
            point0.x = Xmax;
        } else if ((code0 & 2) != 0) {
            point0.x += (point1.x - point0.x) * (Ymin - point0.y) / (point1.y - point0.y);
            point0.y = Ymin;
        } else if ((code0 & 1) != 0) {
            point0.y += (point1.y - point0.y) * (Xmin - point0.x) / (point1.x - point0.x);
            point0.x = Xmin;
        }
 
         if ((code1 & 8) != 0) {
            point1.x += (point0.x - point1.x) * (Ymax - point1.y) / (point0.y - point1.y);
            point1.y = Ymax;
        } else if ((code1 & 4) != 0) {
            point1.y += (point0.y - point1.y) * (Xmax - point1.x) / (point0.x - point1.x);
            point1.x = Xmax;
        } else if ((code1 & 2) != 0) {
            point1.x += (point0.x - point1.x) * (Ymin - point1.y) / (point0.y - point1.y);
            point1.y = Ymin;
        } if ((code1 & 1) != 0) {
            point1.y += (point0.y - point1.y) * (Xmin - point1.x) / (point0.x - point1.x);
            point1.x = Xmin;
        } 
 
    }
}
 
class KK_Spline {
 
public:
 
    Vector points[MAX_SIZE];
    int param[MAX_SIZE];
    int size;
 
    KK_Spline() {
 
        for(int i = 0; i < MAX_SIZE; i++){
            points[i] = Vector(0, 0, 0);
            param[i] = 0;
        }
 
        size = 0;
    }
 
    void drawCurve() {
 
        glColor3f(1, 0, 0);        
        glBegin(GL_LINE_STRIP);
 
        for(int j = 0; j < size - 2; j++) {
 
            Vector point1 = points[j];
            Vector point2 = points[j+1];
            Vector point3 = points[j+2];
            Vector point4 = points[j+3];
 
            int param1 = param[j];
            int param2 = param[j+1];
            int param3 = param[j+2];
            int param4 = param[j+3];
 
            float segmentStep = 1000.0 / (size - 2);
 
            float step = ((param3-param2) / segmentStep);
 
            for(float t = param2; t <= param3; t+= step) {    
 
                float p1u = ((t - param2)*(t - param3))/((param1 - param2)*(param1 - param3));    
                float p2u = ((t - param1)*(t - param3))/((param2 - param1)*(param2 - param3));    
                float p3u = ((t - param1)*(t - param2))/((param3 - param1)*(param3 - param2));    
 
                float p2d = ((t - param3)*(t - param4))/((param2 - param3)*(param2 - param4));    
                float p3d = ((t - param2)*(t - param4))/((param3 - param2)*(param3 - param4));
                float p4d = ((t - param2)*(t - param3))/((param4 - param2)*(param4 - param3));    
 
                Vector drawVectorUp = point1*p1u + point2*p2u + point3*p3u;
                Vector drawVectorDown = point2*p2d + point3*p3d + point4*p4d;        
 
                Vector drawVector = drawVectorDown*((t - param2) / (param3 - param2)) + drawVectorUp*((param3 - t) / (param3 - param2));  
 
                glVertex2f((drawVector.x - 500.0) * (1.0/500.0),(500.0 - drawVector.y)  * (1.0/500.0) * (-1));                                
 
            }
 
        }
        glEnd();
    }
 
    bool selectSpline() {
 
        for(int j = 0; j < size - 2; j++) {
 
            Vector point1 = points[j];
            Vector point2 = points[j+1];
            Vector point3 = points[j+2];
            Vector point4 = points[j+3];
 
            int param1 = param[j];
            int param2 = param[j+1];
            int param3 = param[j+2];
            int param4 = param[j+3];
 
            float segmentStep = 1000.0 / (size - 2);
 
            float step = ((param3-param2) / segmentStep);
 
            for(float t = param2; t <= param3; t+= step) {    
 
                float p1u = ((t - param2)*(t - param3))/((param1 - param2)*(param1 - param3));    
                float p2u = ((t - param1)*(t - param3))/((param2 - param1)*(param2 - param3));    
                float p3u = ((t - param1)*(t - param2))/((param3 - param1)*(param3 - param2));    
 
                float p2d = ((t - param3)*(t - param4))/((param2 - param3)*(param2 - param4));    
                float p3d = ((t - param2)*(t - param4))/((param3 - param2)*(param3 - param4));
                float p4d = ((t - param2)*(t - param3))/((param4 - param2)*(param4 - param3));    
 
                float p11u = ((t + step - param2)*(t + step - param3))/((param1 - param2)*(param1 - param3));        
                float p21u = ((t + step - param1)*(t + step - param3))/((param2 - param1)*(param2 - param3));        
                float p31u = ((t + step - param1)*(t + step - param2))/((param3 - param1)*(param3 - param2));    
 
                float p21d = ((t + step - param3)*(t + step - param4))/((param2 - param3)*(param2 - param4));        
                float p31d = ((t + step - param2)*(t + step - param4))/((param3 - param2)*(param3 - param4));
                float p41d = ((t + step - param2)*(t + step - param3))/((param4 - param2)*(param4 - param3));    
 
                Vector drawVectorUp = point1*p1u + point2*p2u + point3*p3u;
                Vector drawVectorDown = point2*p2d + point3*p3d + point4*p4d;        
 
                Vector drawVectorUp1 = point1*p11u + point2*p21u + point3*p31u;
                Vector drawVectorDown1 = point2*p21d + point3*p31d + point4*p41d;    
 
                Vector point_first = drawVectorUp*((param3 - t) / (param3 - param2)) + drawVectorDown*((t - param2) / (param3 - param2));                      
                Vector point_second = drawVectorUp1*((param3 - t) / (param3 - param2)) + drawVectorDown1*((t - param2) / (param3 - param2));
 
                if(cohenClipper(point_first, point_second)) return true;
 
            }
        }
 
        return false;
    }
 
};
 
class CR_Spline {
 
public:
 
    Vector points[MAX_SIZE];
    int param[MAX_SIZE];
    int size;
 
    CR_Spline() {
 
        for(int i = 0; i < MAX_SIZE; i++){
            points[i] = Vector(0, 0, 0);
            param[i] = 0;
        }
 
        size = 0;
    }
    
    void drawCurve() {
 
        glColor3f(0, 1, 0);
        glBegin(GL_LINE_STRIP);
 
        for (int j = 0; j < size - 2; j++) {
 
            float param0 = param[j];
            float param1 = param[j+1];
            float param2 = param[j+2];
            float param3 = param[j+3];
 
            Vector point_0 = points[j];
            Vector point_1 = points[j+1];
            Vector point_2 = points[j+2];
            Vector point_3 = points[j+3];
 
            Vector drawVector;
 
            float step = ((param2-param1) / (1000.0 / (size - 2)));
 
            for(float t = param1; t <= param2; t += step) {
 
                float param10 = param1 - param0;
                float param21 = param2 - param1;
                float param32 = param3 - param2;
 
                Vector v_1 = (((point_1 - point_0) * (1 / (param10))) + ((point_2 - point_1) * (1 / (param21)))) * (1.0/2.0);
                Vector v_2 = (((point_2 - point_1) * (1 / (param21))) + ((point_3 - point_2) * (1 / (param32)))) * (1.0/2.0);
 
                Vector a0 = point_1;
                Vector a1 = v_1;
 
                Vector a2 = ((point_2 - point_1) * 3) * (1 / pow(param21, 2)) - (v_2 + v_1 * 2) * (1 / (param21));
                Vector a3 = (v_2 + v_1) * (1 / pow(param21, 2)) - ((point_2 - point_1) * 2) * (1 / pow(param21, 3));
 
                drawVector =  a3*pow(t - param1, 3) + a2*pow(t - param1, 2) + a1*(t - param1) + a0;
 
                glVertex2f((drawVector.x - 500.0) * (1.0/500.0),(500.0 - drawVector.y)  * (1.0/500.0) * (-1));
 
            }
        }
        glEnd();
 
    }
 
    bool selectSpline() {
 
        for (int j = 0; j < size - 2; j++) {
 
            float param0 = param[j];
            float param1 = param[j+1];
            float param2 = param[j+2];
            float param3 = param[j+3];
 
            Vector point_0 = points[j];
            Vector point_1 = points[j+1];
            Vector point_2 = points[j+2];
            Vector point_3 = points[j+3];
 
            Vector firstVector;
            Vector secondVector;
 
            float step = ((param2-param1) / (1000 / (size - 2)));
 
            for(float t = param1; t <= param2; t += step) {
 
                float param10 = param1 - param0;
                float param21 = param2 - param1;
                float param32 = param3 - param2;
 
                Vector v_1 = (((point_1 - point_0) * (1 / (param10))) + ((point_2 - point_1) * (1 / (param21)))) * (1.0/2.0);
                Vector v_2 = (((point_2 - point_1) * (1 / (param21))) + ((point_3 - point_2) * (1 / (param32)))) * (1.0/2.0);
 
                Vector a0 = point_1;
                Vector a1 = v_1;
 
                Vector a2 = ((point_2 - point_1) * 3) * (1 / pow(param21, 2)) - (v_2 + v_1 * 2) * (1 / (param21));
                Vector a3 = (v_2 + v_1) * (1 / pow(param21, 2)) - ((point_2 - point_1) * 2) * (1 / pow(param21, 3));
 
                firstVector =  a3*pow(t - param1, 3) + a2*pow(t - param1, 2) + a1*(t - param1) + a0;
                secondVector =  a3*pow(t + step - param1, 3) + a2*pow(t + step - param1, 2) + a1*(t + step - param1) + a0;
 
                if(cohenClipper(firstVector, secondVector)) return true;
            }
        }
        return false;
    }
 
};
 
KK_Spline kkSpline;
CR_Spline crSpline;
 
void drawKnotPoint(Vector center) {
 
    center.x = center.x - 500;
    center.y = (500 - center.y) * (-1);
 
    center = center * (1.0/500.0);
 
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLE_STRIP);
 
    glVertex2f(center.x - 0.005, center.y + 0.005);
    glVertex2f(center.x - 0.005, center.y - 0.005);
    glVertex2f(center.x + 0.005, center.y + 0.005);
    glVertex2f(center.x + 0.005, center.y - 0.005);    
 
    glEnd();
}
 
float mousePosTransformX(float x){
 
    float scale = coordRight - coordLeft;
    float scaledX = x/600.0 * scale + coordLeft;
 
    return scaledX;
}
 
float mousePosTransformY(float y){
 
    float scale = coordTop - coordBottom;
    float scaledY = (600.0 - y)/600.0 * scale + coordBottom;
 
    return scaledY;
}
 
const int screenWidth = 600;    // alkalmazás ablak felbontása
const int screenHeight = 600;
 
Color image[screenWidth*screenHeight];    // egy alkalmazás ablaknyi kép
 
 
// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
    glViewport(0, 0, screenWidth, screenHeight);
 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(((coordLeft - 500.0) / 500.0),
        ((coordRight - 500.0) / 500.0),
        ((coordBottom - 500.0) / 500.0),
        ((coordTop - 500.0) / 500.0));
 
    for(int Y = 0; Y < screenHeight; Y++)
        for(int X = 0; X < screenWidth; X++)
            image[Y*screenWidth + X] = Color(0, 0, 0);
 
}
 
// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);        // torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
 
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
 
    if ( n >= 2 ){
        crSpline.drawCurve();
    }
    
    if ( n >= 2 ){
        kkSpline.drawCurve();
    }
 
    for (int i = 0; i < n; i++){
        drawKnotPoint(crSpline.points[i]);
        drawKnotPoint(kkSpline.points[i]);
    }
 
    glutSwapBuffers();                     // Buffercsere: rajzolas vege
 
}
 
// Billentyuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
 
    if (key == 'z') {
 
        zoomX = mousePosTransformX(x);
        zoomY = mousePosTransformY(y);
 
        float leftFromClick = zoomX - coordLeft;
        float rightFromClick = coordRight - zoomX;
 
        float downFromClick = zoomY - coordBottom;
        float upFromClick = coordTop - zoomY;
 
        leftFromClick /= 10.0;
        rightFromClick /= 10.0;
        downFromClick /= 10.0;
        upFromClick /= 10.0;
 
        coordLeft = zoomX - leftFromClick;
        coordRight = zoomX + rightFromClick;
        coordBottom = zoomY - downFromClick;
        coordTop = zoomY + upFromClick;
 
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
 
        gluOrtho2D(((coordLeft - 500.0) / 500.0),
            ((coordRight - 500.0) / 500.0),
            ((coordBottom - 500.0) / 500.0),
            ((coordTop - 500.0) / 500.0));
 
        glutPostRedisplay();
 
    }
 
    if (key == 'Z') {
 
        zoomX = mousePosTransformX(x);
        zoomY = mousePosTransformY(y);
 
        float leftFromClick = zoomX - coordLeft;
        float rightFromClick = coordRight - zoomX;
 
        float downFromClick = zoomY - coordBottom;
        float upFromClick = coordTop - zoomY;
 
        leftFromClick *= 10.0;
        rightFromClick *= 10.0;
        downFromClick *= 10.0;
        upFromClick *= 10.0;
 
        coordLeft = zoomX - leftFromClick;
        coordRight = zoomX + rightFromClick;
        coordBottom = zoomY - downFromClick;
        coordTop = zoomY + upFromClick;
 
        if (coordLeft < 0 || coordRight > 1000 || coordBottom < 0 || coordTop > 1000) {
 
            coordLeft = 0.0;
            coordRight = 1000.0;
            coordBottom = 0.0;
            coordTop = 1000.0;
 
        }
 
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
 
        gluOrtho2D(((coordLeft - 500.0) / 500.0),
            ((coordRight - 500.0) / 500.0),
            ((coordBottom - 500.0) / 500.0),
            ((coordTop - 500.0) / 500.0));
 
        glutPostRedisplay();
 
    }
 
}
 
 
float pixel() {
    return ((2.5/600.0) * (coordRight-coordLeft)); 
}
 
// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
 
        leftX = mousePosTransformX(x);
        leftY = mousePosTransformY(y);
 
        if (n < 101) {
 
            if (n == 0) {
 
                kkSpline.points[n] = Vector(leftX, leftY, 0);
                kkSpline.points[n+1] = Vector(leftX, leftY, 0);
 
                kkSpline.param[n] = 0;
                kkSpline.param[n+1] = 1;
 
                crSpline.points[n] = Vector(leftX, leftY, 0);
                crSpline.points[n+1] = Vector(leftX, leftY, 0);
 
                crSpline.param[n] = 0;
                crSpline.param[n+1] = 1;
 
                n += 2;
                kkSpline.size = n;
                crSpline.size = n;
 
            } else {
 
                kkSpline.points[n] = Vector(leftX, leftY, 0);
                kkSpline.points[n+1] = Vector(leftX, leftY, 0);
 
                kkSpline.param[n] = kkSpline.param[n-1] + n;
                kkSpline.param[n+1] = kkSpline.param[n-1] + n + 1;
 
                crSpline.points[n] = Vector(leftX, leftY, 0);
                crSpline.points[n+1] = Vector(leftX, leftY, 0);
 
                crSpline.param[n] = kkSpline.param[n-1] + n;
                crSpline.param[n+1] = kkSpline.param[n-1] + n + 1;
 
                n++;
                kkSpline.size = n;
                crSpline.size = n;
 
            }
        }
 
        glutPostRedisplay();
    }
 
   if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
 
        if (right_button_pressed == 0) {
 
            right_first.x = mousePosTransformX(x);
            right_first.y = mousePosTransformY(y);
 
            right_button_pressed++;
 
            Xmax = right_first.x + pixel();
            Xmin = right_first.x - pixel();
            Ymax = right_first.y + pixel();
            Ymin = right_first.y - pixel();
 
            kkSplineSelected = kkSpline.selectSpline();
            crSplineSelected = crSpline.selectSpline();
 
        } else if ( right_button_pressed == 1) {
 
            right_second.x = mousePosTransformX(x);
            right_second.y = mousePosTransformY(y);
            right_button_pressed = 0;
 
            Vector move = right_second - right_first;
 
            if(kkSplineSelected && crSplineSelected) {
 
                for(int i = 0; i < n+1; i++) {
                    kkSpline.points[i] = kkSpline.points[i] + move;
                }
 
            } 
 
            if(kkSplineSelected && !crSplineSelected) {
 
                for(int i = 0; i < n+1; i++) {
                    kkSpline.points[i] = kkSpline.points[i] + move;
                }
 
            } 
 
            if(!kkSplineSelected && crSplineSelected) {
 
                for(int i = 0; i < n+1; i++) {
                    crSpline.points[i] = crSpline.points[i] + move;
                }
 
            }
        }
 
        glutPostRedisplay();
    }
}
 
void onIdle( ) {
     long time = glutGet(GLUT_ELAPSED_TIME);        // program inditasa ota eltelt ido
 
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