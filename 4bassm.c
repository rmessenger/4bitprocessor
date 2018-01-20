/* Custom 4-bit Assembler----------------------------
 * by Robin Messenger
 *
 * This assembles machine code for a custom 4-bit
 * processor designed in LogicWorks, which uses the
 * following instructions:
 * 
 * LOAD  Y  load memory location Y to accumulator
 * STORE Y  store accumulator to memory location Y.
 * LIT   X  set accumulator to literal X
 * ADDL  X  add accumulator to literal X
 * SUBL  X  subtract literal X from accumulator
 * ORL   X  logical-or between X and accumulator
 * ADDM  Y  add accumulator to memory address Y
 * SUBM  Y  subtract memory address Y from accumulator
 * ORM   Y  logical-or between mem. addr. Y and acc.
 * NOT      logical-not on accumulator
 * ZERO     set accumulator to zero
 * NEG      two’s-complement of accumulator
 * INC      increment accumulator
 * DEC      decrement accumulator
 * GOTO  Y  jump to memory location Y
 * GOTOC Y  jump to memory location Y if carry-out low
 * ------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <ctype.h>


// Set of instruction names
const char *INSTR[]= 
    {"lit",   "addl",  "store", "not",   "zero",  "subl",
     "load",  "goto",  "addm",  "subm",  "orl",   "orm",
     "neg",   "inc",   "dec",   "gotoc", "set"};
const int INSTRC=17; 

// The number of operands each instruction takes
const int OPERANDS[]={1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,2};

// Set of hexadecimal digits
const char HEX[]={'0','1','2','3','4','5','6','7','8','9','A',
                  'B','C','D','E','F'};

// Size of hash table
const int HASHMOD=1024;

// Index of SET instruction
const int SET_INSTR=16;


/* Assembler for custom four-bit CPU designed in LogicWorks */
void main(int argc, char *argv[])
{
    //declare variables
    char ifilename[100],ofilename[100];
    char line[1000];
    char hex[1000];
    int hexc;
    FILE *ifile,*ofile;
    int addr;
    int instr_table[HASHMOD];
    int label_table[HASHMOD];
    int hash;
    int i,j;
    int foundi;
    int tostdout;
    int operands;
    int set_hash,set_instr;

    //create instruction hash
    for (i=0; i<HASHMOD; i++)
    {
        instr_table[i]=-1;
        label_table[i]=-1;
    }
    for (i=0; i<INSTRC; i++)
    {
        hash=0;
        for (j=0; INSTR[i][j]!=0; j++)
            hash=(
                    hash
                    +INSTR[i][j]
                    *((j+1)*(j+1)*(j+1))
                 )%HASHMOD;
        instr_table[hash]=i;
    }

    //check arguments for filename
    if (argc==3)
    {
        strcpy(ifilename,argv[1]);
        strcpy(ofilename,argv[2]);
        if (ofilename[0]=='-' && ofilename[1]==0)
            tostdout=1;
        else
            tostdout=0;
    }
    else if (argc==2)
    {
        strcpy(ifilename,argv[1]);
        strcpy(ofilename,argv[1]);
        for (i=0; ofilename[i]!='.'&&ofilename[i]!=0; i++);
        strcpy(ofilename+i,".hex");
        tostdout=0;
    }
    else
    {
        printf("Custom four-bit CPU assembler\n");
        printf("Usage:\n");
        printf("   4bassm [input file] [output file]");
        printf(" (output to file)\n");
        printf("   4bassm [input file] -            ");
        printf(" (output to STDOUT)\n");
        printf("   4bassm [input file]              ");
        printf(" (output to .hex file)\n");
        return;
    }

    //read in file line by line
    ifile=fopen(ifilename,"r");
    if (ifile)
    {
        addr=0;
        hexc=0;
        while (fgets(line,sizeof(line),ifile))
        {
            i=foundi=operands=set_instr=set_hash=0;
            while (line[i]!=0)
            {
                //skip whitespace
                while (!isalnum(line[i]) 
                        && line[i]!=0 
                        && line[i]!='#' 
                        && line[i]!='/') i++;

                //check for comments and end of line
                if (line[i]==0 || line[i]=='#' || line[i]=='/')
                    break;

                //make hash
                hash=0;
                j=0;
                while (isalnum(line[i]))
                    hash=(
                            tolower(line[i++]) 
                            *(j+1)*(j+1)*(++j) 
                            +hash 
                          )%HASHMOD;

                //we haven't found the opcode
                //look for opcode or label
                if (!foundi) 
                {
                    if (instr_table[hash]>-1)
                    {
                        //add space every 4 digits
                        if (hexc%5==4) 
                            hex[hexc++]=' ';  
                        foundi=1;

                        //check for set instruction
                        if (instr_table[hash]==SET_INSTR) 
                            set_instr=1;
                        else
                        {
                            hex[hexc++]=HEX[instr_table[hash]];
                            addr++;
                        }
                        operands=OPERANDS[instr_table[hash]];
                        if (operands==0)
                            break;
                    }
                    else
                        label_table[hash]=HEX[addr];
                }

                //we have the opcode, look for operands
                else 
                {
                    //handle set instruction
                    if (set_instr) 
                    {
                        if (operands==2)
                            set_hash=hash;
                        else
                            label_table[set_hash]=toupper(hash);
                    }

                    else
                    {
                        //add space every 4 digits
                        if (hexc%5==4) 
                            hex[hexc++]=' ';
                        if (label_table[hash]>-1)
                            hex[hexc++]=label_table[hash];
                        else
                            hex[hexc++]=toupper(hash);
                        addr++;
                    }

                    //decrement operator count, check if done
                    if (--operands==0) 
                        break;
                }
            }
        }
        hex[hexc]=0;
        fclose(ifile);
    }
    else
    {
        printf("Failed to read file %s\n",ifilename);
        return;
    }
    
    //output the hex file
    if (tostdout)
        printf("%s\n",hex);
    else
    {
        ofile=fopen(ofilename,"w");
        if (ofile)
        {
            fputs(hex,ofile);
            fclose(ofile);
        }
        else
        {
            printf("Failed to write output file %s\n",ofilename);
            return;
        }
    }
    
    return;
}
