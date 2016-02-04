/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: student.c 217 2012-02-28 17:10:06Z spanel $
 */

/*
 * Datum:   2012/04/07
 * Student: Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 */

#include "student.h"
#include "transform.h"

#include <memory.h>

/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej */

S_Renderer * studrenCreate()
{
    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");

    /* inicializace default rendereru */
    renInit(&renderer->base);

    /* nastaveni ukazatelu na vase upravene funkce */
    renderer->base.projectTriangleFunc = studrenProjectTriangle;
    renderer->base.calcReflectanceFunc = studrenPhongReflectance;
    
    /* vychozi koeficient ostrosti -- nastavime jako u referencniho reseni */
    renderer->mat_shininess = 1.0f;
    //renderer->mat_shininess = 10.0f;

    return (S_Renderer *)renderer;
}

/****************************************************************************
 * Funkce vyhodnoti Phonguv osvetlovaci model pro zadany bod
 * a vraci barvu, kterou se bude kreslit do frame bufferu
 * point - souradnice bodu v prostoru
 * normal - normala k povrchu v danem bode */

S_RGBA studrenPhongReflectance(S_Renderer *pRenderer, const S_Coords *point, const S_Coords *normal)
{
    S_Coords    lvec, eyevec, reflect;
    double      diffuse, specular, reflect_scalar;
    double      r, g, b;

    /* If = Ia + Id + Is */
    S_RGBA      color;

    IZG_ASSERT(pRenderer && point && normal);

    /* vektor ke zdroji svetla */
    lvec = makeCoords(pRenderer->light_position.x - point->x,
                      pRenderer->light_position.y - point->y,
                      pRenderer->light_position.z - point->z);
    coordsNormalize(&lvec);

    /* vektor ke kamere */
    eyevec = makeCoords(-point->x,
                        -point->y,
                        -pRenderer->camera_dist - point->z);
    coordsNormalize(&eyevec);

    /* ambientni cast -- Ia = (Al * Am) + (As * Am) */
    /* As je barva sceny, muzeme zanedbat */
    r = pRenderer->light_ambient.red * pRenderer->mat_ambient.red;
    g = pRenderer->light_ambient.green * pRenderer->mat_ambient.green;
    b = pRenderer->light_ambient.blue * pRenderer->mat_ambient.blue;

    /* difuzni cast -- Id = Dl * Dm * LambertTerm */
    /* LambertTerm = dot(N, L) */
    diffuse = lvec.x * normal->x + lvec.y * normal->y + lvec.z * normal->z;
    if( diffuse > 0 )
    {
        r += diffuse * pRenderer->light_diffuse.red * pRenderer->mat_diffuse.red;
        g += diffuse * pRenderer->light_diffuse.green * pRenderer->mat_diffuse.green;
        b += diffuse * pRenderer->light_diffuse.blue * pRenderer->mat_diffuse.blue;
    }

    /* odraziva cast -- Is = Sm * Sl * pow( max( dot(R, E), 0.0), f ) */
    /* R = reflect(-L, N) = 2 * dot(N, L) * N - L */
    reflect_scalar = 2 * (normal->x * lvec.x + normal->y * lvec.y + normal->z * lvec.z);
    reflect.x = reflect_scalar * normal->x - lvec.x;
    reflect.y = reflect_scalar * normal->y - lvec.y;
    reflect.z = reflect_scalar * normal->z - lvec.z;

    specular = pow(
        MAX(reflect.x * eyevec.x + reflect.y * eyevec.y + reflect.z * eyevec.z, 0.0f),
        ((S_StudentRenderer*)pRenderer)->mat_shininess);

    r += specular * pRenderer->light_specular.red * pRenderer->mat_specular.red;
    g += specular * pRenderer->light_specular.green * pRenderer->mat_specular.green;
    b += specular * pRenderer->light_specular.blue * pRenderer->mat_specular.blue;

    /* saturace osvetleni*/
    r = MIN(1, r);
    g = MIN(1, g);
    b = MIN(1, b);

    /* kreslici barva */
    color.red = ROUND2BYTE(255 * r);
    color.green = ROUND2BYTE(255 * g);
    color.blue = ROUND2BYTE(255 * b);
    return color;
}

