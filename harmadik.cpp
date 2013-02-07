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
 
// FORRASMEGJELOLES: Szirmay-Kalos László: Háromdimenziós grafika,animáció és játékfejlesztés CD melléklete
 
//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
 
struct Vector {
   double x, y, z;
 
   Vector( ) { 
    x = y = z = 0;
   }
 
    void Set(double xx, double yy, double zz) {
       x = xx;
       y = yy;
       z = zz;
   }
 
   Vector(double x0, double y0, double z0 = 0.0) { 
    x = x0; y = y0; z = z0;
   }
 
   Vector operator*(double a) { 
    return Vector(x * a, y * a, z * a); 
   }
 
   Vector operator+(const Vector& v) {
     return Vector(x + v.x, y + v.y, z + v.z); 
   }
 
   Vector operator-(const Vector& v) {
     return Vector(x - v.x, y - v.y, z - v.z); 
   }
 
   double operator*(const Vector& v) {     
    return (x * v.x + y * v.y + z * v.z); 
   }
 
   Vector operator%(const Vector& v) {     
    return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
 
   double Length() { 
       return sqrt(x * x + y * y + z * z); }
 
   Vector Normalize() {
        double length = Length();
        x /= length;
        y /= length;
        z /= length;
        return *this;
   }
};
 
struct Color {
 
   float r, g, b;
 
   Color( ) { 
    r = g = b = 0;
   }
 
   Color(double r0, double g0, double b0) { 
    r = r0; g = g0; b = b0;
   }
 
   Color operator*(double a) { 
    return Color(r * a, g * a, b * a); 
   }
 
   Color operator*(const Color& c) { 
    return Color(r * c.r, g * c.g, b * c.b); 
   }
 
   Color operator+(const Color& c) {
     return Color(r + c.r, g + c.g, b + c.b); 
   }
};
 
const double EPSILON = 1e-3; 
const double PI = 3.1415926536;
const double DMAX = 8;					// MAXIMÁLIS REKURZIÓ
const float FLT_MAX = 1E+37;			
 
const int screenWidth = 600;
const int screenHeight = 600;
 
Color image[screenWidth*screenHeight];    
 
Vector photons[10000];					// FOTONOK
Color ColorIntensity[10000];			// SZÍNE
 
int photonssize = 0;
 
class Ray {								// SUGÁR
 
public:
    Vector origin;						// KIINDULÓ PONT
    Vector dir;							// IRÁNY
 
    Ray() {
        origin = Vector(0, 0, 0);
        dir = Vector(0, 0, 0);
    }
 
    Ray(const Vector& newOrigin,  const Vector& newDir) { 
        origin = newOrigin;
        dir = newDir;
    }
};
 
class Light {					    // FÉNYFORRÁS SZÍNE ÉS HELYE
 
public:
    Color    emission;
    Vector    location;
 
    virtual Color    GetEmission() { return emission; };
};
 
class Camera {						// KAMERA OSZTÁLY
 
public:
    Vector    eyep;        
    Vector    lookp;        
    Vector    updir;        
 
    float    viewdist;        	// LÁTÓTÁVOLSÁG
    float    fov, hfov, vfov;    	// LÁTÓSZÖG
 
    float    nearClip, farClip;		// VÁGÁSI SÍKOK
    long    hres, vres;            	// ABLKAMÉRET
        
    Vector    X, Y, Z;            
    float    pixh, pixv;    
 
    Camera() { 
 
        eyep.Set(0.0, 0.0, 10.0); 	// POZÍCIÓ
        lookp.Set(0.0, 0.0, 0.0);	// HOVA NÉZ
        updir.Set(0, -1.0, 0.0);	// FELFELE MUTATÓ VEKTOR
 
        fov            = 45;
 
        nearClip    = screenHeight; 
        farClip        = screenWidth;
        hres        = screenHeight;
        vres        = screenWidth; 
    }
 
