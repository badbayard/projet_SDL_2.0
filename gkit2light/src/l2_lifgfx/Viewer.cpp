
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "Viewer.h"

using namespace std;


Viewer::Viewer() : App(1024, 768), mb_cullface(true), mb_wireframe(false), b_draw_grid(true), b_draw_axe(true), b_draw_animation(false)
{
}


void Viewer::help()
{
    printf("HELP:\n");
    printf("\th: help\n");
    printf("\tc: (des)active GL_CULL_FACE\n");
    printf("\tw: (des)active wireframe\n");
    printf("\ta: (des)active l'affichage de l'axe\n");
    printf("\tg: (des)active l'affichage de la grille\n");
    printf("\tz: (des)active l'affichage de la courbe d'animation\n");
    printf("\tfleches/pageUp/pageDown: bouge la caméra\n");
    printf("\tCtrl+fleche/pageUp/pageDown: bouge la source de lumière\n");
    printf("\tSouris+bouton gauche: rotation\n");
    printf("\tSouris mouvement vertical+bouton droit: (de)zoom\n");
}



int Viewer::init()
{
    srand(time(NULL));
    // etat par defaut openGL
    glClearColor(0.5f, 0.5f, 0.9f, 1);
    glClearDepthf(1);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    if (mb_cullface)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);        // good for debug
    glEnable(GL_TEXTURE_2D);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glAlphaFunc(GL_GREATER, 0.5);
    //glEnable(GL_ALPHA_TEST);

    m_anim.init( "data/animation/anim1.ani");

    m_camera.lookat( Point(0,0,0), 150 );
    gl.light( Point(0, 20, 20), White() );

    init_axe();
    init_grid();
    init_cube();
    init_quad();
    init_cylindre();

    init_cone();
    init_sphere();

    init_terrain();
    init_foret(100);
    init_arbre();
    init_cubemap();
    init_eau();


    return 0;
}



void Viewer::init_axe()
{
    m_axe = Mesh(GL_LINES);
    m_axe.color( Color(1, 0, 0));
    m_axe.vertex( 0,  0, 0);
    m_axe.vertex( 1,  0, 0);

    m_axe.color( Color(0, 1, 0));
    m_axe.vertex( 0,  0, 0);
    m_axe.vertex( 0,  1, 0);

    m_axe.color( Color( 0, 0, 1));
    m_axe.vertex( 0,  0, 0);
    m_axe.vertex( 0,  0, 1);
}



void Viewer::init_grid()
{
    m_grid = Mesh(GL_LINES);

    m_grid.color( Color(1, 1, 1));
    int i,j;
    for(i=-5;i<=5;++i)
        for(j=-5;j<=5;++j)
        {
            m_grid.vertex( -5, 0, j);
            m_grid.vertex( 5, 0,  j);

            m_grid.vertex( i, 0, -5);
            m_grid.vertex( i, 0, 5);

        }
}



void Viewer::init_cube()
{
    //                          0           1           2       3           4           5       6           7
    static float pt[8][3] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };
    static int f[6][4] = {    {0,1,2,3}, {5,4,7,6}, {2,1,5,6}, {0,3,7,4}, {3,2,6,7}, {1,0,4,5} };
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };
    int i,j;

    m_cube = Mesh(GL_TRIANGLE_STRIP);
    m_cube.color( Color(1, 1, 1) );

    m_cube_texture = read_texture(0, "data/debug2x2red.png");

    for (i=0;i<6;i++)
    {
        m_cube.normal(  n[i][0], n[i][1], n[i][2] );

        m_cube.texcoord( 0,0 );
        m_cube.vertex( pt[ f[i][0] ][0], pt[ f[i][0] ][1], pt[ f[i][0] ][2] );

        m_cube.texcoord( 1,0);
        m_cube.vertex( pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );

        m_cube.texcoord(0,1);
        m_cube.vertex(pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );

        m_cube.texcoord(1,1);
        m_cube.vertex( pt[ f[i][2] ][0], pt[ f[i][2] ][1], pt[ f[i][2] ][2] );

        m_cube.restart_strip();
    }
}



