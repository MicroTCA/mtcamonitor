#include <iostream>
#include <strings.h>
#include <string.h>
#include <fstream>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "pciedev_io.h"

/* useconds from struct timeval */
#define	MIKRS(tv) (((double)(tv).tv_usec ) + ((double)(tv).tv_sec * 1000000.0)) 
#define	MILLS(tv) (((double)(tv).tv_usec/1000 )  + ((double)(tv).tv_sec * 1000.0)) 	
using namespace std;

int	      fd;
struct timeval   start_time;
struct timeval   end_time;

int main(int argc, char* argv[])
{
    char                  nod_name[25] = "";
    int                     tmp_rw = 1;
    int                     tmp_n   = 1;
    u_int                 tmp_data_rw = 0;
    int                     tmp_mode_rw;
    u_int	           tmp_offset_rw;
    int                     len = 0;
    int                     tmp_barx_rw = 0;
    int                     tmp_size = 1;
    int                     itemsize = 0;
    int*                   tmp_dma_buf;
    int*                   tmp_write_buf;
    double              time_tmp;
    double              time_dlt;
    double              time_drv;
    int                    k =0;
    FILE *              dmafile;
    
    device_rw                  l_Read;
    device_ioctrl_dma     DMA_RW;
    device_ioctrl_time     DMA_TIME;

    itemsize = sizeof(device_rw);
	
    if(argc < 5){
        cout<<"\nInput \"device_rw [device] [r/w] [data] [address] [bar] [mode] [words]\"\n ";
        cout<<"device - nodename (/dev/damcs1)\n ";
        cout<<"r/w 1 to read , 0 to write, 2 dma_read, 3 dma_write\n ";
        cout<<"data to write in hexadecimal\n ";
        cout<<"address in hexadecimal\n ";
        cout<<"bar: 0,1,2,3,4 - defaults to 0\n ";
        cout<<"mode: 32,16,8 - defaults to 32\n ";
        cout<<"words: number of values to read - defaults to 1\n ";
        return 0;
    }

    strncpy(nod_name,argv[1],sizeof(nod_name));
    sscanf(argv[2],"%x",&tmp_rw);
    if(argc >= 4)
        sscanf(argv[3],"%x",&tmp_data_rw);
    if(argc >= 5)
        sscanf(argv[4],"%x",&tmp_offset_rw);
    if(argc >= 6)
        sscanf(argv[5],"%d",&tmp_barx_rw);
    if(argc >= 7)
        sscanf(argv[6],"%d",&tmp_mode_rw);
    if(argc >= 8)
        sscanf(argv[7],"%d",&tmp_size);
	    
    switch (tmp_mode_rw){
        case 8:
                tmp_mode_rw = 0;
                tmp_n   = 1;
            break;
        case 16:
                tmp_mode_rw = 1;
                    tmp_n   = 2;
            break;
        case 32:
        default:
                tmp_mode_rw = 2;
                    tmp_n   = 4;
            break;
    }
	
    fd = open (nod_name, O_RDWR);
    if (fd < 0) {
        printf ("%i\n", -3);
        return 0;
    }

    l_Read.offset_rw  = tmp_offset_rw;
    l_Read.mode_rw   = tmp_mode_rw;
    l_Read.barx_rw    = tmp_barx_rw;
    l_Read.size_rw     = 0;
    l_Read.rsrvd_rw   = 0;
	
    switch (tmp_rw) {
         case 0:
             l_Read.data_rw    = tmp_data_rw;
             l_Read.offset_rw  = tmp_offset_rw;
             l_Read.mode_rw  = tmp_mode_rw;
             l_Read.barx_rw    = tmp_barx_rw;
             l_Read.size_rw     = 0;
             l_Read.rsrvd_rw   = 0;

             len = write (fd, &l_Read, 1);
             if (len <0 ){
                 printf ("%d \n", len);
                 close(fd);
                 return 0;
             }
             printf ("%d \n", 1);
             break;
         case 1:
            l_Read.data_rw   = 0;
            for(int count = 0; count < tmp_size; count++) {
                l_Read.offset_rw = tmp_offset_rw + (count * tmp_n);
                len = read (fd, &l_Read, 1);
                if (len <0 ){
                    printf ("%d \n", len);
                    close(fd);
                    return 0;
                }
                printf ("0x%08X\n",l_Read.data_rw);
            }
            break;
        case 2:
            DMA_RW.dma_offset    = 0;
            DMA_RW.dma_size       = 0;
            DMA_RW.dma_cmd      = 0;
            DMA_RW.dma_pattern = 0; 
            
            DMA_RW.dma_size    = sizeof(int)*tmp_size;
            DMA_RW.dma_offset = tmp_offset_rw;
            
            tmp_dma_buf     = new int[tmp_size + DMA_DATA_OFFSET];
            memcpy(tmp_dma_buf, &DMA_RW, sizeof (device_ioctrl_dma));

            gettimeofday(&start_time, 0);
            len = ioctl (fd, PCIEDEV_READ_DMA, tmp_dma_buf);
            gettimeofday(&end_time, 0);
            if (len < 0) {
                if(tmp_dma_buf) delete tmp_dma_buf;
                close(fd);
                printf ("ERROR %i\n", len);
                return 0;
            }
            time_dlt    =  MIKRS(end_time) - MIKRS(start_time);
            
            len = ioctl (fd, PCIEDEV_GET_DMA_TIME, &DMA_TIME);
            if (len < 0) {
                time_drv = 0;
                printf("%i:%s\n",(int)((sizeof(int)*tmp_size)/time_dlt), "NO DRV TIME");
            }else{
                time_drv = MIKRS(DMA_TIME.stop_time) - MIKRS(DMA_TIME.start_time);
                printf("%i:%i\n",(int)((sizeof(int)*tmp_size)/time_dlt), (int)((sizeof(int)*tmp_size)/time_drv));
            }
            dmafile = fopen ("/var/tmp/mtca_dma.dat", "w");
            if(dmafile != NULL){
                k = 0;
                for(int i = 0; i < tmp_size; i++){
                    //printf("NUM %i OFFSET %X : DATA %X\n", i,k, (u_int)(tmp_dma_buf[i] & 0xFFFFFFFF));
                    //printf("0x%08X\n", (u_int)(tmp_dma_buf[i] & 0xFFFFFFFF));
                    fprintf(dmafile, "0x%08X\n", (u_int)(tmp_dma_buf[i] & 0xFFFFFFFF));
                    k += 4;
                }
                fclose(dmafile);
            }
            if(tmp_dma_buf) delete tmp_dma_buf;
            break;
    }
    close(fd);
    return 0;
}