    void CompleteCamera() {
 
        Z = lookp - eyep;			// TÁVOLSÁGA HOGY HOVA NÉZÜNK
        viewdist = Z.Length();
 
        if (viewdist < EPSILON) {
            return;
        }
 
        Z = Z * (1.0 / viewdist);		//NORMALIZÁLJUK
 
        X = Z % updir;					// X IRÁNY VEKTORA
        float lengthX = X.Length();
        if (lengthX < EPSILON) {
            return;
        }
        X = X * (1.0 / lengthX);
 
        Y = Z % X;						// Y IRÁNYVEKTORA
        Y.Normalize();
 
        if (hres < vres) {				
            hfov = fov; 
            vfov = atan(tan(fov * PI/180.0) * (float)vres/(float)hres) * 180.0/PI;
        } else {
            vfov = fov; 
            hfov = atan(tan(fov * PI/180.0) * (float)hres/(float)vres) * 180.0/PI;
        }
 
        float tanFovH = tan(hfov * PI / 180.0);
        float tanFovV = tan(vfov * PI / 180.0);  // LÁTÓSZÖG KISZÁMOLÁSA
 
        pixh = 2.0 * tanFovH / (float)(hres);		// KÉPPONT MAGASSÁG SZÉLESSÉG
        pixv = 2.0 * tanFovV / (float)(vres);
 
    }
};
 
class Object
{
public:
    Color color; 							// DIFFUZ SZÍN
    Color Ka;								// AMBIENS SZÍN
 
    Color fr; 								// törésmutató
    Color kappa;							// anyagjellemzõ
 
    bool isReflective; 
    bool isRefractive; 
 
    virtual double intersect(Ray& ray) = 0; 
    virtual Vector getNormal(Vector& intersect) = 0;
 
    Color calculateFresnel(float costheta, bool recipn) {
 
        Color f;
        Color t;
 
        if(recipn){        		// törésmutató reciproka mikor kilépünk az anyagból
            t.r= 1.0 / fr.r;
            t.g= 1.0 / fr.g;
            t.b= 1.0 / fr.b;
        } else { 				
            t = fr; 			// amugy marad az eredeti
        }
								// frenel együttható kiszámítása 
        f.r = ((pow((t.r - 1.0), 2)) + (pow(kappa.r, 2)) + (pow((1.0 - costheta), 5)) * (4 * t.r)) / ((pow((t.r + 1.0), 2)) + (pow(kappa.r, 2)));
        f.g = ((pow((t.g - 1.0), 2)) + (pow(kappa.g, 2)) + (pow((1.0 - costheta), 5)) * (4 * t.g)) / ((pow((t.g + 1.0), 2)) + (pow(kappa.g, 2)));
        f.b = ((pow((t.b - 1.0), 2)) + (pow(kappa.b, 2)) + (pow((1.0 - costheta), 5)) * (4 * t.b)) / ((pow((t.b + 1.0), 2)) + (pow(kappa.b, 2)));
 
        return f;
    }    
 
    
};
 
class Scene
{
public:
    Object* objects[1000]; 				// ezer objektumot tárolunk
    int objcount; 
    Light light; 						// fény
    
    Scene()
    {		
        light.emission = Color(1, 1, 1);		//fény színe
		light.location = Vector(300, 300, -100);// fény pozíciója
        objcount = 0;
    }
 
    void add(Object* object, Color objColor, Color n, Color k, bool reflective, bool refractive) //objektum hozzáadása a színtérhez
    {
        objects[objcount] = object;
        objects[objcount]->color = objColor; 
        objects[objcount]->Ka = objColor * 0.2;
        objects[objcount]->fr = n;
        objects[objcount]->kappa = k; 
        objects[objcount]->isReflective = reflective; 
        objects[objcount]->isRefractive = refractive;
        objcount++;
    }
 
