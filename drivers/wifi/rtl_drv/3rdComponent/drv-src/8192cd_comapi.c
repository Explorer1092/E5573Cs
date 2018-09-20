/*
 *   API-compatible handling routines
 *
 *
 *
 *  Copyright (c) 2009 Realtek Semiconductor Corp.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#define _8192CD_COMAPI_C_


#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/unistd.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/delay.h>
#endif

#ifdef __LINUX_2_6__
#include <linux/initrd.h>
#include <linux/syscalls.h>
#endif

#include "./8192cd_debug.h"
#include "./8192cd_comapi.h"
#include "./8192cd_headers.h"
 
#ifdef CONFIG_RTL_COMAPI_WLTOOLS
#include <linux/if_arp.h>
#include <net/iw_handler.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#define IWE_STREAM_ADD_EVENT(_A, _B, _C, _D, _E)		iwe_stream_add_event(_A, _B, _C, _D, _E)
#define IWE_STREAM_ADD_POINT(_A, _B, _C, _D, _E)		iwe_stream_add_point(_A, _B, _C, _D, _E)
#define IWE_STREAM_ADD_VALUE(_A, _B, _C, _D, _E, _F)	iwe_stream_add_value(_A, _B, _C, _D, _E, _F)
#else
#define IWE_STREAM_ADD_EVENT(_A, _B, _C, _D, _E)		iwe_stream_add_event(_B, _C, _D, _E)
#define IWE_STREAM_ADD_POINT(_A, _B, _C, _D, _E)		iwe_stream_add_point(_B, _C, _D, _E)
#define IWE_STREAM_ADD_VALUE(_A, _B, _C, _D, _E, _F)	iwe_stream_add_value(_B, _C, _D, _E, _F)
#endif

typedef struct _CH_FREQ_MAP_{
	UINT16		channel;
	UINT16		freqKHz;
}CH_FREQ_MAP;

CH_FREQ_MAP CH_HZ_ID_MAP[] =
{
	{1, 2412},
	{2, 2417},
	{3, 2422},
	{4, 2427},
	{5, 2432},
	{6, 2437},
	{7, 2442},
	{8, 2447},
	{9, 2452},
	{10, 2457},
	{11, 2462},
	{12, 2467},
	{13, 2472},
	{14, 2484},

	/*	UNII */
	{36, 5180},
	{40, 5200},
	{44, 5220},
	{48, 5240},
	{52, 5260},
	{56, 5280},
	{60, 5300},
	{64, 5320},
	{149, 5745},
	{153, 5765},
	{157, 5785},
	{161, 5805},
	{165, 5825},
	{167, 5835},
	{169, 5845},
	{171, 5855},
	{173, 5865},
				
	/* HiperLAN2 */
	{100, 5500},
	{104, 5520},
	{108, 5540},
	{112, 5560},
	{116, 5580},
	{120, 5600},
	{124, 5620},
	{128, 5640},
	{132, 5660},
	{136, 5680},
	{140, 5700},
				
	/* Japan MMAC */
	{34, 5170},
	{38, 5190},
	{42, 5210},
	{46, 5230},
			
	/*	Japan */
	{184, 4920},
	{188, 4940},
	{192, 4960},
	{196, 4980},
	
	{208, 5040},	/* Japan, means J08 */
	{212, 5060},	/* Japan, means J12 */	 
	{216, 5080},	/* Japan, means J16 */
};
	
int CH_HZ_ID_MAP_NUM = (sizeof(CH_HZ_ID_MAP)/sizeof(CH_FREQ_MAP));


#define     MAP_CHANNEL_ID_TO_KHZ(_ch, _khz)                 			\
		do{                                           								\
			int _chIdx;											\
			for (_chIdx = 0; _chIdx < CH_HZ_ID_MAP_NUM; _chIdx++)\
			{													\
				if ((_ch) == CH_HZ_ID_MAP[_chIdx].channel)			\
				{												\
					(_khz) = CH_HZ_ID_MAP[_chIdx].freqKHz * 1000; 	\
					break;										\
				}												\
			}													\
			if (_chIdx == CH_HZ_ID_MAP_NUM)					\
				(_khz) = 2412000;									\
		}while(0)

#define     MAP_KHZ_TO_CHANNEL_ID(_khz, _ch)                 \
			do{ 																		\
				int _chIdx; 										\
				for (_chIdx = 0; _chIdx < CH_HZ_ID_MAP_NUM; _chIdx++)\
				{													\
					if ((_khz) == CH_HZ_ID_MAP[_chIdx].freqKHz) 		\
					{												\
						(_ch) = CH_HZ_ID_MAP[_chIdx].channel;			\
						break;										\
					}												\
				}													\
				if (_chIdx == CH_HZ_ID_MAP_NUM) 				\
					(_ch) = 1;											\
			}while(0)
							
/*
struct iw_statistics *rtl8192cd_get_wireless_stats(struct net_device *net_dev)
{
	// client mode only
	return NULL;
}
*/

void use_ap_scan( RTL_PRIV *priv);

#if 0
void switch_chan(struct rtl8192cd_priv *priv, struct bss_desc *bss_desp)
{
      RTL_PRIV *r_priv;
      u8 is_40m, sec_chan;
      unsigned long flags;

      printk("vxd chan follows\n");

      //RTL_W8(TXPAUSE, 0xff);

      priv->pmib->dot11RFEntry.dot11channel =  bss_desp->channel;

      if ((bss_desp->t_stamp[1] & (BIT(1) | BIT(2))) == (BIT(1) | BIT(2)))
          sec_chan = HT_2NDCH_OFFSET_BELOW;
      else if ((bss_desp->t_stamp[1] & (BIT(1)|BIT(2))) == BIT(1))
          sec_chan = HT_2NDCH_OFFSET_ABOVE;
      else
          sec_chan = 0;
 
      if ( sec_chan != 0 )
          is_40m = 1;

      if ( IS_VXD_INTERFACE(priv) ) {         
          r_priv = GET_ROOT(priv);

          priv->pmib->dot11RFEntry.dot11channel =  bss_desp->channel;
          r_priv->pmib->dot11RFEntry.dot11channel = bss_desp->channel;

          if ( priv->pshare->is_40m_bw && is_40m ) {
              // SAVE_INT_AND_CLI(flags);
              /* set root intf */              
              r_priv->pmib->dot11nConfigEntry.dot11n2ndChOffset = sec_chan;    
          
              /* set vxd intf */
              priv->pshare->offset_2nd_chan = sec_chan;

              // RESTORE_INT(flags);
          }
           
          if ( r_priv->pmib->dot11BssType.net_work_type & WIRELESS_11N )
              r_priv->ht_cap_len = 0;
          
	      init_beacon(r_priv);

          //switch_chan_to_vxd(r_priv);  
      } else
          r_priv = priv;

      if ( priv->pshare->is_40m_bw ) {
           // priv->pshare->CurrentChannelBW = HT_CHANNEL_WIDTH_20_40;
           SwBWMode(r_priv, priv->pshare->CurrentChannelBW, priv->pshare->offset_2nd_chan);      
           SwChnl(r_priv, priv->pmib->dot11Bss.channel, priv->pshare->offset_2nd_chan);
      } 
      else
      {
           // priv->pshare->CurrentChannelBW = HT_CHANNEL_WIDTH_20;
           //SwBWMode(r_priv, priv->pshare->CurrentChannelBW, 0);
           SwChnl(r_priv, priv->pmib->dot11Bss.channel, 0);
      }
      
      // RTL_W8(TXPAUSE, 0x00);

      // update_beacon
}
#endif

void set_hw_mac(struct rtl8192cd_priv *priv)
{

     struct net_device *dev = priv->dev;

     u8  val8;
     unsigned long reg;

     reg = *(unsigned long *)(dev->dev_addr);
     RTL_W32(MACID1, (cpu_to_le32(reg)));
     reg = *(unsigned short *)((unsigned long)dev->dev_addr + 4);
     RTL_W16(MACID1+4, (cpu_to_le16(reg)));

}

int rtl_wx_dummy(struct net_device *dev, 
                 struct iw_request_info *info, 
                 union iwreq_data *wrqu, char *extra)
{
    return -1;
}
 

int rtl_wx_join(struct rtl8192cd_priv *priv, struct bss_desc *bss_desp)
{
	char tmpbuf[33];

	printk("cliW: join opmode: %x res:%x \n", OPMODE, priv->join_res);
	if (!netif_running(priv->dev))
	{
	    printk("WiFi driver is NOT open!!\n");
	    return 1;
	}
	else if (priv->ss_req_ongoing)
	{
	    printk("Site Survey is not finished yet!!\n");
	    return 2;
	}

	//if ( priv->join_res == STATE_Sta_No_Bss )
	//    goto cnti_join;
	
    //	memcpy((void *)&(priv->pmib->dot11Bss) ,
    // 	(void *)&priv->site_survey->bss_backup[bss_num] , sizeof(struct bss_desc));

        memcpy((void *)&(priv->pmib->dot11Bss),
	       (void *)bss_desp, sizeof(struct bss_desc));

#if 0
	if ( priv->join_req_ongoing ) {
	    printk("cliW: no join under connecinting \n");
	    return 0;
    }
#endif

	if( ( priv->pmib->dot11Bss.ssidlen < 0) || (priv->pmib->dot11Bss.ssidlen > 32) ) {
	    printk("cliW: wrong ssid len\n");    	    
	    return 3;
	}	
#if 0
	if (priv->ss_req_ongoing || priv->join_req_ongoing ) {
	     printk("cliW: no join under scanning \n");
         return 0;	     
	}
#endif

        if ( OPMODE & ( WIFI_AUTH_STATE1 | WIFI_AUTH_SUCCESS | WIFI_ASOC_STATE) ) {
            printk("cliW: no joining when in auth\n");
            return 4;
        }

#if 0
    if ( OPMODE &  WIFI_ASOC_STATE || (priv->join_res == STATE_Sta_Bss) ) {       
          printk("cliW: no joining when in asso\n");
          return 0;	     
	}
#endif

//cnti_join:

#ifdef WIFI_SIMPLE_CONFIG
	//_Eric if (priv->pmib->wscEntry.wsc_enable && (priv->pmib->dot11Bss.bsstype&WIFI_WPS)) 
	if (priv->pmib->wscEntry.wsc_enable)
	{
		//priv->pmib->dot11Bss.bsstype &= ~WIFI_WPS;
		priv->wps_issue_join_req = 1;
	}
	else
#endif
	{
		if (check_bss_encrypt(priv) == FAIL)
		{
			printk("Encryption mismatch!\n");
			return 5;
		}
	}

	if ((priv->pmib->dot11Bss.ssidlen == 0) || (priv->pmib->dot11Bss.ssid[0] == '\0')) 
	{
		printk("Error !! Join to a hidden AP!\n");
		return 6;
	}

#ifdef UNIVERSAL_REPEATER
	disable_vxd_ap(GET_VXD_PRIV(priv));
#endif

	memcpy(tmpbuf, priv->pmib->dot11Bss.ssid, priv->pmib->dot11Bss.ssidlen);
	tmpbuf[priv->pmib->dot11Bss.ssidlen] = '\0';
	printk("going to join bss: %s\n", tmpbuf);

	memcpy(SSID2SCAN, priv->pmib->dot11Bss.ssid, priv->pmib->dot11Bss.ssidlen);
	SSID2SCAN_LEN = priv->pmib->dot11Bss.ssidlen;

	SSID_LEN = SSID2SCAN_LEN;
	memcpy(SSID, SSID2SCAN, SSID_LEN);
	memset(BSSID, 0, MACADDRLEN);

#ifdef INCLUDE_WPA_PSK //_Eric ??
	//if (priv->pmib->dot1180211AuthEntry.dot11EnablePSK)
		//derivePSK(priv);
#endif

	priv->join_req_ongoing = 1;
	priv->authModeRetry = 0;

    //switch_chan(priv, bss_desp);

        /* cliW: todo list */
	//if ( IS_VXD_INTERFACE(priv) )
        //set_hw_mac(priv);
 	 
        printk("cliw: start join... in ch:%d\n", bss_desp->channel);
	//mod_timer(&priv->WPAS_timer, jiffies + 300);
        //priv->join_req_ongoing = 1;
#if defined(CONFIG_PCI_HCI)
    start_clnt_join(priv);
#elif defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
    // avoid IQK handling race condition between start_clnt_auth and TXPowerTracking
    // so we indicate WPAS_JOIN event to cmd_thread to do start_clnt_join() 
    notify_wpas_join(priv);
#endif

	return 0;
}

int rtl_wx_get_name(struct net_device *dev, 
		    struct iw_request_info *info, 
		    union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	struct stat_info *pstat;
	struct wifi_mib *pmib;
	char *p;

	DEBUG_INFO("cmd_code=%x\n", info->cmd);

	DBFENTER;
	
	pstat = priv->pstat_cache;
	if (pstat == NULL) {
		DEBUG_INFO("pstat is NULL\n");
		return 0;
	}

	pmib = GET_MIB(priv);
	
	if ((pstat->state & (WIFI_ASOC_STATE | WIFI_AUTH_SUCCESS)) == ((WIFI_ASOC_STATE | WIFI_AUTH_SUCCESS)))
	{
		p = wrqu->name;
		p += sprintf(p, "IEEE 802.11");

		if (pmib->dot11BssType.net_work_type & WIRELESS_11A) {
			p += sprintf(p, "a");
		}

		if (pmib->dot11BssType.net_work_type & WIRELESS_11B) {
			p += sprintf(p, "b");
		}

		if (pmib->dot11BssType.net_work_type & WIRELESS_11G) {
			p += sprintf(p, "g");
		}

		if (pmib->dot11BssType.net_work_type & WIRELESS_11N) {
			p += sprintf(p, "n");
		}
		
		if (pmib->dot11BssType.net_work_type & WIRELESS_11AC) {
			p += sprintf(p, "ac");
		}
	}
	else
	{
		//prates = &padapter->registrypriv.dev_network.SupportedRates;
		//snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11g");
		snprintf(wrqu->name, IFNAMSIZ, "off/any");
	}

	DBFEXIT;

	return 0;
}
	
int rtl_siwfreq(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	unsigned int chan=0;

#ifndef WIFI_HAPD
    //check if the interface is down
    if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	if (wrqu->freq.e > 1)
		return -EINVAL;

	if((wrqu->freq.e == 0) && (wrqu->freq.m <= 1000))
		chan = wrqu->freq.m;	// Setting by channel number 
	else
#ifdef WIFI_WPAS	
		MAP_KHZ_TO_CHANNEL_ID( (wrqu->freq.m /100000) , chan); // Setting by frequency - search the table , like 2.412G, 2.422G, 
#else
		MAP_KHZ_TO_CHANNEL_ID( (wrqu->freq.m /100) , chan); // Setting by frequency - search the table , like 2.412G, 2.422G, 
#endif
    printk("cliW: wrqu->freq.m: %d chan:%d\n", wrqu->freq.m, chan);

#if 0
	priv->pmib->dot11RFEntry.dot11channel = chan;
	
#ifdef WIFI_WPAS		
	printk("cliW: wrqu->freq.m: %d chan:%d \n", wrqu->freq.m, chan);
#else
#ifdef WIFI_HAPD	
    if (!netif_running(priv->dev))
		return 0;
	else		
#endif		
	SwChnl(priv, chan, priv->pshare->offset_2nd_chan);
#endif
#endif
	
    return 0;
}

int rtl_giwfreq(struct net_device *dev,
		   struct iw_request_info *info,
		   union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	unsigned int ch;
	//unsigned long	m = 2412000;

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifndef WIFI_HAPD
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	ch = priv->pmib->dot11RFEntry.dot11channel;
/*
	MAP_CHANNEL_ID_TO_KHZ(ch, m);
	wrqu->freq.m = m * 100;
	wrqu->freq.e = 1;
	wrqu->freq.i = 0;
*/
	wrqu->freq.m = ch;
	wrqu->freq.e = 0;
	wrqu->freq.i = 0;
	
	return 0;
}

int rtl_wx_set_mode(struct net_device *dev, struct iw_request_info *info,
			        union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}
	
	switch(wrqu->mode)
	{
		case IW_MODE_AUTO:
		    if (OPMODE & WIFI_STATION_STATE) {
				OPMODE = WIFI_STATION_STATE;
				printk("set_mode = IW_MODE_AUTO ==> WIFI_STATION_STATE\n");
		 	} else {
				 OPMODE = WIFI_AP_STATE;
				 printk("set_mode = IW_MODE_AUTO ==> WIFI_AP_STATE\n");
		 	}
			break;				
		case IW_MODE_ADHOC:		
			OPMODE = WIFI_ADHOC_STATE;
			printk("set_mode = IW_MODE_ADHOC\n");			
			break;
		case IW_MODE_MASTER:		
			OPMODE = WIFI_AP_STATE;
			printk("set_mode = IW_MODE_MASTER\n");
			break;				
		case IW_MODE_INFRA:
			OPMODE = WIFI_STATION_STATE;
			printk("set_mode = IW_MODE_INFRA\n");
			break;
	
		default :
			ret = -EINVAL;;
			DEBUG_WARN("Mode: %d is not supported\n", wrqu->mode);
			break;
	}

	// TODO: Active the Operation Mode	
	return ret;	
}

int rtl_siwmode(struct net_device *dev,
			   struct iw_request_info *a,
			   union iwreq_data *wrqu, char *b)
{
	int ret = 0;
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifndef WIFI_HAPD
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif
	
