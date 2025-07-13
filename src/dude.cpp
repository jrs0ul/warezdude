#include <cmath>

#include "Usefull.h"
#include "dude.h"
//


//------------------------
void Dude::heal(){
	if (hp<100)
		if (hp>90)
			hp=100;
		else
			hp+=10;
}


//-------------------------------------
bool Dude::arkolidina( DArray<Dude>& chars, int count, float newx, float newy){

	for (int i=0;i<count;i++){
			
		if ((CirclesColide(newx,newy,10.0f,chars[i].x,chars[i].y,10.0f))&&(chars[i].id!=id)){
			
			if ((!chars[i].spawn)&&(!chars[i].shot))
				return true;
		}
	}

	return false;
}
//--------------------------------------
void Dude::respawn(){


		stim++;
		if (stim%5==0){
			if (frame==weaponCount*4+2) frame=weaponCount*4+3;
			else frame=weaponCount*4+2;
		}
		if (stim>=80) {
			spawn=false;
			stim=0;
			frame=(currentWeapon+1)*4-2;
			alive=true;



	}
}
//--------------------------------------------------
void Dude::rotate(float angle){
	this->angle+=angle;
	if (this->angle>=6.28f)
		this->angle=0;
}


//----------------------------------------------------
//judina heroju, speed-greitis, radius - herojaus bounding circle spindulys, map - zemelapio
//praeinamumu masyvas, dirx - ar juda x asimi, diry - ar juda y asimi
void Dude::move(float walkSpeed,float strifeSpeed, float radius, bool** map, int mapsizex, int mapsizey,
				DArray<Dude>& chars,int charcount, int* dirx, int* diry){

	D3DXVECTOR2 vl=MakeVector(walkSpeed,strifeSpeed,angle);

	float difx=vl.x;//cos(-angle)*speed;
	float dify=vl.y; //sin(angle)*speed;

	float newposx=x+difx;
	float newposy=y-dify;


	POINT p1; 
	POINT p2;
	POINT p3;
	POINT p4;

	POINT p5; 
	POINT p6;
	POINT p7;
	POINT p8; 

	

	/*D3DXVECTOR2 v2=MakeVector(radius,0,angle);
	D3DXVECTOR2 v3=MakeVector(radius,0,angle+3.14f/3.0f);
	D3DXVECTOR2 v4=MakeVector(radius,0,angle-3.14f/3.0f);
    

	p1.x=round((newposx+v2.x)/32.0f);
	p1.y=round((y-v2.y)/32.0f);

	p2.x=round((newposx+v3.x)/32.0f);
	p2.y=round((y-v3.y)/32.0f);

	p3.x=round((newposx+v4.x)/32.0f);
	p3.y=round((y-v4.y)/32.0f);*/


	p1.x=round((newposx-radius)/32.0f);
	if (p1.x>mapsizex-1)
		p1.x=mapsizex-1;
	p1.y=round((y-radius)/32.0f);
	if (p1.y>mapsizey-1)
		p1.y=mapsizey-1;

	p2.x=round((newposx+radius)/32.0f);
	if (p2.x>mapsizex-1)
		p2.x=mapsizex-1;
	p2.y=p1.y;

	p3.x=p1.x;
	p3.y=round((y+radius)/32.0f);
	if (p3.y>mapsizey-1)
		p3.y=mapsizey-1;

	p4.x=p2.x;
	p4.y=p3.y;

//--------

	/*p5.x=round((x+v2.x)/32.0f);
	p5.y=round((newposy-v2.y)/32);

	p6.x=round((x+v3.x)/32.0f);
	p6.y=round((newposy-v3.y)/32);

	p7.x=round((x+v4.x)/32.0f);
	p7.y=round((newposy-v4.y)/32);*/

	p5.x=round((x-radius)/32.0f);
	if (p5.x>mapsizex-1)
		p5.x=mapsizex-1;
	p5.y=round((newposy-radius)/32.0f);
	if (p5.y>mapsizey-1)
		p5.y=mapsizey-1;

	p6.x=round((x+radius)/32.0f);
	if (p6.x>mapsizex-1)
		p6.x=mapsizex-1;
	p6.y=p5.y;

	p7.x=p5.x;
	p7.y=round((newposy+radius)/32.0f);
	if (p7.y>mapsizey-1)
		p7.y=mapsizey-1;

	p8.x=p6.x;
	p8.y=p7.y;



	if (dirx)
		*dirx=0;
	if (diry)
		*diry=0;

	

	if ((!map[p1.y][p1.x])&&(!map[p2.y][p2.x])
		&&(!map[p3.y][p3.x])&&(!map[p4.y][p4.x])&&(!arkolidina(chars,charcount,newposx,y))
		&&((newposx)<=(mapsizex-1)*32.0f)&&((newposy)<((mapsizey-1)*32.0f))&&
		((newposx-radius)>=-radius)&&((y-radius)>=-radius)

		){

			if ((x>newposx)&&(dirx))
				*dirx=1;
			else
				if ((x<newposx)&&(dirx))
					*dirx=2;

			x=x+difx;

	}

	if ((!map[p5.y][p5.x])&&(!map[p6.y][p6.x])
		&&(!map[p7.y][p7.x])&&(!map[p8.y][p8.x])&&(!arkolidina(chars,charcount,x,newposy))
		&&((newposx)<=(mapsizex-1)*32.0f)&&((newposy)<((mapsizey-1)*32.0f))&&
		((newposx-radius)>=-radius)&&((y-radius)>=-radius)){

			if ((y>newposy)&&(diry))
				*diry=1;
			else
				if ((y<newposy)&&(diry))
					*diry=2;

			y=y-dify;


	}

	if ((*diry>0)||(*dirx>0)){
		tim++;
		if (tim>=30) tim=0;
		frame=tim/10+currentWeapon*4;
	}


}
//--------------------------------------------------------------
void Dude::draw(Picture& sprite, Renderer3D& rend,
				int pskx, int scrx, int psky, int scry,
				int pushx, int posx, int pushy, int posy
				){
					sprite.Blt(rend.spraitas,round(x)-((pskx-scrx)*32)+pushx-posx,
						round(y)-((psky-scry)*32)+pushy-posy,
						frame,1.0f,1.0f,1.0f,angle-(3.14f/2.0f),
						r,g,b,true); 
}
//----------------------------------------------------------
bool Dude::atack(bool useBullets, bool isMine, CBulletContainer* bulcon){

	if ((useBullets)&&(ammo<=0))
		return false;


	canAtack=false;
	frame=(currentWeapon+1)*4-1;

	if (useBullets){
		if (ammo<=0)
			return false;


		Bullet newbul;

		newbul.x=x+(cos(-angle)*10.0f);
		newbul.y=y-(sin(angle)*10.0f);
		newbul.parentID=id;
		newbul.tim=0;
		newbul.angle=angle;
		newbul.frame=isMine;
		newbul.exists=true;
		newbul.explode=false;
		newbul.explodetim=0; 
		newbul.isMine=isMine;


		ammo--;

		bulcon->add(newbul);
	}

	return true;
}
//-------------------------------
int Dude::hitIt(Dude& enemy, float vectorx, float vectory, int damage){

 if ((CirclesColide(x+vectorx,y-vectory,4.0f,	enemy.x,enemy.y,8.0f))
	 &&(enemy.id!=id)){
		enemy.hp-=damage;
		enemy.hit=true;
		return enemy.id;
 }
 return -1;
}

