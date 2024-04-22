#include "start_screen.h"
#include "draw_helpers.h"
#include "matrix_mapping.h"

static int p_coords[14];
int num_p_coords = 14;

int num_r_coords = 17;
int r_coords[17]; 
int r_coords2[17];

int num_e_coords = 12;
int e_coords[12]; 


int num_s_coords = 13;
int s_coords1[13]; 
int s_coords2[13];

int s_coords3[13];

int num_t_coords = 9;
int t_coords1[9]; 
int t_coords2[9]; 

int num_a_coords = 16;
int a_coords[16]; 

void populate_word()
{
  const int offsetShift = 0;
  // p 
  p_coords[0] = lookup(0,3+offsetShift);
  p_coords[1] = lookup(1,3+offsetShift);
  p_coords[2] = lookup(2,3+offsetShift);
  p_coords[3] = lookup(3,3+offsetShift); 
  p_coords[4] = lookup(4,3+offsetShift);
  p_coords[5] = lookup(5,3+offsetShift);
  p_coords[6] = lookup(0,4+offsetShift);
  p_coords[7] = lookup(0,5+offsetShift);
  p_coords[8] = lookup(0,6+offsetShift);
  p_coords[9] = lookup(1,6+offsetShift); 
  p_coords[10] = lookup(2,6+offsetShift);
  p_coords[11] = lookup(2,5+offsetShift);
  p_coords[12] = lookup(2,4+offsetShift);
  p_coords[13] = lookup(2,3+offsetShift);
  
  // r stem
  r_coords[0] = lookup(0,9+offsetShift);
  r_coords[1] = lookup(1,9+offsetShift);
  r_coords[2] = lookup(2,9+offsetShift);
  r_coords[3] = lookup(3,9+offsetShift); 
  r_coords[4] = lookup(4,9+offsetShift);
  r_coords[5] = lookup(5,9+offsetShift);
  // r loop
  r_coords[6] = lookup(0,10+offsetShift);
  r_coords[7] = lookup(0,11+offsetShift);
  r_coords[8] = lookup(0,12+offsetShift);
  r_coords[9] = lookup(1,12+offsetShift); 
  r_coords[10] = lookup(2,12+offsetShift);
  r_coords[11] = lookup(2,11+offsetShift);
  r_coords[12] = lookup(2,10+offsetShift);
  r_coords[13] = lookup(2,9+offsetShift);
  // r leg
  r_coords[14] = lookup(3,10+offsetShift); 
  r_coords[15] = lookup(4,11+offsetShift);
  r_coords[16] = lookup(5,12+offsetShift);


  // E 
  e_coords[0] = lookup(0,14+offsetShift+1); 
  e_coords[1] = lookup(0,15+offsetShift+1);
  e_coords[2] = lookup(0,16+offsetShift+1);
  e_coords[3] = lookup(2,14+offsetShift+1);
  e_coords[4] = lookup(2,15+offsetShift+1);
  e_coords[5] = lookup(2,16+offsetShift+1); 
  e_coords[6] = lookup(5,14+offsetShift+1);
  e_coords[7] = lookup(5,15+offsetShift+1);
  e_coords[8] = lookup(5,16+offsetShift+1);

  e_coords[9] = lookup(1,14+offsetShift+1);
  e_coords[10] = lookup(3,14+offsetShift+1);
  e_coords[11] = lookup(5,15+offsetShift+1);
  e_coords[11] = lookup(4,14+offsetShift+1);



  // S 1 
  s_coords1[0] = lookup(0,18+offsetShift+2); 
  s_coords1[1] = lookup(0,19+offsetShift+2);
  s_coords1[2] = lookup(0,20+offsetShift+2);
  s_coords1[3] = lookup(2,18+offsetShift+2);
  s_coords1[4] = lookup(2,19+offsetShift+2);
  s_coords1[5] = lookup(2,20+offsetShift+2); 

  s_coords1[6] = lookup(5,19+offsetShift+2);
  s_coords1[7] = lookup(5,20+offsetShift+2);

  s_coords1[8] = lookup(1,18+offsetShift+2);
  s_coords1[9] = lookup(5,19+offsetShift+2);

  s_coords1[10] = lookup(3,20+offsetShift+2);
  s_coords1[11] = lookup(4,20+offsetShift+2);
  s_coords1[12] = lookup(5,18+offsetShift+2);

  // S 2
  s_coords2[0] = lookup(0,23+offsetShift+2); 
  s_coords2[1] = lookup(0,24+offsetShift+2);
  s_coords2[2] = lookup(0,25+offsetShift+2);
  s_coords2[3] = lookup(2,23+offsetShift+2);
  s_coords2[4] = lookup(2,24+offsetShift+2);
  s_coords2[5] = lookup(2,25+offsetShift+2); 

  s_coords2[6] = lookup(5,24+offsetShift+2);
  s_coords2[7] = lookup(5,25+offsetShift+2);

  s_coords2[8] = lookup(1,23+offsetShift+2);
  s_coords2[9] = lookup(5,24+offsetShift+2);

  s_coords2[10] = lookup(3,25+offsetShift+2);
  s_coords2[11] = lookup(4,25+offsetShift+2);
  s_coords2[12] = lookup(5,23+offsetShift+2);


  // start 

  // S 3
  s_coords3[0] = lookup(9,3+offsetShift); 
  s_coords3[1] = lookup(9,4+offsetShift);
  s_coords3[2] = lookup(9,5+offsetShift);
  s_coords3[3] = lookup(11,4+offsetShift);
  s_coords3[4] = lookup(11,5+offsetShift);


  s_coords3[5] = lookup(14,4+offsetShift);
  s_coords3[6] = lookup(14,5+offsetShift);
  s_coords3[7] = lookup(13,5+offsetShift);
 
  s_coords3[8] = lookup(14,3+offsetShift);

  s_coords3[9] = lookup(10,3+offsetShift);
  s_coords3[10] = lookup(11,3+offsetShift);

  s_coords3[11] = lookup(14,3+offsetShift);
  s_coords3[12] = lookup(12,5+offsetShift);

    // T
  t_coords1[0] = lookup(9,9+offsetShift); 
  t_coords1[1] = lookup(9,10+offsetShift);
  t_coords1[2] = lookup(9,11+offsetShift);

  t_coords1[3] = lookup(12,10+offsetShift); 
  t_coords1[4] = lookup(9,10+offsetShift);
  t_coords1[5] = lookup(10,10+offsetShift);
  t_coords1[6] = lookup(11,10+offsetShift);
  t_coords1[7] = lookup(13,10+offsetShift);
  t_coords1[8] = lookup(14,10+offsetShift);
  
   // A
  a_coords[0] = lookup(9,15+offsetShift); 
  a_coords[1] = lookup(9,16+offsetShift);
  a_coords[2] = lookup(9,17+offsetShift);

  a_coords[3] = lookup(11,15+offsetShift); 
  a_coords[4] = lookup(11,16+offsetShift);
  a_coords[5] = lookup(11,17+offsetShift);
  
  a_coords[6] = lookup(10,15+offsetShift);
  a_coords[7] = lookup(10,17+offsetShift);
  
  a_coords[8] = lookup(11,15+offsetShift);
  a_coords[9] = lookup(11,17+offsetShift);

  a_coords[10] = lookup(12,15+offsetShift);
  a_coords[11] = lookup(12,17+offsetShift);

  a_coords[12] = lookup(13,15+offsetShift);
  a_coords[13] = lookup(13,17+offsetShift);

  a_coords[14] = lookup(14,15+offsetShift);
  a_coords[15] = lookup(14,17+offsetShift);

   // r stem
   int r_offset = 11;
  r_coords2[0] = lookup(14-5,9+offsetShift+r_offset);
  r_coords2[1] = lookup(15-5,9+offsetShift+r_offset);
  r_coords2[2] = lookup(16-5,9+offsetShift+r_offset);
  r_coords2[3] = lookup(17-5,9+offsetShift+r_offset); 
  r_coords2[4] = lookup(18-5,9+offsetShift+r_offset);
  r_coords2[5] = lookup(19-5,9+offsetShift+r_offset);
  // r loop
  r_coords2[6] = lookup(14-5,10+offsetShift+r_offset);
  r_coords2[7] = lookup(14-5,11+offsetShift+r_offset);
  r_coords2[8] = lookup(14-5,12+offsetShift+r_offset);
  r_coords2[9] = lookup(15-5,12+offsetShift+r_offset); 
  r_coords2[10] = lookup(16-5,12+offsetShift+r_offset);
  r_coords2[11] = lookup(16-5,11+offsetShift+r_offset);
  r_coords2[12] = lookup(16-5,10+offsetShift+r_offset);
  r_coords2[13] = lookup(16-5,9+offsetShift+r_offset);
  // r leg
  r_coords2[14] = lookup(19-7,10+offsetShift+r_offset); 
  r_coords2[15] = lookup(13,11+offsetShift+r_offset);
  r_coords2[16] = lookup(19-5,12+offsetShift+r_offset);

  // T 2 
  int x_t_offset = 0;
  int y_t_offset = 16;
  t_coords2[0] = lookup(9+x_t_offset,9+offsetShift+y_t_offset); 
  t_coords2[1] = lookup(9+x_t_offset,10+offsetShift+y_t_offset);
  t_coords2[2] = lookup(9+x_t_offset,11+offsetShift+y_t_offset);

  t_coords2[3] = lookup(12+x_t_offset,10+offsetShift+y_t_offset); 
  t_coords2[4] = lookup(9+x_t_offset,10+offsetShift+y_t_offset);
  t_coords2[5] = lookup(10+x_t_offset,10+offsetShift+y_t_offset);
  t_coords2[6] = lookup(11+x_t_offset,10+offsetShift+y_t_offset);
  t_coords2[7] = lookup(13+x_t_offset,10+offsetShift+y_t_offset);
  t_coords2[8] = lookup(14+x_t_offset,10+offsetShift+y_t_offset);

}

