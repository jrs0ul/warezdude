#include <cstdio>
#include <cstring>
#include "maplist.h"

//----------------------
MapList::MapList(){
 FILE* f;
 
 maps=0;

 f=fopen("maps/list.txt","rt");
 if (f!=NULL){
  fscanf(f,"%d",&_count);
 

  fgetc(f);
  if (_count){
   
   maps=new char*[_count];
   

   for (int i=0;i<_count;i++){
      maps[i]=new char[255];
	  char c='.';
	  int a=0;
	  while ((c!=EOF)&&(c!='\n')){
	   c=fgetc(f);
	   if ((c!=EOF)&&(c!='\n'))
	     maps[i][a]=c;
	   else
	     maps[i][a]='\0';
	   a++;
	  }

	   
   }
  }

  
 }
 current=0;
 fclose(f);
}

void MapList::Destroy(){
	if (_count){
	 for (int i=0;i<_count;i++){
		 if (maps[i]){
          delete []maps[i];

		  
		 }
	 }
      delete []maps;
	  maps=0;
	  
	}
	//}

}


void MapList::getMapName(int i, char*name){
  if ((i>=0)&&(i<_count)){
   strcpy(name,maps[i]);  
  }
  else name=0;
}