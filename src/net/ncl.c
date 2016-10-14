/*************************************************************************
*                                                                       
*        Copyright (c) 1993 - 2001 Accelerated Technology, Inc.           
*                                                                       
* PROPRIETARY RIGHTS of Accelerated Technology are involved in the      
* subject matter of this material.  All manufacturing, reproduction,    
* use, and sales rights pertaining to this subject matter are governed  
* by the license agreement.  The recipient of this software implicitly  
* accepts the terms of the license.                                     
*                                                                       
*************************************************************************/

/*************************************************************************
*                                                                       
*   FILE NAME                                        VERSION          
*                                                                    
*       NCL.C                                          4.4         
*                                                                               
*   COMPONENT                                                             
*                                                                       
*       NET - NET 'C' Library 
*                                                                       
*   DESCRIPTION                                                           
*                                                                       
*       This file contains 'C' library functions that are required by
*       Nucleus NET and other Nucleus networking products. These 
*       functions are not supplied by all tool vendors and thus are being
*       supplied here.        
*                                                                       
*   FUNCTIONS                                                             
*
*       NCL_Stricmp
*       NCL_Itoa
*       NCL_To_Upper
*       NCL_Ultoa
*       NCL_Atoi
*       NCL_Atol
*                                                                       
*   DEPENDENCIES                                                          
*                        
*       NCL.H                                               
*                                                                       
*************************************************************************/
#include "ncl.h"

/*************************************************************************
*                                                                       
*   FUNCTION                                                              
*                                                                       
*       NCL_Stricmp                                                        
*                                                                       
*   DESCRIPTION
*
*       The function compares two strings, continuing until a difference 
*       is found or the end of the strings is reached. Case is ignored.
*
*   INPUTS                                                                
*   
*       s1              Pointer to one string
*       s2              Pointer to the string to compare with
*                                                                       
*   OUTPUTS                                                               
*                       
*       < 0             s1 less than s2 
*       0               s1 identical to s2 
*       > 0             s1 greater than s2 
*                                                                       
*************************************************************************/
int NCL_Stricmp(register const char *s1, register const char *s2)
{
    while( (NCL_To_Upper(*s1) == NCL_To_Upper(*s2)) && (*s1) ) ++s1, ++s2;

    return ((int)(unsigned char)*s1) - ((int)(unsigned char)*s2);

} /* NCL_Stricmp */

/*************************************************************************
*                                                                       
*   FUNCTION                                                              
*                                                                       
*       NCL_To_Upper
*                                                                       
*   DESCRIPTION                                                           
*
*       The function converts a character to upper case.                  
*                                                                       
*   INPUTS                                                                
*   
*       ch              Character to convert                              
*                                                                       
*   OUTPUTS                                                               
*
*       The character in uppercase.                            
*                                                                       
*************************************************************************/
int NCL_To_Upper(int ch)
{
    if ( (ch < 'a') || (ch > 'z') )
        return ch;

    ch -= 32;

    return ch;

} /* NCL_To_Upper */

/*************************************************************************
*                                                                       
*   FUNCTION                                                              
*                                                                       
*       NCL_Itoa                                                        
*                                                                       
*   DESCRIPTION  
*
*       This function converts value to a null terminated ascii string.
*                                                                       
*   INPUTS                                                                
*
*       value           The integer to convert to ASCII
*       *string         Pointer to were the string should be built
*       radix           Base to be used for conversion
*                                                                       
*   OUTPUTS                                                               
*                                                     
*       A pointer to the converted integer                  
*                                                                       
*************************************************************************/
char *NCL_Itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*************************************************************************
*                                                                       
*   FUNCTION                                                              
*                                                                       
*       NCL_Ultoa                                                    
*                                                                       
*   DESCRIPTION  
*
*       Binary-to-ascii conversion for an unsigned long.
*
*       Returns a pointer to a string built somewhere
*       in "buf".  The convup parameter should be non-zero
*       if upper case letters are wanted for the conversion. 
*
*       This routine will handle bases from 8 to 16.                     
*                                                                       
*   INPUTS                                                                
*
*       value           The integer to convert to ASCII
*       *string         Pointer to were the string should be built
*       radix           Base to be used for conversion
*                                                                       
*   OUTPUTS                                                               
*                                                     
*       A pointer to the converted long                  
*                                                                       
*************************************************************************/
char *NCL_Ultoa(unsigned long value, char *string, int radix)
{
    unsigned long   i, d;
    int             flag = 0;
    char            *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    for (i = 1000000000UL; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*************************************************************************
*                                                                       
*   FUNCTION                                                              
*                                                                       
*       NCL_Atoi                                                    
*                                                                       
*   DESCRIPTION  
*
*       Converts an ASCII number to the integer equivalent.
*                                                                       
*   INPUTS                                                                
*
*       *nptr           Pointer to the ASCII number to convert
*                                                                       
*   OUTPUTS                                                               
*                                                     
*       The ASCII number in integer form.
*                                                                       
*************************************************************************/
int NCL_Atoi (const char *nptr)
{
    register const unsigned char *ptr = (const unsigned char *)nptr;
    register       unsigned int   num = 0;
    register                int   c   = *ptr;
    register                int   neg = 0;

    while ( NCL_IS_SPACE(c) )  c = *++ptr;   /* skip over whitespace chars */

    if ( c == '-' )                     /* get an optional sign */
    {
      neg = 1;
      c = *++ptr;
    }
    else if ( c == '+' )  c = *++ptr;

    while ( NCL_IS_DIGIT(c) )
    {
      num = ( 10 * num ) + ( c - '0' );
      c = *++ptr;
    }

    if ( neg )  return ( NCL_SINEGATE(num) );

    return ( (int) num );

} /* NCL_Atoi */

/*************************************************************************
*                                                                       
*   FUNCTION                                                              
*                                                                       
*       NCL_Atol                                                    
*                                                                       
*   DESCRIPTION  
*
*       Converts an ASCII number to the long integer equivalent.
*                                                                     
*   INPUTS                                                                
*
*       *nptr           Pointer to the ASCII number to convert
*                                                                       
*   OUTPUTS                                                               
*         
*       The ASCII number in long integer form.
*                                                                       
*************************************************************************/
long NCL_Atol (const char *nptr)
{
    register const unsigned char *ptr = (const unsigned char *)nptr;
    register       unsigned long  num = 0;
    register                int   c   = *ptr;
    register                int   neg = 0;

    while ( NCL_IS_SPACE(c) )  c = *++ptr;   /* skip over whitespace chars */

    if ( c == '-' )                     /* get an optional sign */
    {
      neg = 1;
      c = *++ptr;
    }
    else if ( c == '+' )  c = *++ptr;

    while ( NCL_IS_DIGIT(c) )
    {
      num = ( 10 * num ) + ( c - '0' );
      c = *++ptr;
    }

    if ( neg )  return ( NCL_SNEGATE(num) );

    return ( (long) num );

} 

/* NCL_Atol */