/****************************************************************************
 * Nova fce pro rasterizace trojuhelniku do frame bufferu
 * vcetne interpolace z souradnice a prace se z-bufferem
 * a interpolaci normaly pro Phonguv osvetlovaci model
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky */

void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         )
{
    // oblast trojuhelniku
    int min[2] = {
        MIN(x1, MIN(x2, x3)),
        MIN(y1, MIN(y2, y3))
    };
    int max[2] = {
        MAX(x1, MAX(x2, x3)),
        MAX(y1, MAX(y2, y3))
    };

    // oriznuti rozmerem okna
    min[0] = MAX(min[0], 0);
    min[1] = MAX(min[1], 0);
    max[0] = MIN(max[0], pRenderer->frame_w - 1);
    max[1] = MIN(max[1], pRenderer->frame_h - 1);

    // pro urceni hranic trojuhelnika pouzijeme pineduv algoritmus
    // hranova funkce je rovnice Ax + By + C = 0
    // A,B je normalovy vektor primky, tzn. (-dy, dx)
    // C vyjadrime jako x1y2 - x2y1
    int A[3] = {
        { y1 - y2 },
        { y2 - y3 },
        { y3 - y1 }
    };
    int B[3] = {
        { x2 - x1 },
        { x3 - x2 },
        { x1 - x3 }
    };
    int C[3] = {
        { x1 * y2 - x2 * y1 },
        { x2 * y3 - x3 * y2 },
        { x3 * y1 - x1 * y3 }
    };

    // do hranove funkce dosadime protejsi vrcholy
    // provedeme normalizaci, aby kladna strana byla uvnitr oblasti
    int s0 = A[0] * x3 + B[0] * y3 + C[0];
    int s1 = A[1] * x1 + B[1] * y1 + C[1];
    int s2 = A[2] * x2 + B[2] * y2 + C[2];

    if (s0 < 0) {
        A[0] *= -1; B[0] *= -1; C[0] *= -1;
        s0 *= -1;
    }
    if (s1 < 0) {
        A[1] *= -1; B[1] *= -1; C[1] *= -1;
        s1 *= -1;
    }
    if (s2 < 0) {
        A[2] *= -1; B[2] *= -1; C[2] *= -1;
        s2 *= -1;
    }

    double _s0 = 1.0f / (double)s0;
    double _s1 = 1.0f / (double)s1;
    double _s2 = 1.0f / (double)s2;

    /* gourandovo stinovani */
    //S_RGBA color, b1, b2, b3;
    //b1 = pRenderer->calcReflectanceFunc(pRenderer, v1, n1);
    //b2 = pRenderer->calcReflectanceFunc(pRenderer, v2, n2);
    //b3 = pRenderer->calcReflectanceFunc(pRenderer, v3, n3);

    // vyplnovani pinedovim algoritmem
    for (int y = min[1]; y <= max[1]; ++y) {
        int e[3] = {
            A[0] * min[0] + B[0] * y + C[0],
            A[1] * min[0] + B[1] * y + C[1],
            A[2] * min[0] + B[2] * y + C[2]
        };

        for (int x = min[0]; x <= max[0]; ++x) {
            // uvnitr trojuhelniku jsou cisla kladna
            if (e[0] >= 0 && e[1] >= 0 && e[2] >= 0) {

                // spocitame barycentricke koeficienty u,v,w
                double u = e[1] * _s1;
                double v = e[2] * _s2;
                double w = e[0] * _s0;

                // nemelo by nastat, protoze jsme uvnitr trojuhelniku
                //if (u < 0.0f || u > 1.0f) continue;
                //if (v < 0.0f || v > 1.0f) continue;
                //if (w < 0.0f || w > 1.0f) continue;

                // bod v trojuhelniku pred projekci ve 3D
                S_Coords pt = makeCoords(
                    u * v1->x + v * v2->x + w * v3->x,
                    u * v1->y + v * v2->y + w * v3->y,
                    u * v1->z + v * v2->z + w * v3->z);

                /* gourandovo stinovani */
                //double r = u * b1.red   + v * b2.red   + w * b3.red;
                //double g = u * b1.green + v * b2.green + w * b3.green;
                //double b = u * b1.blue  + v * b2.blue  + w * b3.blue;
                //double a = u * b1.alpha + v * b2.alpha + w * b3.alpha;

                //r = MIN(255.0f, MAX(r, 0.0f));
                //g = MIN(255.0f, MAX(g, 0.0f));
                //b = MIN(255.0f, MAX(b, 0.0f));
                //a = MIN(255.0f, MAX(a, 0.0f));

                //color.red   = ROUND2BYTE(r);
                //color.green = ROUND2BYTE(g);
                //color.blue  = ROUND2BYTE(b);
                //color.alpha = ROUND2BYTE(a);

                // vektor smerujici od bodu ke kamere
                S_Coords P_Cam = makeCoords(
                    -pt.x,
                    -pt.y,
                    -pRenderer->camera_dist - pt.z);

                // vykreslime jen blizsi body
                double depth = sqrt(P_Cam.x * P_Cam.x + P_Cam.y * P_Cam.y + P_Cam.z * P_Cam.z);
                if (depth <= DEPTH(pRenderer, x, y))
                {
                    // normala bodu
                    S_Coords n = makeCoords(
                        u * n1->x + v * n2->x + w * n3->x,
                        u * n1->y + v * n2->y + w * n3->y,
                        u * n1->z + v * n2->z + w * n3->z);
                    coordsNormalize(&n);

                    S_RGBA color;
                    color = pRenderer->calcReflectanceFunc(pRenderer, &pt, &n);

                    /* vybarvi objekt normalou */
                    //color.red = ROUND2BYTE(255.0f * (n.x / 2.0f + 0.5f));
                    //color.green = ROUND2BYTE(255.0f * (n.y / 2.0f + 0.5f));
                    //color.blue = ROUND2BYTE(255.0f * (-n.z / 2.0f + 0.5f));
                    //color.alpha = 255;

                    DEPTH(pRenderer, x, y) = depth;
                    PIXEL(pRenderer, x, y) = color;
                }
            }

            // Ei(x+1,y) = Ei(x,y) + dy
            e[0] += A[0];
            e[1] += A[1];
            e[2] += A[2];
        }
    }
}

