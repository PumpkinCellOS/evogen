import time

def is_prime(n):
    # TEST: convert to number
    n = int(n)
    if n < 2:
        return False
    i = 2
    c = 0
    while i < n:
        if n % i == 0:
            c += 1
        i += 1
    return c == 0

i = 2000
start = time.time()
while i < 2400:
    print(str(i) + ": " + str(is_prime(str(i))))
    i += 1
print("Time: " + str(time.time() - start) + "s")