void Viewer::init_cylindre( )
{
    float a;
    const int N = 20;
    int i,j;

    cylcouv = Mesh(GL_TRIANGLE_FAN);
    cylcouv.color(1,0,0);
    cylcouv.normal(0,-1,0);
    cylcouv.vertex(0,0,0);

    for(j=0; j<=N; ++j)
    {
        a = 2.0 * M_PI * j/N;
        cylcouv.normal(0,-1,0);
        cylcouv.vertex(cos(a), 0, sin(a));
    }

    cyl = Mesh(GL_TRIANGLE_STRIP);
    for(i=0; i<=N; ++i)
    {
        a = 2.0 * M_PI * i/N;
        cyl.normal(cos(a), 0, sin(a));
        cyl.vertex(cos(a), 0, sin(a));
        cyl.normal(cos(a), 0, sin(a));
        cyl.vertex(cos(a), 1, sin(a));
    }
}



void Viewer::init_cone( )
{
    cone = Mesh(GL_TRIANGLE_STRIP);
    float a;
    const int N = 20;
    int i;

    for(i=0; i<=N; ++i)
    {
        a = 2.0 * M_PI * i/N;

        cone.normal(cos(a), 0, sin(a));
        cone.vertex(cos(a), 0, sin(a));
        cone.normal(0,0,0);
        cone.vertex(0,1,0);
    }
}


void Viewer::init_sphere( )
{
    sph = Mesh(GL_TRIANGLE_STRIP);

    int i,j;
    float a,b,a2;
    const int N= 20;
    const int M = 40;

    for(i=0; i<N; ++i)
    {
        a = M_PI * i/N;
        a2 = M_PI * (i+1)/N;

        for(j=0; j<=M; ++j)
        {
            b = 2.0 * M_PI * j/M;
            sph.normal(sin(a2) * cos(b), cos(a2), sin(a2) * sin(b));
            sph.vertex(sin(a2) * cos(b), cos(a2), sin(a2) * sin(b));
            sph.normal(sin(a) * cos(b), cos(a), sin(a) * sin(b));
            sph.vertex(sin(a) * cos(b), cos(a), sin(a) * sin(b));
        }
    }
    sph.restart_strip();
}



void Viewer::init_quad()
{
    m_quad = Mesh(GL_TRIANGLE_STRIP);
    m_quad.color( Color(1, 1, 1));

    m_quad_texture = read_texture(0, "data/papillon.png");

    m_quad.normal(  0, 0, 1 );

    m_quad.texcoord(0,0 );
    m_quad.vertex(-1, -1, 0 );

    m_quad.texcoord(1,0);
    m_quad.vertex(  1, -1, 0 );

    m_quad.texcoord(0,1);
    m_quad.vertex( -1, 1, 0 );

    m_quad.texcoord( 1,1);
    m_quad.vertex(  1,  1, 0 );
}

void Viewer::init_eau()
{
    water= Mesh(GL_TRIANGLE_STRIP);
    water.color(Color(0,0,1.0));

    int x, z;

    for(z=1 ; z<alti.height()-2 ; z++){
        for(x=1 ; x<alti.width()-1 ; x++){
            CalculN(x,z);
            water.vertex(x, alti(x,z).r, z);

            CalculN(x,z+1);
            water.vertex(x, alti(x,z+1).r, (z+1));
        }
        water.restart_strip();
    }

}


void Viewer::CalculN(int x,int y) {
    Vector G(x-1, alti(x-1,y).r, y);
    Vector D(x+1, alti(x+1,y).r, y);
    Vector H(x, alti(x,y+1).r, y+1);
    Vector B(x, alti(x,y-1).r, y-1);

    Vector GD, HB, N;
    GD = normalize(D-G);
    HB = normalize(B-H);
    N = cross(GD,HB);
    ter.normal(N.x, N.y, N.z);
}

