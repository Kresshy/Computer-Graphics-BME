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
 
   Vector(float x0, float y0, float z0 = 0.0) { 
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
 
   float operator*(const Vector& v) {     
    return (x * v.x + y * v.y + z * v.z); 
   }
 
   Vector operator%(const Vector& v) {     
    return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
 
   Vector operator/( float f ) {          
       return Vector( x/f, y/f, z/f ); 
   }
 
   void operator+=( const Vector& v ) {  
       x += v.x; y += v.y; z += v.z; 
   }
   void operator-=( const Vector& v ) {  
       x -= v.x; y -= v.y; z -= v.z; 
   }
   void operator*=( float f ) {          
       x *= f; y *= f; z *= f; 
   }
 
   float Length() { 
       return sqrt(x * x + y * y + z * z); }
 
   Vector Normalize() {
        float length = Length();
        x /= length;
        y /= length;
        z /= length;
        return *this;
   }
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
 
const float PI = 3.14159;
 
// FORRRÁSMEGJELÖLÉS: A STRUKTÚRA MEGVALÓSÍTÁSÁNÁL ALAPUL VETTEM A KÖNYV CD MELLÉKLETÉN TALÁLHATÓ QUATERNION OSZTÁLYT
struct Quaternion {           
 
    float s;    
    Vector d;    
 
public:
 
    Quaternion( ) : d(0, 0, 0) { s = 1; }
    Quaternion( float s0, float x0, float y0, float z0 ) : d(x0, y0, z0) { s = s0; }
    Quaternion( float s0, Vector d0 ) : d( d0 ) { s = s0; }
 
    Quaternion operator+(Quaternion q) {
        return Quaternion( s + q.s, d + q.d );
    }
 
    void operator+=(Quaternion q) {        
        s += q.s;
        d += q.d;
    }
 
    Quaternion operator*(float f) {        
        return Quaternion( s * f, d * f );
    }
 
    float operator*(Quaternion q) {        
        return (s * s + d * d);
    }
 
    void Normalize( ) {                        
        float length = (*this) * (*this);
        (*this) = (*this) * (1/sqrt(length));
    }
 
    Quaternion operator%(Quaternion q) { 
        return Quaternion( s * q.s - d * q.d, q.d * s + d * q.s + d % q.d );
    }
 
    float GetRotationAngle( ) {            
        float cosa2 = s;
        float sina2 = d.Length();
        float angRad = atan2( sina2, cosa2 ) * 2;
        return angRad * 180 / PI;
    }
 
    Vector GetAxis( ) { return d; }        
 
    Quaternion getInverse() {
        Quaternion num = Quaternion(s, (d * -1));
        float den = pow(sqrt(s*s + d.x * d.x + d.y * d.y + d.z * d.z), 2); 
        Quaternion ret = num * (1/den);
        return ret;
    }
 
};
 
const int screenWidth = 600;    
const int screenHeight = 600;
Vector arrowVector;
 
Color image[screenWidth*screenHeight];    
 
struct Triangle{
 
    Vector a, b, c;
    Vector n1, n2, n3;
 
    Triangle() {}
 
    Triangle(Vector aa, Vector bb, Vector cc) {
 
        a = aa;
        b = bb;
        c = cc;
 
        Vector n = (c-a)%(b-a);
        n.Normalize();
 
        n1 = n2 = n3 = n;
 
    }
 
    Triangle(Vector aa, Vector bb, Vector cc, Vector nn1, Vector nn2, Vector nn3) {
 
        a = aa;
        b = bb;
        c = cc;
        n1 = nn1;
        n2 = nn2;
        n3 = nn3;
 
    }
 
    void drawTriangle() {
 
            glNormal3f(n1.x, n1.y, n1.z);
            glVertex3f(a.x, a.y, a.z);
 
            glNormal3f(n2.x, n2.y, n2.z);
            glVertex3f(b.x, b.y, b.z);
        
            glNormal3f(n3.x, n3.y, n3.z);
            glVertex3f(c.x, c.y, c.z);
    }
};
 
struct Sphere{
 
    float radius;
    Triangle triangles[100*100*2];
    int db;
 
    Sphere() {
 
        radius = 1;
        db = 0;
        Vector p1, p2, p3, p4, n1, n2, n3, n4;
        float u = (2 * PI) / 100;
        float v = PI / 100;
 
        for ( int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
 
                p1.x = radius * sin(j*v) * cos(i*u);
                p1.y = radius * sin(j*v) * sin(i*u);
                p1.z = radius * cos(j*v);
                n1 = p1;
 
                p2.x = radius * sin((j+1)*v) * cos(i*u);
                p2.y = radius * sin((j+1)*v) * sin(i*u);
                p2.z = radius * cos((j+1)*v);
                n2 = p2;
 
                p3.x = radius * sin(j*v) * cos((i+1)*u);
                p3.y = radius * sin(j*v) * sin((i+1)*u);
                p3.z = radius * cos(j*v);
                n3 = p3;
 
                p4.x = radius * sin((j+1)*v) * cos((i+1)*u);
                p4.y = radius * sin((j+1)*v) * sin((i+1)*u);
                p4.z = radius * cos((j+1)*v);
                n4 = p4;
 
                triangles[db] = Triangle(p1, p2, p3, n1, n2, n3);
                db++;
                triangles[db] = Triangle(p3, p2, p4, n3, n2, n4);
                db++;
            }
        }
    }
 
    Sphere(float r) {
 
        radius = r;
        db = 0;
        Vector p1, p2, p3, p4, n1, n2, n3, n4;
        float u = (2 * PI) / 100;
        float v = PI / 100;
 
        for ( int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
 
                p1.x = radius * sin(j*v) * cos(i*u);
                p1.y = radius * sin(j*v) * sin(i*u);
                p1.z = radius * cos(j*v);
                n1 = p1;
                n1.Normalize();
 
                p2.x = radius * sin((j+1)*v) * cos(i*u);
                p2.y = radius * sin((j+1)*v) * sin(i*u);
                p2.z = radius * cos((j+1)*v);
                n2 = p2;
                n2.Normalize();
 
                p3.x = radius * sin(j*v) * cos((i+1)*u);
                p3.y = radius * sin(j*v) * sin((i+1)*u);
                p3.z = radius * cos(j*v);
                n3 = p3;
                n3.Normalize();
 
                p4.x = radius * sin((j+1)*v) * cos((i+1)*u);
                p4.y = radius * sin((j+1)*v) * sin((i+1)*u);
                p4.z = radius * cos((j+1)*v);
                n4 = p4;
                n4.Normalize();
 
                triangles[db] = Triangle(p1, p2, p3, n1, n2, n3);
                db++;
                triangles[db] = Triangle(p3, p2, p4, n3, n2, n4);
                db++;
 
            }
        }
    }
 
    void SphereNoFull(float r) {
 
        radius = r;
        db = 0;
        Vector p1, p2, p3, p4, n1, n2, n3, n4;
        float u = (2 * PI) / 100;
        float v = PI / 100;
 
        for ( int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
 
                p1.x = radius * sin(j*v) * cos(i*u);
                p1.y = radius * sin(j*v) * sin(i*u);
                p1.z = radius * cos(j*v);
                n1 = p1;
                n1.Normalize();
 
                p2.x = radius * sin((j+1)*v) * cos(i*u);
                p2.y = radius * sin((j+1)*v) * sin(i*u);
                p2.z = radius * cos((j+1)*v);
                n2 = p2;
                n2.Normalize();
 
                p3.x = radius * sin(j*v) * cos((i+1)*u);
                p3.y = radius * sin(j*v) * sin((i+1)*u);
                p3.z = radius * cos(j*v);
                n3 = p3;
                n3.Normalize();
 
                p4.x = radius * sin((j+1)*v) * cos((i+1)*u);
                p4.y = radius * sin((j+1)*v) * sin((i+1)*u);
                p4.z = radius * cos((j+1)*v);
                n4 = p4;
                n4.Normalize();
 
                if(!(p1.y > 0 && p1.x < 0)){
                    triangles[db] = Triangle(p1, p2, p3, n1, n2, n3);
                    db++;
                    triangles[db] = Triangle(p3, p2, p4, n3, n2, n4);
                    db++;
                }
 
            }
        }
    }
 
    void SphereWindow(float r) {
 
        radius = r;
        db = 0;
        Vector p1, p2, p3, p4, n1, n2, n3, n4;
        float u = (2 * PI) / 100;
        float v = PI / 100;
 
        for ( int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
 
                p1.x = radius * sin(j*v) * cos(i*u);
                p1.y = radius * sin(j*v) * sin(i*u);
                p1.z = radius * cos(j*v);
                n1 = p1;
                n1.Normalize();
 
                p2.x = radius * sin((j+1)*v) * cos(i*u);
                p2.y = radius * sin((j+1)*v) * sin(i*u);
                p2.z = radius * cos((j+1)*v);
                n2 = p2;
                n2.Normalize();
 
                p3.x = radius * sin(j*v) * cos((i+1)*u);
                p3.y = radius * sin(j*v) * sin((i+1)*u);
                p3.z = radius * cos(j*v);
                n3 = p3;
                n3.Normalize();
 
                p4.x = radius * sin((j+1)*v) * cos((i+1)*u);
                p4.y = radius * sin((j+1)*v) * sin((i+1)*u);
                p4.z = radius * cos((j+1)*v);
                n4 = p4;
                n4.Normalize();
 
                if(p1.y > 0 && p1.x < 0){
                    triangles[db] = Triangle(p1, p2, p3, n1, n2, n3);
                    db++;
                    triangles[db] = Triangle(p3, p2, p4, n3, n2, n4);
                    db++;
                }
 
            }
        }
    }
 
    void drawSphere() {
 
        for(int i = 0; i < db; i++){
            triangles[i].drawTriangle();
        }
 
    }
};
 