void draw_start_screen(uint32_t color)
{
  populate_word();
  for(int i =0; i < num_p_coords;i++)
  {
   // Serial.println(p_coords[i]);
    matrix.setPixel(p_coords[i],color);
  }

  for(int i =0; i < num_r_coords;i++)
  {
    
//    Serial.println(r_coords[i]);
    matrix.setPixel(r_coords[i],color);
  }
  
  for(int i =0; i < num_e_coords;i++)
  {
    
//    Serial.println(e_coords[i]);
    matrix.setPixel(e_coords[i],color);
  }
    
  for(int i =0; i < num_s_coords;i++)
  {
//    Serial.println("S");
//    Serial.println(s_coords1[i]);
    matrix.setPixel(s_coords1[i],color);
  }

    for(int i =0; i < num_s_coords;i++)
  {
//    Serial.println("S 2");
//    Serial.println(s_coords2[i]);
    matrix.setPixel(s_coords2[i],color);
  }
  // start 

     for(int i =0; i < num_s_coords;i++)
  {
//    Serial.println("S 3");
//    Serial.println(s_coords3[i]);
    matrix.setPixel(s_coords3[i],color);
  }
       for(int i =0; i < num_t_coords;i++)
  {
//    Serial.println("T 1");
//    Serial.println(t_coords1[i]);
    matrix.setPixel(t_coords1[i],color);
  }

         for(int i =0; i < num_a_coords;i++)
  {
//    Serial.println("A");
//    Serial.println(a_coords[i]);
    matrix.setPixel(a_coords[i],color);
  }

           for(int i =0; i < num_r_coords;i++)
  {
//    Serial.println("R 2");
//    Serial.println(r_coords2[i]);
    matrix.setPixel(r_coords2[i],color);
  }

  for(int i =0; i < num_t_coords;i++)
  {
//    Serial.println("T 2");
//    Serial.println(t_coords2[i]);
    matrix.setPixel(t_coords2[i],color);
  }
  
  
  
  
  

  matrix.show();

  
}