	switch(wrqu->mode)
	{
		case IW_MODE_AUTO: // 0
			if (OPMODE & WIFI_STATION_STATE) {
				OPMODE = WIFI_STATION_STATE;
				printk("set_mode = IW_MODE_AUTO ==> WIFI_STATION_STATE\n");
		 	} else {
				OPMODE = WIFI_AP_STATE;
				printk("set_mode = IW_MODE_AUTO ==> WIFI_AP_STATE\n");
		 	}
			break;	
		case IW_MODE_ADHOC: // 1
			OPMODE_VAL(OPMODE & WIFI_ADHOC_STATE);
			printk("set_mode = IW_MODE_ADHOC\n");
			break;
		case IW_MODE_MASTER: // 3
			OPMODE_VAL(OPMODE & WIFI_AP_STATE);
			printk("set_mode = IW_MODE_MASTER\n");
//			setopmode_cmd(padapter, networkType);
			break;
		case IW_MODE_INFRA: // 2
			OPMODE_VAL(OPMODE & WIFI_STATION_STATE);
			printk("set_mode = IW_MODE_INFRA\n");
			break;

		default :
			ret = -EINVAL;
	}

	return ret;
	
}

int rtl_giwmode(struct net_device *dev,
		   struct iw_request_info *info,
		   union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifndef WIFI_HAPD
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	if (OPMODE & WIFI_AP_STATE)
		wrqu->mode = IW_MODE_MASTER;
	else if (OPMODE & WIFI_STATION_STATE)
		wrqu->mode = IW_MODE_INFRA;
	else if (OPMODE & WIFI_ADHOC_STATE)
		wrqu->mode = IW_MODE_ADHOC;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,4,20))
	else if (OPMODE & WIFI_SITE_MONITOR)
		wrqu->mode = IW_MODE_MONITOR;
#endif
    else
        wrqu->mode = IW_MODE_AUTO;

	return 0;
}

int rtl_wx_get_sens(struct net_device *dev, 
			     struct iw_request_info *info, 
			     union iwreq_data *wrqu, char *extra)
{
	wrqu->sens.value = 0;
	wrqu->sens.fixed = 0;	/* no auto select */
	wrqu->sens.disabled = 1;
	return 0;
}

#define MAX_FRAG_THRESHOLD 2346
#define MIN_FRAG_THRESHOLD 256

#define MAX_RTS_THRESHOLD 2347

#ifdef WLAN_PLATFORM_HUAWEI_COMMON
enum ANDROID_WIFI_CMD {
	ANDROID_WIFI_CMD_START,				
	ANDROID_WIFI_CMD_STOP,			
	ANDROID_WIFI_CMD_SCAN_ACTIVE,
	ANDROID_WIFI_CMD_SCAN_PASSIVE,		
	ANDROID_WIFI_CMD_RSSI,	
	ANDROID_WIFI_CMD_LINKSPEED,
	ANDROID_WIFI_CMD_RXFILTER_START,
	ANDROID_WIFI_CMD_RXFILTER_STOP,	
	ANDROID_WIFI_CMD_RXFILTER_ADD,	
	ANDROID_WIFI_CMD_RXFILTER_REMOVE,
	ANDROID_WIFI_CMD_BTCOEXSCAN_START,
	ANDROID_WIFI_CMD_BTCOEXSCAN_STOP,
	ANDROID_WIFI_CMD_BTCOEXMODE,
	ANDROID_WIFI_CMD_SETSUSPENDOPT,
	ANDROID_WIFI_CMD_P2P_DEV_ADDR,	
	ANDROID_WIFI_CMD_SETFWPATH,		
	ANDROID_WIFI_CMD_SETBAND,		
	ANDROID_WIFI_CMD_GETBAND,			
	ANDROID_WIFI_CMD_COUNTRY,			
	ANDROID_WIFI_CMD_P2P_SET_NOA,
	ANDROID_WIFI_CMD_P2P_GET_NOA,	
	ANDROID_WIFI_CMD_P2P_SET_PS,	
	ANDROID_WIFI_CMD_SET_AP_WPS_P2P_IE,
#ifdef PNO_SUPPORT
	ANDROID_WIFI_CMD_PNOSSIDCLR_SET,
	ANDROID_WIFI_CMD_PNOSETUP_SET,
	ANDROID_WIFI_CMD_PNOENABLE_SET,
	ANDROID_WIFI_CMD_PNODEBUG_SET,
#endif

	ANDROID_WIFI_CMD_MACADDR,

	ANDROID_WIFI_CMD_BLOCK,

	ANDROID_WIFI_CMD_WFD_ENABLE,
	ANDROID_WIFI_CMD_WFD_DISABLE,
	
	ANDROID_WIFI_CMD_WFD_SET_TCPPORT,
	ANDROID_WIFI_CMD_WFD_SET_MAX_TPUT,
	ANDROID_WIFI_CMD_WFD_SET_DEVTYPE,
 
    ANDROID_WIFI_CMD_SET_KEEP_ALIVE,

    PRIV_WIFI_CMD_CSCAN_S,

	ANDROID_WIFI_CMD_MAX
};

const char *android_wifi_cmd_str[ANDROID_WIFI_CMD_MAX] = {
	"START",
	"STOP",
	"SCAN-ACTIVE",
	"SCAN-PASSIVE",
	"RSSI",
	"LINKSPEED",
	"RXFILTER-START",
	"RXFILTER-STOP",
	"RXFILTER-ADD",
	"RXFILTER-REMOVE",
	"BTCOEXSCAN-START",
	"BTCOEXSCAN-STOP",
	"BTCOEXMODE",
	"SETSUSPENDOPT",
	"P2P_DEV_ADDR",
	"SETFWPATH",
	"SETBAND",
	"GETBAND",
	"COUNTRY",
	"P2P_SET_NOA",
	"P2P_GET_NOA",
	"P2P_SET_PS",
	"SET_AP_WPS_P2P_IE",
#ifdef PNO_SUPPORT
	"PNOSSIDCLR",
	"PNOSETUP ",
	"PNOFORCE",
	"PNODEBUG",
#endif
	"MACADDR",
	"BLOCK",
	"WFD-ENABLE",
	"WFD-DISABLE",
	"WFD-SET-TCPPORT",
	"WFD-SET-MAXTPUT",
	"WFD-SET-DEVTYPE",	
    "KEEP_ALIVE",
    "CSCAN S"
};

int rtw_android_cmdstr_to_num(char *cmdstr)
{
	int cmd_num;
        int i;

        if ( cmdstr == NULL ) {
             printk("cliw: no comp str\n");
             return 0;
        }

        // printk("cliW: ANDROID_WIFI_CMD=%s\n", cmdstr);

	for(cmd_num=0 ; cmd_num < ANDROID_WIFI_CMD_MAX; cmd_num++) {
	    if(0 == strnicmp(cmdstr , android_wifi_cmd_str[cmd_num], strlen(android_wifi_cmd_str[cmd_num])) )
	        break;
 	}	
	return cmd_num;
}

int rtl_sipriv(struct net_device *dev,
               struct iw_request_info *info,
               union iwreq_data *awrq,
               char *extra)
{
#ifdef NETDEV_NO_PRIV
        RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
        RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

        if (priv == NULL )
            return -ENETDOWN;

        printk("cliW: rtl_sipriv\n"); 
}

#define WEXT_CSCAN_HEADER_SIZE         10
#define WEXT_CSCAN_HEADER             "CSCAN S\x01\x00\x00"

#define WEXT_CSCAN_SSID_SECTION       'S'
// format : len - 1B , ssid
#define WEXT_CSCAN_CHANNEL_SECTION    'C'
// ch: 1B
#define WEXT_CSCAN_NPROBE_SECTION      'N'
// 1B
#define WEXT_CSCAN_ACTV_DWELL_SECTION  'A'
// 2B
#define WEXT_CSCAN_PASV_DWELL_SECTION  'P'
// 2B
#define WEXT_CSCAN_HOME_DWELL_SECTION  'H'
// 2B


struct combo_scan ssid_scan_list[CSCAN_MAXNUM] = {0};
void parse_combo_ssid_info(char *ext)
{
        int ssid_cnt = 0;
        int i;
        unsigned char ssid_len;
        char *bp = ext;

        printk("cliQ: start parse combo scan\n");

        memset(ssid_scan_list, 0, sizeof(ssid_scan_list));
 
        bp = ext + WEXT_CSCAN_HEADER_SIZE;

        while( bp != NULL ) {
             // printk("cliW: %x %x %x %x\n", *bp, *(bp+1), *(bp+2), 'S');
             if ( *bp == WEXT_CSCAN_SSID_SECTION ) 
             {
                 if ( bp[1] == 0 ) 
                 {
                      printk("cliW: Received broadcast scan command from supplicant. \n");
                      bp += 2;
                      continue;
                 }

                 ssid_len = *(bp + 1);

                 if (ssid_cnt >= CSCAN_MAXNUM)
                 {
                      printk("cliQ: ssid list %d more than max, skip", ssid_cnt);
                      return;
                 }
                 
                 memcpy(ssid_scan_list[ssid_cnt].ssid, (bp + 2) , ssid_len);
                 ssid_scan_list[ssid_cnt].ssid[ssid_len] = '\0';
                 ssid_scan_list[ssid_cnt].ssid_len = ssid_len;
                 ssid_scan_list[ssid_cnt].used = 1;

                 printk("cliQ: ssid_len = %d, ssid = %s\n", ssid_len, ssid_scan_list[ssid_cnt].ssid);
                 ssid_cnt++;
                 bp += ssid_len + 2;

             } else if ( *bp == WEXT_CSCAN_CHANNEL_SECTION || *bp == WEXT_CSCAN_NPROBE_SECTION )  { 
                 i = *(bp + 1);

                 if ( *bp == WEXT_CSCAN_CHANNEL_SECTION )
                     printk("cliQ: scan ch:%d\n", i);

                 if ( *bp == WEXT_CSCAN_NPROBE_SECTION )
                     printk("cliQ: scan np probe:%d\n", i); 

                 bp += 2;
             } else {
                 i = *(bp + 1);
     
                 if ( *bp == WEXT_CSCAN_ACTV_DWELL_SECTION ) {
                     printk("cliQ: scan active dwell time:%d\n", i);
                 } else if ( *bp == WEXT_CSCAN_PASV_DWELL_SECTION ) {
                     printk("cliQ: scan passive dwell time:%d\n", i);
                 } else if ( *bp == WEXT_CSCAN_HOME_DWELL_SECTION ) {
                     printk("cliQ: scan home dwell time:%d\n", i);
                     printk("cliQ: parse done\n");
                     return;
                 } else { 
                     printk("cliQ: parse parm error \n");
                     return;
                 } 

                 bp += 3;
            }
      }
}

int rtl_gipriv(struct net_device *dev, 
    struct iw_request_info *info,
    union iwreq_data *awrq,
    char *extra)
{
#ifdef NETDEV_NO_PRIV
    RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
    RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
    struct stat_info *pstat;
    int ret = 0, len = 0;
    int i;
    char *ext;
    struct iw_point *dwrq = (struct iw_point*)awrq;

    if (priv == NULL )
        return -ENETDOWN;

#if !(defined(WIFI_HAPD) || defined (WIFI_WPAS))
        if (!netif_running(priv->dev))
            return -ENETDOWN;
#endif
    struct priv_shared_info *pshare;

    pshare = priv->pshare;
    if ((TRUE == pshare->bDriverStopped) || (TRUE == pshare->bSurpriseRemoved))
        return -ENETDOWN;

    ASSERT_PRIV_RUNNING(priv);

    SDIO_AP_WAKEUP(priv);
    len = dwrq->length;
    ext = dwrq->pointer;
    ext[len] = '\0';
    
    i = rtw_android_cmdstr_to_num(ext);
        
    printk("acli: gipriv cmd id = %d\n", i);

    switch(i)
    {
    case ANDROID_WIFI_CMD_SET_KEEP_ALIVE :
        if ( (OPMODE & (WIFI_STATION_STATE | WIFI_ASOC_STATE)) == (WIFI_STATION_STATE | WIFI_ASOC_STATE) )
        {
            char *pch, *ptmp, *token, *tmp[3]={0x00,0x00,0x00};
            u32 prd;

            pch = ext;
            i = 0;

            printk("get cmd SET_KEEP_ALIVE \n");
            while ((token = strsep(&pch, " ")) != NULL)
            {
                if (i > 2) break;
                tmp[i] = token;
                i++;
            }

            if ( i >=2 ) 
            { 
                // printk("s1:%s s2:%s s3:%s\n", tmp[0], tmp[1], tmp[2]);

                prd = simple_strtoul(tmp[1], &ptmp, 10);
                // printk("prd:%d\n", prd);

                if ( prd >= 1000 )
                {
                    priv->keep_alive_time = (prd + (1000 / 2))/ 1000;/*�������뻻�㵥λ����*/
                }
                else
                {
                    priv->keep_alive_time = 1;/*����1��ʱ����1�����*/
                }
                priv->keep_alive_cnt = priv->keep_alive_time;/*���³�ʼ��������*/
                printk("prd = %d to keep_alive_time = %d\n", prd, priv->keep_alive_time);
            }                    
                  sprintf(ext, "OK");
              }   
              break;

    case ANDROID_WIFI_CMD_RSSI :
        if ( (OPMODE & (WIFI_STATION_STATE | WIFI_ASOC_STATE)) == (WIFI_STATION_STATE | WIFI_ASOC_STATE) )
        {
            pstat = get_stainfo(priv, BSSID);
            sprintf(ext, "%s rssi %d", SSID, (pstat->rssi - 100) );
        #if 0
              printk("cliW: %s\n", ext);
              printk("cliW: dbm: %d\n", translate_percentage_to_dbm(pstat->rssi));
              printk("cliW: sg  %d\n", pstat->sq);
        #endif
        }
        else
        {
            sprintf(ext, "OK");
            // printk("cliW: %s\n", ext);
        }
        break;

    case PRIV_WIFI_CMD_CSCAN_S:
        parse_combo_ssid_info(ext);
        if (IS_VXD_INTERFACE(priv))
        {
            priv->ss_req_ongoing = 1;
            use_ap_scan(priv);
        }
        else
        {
            priv->ss_req_ongoing = 1;
            start_clnt_ss(priv);
        }
        sprintf(ext, "OK");
        break;

    default :
        printk("cmd not found: %s\n", ext);
        sprintf(ext, "OK");
    }

    out:
    return 0;
}
#endif