//-------------------------------
void Dude::splatter(){

	stim++;
	frame=weaponCount*4;
	if (stim>30){
		frame=weaponCount*4+1;	
		if (stim>50){
			//alive=false;
			shot=false;
			stim=0;
			spawn=true; 
		}
	}
}
//------------------------------
void Dude::appearInRandomPlace(bool** map, int mapwidth, int mapheight){
	
	start.x=rand()%mapwidth;
	start.y=rand()%mapheight;
	while (map[start.y][start.x]){
		start.x=rand()%mapwidth;
		start.y=rand()%mapheight;
	}
	x=start.x*32.0f;
	y=start.y*32.0f;

}
//----------------------------------------
void Dude::damageAnim(){
	g=b=0;
	r=1.0f;
	hittim++;
	if (hittim>=10){
		hittim=0;
		hit=false;
		r=g=b=1.0f;
	}
}
//------------------------------
void Dude::reload(int time){
	reloadtime++;
	if (reloadtime>time/2)
	 frame=(currentWeapon+1)*4-2;
	if (reloadtime==time){
		canAtack=true;
		reloadtime=0;
	}

}
//-------------------------------
void Dude::chageNextWeapon(){
	currentWeapon++;
	if (currentWeapon>=weaponCount)
		currentWeapon=0;
	frame=(currentWeapon+1)*4-2;
	
}