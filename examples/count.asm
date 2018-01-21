#Count to a number then stop
        set     N E           #memory location to count with
        set     COUNTTO 7     #number to count up to
 
LOOP    load    N             #load N into accumulator
        inc                   #increase it by one
        store   N             #store N back to memory
        subl    COUNTTO       #subtract COUNTTO from N
END     gotoc   END           #end the program if N>=COUNTO
        goto    LOOP          #if not, repeat the LOOP
