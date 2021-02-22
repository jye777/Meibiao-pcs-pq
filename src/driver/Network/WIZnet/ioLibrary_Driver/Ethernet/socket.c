//*****************************************************************************
//
//! \file socket.c
//! \brief SOCKET APIs Implements file.
//! \details SOCKET APIs like as Berkeley Socket APIs. 
//! \version 1.0.3
//! \date 2013/10/21
//! \par  Revision history
//!       <2015/02/05> Notice
//!        The version history is not updated after this point.
//!        Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
//!        >> https://github.com/Wiznet/ioLibrary_Driver
//!       <2014/05/01> V1.0.3. Refer to M20140501
//!         1. Implicit type casting -> Explicit type casting.
//!         2. replace 0x01 with PACK_REMAINED in recvfrom()
//!         3. Validation a destination ip in connect() & sendto(): 
//!            It occurs a fatal error on converting unint32 address if uint8* addr parameter is not aligned by 4byte address.
//!            Copy 4 byte addr value into temporary uint32 variable and then compares it.
//!       <2013/12/20> V1.0.2 Refer to M20131220
//!                    Remove Warning.
//!       <2013/11/04> V1.0.1 2nd Release. Refer to "20131104".
//!                    In sendto(), Add to clear timeout interrupt status (Sn_IR_TIMEOUT)
//!       <2013/10/21> 1st Release
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2013, WIZnet Co., LTD.
//! All rights reserved.
//! 
//! Redistribution and use in source and binary forms, with or without 
//! modification, are permitted provided that the following conditions 
//! are met: 
//! 
//!     * Redistributions of source code must retain the above copyright 
//! notice, this list of conditions and the following disclaimer. 
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution. 
//!     * Neither the name of the <ORGANIZATION> nor the names of its 
//! contributors may be used to endorse or promote products derived 
//! from this software without specific prior written permission. 
//! 
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
#include "socket.h"

//M20150401 : Typing Error
//#define SOCK_ANY_PORT_NUM  0xC000;
#define SOCK_ANY_PORT_NUM  0xC000

static uint16_t sock_any_port[W5XX_CHIP_QTY] = {SOCK_ANY_PORT_NUM,SOCK_ANY_PORT_NUM,SOCK_ANY_PORT_NUM};
static uint16_t sock_io_mode[W5XX_CHIP_QTY] = {0};
static uint16_t sock_is_sending[W5XX_CHIP_QTY] = {0};

static uint16_t sock_remained_size[W5XX_CHIP_QTY][_WIZCHIP_SOCK_NUM_] = {0,0,};

//M20150601 : For extern decleation
//static uint8_t  sock_pack_info[_WIZCHIP_SOCK_NUM_] = {0,};
uint8_t  sock_pack_info[W5XX_CHIP_QTY][_WIZCHIP_SOCK_NUM_] = {0,};
//

#if _WIZCHIP_ == 5200
   static uint16_t sock_next_rd[W5XX_CHIP_QTY][_WIZCHIP_SOCK_NUM_] ={0,};
#endif

//A20150601 : For integrating with W5300
#if _WIZCHIP_ == 5300
   uint8_t sock_remained_byte[W5XX_CHIP_QTY][_WIZCHIP_SOCK_NUM_] = {0,}; // set by wiz_recv_data()
#endif

#define CHECK_CHIPID()   \
   do{                    \
      if(chipId > W5XX_CHIP_QTY) return SOCKERR_CHIPID;   \
   }while(0);             \


#define CHECK_SOCKNUM()   \
   do{                    \
      if(sn > _WIZCHIP_SOCK_NUM_) return SOCKERR_SOCKNUM;   \
   }while(0);             \

#define CHECK_SOCKMODE(mode)  \
   do{                     \
      if((getSn_MR(chipId,sn) & 0x0F) != mode) return SOCKERR_SOCKMODE;  \
   }while(0);              \

#define CHECK_SOCKINIT()   \
   do{                     \
      if((getSn_SR(chipId,sn) != SOCK_INIT)) return SOCKERR_SOCKINIT; \
   }while(0);              \

#define CHECK_SOCKDATA()   \
   do{                     \
      if(len == 0) return SOCKERR_DATALEN;   \
   }while(0);              \