void Viewer::init_terrain()
{
     ter= Mesh(GL_TRIANGLE_STRIP);
    alti = read_image("data/terrain/hawaii11.jpg");
    tex = read_texture(0,"data/terrain/island.jpg");

    int x, z;

    for(z=1 ; z<alti.height()-2 ; z++){
        for(x=1 ; x<alti.width()-1 ; x++){
            CalculN(x,z);
            ter.texcoord(((float) x)/alti.width(),((float) z)/alti.height());
            ter.vertex(x, alti(x,z).r, z);

            CalculN(x,z+1);
            ter.texcoord(((float) x)/alti.width(), ((float) z+1)/alti.height());
            ter.vertex(x, alti(x,z+1).r, (z+1));
        }
        ter.restart_strip();
    }
}



void Viewer::init_arbre()
{
    abri = read_texture(0,"data/billboard/arbre.png");

    m_arbre.normal(  0, 0, 1 );

    m_arbre.texcoord(0,0 );
    m_arbre.vertex(-1, -1, 0 );

    m_arbre.texcoord(1,0);
    m_arbre.vertex(  1, -1, 0 );

    m_arbre.texcoord(0,1);
    m_arbre.vertex( -1, 1, 0 );

    m_arbre.texcoord( 1,1);
    m_arbre.vertex(  1,  1, 0 );
}

void Viewer::init_foret(int n) // permet de crée un tableau pour pouvoir mettre des cordonnée aléatoire qui seront compris dans les dimensions de l'image
{
    nb_arbre=n;
    pos_arbre=new int[n*2];


    for(int i=0; i<n*2;i+=2)
    {
        pos_arbre[i]= rand()% alti.height();
        pos_arbre[i+1]= rand()% alti.width();
    }
}



void Viewer::arbre (Transform T) { //dessine un arbre
        const int N=8;
        int i;
        gl.texture(abri);
        gl.alpha(0.3f);
        for (i=0;i<N;i++)
        {
            gl.model(Translation(0,1,0)*T*RotationY(i*360/N));
            gl.draw(m_quad);
        }
        gl.lighting(true);


}

void Viewer::draw_foret() //dessine la foret et utilise le tableau pos arbre pour le multiplier par la matrice de translation et le scale en récuprérant l'altitude pour l'axe y pour que l'arbre soit sur la map
{

    for(int i=0;i<nb_arbre*2; i+= 2)
    {
        float cor = (alti(pos_arbre[i],pos_arbre[i+1]).r);
        if(cor==0)
        {
            //on n'affiche pas d'arbre si les coordonnées sont dans l'eau (noir)
        }
        else
        {
            arbre(Translation(-90,-0.1,-90)*Translation(pos_arbre[i]*0.2,cor*11,pos_arbre[i+1]*0.2) );
        }

    }
}