/*****************************************************************************
 * Vykresli i-ty trojuhelnik modelu
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i)
{
    S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci ve 3D pred projekci */
    S_Coords    nn, nn1, nn2, nn3;      /* normala trojuhelniku po transformaci */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;

    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles));

    /* z modelu si vytahneme trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);

    /* transformace vrcholu matici model */
    trTransformVertex(&aa, cvecGetPtr(pModel->vertices, triangle->v[0]));
    trTransformVertex(&bb, cvecGetPtr(pModel->vertices, triangle->v[1]));
    trTransformVertex(&cc, cvecGetPtr(pModel->vertices, triangle->v[2]));

    /* promitneme vrcholy trojuhelniku na obrazovku */
    trProjectVertex(&u1, &v1, &aa);
    trProjectVertex(&u2, &v2, &bb);
    trProjectVertex(&u3, &v3, &cc);

    /* transformace normaly trojuhelniku matici model */
    trTransformVector(&nn, cvecGetPtr(pModel->trinormals, triangle->n));
    trTransformVector(&nn1, cvecGetPtr(pModel->normals, triangle->v[0]));
    trTransformVector(&nn2, cvecGetPtr(pModel->normals, triangle->v[1]));
    trTransformVector(&nn3, cvecGetPtr(pModel->normals, triangle->v[2]));

    /* normalizace normaly */
    coordsNormalize(&nn);
    coordsNormalize(&nn1);
    coordsNormalize(&nn2);
    coordsNormalize(&nn3);

    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }

    /* rasterizace celeho trojuhelniku */
    studrenDrawTriangle(pRenderer,
                    &aa, &bb, &cc, &nn1, &nn2, &nn3,
                    u1, v1, u2, v2, u3, v3
                    );
}

/*****************************************************************************
 *****************************************************************************/
