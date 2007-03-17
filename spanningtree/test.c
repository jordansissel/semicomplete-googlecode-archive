
#define FORESTBLOCK (sizeof(int) * 8)
#define SETFORESTBIT(f, e) (*((f) + (e / FORESTBLOCK)) |= 1 << (e % FORESTBLOCK))
#define GETFORESTBIT(f, e) ((*((f) + (e / FORESTBLOCK)) & (1 << (e % FORESTBLOCK))) > 0)

int main() {
	int *f = malloc(300 / 8 + 1);

	memset(f, 0, 300 / 8 + 1);

	SETFORESTBIT(f, 0);
	SETFORESTBIT(f, 2);
	SETFORESTBIT(f, 4);

	SETFORESTBIT(f, 185);

	printf("%d\n", *f);

	printf("Bit %d: %d\n", 2, GETFORESTBIT(f, 2));
	printf("Bit %d: %d\n", 1, GETFORESTBIT(f, 1));
	printf("Bit %d: %d\n", 4, GETFORESTBIT(f, 4));
	printf("Bit %d: %d\n", 185, GETFORESTBIT(f, 185));
	return 0;
}