void Viewer::avion( const Transform& _T)
{
    Transform T = _T*RotationX(-90)*RotationZ(-90)*Scale(0.3,0.3,0.3);
    gl.model(T*Scale(1,10,1));
    gl.draw(sph);
    gl.model(T*Translation(0,3,1) * Scale(10,1,0.2));
    gl.draw(m_cube);

    gl.model(T*Translation(0,-9,0.5) * Scale(5,0.5,0.1));
    gl.draw(m_cube);

    //pour les réacteurs
    gl.model(T*Translation(4,3,0) * Scale(1,1,1));
    gl.draw(cyl);

    gl.model(T*Translation(4,3,0)*Scale(1,1,1));
    gl.draw(cylcouv);

    gl.model(T*Translation(4,4,0)*Scale(1,1,1)*RotationX(180));
    gl.draw(cylcouv);

    //pour le deuxiéme réacteur

    gl.model(T*Translation(-4,3,0)*Scale(1,1,1));
    gl.draw(cyl);

    gl.model(T*Translation(-4,3,0)*Scale(1,1,1));
    gl.draw(cylcouv);

    gl.model(T*Translation(-4,4,0)*Scale(1,1,1)*RotationX(180));
    gl.draw(cylcouv);

    //les ailes
    gl.model(T*Translation(0,-9,1)*Scale(0.2,0.1,0.5));
    gl.draw(m_cube);

    gl.model(T*Translation(0,-9,1.5)*Scale(2.5,0.5,0.1));
    gl.draw(m_cube);
}
/*
void Viewer::DrawTreeRec(const Transform& T, int n, float a) {
	if(n == 0){
        return;
	}
	else
    {
	gl.model(T * Scale(1,a,1));
	gl.draw(cyl);
	Transform Branche = T * Translation(0,2,0);

	Transform B1 = Branche * RotationZ(45) * Scale(1/2,a/2,1/2);
	gl.model(B1);
	//gl.draw(cyl);


	DrawTreeRec(B1,n-1,a/2);
	Transform B2 = Branche * RotationZ(-45) * Scale(1/2,a/2,1/2);
	gl.model(B2);
	//gl.draw(cyl);


	DrawTreeRec(B2,n-1,a/2);
    }
} //marche pas*/

/* ne marche pas aussi :(
void Viewer::init_Arbre3D()
{
    int i,j,z;
    branche_inclinaison = new int[5];
    branche_rotation = new int[5];
    branche_taille = new float[5];
    branche_autre=new int[5];

    for(i=0;i<5;i++)
    {
        branche_inclinaison[i]=new int[3];

        for(j=0;j<3;j++)
        {
           branche_inclinaison[i][j] =rand()%60;
        }
        branche_taille[i]=new float[3];

        for(z=0;z<3;z++)
        {
            branche_taille[i][z]= 1+(rand() % 6 + 1)/10.0;
        }
        branche_rotation[i] = new int[3];
        branche_rotation[i][0] = rand() % 40;
        branche_rotation[i][1] = rand() % 40 + 120;
        branche_rotation[i][2] = rand() % 40 + 240;
        branche_autre[i]=rand()%4;
    }

}

void Viewer::Arbre3D(const Transform& T,float n,float a)  // n=0 et a=1.0
{
    float taille= a;

    if(n > 6)
    {
    }
    gl.model(T * Scale(0.2*a,taille*10,0.2*a));
    gl.draw(cyl);

    //draw_cylindre(T*Scale(0.2*a,taille*10,0.2*a));
    Transform b =T*Translation(0,2*taille,0)*RotationT(branche_rotation[n][0])*RotationZ(branche_inclinaison[n][0]);
    Arbre3D(b,n+1,a/1.2);

    Transform b2 =T*Translation(0,2*taille,0)*RotationT(branche_rotation[n][1])*RotationZ(branche_inclinaison[n][1]);
    Arbre3D(b2,n+1,a/1.2);

    if(branche_autre[n]>2){
        Transform b3= T*Translation(0,2*height,0)*RotationT(branche_rotation[n][2])*RotationZ(branche_inclinaison[n][2])
        Arbre3D(b3,n+1,a/1.2);
    }
}
*/

void Viewer::init_cubemap()
{
     m_cubemap_texture =read_texture(0,"data/cubemap/skybox2.jpg");
    //                          0           1           2       3           4           5       6           7
    static float pt[8][3] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };
    static int f[6][4] = {    {0,1,2,3}, {5,4,7,6}, {2,1,5,6}, {0,3,7,4}, {3,2,6,7}, {1,0,4,5} };
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };

