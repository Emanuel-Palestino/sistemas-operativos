#include<stdio.h>
int main() {
	char entrada[255];
	fgets(entrada, 255, stdin);

	printf("Entrada Proceso 2: %s", entrada);
	return 0;
}
