#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include <unistd.h>
#include "rsa-sample.h"
#define CHNK_SIZE 50

int x, y, n, t, i, flag;
long int e[50], d[50], j;
char msg[100];
int prime(long int);
void encryption_key();
long int cd(long int);
void encrypt();
void decrypt();

int rsa_init()
{
	x = 19; //first prime number
	y = 11; //second prime number
	n = x * y;

	t = (x-1) * (y-1);

	encryption_key();

	return 0;
}

int prime(long int pr)
{
	int i;
	j = sqrt(pr);
	for(i = 2; i <= j; i++)
	{
		if(pr % i == 0)
			return 0;
	}
	return 1;
}

//function to generate encryption key
void encryption_key()
{
	int k;
	k = 0;
	for(i = 2; i < t; i++)
	{
		if(t % i == 0)
			continue;
		flag = prime(i);
		if(flag == 1 && i != x && i != y)
		{
			e[k] = i;
			flag = cd(e[k]);
			if(flag > 0)
			{
				d[k] = flag;
				k++;
			}
			if(k == 99)
				break;
		}
	}
}

long int cd(long int a)
{
	long int k = 1;
	while(1)
	{
		k = k + t;
		if(k % a == 0)
			return(k / a);
	}
}

//function to encrypt the message
void encrypt(rsa_variable *variable )
{
	long int pt, ct, key = e[0], k, len;
	int i = 0;
	len = strlen(variable->message);
	int m[CHUNK_SIZE] ;
	for(int i = 0 ; variable->message[i]!=0 ; i++){
		m[i] = (int) variable->message[i];
	}
	int * temp = variable->temp_message;
	int * en = variable->en_message ;
	// printf("len = %ld\n", len);
	while(i != len)
	{
		pt = m[i];
		pt = pt - 96;
		k = 1;
		for(j = 0; j < key; j++)
		{
			k = k * pt;
			k = k % n;
		}
		temp[i] = k;
		ct = k + 96;
		en[i] = ct;
		i++;
	}
	en[i] = -1;
	// printf("\n\nTHE ENCRYPTED MESSAGE IS\n");
	// for(i = 0; en[i] != -1; i++)
	// 	printf("%c", (char)en[i]);
	// printf("\n");
}

//function to decrypt the message
void decrypt(rsa_variable * variable)
{
	long int pt, ct, key = d[0], k;
	int * en = variable->en_message ; 
	int * temp = variable->temp_message;
	int * m = variable->decrypted;
	int i = 0;
	while(en[i] != -1)
	{
		ct = temp[i];
		k = 1;
		for(int j = 0; j < key; j++)
		{
			k = k * ct;
			k = k % n;
		}
		pt = k + 96;
		m[i] = pt;
		i++;
	}
	m[i] = -1;
	// printf("\n\nTHE DECRYPTED MESSAGE IS\n");
	// for(i = 0; m[i] != -1; i++)
	// 	printf("%c", (char)m[i]);
	// printf("\n");
}