struct Cylinder {
 
    float radius;
    Triangle triangles[100*2];
    int db;
 
    Cylinder() {
 
    }
 
    Cylinder(float r, int height) {
    
        float u = (PI * 2) / 100;
        float radius = r;
        Vector p1, p2, p3, p4, n1, n2, n3, n4;
        db = 0;
 
        for(int i = 0; i < 100; i++) {
 
            p1.x = radius * cos(i*u);
            p1.z = 0;
            p1.y = radius * sin(i*u);
 
            n1 = p1;
            n1.z = 0;
            n1.Normalize();
 
            p2.x = radius * cos((i+1)*u);
            p2.z = 0;
            p2.y = radius * sin((i+1)*u);
 
            n2 = p2;
            n2.z = 0;
            n2.Normalize();
 
            p3.x = radius * cos(i*u);
            p3.z = height;
            p3.y = radius * sin(i*u);
 
            n3 = p3;
            n3.z = 0;
            n3.Normalize();
 
            p4.x = radius * cos((i+1)*u);
            p4.z = height;
            p4.y = radius * sin((i+1)*u);
 
            n4 = p4;
            n4.z = 0;
            n4.Normalize();
 
            triangles[db] = Triangle(p2, p4, p3, n2, n4, n3);
            db++;
            triangles[db] = Triangle(p1, p2, p3, n1, n2, n3);
            db++;
        }
    }
 
