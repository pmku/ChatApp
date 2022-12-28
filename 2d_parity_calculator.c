#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    
    char *received_message = "MGG|10100010000010110000|bu ne len mk";
    char *state = "true";
    int count,firstcount,i,j,messagelength = 0;
    int stack = 0;
    
    //Taking Length of the parities and message
        
    for(i = 0 ; i < strlen(received_message); i++)
    {
        count++;
        if(received_message[i] == '|')
        {
            if(stack == 0)
            {
                firstcount=i+1;
                stack++;
                count = 0;
                i++;
            }
            else if(stack == 1)
            {
                stack++;
            }
        }
        else if(stack == 2)
        {
            messagelength++;
        }
    }
    
    count = count-messagelength;
    
    printf("Length of the parity bits : %d\n",count);
    printf("Bits start index : %d\n",firstcount);
    printf("Length of the message : %d",messagelength);
    
    printf("\n\n");
    printf("-------------------------------------------------------------------");
    printf("\n\n");
    
    int checkverticalarr[8];
    int checkhorizontalarr[messagelength];
    char selected_string[messagelength];
    int multcheck = firstcount;
    
    // Assignment of message to selected_string
    for(i = 0; i < messagelength; i++)
    {
        selected_string[i] = received_message[count + firstcount + i + 1];
    }
    
    
    // Assignment of verticalbits to checkverticalarr
    for(i = 0; i < 8;i++)
    {
        checkverticalarr[i] = received_message[multcheck];
        multcheck++;
    }
    
    
    // Assignment of verticalbits to checkhorizontalarr

    for(i = 0; i < messagelength;i++)
    {
        checkhorizontalarr[i] = received_message[multcheck];
        multcheck++;
    }
    
    //Cheking if selected_string is equal to message
    printf("Selected string is : ");
    for(i = 0; i < messagelength;i++)
    {
        printf("%c",selected_string[i]);
    }
    
    printf("\n");
    
    //cheking if checkverticalarr is equal to verticalbits
    
    printf("Vertical parity bits that are received with message : ");

    for(i = 0; i < 8;i++)
    {
        printf("%c",checkverticalarr[i]);
        
    }
    printf("\n");
    
    //cheking if checkhorizontalarr is equal to horizontalbits

    printf("Horizontal parity bits that are received with message : ");

    for(i = 0; i < count - 8;i++)
    {
        printf("%c",checkhorizontalarr[i]);
        
    }
    
    
    printf("\n\n");
    printf("-------------------------------------------------------------------");
    printf("\n\n");
    
    int arr[messagelength][8];
    // Assignment of messages bits to two dimentional array
    for(j = 0; j < messagelength; j++ )
    {
        int x = 0;
        char c = selected_string[j];
        for (i = 7; i >= 0; i--) 
        {
            arr[j][x] = (c >> i) & 1;
            x++;
        }
    }
    
    // Checking of two dimentional Array
    
    for(j = 0; j < messagelength; j++)
    {
        printf("Arrays binary number of row %d :",j+1);
        for (i = 0; i < 8; i++) 
        {
            printf("%d",arr[j][i]);
        }
        printf("\n");
    }
    
    printf("\n\n");
    printf("-------------------------------------------------------------------");
    printf("\n\n");
    
    int verticalparityarr[8];
    int col,row;
    int horizontalarray[messagelength];
    int parity = 0;
    
    //Calculation of messages vertical parity
    
    for (col = 0; col < 8; col++) 
    {
      int parity = 1;
      for (row = 0; row < messagelength; row++) 
      {
        parity ^= arr[row][col];
      }
      verticalparityarr[col] = parity;
      
      // Cheking of horizontalarrays parity
      
      printf("Vertical parity of column %d: %d\n", col+1, verticalparityarr[col]);
    }
    
    printf("\n");
   
    //Calculation of messages horizontal parity
    
    for (i = 0; i < messagelength; i++) 
    {
        parity = 1;
        char c = selected_string[i];
        while (c) 
        {
          parity ^= c & 1;
          c >>= 1;
        }
        horizontalarray[i] = parity;
    }
    
    // Cheking of horizontalarrays parity
    
    for(i=0 ; i < messagelength;i++)
    {
        printf("Horizontal parity value of %d. char is :%d\n",i+1,horizontalarray[i]);
    }
    
    printf("\n\n");
    printf("-------------------------------------------------------------------");
    printf("\n\n");
    
    int x = 0;
    j = firstcount;
    
    
    // Cheking if calculated parities are mathing with the parities that are received with the message
    
    for(i = firstcount; i < firstcount + count; i++)
    {
        if(i < firstcount + 8)
        {
            for(j = 0;j < 8;j++)
            {
                printf("is %d equal to %d\n",checkverticalarr[j]-48,verticalparityarr[j]);
                if(checkverticalarr[j]-48 != verticalparityarr[j])
                {
                    state = "false";
                    printf("pointer\n");
                }
                i++;
            }
            printf("\n\n");
            
        }
        else
        {
            for(j = 0; j < count-8;j++)
            {
                
                printf("is %c equal to %d\n",checkhorizontalarr[j],horizontalarray[j]);
                if(checkhorizontalarr[j]-48 != horizontalarray[j])
                {
                    state = "false";
                    printf("pointer\n");
                }
                i++;
            }
        }
        
    }
    printf("\n\n%s",state);
    return 0;
    // horizontalarray is holding Horizontal parity values
    // verticalparityarr is holding vertical parity values
    // checkhorizontalarr is holding horizontal parity bits that are received with the message
    // checkhorizontalarr is holding vertical parity bits that are received with the message
    // 

}