int rtl_giwrange(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	struct iw_range *range = (struct iw_range *)extra;
	int i;
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	unsigned int Rate;
	
	if (priv == NULL || range == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifndef WIFI_HAPD
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	wrqu->data.length = sizeof(*range);
	memset(range, 0, sizeof(*range));

	/* Let's try to keep this struct in the same order as in
	 * linux/include/wireless.h
	 */

	/* TODO: See what values we can set, and remove the ones we can't
	 * set, or fill them with some default data.
	 */

#if 0
def WIFI_WPAS
	Rate = find_rate(priv, NULL, 1, 0);
	if (is_MCS_rate(Rate))
		range->throughput = (VHTMcsToDataRate(priv, Rate)*1000*1000) >> 1;	//in bps
	else
		range->throughput = (dot11_rate_table[Rate])*1000*1000 >> 1;	
#else	 
	/* ~5 Mb/s real (802.11b) */
	range->throughput = 5 * 1000 * 1000;
#endif

	// TODO: Not used in 802.11b?
//	range->min_nwid;	/* Minimal NWID we are able to set */
	// TODO: Not used in 802.11b?
//	range->max_nwid;	/* Maximal NWID we are able to set */

	/* Old Frequency (backward compat - moved lower ) */
//	range->old_num_channels;
//	range->old_num_frequency;
//	range->old_freq[6]; /* Filler to keep "version" at the same offset */

	/* signal level threshold range */


	//percent values between 0 and 100.
	range->max_qual.qual = 100;
	//range->max_qual.level = 100;
	//range->max_qual.noise = 100;
	range->max_qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_INVALID | IW_QUAL_NOISE_INVALID; /* Updated only qual b'coz not sure */


	range->avg_qual.qual = 60; 
	/* TODO: Find real 'good' to 'bad' threshol value for RSSI */
	//range->avg_qual.level = 20;
	//range->avg_qual.noise = 0;
	range->avg_qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_INVALID | IW_QUAL_NOISE_INVALID; /* Updated only qual b'coz not sure */

	range->num_bitrates = AP_BSSRATE_LEN;
	
	for(i=0; i<AP_BSSRATE_LEN && i < IW_MAX_BITRATES; i++)
	{
		if (AP_BSSRATE[i] == 0x00)
			break;
		range->bitrate[i] = (AP_BSSRATE[i]&0x7f)* 500000;
	}

	range->min_frag = MIN_FRAG_THRESHOLD;
	range->max_frag = MAX_FRAG_THRESHOLD;

	range->pm_capa = 0;

	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = 12;

//	range->retry_capa;	/* What retry options are supported */
//	range->retry_flags;	/* How to decode max/min retry limit */
//	range->r_time_flags;	/* How to decode max/min retry life */
//	range->min_retry;	/* Minimal number of retries */
//	range->max_retry;	/* Maximal number of retries */
//	range->min_r_time;	/* Minimal retry lifetime */
//	range->max_r_time;	/* Maximal retry lifetime */

	range->encoding_size[0]=5;
	range->encoding_size[1]=13;
	range->num_encoding_sizes = 2;
	range->max_encoding_tokens = 4;
	range->num_channels = priv->available_chnl_num;
	range->num_frequency = priv->available_chnl_num;
	
	for (i = 0; i < priv->available_chnl_num && i < IW_MAX_FREQUENCIES; i++) {
		u32 m = 0;
		range->freq[i].i = i + 1;
		//range->freq[val].m = CH_HZ_ID_MAP[i].freqKHz * 100000;
		MAP_CHANNEL_ID_TO_KHZ(priv->available_chnl[i], m);
		range->freq[i].m = m* 100;
		range->freq[i].e = 1;
	}

// Commented by Albert 2009/10/13
// The following code will proivde the security capability to network manager.
// If the driver doesn't provide this capability to network manager,
// the WPA/WPA2 routers can't be choosen in the network manager.

/*
#define IW_SCAN_CAPA_NONE		0x00
#define IW_SCAN_CAPA_ESSID		0x01
#define IW_SCAN_CAPA_BSSID		0x02
#define IW_SCAN_CAPA_CHANNEL	0x04
#define IW_SCAN_CAPA_MODE		0x08
#define IW_SCAN_CAPA_RATE		0x10
#define IW_SCAN_CAPA_TYPE		0x20
#define IW_SCAN_CAPA_TIME		0x40
*/

#if WIRELESS_EXT > 17
	range->enc_capa = IW_ENC_CAPA_WPA|IW_ENC_CAPA_WPA2|
				IW_ENC_CAPA_CIPHER_TKIP|IW_ENC_CAPA_CIPHER_CCMP;
#endif

#ifdef IW_SCAN_CAPA_ESSID
	range->scan_capa = IW_SCAN_CAPA_ESSID | IW_SCAN_CAPA_TYPE |IW_SCAN_CAPA_BSSID|
					IW_SCAN_CAPA_CHANNEL|IW_SCAN_CAPA_MODE|IW_SCAN_CAPA_RATE;
#endif
	return 0;
}

#ifdef WIFI_WPAS
extern void clean_for_join(struct rtl8192cd_priv *priv);
#endif

#ifdef CONFIG_SDIO_HCI
int rtl_siwap(struct net_device *dev,
		      struct iw_request_info *info,
		      struct sockaddr *awrq, char *extra)
#else
int rtl_siwap(struct net_device *dev,
		      struct iw_request_info *info,
		      union iwreq_data *wrqu, char *extra)
#endif		      
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	int i;
	unsigned char found;
	int ret;
    struct priv_shared_info *pshare;

    printk("cliW: rtl_siwap\n");

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

    pshare = priv->pshare;
    if ((TRUE == pshare->bDriverStopped) || (TRUE == pshare->bSurpriseRemoved))
		return -ENETDOWN;

#ifndef WIFI_HAPD
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif
	
#ifdef CONFIG_SDIO_HCI	    
        awrq->sa_family = ARPHRD_ETHER;
	if (priv->pmib->dot11OperationEntry.opmode & WIFI_AP_STATE) {
	    memset(&priv->pmib->dot11OperationEntry.hwaddr, 0, WLAN_ADDR_LEN);
	    memcpy(&priv->pmib->dot11OperationEntry.hwaddr, awrq->sa_data, MACADDRLEN);
            clean_for_join(priv);
	    return 0;
	} 
#if defined (WIFI_WPAS)//_Eric ??
	else if ( (priv->pmib->dot11OperationEntry.opmode & WIFI_STATION_STATE) ||
		 (priv->pmib->dot11OperationEntry.opmode & WIFI_ADHOC_STATE) )
	{
		unsigned char null_mac[] = {0,0,0,0,0,0};
		struct bss_desc bss_desp;

        printk("bssid: %x %x %x %x %x %x\n",awrq->sa_data[0], awrq->sa_data[1], 
		     awrq->sa_data[2], awrq->sa_data[3], awrq->sa_data[4], awrq->sa_data[5]);

		memcpy(priv->pmib->dot11Bss.bssid, awrq->sa_data, MACADDRLEN);

		if ( memcmp(awrq->sa_data, null_mac, MACADDRLEN) )  {					    			
		    for(i = 0 ; i < priv->site_survey->count_backup ; i++)
		    {	
			    if(!memcmp(priv->site_survey->bss_backup[i].bssid , awrq->sa_data, MACADDRLEN))
			    {
				    memcpy((void *)&bss_desp,
				       (void *)&priv->site_survey->bss_backup[i], sizeof(struct bss_desc));
				    found = 1;
				    break;
			    }
		    }
		
	            if(found == 0)  {	
		        printk("BSSID NOT Found !!\n");
		    } else {
		        ret = rtl_wx_join(priv, &bss_desp);	      	      
		        // ret = rtl_wx_join(priv, i);
		    }
    
		    if(ret != 0)
		         printk("cliW: rtl_wpas_join Failed: err:%d !!\n", ret);
	        }    
		return 0;
	}
#endif
	else
		return -EOPNOTSUPP;
#else
	if (priv->pmib->dot11OperationEntry.opmode & WIFI_AP_STATE) {
	    memset(&priv->pmib->dot11OperationEntry.hwaddr, 0, WLAN_ADDR_LEN);
		memcpy(&priv->pmib->dot11OperationEntry.hwaddr, wrqu->ap_addr.sa_data, MACADDRLEN);
		return 0;
	} else {
#ifdef WIFI_WPAS //_Eric ??
		if ( (priv->pmib->dot11OperationEntry.opmode & WIFI_STATION_STATE) ||
			 (priv->pmib->dot11OperationEntry.opmode & WIFI_ADHOC_STATE) )
			{
				memset(&priv->pmib->dot11Bss.bssid, 0, WLAN_ADDR_LEN);
				memcpy(&priv->pmib->dot11Bss.bssid, wrqu->ap_addr.sa_data, MACADDRLEN);
				return 0;
			}
		else
			return -EOPNOTSUPP;
#else
		//memset(&priv->pmib->dot11Bss.bssid, 0, WLAN_ADDR_LEN);
		//memcpy(&priv->pmib->dot11Bss.bssid, ap_addr->sa_data, MACADDRLEN);
		return -EOPNOTSUPP;
#endif
	}
#endif	
}

#ifdef CONFIG_SDIO_HCI
int rtl_giwap(struct net_device *dev,
		      struct iw_request_info *info,
		      struct sockaddr *awrq, char *extra)
#else
int rtl_giwap(struct net_device *dev,
		      struct iw_request_info *info,
		      union iwreq_data *wrqu, char *extra)
#endif		      
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifdef CONFIG_SDIO_HCI
#if !(defined (WIFI_HAPD) || defined (WIFI_WPAS))
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	awrq->sa_family = ARPHRD_ETHER;

	if (priv->pmib->dot11OperationEntry.opmode & WIFI_AP_STATE)
		memcpy(awrq->sa_data, priv->pmib->dot11OperationEntry.hwaddr, MACADDRLEN);
	else
		memcpy(awrq->sa_data, priv->pmib->dot11Bss.bssid, MACADDRLEN);
	
	return 0;
#else

#ifndef WIFI_HAPD
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	wrqu->ap_addr.sa_family = ARPHRD_ETHER;
	if (priv->pmib->dot11OperationEntry.opmode & WIFI_AP_STATE)
		memcpy(wrqu->ap_addr.sa_data, &priv->pmib->dot11OperationEntry.hwaddr, MACADDRLEN);
	else
		memcpy(wrqu->ap_addr.sa_data, &priv->pmib->dot11Bss.bssid, MACADDRLEN);
	
	return 0;
#endif	
}

#ifdef WIFI_WPAS
void clean_conn_var( RTL_PRIV *priv )
{
  	if (timer_pending(&priv->reauth_timer))
		del_timer_sync (&priv->reauth_timer);

	if (timer_pending(&priv->reassoc_timer))
		del_timer_sync (&priv->reassoc_timer);
#if 0
    priv->reauth_count = 0;
	priv->reassoc_count = 0;
	priv->auth_seq = 0;

    priv->join_res = STATE_Sta_No_Bss;
	priv->reauth_count = 0;
	priv->reassoc_count = 0;
#endif
    printk("cliW: clean_conn_var: set ss_req_ongoing=0\n");
    priv->join_req_ongoing = 0;
    priv->ss_req_ongoing = 0;

    clean_for_join(priv);        
}

#if WIRELESS_EXT >= 18
int rtl_wx_set_mlme(
	struct net_device *dev,
	struct iw_request_info *info, 
	struct iw_point *erq, 
	char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	struct iw_mlme *wextmlme = (struct iw_mlme *)extra;
	struct stat_info *pstat;

	switch(wextmlme->cmd) {
	case IW_MLME_DEAUTH:
		DEBUG_INFO("Set MLME: IW_MLME_DEAUTH, Reason=%d\n", wextmlme->reason_code);

        printk("Set MLME: IW_MLME_DEAUTH, Reason=%d\n", wextmlme->reason_code);
		if (OPMODE & (WIFI_AUTH_SUCCESS))
		{
			issue_deauth(priv,BSSID,wextmlme->reason_code);
			delay_ms(50); //Give some time to wait TX done
			OPMODE &= ~(WIFI_AUTH_SUCCESS|WIFI_ASOC_STATE) ;

			pstat = get_stainfo(priv, BSSID);
			if (pstat != NULL)
			{
				if (asoc_list_del(priv, pstat))
				{
					if (pstat->expire_to > 0)
					{
						cnt_assoc_num(priv, pstat, DECREASE, (char *)__FUNCTION__);
						check_sta_characteristic(priv, pstat, DECREASE);
					}
				}
				
				free_stainfo(priv, pstat);
			}

			event_indicate_wpas(priv, NULL, WPAS_DISCON, NULL);
		}
        clean_conn_var(priv);
		break;

	case IW_MLME_DISASSOC:
		DEBUG_INFO("Set MLME: IW_MLME_DISASSOC, Reason=%d\n", wextmlme->reason_code);
                printk("Set MLME: IW_MLME_DISASSOC, Reason=%d\n", wextmlme->reason_code);

		if((OPMODE & (WIFI_STATION_STATE | WIFI_ASOC_STATE)) == (WIFI_STATION_STATE | WIFI_ASOC_STATE))
		{
			pstat = get_stainfo(priv, BSSID);
			issue_disassoc(priv, BSSID, wextmlme->reason_code);

			if (pstat != NULL)
			{
				if (pstat->expire_to > 0)
				{
					cnt_assoc_num(priv, pstat, DECREASE, (char *)__FUNCTION__);
					check_sta_characteristic(priv, pstat, DECREASE);
				}
			
				free_stainfo(priv, pstat);

/*				memset(&priv->pmib->dot11Bss, 0, sizeof(struct bss_desc));
				memset(priv->pmib->dot11StationConfigEntry.dot11Bssid, 0, MACADDRLEN);
				memset(priv->pmib->dot11StationConfigEntry.dot11DesiredSSID, 0, sizeof(priv->pmib->dot11StationConfigEntry.dot11DesiredSSID));
				priv->pmib->dot11StationConfigEntry.dot11DesiredSSIDLen = 0;
				memset(priv->pmib->dot11StationConfigEntry.dot11DefaultSSID, 0, sizeof(priv->pmib->dot11StationConfigEntry.dot11DesiredSSID));
				priv->pmib->dot11StationConfigEntry.dot11DefaultSSIDLen = 0;
				memset(priv->pmib->dot11StationConfigEntry.dot11SSIDtoScan, 0, sizeof(priv->pmib->dot11StationConfigEntry.dot11SSIDtoScan));
				priv->pmib->dot11StationConfigEntry.dot11SSIDtoScanLen = 0;
				memset(priv->pmib->dot11StationConfigEntry.dot11DesiredBssid, 0, 6);
		
				priv->wpas_manual_assoc = 1; //_Eric ??  when to let driver auto-connect ??
*/		
				priv->join_res = STATE_Sta_No_Bss;
				pstat->state &= (~WIFI_ASOC_STATE);
			}
		}
		OPMODE &= ~(WIFI_AUTH_SUCCESS | WIFI_ASOC_STATE);
		event_indicate_wpas(priv, NULL, WPAS_DISCON, NULL);
                clean_conn_var(priv);
		break;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20))
	case IW_MLME_AUTH:
	case IW_MLME_ASSOC:
		break;
#endif
		
	default:
		return -EINVAL;
	}

	return 0;
}
#endif

int rtl_iwaplist(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *data, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	struct sockaddr addr[IW_MAX_AP];
	struct iw_quality qual[IW_MAX_AP];
	int i;
	struct list_head *phead, *plist;
	struct stat_info *pstat;
#ifdef SMP_SYNC
	unsigned long flags = 0;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifndef WIFI_HAPD
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif
	
	i = 0;
	phead = &priv->asoc_list;

	SMP_LOCK_ASOC_LIST(flags);
	
	plist = phead->next;
	while (plist != phead && i < IW_MAX_AP) {
		pstat = list_entry(plist, struct stat_info, asoc_list);  
		addr[i].sa_family = ARPHRD_ETHER;
		memcpy(addr[i].sa_data, &pstat->hwaddr, WLAN_ADDR_LEN);

		qual[i].qual = pstat->rssi;
		//qual[i].level = pstat->sq;
		//qual[i].noise = 0
		qual[i].updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_INVALID | IW_QUAL_NOISE_INVALID;
		
		plist = plist->next;
		i++;
	}

	SMP_UNLOCK_ASOC_LIST(flags);

	data->length = i;
	memcpy(extra, &addr, i*sizeof(addr[0]));
	data->flags = 1;		/* signal quality present (sort of) */
	memcpy(extra + i*sizeof(addr[0]), &qual, i*sizeof(qual[i]));

	return 0;
}

int rtl_siwessid(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *essid)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	char str[100];

#ifdef WIFI_WPAS
	int ssidlen;
	char ssid[40];
	struct iw_scan_req req;
	unsigned int i;
	unsigned char found=0;
	int ret=0;
    unsigned char null_mac[] = {0,0,0,0,0,0};     
    struct priv_shared_info *pshare;
#endif		
	
	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

    pshare = priv->pshare;
    if ((TRUE == pshare->bDriverStopped) || (TRUE == pshare->bSurpriseRemoved))
        return -ENETDOWN;
        	
#if !(defined (WIFI_HAPD) || defined (WIFI_WPAS))
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

#ifdef WIFI_WPAS
	if ( ! memcmp(priv->pmib->dot11Bss.bssid, null_mac, MACADDRLEN) )
	    return 0;	

    printk("cli: essid: %x %x %x\n", essid[0], essid[1], essid[2]);

	memset(ssid, 0, sizeof(ssid));
	if (wrqu->essid.flags && wrqu->essid.length) {
		struct bss_desc bss_desp;

		ssidlen = (wrqu->essid.length < IW_ESSID_MAX_SIZE) ? wrqu->essid.length : IW_ESSID_MAX_SIZE;
		DEBUG_INFO("%s: ssid=%s len=%d\n", __FUNCTION__, essid, ssidlen);

		printk("cliW %s: ssid=%s len=%d\n", __FUNCTION__, essid, ssidlen);

#if 1
		memcpy(ssid, essid, ssidlen);
#else
		for(i = 0 ; i < priv->site_survey->count_backup ; i++)
		{	
			if(!memcmp(priv->site_survey->bss_backup[i].ssid , essid, ssidlen)) {
			    memcpy((void *)&bss_desp,
				       (void *)&priv->site_survey->bss_backup[i], sizeof(struct bss_desc));	
				found = 1;
				break;
			}
		}

		if(found) {
			// ret = rtl_wx_join(priv, i);
			ret = rtl_wx_join(priv, &bss_desp);

		} else	{
			memcpy(req.essid, essid, ssidlen);
			req.essid_len = ssidlen;
			memset(priv->pmib->dot11Bss.bssid, 0, WLAN_ADDR_LEN);
			if (OPMODE & WIFI_STATION_STATE)
				priv->pmib->dot11Bss.bsstype = WIFI_AP_STATE;
			else if (OPMODE & WIFI_ADHOC_STATE)
				priv->pmib->dot11Bss.bsstype = WIFI_ADHOC_STATE;
			ret = rtl8192cd_join(priv, &req);
		}

		if(ret != 0)
			printk("rtl_wpas_join Failed !!\n");
		
#endif		
	}
	else{
		// asked connect to "any"
		sprintf(ssid, "any");
	}
#endif

	snprintf(str, sizeof(str), "ssid=%s", ssid);
	return set_mib(priv, str);
		
}

int rtl_giwessid(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *essid)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifndef WIFI_HAPD
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif
	
	wrqu->essid.flags = 1;
	wrqu->essid.length = SSID_LEN;
	memcpy(essid, SSID, SSID_LEN);
	
	return 0;
}

