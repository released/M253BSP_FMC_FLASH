# M253BSP_FMC_FLASH
 M253BSP_FMC_FLASH

update @ 2023/06/14

1. refer DataFlashProg function , under BSP sample code : USBD_VCOM_And_Mass_Storage

2. data flash start address : 0x10000

3. by terminal control

below is log message , 

![image](https://github.com/released/M253BSP_FMC_FLASH/blob/main/log.jpg)	

press digit 1 : write ram buffer data : File_Data_Buf , into flash , 

press digit 2 : read data from flash , to ram buffer : read_Data_Buf

![image](https://github.com/released/M253BSP_FMC_FLASH/blob/main/log_digit_1_2.jpg)	

press digit 3 : data in ram buffer data : File_Data_Buf , will increase counter

![image](https://github.com/released/M253BSP_FMC_FLASH/blob/main/log_digit_3.jpg)	

use ICP tool , to read data in On-board Flash 

![image](https://github.com/released/M253BSP_FMC_FLASH/blob/main/ICP_read_flash.jpg)	

