#include <stdlib.h>
#include <stdio.h>


void sochet(int n, int k)
{
	int* A = (int*)calloc(k, sizeof(int));
	int i;
	for (i = 0; i < k; ++i)
	{
		A[i] = i;
	}
	int p = k;
	int C = 0;
	while (p > -1)
	{
		for (i = 0; i < k; ++i)
		{
			printf("%d ", A[i]);
		}
//		++C;
		printf("\n");
		if (A[k-1] == n-1) --p;
		else p = k-1;
		if (p > -1)
		{
			for (i = k-1; i > p-1; --i)
			{
				A[i] = A[p]+i-p+1;
			}
		}
	}
//	printf("C = %d\n", C);
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Usage: prog <n><k>\n");
		return 1;
	}
	sochet(atoi(argv[1]),atoi(argv[2]));
}