int rtl_siwrate(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	UINT32 rate = wrqu->bitrate.value, fixed = wrqu->bitrate.fixed;

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#if !(defined (WIFI_HAPD) || defined (WIFI_WPAS))
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif
    
    //printk("rtl_siwrate::(rate = %d, fixed = %d)\n", rate, fixed);
    /* rate = -1 => auto rate
       rate = X, fixed = 1 => (fixed rate X)       
    */
    
    if (rate == -1)
    {
        //Auto Rate
        priv->pmib->dot11StationConfigEntry.autoRate = TRUE;
		priv->pmib->dot11StationConfigEntry.fixedTxRate = 0;
    }
    else
    {        
        if (fixed)
        {	
        	unsigned int txRate = rate / 1000000;
			int i, len;
			unsigned char *rateset, *p;

#ifdef WIFI_WPAS			
			unsigned int support_rate;
#endif
						
			rateset = AP_BSSRATE;
			len = AP_BSSRATE_LEN;

			for(i=0,p=rateset; i<len; i++,p++)
			{
				if (*p == 0x00)
					break;

				support_rate = get_rate_from_bit_value(*p);

#if 0
//def WIFI_WPAS				
				if (support_rate > 0x80)
				{
					// MCS rate, for HT/VHT
					support_rate = McsToDataRate(priv, support_rate);
				}
#endif
									
				if (support_rate == txRate) {
					priv->pmib->dot11StationConfigEntry.autoRate = FALSE;
					priv->pmib->dot11StationConfigEntry.fixedTxRate = *p;
					return 0;
				}
			}
			return -EOPNOTSUPP;
			
        }
        else
        {
            // TODO: rate = X, fixed = 0 => (rates <= X)
            return -EOPNOTSUPP;
        }
    }

    return 0;
}

int rtl_giwrate(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	unsigned int txRate;

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#if !(defined (WIFI_HAPD) || defined (WIFI_WPAS))
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	if (priv->pmib->dot11StationConfigEntry.autoRate)
		txRate = find_rate(priv, NULL, 1, 0);
	else
		txRate = get_rate_from_bit_value(priv->pmib->dot11StationConfigEntry.fixedTxRate);

	//printk ("txRate = %d\n", txRate);
	wrqu->bitrate.value = txRate * 1000000;
    wrqu->bitrate.disabled = 0;

    return 0;
}

#define MAX_RTS_THRESHOLD 2347

int rtl_siwrts(struct net_device *dev,
		       struct iw_request_info *info,
		       union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	u16 val;

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifndef WIFI_HAPD
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	if (wrqu->rts.disabled)
		val = MAX_RTS_THRESHOLD;
	else if (wrqu->rts.value < 0 || wrqu->rts.value > MAX_RTS_THRESHOLD)
		return -EINVAL;
	else if (wrqu->rts.value == 0)
	    val = MAX_RTS_THRESHOLD;
	else
		val = wrqu->rts.value;
	
	if (val != RTSTHRSLD)
		RTSTHRSLD = val;

	return 0;
}

int rtl_giwrts(struct net_device *dev,
		       struct iw_request_info *info,
		       union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#if !(defined (WIFI_HAPD) || defined (WIFI_WPAS))
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	wrqu->rts.value = RTSTHRSLD;
	wrqu->rts.disabled = (wrqu->rts.value == MAX_RTS_THRESHOLD);
	wrqu->rts.fixed = 1;

	return 0;
}

int rtl_siwfrag(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	u16 val;

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#if !(defined (WIFI_HAPD) || defined (WIFI_WPAS))
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

	if (wrqu->frag.disabled)
		val = MAX_FRAG_THRESHOLD;
	else if (wrqu->frag.value >= MIN_FRAG_THRESHOLD || wrqu->frag.value <= MAX_FRAG_THRESHOLD)
        val = __cpu_to_le16(wrqu->frag.value & ~0x1); /* even numbers only */
	else if (wrqu->frag.value == 0)
	    val = MAX_FRAG_THRESHOLD;
	else
		return -EINVAL;

	FRAGTHRSLD = val;
	
	return 0;
}

int rtl_wx_get_freq(struct net_device *dev,
				   struct iw_request_info *info,
				   		   union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	unsigned int ch;
	unsigned long khz;

	if (priv == NULL) {
		/* if 1st open fail, pAd will be free;
		 * So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	ch = priv->pmib->dot11RFEntry.dot11channel;

	MAP_CHANNEL_ID_TO_KHZ(ch, khz);
	wrqu->freq.m = khz * 100;
	wrqu->freq.e = 1;
	wrqu->freq.i = ch;

	return 0;
}

int rtl_giwfrag(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#if !(defined (WIFI_HAPD) || defined (WIFI_WPAS))
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif

		
	wrqu->frag.value = FRAGTHRSLD;
	wrqu->frag.disabled = (wrqu->frag.value == MAX_FRAG_THRESHOLD);
	wrqu->frag.fixed = 1;

	return 0;
}


int rtl_siwretry(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	if (wrqu->retry.flags & IW_RETRY_LIMIT) {
		if (wrqu->retry.flags & IW_RETRY_SHORT)
			priv->pmib->dot11OperationEntry.dot11ShortRetryLimit = wrqu->retry.value;
		else if (wrqu->retry.flags & IW_RETRY_LONG)
			priv->pmib->dot11OperationEntry.dot11LongRetryLimit = wrqu->retry.value;
		else {
			/* we are asked to set both */
			priv->pmib->dot11OperationEntry.dot11ShortRetryLimit = wrqu->retry.value;
			priv->pmib->dot11OperationEntry.dot11LongRetryLimit = wrqu->retry.value;
		}
	}
	return 0;
}


int rtl_giwretry(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	if ((wrqu->retry.flags & IW_RETRY_TYPE) == IW_RETRY_LIFETIME) 
	{
		wrqu->retry.value = 1 * 1024;	// 1024 us
	} 
	else
	{
		if ((wrqu->retry.flags & IW_RETRY_LONG)) {
			// Asked for the long retry limit 
			wrqu->retry.value = priv->pmib->dot11OperationEntry.dot11LongRetryLimit;
			wrqu->retry.flags |= (IW_RETRY_LIMIT | IW_RETRY_MAX);
		} else {
			// default. get the  short retry limit 
			wrqu->retry.value = priv->pmib->dot11OperationEntry.dot11ShortRetryLimit;
			wrqu->retry.flags |= (IW_RETRY_LIMIT | IW_RETRY_SHORT | IW_RETRY_MAX);
		}
	}
	wrqu->retry.fixed = 0;	/* no auto select */
	wrqu->retry.disabled = 0;

	return 0;
}

int rtl_siwencode(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *keybuf)
{
	u32 key, ret = 0;
	u32 keyindex_provided;
	int i;
//	NDIS_802_11_WEP	 wep;	
//	NDIS_802_11_AUTHENTICATION_MODE authmode;

	struct iw_point *erq = &(wrqu->encoding);
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	
	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}
	
	printk("rtl_siwencode: erq->flags=0x%x erq->length=%d keybuf=%02x%02x%02x%02x%02x\n", erq->flags, erq->length, 
		keybuf[0],keybuf[1],keybuf[2],keybuf[3],keybuf[4]);	

	if ((erq->flags & IW_ENCODE_MODE) == IW_ENCODE_DISABLED)
	{
		printk("rtl_siwencode: EncryptionDisabled\n");
		priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0; //open system
		priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;
		goto exit;
	}

	key = erq->flags & IW_ENCODE_INDEX;

	if (key) {
		if (key > 4)
			return -EINVAL;
		key--;
		keyindex_provided = 1;
	} 
	else
	{
		keyindex_provided = 0;
		key = priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex;
		printk("rtl_siwencode, key=%d\n", key);
	}

	//set authentication mode
	switch (erq->flags & IW_ENCODE_MODE)
	{
	case IW_ENCODE_OPEN:
		printk("rtl_siwencode: IW_ENCODE_OPEN\n");
		priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0; //open system
		priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;
		break;

	case IW_ENCODE_RESTRICTED:
		printk("rtl_siwencode: IW_ENCODE_RESTRICTED\n");
		priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 1; //shared system
		priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;
		priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = erq->length <= 5 ? _WEP_40_PRIVACY_ : _WEP_104_PRIVACY_;
		break;

	case (IW_ENCODE_OPEN | IW_ENCODE_RESTRICTED):
		printk("rtl_siwencode: IW_ENCODE_AUTO\n");
		priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 2; // Auto
		priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;
		break;
		
	case IW_ENCODE_DISABLED:
	default:
		printk("rtl_siwencode: IW_ENCODE_OPEN\n");
		priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0; //open system
		priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;
		break;
	}

	priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex = key;

	if (erq->length > 0) {
#ifdef WIFI_WPAS		
		priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = erq->length <= 5 ? _WEP_40_PRIVACY_ : _WEP_104_PRIVACY_;
#endif
		priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyLen = erq->length <= 5 ? 5 : 13;
		priv->pmib->dot11DefaultKeysTable.keylen[key] = erq->length <= 5 ? 5 : 13;
	} else {
	
		if (keyindex_provided == 1)// set key_id only, no given KeyMaterial(erq->length==0).
		{
			printk("rtl_siwencode: keyindex provided, keyid=%d, key_len=%d\n", key, erq->length);
#ifdef WIFI_WPAS
			priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex = key;
#endif
			switch (priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyLen)
			{
				case 5:
					priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _WEP_40_PRIVACY_;
					break;
				case 13:
					priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _WEP_104_PRIVACY_;
					break;
				default:
					priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _NO_PRIVACY_;
					break;
			}

			goto exit;
		}
	}

#ifdef WIFI_WPAS
	memcpy(priv->pmib->dot11DefaultKeysTable.keytype[key].skey, keybuf, erq->length);
	CamAddOneEntry(priv, priv->pmib->dot11Bss.bssid, key,
			(priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm)<<2,
			0, priv->pmib->dot11DefaultKeysTable.keytype[key].skey);
#else	
	for (i=0; i<4; i++) {
		memcpy(&priv->pmib->dot11DefaultKeysTable.keytype[i].skey[0], keybuf, erq->length);
	}
#endif

exit:

	return ret;
}


int rtl_giwencode(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *key)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	struct iw_point *erq = &(wrqu->encoding);
	int key_index;
	int key_len;

	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#if !(defined (WIFI_HAPD) || defined (WIFI_WPAS))
	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}
#endif 
	
	if ((OPMODE & (WIFI_AP_STATE|WIFI_STATION_STATE|WIFI_ADHOC_STATE)) &&
		!priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm &&
			(priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _WEP_40_PRIVACY_ ||
				priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _WEP_104_PRIVACY_)) 
	{
		priv->pmib->dot11GroupKeysTable.dot11Privacy = priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm;
		if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _WEP_40_PRIVACY_)
			key_len = 5;
		else
			key_len = 13;

		// copy wep key
		key_index = priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex;
		erq->flags = (key_index + 1) & IW_ENCODE_INDEX;
		erq->length = key_len; 
		memcpy(key, &priv->pmib->dot11DefaultKeysTable.keytype[key_index].skey[0], key_len);

		if (priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm == 1)
			erq->flags |= IW_ENCODE_RESTRICTED;		/* XXX */
		else
			erq->flags |= IW_ENCODE_OPEN;		/* XXX */
		
		erq->flags |= IW_ENCODE_ENABLED;	/* XXX */
	}
	else if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _NO_PRIVACY_) {
		erq->length = 0;
		erq->flags = IW_ENCODE_DISABLED;
	} 
	
	return 0;
}

int rtl_giwpower(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	//_adapter *padapter = netdev_priv(dev);

	wrqu->power.value = 0;
	wrqu->power.fixed = 0;	/* no auto select */
	wrqu->power.disabled = 1;

	return 0;
}

void use_ap_scan( RTL_PRIV *priv )
{
    RTL_PRIV *r_priv;
    if (GET_ROOT(priv)->pmib->dot11OperationEntry.opmode & WIFI_WAIT_FOR_CHANNEL_SELECT)
    {
        event_indicate_wpas(priv, NULL, WPAS_SCAN_DONE, NULL);
        printk("wait for auto channel select ,return \n");
        return;
    }
    if (SSFROM_SUPLICANT == GET_ROOT(priv)->ss_req_ongoing)
    {
        printk("STA is scanning ,return \n");
        return;
    }
    priv->pshare->bScanInProcess = FALSE;

    if ( IS_ROOT_INTERFACE(priv) ) {
        printk("cliW: root_req\n");
        priv->ss_req_ongoing = 1;
        start_clnt_ss(priv);	
    } else {
        printk("cliW: vxd req\n");
        r_priv = GET_ROOT(priv);
        r_priv->ss_ssidlen = 0;
        r_priv->ss_req_ongoing = 4; /* RTW_SCAN_AP_STA */
        start_clnt_ss(r_priv);	    
    }
}

int rtl_siwscan(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	struct priv_shared_info *pshare;
	struct iw_point *data = &wrqu->data;
	
	printk("%s==>Enrty\n", __FUNCTION__);	
	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}

#ifdef WIFI_WPAS		
	pshare = priv->pshare;
        if ((TRUE == pshare->bDriverStopped) || (TRUE == pshare->bSurpriseRemoved))
                return -ENETDOWN;
#endif

#if defined(UNIVERSAL_REPEATER) || defined(MBSSID)
	if (!IS_ROOT_INTERFACE(priv))
	{
		RTL_PRIV *root_priv;
		
		root_priv = GET_ROOT(priv);
		if ((root_priv->drv_state & DRV_STATE_OPEN) == 0)
		{
			printk("%s: The Root Interface Not Open Yet!! drv_state=0x%x\n", __FUNCTION__, root_priv->drv_state);
			return -ENETDOWN;
		}
	}
#endif
    printk("cliW: scan opmode: %x join_res:%x\n", OPMODE, priv->join_res);

#ifdef WIFI_WPAS		
	// TODO: If traffic is busy, we may need to skip this scanning
	if ( OPMODE &  WIFI_ASOC_STATE ) {
    #if 1
        if (IS_VXD_INTERFACE(priv)) { 
            use_ap_scan(priv);
        }
        printk("cliW: try scanning in association\n");
    #else
        goto ind_done;
    #endif
	}  else if ( OPMODE & ( WIFI_AUTH_STATE1 | WIFI_AUTH_SUCCESS) ) {
           printk("cliW: no scanning when connecting\n");
           goto ind_done;
        } else {	
	//#if WIRELESS_EXT > 17
		if (data && (data->flags & IW_SCAN_THIS_ESSID)) {
			struct iw_scan_req* req = (struct iw_scan_req*)extra;
			
			if (req->essid_len)
			{
                printk("cliW: set ssid :%s\n", req->essid);       
				priv->ss_ssidlen = req->essid_len;
				if (priv->ss_ssidlen > 32)
				    priv->ss_ssidlen = 32;
                                
                                memset(priv->ss_ssid, 0, 32);
				memcpy(priv->ss_ssid, req->essid, priv->ss_ssidlen);
				
				SSID2SCAN_LEN = (req->essid_len > 32) ? 32 : req->essid_len;
				memcpy(SSID2SCAN, req->essid, SSID2SCAN_LEN);
			
			        priv->ss_req_ongoing = 1;
			        use_ap_scan(priv);

                                //if (IS_VXD_INTERFACE(priv)) {
                               //      priv->ss_req_ongoing = 1;
                                    //priv->ss_ssidlen = 0;
                                //   use_ap_scan(priv);
                                //} else {
                                    // priv->ss_req_ongoing = 3;
                                //    priv->ss_req_ongoing = 1; 
                                //    start_clnt_ss(priv);
                                //}
			}
		}  else { 
                        printk("cliW: wildcard ssid \n");
		        priv->ss_ssidlen = 0;
		        priv->ss_req_ongoing = 1;
                        use_ap_scan(priv);
		        //start_clnt_ss(priv);
                }
        }
done:
	return 0;

ind_done:
        event_indicate_wpas(priv, NULL, WPAS_SCAN_DONE, NULL); 
	return 0;
#else          
	rtl8192cd_ss_req(priv, (unsigned char *) &wrqu->data, 0);
	//wait ss done
	wait_event_interruptible_timeout(priv->ss_wait,(!priv->ss_req_ongoing),RTL_SECONDS_TO_JIFFIES(5));
#endif	
	return 0;
}