    double Intersect(Ray& ray, int& index) {			// metszés vizsgálat az objektumokkal
        
        double t = -1; 
        double mint = FLT_MAX;
        
        for(int i = 0; i < objcount; i++)
        {
            t = objects[i]->intersect(ray); 
            if(t > EPSILON) 
            {
                if(t < mint)
                {
                    mint = t; 
                    index = i; 
                }
            }
        }
        return mint; 
    }
 
    Color DirectLightsource(Ray& lightRay, int idx, float cost) {		// diffuz felületek árányalása
 
        if(cost < 0) {
            cost = 0;
        }
        
        int minindex = -1;
        double t = -1; 
        double mint = FLT_MAX;
 
        for(int i = 0; i < objcount; i++)								// SHADOWRAY VIZSGÁLAT
        {
            t = objects[i]->intersect(lightRay); 						// HA METSZI A FÉNY ÚTJÁT VALAMI AKKOR ÁRNYÉKBAN VAN
            if(t > EPSILON) 
            {
                if(t < mint)
                {
                    mint = t; 
                    minindex = i; 
                }
            }
        }
 
        t = mint; 
 
        bool isIntersect = false;
        if(minindex >= 0) {
            isIntersect = true;
        }
 
        Color sumColor = Color(0, 0, 0); 								// feketérõl indul
        Color La = Color(1.5, 1.5, 1.5);								// ambiens fényforrás
        Color ambient =  objects[idx]-> Ka * La;						// ambiens szín
 
        if (isIntersect) {												// ha van metszés akkor árnyékot adunk vissza
            return ambient;
        }
 
        Color color = objects[idx]->color;								// ha nincs akkor pedig az objektum színét árnyalva a costhetaval
        sumColor = sumColor + color * cost;
        return sumColor;
    }
 
