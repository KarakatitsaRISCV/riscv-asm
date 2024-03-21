#include "usb_lib.h"
#include <wchar.h>
#include "hardware.h"
#include "usb_audio.h"
#include "class_mic.h"
#include "class_hid.h"
#include "programmer.h"

#define STD_DESCR_LANG	0
#define STD_DESCR_VEND	1
#define STD_DESCR_PROD	2
#define STD_DESCR_SN	3
#define STR_TTY		4
#define STR_PROGR	5

USB_STRING(USB_StringLangDescriptor, u"\x0409"); //lang US
USB_STRING(USB_StringManufacturingDescriptor, u"COKPOWEHEU"); //Vendor
USB_STRING(USB_StringProdDescriptor, u"Karakatitsa_CH32"); //Product
USB_STRING(USB_StringSerialDescriptor, u"1"); //Serial (BCD)
USB_STRING(USB_String_TTY, u"CH32_DBG");
USB_STRING(USB_String_PROGR, u"CH32_PROG");

#define USB_VID 0x16C0
#define USB_PID 0x05DF

#define DEVCLASS_CDC          0x02
#define CDCSUBCLASS_ACM       0x02
#define CDCCLASS_DATA         0x0A
#define CDCPROTOCOL_UNDEF     0x00
#define CDCPROTOCOL_VENDOR    0xFF

#define USB_DESCR_IAD	0x0B


static const uint8_t USB_DeviceDescriptor[] = {
  ARRLEN1(
  bLENGTH,     // bLength
  USB_DESCR_DEVICE,   // bDescriptorType - Device descriptor
  USB_U16(0x0110), // bcdUSB
  0xEF, // DEVICE_CLASS_MISC  0,   // bDevice Class
  0x02, //DEVICE_SUBCLASS_IAD  0,   // bDevice SubClass
  0x01, //DEVICE_PROTOCOL_IAD  0,   // bDevice Protocol
  USB_EP0_BUFSZ,   // bMaxPacketSize0
  USB_U16( USB_VID ), // idVendor
  USB_U16( USB_PID ), // idProduct
  USB_U16( 1 ), // bcdDevice_Ver
  STD_DESCR_VEND,   // iManufacturer
  STD_DESCR_PROD,   // iProduct
  STD_DESCR_SN,   // iSerialNumber
  1    // bNumConfigurations
  )
};

static const uint8_t USB_DeviceQualifierDescriptor[] = {
  ARRLEN1(
  bLENGTH,     //bLength
  USB_DESCR_QUALIFIER,   // bDescriptorType - Device qualifier
  USB_U16(0x0200), // bcdUSB
  0,   // bDeviceClass
  0,   // bDeviceSubClass
  0,   // bDeviceProtocol
  USB_EP0_BUFSZ,   // bMaxPacketSize0
  1,   // bNumConfigurations
  0x00    // Reserved
  )
};