#ifdef WIFI_WPAS //_Eric ?? AP mode (HAPD) will attemp to scan or not ??
int rtl_giwscan(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
		RTL_PRIV *priv = (RTL_PRIV *)netdev_priv(dev);
#else
		RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
		int i=0, j=0;
		char * current_ev = extra;
		char * previous_ev = extra;
		char *current_val;
		char * end_buf;
		//char * current_val;
#ifndef IWEVGENIE
		unsigned char idx;
#endif // IWEVGENIE //
		struct iw_event iwe;
		unsigned int rate;
                struct priv_shared_info *pshare;
		
		if (priv == NULL)
		{
			/* if 1st open fail, pAd will be free;
			   So the net_dev->priv will be NULL in 2rd open */
			return -ENETDOWN;
		}
	
		//check if the interface is down
		if (!netif_running(priv->dev))
		{
			printk("\nFail: interface not opened\n");
			return -ENETDOWN;
		}

                pshare = priv->pshare;
                if ((TRUE == pshare->bDriverStopped) || (TRUE == pshare->bSurpriseRemoved)) {
                   printk("cliw: driver shutdow now\n");
                   return -EAGAIN;
                }

#if 0
		if (priv->ss_req_ongoing) {
                        printk("\nFail: under scanning \n");
			return -EAGAIN;
		}
#endif	
		if (priv->site_survey->count_backup== 0)
		{
			wrqu->data.length = 0;
			return 0;
		}
		
#if WIRELESS_EXT >= 17
		if (wrqu->data.length > 0)
			end_buf = extra + wrqu->data.length;
		else
			end_buf = extra + IW_SCAN_MAX_DATA;
#else
		end_buf = extra + IW_SCAN_MAX_DATA;
#endif
		memset(extra, 0, wrqu->data.length);

//		printk("req_len=%d ssid_cnt=%d\n", wrqu->data.length, priv->site_survey->count_backup);
		for (i = 0; i < priv->site_survey->count_backup; i++) 
		{
			if ((current_ev >= end_buf) || ( (wrqu->data.length - (current_ev - extra)) <= 200))
			{
#if WIRELESS_EXT >= 17
				return -E2BIG;
#else
				break;
#endif
			}

			
			// The first entry must be the MAC address 
			//MAC address
			//================================
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = SIOCGIWAP;
			iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
			memcpy(iwe.u.ap_addr.sa_data, priv->site_survey->bss_backup[i].bssid, MACADDRLEN);
	
			previous_ev = current_ev;
			current_ev = IWE_STREAM_ADD_EVENT(info, current_ev,end_buf, &iwe, IW_EV_ADDR_LEN);
//			mem_dump("MAC_Addr:", priv->site_survey->bss_backup[i].bssid, MACADDRLEN);
			if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
				return -E2BIG;
#else
				break;
#endif

			//ESSID 
			//================================
			memset(&iwe, 0, sizeof(iwe));
	
//			printk("ESSID %s %d\n",priv->site_survey->bss_backup[i].ssid, priv->site_survey->bss_backup[i].ssidlen);
			iwe.cmd = SIOCGIWESSID;
			iwe.u.data.length = (priv->site_survey->bss_backup[i].ssidlen <= 32) ? priv->site_survey->bss_backup[i].ssidlen : 32;
			iwe.u.data.flags = 1;
	 
			previous_ev = current_ev;
			current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, (char *)priv->site_survey->bss_backup[i].ssid);
			if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
				return -E2BIG;
#else
				break;
#endif
            #define WLAN_CAPABILITY_PRIVACY (1<<4)	


			//Network Type 
			//================================
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = SIOCGIWMODE;
			if (priv->site_survey->bss_backup[i].bsstype & WIFI_ADHOC_STATE)
			{
				iwe.u.mode = IW_MODE_ADHOC;
			}
			else if (priv->site_survey->bss_backup[i].bsstype & WIFI_STATION_STATE)
			{
				iwe.u.mode = IW_MODE_INFRA;
			}
			else if (priv->site_survey->bss_backup[i].bsstype & WIFI_AP_STATE)
			{
				iwe.u.mode = IW_MODE_MASTER;
			}
			else
			{
				iwe.u.mode = IW_MODE_AUTO;
			}
			iwe.len = IW_EV_UINT_LEN;
	
			previous_ev = current_ev;
			current_ev = IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe,	IW_EV_UINT_LEN);
			if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
				return -E2BIG;
#else
				break;
#endif

			//Channel and Frequency
			//================================
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = SIOCGIWFREQ;
			{
				u8 ch = priv->site_survey->bss_backup[i].channel;
				u32	m = 0;
				
				MAP_CHANNEL_ID_TO_KHZ(ch, m);
				iwe.u.freq.m = m;
				iwe.u.freq.e = 3;
				previous_ev = current_ev;
				current_ev = IWE_STREAM_ADD_EVENT(info, current_ev,end_buf, &iwe, IW_EV_FREQ_LEN);
				if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
					return -E2BIG;
#else
					break;
#endif
			}		

			//Add quality statistics
			//================================
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = IWEVQUAL;
			// iwe.u.qual.level = translate_percentage_to_dbm(priv->site_survey->bss_backup[i].rssi) + 20;
			iwe.u.qual.level = priv->site_survey->bss_backup[i].rssi - 100;
			iwe.u.qual.qual = priv->site_survey->bss_backup[i].sq;
            iwe.u.qual.noise = 0; /* cliW */

			// not sure about signal level and noise level, To Do: determin the SNR
			//iwe.u.qual.noise = signal_todbm((u8)(100-priv->site_survey->bss[i].rssi)) -25;
//			iwe.u.qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_INVALID | IW_QUAL_NOISE_INVALID;
			iwe.u.qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_UPDATED | IW_QUAL_NOISE_INVALID | IW_QUAL_DBM ;
			
			current_ev = IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe, IW_EV_QUAL_LEN);
		if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
				return -E2BIG;
#else
				break;
#endif
	
			// Encryption capability 
			//================================
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = SIOCGIWENCODE;
			if (priv->site_survey->bss_backup[i].capability & WLAN_CAPABILITY_PRIVACY)	// if Privacy
				iwe.u.data.flags =IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
			else
				iwe.u.data.flags = IW_ENCODE_DISABLED;
	
			previous_ev = current_ev;		
			current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf,&iwe, (u8 *)&priv->pmib->dot11DefaultKeysTable.keytype[priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex].skey[0]);
			if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
				return -E2BIG;
#else
				break;
#endif
			
	
			// support rate
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = SIOCGIWRATE;
			iwe.u.bitrate.fixed = 0;
			iwe.u.bitrate.disabled = 0;
			iwe.u.bitrate.value = 0;
			current_val = current_ev + IW_EV_LCP_LEN;

			for (j=0;j<12;j++) 
			{
				if (priv->site_survey->bss[i].supportrate & (BIT(j)))
				{
					rate = dot11_rate_table[j];
					iwe.u.bitrate.value = (rate >= 0x80)? ((rate - 0x80) * 500000): (rate*500000);
					current_val = IWE_STREAM_ADD_VALUE(info, current_ev, current_val, end_buf, &iwe, IW_EV_PARAM_LEN);
				}
			}

			if (priv->site_survey->bss[i].network & WIRELESS_11N)
			{
				// TODO: Report MCS Rate
			}
			
			/* remove fixed header if no rates were added */
			if ((current_val - current_ev) > IW_EV_LCP_LEN)
				current_ev = current_val;
	
	
			/* Add WPA/RSN IE */
#if WIRELESS_EXT >= 18
			//WPAIE 
			//================================
			if(priv->site_survey->wpa_ie_backup[i].wpa_ie_len > 0){
				memset(&iwe, 0, sizeof(iwe));
		
				iwe.cmd = IWEVGENIE;
				iwe.u.data.length = priv->site_survey->wpa_ie_backup[i].wpa_ie_len;
				iwe.u.data.flags = 1;
		 
				previous_ev = current_ev;
				current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, (char *)priv->site_survey->wpa_ie_backup[i].data);
				if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
					return -E2BIG;
#else
					break;
#endif
			}
			
			//RSNIE 
			//================================
			if(priv->site_survey->rsn_ie_backup[i].rsn_ie_len > 0){
				memset(&iwe, 0, sizeof(iwe));
					
				iwe.cmd = IWEVGENIE;
				iwe.u.data.length = priv->site_survey->rsn_ie_backup[i].rsn_ie_len;
				iwe.u.data.flags = 1;
					 
				previous_ev = current_ev;
				current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, (char *)priv->site_survey->rsn_ie_backup[i].data);
				if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
					return -E2BIG;
#else
					break;
#endif
			}
#endif

			/* 
			Protocol:
				it will show scanned AP's WirelessMode .
				it might be
						802.11a
						802.11a/n
						802.11g/n
						802.11b/g/n
						802.11g
						802.11b/g
			*/
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = SIOCGIWNAME;
	
		{
			struct bss_desc *pBss=&priv->site_survey->bss_backup[i];
			//int rateCnt=0;
	
			if (pBss->network==WIRELESS_11B)
					strcpy(iwe.u.name, "802.11b");
			else if (pBss->network==WIRELESS_11G)
					strcpy(iwe.u.name, "802.11g");
			else if (pBss->network==(WIRELESS_11G|WIRELESS_11B))
					strcpy(iwe.u.name, "802.11b/g");
			else if (pBss->network==(WIRELESS_11N))
					strcpy(iwe.u.name, "802.11n");
			else if (pBss->network==(WIRELESS_11G|WIRELESS_11N))
					strcpy(iwe.u.name, "802.11g/n");
			else if (pBss->network==(WIRELESS_11G|WIRELESS_11B | WIRELESS_11N))
					strcpy(iwe.u.name, "802.11b/g/n");
			else if(pBss->network== WIRELESS_11A)
					strcpy(iwe.u.name, "802.11a");
			else
					strcpy(iwe.u.name, "---");
		}
	
			previous_ev = current_ev;
			current_ev = IWE_STREAM_ADD_EVENT(info, current_ev,end_buf, &iwe, IW_EV_ADDR_LEN);
			if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
				return -E2BIG;
#else
				break;
#endif

			//WPSIE 
			//================================
			if(priv->site_survey->wscie_backup[i].data[1] > 0) {
				memset(&iwe, 0, sizeof(iwe));

				priv->site_survey->wscie_backup[i].data[0] = VENDOR_SPECIFIC_IE;
				iwe.cmd = IWEVGENIE;
				iwe.u.data.length = priv->site_survey->wscie_backup[i].data[1] + 2;
				iwe.u.data.flags = 1;
					 
				previous_ev = current_ev;
				current_ev = IWE_STREAM_ADD_POINT(info, current_ev, 
				             end_buf, &iwe, (char *)priv->site_survey->wscie_backup[i].data);

				if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
				return -E2BIG;
#else
				break;
#endif
			}

	
			/* Add EXTRA: Age to display seconds since last beacon/probe response
			 * for given network. */
			{
				char tmp_buf[128];

				memset(tmp_buf, 0, sizeof(tmp_buf));			
				memset(&iwe, 0, sizeof(iwe));
				iwe.cmd = IWEVCUSTOM;
				iwe.u.data.length = snprintf(tmp_buf, 128,
						  				" Last beacon: %lums ago", (jiffies - priv->site_survey->bss[i].last_scan_time) / (HZ / 100));
				if (iwe.u.data.length) {
					current_ev = IWE_STREAM_ADD_POINT(
									info,
									current_ev, end_buf, &iwe, tmp_buf);
				}
			}
		}
	
		wrqu->data.length = current_ev - extra;

		return 0;
}

#else
int rtl_giwscan(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	int i=0;
	char * current_ev = extra;
	char * previous_ev = extra;
	char * end_buf;
	//char * current_val;
#ifndef IWEVGENIE
	unsigned char idx;
#endif // IWEVGENIE //
	struct iw_event iwe;
	
	if (priv == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	//check if the interface is down
	if (!netif_running(priv->dev))
	{
		//printk("\nFail: interface not opened\n");
		return -ENETDOWN;
	}

	if (priv->site_survey->count == 0)
	{
		wrqu->data.length = 0;
		return 0;
	}
	
#if WIRELESS_EXT >= 17
    if (wrqu->data.length > 0)
        end_buf = extra + wrqu->data.length;
    else
        end_buf = extra + IW_SCAN_MAX_DATA;
#else
    end_buf = extra + IW_SCAN_MAX_DATA;
#endif

	for (i = 0; i < priv->site_survey->count; i++) 
	{
		if (current_ev >= end_buf)
        {
#if WIRELESS_EXT >= 17
            return -E2BIG;
#else
			break;
#endif
        }
		
		//MAC address
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWAP;
		iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
				memcpy(iwe.u.ap_addr.sa_data, &priv->site_survey->bss[i].bssid, MACADDRLEN);

        previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_EVENT(info, current_ev,end_buf, &iwe, IW_EV_ADDR_LEN);
        if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
            return -E2BIG;
#else
			break;
#endif

		/* 
		Protocol:
			it will show scanned AP's WirelessMode .
			it might be
					802.11a
					802.11a/n
					802.11g/n
					802.11b/g/n
					802.11g
					802.11b/g
		*/
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWNAME;

	{
		struct bss_desc *pBss=&priv->site_survey->bss[i];
		//int rateCnt=0;

		if (pBss->network==WIRELESS_11B)
				strcpy(iwe.u.name, "802.11b");
		else if (pBss->network==WIRELESS_11G)
				strcpy(iwe.u.name, "802.11g");
		else if (pBss->network==(WIRELESS_11G|WIRELESS_11B))
				strcpy(iwe.u.name, "802.11b/g");
		else if (pBss->network==(WIRELESS_11N))
				strcpy(iwe.u.name, "802.11n");
		else if (pBss->network==(WIRELESS_11G|WIRELESS_11N))
				strcpy(iwe.u.name, "802.11g/n");
		else if (pBss->network==(WIRELESS_11G|WIRELESS_11B | WIRELESS_11N))
				strcpy(iwe.u.name, "802.11b/g/n");
		else if(pBss->network== WIRELESS_11A)
				strcpy(iwe.u.name, "802.11a");
		else
				strcpy(iwe.u.name, "---");
	}

		previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_EVENT(info, current_ev,end_buf, &iwe, IW_EV_ADDR_LEN);
		if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
	   		return -E2BIG;
#else
			break;
#endif

		//ESSID	
		//================================
		memset(&iwe, 0, sizeof(iwe));

		//printk("ESSID %s %d\n",priv->site_survey->bss[i].ssid, priv->site_survey->bss[i].ssidlen);
		iwe.cmd = SIOCGIWESSID;
		iwe.u.data.length = priv->site_survey->bss[i].ssidlen;
		iwe.u.data.flags = 1;
 
        previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, (char *)priv->site_survey->bss[i].ssid);
        if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
            return -E2BIG;
#else
			break;
#endif
		
		//Network Type 
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWMODE;
		if (priv->site_survey->bss[i].bsstype & WIFI_ADHOC_STATE)
		{
			iwe.u.mode = IW_MODE_ADHOC;
		}
		else if (priv->site_survey->bss[i].bsstype & WIFI_STATION_STATE)
		{
			iwe.u.mode = IW_MODE_INFRA;
		}
		else if (priv->site_survey->bss[i].bsstype & WIFI_AP_STATE)
		{
			iwe.u.mode = IW_MODE_MASTER;
		}
		else
		{
			iwe.u.mode = IW_MODE_AUTO;
		}
		iwe.len = IW_EV_UINT_LEN;

        previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe,  IW_EV_UINT_LEN);
        if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
            return -E2BIG;
#else
			break;
#endif

		//Channel and Frequency
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWFREQ;
		{
			u8 ch = priv->site_survey->bss[i].channel;
			//u32	m = 0;
			//MAP_CHANNEL_ID_TO_KHZ(ch, m);
			//iwe.u.freq.m = m * 100;
			//iwe.u.freq.e = 1;
			iwe.u.freq.m = ch;
			iwe.u.freq.e = 0;
		iwe.u.freq.i = 0;
		previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_EVENT(info, current_ev,end_buf, &iwe, IW_EV_FREQ_LEN);
        if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
            return -E2BIG;
#else
			break;
#endif
		}	    

        //Add quality statistics
        //================================
        memset(&iwe, 0, sizeof(iwe));
    	iwe.cmd = IWEVQUAL;
		iwe.u.qual.qual = priv->site_survey->bss[i].rssi;

		// not sure about signal level and noise level
		//iwe.u.qual.level = (u8) priv->site_survey->bss[i].sq;  
		//iwe.u.qual.noise = signal_todbm((u8)(100-priv->site_survey->bss[i].rssi)) -25;
		iwe.u.qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_INVALID | IW_QUAL_NOISE_INVALID;
		
    	current_ev = IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe, IW_EV_QUAL_LEN);
	if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
            return -E2BIG;
#else
			break;
#endif
#if 0
		//Encyption key
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWENCODE;
		if (priv->site_survey->bss[i].capability & 0x0010)
			iwe.u.data.flags =IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		else
			iwe.u.data.flags = IW_ENCODE_DISABLED;

        previous_ev = current_ev;		
        current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf,&iwe, (u8 *)priv->pmib->dot11DefaultKeysTable.keytype[priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex].skey[0]);
        if (current_ev == previous_ev)
#if WIRELESS_EXT >= 17
            return -E2BIG;
#else
			break;
#endif
#endif 
	}

	wrqu->data.length = current_ev - extra;
	return 0;
}
#endif

#endif


//#ifdef CONFIG_SDIO_HCI

const u1Byte WPA_OUI_TYPE[] = { 0x00, 0x50, 0xf2, 1 };
const u1Byte WPA_CIPHER_SUITE_NONE[] = { 0x00, 0x50, 0xf2, 0 };
const u1Byte WPA_CIPHER_SUITE_WEP40[] = { 0x00, 0x50, 0xf2, 1 };
const u1Byte WPA_CIPHER_SUITE_TKIP[] = { 0x00, 0x50, 0xf2, 2 };
const u1Byte WPA_CIPHER_SUITE_CCMP[] = { 0x00, 0x50, 0xf2, 4 };
const u1Byte WPA_CIPHER_SUITE_WEP104[] = { 0x00, 0x50, 0xf2, 5 };

const u1Byte RSN_CIPHER_SUITE_NONE[] = { 0x00, 0x0f, 0xac, 0 };
const u1Byte RSN_CIPHER_SUITE_WEP40[] = { 0x00, 0x0f, 0xac, 1 };
const u1Byte RSN_CIPHER_SUITE_TKIP[] = { 0x00, 0x0f, 0xac, 2 };
//const u1Byte RSN_CIPHER_SUITE_WRAP[] = { 0x00, 0x0f, 0xac, 3 };
const u1Byte RSN_CIPHER_SUITE_CCMP[] = { 0x00, 0x0f, 0xac, 4 };
const u1Byte RSN_CIPHER_SUITE_WEP104[] = { 0x00, 0x0f, 0xac, 5 };