    Color Trace(Ray& ray, int depth){									// trace függvény
 
        if (depth > DMAX) {												// maximális iteráció
            return Color(0, 0, 0);
        }
        
        depth++;
        int minindex = -1;
        double t = Intersect(ray, minindex);							// elmetszük a sugarunkat
 
        if(minindex < 0) 
            return Color(0.7, 0.8, 1);									// ha nincs metszés akkor a környezet színét adjuk vissza (világoskék)
 
        Vector intersectPoint; 											// metszéspont
        intersectPoint = ((ray.origin) + (ray.dir * t)); 				// a sugáron a kiinduló pont + irány * t távolság
        Vector normal = objects[minindex]->getNormal(intersectPoint); 	// a metszéspontban elkérjük az objektum normálvektorát
 
        Ray lightRay; 													// fénysugár	
        lightRay.origin = intersectPoint;
        lightRay.dir = light.location - intersectPoint; 
        lightRay.dir.Normalize(); 
 
        float costheta = normal * lightRay.dir; 						// a fény beesési szöge
 
        Color color = Color(0, 0, 0);									// kezdeti szín
        Color fren;														// frenel együttható
 
        if(!objects[minindex]->isReflective && !objects[minindex]->isRefractive){			// diffuz felület se nem törõ és se nem tükrözõ
            
            for(int i = 0; i < photonssize; i++) {						// fotontérkép kiolvasása
                if((photons[i] - intersectPoint).Length() < 10)			// ha valamilyen sugarában a metszéspontnak található foton belõve akkor kiolvassuk a színét
                    color = color + ColorIntensity[i];
            }
 
            color = color + DirectLightsource(lightRay, minindex, costheta);		// diffuz felületet árnyalunk
        }
 
        if(objects[minindex]->isReflective) {							// ha törõ anyag
 
            double n = objects[minindex]->fr.r;							// törésmutatója
            double cosalpha = ray.dir * normal * -1.0;					// beesési szög * -1
            Vector tempNormal = normal;									// normálvektor
	
            Ray reflectedRay;											// tükrözött fénysugár és iránya 
            reflectedRay.origin = intersectPoint;
            reflectedRay.dir = ray.dir + normal * 2 * cosalpha; 
            reflectedRay.dir.Normalize(); 
 
            Color plusColor = Trace(reflectedRay, depth);				// elindítjuk rekurzívan az új sugarat
 
            if(cosalpha < 0) {											// az üveg belsejében vagyunk-e és törünk
                n = 1.0 / n; 											// törésmutató reciprok
                fren = objects[minindex]->calculateFresnel(-cosalpha, true);	// megfelelõ szöget beállítjuk
            } else{
                fren = objects[minindex]->calculateFresnel(cosalpha, false);	// ha nem az anyag belsejébõl jövünk
            }
 
            float disc = 1.0 - ((1.0 - cosalpha * cosalpha ) / (n * n)); 	// sikerül-e betörni
 
            if(disc > 0 || !objects[minindex]->isRefractive){	//ha sikerült betörni a fényt vagy csak simán tükrözõ anyag akkor beszorzunk a frenel együtthatóval
                plusColor = plusColor * fren;
            }
 
            color = color + plusColor; 	//hozzáadjuk a pluscolort
        } 
 
        if(objects[minindex]->isRefractive) {		//ha törõ
 
            double n = objects[minindex]->fr.r;		//törésmutató
            Vector tnormal = normal; 				//normal vektor
 
            float cosalpha = tnormal * ray.dir * -1.0; 	//beesési szög
 
            if(cosalpha < 0) {					// ha anyag belsejébõl jövünk 
                n = 1.0 / n; 					//törésmutató reciprok
                tnormal = tnormal * -1.0; 		//normálvektor fordít
                cosalpha = tnormal * ray.dir * -1.0; 	//coslapha ujraszámolás
                fren = objects[minindex]->calculateFresnel(cosalpha, true);  //frenel kiszámolása újra
 
            } else {
 
                fren = objects[minindex]->calculateFresnel(cosalpha, false);   // ha kivülrõl jövünk frenelt számolunk
            }
 
            float disc = 1.0 - ((1.0 - cosalpha * cosalpha ) / (n * n)); // ha be tudjuk törni a sugarat
 
            if(disc > 0) {
 
                Ray refractedRay; 							// törtsugár
                refractedRay.origin = intersectPoint;  // pozicioja
                refractedRay.dir = (ray.dir * (1 / n)) + tnormal * (cosalpha / n -sqrt(disc));  // irnaya
                refractedRay.dir.Normalize(); 
 
                Color retColor = Trace(refractedRay, depth);  // viszatért szín
                Color plusColor;                
                
                plusColor.r = retColor.r * (1.0 - fren.r);   // 1 -frenellel szorzunk mivel türtök az anyagot
                plusColor.g = retColor.g * (1.0 - fren.g);
                plusColor.b = retColor.b * (1.0 - fren.b);
 
                color = color + plusColor; //hozzáadjuk
            }
        }
        return color;
    }
 
