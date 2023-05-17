#include <fcntl.h>

int main() {
    int t = open("/home/students/k/kuzinov.vs/lab3/file", O_RDONLY);
    printf("%d\n", t);
    return 0;

}