    void drawCylinder(){
        for(int i = 0; i < db; i++){
            triangles[i].drawTriangle();
        }
    }
};
 
struct House {
 
    Vector p1, p2, p3, p4, p5, p6, p7, p8, p9;
    Triangle triangle[12];
 
    House() {
 
        p1 = Vector(-1, 0, 1);
        p2 = Vector(1, 0, 1);
        p3 = Vector(-1, 1, 1);
        p4 = Vector(1, 1, 1);
        p5 = Vector(1, 0, -1);
        p6 = Vector(1, 1, -1);
        p7 = Vector(-1, 0, -1);
        p8 = Vector(-1, 1, -1);
        p9 = Vector(0, 2, 0);
 
        Vector n1 = Vector(0, 0, 1);
        Vector n2 = Vector(0, 0, -1);
        Vector n3 = Vector(1, 0, 0);
        Vector n4 = Vector(-1, 0, 0);
        Vector n5 = Vector(1, 1, 0);
        Vector n6 = Vector(-1, 1, 0);
        Vector n7 = Vector(0, 1, 1);
        Vector n8 = Vector(0, 1, -1);
 
        triangle[0] = Triangle(p1, p3, p4, n1, n1, n1);
        triangle[1] = Triangle(p1, p2, p4, n1, n1, n1);
        triangle[2] = Triangle(p2, p4, p6, n3, n3, n3);
        triangle[3] = Triangle(p5, p2, p6, n3, n3, n3);
        triangle[4] = Triangle(p7, p8, p5, n2, n2, n2);
        triangle[5] = Triangle(p5, p6, p8, n2, n2, n2);
        triangle[6] = Triangle(p1, p7, p3, n4, n4, n4);
        triangle[7] = Triangle(p3, p7, p8, n4, n4, n4);
        triangle[8] = Triangle(p4, p6, p9, n5, n5, n5);
        triangle[9] = Triangle(p4, p3, p9, n7, n7, n7);
        triangle[10] = Triangle(p3, p8, p9, n6, n6, n6);
        triangle[11] = Triangle(p6, p8, p9, n8, n8, n8);
 
    }
 
    void drawHouse(){
 
        float sa[] = {0.4, 0.4, 0.3, 1};
        float sd[] = {0.8, 0.8, 0.7 ,1};
        float ss[] = {0.94, 0.94, 0.8 ,1};
        float sshine = 20;
 
        glMaterialfv(GL_FRONT, GL_AMBIENT, sa);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, sd);
        glMaterialfv(GL_FRONT, GL_SPECULAR, ss);
        glMaterialf(GL_FRONT, GL_SHININESS, sshine);
 
        for( int i = 0; i < 12; i++) {
 
            if (i > 7) {
                float sa[] = {0.4, 0.11, 0.11, 1};
                float sd[] = {0.8, 0.2, 0.3 ,1};
                float ss[] = {0.94, 0.4, 0.5 ,1};
                float sshine = 20;
 
                glMaterialfv(GL_FRONT, GL_AMBIENT, sa);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, sd);
                glMaterialfv(GL_FRONT, GL_SPECULAR, ss);
                glMaterialf(GL_FRONT, GL_SHININESS, sshine);
            }
 
            triangle[i].drawTriangle();
        }
    }
 
};
 
struct Cone{
 
    Triangle triangles[100];
    int db;
    float radius;
    Vector p1, p2, p3, n1, n2, n3;
 
