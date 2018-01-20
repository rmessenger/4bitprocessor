#Fibonacci numbers
#this program produces fibonacci numbers in the
#alternating memory locations X and Y
        set     X E      #the first memory location
        set     Y F      #the second memory location
 
        lit     1        #load a 1 into the accumulator
LOOP    store   X        #store it to X
        addm    Y        #add Y to the accumulator
        store   Y        #store the accumulator back to Y
        addm    X        #add X to the accumulator
        goto    LOOP     #repeat the loop