unsigned char rtw_get_wpa_cipher_suite(u1Byte *s)
{
	unsigned char cipher = 0;
	
	if (memcmp(s, WPA_CIPHER_SUITE_NONE, WPA_SELECTOR_LEN) == 0)
		cipher = _NO_PRIVACY_;
	else if (memcmp(s, WPA_CIPHER_SUITE_WEP40, WPA_SELECTOR_LEN) == 0)
		cipher = _WEP_40_PRIVACY_;
	else if (memcmp(s, WPA_CIPHER_SUITE_TKIP, WPA_SELECTOR_LEN) == 0)
		cipher = _TKIP_PRIVACY_;
	else if (memcmp(s, WPA_CIPHER_SUITE_CCMP, WPA_SELECTOR_LEN) == 0)
		cipher = _CCMP_PRIVACY_;
	else if (memcmp(s, WPA_CIPHER_SUITE_WEP104, WPA_SELECTOR_LEN) == 0)
		cipher = _WEP_104_PRIVACY_;

	return cipher;
}

unsigned char rtw_get_wpa2_cipher_suite(u1Byte *s)
{
	unsigned char cipher = 0;
	
	if (memcmp(s, RSN_CIPHER_SUITE_NONE, RSN_SELECTOR_LEN) == 0)
		cipher = _NO_PRIVACY_;
	if (memcmp(s, RSN_CIPHER_SUITE_WEP40, RSN_SELECTOR_LEN) == 0)
		cipher = _WEP_40_PRIVACY_;
	if (memcmp(s, RSN_CIPHER_SUITE_TKIP, RSN_SELECTOR_LEN) == 0)
		cipher = _TKIP_PRIVACY_;
	if (memcmp(s, RSN_CIPHER_SUITE_CCMP, RSN_SELECTOR_LEN) == 0)
		cipher = _CCMP_PRIVACY_;
	if (memcmp(s, RSN_CIPHER_SUITE_WEP104, RSN_SELECTOR_LEN) == 0)
		cipher = _WEP_104_PRIVACY_;

	return cipher;
}

int rtw_parse_wpa_ie(u1Byte* wpa_ie, int wpa_ie_len, unsigned char *group_cipher, unsigned char *pairwise_cipher)
{
	int i, ret=0;
	int left, count;
	pu1Byte pos;

	if (wpa_ie_len <= 0) {
		/* No WPA IE - fail silently */
		return 1;
	}
	
	if ((*wpa_ie != WPA_IE_ID) || (*(wpa_ie+1) != (u1Byte)(wpa_ie_len - 2)) ||
	   (memcmp(wpa_ie+2, WPA_OUI_TYPE, WPA_SELECTOR_LEN) != 0) )
	{		
		return 1;
	}

	pos = wpa_ie;

	pos += 8;
	left = wpa_ie_len - 8;	


	//group_cipher
	if (left >= WPA_SELECTOR_LEN) 
	{
		*group_cipher = rtw_get_wpa_cipher_suite(pos);
		
		pos += WPA_SELECTOR_LEN;
		left -= WPA_SELECTOR_LEN;
	} 
	else if (left > 0)
	{
		DEBUG_INFO("%s: ie length mismatch, %u too much", __FUNCTION__, left);
		return 1;
	}


	//pairwise_cipher
	if (left >= 2)
	{		
		count = le16_to_cpu(*(u2Byte*)pos);//			
		pos += 2;
		left -= 2;
		
		if (count == 0 || left < count * WPA_SELECTOR_LEN) {
			DEBUG_INFO("%s: ie count botch (pairwise), "
				   		"count %u left %u", __FUNCTION__, count, left);
			return 1;
		}
		
		for (i = 0; i < count; i++)
		{
			*pairwise_cipher = rtw_get_wpa_cipher_suite(pos);
			
			pos += WPA_SELECTOR_LEN;
			left -= WPA_SELECTOR_LEN;
		}
		
	} 
	else if (left == 1)
	{
		DEBUG_INFO("%s: ie too short (for key mgmt)",   __FUNCTION__);
		return 1;
	}

	
	return ret;
	
}

int rtw_parse_wpa2_ie(u1Byte* rsn_ie, int rsn_ie_len, unsigned char *group_cipher, unsigned char *pairwise_cipher)
{
	int i, ret=0;
	int left, count;
	u1Byte *pos;

	if (rsn_ie_len <= 0) 
	{
		/* No RSN IE - fail silently */
		return 1;
	}


	if ((*rsn_ie!= WPA2_IE_ID) || (*(rsn_ie+1) != (u1Byte)(rsn_ie_len - 2)))
	{		
		return 1;
	}
	
	pos = rsn_ie;
	pos += 4;
	left = rsn_ie_len - 4;	

	//group_cipher
	if (left >= RSN_SELECTOR_LEN) {

		*group_cipher = rtw_get_wpa2_cipher_suite(pos);
		
		pos += RSN_SELECTOR_LEN;
		left -= RSN_SELECTOR_LEN;
		
	} else if (left > 0) {
		DEBUG_INFO("%s: ie length mismatch, %u too much", __FUNCTION__, left);
		return 1;
	}

	//pairwise_cipher
	if (left >= 2)
	{		
		count = le16_to_cpu(*(u16*)pos);//			
		pos += 2;
		left -= 2;

		if (count == 0 || left < count * RSN_SELECTOR_LEN) {
			DEBUG_INFO("%s: ie count botch (pairwise), "
				  		 "count %u left %u", __FUNCTION__, count, left);
			return 1;
		}
		
		for (i = 0; i < count; i++)
		{			
			*pairwise_cipher = rtw_get_wpa2_cipher_suite(pos);
			
			pos += RSN_SELECTOR_LEN;
			left -= RSN_SELECTOR_LEN;
		}

	} 
	else if (left == 1)
	{
		DEBUG_INFO("%s: ie too short (for key mgmt)",  __FUNCTION__);
		
		return 1;
	}

	return ret;
	
}

void rtw_set_wpa_cipher_bit(struct rtl8192cd_priv *priv, unsigned char enc_type)
{
	switch (enc_type)
	{
	case _WEP_40_PRIVACY_:
		priv->pmib->dot1180211AuthEntry.dot11WPACipher |= BIT(0);		
		break;

	case _TKIP_PRIVACY_:
		priv->pmib->dot1180211AuthEntry.dot11WPACipher |= BIT(1);
		break;

	case _WRAP_PRIVACY_:
		priv->pmib->dot1180211AuthEntry.dot11WPACipher |= BIT(2);
		break;

	case _CCMP_PRIVACY_:
		priv->pmib->dot1180211AuthEntry.dot11WPACipher |= BIT(3);
		break;

	case _WEP_104_PRIVACY_:
		priv->pmib->dot1180211AuthEntry.dot11WPACipher |= BIT(4);
		break;

	default:
		break;
	}
}


int rtl_siwgenie(struct net_device *dev, 
			     struct iw_request_info *info, 
			     union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	u1Byte				*buf=NULL;	
	unsigned char 		group_cipher = 0, pairwise_cipher = 0;
	int					ret = 0;
	u2Byte				ielen;
	u2Byte				rsnie_len;
	WPA_GLOBAL_INFO *pGblInfo = priv->wpa_global_info;
	
	DEBUG_INFO("%s:IE_Len=%d\n", __FUNCTION__, wrqu->data.length);
	
	ielen = wrqu->data.length;
	if((ielen > MAX_WPA_IE_LEN) || (extra == NULL)){
		priv->pmib->wscEntry.wsc_enable = 0;				
		priv->pmib->wscEntry.assoc_ielen = 0;
		if(extra == NULL)	
			return ret;
		else
			return -EINVAL;
	}

	printk("rtl_siwgenie\n");

    ASSERT_PRIV_RUNNING(priv);    
    SDIO_AP_WAKEUP(priv);
	if ( ielen == 0 )  {
        priv->pmib->dot11RsnIE.rsnie_len = 0;
 	    priv->pmib->dot11RsnIE.wpaie_len = 0;
		priv->pmib->dot11RsnIE.rsnielen = 0;
	}  else	{
#if 0
		if  (wrqu->data.length < sizeof(DOT11_RSN_IE_HEADER))
		{
			DEBUG_ERR("Invalid IE Length(%d) or Null data pointer\n", wrqu->data.length);
			return -EINVAL;	
		}
#endif		
		buf = kzalloc(ielen, GFP_KERNEL);
		if (buf == NULL){
			return -ENOMEM;
		}

		memcpy(buf, extra, ielen);
		priv->pmib->dot11RsnIE.wpaie_len = 0;
		if(rtw_parse_wpa_ie(buf, ielen, &group_cipher, &pairwise_cipher) == 0)
		{
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 2;
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = pairwise_cipher;
			priv->pmib->dot1180211AuthEntry.dot11EnablePSK |= BIT(0);
			priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 1;	
			priv->pmib->dot1180211AuthEntry.dot11WPACipher |= 1 << (pairwise_cipher-1);
			priv->pmib->dot11GroupKeysTable.dot11Privacy = group_cipher;
			pGblInfo->MulticastCipher = group_cipher ;
			pGblInfo->UnicastCipher[0] = pairwise_cipher ;
		
			priv->pmib->dot11RsnIE.wpaie_len = ielen;
			memcpy(priv->pmib->dot11RsnIE.wpa_ie, &buf[0], ielen);
		}

		priv->pmib->dot11RsnIE.rsnie_len = 0;
		if(rtw_parse_wpa2_ie(buf, ielen, &group_cipher, &pairwise_cipher) == 0)
		{
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 2;
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = pairwise_cipher;
			priv->pmib->dot1180211AuthEntry.dot11EnablePSK |= BIT(1);
			priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 1;	
			priv->pmib->dot1180211AuthEntry.dot11WPA2Cipher |= 1 << (pairwise_cipher-1);
			priv->pmib->dot11GroupKeysTable.dot11Privacy = group_cipher;
			pGblInfo->MulticastCipher = group_cipher ;
			pGblInfo->UnicastCipherWPA2[0] = pairwise_cipher ;
		
			priv->pmib->dot11RsnIE.rsnie_len = ielen;
			memcpy(priv->pmib->dot11RsnIE.rsn_ie, &buf[0], ielen);
		}
		
		rsnie_len = 0;
		if (priv->pmib->dot11RsnIE.rsnie_len)
		{
			memcpy(priv->pmib->dot11RsnIE.rsnie, priv->pmib->dot11RsnIE.rsn_ie, priv->pmib->dot11RsnIE.rsnie_len);
			rsnie_len += priv->pmib->dot11RsnIE.rsnie_len;
		}
		
		if (priv->pmib->dot11RsnIE.wpaie_len)
		{
			memcpy(&(priv->pmib->dot11RsnIE.rsnie[rsnie_len]), priv->pmib->dot11RsnIE.wpa_ie, priv->pmib->dot11RsnIE.wpaie_len);
			rsnie_len += priv->pmib->dot11RsnIE.wpaie_len;
		}
		
		priv->pmib->dot11RsnIE.rsnielen = rsnie_len;
		
		{//set wps_ie	
			int cnt = 0;
			unsigned char wps_ie_found=0;
			unsigned char eid, wps_oui[4]={0x0,0x50,0xf2,0x04};
			 
			while( cnt < ielen )
			{
				eid = buf[cnt];
		
				if((eid==VENDOR_SPECIFIC_IE)&&(memcmp(&buf[cnt+2], wps_oui, 4)==0))
				{
					DEBUG_INFO("%s: SET WPS_IE\n",__FUNCTION__);
					priv->pmib->wscEntry.wsc_enable = 1;				
					priv->pmib->wscEntry.assoc_ielen = ( (buf[cnt+1]+2) < (256)) ? (buf[cnt+1]+2):(256);
					memset(priv->pmib->wscEntry.assoc_ie, 0x0, 256);
					memcpy(priv->pmib->wscEntry.assoc_ie, &buf[cnt], priv->pmib->wscEntry.assoc_ielen);
					cnt += buf[cnt+1]+2;
					wps_ie_found = 1;
					break;
				} else {
					cnt += buf[cnt+1]+2; //goto next	
				}				
			}

			if (!wps_ie_found)
			{
				priv->pmib->wscEntry.wsc_enable = 0;				
				priv->pmib->wscEntry.assoc_ielen = 0;
			}
		}

		kfree(buf);
	}   

	return ret;
}


int rtl_siwauth1(struct net_device *dev, 
					struct iw_request_info *info, 
					union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	struct iw_param *param = (struct iw_param*)&(wrqu->param);
	int value = param->value;
	int ret = 0;

        printk("rtl_siwauth : cmd:%d val:%d\n", (param->flags & IW_AUTH_INDEX),  param->value);

	switch (param->flags & IW_AUTH_INDEX) {

	case IW_AUTH_WPA_VERSION:
#ifdef CONFIG_RTL_WAPI_SUPPORT
		 priv->pmib->wapiInfo.wapiType = wapiDisable;
		 if(value == IW_AUTH_WAPI_VERSION_1)
		 {
		 	priv->pmib->wapiInfo.wapiType = wapiTypePSK;
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0;
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == 0;
			priv->pmib->dot1180211AuthEntry.dot11EnablePSK = 0;
			priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;	
			priv->pmib->dot1180211AuthEntry.dot11WPACipher = 0;
			priv->pmib->dot1180211AuthEntry.dot11WPA2Cipher = 0;
		} else
#endif
		if ( value == IW_AUTH_WPA_VERSION_DISABLED ) {	
		        priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
		        priv->pmib->dot1180211AuthEntry.dot11EnablePSK = 0;
		        priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;
		        priv->pmib->dot1180211AuthEntry.dot11WPACipher = 0;
	                priv->pmib->dot1180211AuthEntry.dot11WPA2Cipher = 0;	
		}
	       	else if(value == IW_AUTH_WPA_VERSION_WPA)
		{
		 	priv->pmib->wapiInfo.wapiType = wapiDisable;
			priv->pmib->dot1180211AuthEntry.dot11EnablePSK |= BIT(0);
		} 
		else if (value == IW_AUTH_WPA_VERSION_WPA2)
		{
		 	priv->pmib->wapiInfo.wapiType = wapiDisable;
			priv->pmib->dot1180211AuthEntry.dot11EnablePSK |= BIT(1);
		}
		break;

	case IW_AUTH_CIPHER_PAIRWISE:
		DEBUG_INFO("set_auth(IW_AUTH_CIPHER_PAIRWISE) value=%d\n", param->value);
		if (param->value & IW_AUTH_CIPHER_WEP40)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _WEP_40_PRIVACY_;
		else if (param->value & IW_AUTH_CIPHER_WEP104)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _WEP_104_PRIVACY_;
		else if (param->value & IW_AUTH_CIPHER_TKIP)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _TKIP_PRIVACY_;
		else if (param->value & IW_AUTH_CIPHER_CCMP)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _CCMP_PRIVACY_;
		else if (param->value & IW_AUTH_CIPHER_NONE)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		else {
			return -EINVAL;
		}
		break;
		
	case IW_AUTH_CIPHER_GROUP:
		DEBUG_INFO("set_auth(IW_AUTH_CIPHER_GROUP) value=%d\n", value);

		if (value & IW_AUTH_CIPHER_WEP40)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_WEP40;
		else if (value & IW_AUTH_CIPHER_WEP104)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_WEP104;
		else if (value & IW_AUTH_CIPHER_TKIP)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_TKIP;
		else if (value & IW_AUTH_CIPHER_CCMP)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_CCMP;
		else if (value & IW_AUTH_CIPHER_NONE)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_NONE;
		else {
			return -EINVAL;
		}
		
		break;
		
	case IW_AUTH_KEY_MGMT:
#ifdef CONFIG_RTL_WAPI_SUPPORT
		if (priv->pmib->wapiInfo.wapiType != wapiDisable)
		{
			if(value == IW_AUTH_KEY_MGMT_WAPI_PSK)
				priv->pmib->wapiInfo.wapiType = wapiTypePSK;
			else
				priv->pmib->wapiInfo.wapiType = wapiTypeCert;
			DEBUG_INFO("%s: IW_AUTH_KEY_MGMT wapiType %d \n", __FUNCTION__, priv->pmib->wapiInfo.wapiType);
		} else
#endif
#if 0
		if (value & IW_AUTH_KEY_MGMT_802_1X)
			priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 1;	
		else
			priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;	
#endif			
		break;

	case IW_AUTH_TKIP_COUNTERMEASURES:
		// TODO: TKIP countermeasures
		break;

	case IW_AUTH_DROP_UNENCRYPTED:
		break;

	case IW_AUTH_80211_AUTH_ALG:
		if ((param->value & IW_AUTH_ALG_SHARED_KEY)&&(param->value & IW_AUTH_ALG_OPEN_SYSTEM))
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 2;	// Auto			
		else if(param->value & IW_AUTH_ALG_OPEN_SYSTEM)
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0;	// Open
		else if (param->value & IW_AUTH_ALG_SHARED_KEY)
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 1;	// Shared
		else if (param->value & IW_AUTH_ALG_LEAP)
		{
			DEBUG_WARN("%s: wpa_set_auth_algs, AUTH_ALG_LEAP\n", __FUNCTION__);
		}
		else
			ret = -EINVAL;
		break;

	case IW_AUTH_WPA_ENABLED:
		if ( param->value == 0 ) {
		     priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0;
             priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
             priv->pmib->dot1180211AuthEntry.dot11EnablePSK = 0;
             priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;

             priv->pmib->dot1180211AuthEntry.dot11WPACipher = 0;
             priv->pmib->dot11GroupKeysTable.dot11Privacy = 0;			    

		     priv->pmib->dot1180211AuthEntry.dot11WPA2Cipher = 0;
             priv->pmib->dot11RsnIE.wpaie_len = 0;
             priv->pmib->dot11RsnIE.rsnie_len = 0;
             priv->pmib->dot11RsnIE.rsnielen = 0;
		}	
		
		break;

	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		//ieee->ieee802_1x = param->value;
		break;

	case IW_AUTH_PRIVACY_INVOKED:
 		//ieee->privacy_invoked = param->value;
		break;

#ifdef CONFIG_RTL_WAPI_SUPPORT
	case IW_AUTH_WAPI_ENABLED:
		break;
#endif

	default:
		return -EOPNOTSUPP;
		
	}
	
	return ret;
	
}