    Cone(float r) {
        float u = (2 * PI) / 100;
        db = 0;
        radius = r;
 
        for (int i = 0; i < 100; ++i)
        {
            p1.x = radius * sin((i + 1) * u),
            p1.y = 0;
            p1.z = radius * cos((i + 1) * u);
 
            p2.x = 0.0;
            p2.y = radius;
            p2.z = 0.0;
 
            p3.x = radius * sin(i * u);
            p3.y = 0;
            p3.z = radius * cos(i * u);
 
            n1.x = sin((i + 1) * u);
            n1.y = sqrt(2.0) / 2.0;
            n1.z = cos((i + 1) * u);
 
            n1.Normalize();
 
            n2.x = sin((i + 0.5) * u);
            n2.y = sqrt(2.0) / 2.0;
            n2.z = cos((i + 0.5) * u);
 
            n2.Normalize();
 
            n3.x = sin(i * u);
            n3.y = sqrt(2.0) / 2.0;
            n3.z = cos(i * u);
 
            n3.Normalize();
 
            triangles[db] = Triangle(p1, p2, p3, n1, n2, n3);
            db++;
        }
    }
 
    void drawCone() {
 
        for(int i = 0; i < db; i++){
            triangles[i].drawTriangle();
        }
 
    }
 
};
 
 
struct Cube{
 
    Vector p1, p2, p3, p4, p5, p6, p7, p8, n1, n2, n3, n4, n5, n6;
 
    Cube(float x, float y, float z){
        p1 = Vector(-x/2.0, -y/2.0, z/2.0);
        p2 = Vector(-x/2.0, -y/2.0, -z/2.0);
        p3 = Vector(x/2.0, -y/2.0, z/2.0);
        p4 = Vector(x/2.0, -y/2.0, -z/2.0);
        p5 = Vector(x/2.0, y/2.0, z/2.0);
        p6 = Vector(x/2.0, y/2.0, -z/2.0);
        p7 = Vector(-x/2.0, y/2.0, z/2.0);
        p8 = Vector(-x/2.0, y/2.0, -z/2.0);
 
        n1 = Vector(1, 0, 0);
        n2 = Vector(-1, 0, 0);
        n3 = Vector(0, 1, 0);
        n4 = Vector(0, -1, 0);
        n5 = Vector(0, 0, -1);
        n6 = Vector(0, 0, 1);
    }
 
    void drawCube() {
 
        glNormal3f(n6.x, n6.y, n6.z);
        glVertex3f(p1.x, p1.y, p1.z);
        
        glNormal3f(n6.x, n6.y, n6.z);
        glVertex3f(p3.x, p3.y, p3.z);
        
        glNormal3f(n6.x, n6.y, n6.z);
        glVertex3f(p5.x, p5.y, p5.z);
        
        glNormal3f(n6.x, n6.y, n6.z);
        glVertex3f(p7.x, p7.y, p7.z);
            
 
        glNormal3f(n1.x, n1.y, n1.z);
        glVertex3f(p3.x, p3.y, p3.z);
        
        glNormal3f(n1.x, n1.y, n1.z);
        glVertex3f(p4.x, p4.y, p4.z);
        
        glNormal3f(n1.x, n1.y, n1.z);
        glVertex3f(p6.x, p6.y, p6.z);
        
        glNormal3f(n1.x, n1.y, n1.z);
        glVertex3f(p5.x, p5.y, p5.z);
        
        
        glNormal3f(n5.x, n5.y, n5.z);
        glVertex3f(p2.x, p2.y, p2.z);
        
        glNormal3f(n5.x, n5.y, n5.z);
        glVertex3f(p4.x, p4.y, p4.z);
        
        glNormal3f(n5.x, n5.y, n5.z);
        glVertex3f(p6.x, p6.y, p6.z);
        
        glNormal3f(n5.x, n5.y, n5.z);
        glVertex3f(p8.x, p8.y, p8.z);
        
        
        glNormal3f(n2.x, n2.y, n2.z);
        glVertex3f(p2.x, p2.y, p2.z);
        
        glNormal3f(n2.x, n2.y, n2.z);
        glVertex3f(p1.x, p1.y, p1.z);
        
        glNormal3f(n2.x, n2.y, n2.z);
        glVertex3f(p7.x, p7.y, p7.z);
        
        glNormal3f(n2.x, n2.y, n2.z);
        glVertex3f(p8.x, p8.y, p8.z);
        
        
        glNormal3f(n3.x, n3.y, n3.z);
        glVertex3f(p7.x, p7.y, p7.z);
        
        glNormal3f(n3.x, n3.y, n3.z);
        glVertex3f(p5.x, p5.y, p5.z);
        
        glNormal3f(n3.x, n3.y, n3.z);
        glVertex3f(p6.x, p6.y, p6.z);
        
        glNormal3f(n3.x, n3.y, n3.z);
        glVertex3f(p8.x, p8.y, p8.z);
        
        
        glNormal3f(n4.x, n4.y, n4.z);
        glVertex3f(p1.x, p1.y, p1.z);
        
        glNormal3f(n4.x, n4.y, n4.z);
        glVertex3f(p3.x, p3.y, p3.z);
        
        glNormal3f(n4.x, n4.y, n4.z);
        glVertex3f(p4.x, p4.y, p4.z);
        
        glNormal3f(n4.x, n4.y, n4.z);
        glVertex3f(p2.x, p2.y, p2.z);
        
    }
 
};
 
