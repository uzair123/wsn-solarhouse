#include "stdio.h"
#include "time.h"



void wait ( int seconds )
{
  clock_t endwait;
  endwait = clock () + seconds * CLOCKS_PER_SEC ;
  while (clock() < endwait) {}
}

int main() {
	FILE* f = fopen("output.encoded", "r");
	char buffer[130];
	
	printf("~K\n");
	wait (2);
	printf("rm encoded\n");
	wait(1);
	printf("write encoded | null\n");
	wait (2);
		
	while(!feof(f)) {
		fread(buffer, 1, 126, f);
		printf("%s\n", buffer);
		wait(1);		
	}
	
	fclose(f);
	
}



