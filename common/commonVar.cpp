#include "common.h"

using namespace std;

map<short, int> ServerPackType = {{0x01, PACK_IDS_REQ_S},
                                  {0x02, PACK_SYS_INFO_REQ_S},
                                  {0x03, PACK_CONF_INFO_REQ_S},
                                  {0x04, PACK_PROC_INFO_REQ_S},
                                  {0x05, PACK_ETH_PORT_INFO_REQ_S},
                                  {0x07, PACK_FLASH_INFO_REQ_S},
                                  {0x08, PACK_PRINT_PORT_INFO_REQ_S},
                                  {0x09, PACK_TTY_SER_INFO_REQ_S},
                                  {0x0a, PACK_TTY_INFO_REQ_S},
                                  {0x0b, PACK_IP_TTY_INFO_REQ_S},
                                  {0x0c, PACK_FLASH_FILE_INFO_REQ_S},
                                  {0x0d, PACK_PRINT_QUE_INFO_REQ_S},
                                  {0xff, PACK_DISCON_REQ_S}};

map<short, int> ClientPackType = {{0x00, PACK_VER_INFO_C},
                                  {0x01, PACK_IDS_ANS_C},
                                  {0x02, PACK_SYS_INFO_ANS_C},
                                  {0x03, PACK_CONF_INFO_ANS_C},
                                  {0x04, PACK_PROC_INFO_ANS_C},
                                  {0x05, PACK_ETH_PORT_INFO_ANS_C},
                                  {0x07, PACK_FLASH_INFO_ANS_C},
                                  {0x08, PACK_PRINT_PORT_INFO_ANS_C},
                                  {0x09, PACK_TTY_SER_INFO_ANS_C},
                                  {0x0a, PACK_TTY_INFO_ANS_C},
                                  {0x0b, PACK_IP_TTY_INFO_ANS_C},
                                  {0x0c, PACK_FLASH_FILE_INFO_ANS_C},
                                  {0x0d, PACK_PRINT_QUE_INFO_ANS_C},
                                  {0xff, PACK_DISCON_ANS_C}};