int8_t w5xx_socket(uint32_t chipId,uint8_t sn, uint8_t protocol, uint16_t port, uint8_t flag)
{
    CHECK_CHIPID();
	CHECK_SOCKNUM();
	switch(protocol)
	{
      case Sn_MR_TCP :
         {
            //M20150601 : Fixed the warning - taddr will never be NULL
		    /*
            uint8_t taddr[4];
            getSIPR(taddr);
            */
            uint32_t taddr;
            getSIPR(chipId,(uint8_t*)&taddr);
            if(taddr == 0) return SOCKERR_SOCKINIT;
         }
      case Sn_MR_UDP :
      case Sn_MR_MACRAW :
	  case Sn_MR_IPRAW :
         break;
   #if ( _WIZCHIP_ < 5200 )
      case Sn_MR_PPPoE :
         break;
   #endif
      default :
         return SOCKERR_SOCKMODE;
	}
	//M20150601 : For SF_TCP_ALIGN & W5300
	//if((flag & 0x06) != 0) return SOCKERR_SOCKFLAG;
	if((flag & 0x04) != 0) return SOCKERR_SOCKFLAG;
#if _WIZCHIP_ == 5200
   if(flag & 0x10) return SOCKERR_SOCKFLAG;
#endif
	   
	if(flag != 0)
	{
   	switch(protocol)
   	{
   	   case Sn_MR_TCP:
   		  //M20150601 :  For SF_TCP_ALIGN & W5300
          #if _WIZCHIP_ == 5300
   		     if((flag & (SF_TCP_NODELAY|SF_IO_NONBLOCK|SF_TCP_ALIGN))==0) return SOCKERR_SOCKFLAG;
          #else
   		     if((flag & (SF_TCP_NODELAY|SF_IO_NONBLOCK))==0) return SOCKERR_SOCKFLAG;
          #endif

   	      break;
   	   case Sn_MR_UDP:
   	      if(flag & SF_IGMP_VER2)
   	      {
   	         if((flag & SF_MULTI_ENABLE)==0) return SOCKERR_SOCKFLAG;
   	      }
   	      #if _WIZCHIP_ == 5500
      	      if(flag & SF_UNI_BLOCK)
      	      {
      	         if((flag & SF_MULTI_ENABLE) == 0) return SOCKERR_SOCKFLAG;
      	      }
   	      #endif
   	      break;
   	   default:
   	      break;
   	}
   }
	w5xx_close(chipId,sn);
	//M20150601
	#if _WIZCHIP_ == 5300
	   setSn_MR(chipId,sn, ((uint16_t)(protocol | (flag & 0xF0))) | (((uint16_t)(flag & 0x02)) << 7) );
    #else
	   setSn_MR(chipId,sn, (protocol | (flag & 0xF0)));
    #endif
	if(!port)
	{
	   port = sock_any_port[chipId]++;
	   if(sock_any_port[chipId] == 0xFFF0) sock_any_port[chipId] = SOCK_ANY_PORT_NUM;
	}
   setSn_PORT(chipId,sn,port);	
   setSn_CR(chipId,sn,Sn_CR_OPEN);
   while(getSn_CR(chipId,sn));
   //A20150401 : For release the previous sock_io_mode
   sock_io_mode[chipId] &= ~(1 <<sn);
   //
	sock_io_mode[chipId] |= ((flag & SF_IO_NONBLOCK) << sn);   
   sock_is_sending[chipId] &= ~(1<<sn);
   sock_remained_size[chipId][sn] = 0;
   //M20150601 : repalce 0 with PACK_COMPLETED
   //sock_pack_info[sn] = 0;
   sock_pack_info[chipId][sn] = PACK_COMPLETED;
   //
   while(getSn_SR(chipId,sn) == SOCK_CLOSED);
   return (int8_t)sn;
}	   

int8_t w5xx_close(uint32_t chipId,uint8_t sn)
{
    CHECK_CHIPID();
	CHECK_SOCKNUM();
//A20160426 : Applied the erratum 1 of W5300
#if   (_WIZCHIP_ == 5300) 
   //M20160503 : Wrong socket parameter. s -> sn 
   //if( ((getSn_MR(s)& 0x0F) == Sn_MR_TCP) && (getSn_TX_FSR(s) != getSn_TxMAX(s)) ) 
   if( ((getSn_MR(chipId,sn)& 0x0F) == Sn_MR_TCP) && (getSn_TX_FSR(chipId,sn) != getSn_TxMAX(chipId,sn)) ) 
   { 
      uint8_t destip[4] = {0, 0, 0, 1};
      // TODO
      // You can wait for completing to sending data;
      // wait about 1 second;
      // if you have completed to send data, skip the code of erratum 1
      // ex> wait_1s();
      //     if (getSn_TX_FSR(s) == getSn_TxMAX(s)) continue;
      // 
      //M20160503 : The socket() of close() calls close() itself again. It occures a infinite loop - close()->socket()->close()->socket()-> ~
      //socket(s,Sn_MR_UDP,0x3000,0);
      //sendto(s,destip,1,destip,0x3000); // send the dummy data to an unknown destination(0.0.0.1).
      setSn_MR(chipId,sn,Sn_MR_UDP);
      setSn_PORTR(chipId,sn, 0x3000);
      setSn_CR(chipId,sn,Sn_CR_OPEN);
      while(getSn_CR(chipId,sn) != 0);
      while(getSn_SR(chipId,sn) != SOCK_UDP);
      w5xx_sendto(chipId,sn,destip,1,destip,0x3000); // send the dummy data to an unknown destination(0.0.0.1).
   };   
#endif 
	setSn_CR(chipId,sn,Sn_CR_CLOSE);
   /* wait to process the command... */
	while( getSn_CR(chipId,sn) );
	/* clear all interrupt of the socket. */
	setSn_IR(chipId,sn, 0xFF);
	//A20150401 : Release the sock_io_mode of socket n.
	sock_io_mode[chipId] &= ~(1<<sn);
	//
	sock_is_sending[chipId] &= ~(1<<sn);
	sock_remained_size[chipId][sn] = 0;
	sock_pack_info[chipId][sn] = 0;
	while(getSn_SR(chipId,sn) != SOCK_CLOSED);
	return SOCK_OK;
}