int rtl_siwauth(struct net_device *dev, 
				struct iw_request_info *info, 
				union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	struct iw_param *param = (struct iw_param*)&(wrqu->param);
	int value = param->value;
	int ret = 0;

    ASSERT_PRIV_RUNNING(priv);    
    SDIO_AP_WAKEUP(priv);
    printk("rtl_siwauth : cmd:%d val:%d\n", (param->flags & IW_AUTH_INDEX),  param->value);

	switch (param->flags & IW_AUTH_INDEX) 
    {
	case IW_AUTH_WPA_VERSION:
#ifdef CONFIG_RTL_WAPI_SUPPORT
		 priv->pmib->wapiInfo.wapiType = wapiDisable;
		 if(value == IW_AUTH_WAPI_VERSION_1)
		 {
		 	priv->pmib->wapiInfo.wapiType = wapiTypePSK;
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0;
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == 0;
			priv->pmib->dot1180211AuthEntry.dot11EnablePSK = 0;
			priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;	
			priv->pmib->dot1180211AuthEntry.dot11WPACipher = 0;
			priv->pmib->dot1180211AuthEntry.dot11WPA2Cipher = 0;
		} else
#endif
        if ( value == IW_AUTH_WPA_VERSION_DISABLED ) {	
            printk("cliW: wpa_diable\n");
            priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
            priv->pmib->dot1180211AuthEntry.dot11EnablePSK = 0;
            priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;
            priv->pmib->dot1180211AuthEntry.dot11WPACipher = 0;
            priv->pmib->dot1180211AuthEntry.dot11WPA2Cipher = 0;	
		}
	       	else if(value == IW_AUTH_WPA_VERSION_WPA)
		{
            printk("cliW: wpa 1\n");
            priv->pmib->wapiInfo.wapiType = wapiDisable;
            priv->pmib->dot1180211AuthEntry.dot11EnablePSK |= BIT(0);
		} 
		else if (value == IW_AUTH_WPA_VERSION_WPA2)
		{
            printk("cliW: wpa 2\n");
            priv->pmib->wapiInfo.wapiType = wapiDisable;
            priv->pmib->dot1180211AuthEntry.dot11EnablePSK |= BIT(1);
		}
		break;

	case IW_AUTH_CIPHER_PAIRWISE:
        DEBUG_INFO("set_auth(IW_AUTH_CIPHER_PAIRWISE) value=%d\n", param->value);
		if (param->value & IW_AUTH_CIPHER_WEP40)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _WEP_40_PRIVACY_;
		else if (param->value & IW_AUTH_CIPHER_WEP104)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _WEP_104_PRIVACY_;
		else if (param->value & IW_AUTH_CIPHER_TKIP)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _TKIP_PRIVACY_;
		else if (param->value & IW_AUTH_CIPHER_CCMP)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _CCMP_PRIVACY_;
		else if (param->value & IW_AUTH_CIPHER_NONE)
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		else {
			return -EINVAL;
		}
		break;
		
	case IW_AUTH_CIPHER_GROUP:
		DEBUG_INFO("set_auth(IW_AUTH_CIPHER_GROUP) value=%d\n", value);

		if (value & IW_AUTH_CIPHER_WEP40)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_WEP40;
		else if (value & IW_AUTH_CIPHER_WEP104)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_WEP104;
		else if (value & IW_AUTH_CIPHER_TKIP)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_TKIP;
		else if (value & IW_AUTH_CIPHER_CCMP)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_CCMP;
		else if (value & IW_AUTH_CIPHER_NONE)
			priv->pmib->dot11GroupKeysTable.dot11Privacy = DOT11_ENC_NONE;
		else {
			return -EINVAL;
		}
		
		break;
		
	case IW_AUTH_KEY_MGMT:
#ifdef CONFIG_RTL_WAPI_SUPPORT
		if (priv->pmib->wapiInfo.wapiType != wapiDisable)
		{
			if(value == IW_AUTH_KEY_MGMT_WAPI_PSK)
				priv->pmib->wapiInfo.wapiType = wapiTypePSK;
			else
				priv->pmib->wapiInfo.wapiType = wapiTypeCert;
			DEBUG_INFO("%s: IW_AUTH_KEY_MGMT wapiType %d \n", __FUNCTION__, priv->pmib->wapiInfo.wapiType);
		} else
#endif
#if 0
		if (value & IW_AUTH_KEY_MGMT_802_1X)
			priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 1;	
		else
			priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;	
#endif			
		break;

	case IW_AUTH_TKIP_COUNTERMEASURES:
		// TODO: TKIP countermeasures
		break;

	case IW_AUTH_DROP_UNENCRYPTED:
		break;

	case IW_AUTH_80211_AUTH_ALG:
		if ((param->value & IW_AUTH_ALG_SHARED_KEY)&&(param->value & IW_AUTH_ALG_OPEN_SYSTEM))
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 2;	// Auto			
		else if(param->value & IW_AUTH_ALG_OPEN_SYSTEM)
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0;	// Open
		else if (param->value & IW_AUTH_ALG_SHARED_KEY)
			priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 1;	// Shared
		else if (param->value & IW_AUTH_ALG_LEAP)
		{
			DEBUG_WARN("%s: wpa_set_auth_algs, AUTH_ALG_LEAP\n", __FUNCTION__);
		}
		else
			ret = -EINVAL;
		break;

	case IW_AUTH_WPA_ENABLED:
		if ( param->value == 0 ) {
		     priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0;
             priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
             priv->pmib->dot1180211AuthEntry.dot11EnablePSK = 0;
             priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm = 0;

             priv->pmib->dot1180211AuthEntry.dot11WPACipher = 0;
             priv->pmib->dot11GroupKeysTable.dot11Privacy = 0;			    

		     priv->pmib->dot1180211AuthEntry.dot11WPA2Cipher = 0;
             priv->pmib->dot11RsnIE.wpaie_len = 0;
             priv->pmib->dot11RsnIE.rsnie_len = 0;
             priv->pmib->dot11RsnIE.rsnielen = 0;
		}	
		
		break;

	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		//ieee->ieee802_1x = param->value;
		break;

	case IW_AUTH_PRIVACY_INVOKED:
 		//ieee->privacy_invoked = param->value;
		break;

#ifdef CONFIG_RTL_WAPI_SUPPORT
	case IW_AUTH_WAPI_ENABLED:
		break;
#endif

	default:
		return -EOPNOTSUPP;
		
	}	

    printk("Exit, ret = %d\n", ret);
    return ret;
}


int rtl_giwauth(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu,
		char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	struct iw_param *param = (struct iw_param*)&(wrqu->param);
	int ret = 0;
	
    ASSERT_PRIV_RUNNING(priv);

    SDIO_AP_WAKEUP(priv);
    DEBUG_WARN("%s: get_auth: flags=0x%x authmode=%d\n", __FUNCTION__, param->flags
                                , priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm);
	param->value = 0;

	switch (param->flags & IW_AUTH_INDEX) {
	case IW_AUTH_WPA_VERSION:
#ifdef CONFIG_RTL_WAPI_SUPPORT
		if (priv->pmib->wapiInfo.wapiType != wapiDisable;)
		{
			param->value |= IW_AUTH_WAPI_VERSION_1;
		}
#endif
		if (priv->pmib->dot1180211AuthEntry.dot11EnablePSK & BIT(0))
		{
			param->value |= IW_AUTH_WPA_VERSION_WPA;
		}

		if (priv->pmib->dot1180211AuthEntry.dot11EnablePSK & BIT(1))
		{
			param->value |= IW_AUTH_WPA_VERSION_WPA2;
		}
		break;
		
	case IW_AUTH_CIPHER_PAIRWISE:
		if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _WEP_40_PRIVACY_)
		{
			param->value |= IW_AUTH_CIPHER_WEP40;
		}
		else if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _WEP_104_PRIVACY_)
		{
			param->value |= IW_AUTH_CIPHER_WEP104;
		}
		else if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _TKIP_PRIVACY_)
		{
			param->value |= IW_AUTH_CIPHER_TKIP;
		}
		else if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _CCMP_PRIVACY_)
		{
			param->value |= IW_AUTH_CIPHER_CCMP;
		}
		else if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _NO_PRIVACY_)
		{
			param->value |= IW_AUTH_CIPHER_NONE;
		}
		break;

	case IW_AUTH_KEY_MGMT:
#ifdef CONFIG_RTL_WAPI_SUPPORT
		if (priv->pmib->wapiInfo.wapiType != wapiDisable)
		{
			if (priv->pmib->wapiInfo.wapiType == wapiTypePSK)
				param->value = IW_AUTH_KEY_MGMT_WAPI_PSK;
			else
				param->value = IW_AUTH_KEY_MGMT_WAPI_CERT;
		} else
#endif
		if (priv->pmib->dot118021xAuthEntry.dot118021xAlgrthm)
		{
			if (priv->pmib->dot1180211AuthEntry.dot11EnablePSK)
				param->value = IW_AUTH_KEY_MGMT_PSK;
			else
				param->value = IW_AUTH_KEY_MGMT_802_1X;
		}
		break;
// TODO:==> 
	case IW_AUTH_80211_AUTH_ALG:
		switch (priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm) {
		case 0:
			param->value = IW_AUTH_ALG_OPEN_SYSTEM;
			break;

		case 1:
			param->value = IW_AUTH_ALG_OPEN_SYSTEM;
			break;

		default:
			DEBUG_ERR("%s: invalid auth_alg(%d)\n", __FUNCTION__,priv->pmib->dot1180211AuthEntry.dot11AuthAlgrthm);
			param->value = IW_AUTH_ALG_OPEN_SYSTEM;
			break;
		}
		break;

	case IW_AUTH_WPA_ENABLED:
		// TODO:
		break;

	default:
		ret = -EOPNOTSUPP;
	}
	DEBUG_INFO("%s: flags=0x%x value=%d ret=%d\n", __FUNCTION__, param->flags, param->value, ret);

	return ret;
}

extern int DOT11_Process_Delete_Key(struct net_device *dev, struct iw_point *data);

int rtl_siwencodeext(struct net_device *dev, 
			     struct iw_request_info *info, 
			     union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	struct iw_point 		*encoding = &wrqu->encoding;
	struct iw_encode_ext	*ext = (struct iw_encode_ext *)extra;
	s4Byte					alg = ext->alg;
	u1Byte					EncAlgorithm = DOT11_ENC_NONE;
	s4Byte					KeyIndex;
	u4Byte					KeyLength=0;
	BOOLEAN 				IsDefaultKey = FALSE;
	pu1Byte 				KeyMaterial = ext->key;
	BOOLEAN 				IsGroupKey = FALSE;
//	u8Byte					KeyRSC;
	int 					ret=0;
	DOT11_SET_KEY Set_Key;
	struct stat_info	*pstat;

    ASSERT_PRIV_RUNNING(priv);    
    SDIO_AP_WAKEUP(priv);
	memset((char *)&Set_Key, 0, sizeof(Set_Key));
	
	if (encoding->flags & IW_ENCODE_DISABLED) {
		alg = IW_ENCODE_ALG_NONE;
	} 

	/* Get Key Index and convet to our own defined key index */
	KeyIndex = (encoding->flags & IW_ENCODE_INDEX) - 1;
	KeyLength = ext->key_len;
	if (ext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
		IsDefaultKey = TRUE;
	}

	//if ((ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY) || (IS_BCAST(ext->addr.sa_data))) {
	if ( (ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY) ) {
		IsGroupKey = TRUE;
	}

    printk("siwencode cmd:%x val:%x/ len:%u\n", (u4Byte)alg, ext->ext_flags, KeyLength);
    printk("KeyIndex = %d, KeyLength = %d, IsDefaultKey = %d, IsGroupKey = %d\n"
        , KeyIndex, KeyLength, IsDefaultKey, IsGroupKey);

	switch (alg) {
	case IW_ENCODE_ALG_NONE:
		/* Clear all keys */
		EncAlgorithm = DOT11_ENC_NONE;
		break;
		
	case IW_ENCODE_ALG_WEP:
		if ((KeyIndex < 0) || (KeyIndex >= 4))
		{
			ret = -EINVAL;
			goto set_encodeext_end;
		}

		if(KeyLength == 5){
			EncAlgorithm = DOT11_ENC_WEP40;
		}
		else if(KeyLength == 13) {
			EncAlgorithm = DOT11_ENC_WEP104;
		}
		else
		{
			ret = -EINVAL;
			goto set_encodeext_end;
		}

		break;
			
	case IW_ENCODE_ALG_TKIP:
		EncAlgorithm = DOT11_ENC_TKIP;
		// Check key length for TKIP.
		if(KeyLength != 32) {
			if (IsGroupKey && (KeyLength == 5 || KeyLength == 13)) {  
				// 5 = WEP40 13 = WEP104 16 = CKIP
				if(KeyLength == 5) {
					EncAlgorithm = DOT11_ENC_WEP40;
				} else {
					EncAlgorithm = DOT11_ENC_WEP104;
				}
				DEBUG_WARN("%s: Check key length 5 or 13 for Mix-mode WEP.\n", __FUNCTION__);
			} else {
				DEBUG_ERR("TKIP KeyLength:%u != 32\n", KeyLength);
				ret = -EINVAL;
				goto set_encodeext_end;
			}
		}
			
		break;

	case IW_ENCODE_ALG_CCMP:
		EncAlgorithm = DOT11_ENC_CCMP;
		if(KeyLength != 16) {
			// For our supplicant, EAPPkt9x.vxd, cannot differentiate TKIP and AES case.
			if(KeyLength == 32)
				KeyLength = 16; 
		}

		if((KeyLength != 16) && (KeyLength != 32)) {
			if (IsGroupKey && (KeyLength == 5 || KeyLength == 13)) {  
				// 5 = WEP40 13 = WEP104 16 = CKIP
				if(KeyLength == 5) {
					EncAlgorithm = DOT11_ENC_WEP40;
				} else {
					EncAlgorithm = DOT11_ENC_WEP104;
				}
				DEBUG_WARN("%s: Check key length 5 or 13 for Mix-mode WEP.\n", __FUNCTION__);
			} else {
				DEBUG_ERR("AES_CCM KeyLength:%u != 16\n", KeyLength);
				ret = -EINVAL;
				goto set_encodeext_end;
			}
		}
		break;

	default:
			ret = -EINVAL;
			goto set_encodeext_end;
	}

	if (KeyLength > 0) {
		if ((EncAlgorithm == DOT11_ENC_WEP40) || (EncAlgorithm == DOT11_ENC_WEP104))
		{
			priv->pmib->dot11DefaultKeysTable.keylen[KeyIndex] = KeyLength <= 5 ? 5 : 13;
			memcpy(priv->pmib->dot11DefaultKeysTable.keytype[KeyIndex].skey, KeyMaterial, KeyLength);			
		}

		if (IsDefaultKey)
		{
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = KeyLength <= 5 ? _WEP_40_PRIVACY_ : _WEP_104_PRIVACY_;
			priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyLen = KeyLength <= 5 ? 5 : 13;
			priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex = KeyIndex;

			/* cliW add WEP key directly for stainfo not ctrated yet */
			if ((EncAlgorithm == DOT11_ENC_WEP40) || (EncAlgorithm == DOT11_ENC_WEP104)) {
                /* BEGIN PN: DTS2014061707718,Added by h00231374, 2014/06/24*/
				// don't setup PTK CAM because BSSID is unavailable before first join.
				// PTK CAM will be set on set_keymapping_wep when association success.
                if (IsGroupKey) 
                {
                    struct Dot11EncryptKey *pEncryptKey;
					
                    priv->pmib->dot11GroupKeysTable.dot11Privacy = EncAlgorithm;
                    priv->pmib->dot11GroupKeysTable.keyid = KeyIndex;
					
                    pEncryptKey = &priv->pmib->dot11GroupKeysTable.dot11EncryptKey;
                    pEncryptKey->dot11TTKeyLen = KeyLength <= 5 ? 5 : 13;
                    pEncryptKey->dot11TMicKeyLen = 0;
                    memset(pEncryptKey->dot11TTKey.skey, 0, sizeof(pEncryptKey->dot11TTKey.skey));
                    memcpy(pEncryptKey->dot11TTKey.skey, KeyMaterial, pEncryptKey->dot11TTKeyLen);
                }
			    goto set_encodeext_end;
			}
		}

		if (IsGroupKey)  {
			/* cliW: not set group key in wep key */
			if ((EncAlgorithm == DOT11_ENC_WEP40) || (EncAlgorithm == DOT11_ENC_WEP104)) {
			    goto set_encodeext_end;
            }
			/* cliW: fix group key for wpa_supplicant rekey use toogle index */
			if ((EncAlgorithm == DOT11_ENC_TKIP) || (EncAlgorithm == DOT11_ENC_CCMP)) 
			    KeyIndex = 1;

			priv->pmib->dot11GroupKeysTable.dot11Privacy = EncAlgorithm;
			priv->pmib->dot11GroupKeysTable.keyid = KeyIndex;
		}

	        /* cliW test supplicant using [ len : tx : rx ] key format */
		if ( EncAlgorithm == DOT11_ENC_TKIP ) {
  		     char tmp[8];
#if 1		     
		     WPA_GLOBAL_INFO *pGblInfo = priv->wpa_global_info;

		     if (  IsDefaultKey ) {
                         pGblInfo->GkeyReady = TRUE;
                         pGblInfo->GN = KeyIndex;
                     }
                     pGblInfo->MulticastCipher = _TKIP_PRIVACY_;
#endif
		     memcpy(tmp, KeyMaterial+16, 8);
		     memcpy(KeyMaterial+16, KeyMaterial+24, 8);
		     memcpy(KeyMaterial+24, tmp, 8);
		}      

		memcpy(Set_Key.MACAddr, ext->addr.sa_data, MACADDRLEN);
		if (IsDefaultKey)
			Set_Key.KeyType = DOT11_KeyType_Pairwise;
		else
			Set_Key.KeyType = (IsGroupKey ? DOT11_KeyType_Group : DOT11_KeyType_Pairwise);

		Set_Key.EncType = EncAlgorithm;
		Set_Key.KeyIndex = KeyIndex;
		Set_Key.KeyLen = KeyLength;
		ret = DOT11_Process_Set_Key(dev, NULL, &Set_Key, KeyMaterial);

		if ((EncAlgorithm == DOT11_ENC_TKIP) || (EncAlgorithm == DOT11_ENC_CCMP))
		{
			pstat = get_stainfo(priv, ext->addr.sa_data);
			if ((pstat != NULL) && (pstat->state & WIFI_ASOC_STATE))
				pstat->ieee8021x_ctrlport = 1;
		}
	}
	else if (EncAlgorithm == DOT11_ENC_NONE)
	{
		// Remove Key
		DOT11_DELETE_KEY	Delete_Key;
                struct iw_point wrq;
        
		if ((EncAlgorithm == DOT11_ENC_WEP40) || (EncAlgorithm == DOT11_ENC_WEP104))
		{
			priv->pmib->dot11DefaultKeysTable.keylen[KeyIndex] =0;
			memset(priv->pmib->dot11DefaultKeysTable.keytype[KeyIndex].skey, 0, 16);			
		}

		if (IsDefaultKey)
		{
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = _NO_PRIVACY_;
			priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyLen = 0;
		}

	        wrq.pointer = (caddr_t)&Delete_Key;
	        wrq.length = sizeof(DOT11_DELETE_KEY);
	
		memset(&Delete_Key, 0, sizeof(Delete_Key));
		Delete_Key.EventId = DOT11_EVENT_DELETE_KEY;
	        Delete_Key.IsMoreEvent = FALSE;
	
		Delete_Key.KeyType = (IsGroupKey ? DOT11_KeyType_Group : DOT11_KeyType_Pairwise);
		memcpy(Delete_Key.MACAddr, ext->addr.sa_data, MACADDRLEN);
		
                ret = DOT11_Process_Delete_Key(priv->dev, &wrq);		
		// ret = DOT11_Process_Delete_Key(dev, NULL, &Delete_Key);
	}
	
set_encodeext_end:	
	return ret;
}

