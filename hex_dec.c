#include <stdio.h>
#include <unistd.h>

int main(void)
{
	unsigned long value;
	while(scanf("%li", &value) == 1)
		printf("%lu\n",value);
	return 0;
}