int8_t w5xx_listen(uint32_t chipId,uint8_t sn)
{
    CHECK_CHIPID();
    CHECK_SOCKNUM();
    CHECK_SOCKMODE(Sn_MR_TCP);
    CHECK_SOCKINIT();
	setSn_CR(chipId,sn,Sn_CR_LISTEN);
	while(getSn_CR(chipId,sn));
   while(getSn_SR(chipId,sn) != SOCK_LISTEN)
   {
         w5xx_close(chipId,sn);
         return SOCKERR_SOCKCLOSED;
   }
   return SOCK_OK;
}


int8_t w5xx_connect(uint32_t chipId,uint8_t sn, uint8_t * addr, uint16_t port)
{
    CHECK_CHIPID();
    CHECK_SOCKNUM();
    CHECK_SOCKMODE(Sn_MR_TCP);
    CHECK_SOCKINIT();
   //M20140501 : For avoiding fatal error on memory align mismatched
   //if( *((uint32_t*)addr) == 0xFFFFFFFF || *((uint32_t*)addr) == 0) return SOCKERR_IPINVALID;
   {
      uint32_t taddr;
      taddr = ((uint32_t)addr[0] & 0x000000FF);
      taddr = (taddr << 8) + ((uint32_t)addr[1] & 0x000000FF);
      taddr = (taddr << 8) + ((uint32_t)addr[2] & 0x000000FF);
      taddr = (taddr << 8) + ((uint32_t)addr[3] & 0x000000FF);
      if( taddr == 0xFFFFFFFF || taddr == 0) return SOCKERR_IPINVALID;
   }
   //
	
	if(port == 0) return SOCKERR_PORTZERO;
	setSn_DIPR(chipId,sn,addr);
	setSn_DPORT(chipId,sn,port);
	setSn_CR(chipId,sn,Sn_CR_CONNECT);
   while(getSn_CR(chipId,sn));
   if(sock_io_mode[chipId] & (1<<sn)) return SOCK_BUSY;
   while(getSn_SR(chipId,sn) != SOCK_ESTABLISHED)
   {
		if (getSn_IR(chipId,sn) & Sn_IR_TIMEOUT)
		{
			setSn_IR(chipId,sn, Sn_IR_TIMEOUT);
            return SOCKERR_TIMEOUT;
		}

		if (getSn_SR(chipId,sn) == SOCK_CLOSED)
		{
			return SOCKERR_SOCKCLOSED;
		}
	}
   
   return SOCK_OK;
}

int8_t w5xx_disconnect(uint32_t chipId,uint8_t sn)
{
    CHECK_CHIPID();
    CHECK_SOCKNUM();
    CHECK_SOCKMODE(Sn_MR_TCP);
    setSn_CR(chipId,sn,Sn_CR_DISCON);
    /* wait to process the command... */
    while(getSn_CR(chipId,sn));
    sock_is_sending[chipId] &= ~(1<<sn);
    if(sock_io_mode[chipId] & (1<<sn)) return SOCK_BUSY;
    while(getSn_SR(chipId,sn) != SOCK_CLOSED)
    {
        if(getSn_IR(chipId,sn) & Sn_IR_TIMEOUT)
        {
            w5xx_close(chipId,sn);
            return SOCKERR_TIMEOUT;
        }
    }
    return SOCK_OK;
}

