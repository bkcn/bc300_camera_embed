#include <stdint.h>
#include "setup.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sd_extr.h"
#include "modem.h"


#define MAX_PHONEBOOK_NUM_SIZE 20
#define MAX_PHONEBOOK_TEXT_SIZE 20

#define REMOTE_UPGRADE_INDEX       8
#define SERVER_PORT_INDEX              8
#define TRANSFER_TYPE_INDEX           8
#define PHOTO_RESOLUTION_INDEX    8

#define SERVER_IP_INDEX                   9
#define GPRS_APN_INDEX                    11
#define GPRS_USRNAME_INDEX           12
#define GPRS_PASSWD_INDEX             13
#define FW_NAME_INDEX                     14

/*Define variant to save all parameters*/
SYS_PARM sysParm;


/*Verify phone book number , text, index*/
static int VerifyPhoneBookRec(char* pPhoneNum,char*pPhoneText, int index)
{
    int result=-1;
    char tmpstr[4];
    int length,i;
    unsigned char sum=0;
    
    /*Check header "30"*/
    if((0 == strncmp(pPhoneNum,"30",2))&&(99>=index))
    {
    
        memset(tmpstr,0,sizeof(tmpstr));
        sprintf(tmpstr,"%02d",index);
        /*Check index*/
        if(0 == strncmp(&pPhoneNum[2],tmpstr,2))
        {
            /*Check sum*/
            length=strlen(pPhoneText);
            for(i=0;i<length;i++)
            {
                sum+=(unsigned char)pPhoneText[i];
            }
             memset(tmpstr,0,sizeof(tmpstr));
            sprintf(tmpstr,"%03d",sum);
            if(0 == strncmp(&pPhoneNum[4],tmpstr,3))
            {
                result =0;
            }
         }
    }
}

/*Initializing parameters, and resolve some import parameter*/
void vInitSetup(void)
{
    char PhoneNum[MAX_PHONEBOOK_NUM_SIZE+1];
    char PhoneText[MAX_PHONEBOOK_TEXT_SIZE+1];
    char ParamStr[6];
    int ParamInt;

    memset(&sysParm, 0, sizeof(sysParm));

    sysParm.PhotoResolution = DEFAULT_JPEG_SIZE;
    strcpy(sysParm.ServerIP,DEFAULT_SERVER_IP);
    sysParm.ServerPort = DEFAULT_SERVER_PORT;
    sysParm.TransportType = DEFAULT_TRANSFER_TYPE;
    strcpy(sysParm.GPRSApn,DEFAULT_GPRS_APN);
    strcpy(sysParm.GPRSUsrname,DEFAULT_GPRS_USRNAME);
    strcpy(sysParm.GPRSUsrpasswd,DEFAULT_GPRS_PASSWD);
    sysParm.RemoteUpgradeAppFW = DEFAULT_UPGRADE_FLAG;

    memset(PhoneNum,0,sizeof(PhoneNum));
    memset(PhoneText,0,sizeof(PhoneText));
    if(AT_RSP_SUCCESS==ucGetPhoneBookRecord(&port[MODEM_UART],PHOTO_RESOLUTION_INDEX,
                                        PhoneText,sizeof(PhoneText),
                                        NULL,0,
                                        PhoneText,sizeof(PhoneText)))
    {
        if(0 == VerifyPhoneBookRec(PhoneNum,PhoneText,PHOTO_RESOLUTION_INDEX))
        {
             /*Resolve server port: 1~4 */
             memset(ParamStr,0,sizeof(ParamStr));
             strncpy(ParamStr,&PhoneText[0],4);
             ParamInt = atoi(ParamStr);
             if(0!=ParamInt)
             {
                sysParm.ServerPort = ParamInt;
             }

             /*Photo resolution: 5*/
             memset(ParamStr,0,sizeof(ParamStr));
             strncpy(ParamStr,&PhoneText[4],1);
             ParamInt = atoi(ParamStr);
             if(MAX_JPEG_SIZE>=ParamInt)
             {
                sysParm.PhotoResolution = atoi(ParamStr);
             }

             /*Transfer type: 6*/
             memset(ParamStr,0,sizeof(ParamStr));
             strncpy(ParamStr,&PhoneText[5],1);
             ParamInt = atoi(ParamStr);
             if(MAX_TRANSFER_TYPE>=ParamInt)
             {
                sysParm.TransportType = ParamInt;
             }

            /*Upgrade flag: 7*/
             memset(ParamStr,0,sizeof(ParamStr));
             strncpy(ParamStr,&PhoneText[6],1);             
             ParamInt = atoi(ParamStr);             
             sysParm.RemoteUpgradeAppFW = ParamInt;
             
        }
    }
}

