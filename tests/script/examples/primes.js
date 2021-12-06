function is_prime(n)
{
    // TEST: convert to number
    n = new Number(n);
    if(n < 2) return false;
    let i = 2;
    let c = 0;
    while(i < n)
    {
        if(n % i == 0)
            c++;
        i++;
    }
    return c == 0;
}

let i = 2000;
let start = new Date();
while(i < 2400) console.log(i, ": ", is_prime(new String(i++)));
console.log("Time: " + (new Date().getTime() - start.getTime()) + "s");