int32_t w5xx_send(uint32_t chipId,uint8_t sn, uint8_t * buf, uint16_t len)
{
   CHECK_CHIPID();
   uint8_t tmp=0;
   uint16_t freesize=0;
   
   CHECK_SOCKNUM();
   CHECK_SOCKMODE(Sn_MR_TCP);
   CHECK_SOCKDATA();
   tmp = getSn_SR(chipId,sn);
   if(tmp != SOCK_ESTABLISHED && tmp != SOCK_CLOSE_WAIT) return SOCKERR_SOCKSTATUS;
   if( sock_is_sending[chipId] & (1<<sn) )
   {
      tmp = getSn_IR(chipId,sn);
      if(tmp & Sn_IR_SENDOK)
      {
         setSn_IR(chipId,sn, Sn_IR_SENDOK);
         //M20150401 : Typing Error
         //#if _WZICHIP_ == 5200
         #if _WIZCHIP_ == 5200
            if(getSn_TX_RD(chipId,sn) != sock_next_rd[chipId][sn])
            {
               setSn_CR(chipId,sn,Sn_CR_SEND);
               while(getSn_CR(chipId,sn));
               return SOCK_BUSY;
            }
         #endif
         sock_is_sending[chipId] &= ~(1<<sn);         
      }
      else if(tmp & Sn_IR_TIMEOUT)
      {
         w5xx_close(chipId,sn);
         return SOCKERR_TIMEOUT;
      }
      else return SOCK_BUSY;
   }
   freesize = getSn_TxMAX(chipId,sn);
   if (len > freesize) len = freesize; // check size not to exceed MAX size.
   while(1)
   {
      freesize = getSn_TX_FSR(chipId,sn);
      tmp = getSn_SR(chipId,sn);
      if ((tmp != SOCK_ESTABLISHED) && (tmp != SOCK_CLOSE_WAIT))
      {
         w5xx_close(chipId,sn);
         return SOCKERR_SOCKSTATUS;
      }
      if( (sock_io_mode[chipId] & (1<<sn)) && (len > freesize) ) return SOCK_BUSY;
      if(len <= freesize) break;
   }
   wiz_send_data(chipId,sn, buf, len);
   #if _WIZCHIP_ == 5200
      sock_next_rd[chipId][sn] = getSn_TX_RD(chipId,sn) + len;
   #endif

   #if _WIZCHIP_ == 5300
      setSn_TX_WRSR(chipId,sn,len);
   #endif
   
   setSn_CR(chipId,sn,Sn_CR_SEND);
   /* wait to process the command... */
   while(getSn_CR(chipId,sn));
   sock_is_sending[chipId] |= (1 << sn);
   //M20150409 : Explicit Type Casting
   //return len;
   return (int32_t)len;
}


int32_t w5xx_recv(uint32_t chipId,uint8_t sn, uint8_t * buf, uint16_t len)
{
   uint8_t  tmp = 0;
   uint16_t recvsize = 0;
//A20150601 : For integarating with W5300
#if   _WIZCHIP_ == 5300
   uint8_t head[2];
   uint16_t mr;
#endif
//
   CHECK_CHIPID();
   CHECK_SOCKNUM();
   CHECK_SOCKMODE(Sn_MR_TCP);
   CHECK_SOCKDATA();
   
   recvsize = getSn_RxMAX(chipId,sn);
   if(recvsize < len) len = recvsize;
      
//A20150601 : For Integrating with W5300
#if _WIZCHIP_ == 5300
   //sock_pack_info[sn] = PACK_COMPLETED;    // for clear      
   if(sock_remained_size[chipId][sn] == 0)
   {
#endif
//
      while(1)
      {
         recvsize = getSn_RX_RSR(chipId,sn);
         tmp = getSn_SR(chipId,sn);
         if (tmp != SOCK_ESTABLISHED)
         {
            if(tmp == SOCK_CLOSE_WAIT)
            {
               if(recvsize != 0) break;
               else if(getSn_TX_FSR(chipId,sn) == getSn_TxMAX(chipId,sn))
               {
                  w5xx_close(chipId,sn);
                  return SOCKERR_SOCKSTATUS;
               }
            }
            else
            {
               w5xx_close(chipId,sn);
               return SOCKERR_SOCKSTATUS;
            }
         }
         if((sock_io_mode[chipId] & (1<<sn)) && (recvsize == 0)) return SOCK_BUSY;
         if(recvsize != 0) break;
      };
#if _WIZCHIP_ == 5300
   }
#endif

//A20150601 : For integrating with W5300
#if _WIZCHIP_ == 5300
   if((sock_remained_size[chipId][sn] == 0) || (getSn_MR(chipId,sn) & Sn_MR_ALIGN))
   {
      mr = getMR(chipId);
      if((getSn_MR(chipId,sn) & Sn_MR_ALIGN)==0)
      {
         wiz_recv_data(chipId,sn,head,2);
         if(mr & MR_FS)
            recvsize = (((uint16_t)head[1]) << 8) | ((uint16_t)head[0]);
         else
            recvsize = (((uint16_t)head[0]) << 8) | ((uint16_t)head[1]);
         sock_pack_info[chipId][sn] = PACK_FIRST;
      }
      sock_remained_size[chipId][sn] = recvsize;
   }
   if(len > sock_remained_size[chipId][sn]) len = sock_remained_size[chipId][sn];
   recvsize = len;   
   if(sock_pack_info[chipId][sn] & PACK_FIFOBYTE)
   {
      *buf = sock_remained_byte[chipId][sn];
      buf++;
      sock_pack_info[chipId][sn] &= ~(PACK_FIFOBYTE);
      recvsize -= 1;
      sock_remained_size[chipId][sn] -= 1;
   }
   if(recvsize != 0)
   {
      wiz_recv_data(chipId,sn, buf, recvsize);
      setSn_CR(chipId,sn,Sn_CR_RECV);
      while(getSn_CR(chipId,sn));
   }
   sock_remained_size[chipId][sn] -= recvsize;
   if(sock_remained_size[chipId][sn] != 0)
   {
      sock_pack_info[chipId][sn] |= PACK_REMAINED;
      if(recvsize & 0x1) sock_pack_info[chipId][sn] |= PACK_FIFOBYTE;
   }
   else sock_pack_info[chipId][sn] = PACK_COMPLETED;
   if(getSn_MR(chipId,sn) & Sn_MR_ALIGN) sock_remained_size[chipId][sn] = 0;
   //len = recvsize;
#else   
   if(recvsize < len) len = recvsize;   
   wiz_recv_data(chipId,sn, buf, len);
   setSn_CR(chipId,sn,Sn_CR_RECV);
   while(getSn_CR(chipId,sn));
#endif
     
   //M20150409 : Explicit Type Casting
   //return len;
   return (int32_t)len;
}

