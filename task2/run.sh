set -x

gcc ver0.c && ./a.out
gcc ver0.c -O2 && ./a.out

gcc ver1.c && ./a.out
gcc ver1.c -O2 && ./a.out

gcc ver2.c && ./a.out
gcc ver2.c -O2 && ./a.out

gcc ver3.c && ./a.out
gcc ver3.c -O2 && ./a.out

gcc ver4.c && ./a.out
gcc ver4.c -O2 && ./a.out

gcc ver5.c && ./a.out
gcc ver5.c -O2 && ./a.out

gcc ver6.c && ./a.out
gcc ver6.c -O2 && ./a.out

gcc -mavx ver7.c && ./a.out
gcc -mavx -march=native ver7.c && ./a.out
gcc -mavx -mfma ver7.c && ./a.out
gcc -mavx -march=native -mfma ver7.c && ./a.out
gcc -mavx -O2 ver7.c && ./a.out
gcc -mavx -O2 -march=native ver7.c && ./a.out
gcc -mavx -O2 -mfma ver7.c && ./a.out
gcc -mavx -O2 -march=native -mfma ver7.c && ./a.out

gcc -mavx ver8.c && ./a.out
gcc -mavx -march=native ver8.c && ./a.out
gcc -mavx -mfma ver8.c && ./a.out
gcc -mavx -march=native -mfma ver8.c && ./a.out
gcc -mavx -O2 ver8.c && ./a.out
gcc -mavx -O2 -march=native ver8.c && ./a.out
gcc -mavx -O2 -mfma ver8.c && ./a.out
gcc -mavx -O2 -march=native -mfma ver8.c && ./a.out

gcc -mavx512f ver8.c && ./a.out
