#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
     char buff1[100]; strcpy(buff1, "This is a test string");
     char buff2[100]; strcpy(buff2, "This is a test string.");
     char buff3[100]; strcpy(buff3, "This is a test string/");
     char buff4[100]; strcpy(buff4, "This is a test string..");
     char buff5[100]; strcpy(buff5, "This is a test string//");
     char buff6[100]; strcpy(buff6, "This is a test.. string//");
     char buff7[100]; strcpy(buff7, "This/ is/ a test string");
     char buff8[100]; strcpy(buff8, "This//. is/. a test string");

     // Checking for .. and //
     int foundPeriod = 0;
     int foundSlash = 0;
     for(int i = 0; i < 100; i++){
		 printf("%c\n", buff8[i]);
         if (buff8[i] == '.'){
             if (foundPeriod){
                 printf("Found \"..\" in url. Not allowed you dirty hacker.\n");
                 return -1;
             }
             foundPeriod = 1;
         }
         else{
             foundPeriod = 0;
         }
         if (buff8[i]== '/'){
             if (foundSlash){
                 printf("Found \"//\" in url. Not allowed you dirty hacker.\n");
                 return -1;
             }
             foundSlash = 1;
         }
         else{
             foundSlash = 0;
         }
     }

}