int32_t w5xx_sendto(uint32_t chipId,uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port)
{
   uint8_t tmp = 0;
   uint16_t freesize = 0;
   uint32_t taddr;
   CHECK_CHIPID();

   CHECK_SOCKNUM();
   switch(getSn_MR(chipId,sn) & 0x0F)
   {
      case Sn_MR_UDP:
      case Sn_MR_MACRAW:
//         break;
//   #if ( _WIZCHIP_ < 5200 )
      case Sn_MR_IPRAW:
         break;
//   #endif
      default:
         return SOCKERR_SOCKMODE;
   }
   CHECK_SOCKDATA();
   //M20140501 : For avoiding fatal error on memory align mismatched
   //if(*((uint32_t*)addr) == 0) return SOCKERR_IPINVALID;
   //{
      //uint32_t taddr;
      taddr = ((uint32_t)addr[0]) & 0x000000FF;
      taddr = (taddr << 8) + ((uint32_t)addr[1] & 0x000000FF);
      taddr = (taddr << 8) + ((uint32_t)addr[2] & 0x000000FF);
      taddr = (taddr << 8) + ((uint32_t)addr[3] & 0x000000FF);
   //}
   //
   //if(*((uint32_t*)addr) == 0) return SOCKERR_IPINVALID;
   if((taddr == 0) && ((getSn_MR(chipId,sn)&Sn_MR_MACRAW) != Sn_MR_MACRAW)) return SOCKERR_IPINVALID;
   if((port  == 0) && ((getSn_MR(chipId,sn)&Sn_MR_MACRAW) != Sn_MR_MACRAW)) return SOCKERR_PORTZERO;
   tmp = getSn_SR(chipId,sn);
//#if ( _WIZCHIP_ < 5200 )
   if((tmp != SOCK_MACRAW) && (tmp != SOCK_UDP) && (tmp != SOCK_IPRAW)) return SOCKERR_SOCKSTATUS;
//#else
//   if(tmp != SOCK_MACRAW && tmp != SOCK_UDP) return SOCKERR_SOCKSTATUS;
//#endif
      
   setSn_DIPR(chipId,sn,addr);
   setSn_DPORT(chipId,sn,port);      
   freesize = getSn_TxMAX(chipId,sn);
   if (len > freesize) len = freesize; // check size not to exceed MAX size.
   while(1)
   {
      freesize = getSn_TX_FSR(chipId,sn);
      if(getSn_SR(chipId,sn) == SOCK_CLOSED) return SOCKERR_SOCKCLOSED;
      if( (sock_io_mode[chipId] & (1<<sn)) && (len > freesize) ) return SOCK_BUSY;
      if(len <= freesize) break;
   };
	wiz_send_data(chipId,sn, buf, len);

   #if _WIZCHIP_ < 5500   //M20150401 : for WIZCHIP Errata #4, #5 (ARP errata)
      getSIPR(chipId,(uint8_t*)&taddr);
      if(taddr == 0)
      {
         getSUBR(chipId,(uint8_t*)&taddr);
         setSUBR(chipId,(uint8_t*)"\x00\x00\x00\x00");
      }
      else taddr = 0;
   #endif

//A20150601 : For W5300
#if _WIZCHIP_ == 5300
   setSn_TX_WRSR(chipId,sn, len);
#endif
//   
	setSn_CR(chipId,sn,Sn_CR_SEND);
	/* wait to process the command... */
	while(getSn_CR(chipId,sn));
   while(1)
   {
      tmp = getSn_IR(chipId,sn);
      if(tmp & Sn_IR_SENDOK)
      {
         setSn_IR(chipId,sn, Sn_IR_SENDOK);
         break;
      }
      //M:20131104
      //else if(tmp & Sn_IR_TIMEOUT) return SOCKERR_TIMEOUT;
      else if(tmp & Sn_IR_TIMEOUT)
      {
         setSn_IR(chipId,sn, Sn_IR_TIMEOUT);
         //M20150409 : Fixed the lost of sign bits by type casting.
         //len = (uint16_t)SOCKERR_TIMEOUT;
         //break;
         #if _WIZCHIP_ < 5500   //M20150401 : for WIZCHIP Errata #4, #5 (ARP errata)
            if(taddr) setSUBR(chipId,(uint8_t*)&taddr);
         #endif
         return SOCKERR_TIMEOUT;
      }
      ////////////
   }
   #if _WIZCHIP_ < 5500   //M20150401 : for WIZCHIP Errata #4, #5 (ARP errata)
      if(taddr) setSUBR(chipId,(uint8_t*)&taddr);
   #endif
   //M20150409 : Explicit Type Casting
   //return len;
   return (int32_t)len;
}