static const uint8_t USB_ConfigDescriptor[] = {
  ARRLEN34(
  ARRLEN1(
    bLENGTH, // bLength: Configuration Descriptor size
    USB_DESCR_CONFIG,    //bDescriptorType: Configuration
    wTOTALLENGTH, //wTotalLength
    interface_count, // bNumInterfaces
    1, // bConfigurationValue: Configuration value
    0, // iConfiguration: Index of string descriptor describing the configuration
    0x80, // bmAttributes: bus powered
    0x32, // MaxPower 100 mA
  )
  
//HID
  ARRLEN1(
    bLENGTH, //bLength
    USB_DESCR_INTERFACE, //bDescriptorType
    ifnum( interface_hid ), //bInterfaceNumber
    0, // bAlternateSetting
    0, // bNumEndpoints
    HIDCLASS_HID, // bInterfaceClass: 
    HIDSUBCLASS_NONE, // bInterfaceSubClass: 
    HIDPROTOCOL_NONE, // bInterfaceProtocol: 
    0x00, // iInterface
  )
  ARRLEN1(
    bLENGTH, //bLength
    USB_DESCR_HID, //bDescriptorType
    USB_U16(0x0101), //bcdHID
    0, //bCountryCode
    1, //bNumDescriptors
    USB_DESCR_HID_REPORT, //bDescriptorType
    USB_U16( sizeof(USB_HIDDescriptor) ), //wDescriptorLength
  )
  
//ttyACM0 (interface 0, 1) - TTY
  ARRLEN1( //IAD
    bLENGTH,
    USB_DESCR_IAD, //bDescriptorType
    ifnum( interface_tty ), //0, //bFirstInterface
    ifcnt( interface_tty ) ,//2, //bInterfaceCount
    DEVCLASS_CDC, // bInterfaceClass: 
    CDCSUBCLASS_ACM, // bInterfaceSubClass: 
    CDCPROTOCOL_UNDEF, // bInterfaceProtocol: 
    STR_TTY,//0x00, // iFuncion
  )
    ARRLEN1(//CDC control Interface
      bLENGTH, // bLength
      USB_DESCR_INTERFACE, // bDescriptorType
      ifnum( interface_tty ), //0, // bInterfaceNumber
      0, // bAlternateSetting
      1, // bNumEndpoints
      DEVCLASS_CDC, // bInterfaceClass: 
      CDCSUBCLASS_ACM, // bInterfaceSubClass: 
      CDCPROTOCOL_UNDEF, // bInterfaceProtocol: 
      STR_TTY,//0x00, // iInterface
    )
      ARRLEN1(//Header
        bLENGTH, //bLength
        USB_DESCR_CS_INTERFACE, //bDescriptorType
        0, //bDescriptorSubType
        USB_U16(0x0110), //bcdCDC
      )
      ARRLEN1( //Call mamagement
        bLENGTH,
        USB_DESCR_CS_INTERFACE,
        0x01, //Call mamagement
        0x00, //TODO: bmCapabilities D0+D1
        ifnum( interface_tty )+1, //1, //bDataInterface
      )
      ARRLEN1( //ACM
        bLENGTH,
        USB_DESCR_CS_INTERFACE,
        2, //bDescriptorSubType = Abstract Control Management
        0x02, //bmCapabilities
          //7-4 : reserved
          //3   : supports 'Network connection'
          //2   : supports 'Send break'
          //1   : supports 'Set line coding', 'set control line state', 'get line coding', 'serial state'
          //0   : supports 'Set comm feature', 'clear comm feature', 'get comm feature'
      )
      ARRLEN1( //Union
        bLENGTH,
        USB_DESCR_CS_INTERFACE,
        6, //bDescriptorSubType = Union
        ifnum( interface_tty ), //0, //bMasterInterface = Communication class interface
        ifnum( interface_tty )+1, //1, //bSlaveInterface0 = Data Class Interface
      )
      ARRLEN1(//Endpoint descriptor
        bLENGTH, //bLength
        USB_DESCR_ENDPOINT, //bDescriptorType
        ENDP_TTY_CTL | 0x80,
        USB_ENDP_INTR,
        USB_U16(ENDP_CTL_SIZE),
        100, //bInterval - частота опроса, для изохронных всегда 1
      )
    ARRLEN1( //CDC data interface
      bLENGTH,
      USB_DESCR_INTERFACE,
      ifnum( interface_tty )+1, //1, //bInterfaceNumber
      0, //bAlternateSetting
      2, //bNumEndpoints
      CDCCLASS_DATA, //bInterfaceClass
      0, //bInterfaceSubClass
      0, //bInterfaceProtocol
      0, //iInterface
    )
      ARRLEN1(//Endpoint descriptor
        bLENGTH, //bLength
        USB_DESCR_ENDPOINT, //bDescriptorType
        ENDP_TTY_OUT,
        USB_ENDP_BULK,
        USB_U16(ENDP_TTY_SIZE),
        0, //bInterval - частота опроса, для изохронных всегда 1
      )
      ARRLEN1(//Endpoint descriptor
        bLENGTH, //bLength
        USB_DESCR_ENDPOINT, //bDescriptorType
        ENDP_TTY_IN | 0x80,
        USB_ENDP_BULK,
        USB_U16(ENDP_TTY_SIZE),
        0, //bInterval - частота опроса, для изохронных всегда 1
      )
   
// /dev/ttyACM1 (interfaces 2, 3) - PROGR
  ARRLEN1( //IAD
    bLENGTH,
    0x0B, //IAD descriptor
    ifnum( interface_progr ), //2, //bFirstInterface
    ifcnt( interface_progr ), //2, //bInterfaceCount
    DEVCLASS_CDC, // bInterfaceClass: 
    CDCSUBCLASS_ACM, // bInterfaceSubClass: 
    CDCPROTOCOL_UNDEF, // bInterfaceProtocol: 
    STR_PROGR, //0x00, // iFuncion
  )
  ARRLEN1(//CDC control Interface
    bLENGTH, // bLength
    USB_DESCR_INTERFACE, // bDescriptorType
    ifnum( interface_progr ), //2, // bInterfaceNumber
    0, // bAlternateSetting
    1, // bNumEndpoints
    DEVCLASS_CDC, // bInterfaceClass: 
    CDCSUBCLASS_ACM, // bInterfaceSubClass: 
    CDCPROTOCOL_UNDEF, // bInterfaceProtocol: 
    STR_PROGR, //0x00, // iInterface
  )
    ARRLEN1(//Header
      bLENGTH, //bLength
      USB_DESCR_CS_INTERFACE, //bDescriptorType
      0, //bDescriptorSubType
      USB_U16(0x0110), //bcdCDC
    )
    ARRLEN1( //Call mamagement
      bLENGTH,
      USB_DESCR_CS_INTERFACE,
      0x01, //Call mamagement
      0x00, //TODO: bmCapabilities D0+D1
      ifnum( interface_progr )+1, //3, //bDataInterface
    )
    ARRLEN1( //ACM
      bLENGTH,
      USB_DESCR_CS_INTERFACE,
      2, //bDescriptorSubType = Abstract Control Management
      0x02, //bmCapabilities
        //7-4 : reserved
        //3   : supports 'Network connection'
        //2   : supports 'Send break'
        //1   : supports 'Set line coding', 'set control line state', 'get line coding', 'serial state'
        //0   : supports 'Set comm feature', 'clear comm feature', 'get comm feature'
    )
    ARRLEN1( //Union
      bLENGTH,
      USB_DESCR_CS_INTERFACE,
      6, //bDescriptorSubType = Union
      ifnum( interface_progr ), //2, //bMasterInterface = Communication class interface
      ifnum( interface_progr )+1, //3, //bSlaveInterface0 = Data Class Interface
    )
    ARRLEN1(//Endpoint descriptor
      bLENGTH, //bLength
      USB_DESCR_ENDPOINT, //bDescriptorType
      ENDP_PROG_CTL | 0x80,
      USB_ENDP_INTR,
      USB_U16(ENDP_CTL_SIZE),
      100, //bInterval - частота опроса, для изохронных всегда 1
    )
  ARRLEN1( //CDC data interface
    bLENGTH,
    USB_DESCR_INTERFACE,
    ifnum( interface_progr )+1, //3, //bInterfaceNumber
    0, //bAlternateSetting
    2, //bNumEndpoints
    CDCCLASS_DATA, //bInterfaceClass
    0, //bInterfaceSubClass
    0, //bInterfaceProtocol
    0, //iInterface
  )
    ARRLEN1(//Endpoint descriptor
      bLENGTH, //bLength
      USB_DESCR_ENDPOINT, //bDescriptorType
      ENDP_PROG_OUT,
      USB_ENDP_BULK,
      USB_U16(ENDP_PROG_SIZE),
      0, //bInterval - частота опроса, для изохронных всегда 1
    )
    ARRLEN1(//Endpoint descriptor
      bLENGTH, //bLength
      USB_DESCR_ENDPOINT, //bDescriptorType
      ENDP_PROG_IN | 0x80,
      USB_ENDP_BULK,
      USB_U16(ENDP_PROG_SIZE),
      0, //bInterval - частота опроса, для изохронных всегда 1
    )
    
//Microphone (interfaces 4, 5)
  ARRLEN1( //IAD
    bLENGTH,
    0x0B, //IAD descriptor
    ifnum( interface_mic ), //2, //bFirstInterface
    ifcnt( interface_mic ), //2, //bInterfaceCount
    1,//USB_CLASS_AUDIO, // bInterfaceClass: 
    1,//CDCSUBCLASS_ACM, // bInterfaceSubClass: 
    0, // bInterfaceProtocol: 
    0x00, // iFuncion
  )//*/
  ARRLEN1(//0: Audio control Interface
    bLENGTH, // bLength
    USB_DESCR_INTERFACE, // bDescriptorType
    ifnum( interface_mic ), // bInterfaceNumber
    0, // bAlternateSetting
    0, // bNumEndpoints (если испольуется Interrupt endpoint, может быть 1)
    USB_CLASS_AUDIO, // bInterfaceClass: 
    USB_SUBCLASS_AUDIOCONTROL, // bInterfaceSubClass: 
    0x00, // bInterfaceProtocol: 
    0x00, // iInterface
  )
    ARRLEN67(//AC interface
      ARRLEN1(//AC interface header
        bLENGTH, //bLength
        USB_DESCR_CS_INTERFACE, //bDescriptorType
        1, //bDescriptorSubType
        USB_U16(0x0100), //bcdADC //AudioDeviceClass серийный номер
        wTOTALLENGTH, //wTotalLength
        ifcnt( interface_mic )-1, //1, //bInCollection //количество интерфейсов в коллекции
        ifnum( interface_mic )+1, //5, //bInterfaceNr(1), //массив (список) номеров интерфейсов в коллекции
      )
      ARRLEN1(//1. AC Input terminal
        bLENGTH, //bLength
        USB_DESCR_CS_INTERFACE, //bDescriptorType
        USBAUDIO_IF_TERM_IN, //bDescriptorSubType
        1, //bTerminalID
        USB_U16( USBAUDIO_TERMINAL_MIC ), //wTerminalType
        0, //bAssocTerminal привязка выходного терминала для создания пары
        1, //bNrChannels
        USB_U16( 0 ), //wChannelConfig //к чему именно подключены каналы
        0, //iChannelNames
        0, //iTerminal
      )
      ARRLEN1(//2. AC Output Terminal
        bLENGTH, //bLength
        USB_DESCR_CS_INTERFACE, //bDescriptorType
        USBAUDIO_IF_TERM_OUT, //bDescriptorSubType
        2, //bTerminalID
        USB_U16( USBAUDIO_TERMINAL_USB ),//USB_U16( USBAUDIO_TERMINAL_USB ), //wTerminalType:speaker
        0, //bAssocTerminal
        1, //bSourceID  <-------------------------------------------
        0, //iTerminal
      )
    )
  ARRLEN1(//1 Audio Streaming Interface
    bLENGTH, //bLength
    USB_DESCR_INTERFACE, //bDescriptorType
    ifnum( interface_mic )+1, //5, //bInterfaceNumber
    0, //bAlternateSetting
    0, //bNumEndpoints
    USB_CLASS_AUDIO, //bInterfaceClass
    USB_SUBCLASS_AUDIOSTREAMING, //bInterfaceSubClass
    0, //bInterfaceProtocol
    0, //iInterface
  )
  ARRLEN1(//1alt Audio Streaming Interface (alternative)
    bLENGTH, //bLength
    USB_DESCR_INTERFACE, //bDescriptorType
    ifnum( interface_mic )+1, //5, //bInterfaceNumber
    1, //bAlternateSetting
    1, //bNumEndpoints
    USB_CLASS_AUDIO, //bInterfaceClass
    USB_SUBCLASS_AUDIOSTREAMING, //bInterfaceSubClass
    0, //bInterfaceProtocol
    0, //iInterface
  )
  ARRLEN1(//AS Interface
    bLENGTH, //bLength
    USB_DESCR_CS_INTERFACE, //bDescriptorType
    USBAUDIO_AS_GENERAL, //bDescriptorSubType
    2, //bTerminalLink  <----------------------------------------
    1, //bDelay //задержка, вносимая устройством (в единицах числа фреймов)
    USB_U16( USBAUDIO_FORMAT_PCM ), //wFormatTag=PCM
  )
  ARRLEN1(//AS Format Type 1
    bLENGTH, //bLength
    USB_DESCR_CS_INTERFACE, //bDescriptorType
    USBAUDIO_AS_FORMAT, //bDescriptorSubType
    1, //bFormatType
    1, //bNrChannels
    2, //bSubFrameSize //количество БАЙТОВ на отсчет (1-4)
    16, //bBitResolution //количество БИТОВ на отсчет (<= bSubFrameSize*8) //наверное, то-занимаемое в потоке место, а это - реальная разрешающая способность
    1, //bSamFreqType //количество поддерживаемых частот
    USB_AC24(F_SAMPLE), //tSamFreq //(6 байт!) массив диапазонов частот
  )
  ARRLEN1(//Endpoint descriptor
    bLENGTH, //bLength
    USB_DESCR_ENDPOINT, //bDescriptorType
    ENDP_MIC | 0x80, 
    USB_ENDP_ISO, //Isochronous / Synch=none / usage=data
    USB_U16( ENDP_MIC_SIZE ),
    1, //bInterval - частота опроса, для изохронных всегда 1
    0, //bRefresh - хз что это, сказано выставить в 0
    0, //bSynchAddress - адрес endpoint'а для синхронизации
  )
  ARRLEN1(//Isochronous endpoint descriptor
    bLENGTH, //bLength
    USB_DESCR_ENDP_ISO, //bDescriptorType
    1, //bDescriptorSubType
    0, //bmAttributes
    0, //bLockDelayUnits (undefned)
    USB_U16(0), //wLockDelay
  )
  
  )
};