    void Shoot(Ray &ray, Color color, int depth){				// foton lövés
        
        if(depth > DMAX){										// iterációk száma
            return;
        }
 
        depth++;
 
        int minindex = -1;
        double t = Intersect(ray, minindex);					// sikerül-e metszést találni
 
        if(minindex < 0) {										// ha nem akkor nincs tovább
            return;
        }
 
        Vector intersectPoint; 
        intersectPoint = ((ray.origin) + (ray.dir * t)); 		// metszéspont
 
        Vector normal = objects[minindex]->getNormal(intersectPoint);  // normálvektor a metszéspontban
 
        if(!objects[minindex]->isReflective && !objects[minindex]->isRefractive && depth > 1){  // ha se nem törõ és se nem tükrözõ anyagot találtunk el és a mélység nagyobb mint 1 
																								// tehát már egyszer megtörtük a fényt 
            if(photonssize < 10000) {															// hozzáadjuk a fotontérképhez
            photons[photonssize] = intersectPoint;
            ColorIntensity[photonssize] = color;
            photonssize++;
            } else {																			// csak 10000 fotonig dolgozunk
                return;
            }
    
        }
 
        Color fr;
 
        if(objects[minindex]->isReflective){													// ha tükrözõ anyag tükrözünk
 
            float cosalpha = normal * -1.0 * ray.dir; 
            float n = objects[minindex]->fr.r;
 
            Ray reflectedRay; 
            reflectedRay.origin = intersectPoint;
            reflectedRay.dir = ray.dir + normal * 2 * cosalpha; 
            reflectedRay.dir.Normalize(); 														// anyag belsejében tükrözünk vagy kívülrõl jövünk
 
            if(cosalpha < 0){
 
                n=1/n;
                fr = objects[minindex]->calculateFresnel(-cosalpha, true);
 
            } else {
 
                fr = objects[minindex]->calculateFresnel(cosalpha, false);
 
            }
 
            float disc = 1.0 - ((1.0 - cosalpha * cosalpha ) / (n * n)); 						// megnézzük, hogy sikerül-e betörni
 
            if(disc > 0 || !objects[minindex]->isRefractive){
                Shoot(reflectedRay, color*fr, depth);											// továbblõjuk ha sikerül megtörni vagy csak tükrözõ anyag
            } else {
                Shoot(reflectedRay, color, depth);
            }
        }
 
        if(objects[minindex]->isRefractive){													// törõ
 
            float n = objects[minindex]->fr.r; 
            Vector tnormal = normal; 
            float cosalpha = tnormal * ray.dir * -1.0; 											// beesési szög és normálvektor
            
            if(cosalpha < 0) 																	// anyagon belülrõl jövünk-e
            {
                n = 1.0 / n;																	// törési együtthatót fordítjuk
                tnormal = tnormal * -1.0;														// normálvektort fordítunk
                cosalpha = tnormal * ray.dir * -1.0;											// cosalphat ujra számoljuk
 
                fr = objects[minindex]->calculateFresnel(cosalpha, true);						// kiszámoljuk az új frenelt
            
            } else {
 
                fr = objects[minindex]->calculateFresnel(cosalpha, false);
            }
 
            float disc = 1.0 - ((1.0 - cosalpha * cosalpha ) / (n * n)); 						// sikerül megtörni
 
            if(disc > 0) 
            {
                Ray refractedRay;
                refractedRay.origin = intersectPoint; 
                refractedRay.dir = ray.dir * (1 / n) + tnormal * (cosalpha / n -sqrt(disc)); 	// a tört sugár iránya
                refractedRay.dir.Normalize(); 
 
                color.r = color.r * (1.0-fr.r);
                color.g = color.g * (1.0-fr.g);
                color.b = color.b * (1.0-fr.b);
 
                Shoot(refractedRay, color, depth);
            }
        }
    }
    
};
 
class Sphere : public Object {
public:
    Vector origo; 
    float radius; 
    
    Sphere(Vector o = Vector(), float r = 1.0) {
        origo = o;
        radius = r;
    }
 
    double intersect(Ray& ray) {
        
        Vector dist = ray.origin - origo;
        double b = (dist * ray.dir) * 2.0;
        double a = (ray.dir * ray.dir);
        double c = (dist * dist) - radius * radius;
 
        double discr = b * b - 4.0 * a * c;
 
        if ( discr < 0 ) 
            return -1;
 
        double sqrt_discr = sqrt( discr );
        double t1 = (-b + sqrt_discr)/2.0/a;
        double t2 = (-b - sqrt_discr)/2.0/a;
 
        if (t1 < EPSILON) t1 = -EPSILON;
        if (t2 < EPSILON) t2 = -EPSILON;
        if (t1 < 0 && t2 < 0)
            return -1;
 
        float t;
        if (t1 < 0 && t2 >= 0) t = t2;
        else if (t2 < 0 && t1 >= 0) t = t1;
        else if (t1 < t2)  t = t1; 
        else t = t2;
 
        return t;
}
 
