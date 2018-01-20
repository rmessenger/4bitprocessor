#Two Digit Hex Counting
        set     D0 E     #first digit
        set     D1 F     #second digit
 
START   inc              #increment accumulator (starts at zero)
        store   D0       #store accumulator to first digit
        gotoc   START    #go back to START if accumulator didn't roll over
        load    D1       #if it did, load the second digit
        inc              #increment it
        store   D1       #store it
        zero             #set accumulator to zero
        goto    START    #go back to the START