int rtl_giwencodeext(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu,
		char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	struct iw_point 		*encoding = &wrqu->encoding;
	struct iw_encode_ext 	*ext = (struct iw_encode_ext *)extra;
	s4Byte					key_index;
	s4Byte					ret = 0;
	s4Byte 					max_key_len;
	char					*pkey=NULL;
//	char					keybuf[32];
	int						keylen=0;

    ASSERT_PRIV_RUNNING(priv);
    SDIO_AP_WAKEUP(priv);
	DEBUG_INFO("rtwlan_ioctl: rtioctl_get_encodeext: \n");
	
	max_key_len = encoding->length - sizeof(struct iw_encode_ext);
	if (max_key_len < 0)
		return -EINVAL;
	key_index = (encoding->flags & IW_ENCODE_INDEX) - 1;

	memset(ext, 0, sizeof(struct iw_encode_ext));

	switch (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm) {
	case _WEP_40_PRIVACY_:
	case _WEP_104_PRIVACY_:
		ext->alg = IW_ENCODE_ALG_WEP;
		encoding->flags |= IW_ENCODE_ENABLED;
		pkey = priv->pmib->dot11DefaultKeysTable.keytype[key_index].skey;
		keylen = 16;
		break;

	case _TKIP_PRIVACY_:
		ext->alg = IW_ENCODE_ALG_TKIP;
		encoding->flags |= IW_ENCODE_ENABLED;
		// TODO: get key
		break;

	case _CCMP_PRIVACY_:
		ext->alg = IW_ENCODE_ALG_CCMP;
		encoding->flags |= IW_ENCODE_ENABLED;
		// TODO: get key
		break;

	case _NO_PRIVACY_:
	default:
		ext->alg = IW_ENCODE_ALG_NONE;
		encoding->flags |= IW_ENCODE_NOKEY;
		break;
	}
	
	if (!(encoding->flags & IW_ENCODE_NOKEY)) {
		if (max_key_len < keylen) {
				ret = -E2BIG;
				goto out;
		}
		if (pkey)
			memcpy (ext->key, pkey, keylen);
	}

	out:
	return ret;
}

int rtl_siwpmkid(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = (RTL_PRIV *)dev->priv;
#endif
	unsigned char	j,blInserted = _FALSE;
	int ret=0;

#if 0		
	struct iw_pmksa*  pPMK = ( struct iw_pmksa* ) extra;
	unsigned char     strZeroMacAddress[ ETH_ALEN ] = { 0x00 };
	unsigned char     strIssueBssid[ ETH_ALEN ] = { 0x00 };
        
	/*
	struct iw_pmksa
    {
        __u32   cmd;
        struct sockaddr bssid;
        __u8    pmkid[IW_PMKID_LEN];   //IW_PMKID_LEN=16
    }
    There are the BSSID information in the bssid.sa_data array.
    If cmd is IW_PMKSA_FLUSH, it means the wpa_suppplicant wants to clear all the PMKID information.
    If cmd is IW_PMKSA_ADD, it means the wpa_supplicant wants to add a PMKID/BSSID to driver.
    If cmd is IW_PMKSA_REMOVE, it means the wpa_supplicant wants to remove a PMKID/BSSID from driver.
	*/

	memcpy( strIssueBssid, pPMK->bssid.sa_data, ETH_ALEN);
    if ( pPMK->cmd == IW_PMKSA_ADD )
    {
		DEBUG_INFO( "%s: IW_PMKSA_ADD!\n", __FUNCTION__ );
		if (!memcmp( strIssueBssid, strZeroMacAddress, ETH_ALEN ))
		{
		    return (-EINVAL);
		}

		blInserted = _FALSE;
		
		//overwrite PMKID
		for(j=0 ; j<NUM_PMKID_CACHE; j++)
		{
			if( !memcmp( priv->PMKIDList[j].Bssid, strIssueBssid, ETH_ALEN) )
			{ // BSSID is matched, the same AP => rewrite with new PMKID.

				DEBUG_INFO( "%s: BSSID exists in the PMKList.\n", __FUNCTION__ );

				memcpy( priv->PMKIDList[j].PMKID, pPMK->pmkid, IW_PMKID_LEN);
				priv->PMKIDList[ j ].bUsed = _TRUE;
				priv->PMKIDIndex = j+1;
				blInserted = _TRUE;
				break;
			}	
		}

		if(!blInserted)
		{
			// Find a new entry
			DEBUG_INFO( "[%s] Use the new entry index = %d for this PMKID.\n", __FUNCTION__,
				priv->PMKIDIndex );

			memcpy(priv->PMKIDList[priv->PMKIDIndex].Bssid, strIssueBssid, ETH_ALEN);
			memcpy(priv->PMKIDList[priv->PMKIDIndex].PMKID, pPMK->pmkid, IW_PMKID_LEN);

			priv->PMKIDList[priv->PMKIDIndex ].bUsed = _TRUE;
			priv->PMKIDIndex++ ;
			if(priv->PMKIDIndex == NUM_PMKID_CACHE)
			{
				priv->PMKIDIndex = 0;
			}
		}
	}
	else if ( pPMK->cmd == IW_PMKSA_REMOVE )
	{
		DEBUG_INFO( "[%s] IW_PMKSA_REMOVE!\n", __FUNCTION__ );
		for(j=0 ; j<NUM_PMKID_CACHE; j++)
		{
			if( !memcmp( priv->PMKIDList[j].Bssid, strIssueBssid, ETH_ALEN))
			{ // BSSID is matched, the same AP => Remove this PMKID information and reset it. 
				memset( priv->PMKIDList[ j ].Bssid, 0x00, ETH_ALEN );
				priv->PMKIDList[ j ].bUsed = _FALSE;
				break;
			}	
		}
	}
	else if ( pPMK->cmd == IW_PMKSA_FLUSH ) 
	{
		DEBUG_INFO( "[%s] IW_PMKSA_FLUSH!\n", __FUNCTION__ );
		memset( &priv->PMKIDList[ 0 ], 0x00, sizeof( RT_PMKID_LIST ) * NUM_PMKID_CACHE );
		priv->PMKIDIndex = 0;
	}
#endif	
    return( ret );
}
#endif


#ifdef CONFIG_RTL_COMAPI_CFGFILE

#define CFG_FILE_PATH			"/etc/Wireless/RTL8192CD.dat"

void del_mib_list(struct net_device *dev);
int CfgFileSetMib(struct net_device *dev, char *buf);

int CfgFileProc(struct net_device *dev)
{
	//RTL_PRIV *priv = dev->priv;
	unsigned char *mem_ptr;
	int ret = 0;
	
	printk("-------> Set MIB from " CFG_FILE_PATH "\n");	
	if((mem_ptr = (unsigned char *)kmalloc(MAX_CONFIG_FILE_SIZE, GFP_ATOMIC)) == NULL) {
		printk("%s: not enough memory\n", __FUNCTION__);
		return -1;
	}
	
	memset(mem_ptr, 0, MAX_CONFIG_FILE_SIZE);
	
	ret = CfgFileRead(dev, mem_ptr);

	if (ret < 0)
	{
		printk("%s: ReadCfgFile failed (%d)\n", __FUNCTION__, ret);
		goto proc_exit;
	}

	//printk("%s\n", mem_ptr);

	del_mib_list(dev);

	CfgFileSetMib(dev, mem_ptr);

	printk("<------- Set MIB from " CFG_FILE_PATH " Success\n");

proc_exit: 
	kfree(mem_ptr);
	return ret;
	
}

int CfgFileRead(struct net_device *dev, char *buf)
{
	//RTL_PRIV *priv = dev->priv;
	struct file *fp;
	mm_segment_t oldfs;
	//size_t len;

	//int read_bytes = 0;
	int ret = 0;

	oldfs = get_fs();
	set_fs(get_ds());
	fp = filp_open(CFG_FILE_PATH, O_RDONLY, 0);
	if(IS_ERR(fp)) {
		ret = PTR_ERR(fp);
		printk("Fail to open configuration file. (%d)\n", ret);
		goto err_exit;
	}
	
	if (!(fp->f_op && fp->f_op->read)) {	
		printk("Fail to support file ops: read\n");
		ret = -1;
		goto err_close;
	}	
	
	if ((ret = fp->f_op->read(fp, buf, MAX_CONFIG_FILE_SIZE, &fp->f_pos))< 0){
		printk("Fail to read file. (%d)\n", ret);
		goto err_close;
	}

err_close:
	filp_close(fp, NULL);
err_exit:	
	set_fs(oldfs);
	return ret;
	
}

static int rewrite_line (unsigned char **dst, unsigned char **src)
{
	int cnt=0;
	char *s = *src;
	char *d = *dst;
	char *loc=NULL, *vl_s=NULL, *vl_e=NULL;
	unsigned char quoted = 0;

	//printk("src = %s(%d)\n", *src, strlen(*src));
	loc=strchr(s, '"');
	if (loc) {
		unsigned int i = strlen(*src);
		vl_s=loc;
		while (i>0){
			char *t = (char *)((unsigned long)s+i-1);
			if (*t=='"' && t > vl_s ){
					vl_e = t;
					quoted = 1;
					break;
			}
			i--;
		}
	}
	
	while (*s) {
		u8 noop = 0;
		if (quoted ==1 && (vl_s < s && s < vl_e))
			noop = 1;
			
		if ( noop == 0 ) {
			if ((*s=='\r') || (*s=='\n') || (*s=='#') || (*s=='\0'))
				break;
			if ((*s == '\t')||(*s == ' ')||(*s == '"')){
			s++;
				continue;
			}
		}

			*d = *s;
			s++;
			d++;
			cnt++;

	}
	*d = '\0';
	//printk("   dst = %s\n", *dst);
	return cnt;
}


int CfgFileSetMib(struct net_device *dev, char *buf)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = dev->priv;
#endif
	unsigned char *line_head, *next_head;
	unsigned char *cmd_buf, *mibstr, *valstr, *mibstart;
	//struct mib_cfg_func *tmp_mibcfg;
	int ret = 0;
#ifdef VENDOR_PARAM_COMPATIBLE
	int arg_num = sizeof(RTL_SUPPORT_MIBCFG)/sizeof(struct mib_cfg_func);
#endif //VENDOR_PARAM_COMPATIBLE

	if((cmd_buf = (unsigned char *)kmalloc(MAX_PARAM_BUF_SIZE, GFP_ATOMIC)) == NULL) {
		printk("%s(%d): not enough memory\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	if((mibstr = (unsigned char *)kmalloc(20, GFP_ATOMIC)) == NULL) {
		printk("%s(%d): not enough memory\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	if((valstr = (unsigned char *)kmalloc(MAX_PARAM_BUF_SIZE, GFP_ATOMIC)) == NULL) {
		printk("%s(%d): not enough memory\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	next_head = buf;
	
	do {
		char *loc;
		int len = 0, miblen = 0, vallen = 0;
		//int i=0;
		
		memset(cmd_buf, 0, MAX_PARAM_BUF_SIZE);
		memset(mibstr, 0, 20);
		memset(valstr, 0, MAX_PARAM_BUF_SIZE);
		
		line_head = next_head;
		next_head = get_line(&line_head);
		if (line_head == NULL)
			break;
		
		if (line_head[0] == '#')
			continue;
		
		len = rewrite_line(&cmd_buf, &line_head);
		//printk("%s (%d)\n", cmd_buf, len);
		
#ifdef VENDOR_PARAM_COMPATIBLE	
		/* To compatible with other vendor's parameters, each parameter must have its own process function - chris*/
		loc = strchr(mibstart, '=');
		miblen = (u32)loc - (u32)mibstart;
		vallen = len - miblen -1;
		if (vallen>0) {			
			for (i=0; i<arg_num; i++) {
				if (strcmp(mibstr, RTL_SUPPORT_MIBCFG[i].name) == 0) {
					if(!RTL_SUPPORT_MIBCFG[i].set_proc(priv, valstr)) {
						printk("CFGFILE set %s failed \n", mibstr);
						return -1;
					}
					break;
				}
			}
		}
#else
			
		//printk(">>>>>>>> cmd=%s , %s, %c \n",cmd_buf, dev->name, cmd_buf[strlen(dev->name)]);
		if (!strncmp(dev->name, cmd_buf, strlen(dev->name))&&(cmd_buf[strlen(dev->name)]!='-')) {
			mibstart = cmd_buf + strlen(dev->name)+1;
		} else
			continue;
		
		loc = strchr(mibstart, '=');
		miblen = (u32)loc - (u32)mibstart;
		vallen = len - (strlen(dev->name)+1) - (miblen+1);
		
		if (vallen>0) {
			
			ret = set_mib(priv, mibstart);
			if (ret < 0) {
				strncpy(mibstr, mibstart, miblen);
				strncpy(valstr, (char*)((u32)loc+1), vallen);
				//printk("(%s) = (%s) (%d)\n", mibstr, valstr, vallen);
				printk("CFGFILE set_mib \"%s\" failed \n", mibstart);
				//return -1;
			}

#endif // VENDOR_PARAM_COMPATIBLE
		}
		
	} while (1);

	kfree(cmd_buf);
	kfree(mibstr);
	kfree(valstr);
	
	return ret;
}

void del_mib_list(struct net_device *dev)
{
#ifdef NETDEV_NO_PRIV
	RTL_PRIV *priv = ((RTL_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	RTL_PRIV *priv = dev->priv;
#endif
	
	if (priv->pmib) {
#ifdef WDS
		priv->pmib->dot11WdsInfo.wdsNum = 0;
#endif
		priv->pmib->dot11StationConfigEntry.dot11AclNum=0;
	}
	
}

#endif //CONFIG_RTL_COMAPI_CFGFILE