static float uv[6][4][2] = {
        { {1.0/4,1.0/3},{1.0/2,1.0/3},{1.0/2,0},{1.0/4,0} },    //bas
        { {1.0/2,2.0/3},{1.0/4,2.0/3},{1.0/4,1}, {1.0/2,1} },    //haut
        { {3.0/4,1.0/3},{1.0/2,1.0/3},{1.0/2,2.0/3},{3.0/4,2.0/3} },    //droite
        { {1.0/4,1.0/3},{0,1.0/3},{0,2.0/3},{1.0/4,2.0/3} },    //gauche
        { {1,1.0/3},{3.0/4,1.0/3},{3.0/4,2.0/3},{1,2.0/3} },    //arrière
        { {1.0/2,1.0/3},{1.0/4,1.0/3},{1.0/4,2.0/3},{1.0/2,2.0/3}, }     //devant
    };

    int i,j;

    m_cubemap = Mesh(GL_TRIANGLE_STRIP);
    m_cubemap.color( Color(1, 1, 1) );

    for (i=0;i<6;i++)
    {
        m_cubemap.normal(  -n[i][0], -n[i][1], -n[i][2] );

        m_cubemap.texcoord( uv[i][3][0], uv[i][3][1] );
        m_cubemap.vertex(pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );

        m_cubemap.texcoord( uv[i][2][0], uv[i][2][1] );
        m_cubemap.vertex( pt[ f[i][2] ][0], pt[ f[i][2] ][1], pt[ f[i][2] ][2] );

        m_cubemap.texcoord( uv[i][0][0], uv[i][0][1] );
        m_cubemap.vertex( pt[ f[i][0] ][0], pt[ f[i][0] ][1], pt[ f[i][0] ][2] );

        m_cubemap.texcoord( uv[i][1][0], uv[i][1][1] );
        m_cubemap.vertex( pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );

        m_cubemap.restart_strip();
    }
}

void Viewer::cubemap()
{

    gl.lighting(false);
    gl.texture(m_cubemap_texture);
    gl.model( Scale(135,135,135) ); //taille du cubemap
    gl.draw( m_cubemap );
    gl.lighting(true);
}



int Viewer::render( )
{
    // Efface l'ecran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Deplace la camera, lumiere, etc.
    manageCameraLight();

    // donne notre camera au shader
    gl.camera(m_camera);

   gl.texture(m_quad_texture);
    gl.model( Tquad );
    gl.draw(m_quad);

    gl.texture(m_cube_texture);
    gl.model(Translation( -3, 5, 0 ));
    gl.draw(m_cube);

    //cylindre
    gl.model(Identity());
    gl.draw(cylcouv);
    gl.draw(cyl);
    gl.model(Translation(0,1,0)*RotationX(180));
    gl.draw(cylcouv);

    //cone
    gl.model(Translation(2,0,0));
    gl.draw(cone);
    gl.draw(cylcouv);

    //sphere
    gl.model(Translation(-2,1,0));
    gl.draw(sph);

    avion(Tplane); //RotationX(90)
    //cubemap
    cubemap();

    //pour le terrain
    gl.texture(tex);
    gl.model(Translation(-90,-0.1,-90)*Scale(0.2,11,0.2));
    gl.draw(ter);

    //pour la forêt
    draw_foret();

    //eau
    gl.model(Translation(-90,0.2,-90)*Scale(0.2,0,0.2));
    gl.draw(water);


    return 1;
}



int Viewer::update( const float time, const float delta )
{


	int te = int (time/1000);
	float r =(time/1000)-te;
	int tes =te+1;
	te = te % m_anim.nb_points();
	tes = tes % m_anim.nb_points();
	Vector p = r * Vector(m_anim[tes]) + (1-r) * Vector(m_anim[te]);

	float angle;
	Vector DIR=m_anim[tes]-m_anim[te];
	DIR=normalize(DIR);
	Vector X(1,0,0);
	float cosa=dot(X,DIR);
	angle=acos(cosa);

	if(DIR.z > 0) angle = -angle;
	Tplane=Translation(p)*RotationY(degrees(angle))*RotationX(25); //permet a l'avion de s'incliner et de tourner

	t_water=Translation(0,0.2*cos(time/800)/2,0);


//papi
    Tquad = Translation( 3, 5, 0 ) * Rotation( Vector(0,0,1), 0.1f*time);
    return 1;
}