Sphere sphere;
Sphere window;
Cylinder cylinder = Cylinder(10, 120);
Cylinder cylinder1 = Cylinder(5, 20);
Sphere sphere1 = Sphere(9.5);
Sphere sphere2 = Sphere(5);
Sphere sphere3 = Sphere(15);
Cylinder cylinder2 = Cylinder(10, 150);
Cylinder cylinder3 = Cylinder(10, 150);
Sphere sphere4 = Sphere(10);
Sphere sphere5 = Sphere(10);
House house = House();
Cylinder arrow;
Cone cone = Cone(20);
House house1 = House();
House house2 = House();
House house3 = House();
Cube rotor1 = Cube(150, 2, 20);
Cube rotor2 = Cube(150, 2, 20);
Cube rearrotor1 = Cube(8, 50, 2);
Cube rearrotor2 = Cube(8, 50, 2);
Cube rearrotor3 = Cube(8, 50, 2);
Cube foot1 = Cube(150, 2, 20);
Cube foot2 = Cube(150, 2, 20);
Cube connection1 = Cube(4, 60, 4);
Cube connection2 = Cube(4, 60, 4);
Cube connection3 = Cube(4, 60, 4);
Cube connection4 = Cube(4, 60, 4);
Cube horizontal = Cube(20, 2, 20);
Cube vertical = Cube(40, 80, 2);
 
// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
    glViewport(0, 0, screenWidth, screenHeight);
 
    sphere.SphereNoFull(100);
    window.SphereWindow(100);
 
    // Peldakent keszitunk egy kepet az operativ memoriaba
    for(int Y = 0; Y < screenHeight; Y++)
        for(int X = 0; X < screenWidth; X++)
            image[Y*screenWidth + X] = Color(0, 0, 0);
 
}
 
Color terrain[32*32];
 
void generateTexture(){
   
    for(int y = 0; y < 32; y++){
        for(int x = 0; x < 32; x++){
 
            if ( x % 5 == 0) {
 
                if (y % 10 == 0){
                    terrain[y*32 + x] = Color(0.1, 0.9, 0.2);
                } else {
                    terrain[y*32 + x] = Color(0, 0.7, 0);
                }
 
            } else {
 
                if ( y % 2 == 0){
                    terrain[y*32 + x] = Color(0.2, 0.7, 0.2);
                } else {
                    terrain[y*32 + x] = Color(0.3, 0.7, 0.3);
                }
 
            }
        }
    }
}
 
void setLighting(){
    
    float light0pos[] = {300, 300, 0, 1};
    float light0amb[] = {0.6, 0.6, 0.6, 1};
    float light0diff[] = {0.7, 0.7, 0.7, 1};
    float light0spec[] = {1, 1, 1, 1};
    float light0dir[] = {-1, -1, 0, 1};
    float shine0 = 100;
    float cutoff = 180;
 
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0dir);
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0spec);
    glLightf(GL_LIGHT0, GL_SHININESS, shine0);
 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
 
    float light1pos[] = {300, 300, 300, 0};
    float light1amb[] = {0.2, 0.2, 0.4, 1};
    float light1diff[] = {0.1, 0.1, 0.1, 1};
    float light1spec[] = {0, 0, 0, 1};
    float shine1 = 100;
 
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1diff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1spec);
    glLightf(GL_LIGHT1, GL_SHININESS, shine1);
 
    glEnable(GL_LIGHT1);
}
 
void setHelicopterMaterial() {
 
    float sa[] = {0.24, 0.21, 0.11, 1};
    float hd[] = {0.76, 0.69, 0.56 ,1};
    float hs[] = {0.94, 0.90, 0.56 ,1};
    float hshine = 20;
 
    glMaterialfv(GL_FRONT, GL_AMBIENT, sa);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, hd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, hs);
    glMaterialf(GL_FRONT, GL_SHININESS, hshine);
 
}
 
void setGreyMaterial() {
 
    float ga[] = {0.2, 0.2, 0.2, 1};
    float gd[] = {0.3, 0.3, 0.3 ,1};
    float gs[] = {0.7, 0.7, 0.7, 1};
    float gshine = 20;
 
    glMaterialfv(GL_FRONT, GL_AMBIENT, ga);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gs);
    glMaterialf(GL_FRONT, GL_SHININESS, gshine);
 
}
 
void setWidowMaterial() {
 
    float wa[] = {0, 0, 0, 0.3};
    float wd[] = {0.3, 0.3, 0.3, 0.3};
    float ws[] = {1, 1, 1, 1};
    float wshine = 40;
 
    glMaterialfv(GL_FRONT, GL_AMBIENT, wa);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ws);
    glMaterialf(GL_FRONT, GL_SHININESS, wshine);
 
}
 
 
void setRedMaterial() {
 
    float ra[] = {0.4, 0.2, 0.2, 1};
    float rd[] = {0.7, 0.3, 0.3, 1};
    float rs[] = {1, 1, 1, 1};
    float rshine = 20;
 
    glMaterialfv(GL_FRONT, GL_AMBIENT, ra);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, rd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, rs);
    glMaterialf(GL_FRONT, GL_SHININESS, rshine);
 
}
 