/*Resolve remaining parameters from Phone book*/
void vResolveParam(void)
{
    char PhoneNum[MAX_PHONEBOOK_NUM_SIZE+1];
    char PhoneText[MAX_PHONEBOOK_TEXT_SIZE+1];
    char ParamStr[6];
    int ParamInt;
    int Paramlen;

    if(AT_RSP_SUCCESS==ucGetPhoneBookRecord(&port[MODEM_UART],SERVER_IP_INDEX,
                                        PhoneText,sizeof(PhoneText),
                                        NULL,0,
                                        PhoneText,sizeof(PhoneText)))
    {
        if(0 == VerifyPhoneBookRec(PhoneNum,PhoneText,SERVER_IP_INDEX))
        {
            Paramlen = strlen(PhoneText);
            if(Paramlen<(sizeof(sysParm.ServerIP)-1))
            {
                strcpy(sysParm.ServerIP,PhoneText);
            }
        }
    }

    if(AT_RSP_SUCCESS==ucGetPhoneBookRecord(&port[MODEM_UART],GPRS_APN_INDEX,
                                        PhoneText,sizeof(PhoneText),
                                        NULL,0,
                                        PhoneText,sizeof(PhoneText)))
    {
        if(0 == VerifyPhoneBookRec(PhoneNum,PhoneText,GPRS_APN_INDEX))
        {
            Paramlen = strlen(PhoneText);
            if(Paramlen<(sizeof(sysParm.GPRSApn)-1))
            {
                strcpy(sysParm.GPRSApn,PhoneText);
            }
        }
    }

    if(AT_RSP_SUCCESS==ucGetPhoneBookRecord(&port[MODEM_UART],GPRS_USRNAME_INDEX,
                                        PhoneText,sizeof(PhoneText),
                                        NULL,0,
                                        PhoneText,sizeof(PhoneText)))
    {
        if(0 == VerifyPhoneBookRec(PhoneNum,PhoneText,GPRS_USRNAME_INDEX))
        {
            Paramlen = strlen(PhoneText);
            if(Paramlen<(sizeof(sysParm.GPRSUsrname)-1))
            {
                strcpy(sysParm.GPRSUsrname,PhoneText);
            }
        }
    }

    if(AT_RSP_SUCCESS==ucGetPhoneBookRecord(&port[MODEM_UART],GPRS_PASSWD_INDEX,
                                        PhoneText,sizeof(PhoneText),
                                        NULL,0,
                                        PhoneText,sizeof(PhoneText)))
    {
        if(0 == VerifyPhoneBookRec(PhoneNum,PhoneText,GPRS_PASSWD_INDEX))
        {
            Paramlen = strlen(PhoneText);
            if(Paramlen<(sizeof(sysParm.GPRSUsrpasswd)-1))
            {
                strcpy(sysParm.GPRSUsrpasswd,PhoneText);
            }
        }
    }

    if(AT_RSP_SUCCESS==ucGetPhoneBookRecord(&port[MODEM_UART],FW_NAME_INDEX,
                                        PhoneText,sizeof(PhoneText),
                                        NULL,0,
                                        PhoneText,sizeof(PhoneText)))
    {
        if(0 == VerifyPhoneBookRec(PhoneNum,PhoneText,FW_NAME_INDEX))
        {
            Paramlen = strlen(PhoneText);
            if(Paramlen<(sizeof(sysParm.AppFWName)-1))
            {
                strcpy(sysParm.AppFWName,PhoneText);
            }
        }
    }
}