void Viewer::manageCameraLight()
{
    // recupere les mouvements de la souris pour deplacer la camera, cf tutos/tuto6.cpp
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
    // deplace la camera
    if((mb & SDL_BUTTON(1)) &&  (mb& SDL_BUTTON(3)))                 // le bouton du milieu est enfonce
        m_camera.translation( (float) mx / (float) window_width(), (float) my / (float) window_height());         // deplace le point de rotation
    else if(mb & SDL_BUTTON(1))                      // le bouton gauche est enfonce
        m_camera.rotation( mx, my);       // tourne autour de l'objet
    else if(mb & SDL_BUTTON(3))                 // le bouton droit est enfonce
        m_camera.move( my);               // approche / eloigne l'objet
    if (key_state(SDLK_PAGEUP) && (!key_state(SDLK_LCTRL))) { m_camera.translation( 0,0.01); }
    if (key_state(SDLK_PAGEDOWN) && (!key_state(SDLK_LCTRL))) { m_camera.translation( 0,-0.01); }
    if (key_state(SDLK_LEFT) && (!key_state(SDLK_LCTRL))) { m_camera.translation(  0.01,0); }
    if (key_state(SDLK_RIGHT) && (!key_state(SDLK_LCTRL))) { m_camera.translation( -0.01,0); }
    if (key_state(SDLK_UP) && (!key_state(SDLK_LCTRL))) { m_camera.move( 1); }
    if (key_state(SDLK_DOWN) && (!key_state(SDLK_LCTRL))) { m_camera.move( -1); }


    // Deplace la lumiere
    const float step = 0.1f;
    if (key_state(SDLK_RIGHT) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(step,0,0)); }
    if (key_state(SDLK_LEFT) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(-step,0,0)); }
    if (key_state(SDLK_UP) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(0,0,-step)); }
    if (key_state(SDLK_DOWN) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(0,0,step)); }
    if (key_state(SDLK_PAGEUP) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(0,step,0)); }
    if (key_state(SDLK_PAGEDOWN) && key_state(SDLK_LCTRL)) { gl.light( gl.light()+Vector(0,-step,0)); }



    // (De)Active la grille / les axes
    if (key_state('h')) help();
    if (key_state('c')) { clear_key_state('c'); mb_cullface=!mb_cullface; if (mb_cullface) glEnable(GL_CULL_FACE);else glDisable(GL_CULL_FACE); }
    if (key_state('w')) { clear_key_state('w'); mb_wireframe=!mb_wireframe; if (mb_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
    if (key_state('g')) { b_draw_grid = !b_draw_grid; clear_key_state('g'); }
    if (key_state('a')) { b_draw_axe = !b_draw_axe; clear_key_state('a'); }
    if (key_state('z')) { b_draw_animation=!b_draw_animation; clear_key_state('z');}

    gl.camera(m_camera);
    //draw(cube, Translation( Vector( gl.light()))*Scale(0.3, 0.3, 0.3), camera);
    //draw_param.texture(quad_texture).camera(camera).model(Translation( 3, 5, 0 )).draw(quad);

    // AXE et GRILLE
    gl.model( Identity() );
    if (b_draw_grid) gl.draw(m_grid);
    if (b_draw_axe) gl.draw(m_axe);
    if (b_draw_animation) m_anim.draw(m_camera);

     // LIGHT
    gl.texture( 0 );
    gl.model( Translation( Vector( gl.light()))*Scale(0.3, 0.3, 0.3) );
    gl.draw(m_cube);
}

int Viewer::quit( )
{
    return 0;
}
