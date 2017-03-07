/* 

  genuino.ino : Sample arduino entry point that uses the
  arduinoGA (Copyright (C) 2010 Nuno Alves) library.

  By Alberto Rogério e Silva - UFRPE  

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <GA.h>
#include <Servo.h>

//Vetor direção
int vetorDir[9];
//Passando na ordem: centroA, centroB, inclinaçãoA; (raio fixado em 1) 
char ang[] = {'0', '1', '0', '2', '0', '3', '0', '4', '1', '2', '0'};
int cxA = 1, cyA = 10, cxB = 1, cyB = 15, inclina = 30, giro = 0;
unsigned int raio = 1;
float dist_min;

//initializing the arduinoGA library
int populationsize=99;   //population must be in [0..99]
int numgenerations=20;  //number of generations cannot exceed 65k
//the unit of bit mutation is per thousand. For example, if we set the
//bitmutation to 10, approximately every 10 out of 1k bits will be mutated
int bitmutation=1;
//we start in the generation 0
int generation=0, pos_servo = 0;
Servo servo;

void setup()
{
   Serial.begin(9600);
   randomSeed(analogRead(0));
   servo.attach(9);
   dist_min = sqrt(pow((cxA - cxB),2) + pow((cyA - cyB),2)) - 2*raio;
   
   for(pos_servo = 0; pos_servo < inclina; pos_servo += 1)     
   {                                             
      servo.write(pos_servo);                
      delay(15);                         
   } 
}      

//instanitating the class. this will initialize a set of random genes.
GA ga(populationsize,numgenerations,bitmutation);

float AngToRad(int angulo)
{
  return angulo * 3.14159265358 / 180.0;
}

int fit_dist(int ang)
{
  float x,y;
  float dist, fit;
  float rad = AngToRad(ang);
  
  x = cos(rad) + cxA;
  y = sin(rad) + cyA;
  dist = sqrt(pow((x - cxB),2) + pow((y - cyB),2)) - raio;  
  fit = map(dist*100, (dist_min + 2*raio)*100, dist_min*100, 0, 32);  
  return fit;  
}

void gerarVetorDir(char dir[])
{
    int valor = atoi(dir);
    Serial.println(valor);
    Serial.println(valor, BIN);
    for(int i = 0; i < sizeof(vetorDir)/sizeof(int); i++)
    {
      vetorDir[i] = bitRead(valor, i);
    }
}

void loop()
{

  if (generation==0) { Serial.println(""); Serial.println(""); }
  
  while (generation<numgenerations)
	{ 
       
       //======================================================================
       //Fitness Evaluation
       //======================================================================       
       //NOTE: due to memory constraints, the fitness value CANNOT(!!!) be greater
       //than 100.
       //======================================================================
       //begin: Fitness Evaluation
               for (int i=0 ; i < populationsize ; i++)
               {
        		unsigned int t0_fitness=0;
                        long aux;
        	        unsigned int t0_a_population=ga.read_t0_a_population(i); 
        	        unsigned int t0_b_population=ga.read_t0_b_population(i); 
               
        	        //we are evaluating the fitness on 32 bits... dividing by
        	        //ga.t0_a_population[i] and ga.t0_b_population[i]
        
                        aux = t0_a_population/100 + t0_b_population/100; 
                        
                        //Converter ângulo
                        int novo_ang = map(aux, 0, 1310, 0, 359);
                        t0_fitness = fit_dist(novo_ang);  
        	   	ga.write_t0_fitness(i,t0_fitness);
        	}
                //end: Fitness Evaluation
          
                //the next steps perform the genetic optimization process
                //behind the scenes. It mates the best individuals and create the 
                //next generation with a new set of genes.   
          
                ga.process_generation(generation);
                //ga.reportStatistics(generation,1);
                ga.prepare_next_generation();
                generation=generation+1;
                
                if (generation == 20) 
                { 
                  
                  Serial.println("");
                  giro = map(ga.read_best_candidates(), 0, 1310, 0, 359);
                  Serial.print(giro);
                  Serial.println("");
                  
             
                  if(giro > inclina)
                  {
                    for(pos_servo = inclina; pos_servo < giro; pos_servo += 1)   
                    {                                   
                      servo.write(pos_servo);                 
                      delay(15);                        
                    }
                  }
                  else
                  {
                    for(pos_servo = inclina; pos_servo > giro; pos_servo -= 1)   
                    {                                   
                      servo.write(pos_servo);                 
                      delay(15);                        
                    }
                  } 
                }
	}
        

}//end of void loop()