void setYellowMaterial() {
 
    float ya[] = {0.4, 0.4, 0.2, 1};
    float yd[] = {0.7, 0.7, 0.3, 1};
    float ys[] = {1, 1, 1, 1};
    float yshine = 20;
 
    glMaterialfv(GL_FRONT, GL_AMBIENT, ya);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, yd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ys);
    glMaterialf(GL_FRONT, GL_SHININESS, yshine);
 
}
 
Quaternion qua = Quaternion(1.0, 0.0, 0.0, 0.0);
 
// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.6f, 0.6f, 1.0f);        // torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
 
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
 
    glViewport(0,0,600,600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    gluPerspective(60, 1, 10, 20000);
 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(300, 300, 600, 0, 0, 0, 0, 1, 0);
 
    setLighting();
 
    generateTexture();
 
    glEnable(GL_TEXTURE_2D);
    unsigned int texid;
 
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 32, 32, 0, GL_RGB, GL_FLOAT, terrain);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
 
    setGreyMaterial();
 
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    {
        Vector p1 = Vector(1600, -400, -1600);
        Vector p2 = Vector(-1600, -400, -1600);
        Vector p3 = Vector(1600, -400, 1600);
        Vector p4 = Vector(-1600, -400, 1600);
 
        Vector n = (p3-p1) % (p2-p1);
 
        glTexCoord2f(1, 0);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p1.x, p1.y, p1.z);
 
        glTexCoord2f(0, 0);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p2.x, p2.y, p2.z);
 
        glTexCoord2f(1, 1);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p3.x, p3.y, p3.z);
 
        glTexCoord2f(0, 0);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p2.x, p2.y, p2.z);
 
        glTexCoord2f(1, 1);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p3.x, p3.y, p3.z);
 
        glTexCoord2f(0, 1);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p4.x, p4.y, p4.z);
    }
 
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
 
    glPushMatrix();
    glScalef(50, 50, 50);
    glTranslatef(0, -8, 0);
    glBegin(GL_TRIANGLES);
 
    house.drawHouse();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glScalef(50, 50, 50);
    glTranslatef(-8, -8, 5);
    glBegin(GL_TRIANGLES);
 
    house1.drawHouse();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glScalef(50, 50, 50);
    glTranslatef(10, -8, -10);
    glBegin(GL_TRIANGLES);
 
    house3.drawHouse();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glScalef(50, 50, 50);
    glTranslatef(-10, -8, -10);
    glBegin(GL_TRIANGLES);
 
    house2.drawHouse();
 
    glEnd();
    glPopMatrix();
 
    setHelicopterMaterial();
 
    glPushMatrix();
    glRotatef(qua.GetRotationAngle(), qua.GetAxis().x, qua.GetAxis().y, qua.GetAxis().z);
 
    glPushMatrix();    
    glScalef(1, 0.4, 0.4);
    glTranslatef(0, 0, 0);
    glBegin(GL_TRIANGLES);
 
    sphere.drawSphere();
    
    glEnd();
    glPopMatrix();    
 
    glPushMatrix();
    glTranslatef(0, -69, 65);
    glBegin(GL_QUADS);
 
    foot1.drawCube();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glTranslatef(0, -69, -65);
    glBegin(GL_QUADS);
 
    foot2.drawCube();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glRotatef(-45, 1, 0, 0);
    glTranslatef(-40, -66, 0);
    glBegin(GL_QUADS);
 
    connection1.drawCube();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glRotatef(-45, 1, 0, 0);
    glTranslatef(40, -66, 0);
    glBegin(GL_QUADS);
 
    connection2.drawCube();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glRotatef(45, 1, 0, 0);
    glTranslatef(-40, -66, 0);
    glBegin(GL_QUADS);
 
    connection3.drawCube();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glRotatef(45, 1, 0, 0);
    glTranslatef(40, -66, 0);
    glBegin(GL_QUADS);
 
    connection4.drawCube();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();    
    glTranslatef(215, 0, 12);
    glBegin(GL_TRIANGLES);
 
    sphere2.drawSphere();
    
    glEnd();
    glPopMatrix();    
 
    setGreyMaterial();
 
    glPushMatrix();    
    glTranslatef(215, 27, 12);
    glBegin(GL_QUADS);
 
    rearrotor1.drawCube();
 
    glEnd();
    glPopMatrix();    
 
    glPushMatrix();    
    glTranslatef(196, -11, 12);
    glRotatef(120, 0, 0, 1);
    glBegin(GL_QUADS);
    
    rearrotor2.drawCube();
 
    glEnd();
    glPopMatrix();    
 
 
    glPushMatrix();    
    glTranslatef(234, -11, 12);
    glRotatef(-120, 0, 0, 1);
    glBegin(GL_QUADS);
    
    rearrotor3.drawCube();
 
    glEnd();
    glPopMatrix();    
 
    setHelicopterMaterial();
 
    glPushMatrix();    
    glRotatef(90,0, 1, 0);
    glTranslatef(0, 0, 97);
    glBegin(GL_TRIANGLES);
 
    cylinder.drawCylinder();
 
    glEnd();
    glPopMatrix();    
 
    glPushMatrix();    
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, -55);
    glBegin(GL_TRIANGLES);
 
    cylinder1.drawCylinder();
 
    glEnd();
    glPopMatrix();    
 
    glPushMatrix();    
    glScalef(2, 1, 1);
    glTranslatef(109, 0, 0);
    glBegin(GL_TRIANGLES);
 
    sphere1.drawSphere();
    
    glEnd();
    glPopMatrix(); 
 
    glPushMatrix();    
    glScalef(1, 0.5, 1);
    glTranslatef(0, 110, 0);
    glBegin(GL_TRIANGLES);
 
    sphere3.drawSphere();
    
    glEnd();
    glPopMatrix();    
 
    glPushMatrix();
    glTranslatef(210, 20, 0);
    glRotatef(-20, 0, 0, 1);
    glBegin(GL_QUADS);
 
    vertical.drawCube();
 
    glEnd();
    glPopMatrix();
 
    glPushMatrix();    
    glTranslatef(200, 0, -1);
    glBegin(GL_TRIANGLES);
    {
        Vector p1 = Vector(-8, 1, -40);
        Vector p2 = Vector(16, 1, -40);
        Vector p3 = Vector(-40, 1, 0);
        Vector p4 = Vector(16, 1, 0);
        Vector n1 = Vector(0, 1, 0);
 
        Triangle blade1 = Triangle(p2, p1, p3, n1, n1, n1);
        Triangle blade2 = Triangle(p2, p3, p4, n1, n1, n1);
 
        blade1.drawTriangle();
        blade2.drawTriangle();
    }
    glEnd();
    glPopMatrix();    
 
    glPushMatrix();    
    glTranslatef(200, 0, -1);
    glBegin(GL_TRIANGLES);
    {
        Vector p1 = Vector(-8, -1, -40);
        Vector p2 = Vector(16, -1, -40);
        Vector p3 = Vector(-40, -1, 0);
        Vector p4 = Vector(16, -1, 0);
        Vector n1 = Vector(0, -1, 0);
 
        Triangle blade1 = Triangle(p2, p3, p1, n1, n1, n1);
        Triangle blade2 = Triangle(p2, p4, p3, n1, n1, n1);
 
        blade1.drawTriangle();
        blade2.drawTriangle();
    }
    glEnd();
    glPopMatrix();    
 
    glPushMatrix();
    glTranslatef(200, 0, -1);
    glBegin(GL_QUADS);
    {
    
    Vector p1 = Vector(-8, 1, -40);
    Vector p2 = Vector(16, 1, -40);
    Vector p3 = Vector(-8, -1, -40);
    Vector p4 = Vector(16, -1, -40);
 
    glNormal3f(0, 0, -1);
    glVertex3f(p1.x, p1.y, p1.z);
 
    glNormal3f(0, 0, -1);
    glVertex3f(p2.x, p2.y, p2.z);
 
    glNormal3f(0, 0, -1);
    glVertex3f(p4.x, p4.y, p4.z);
 
    glNormal3f(0, 0, -1);
    glVertex3f(p3.x, p3.y, p3.z);
    
    }
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glTranslatef(200, 0, -1);
    glBegin(GL_QUADS);
    {
    
    Vector p1 = Vector(16, 1, -40);
    Vector p2 = Vector(16, -1, -40);
    Vector p3 = Vector(16, -1, 0);
    Vector p4 = Vector(16, 1, 0);
 
    glNormal3f(1, 0, 0);
    glVertex3f(p1.x, p1.y, p1.z);
 
    glNormal3f(1, 0, 0);
    glVertex3f(p2.x, p2.y, p2.z);
 
    glNormal3f(1, 0, 0);
    glVertex3f(p3.x, p3.y, p3.z);
 
    glNormal3f(1, 0, 0);
    glVertex3f(p4.x, p4.y, p4.z);
    
    }
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glTranslatef(200, 0, -1);
    glBegin(GL_QUADS);
    {
    
    Vector p1 = Vector(-8, -1, -40);
    Vector p2 = Vector(-40, 1, 0);
    Vector p3 = Vector(-40, -1, 0);
    Vector p4 = Vector(-8, 1, -40);
 
    glNormal3f(1, 0, -1);
    glVertex3f(p1.x, p1.y, p1.z);
 
    glNormal3f(1, 0, -1);
    glVertex3f(p2.x, p2.y, p2.z);
 
    glNormal3f(1, 0, -1);
    glVertex3f(p3.x, p3.y, p3.z);
 
    glNormal3f(1, 0, -1);
    glVertex3f(p4.x, p4.y, p4.z);
    
    }
    glEnd();
    glPopMatrix();
 
    setGreyMaterial();
 
    glPushMatrix();    
 
    glPushMatrix(); 
    glRotatef(-90, 0, 1, 0);
    glRotatef(10, 1, 0, 0);
    glTranslatef(75, 55, -10);
    glBegin(GL_QUADS);
 
    rotor1.drawCube();
 
    glEnd();
    glPopMatrix();    
 
    glPushMatrix();    
    glRotatef(-90, 0, 1, 0);
    glRotatef(-10, 1, 0, 0);
    glTranslatef(-75, 55, 10);
    glBegin(GL_QUADS);
 
    rotor2.drawCube();
 
    glEnd();
    glPopMatrix();    
 
    glPopMatrix();    
 
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    setWidowMaterial();
 
    glPushMatrix(); 
    glScalef(1, 0.4, 0.4);
    glBegin(GL_TRIANGLES);
 
    window.drawSphere();
 
    glEnd();
    glPopMatrix();    
 
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
 
    glPopMatrix();
 
    setRedMaterial();
 
    arrow = Cylinder(5, fabs(qua.s)*800);
 
    glPushMatrix();
    if(qua.d.Length() > 0.001)
        glRotatef(180, arrowVector.x, arrowVector.y, arrowVector.z);
    glRotatef(-90, 1, 0, 0);
    
    glBegin(GL_TRIANGLES);
 
    arrow.drawCylinder();
 
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    if(qua.d.Length() > 0.001)
        glRotatef(180, arrowVector.x, arrowVector.y, arrowVector.z);
    glTranslatef(0, fabs(qua.s)*790, 0);
    glBegin(GL_TRIANGLES);
 
    cone.drawCone();
 
    glEnd();
    glPopMatrix();
 
    glutSwapBuffers();                     // Buffercsere: rajzolas vege
}
 
// Billentyuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
 
    if (key == 'R') {
 
        Quaternion quaInverse = qua.getInverse();
        Quaternion e = qua % Quaternion(0, Vector(1, 0, 0)) % quaInverse;
 
        float degreeRad = 20 * (PI / 180.0);
        Quaternion q = Quaternion(cosf(degreeRad/2.0), e.d * sinf(degreeRad/2.0));
        qua = q % qua;
 
    }
 
    if (key == 'E')    {
 
        Quaternion quaInverse = qua.getInverse();
        Quaternion e = qua % Quaternion(0, Vector(1, 0, 0)) % quaInverse;
 
        float degreeRad = -20 * (PI / 180.0);
        Quaternion q = Quaternion(cosf(degreeRad/2.0), e.d * sinf(degreeRad/2.0));
        qua = q % qua;
 
    }
 
    if (key == 'Y')    {
 
        Quaternion quaInverse = qua.getInverse();
        Quaternion e = qua % Quaternion(0, Vector(0, 1, 0)) % quaInverse;
 
        float degreeRad = 20 * (PI / 180.0);
        Quaternion q = Quaternion(cosf(degreeRad/2.0), e.d * sinf(degreeRad/2.0));
        qua = q % qua;
 
    }
 
    if (key == 'X') {
 
        Quaternion quaInverse = qua.getInverse();
        Quaternion e = qua % Quaternion(0, Vector(0, 1, 0)) % quaInverse;
 
        float degreeRad = -20 * (PI / 180.0);
        Quaternion q = Quaternion(cosf(degreeRad/2.0), e.d * sinf(degreeRad/2.0));
        qua = q % qua;
 
    }
 
    if (key == 'P')    {
 
        Quaternion quaInverse = qua.getInverse();
        Quaternion e = qua % Quaternion(0, Vector(0, 0, 1)) % quaInverse;
 
        float degreeRad = 20 * (PI / 180.0);
        Quaternion q = Quaternion(cosf(degreeRad/2.0), e.d * sinf(degreeRad/2.0));
        qua = q % qua;
 
    }
 
    if (key == 'O') {
 
        Quaternion quaInverse = qua.getInverse();
        Quaternion e = qua % Quaternion(0, Vector(0, 0, 1)) % quaInverse;
 
        float degreeRad = -20 * (PI / 180.0);
        Quaternion q = Quaternion(cosf(degreeRad/2.0), e.d * sinf(degreeRad/2.0));
        qua = q % qua;
 
    }
 
    Vector quaVector = qua.GetAxis();
    quaVector.Normalize();
    Vector tmp = (Vector(0, 1, 0) + quaVector)*0.5;
    arrowVector = tmp;
    
    glutPostRedisplay();
 
}
 
 
// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
        glutPostRedisplay( );                          // Ilyenkor rajzold ujra a kepet
}
 
// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
     long time = glutGet(GLUT_ELAPSED_TIME);        // program inditasa ota eltelt ido
 
     //glutPostRedisplay();
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