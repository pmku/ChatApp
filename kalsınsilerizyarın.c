    
int main()
{
    time_t t;
    srand((unsigned) time(&t));
    int randomNumbers[5];
    char teststring[100];
    int testnumber = 0;
    
    for(i = 0; i < strlen(rmessage);i++)
    {
        teststring[i] = rmessage[i];
    }
        
    printf("Original string is : %s",rmessage);
    
    
    for(i = 0;i < 5;i++)
    {
       randomNumbers[i] = rand() % 2;
    }
        
    
    
    for(i = 0;i < strlen(teststring);i++)
    {
       if(teststring[i] == '|')
       {
           testnumber++;
           i++;
       }
       
       if(testnumber == 1)
       {
           if(randomNumbers[0] == 1)
            {
                teststring[4 + randomNumbers[3]] = (char) randomNumbers[2] + 48;
                testnumber++;
            }
       }
    }

    
}