int32_t w5xx_recvfrom(uint32_t chipId,uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port)
{
//M20150601 : For W5300   
#if _WIZCHIP_ == 5300
    uint16_t mr;
    uint16_t mr1;
#else   
    uint8_t  mr;
#endif
//   
    uint8_t  head[8];
    uint16_t pack_len=0;
    CHECK_CHIPID();

    CHECK_SOCKNUM();
   //CHECK_SOCKMODE(Sn_MR_UDP);
//A20150601
#if _WIZCHIP_ == 5300
   mr1 = getMR(chipId);
#endif   

   switch((mr=getSn_MR(chipId,sn)) & 0x0F)
   {
      case Sn_MR_UDP:
	  case Sn_MR_IPRAW:
      case Sn_MR_MACRAW:
         break;
   #if ( _WIZCHIP_ < 5200 )         
      case Sn_MR_PPPoE:
         break;
   #endif
      default:
         return SOCKERR_SOCKMODE;
   }
   CHECK_SOCKDATA();
   if(sock_remained_size[chipId][sn] == 0)
   {
      while(1)
      {
         pack_len = getSn_RX_RSR(chipId,sn);
         if(getSn_SR(chipId,sn) == SOCK_CLOSED) return SOCKERR_SOCKCLOSED;
         if( (sock_io_mode[chipId] & (1<<sn)) && (pack_len == 0) ) return SOCK_BUSY;
         if(pack_len != 0) break;
      };
   }
//D20150601 : Move it to bottom
// sock_pack_info[sn] = PACK_COMPLETED;
	switch (mr & 0x07)
	{
	   case Sn_MR_UDP :
	      if(sock_remained_size[chipId][sn] == 0)
	      {
   			wiz_recv_data(chipId,sn, head, 8);
   			setSn_CR(chipId,sn,Sn_CR_RECV);
   			while(getSn_CR(chipId,sn));
   			// read peer's IP address, port number & packet length
   	   //A20150601 : For W5300
   		#if _WIZCHIP_ == 5300
   		   if(mr1 & MR_FS)
   		   {
   		      addr[0] = head[1];
   		      addr[1] = head[0];
   		      addr[2] = head[3];
   		      addr[3] = head[2];
   		      *port = head[5];
   		      *port = (*port << 8) + head[4];
      			sock_remained_size[chipId][sn] = head[7];
      			sock_remained_size[chipId][sn] = (sock_remained_size[chipId][sn] << 8) + head[6];
   		   }
            else
            {
         #endif
                addr[0] = head[0];
      			addr[1] = head[1];
      			addr[2] = head[2];
      			addr[3] = head[3];
      			*port = head[4];
      			*port = (*port << 8) + head[5];
      			sock_remained_size[chipId][sn] = head[6];
      			sock_remained_size[chipId][sn] = (sock_remained_size[chipId][sn] << 8) + head[7];
         #if _WIZCHIP_ == 5300
            }
         #endif
   			sock_pack_info[chipId][sn] = PACK_FIRST;
   	   }
			if(len < sock_remained_size[chipId][sn]) pack_len = len;
			else pack_len = sock_remained_size[chipId][sn];
			//A20150601 : For W5300
			len = pack_len;
			#if _WIZCHIP_ == 5300
			   if(sock_pack_info[chipId][sn] & PACK_FIFOBYTE)
			   {
			      *buf++ = sock_remained_byte[chipId][sn];
			      pack_len -= 1;
			      sock_remained_size[chipId][sn] -= 1;
			      sock_pack_info[chipId][sn] &= ~PACK_FIFOBYTE;
			   }
			#endif
			//
			// Need to packet length check (default 1472)
			//
   		wiz_recv_data(chipId,sn, buf, pack_len); // data copy.
			break;
	   case Sn_MR_MACRAW :
	      if(sock_remained_size[chipId][sn] == 0)
	      {
   			wiz_recv_data(chipId,sn, head, 2);
   			setSn_CR(chipId,sn,Sn_CR_RECV);
   			while(getSn_CR(chipId,sn));
   			// read peer's IP address, port number & packet length
    			sock_remained_size[chipId][sn] = head[0];
   			sock_remained_size[chipId][sn] = (sock_remained_size[chipId][sn] <<8) + head[1] -2;
   			#if _WIZCHIP_ == W5300
   			if(sock_remained_size[chipId][sn] & 0x01)
   				sock_remained_size[chipId][sn] = sock_remained_size[chipId][sn] + 1 - 4;
   			else
   				sock_remained_size[chipId][sn] -= 4;
			#endif
   			if(sock_remained_size[chipId][sn] > 1514) 
   			{
   			   w5xx_close(chipId,sn);
   			   return SOCKFATAL_PACKLEN;
   			}
   			sock_pack_info[chipId][sn] = PACK_FIRST;
   	   }
			if(len < sock_remained_size[chipId][sn]) pack_len = len;
			else pack_len = sock_remained_size[chipId][sn];
			wiz_recv_data(chipId,sn,buf,pack_len);
		   break;
   //#if ( _WIZCHIP_ < 5200 )
		case Sn_MR_IPRAW:
		   if(sock_remained_size[chipId][sn] == 0)
		   {
   			wiz_recv_data(chipId,sn, head, 6);
   			setSn_CR(chipId,sn,Sn_CR_RECV);
   			while(getSn_CR(chipId,sn));
   			addr[0] = head[0];
   			addr[1] = head[1];
   			addr[2] = head[2];
   			addr[3] = head[3];
   			sock_remained_size[chipId][sn] = head[4];
   			//M20150401 : For Typing Error
   			//sock_remaiend_size[sn] = (sock_remained_size[sn] << 8) + head[5];
   			sock_remained_size[chipId][sn] = (sock_remained_size[chipId][sn] << 8) + head[5];
   			sock_pack_info[chipId][sn] = PACK_FIRST;
         }
			//
			// Need to packet length check
			//
			if(len < sock_remained_size[chipId][sn]) pack_len = len;
			else pack_len = sock_remained_size[chipId][sn];
   		    wiz_recv_data(chipId,sn, buf, pack_len); // data copy.
			break;
   //#endif
      default:
         wiz_recv_ignore(chipId,sn, pack_len); // data copy.
         sock_remained_size[chipId][sn] = pack_len;
         break;
   }
	setSn_CR(chipId,sn,Sn_CR_RECV);
	/* wait to process the command... */
	while(getSn_CR(chipId,sn)) ;
	sock_remained_size[chipId][sn] -= pack_len;
	//M20150601 : 
	//if(sock_remained_size[sn] != 0) sock_pack_info[sn] |= 0x01;
	if(sock_remained_size[chipId][sn] != 0)
	{
	   sock_pack_info[chipId][sn] |= PACK_REMAINED;
   #if _WIZCHIP_ == 5300	   
	   if(pack_len & 0x01) sock_pack_info[chipId][sn] |= PACK_FIFOBYTE;
   #endif	      
	}
	else sock_pack_info[chipId][sn] = PACK_COMPLETED;
#if _WIZCHIP_ == 5300	   
   pack_len = len;
#endif
   //
   //M20150409 : Explicit Type Casting
   //return pack_len;
   return (int32_t)pack_len;
}