    Vector getNormal(Vector& intersect)
    {
        return (intersect - origo).Normalize();
    }
};
 
class Plane : public Object
{
public:
 
    Vector point;
    Vector normal;
 
    Plane(Vector p, Vector n)
    {
        point = p;
        normal = n;
    }
 
    double intersect(Ray& ray)
    {
        float d = normal * ray.dir;
 
        if(d == 0.0) {
            return -1.0;
        }
 
        float nx = normal.x;
        float ny = normal.y;
        float nz = normal.z;
        float Psx = point.x;
        float Psy = point.y;
        float Psz = point.z;
        float dvx = ray.dir.x;
        float dvy = ray.dir.y;
        float dvz = ray.dir.z;
        float Pex = ray.origin.x;
        float Pey = ray.origin.y;
        float Pez = ray.origin.z;
        float t = -1.0 * (nx * Pex - nx * Psx + ny * Pey - ny * Psy + nz * Pez - nz * Psz) / (nx * dvx + ny * dvy + nz * dvz);
 
        if(t > EPSILON) return t;
        if(t > 0) return 0;
        return -1;
    }
 
    Vector getNormal(Vector&)
    {
        return normal;
    }
};
 
class Triangle : public Object
{
public:
    Vector            a, b, c;        
    long            ai, bi, ci;        
 
    Vector            normal;    
    Vector            Na, Nb, Nc;    
 
    Triangle(Vector p0, Vector p1, Vector p2, Vector n0, Vector n1, Vector n2) {
        a = p0;
        b = p1;
        c = p2;
 
        Na = n0.Normalize();
        Nb = n1.Normalize();
        Nc = n2.Normalize();
 
        normal = (b - a) % (c - a);
        normal.Normalize();
    }
 
    double intersect(Ray& ray) {
        double cost = normal * ray.dir;
    
        if (fabs(cost) <= EPSILON) {
            return -1;
        }
 
        double t = ((a - ray.origin) * normal) / cost;
        if(t < EPSILON) 
            return -1;
 
        Vector ip = ray.origin + ray.dir * t;
 
        double c1 = (((b - a) % (ip - a)) * normal);
        double c2 = (((c - b) % (ip - b)) * normal);
        double c3 = (((a - c) % (ip - c)) * normal);
        
        if (c1 >= 0 && c2 >= 0 && c3 >= 0)
        {
            return t;
        }
        if (c1 <= 0 && c2 <= 0 && c3 <= 0)
        {
            return t;
        }
        return -1;
    }
 
