#ifndef DECAL_H
#define DECAL_H


class Decal{
public:
    int x,y;
    float alpha;
    float r,g,b;
    int frame;

    Decal(){
        x=y=0;
        alpha=1.0f;
        r=g=b=1.0f;
        frame=0;
    }
    void draw(Picture& sprite, Renderer3D& rend,
                int pskx, int scrx, int psky, int scry,
                int pushx, int posx, int pushy, int posy);
};


#endif //DECAL_H