int8_t  w5xx_ctlsocket(uint32_t chipId,uint8_t sn, ctlsock_type cstype, void* arg)
{
   uint8_t tmp = 0;
   CHECK_CHIPID();
   CHECK_SOCKNUM();
   switch(cstype)
   {
      case CS_SET_IOMODE:
         tmp = *((uint8_t*)arg);
         if(tmp == SOCK_IO_NONBLOCK)  sock_io_mode[chipId] |= (1<<sn);
         else if(tmp == SOCK_IO_BLOCK) sock_io_mode[chipId] &= ~(1<<sn);
         else return SOCKERR_ARG;
         break;
      case CS_GET_IOMODE:   
         //M20140501 : implict type casting -> explict type casting
         //*((uint8_t*)arg) = (sock_io_mode >> sn) & 0x0001;
         *((uint8_t*)arg) = (uint8_t)((sock_io_mode[chipId] >> sn) & 0x0001);
         //
         break;
      case CS_GET_MAXTXBUF:
         *((uint16_t*)arg) = getSn_TxMAX(chipId,sn);
         break;
      case CS_GET_MAXRXBUF:    
         *((uint16_t*)arg) = getSn_RxMAX(chipId,sn);
         break;
      case CS_CLR_INTERRUPT:
         if( (*(uint8_t*)arg) > SIK_ALL) return SOCKERR_ARG;
         setSn_IR(chipId,sn,*(uint8_t*)arg);
         break;
      case CS_GET_INTERRUPT:
         *((uint8_t*)arg) = getSn_IR(chipId,sn);
         break;
   #if _WIZCHIP_ != 5100
      case CS_SET_INTMASK:  
         if( (*(uint8_t*)arg) > SIK_ALL) return SOCKERR_ARG;
         setSn_IMR(chipId,sn,*(uint8_t*)arg);
         break;
      case CS_GET_INTMASK:   
         *((uint8_t*)arg) = getSn_IMR(chipId,sn);
         break;
   #endif
      default:
         return SOCKERR_ARG;
   }
   return SOCK_OK;
}