    Vector getNormal(Vector& intersect)    {
 
        float u, v;
        Vector v0, v1, v2;
 
        v0 = c - a;
        v1 = b - a;
        v2 = intersect - a;
 
        u = ((v1 * v1) * (v2 * v0) - (v1 * v0) * (v2 * v1)) / ((v0 * v0) * (v1 * v1) - (v0 * v1) * (v1 * v0));
        v = ((v0 * v0) * (v2 * v1) - (v0 * v1) * (v2 * v0)) / ((v0 * v0) * (v1 * v1) - (v0 * v1) * (v1 * v0));
 
        return Nc * u + Nb * v + Na * (1 - (u + v));
    }
 
};
 
 
void buildTorus(Vector center,  Scene& scene)
{
    float R = 140;
    float r = 40;    
 
     for(int i = 0; i < 10; i++){
 
        float AngleOuter = (2.0*PI/10);
                
        for(int j = 0; j < 5; j++){
 
            float AngleInner = (2.0*PI/5);
 
            Vector v1, v2, v3, v4;
 
            v1.x = cos (AngleOuter * i) * (R + r * cos(AngleInner * j));
            v1.y = sin (AngleOuter * i) * (R + r * cos(AngleInner * j));
            v1.z = r * sin (AngleInner * j);
 
            v2.x = cos (AngleOuter * (i+1)) * (R + r * cos(AngleInner * j));
            v2.y = sin (AngleOuter * (i+1)) * (R + r * cos(AngleInner * j));
            v2.z = r * sin (AngleInner * j);
 
            v3.x = cos (AngleOuter * (i)) * (R + r * cos(AngleInner * (j+1)));
            v3.y = sin (AngleOuter * (i)) * (R + r * cos(AngleInner * (j+1)));
            v3.z = r * sin (AngleInner * (j+1));
 
            v4.x = cos (AngleOuter * (i+1)) * (R + r * cos(AngleInner * (j+1)));
            v4.y = sin (AngleOuter * (i+1)) * (R + r * cos(AngleInner * (j+1)));
            v4.z = r * sin (AngleInner * (j+1));
 
            Vector p1, p2, p3, p4;
        
            p1 = center + v1;
            p2 = center + v2;
            p3 = center + v3;
            p4 = center + v4;
 
            scene.add(new Triangle(p1, p2, p3,
                (v1 - Vector(cos( AngleOuter * i) * R, sin( AngleOuter * i) * R, 0)),
                (v2 - Vector(cos( AngleOuter * (i+1)) * R, sin( AngleOuter * (i+1)) * R, 0)),
                (v3 - Vector(cos( AngleOuter * i) * R, sin( AngleOuter * i) * R, 0))), Color(0.1, 0.8, 0.2), Color(0.17, 0.35, 1.5),Color(3.1, 2.7, 1.9),true, false);
 
            scene.add(new Triangle(p4, p3, p2,
                (v4 - Vector(cos(AngleOuter * (i+1)) * R, sin( AngleOuter * (i+1)) * R, 0)),
                (v3 - Vector(cos(AngleOuter * i) * R, sin( AngleOuter * i) * R, 0)),
                (v2 - Vector(cos(AngleOuter * (i+1)) * R, sin( AngleOuter * (i+1)) * R, 0))), Color(0.1, 0.8, 0.2), Color(0.17, 0.35, 1.5), Color(3.1, 2.7, 1.9), true, false);
        }
    }
}
 
void buildCube(Vector center, Scene& scene, int size)
{
    Vector a = Vector(size, 0, 0);
    Vector b = Vector(0, size, 0);
    Vector c = Vector(0, 0, size);
 
 
    Vector p0, p1, p2, n;
 
    p0 = center - a + b - c;
    p1 = center + a + b - c;
    p2 = center + a - b - c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n), Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center + a - b - c;
    p1 = center - a - b - c;
    p2 = center - a + b - c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center + a + b - c;
    p1 = center + a + b + c;
    p2 = center + a - b + c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center + a - b + c;
    p1 = center + a - b - c;
    p2 = center + a + b - c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center - a + b + c;
    p1 = center + a + b + c;
    p2 = center + a + b - c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center + a + b - c;
    p1 = center - a + b - c;
    p2 = center - a + b + c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center + a + b + c;
    p1 = center - a + b + c;
    p2 = center - a - b + c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center - a - b + c;
    p1 = center + a - b + c;
    p2 = center + a + b + c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center - a + b + c;
    p1 = center - a + b - c;
    p2 = center - a - b - c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center - a - b - c;
    p1 = center - a - b + c;
    p2 = center - a + b + c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center - a - b - c;
    p1 = center + a - b - c;
    p2 = center + a - b + c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
    p0 = center + a - b + c;
    p1 = center - a - b + c;
    p2 = center - a - b - c;
 
    n = (p2 - p1) % (p0 - p1);
 
    scene.add(new Triangle(p0, p1, p2, n, n, n),  Color(0, 0, 0), Color(1.5, 1.5, 1.5), Color(0, 0, 0), true, true);
 
}
 
