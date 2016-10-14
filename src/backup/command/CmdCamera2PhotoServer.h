#ifndef __CMDCAMERA2PHOTOSERVER_H_
    #define __CMDCAMERA2PHOTOSERVER_H_

    // TODO: Should open compiling option(pack struct) in IDE. Arthur@2014/12/1
    #ifdef __GNUC__
        #define GNUC_PACKED    __attribute__ ((__packed__))
    #else
        #define GNUC_PACKED
    #endif

    #ifdef __arm
        #define ARM_PACKED    __packed
    #else
        #define ARM_PACKED
    #endif

    #ifdef WIN32
        #pragma pack(1)
    #endif

    /*Define comand DWORD union type*/
     typedef ARM_PACKED union
    {
         ARM_PACKED struct  {
            unsigned char B[4];
        }by;
        unsigned long int u_li;
        long int s_li;
    }CMD_DWORD;
    
    /*Define comand WORD union type*/
     typedef ARM_PACKED union
    {
         ARM_PACKED struct  {
            unsigned char B[2];
        }by;
        unsigned short int u_s;
        short int s_s;
    }CMD_WORD;

    /*Define comand BYTE union type*/
     typedef ARM_PACKED union
    {
        ARM_PACKED struct {
                unsigned char B[1];
        }by;
        unsigned char u_c;
        char    s_c;
    }CMD_BYTE;


    #define PUT_CMD_DWORD(cmd,value)    do{\
                                    cmd.by.B[0] = (unsigned char)(value&0xFF);\
                                    cmd.by.B[1] = (unsigned char)((value>>8)&0xFF);\
                                    cmd.by.B[2] = (unsigned char)((value>>16)&0xFF);\
                                    cmd.by.B[3] = (unsigned char)((value>>24)&0xFF);\
                                }while(0)

     #define PUT_CMD_WORD(cmd,value)    do{\
                                    cmd.by.B[0] = (unsigned char)(value&0xFF);\
                                    cmd.by.B[1] = (unsigned char)((value>>8)&0xFF);\
                                }while(0)

     #define PUT_CMD_BYTE(cmd,value)    do{\
                                    cmd.by.B[0] = (unsigned char)(value&0xFF);\
                                }while(0)

     #define PUT_CMD_CHECKSUM(pCmd,CmdLen,CheckSum)   do{\
                                  pCmd[CmdLen-2] = (unsigned char)(CheckSum&0xFF);\
                                  pCmd[CmdLen-1] = (unsigned char)((CheckSum>>8)&0xFF);\
                                }while(0)

     #define GET_CMD_DWORD(cmd,value)    do{\
                                    value = (unsigned long)cmd.by.B[0];\
                                    value |= ((unsigned long)cmd.by.B[1])<<8;\
                                    value |= ((unsigned long)cmd.by.B[2])<<16;\
                                    value |= ((unsigned long)cmd.by.B[3])<<24;\
                                }while(0)

      #define GET_CMD_WORD(cmd,value)    do{\
                                value = (unsigned short)cmd.by.B[0];\
                                value |= ((unsigned short)cmd.by.B[1])<<8;\
                            }while(0)

      #define GET_CMD_BYTE(cmd,value)    do{\
                                value = cmd.by.B[0];\
                           }while(0)

      #define GET_CMD_CHECKSUM(pCmd,CmdLen,CheckSum)   do{\
                              CheckSum = (unsigned short)pCmd[CmdLen-2];\
                              CheckSum |= ((unsigned short)pCmd[CmdLen-1])<<8;\
                           }while(0)

    /*Define checksum length*/
    #define CMD_CHECKSUM_LEN    2
    
    /*Define command header*/    
     typedef ARM_PACKED struct {
        CMD_WORD Len;                  // Command frame totoal length(from itself to checksum)
        CMD_WORD CameraID;        // Camera ID which is got by login ack from remote server
        CMD_BYTE Type;                 // Command type
        CMD_WORD Seq;                 // Command sequence (0x0000~0xFFFF)
    }CmdHeader;

    /*Define command type*/
    #define CMD_TYPE_LOGIN                                             0x01
    #define CMD_TYPE_START_UPLOAD_PHOTO                 0x02
    #define CMD_TYPE_UPLOAD_PHOTO_SEG                     0x03
    #define CMD_TYPE_END_UPLOAD_PHOTO                     0x04
    #define CMD_TYPE_LOGOUT                                          0x05

    #define CMD_TYPE_ACK_LOGIN                                     0x81
    #define CMD_TYPE_ACK_START_UPLOAD_PHOTO          0x82
    #define CMD_TYPE_ACK_UPLOAD_PHOTO_SEG              0x83
    #define CMD_TYPE_ACK_END_UPLOAD_PHOTO              0x84

    /*Define "login" command body*/
    typedef ARM_PACKED struct {
        CMD_WORD HWModel;         // Hardware model, 2 bytes
        CMD_WORD SWVersion;       // SW version, 2 bytes
        CMD_BYTE SNLen;               //  Serial number: SIM card number or IMEI
#ifdef SUPPORT_ARRAY_ZERO_LENGTH           
        CMD_BYTE SN[0];
#endif
    }CmdBodyLogin;

    /*Define "login ack" command body*/
    typedef ARM_PACKED struct {
        CMD_WORD PeerCmdSeq;         // Command sequence got from login command header
        CMD_WORD CameraID;              // Allocated by SN by remote server
    }CmdBodyLoginACK;

    /*Define "start to transfer photo" command body*/
    typedef ARM_PACKED struct {
        CMD_DWORD FileOffset;            // The file offset which will be written
        CMD_DWORD FileLen;                // The file length 
        CMD_BYTE FileNameLen;           //  The file name length 
#ifdef SUPPORT_ARRAY_ZERO_LENGTH        
        CMD_BYTE FileName[0];
#endif
    }CmdBodyStartTransPhoto;

    /*Define "Start to transfer photo ACK" command body */
    typedef ARM_PACKED struct {
        CMD_WORD PeerCmdSeq;         // Command sequence got from login command header
        CMD_DWORD FileOffset;            // The availble file offset which will be written
        CMD_DWORD FileLen;                // The file curent length 
        CMD_BYTE FileNameLen;           //  The file name length 
#ifdef SUPPORT_ARRAY_ZERO_LENGTH           
        CMD_BYTE FileName[0];
#endif
    }CmdBodyStartTransPhotoACK;

    /*Define "Transfer 1 segment photo" command body*/
    typedef ARM_PACKED struct {
        CMD_DWORD   FileOffset;            // Current data file offset
        CMD_WORD     SegLen;               // Segment length field len
#ifdef SUPPORT_ARRAY_ZERO_LENGTH           
        CMD_BYTE      SegData[0];
#endif
    }CmdBodyTrans1SegPhoto;

    /*Define "Transfer 1 segment photo ACK" command body*/
    typedef ARM_PACKED struct {
        CMD_WORD    PeerCmdSeq;       // Command sequence got from login command header
        CMD_DWORD  FileOffset;            // current file offset
        CMD_DWORD  FileLen;                // The file curent length 
    }CmdBodyTrans1SegPhotoACK;

    /*Define "End to Transfer  photo" command body*/
    typedef ARM_PACKED struct {
        CMD_DWORD FileOffset;             // Current data file offset
        CMD_DWORD FileLen;                 // The file curent length 
    }CmdBodyEndTransPhoto;

    /*Define "End to Transfer  photo ACK" command body*/
    typedef ARM_PACKED struct {
        CMD_WORD    PeerCmdSeq;       // Command sequence got from login command header
        CMD_DWORD  FileOffset;            // Current data file offset
        CMD_DWORD  FileLen;                // The file curent length 
    }CmdBodyEndTransPhotoACK;


    /*Define all commands*/

    /*Login*/
    typedef ARM_PACKED struct {
        CmdHeader Head;
        CmdBodyLogin Body;
    }CmdLogin;
    /*Start to transfer photo*/
     typedef ARM_PACKED struct {
        CmdHeader Head;
        CmdBodyStartTransPhoto Body;
    }CmdStartTransPhoto;
    /*Transfer 1 segment photo*/
    typedef ARM_PACKED struct {
        CmdHeader Head;
        CmdBodyTrans1SegPhoto Body;
    }CmdTrans1SegPhoto;
    /*End to transfer photo*/
     typedef ARM_PACKED struct {
        CmdHeader Head;
        CmdBodyEndTransPhoto Body;
    }CmdEndTransPhoto;
    /*Logout*/
    typedef ARM_PACKED struct {
        CmdHeader Head;
    }CmdLogout;
    /*Login ACK*/
    typedef ARM_PACKED struct {
        CmdHeader Head;
        CmdBodyLoginACK Body;
    }CmdLoginACK;
    /*Start to transfer photo ACK*/
     typedef ARM_PACKED struct {
        CmdHeader Head;
        CmdBodyStartTransPhotoACK Body;
    }CmdStartTransPhotoACK;
    /*Transfer 1 segment photo ACK*/
    typedef ARM_PACKED struct {
        CmdHeader Head;
        CmdBodyTrans1SegPhotoACK Body;
    }CmdTrans1SegPhotoACK;
    /*End to transfer photo ACK*/
     typedef ARM_PACKED struct {
        CmdHeader Head;
        CmdBodyEndTransPhotoACK Body;
    }CmdEndTransPhotoACK;
#endif    
