#include "mpi.h"
#include <stdio.h>
#include<string.h>
#include<stdlib.h>


int max(int a, int b)
{
	if (a>b) return a;
	else return b;
}

int matchfirst(char word[100], int sub, int flen)
{
	int i;
	int lensub = flen - sub;
	for (i = 0; i<lensub; i++)
	{
		if (word[i] != word[sub + i])
		{
			return -1;
		}

	}
	return 0;
}


int match(char word[100], int sub, int flen)
{
	char prev = word[sub - 1];//d index
	//printf("\nprev=%c", prev);
	int lensub = flen - sub;// length of that k
	//printf("\nlensub=%d", lensub);
	int j; int h;

	for (j = sub - 1; j >= 0; j--)
	{
		//printf("\nj=%d ", j);
		if (j - 1 >= 0)
		{
			if (prev == word[j - 1])continue;
		}
		int flag = 0; int count = 0;
		for (h = j; h <= j + lensub - 1; h++)
		{
			if (word[h] != word[sub + count])
			{
				flag = 1;
			}
			count++;
		}

		if (flag == 1)
			continue;
		else
			return j;

	}
	//printf("here");
	return -1;

}

int main(int argc, char* argv[])
{
	int rank, size;
	char doc[100]; 
	char word[100];
	char send[10];
	int dlen;
	int flen;
	int si;
	int gstable[100]; int  bstable[128];
	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	size--;
	int *start_endi=(int *)malloc(sizeof(int)*size * 2);
	if (rank == 0)
	{
		    
			fprintf(stdout,"\nEnter the document\n");
			fflush(stdout);
		gets(doc);
		dlen = strlen(doc);
		
		fprintf(stdout,"\nEnter the word to find\n");
		fflush(stdout);
		gets(word);
		flen = strlen(word);
		int i = 0, wcount = 0;
		while (doc[i] != '\0')
		{
			if (doc[i] == ' ')
				wcount++;
			i++;
		}
		wcount++;
		int nwpp = wcount / size;
		int* lena = (int*)malloc(sizeof(int)*wcount);
		int j = 0;
		doc[dlen] = '\0';
		
		for (i = 0; i<wcount; i++)
		{
			lena[i] = 0;
			while (doc[j] != ' ' && doc[j] != '\0')
			{
				lena[i]++; j++;
			}
			j++;

		}
		

		int count = 0,ei;
		int lengthindex = 0; int k1 = 0;
		for (i = 1; i <= size; i++)
		{
			si = count; ei = si;
			for (j = lengthindex; j < lengthindex + nwpp; j++)
			{

				ei += lena[j];
			}
			lengthindex += nwpp;
			ei = ei + nwpp - 1;
			start_endi[k1++] = si;
			start_endi[k1++] = ei - 1;
			count = ei + 1;
		}
		/*for (i = 1; i <= size; i++){
			si = 0;
			char *send = (char*)malloc((start_endi[i * 2 + 1] - start_endi[i * 2] + 1)*sizeof(char));
			for (j = start_endi[i * 2]; j <= start_endi[i * 2 + 1]; j++)
			{
				send[si++] = doc[j];
			}
			MPI_Send(&si, 1, MPI_INT, i, i, MPI_COMM_WORLD);
			MPI_Send(send, si, MPI_CHAR, i, i, MPI_COMM_WORLD);

		}*/

			//gstable=(int*)malloc(sizeof(int)*flen);
			
			//bstable = (int*)malloc(sizeof(int)*128);
			for (i = 0; i <= 127; i++)
			{
				bstable[i] = flen;
			}
			for (i = 0; i <= flen - 2; i++)
			{
				bstable[(int)word[i]] = flen - 1 - i;
			}
			int k, sub, result, f;
			for (k = 1; k <= flen - 1; k++)
			{
				sub = flen - k; f = 0;
				result = match(word, sub,flen);
				if (result >= 0)
				{
					
					gstable[k] = sub - result;
					continue;
				}
				for (sub = flen - k + 1; sub <= flen - 1; sub++)
				{
					result = matchfirst(word, sub,flen);
					if (result == 0)
					{
					
						gstable[k] = sub - result;
						f = 1;
						break;
					}
				}
				if (f == 0)
				{

					gstable[k] = flen;
				}
			}

			/*fprintf(stdout,"\n\n*********BAD SYMBOL TABLE*********\n");
			fflush(stdout);
			for (i = 0; i <= 127; i++)
			{
				fprintf(stdout,"\n%d ", bstable[i]); 
				fflush(stdout);
			}
			printf("\n\n******GOOD SUFFIX TABLE IS **********\n");
			for (i = 1; i <= flen - 1; i++)
			{
				fprintf(stdout,"\n%d", gstable[i]);
				fflush(stdout);
			}
	*/
				
	}// rank 0 
	
	    MPI_Bcast(&flen, 1, MPI_INT, 0, MPI_COMM_WORLD);
	    MPI_Bcast(word, flen, MPI_CHAR, 0, MPI_COMM_WORLD); 
	    MPI_Bcast(&dlen, 1, MPI_INT, 0, MPI_COMM_WORLD);
	    MPI_Bcast(doc, dlen, MPI_CHAR, 0, MPI_COMM_WORLD);
		MPI_Bcast(bstable, 128, MPI_INT,0, MPI_COMM_WORLD);
        MPI_Bcast(gstable, flen, MPI_INT,0, MPI_COMM_WORLD);
		MPI_Bcast(start_endi, size * 2, MPI_INT, 0, MPI_COMM_WORLD);
	
		
		if (rank != 0){
			word[flen] = '\0';
			int i;
			doc[dlen] = '\0';
			
	
			
			
		  
			int fl=0, shift;

			dlen=start_endi[(rank-1)*2+1];
			 i=start_endi[(rank-1)*2]+flen-1;
			 
			 int occ = 0;
			while (i <= dlen )
			{

				int k = 0;
				
				while (k <= flen - 1 && doc[i - k] == word[flen - 1 - k])
				{
					k = k + 1;
				}

				if (k == flen) { fprintf(stdout, "\nFound by %d at : %d", rank, (i - (flen - 1))); fflush(stdout); i = i + 1; fl = 1; occ++; continue; }
				int num = (int)doc[i];
				int d1 = max(bstable[num] - k, 1);
				int d2 = gstable[k];
				if (k == 0) { shift = d1; }
				else if (k > 0) shift = max(d1, d2);
				else {}
				i = i + shift;
			}
			
			fprintf(stdout,"\nThe no of occurrences by process %d is %d", rank, occ);
			fflush(stdout);
			
			
		}

		MPI_Finalize();
		return 0; 

}