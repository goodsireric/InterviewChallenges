#include <stdio.h>
#include <stdint.h>

//Tested in Ubuntu 20.04 LTS using Windows Subsystem Linux
//Compiled using g++ (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0

#define HYSTERESIS_DISCRETE_LEVELS 4
static unsigned int hysteresis_inflection_ranges[HYSTERESIS_DISCRETE_LEVELS][2] =
{
    //Must be listed in increasing order
    //Must have HYSTERESIS_DISCRETE_LEVELS rows
    //ranges cannot overlap
    {10,15},
    {35,40},
    {60,65},
    {85,90}
};

//Generic hysteresis for an ADC output, 
// rejecting small sudden voltage variations due to noise
//input_percent: percentage reading by an ADC
//returns: a discrete level in the range 0 to HYSTERESIS_DISCRETE_LEVELS
unsigned int hysteresis(unsigned int input_percent)
{
    static unsigned int prev_discrete = 0; //assume 0 from power off
    unsigned int i;

    //set to the max value in case the input is greater than the last transition level
    unsigned int discrete_output = HYSTERESIS_DISCRETE_LEVELS; 

    for(i = 0; i < HYSTERESIS_DISCRETE_LEVELS; i++)
    {
        if(input_percent <= hysteresis_inflection_ranges[i][0])  
        {
            //for values outside the transition ranges, set to the expected level
            discrete_output = i;
            break;
        } 
        else if(input_percent < hysteresis_inflection_ranges[i][1] )
        {
            //do not transition from 1 level higher to lower or vice versa
            //only transition when at least 2 discrete levels away
            if(prev_discrete <= i)
                discrete_output = i;
            else
                discrete_output = i+1;
            break;
        }
    }

    prev_discrete = discrete_output;
    return discrete_output;
}

unsigned int hysteresisTestList[][3] =
{
    //format: percent 1, percent 2, resulting level (0 to 4)
    //provided tests
    {9, 15, 1},
    {15, 11, 1},
    {11, 10, 0},

    //additional tests
    {0, 14, 0},
    {0, 15, 1},
    {15, 35, 1},
    {15, 35, 1},
    {36, 62, 2},
    {10, 90, 4},
    {95, 86, 4},
    {95, 62, 3},
    {0, 1, 0}

};

int main(int argc,char* argv[])
{
    int i;
    //iterate through all test rows
    int testLen = (sizeof (hysteresisTestList) / sizeof (hysteresisTestList)[0]);
    int testPassCount = 0;
    unsigned int result;
    for(i = 0; i < testLen; i++)
    {
        //execute the hysteresis test to verify our function behaves appropriately in all circumstances
        //mimic the behavior of cpputest which outputs . for pass, or an error line upon failure
        hysteresis(hysteresisTestList[i][0]);
        result = hysteresis(hysteresisTestList[i][1]);
        if(result == hysteresisTestList[i][2])
        {
            printf(".");
            testPassCount++;
        }
        else
        {
            printf("\nERR: hysteresis line %d fails, result %d, expected %d", i, result, hysteresisTestList[i][2]);
        }
    }
    printf("\n%d tests passed of %d\n",testPassCount,testLen);
    return 0;
}