void buildTable(Vector c, Scene &scene, int size) {
    
    Vector p1 = Vector(c.x - size, c.y, c.z - size);
    Vector p2 = Vector(c.x + size, c.y, c.z - size);
    Vector p3 = Vector(c.x - size, c.y, c.z + size);
    Vector p4 = Vector(c.x + size, c.y, c.z + size);
 
    Vector n1 = (p3 - p1) % (p2 - p1);
    Vector n2 = (p2 - p4) % (p3 - p4);
 
    scene.add(new Triangle(p1, p2, p3, n1, n1, n1), Color(0.7, 0.58, 0.17), Color(1.5, 1.5, 1.5), Color(0, 0, 0), false, false);
    scene.add(new Triangle(p2, p3, p4, n2, n2, n2), Color(0.7, 0.58, 0.17), Color(1.5, 1.5, 1.5), Color(0, 0, 0), false, false);
 
}
 
Ray GetRay(int x, int y, Camera& cam){
 
    float h = cam.pixv;
    float v = cam.pixh;
 
    float pixX = -h * cam.vres/2.0 + x * h + h / 2.0;
    float pixY = -v * cam.hres/2.0 + y * v + v / 2.0;
 
    Vector raydir = cam.Z + cam.X * pixX + cam.Y * pixY;
 
    raydir = raydir.Normalize();
 
    return Ray(cam.eyep, raydir);
}
 
 
void onInitialization( ) { 
    glViewport(0, 0, screenWidth, screenHeight);
 
    Camera cam; 
    cam.eyep = Vector(-300, 200, -100);
    cam.lookp = Vector(0, 0, 300);
    cam.CompleteCamera();
 
    Scene scene; 
    
    scene.add(new Sphere(Vector(0, -89, 300), 190), Color(0.01, 0.01, 0.10), Color(0.6, 0.6, 0.6), Color(0, 0, 0), true, true);
 
    buildCube(Vector(0, -89, 300), scene, 200);
    buildTorus(Vector(0, -89, 300), scene);
    buildTable(Vector(0, -290, 300), scene, 500);
 
    Vector lightdir = Vector(0,0,0)-scene.light.location;
    lightdir.Normalize();
 
    int photoncount = 0;
 
    while(photoncount < 10000) {
 
        float x = (2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f);
        float y = (2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f);
        float z = (2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f);
        float r=1.0;
 
        if(pow(x, 2) + pow(y, 2) + pow(z, 2) < pow(r,2)){
 
            if(y<0 && x<0 && z>0){
                photoncount++;
                Ray r;
                r.origin = scene.light.location;
                r.dir = Vector(x,y,z);
                scene.Shoot(r,Color(0.3,0.3,0.3),0);
            }
        }
    }
 
    for (int i = 0; i < screenHeight; i++)
    {
        for (int j = 0; j < screenWidth; j++)
        {
            Ray ray;
            ray = GetRay(j, i, cam);
            image[i * screenWidth + j] = scene.Trace(ray, 0);
        }
    }
 
    for (int i = 0; i < screenHeight; i++)
    {
        for (int j = 0; j < screenWidth; j++)
        {
            double L = 0.21 * image[i * screenWidth + j].r + 0.72 * image[i * screenWidth + j].g + 0.07 * image[i * screenWidth + j].b;
            double Lavg = (image[i * screenWidth + j].r + image[i * screenWidth + j].g + image[i * screenWidth + j].b) / 3.0;
 
            image[i * screenWidth + j].r = image[i * screenWidth + j].r * (1 - exp(-L/Lavg));
            image[i * screenWidth + j].g = image[i * screenWidth + j].g * (1 - exp(-L/Lavg));
            image[i * screenWidth + j].b = image[i * screenWidth + j].b * (1 - exp(-L/Lavg));
        
        }
    }
 
    for (int i = 0; i < scene.objcount; i++) {
        delete scene.objects[i];
    }
}
 
// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);        // torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
 
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
 
    glutSwapBuffers();                     // Buffercsere: rajzolas vege
}
 
// Billentyuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( );         // d beture rajzold ujra a kepet
 
}
 
// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
        glutPostRedisplay( );                          // Ilyenkor rajzold ujra a kepet
}
 
// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
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