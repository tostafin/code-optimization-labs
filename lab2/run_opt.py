import subprocess

options = "-O2"
print(f"With {options}")
for i in range(1, 5 + 1):
    print(f"chol{i}.c: ", end="")
    print(subprocess.call(([f"gcc -lm {options} chol{i}.c && ./a.out 4000"]), shell=True))

print(f"chol6.c: ", end="")
print(subprocess.call(([f"gcc -lm -mavx {options} chol6.c && ./a.out 4000"]), shell=True))