void usb_class_get_std_descr(uint16_t descr, const void **data, uint16_t *size){
  switch(descr & 0xFF00){
    case DEVICE_DESCRIPTOR:
      *data = &USB_DeviceDescriptor;
      *size = sizeof(USB_DeviceDescriptor);
      break;
    case CONFIGURATION_DESCRIPTOR:
      *data = &USB_ConfigDescriptor;
      *size = sizeof(USB_ConfigDescriptor);
      break;
    case DEVICE_QUALIFIER_DESCRIPTOR:
      *data = &USB_DeviceQualifierDescriptor;
      *size = USB_DeviceQualifierDescriptor[0];
      break;
    case STRING_DESCRIPTOR:
      switch(descr & 0xFF){
        case STD_DESCR_LANG:
          *data = &USB_StringLangDescriptor;
          break;
        case STD_DESCR_VEND:
          *data = &USB_StringManufacturingDescriptor;
          break;
        case STD_DESCR_PROD:
          *data = &USB_StringProdDescriptor;
          break;
        case STD_DESCR_SN:
          *data = &USB_StringSerialDescriptor;
          break;
        case STR_TTY:
          *data = &USB_String_TTY;
          break;
        case STR_PROGR:
          *data = &USB_String_PROGR;
          break;
        default:
          return;
      }
      *size = ((uint8_t*)*data)[0]; //data->bLength
      break;
    default:
      break;
  }
}

char usb_class_ep0_in(config_pack_t *req, void **data, uint16_t *size){
  if( hid_ep0_in( req, data, size ) )return 1;
  if( mic_ep0_in( req, data, size ) )return 1;
  if( programmer_ep0_in( req, data, size ) )return 1;
  return 0;
}

char usb_class_ep0_out(config_pack_t *req, uint16_t offset, uint16_t rx_size){
  if( hid_ep0_out( req, offset, rx_size ) )return 1;
  if( mic_ep0_out( req, offset, rx_size ) )return 1;
  if( programmer_ep0_out( req, offset, rx_size ) )return 1;
  return 0;
}

void usb_class_init(){
  hid_init();
  mic_init();
  programmer_init();
}

void usb_class_poll(){
  hid_poll();
  mic_poll();
  programmer_poll();
}

void usb_class_sof(){
  mic_sof();
}
