#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

/*Definiare strcturii pentru arbore*/
typedef struct nod {
	char* info;
	struct nod *st, *dr;
} TNod, *TArb, **AArb;

TArb ConstrFr(char *x)
{
/*alocam memorie pentru un element de timp TArb*/
	TArb a;
	a = (TArb)malloc(sizeof(struct nod));
		if(!a)
			return NULL;
/*alocam memorie pentru informatia pe care o bagam in nod*/
	a->info = (char*)malloc(10);
		if(!a->info)
		{
			free(a);
			return NULL;
		}
/*copiem in nod informatia pe care vrem sa o introducem*/
		strcpy(a->info,x);
		a->st = NULL;
		a->dr = NULL;

	return a;
}

void ConstrArb(TArb *a, char *x)
{
/*conditia de iesire din recursivitate*/
	if(x == NULL)
		return ;

/*in cazul in care info == null atunci apelez functia 
  care imi construieste frunza*/
	if( *a == NULL)
	   *a = ConstrFr(x);

/*daca informatia din nod este operator unar atunci apelam
recursiv functia pentru stanga*/
	if( strcmp(x, "sqrt") == 0)
		ConstrArb(&(*a)->st, strtok(NULL," \n"));

/*daca avem operator binar, apelam recursiv si
  pentru stanga si pentru dreapta*/
	if(strcmp(x,"+")==0 || strcmp(x,"-")==0 || strcmp(x,"*")==0 
	   || strcmp(x,"/")==0 || strcmp(x,"^")==0)
		{ 
			ConstrArb(&(*a)->st, strtok(NULL," \n"));
			ConstrArb(&(*a)->dr, strtok(NULL," \n"));
		}

	return ;
}

/*functia care verifica daca un string este operator;
intoarce 1 daca este operator sau 0 altfel*/
int isOperator(char *c)
{
	if(strcmp(c, "+")==0)
		return 1;
	if(strcmp(c, "-")==0)
		return 1;
	if(strcmp(c, "*")==0)
		return 1;
	if(strcmp(c, "^")==0)
		return 1;
	if(strcmp(c, "sqrt")==0)
		return 1;
	if(strcmp(c, "/")==0)
		return 1;
return 0;
}

/*functia care verifica daca un string este numar;
intoarce 1 daca este operator sau 0 altfel*/
int isNumber(char *c)
{ 
	if( (c[0]>='0'&&c[0]<='9') || (c[0]== '-'&& c[1] >='0'&& c[1]<='9') )
		return 1;
   return 0;
}

 /*functia care calculeaza valoarea expresiei si afiseaza 
   mesajele de eroare corespunzatoare*/
int eval( FILE *f, TArb a, int n, char v1[n][10], int v2[n],int *ok)
{/* pt ok=0 intoarce valoarea expresiei, -1 pt invalid expression
    -2 pt cazul in care variabila nu exista si 1 altfel*/
	int i,val=0;
	if(*ok == 0 && a != NULL)
{
if( isOperator(a->info)==0 && isNumber(a->info) == 0){
	for(i=1;i<=n;i++)
		if( strcmp(a->info, v1[i])==0 )
			{ 	val=v2[i];
				break;
			}
		/*atunci cand trece de for fara sa-l gasesaca pe a->info in vector*/
		  if( i == n+1)
			{
				fprintf(f,"Variable %s undeclared\n",a->info);
				*ok=-2;
				return -2;
			}	
		else return val;}
	else if(isOperator(a->info) == 0 && isNumber(a->info) == 1)
		return atoi(a->info);

	//valoarea returnata dupa apelul functiei pentru subarborele stang
	int left = eval(f,a->st,n,v1,v2,ok);
	/*valoarea returnata dupa apelul functiei pentru subarborele drept */
	int right = eval(f,a->dr,n,v1,v2,ok);

/*calculul valorii expresiei in functie de cei 2 subarbori*/
	if( strcmp(a->info, "+")==0&& *ok == 0)
		return left + right;
	

	else if( strcmp(a->info, "-")==0&& *ok == 0)
		return left - right;
		

 	else if( strcmp(a->info, "/")==0 && *ok == 0)
 		/* afisarea mesajului de eroare in functie de restrictia pusa*/
		if(right == 0)
		{	*ok=-1;
			fprintf(f,"Invalid expression\n"); 
			return -1;
		}
		else return left / right;
		

 	else if( strcmp(a->info, "*")==0 && *ok == 0)
		return left * right;


 	else if( strcmp(a->info, "^")==0&& *ok == 0 )
		return pow(left, right);
		

 	else if( strcmp(a->info, "sqrt")==0&& *ok == 0)
 		/* afisarea mesajului de eroare in functie de restrictia pusa*/
			if( left < 0)
		{	*ok=-1; 
			 fprintf(f,"Invalid expression\n");
			 return -1;
		}
		return sqrt(left);
}
return 1;
}

/* functie auxiliara - distruge toate nodurile
   si elibereaza informatia din noduri */
void distruge(TArb r) 	
{
	if (!r) return;
	distruge (r->st);
	distruge (r->dr);   
	free(r->info);
	free(r);         
}

/* distruge toate nodurile arborelui de la adresa a */
void DistrArb(TArb *a) 
{
	if (!(*a)) return; 
	distruge (*a);         
	*a = NULL;               
}

int main(int argc, char *argv[])
{
 int N, M,i,j,c;
 FILE *in;
 FILE *out;
 //deschiderea fisierelor
 in = fopen(argv[1],"rt");
	if(in == NULL)
		return -1;

 out = fopen(argv[2],"wt");
	if(out == NULL)
		return -1;
/*vector de delimitatori*/
	char delims[] = " =\n";
	size_t size=32;
	/* alocam memorie pentru vectorul in care urmeaza 
	   sa citim liniile din fisierul de input*/
	char *line = malloc(size);
	/*citim numarul de asignari de variabile*/
	getline(&line,&size,in);
	N = atoi(line);

	char var[N][10]; int intvar[N];
    /*citim fiecare asignare de variabila si le punem in cei 2 vectori,
      de valori si variabile*/
	for(i=1;i<=N;i++)
	{
/*citim cu getline fiecare linie, variabila o memoram in vectorul
  de stringuri var si valoarea ei in vectorul intvar*/
		getline(&line, &size, in);
		char *variabila = strtok(line, delims);
		strcpy(var[i], variabila);
		int valoare = atoi(strtok(NULL,delims));
		intvar[i] = valoare;
	}
	/*citim numarul de expresii*/
	getline(&line,&size,in);
	M = atoi(line);
	/*citirea celor M expresii ce trebuie evaluate*/
 	for(j=1; j<=M; j++)
	{
		/*initializam arborele cu null*/
		TArb arb = NULL;
		int ok=0; 
		getline(&line, &size, in);
		char *p = strtok(line," \n");
		/*apelarea functiilor scrise mai sus*/
		ConstrArb(&arb, p);
		c = eval(out,arb,N,var,intvar, &ok);
			if(ok == 0)
		fprintf(out,"%d\n",c);
		DistrArb(&arb);
	}
	free(line);
/*inchiderea fisierelor*/
 fclose(in);
 fclose(out);
 return 0;
}