int8_t  w5xx_setsockopt(uint32_t chipId,uint8_t sn, sockopt_type sotype, void* arg)
{
 // M20131220 : Remove warning
 //uint8_t tmp;
   CHECK_CHIPID();
   CHECK_SOCKNUM();
   switch(sotype)
   {
      case SO_TTL:
         setSn_TTL(chipId,sn,*(uint8_t*)arg);
         break;
      case SO_TOS:
         setSn_TOS(chipId,sn,*(uint8_t*)arg);
         break;
      case SO_MSS:
         setSn_MSSR(chipId,sn,*(uint16_t*)arg);
         break;
      case SO_DESTIP:
         setSn_DIPR(chipId,sn, (uint8_t*)arg);
         break;
      case SO_DESTPORT:
         setSn_DPORT(chipId,sn, *(uint16_t*)arg);
         break;
#if _WIZCHIP_ != 5100
      case SO_KEEPALIVESEND:
         CHECK_SOCKMODE(Sn_MR_TCP);
         #if _WIZCHIP_ > 5200
            if(getSn_KPALVTR(chipId,sn) != 0) return SOCKERR_SOCKOPT;
         #endif
            setSn_CR(chipId,sn,Sn_CR_SEND_KEEP);
            while(getSn_CR(chipId,sn) != 0)
            {
               // M20131220
         		//if ((tmp = getSn_IR(sn)) & Sn_IR_TIMEOUT)
               if (getSn_IR(chipId,sn) & Sn_IR_TIMEOUT)
         		{
         			setSn_IR(chipId,sn, Sn_IR_TIMEOUT);
                  return SOCKERR_TIMEOUT;
         		}
            }
         break;
   #if _WIZCHIP_ > 5100
      case SO_KEEPALIVEAUTO:
         CHECK_SOCKMODE(Sn_MR_TCP);
         setSn_KPALVTR(chipId,sn,*(uint8_t*)arg);
         break;
   #endif      
#endif   
      default:
         return SOCKERR_ARG;
   }   
   return SOCK_OK;
}

int8_t  w5xx_getsockopt(uint32_t chipId,uint8_t sn, sockopt_type sotype, void* arg)
{
    CHECK_CHIPID();
   CHECK_SOCKNUM();
   switch(sotype)
   {
      case SO_FLAG:
         *(uint8_t*)arg = getSn_MR(chipId,sn) & 0xF0;
         break;
      case SO_TTL:
         *(uint8_t*) arg = getSn_TTL(chipId,sn);
         break;
      case SO_TOS:
         *(uint8_t*) arg = getSn_TOS(chipId,sn);
         break;
      case SO_MSS:   
         *(uint16_t*) arg = getSn_MSSR(chipId,sn);
         break;
      case SO_DESTIP:
         getSn_DIPR(chipId,sn, (uint8_t*)arg);
         break;
      case SO_DESTPORT:  
         *(uint16_t*) arg = getSn_DPORT(chipId,sn);
         break;
   #if _WIZCHIP_ > 5200   
      case SO_KEEPALIVEAUTO:
         CHECK_SOCKMODE(Sn_MR_TCP);
         *(uint16_t*) arg = getSn_KPALVTR(chipId,sn);
         break;
   #endif      
      case SO_SENDBUF:
         *(uint16_t*) arg = getSn_TX_FSR(chipId,sn);
         break;
      case SO_RECVBUF:
         *(uint16_t*) arg = getSn_RX_RSR(chipId,sn);
         break;
      case SO_STATUS:
         *(uint8_t*) arg = getSn_SR(chipId,sn);
         break;
      case SO_REMAINSIZE:
         if(getSn_MR(chipId,sn) & Sn_MR_TCP)
            *(uint16_t*)arg = getSn_RX_RSR(chipId,sn);
         else
            *(uint16_t*)arg = sock_remained_size[chipId][sn];
         break;
      case SO_PACKINFO:
         //CHECK_SOCKMODE(Sn_MR_TCP);
#if _WIZCHIP_ != 5300
         if((getSn_MR(chipId,sn) == Sn_MR_TCP))
             return SOCKERR_SOCKMODE;
#endif
         *(uint8_t*)arg = sock_pack_info[chipId][sn];
         break;
      default:
         return SOCKERR_SOCKOPT;
   }
   return SOCK_OK;
}
