
#ifndef _ATDEVICECMD_H_
#define _ATDEVICECMD_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "v_typdef.h"
/* Added by f62575 for B050 Project, 2012-2-3, begin   */
#include "ATCmdProc.h"
/* Added by f62575 for B050 Project, 2012-2-3, end */

#if (FEATURE_ON == MBB_COMMON)
#include "wlan_at_api.h"
#include "product_nv_id.h"
#include "product_nv_def.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*生产使用的加密结构*/
/* Added by f62575 for SMALL IMAGE, 2012-1-3, begin */

#if (FEATURE_ON == MBB_COMMON)
#define AT_NV_WLKEY_LEN1                (5)
#define AT_NV_WLKEY_LEN2                (13)
#define AT_NV_WLKEY_LEN3                (10)
#define AT_NV_WLKEY_LEN4                (26)

#define AT_WIFI_KEY_LEN_MIN           (8)
#define AT_WIFI_KEY_LEN_MAX_MBB           (64)

#define AT_WIFI_WEP_INDEX0           (0)
#define AT_WIFI_WEP_INDEX1           (1)
#define AT_WIFI_WEP_INDEX2           (2)
#define AT_WIFI_WEP_INDEX3           (3)

/*WEB UI 网址存储最大长度*/
#define AT_WEBUI_SITE_NV_LEN_MAX      (36)
/*WEB UI 网址读写最大长度*/
#define AT_WEBUI_SITE_WR_LEN_MAX      (32)

/*WEB UI USER 用户名存储最大长度*/
#define AT_WEBUI_USER_NV_LEN_MAX      (36)
/*WEB UI USER 用户名读写最大长度*/
#define AT_WEBUI_USER_WR_LEN_MAX      (32)

/*WIFI PIN 检查*/
#define AT_WIFI_PIN_NV_LEN_MAX        (36)
#define AT_WIFI_8BIT_PIN_LEN          (8)
#define AT_WIFI_4BIT_PIN_LEN          (4)

/* ^VERSION命令INI长度 */
#define AT_VERSION_INI_LEN               (32)
/* ^TBATDATA 电池电压校准参数数量 */
#define VOLT_CALC_NUM_MAX               (2)
/* ^TBATDATA 电流校准参数数量 */
#define CURRENT_CALC_NUM_MAX            (0) 
/* ^TBATDATA 电池电压设置命令参数个数*/
#define AT_TBATDATA_SET_PARA_NUM        (3)

/* ^TSELRF? FDD TDD 频段范围 */
#define AT_TSELRF_FDD_BAND_MIN        (1)
#define AT_TSELRF_FDD_BAND_MAX        (32)
#define AT_TSELRF_TDD_BAND_MIN        (33)
#define AT_TSELRF_TDD_BAND_MAX        (64)

/* ^VERSION命令信息字段名最大长度以及值最大长度定义 */
#define TAF_MAX_VERSION_VALUE_LEN    (129)
#endif


/* WIFI KEY字符串长度，与NVIM模块Ipcom_parameters_handle.h文件的 NV_WLKEY_LEN 保持一致 */
#define AT_NV_WLKEY_LEN                 (27)

/* 鉴权模式字符串长度，与NVIM模块Ipcom_parameters_handle.h文件的 NV_WLAUTHMODE_LEN 保持一致 */
#define AT_NV_WLAUTHMODE_LEN            (16)

/* 加密模式字符串长度，与NVIM模块Ipcom_parameters_handle.h文件的 NV_ENCRYPTIONMODES_LEN 保持一致 */
#define AT_NV_ENCRYPTIONMODES_LEN       (5)

/* WPA的密码字符串长度，与NVIM模块Ipcom_parameters_handle.h文件的 NV_WLWPAPSK_LEN 保持一致 */
#define AT_NV_WLWPAPSK_LEN              (65)

/* NV项50012的数据结构nv_wifisec_type WIFI KEY后面的数据长度 */
#define AT_NV_WIFISEC_OTHERDATA_LEN     (72)

/* 单板侧支持的WIFI KEY个数 */
#define AT_WIWEP_CARD_WIFI_KEY_TOTAL    (4)

/* Add by z60575 for multi_ssid, 2012-9-5 begin */
/* 工位侧支持的WIFI KEY个数 */
#define AT_WIWEP_TOOLS_WIFI_KEY_TOTAL   (16)
/* Add by z60575 for multi_ssid, 2012-9-5 end */

/* PHYNUM命令物理号类型MAC的物理号长度 */
#define AT_PHYNUM_MAC_LEN               (12)

/* PHYNUM命令物理号类型MAC的物理号与^WIFIGLOBAL命令匹配需要增加的冒号个数 */
#define AT_PHYNUM_MAC_COLON_NUM         (5)

#define AT_TSELRF_PATH_TOTAL            (4)

#define AT_WIFIGLOBAL_MAC_LEN_BETWEEN_COLONS   (2)                             /* MAC地址中冒号之间的字符个数*/

/* Added by f62575 for SMALL IMAGE, 2012-1-3, end   */

#if(FEATURE_ON == FEATURE_UE_MODE_TDS)
#define AT_TDS_SCALIB_STR (VOS_UINT8 *)"(\"CACHE\",\"USE\",\"SAVE\",\"GET\",\"READ\",\"INI\",\"BEGIN\",\"END\",\"SET\",\"\"),(\"APCOFFSETFLAG\",\"APCOFFSET\",\"APC\",\"APCFREQ\",\"AGC\",\"AGCFREQ\",\"\"),(1,5,6),(@pram)"
#define AT_TDS_SCALIB_TEST_STR "(CACHE,USE,SAVE,GET,READ,INI,BEGIN,END,SET),(APCOFFSETFLAG,APCOFFSET,APC,APCFREQ,AGC,AGCFREQ),(1,5,6),(DATA)"
#endif

/*begin add by wanghaijie for simlock 3.0*/
#if (FEATURE_ON == MBB_SIMLOCK_THREE)
#define AT_HWLOCK_PARA_LEN               (8)
#endif
/*end   add by wanghaijie for simlock 3.0*/

#if (FEATURE_ON == MBB_SIMLOCK_FOUR)
#define AT_HWLOCK_PARA_LEN               (16)
#endif
/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum AT_TBAT_OPERATION_TYPE_ENUM
{
    AT_TBAT_BATTERY_ANALOG_VOLTAGE      = 0,
    AT_TBAT_BATTERY_DIGITAL_VOLTAGE     = 1,
    AT_TBAT_BATTERY_VOLUME              = 2,
    AT_TBAT_OPERATION_TYPE_BUTT
};
typedef VOS_UINT32  AT_TBAT_OPERATION_TYPE_ENUM_UINT32;


enum AT_TBAT_OPERATION_DIRECTION_ENUM
{
    AT_TBAT_READ_FROM_UUT,
    AT_TBAT_SET_TO_UUT,
    AT_TBAT_OPERATION_DIRECTION_BUTT
};
typedef VOS_UINT32  AT_TBAT_OPERATION_DIRECTION_ENUM_UINT32;


enum AT_TSELRF_PATH_ENUM
{
    AT_TSELRF_PATH_GSM                  = 1,
    AT_TSELRF_PATH_WCDMA_PRI            = 2,
    AT_TSELRF_PATH_WCDMA_DIV            = 3,
    AT_TSELRF_PATH_TD					= 6,
    AT_TSELRF_PATH_WIFI                 = 7,
    AT_TSELRF_PATH_BUTT
};
typedef VOS_UINT32  AT_TSELRF_PATH_ENUM_UINT32;

#if (FEATURE_ON == MBB_COMMON)
enum AT_MMI_TEST{
    AT_MMI_TEST_SUPPORT_NONSUPPORT = 0, /* 不支持 */
    AT_MMI_TEST_SUPPORT_MANUAL,         /* 支持手动 */
    AT_MMI_TEST_SUPPORT_AUTO,           /* 支持自动 */
    AT_MMI_TEST_SUPPORT_MANUAL_AUTO,    /* 支持手动和自动 */
    AT_MMI_TEST_SUPPORT_BUTT
};

enum AT_TCHRENABEL_SWITCH_ENUM
{
    AT_TCHRENABEL_SWITCH_CHARG_CLOSE = 0,      /* 关闭充电功能 */
    AT_TCHRENABEL_SWITCH_CHARG_OPEN,           /* 打开充电功能 */
    AT_TCHRENABEL_SWITCH_DISCHARG_CLOSE,       /* 关闭充电功能 */
    AT_TCHRENABEL_SWITCH_DISCHARG_OPEN,        /* 打开充电功能 */
    AT_TCHRENABEL_SWITCH_SUPPLY,               /* 打开补电功能 */
    AT_TCHRENABEL_SWITCH_BUTT
};

enum AT_TCHRENABEL_CHARGE_MODE_ENUM
{
    AT_TCHRENABEL_CHARGE_MODE_DEFAULT = 0,   /* 默认充电模式 */
    AT_TCHRENABEL_CHARGE_MODE_FAST,          /* 快充电模式 */
    AT_TCHRENABEL_CHARGE_MODE_TRICKLE,       /* 涓充电模式 */
    AT_TCHRENABEL_CHARGE_MODE_BUTT
};

enum AT_TSELRF_PATH_TYPE_ENUM
{
    AT_TSELRF_ALL     = 0,                /* 所有通路 */
    AT_TSELRF_GSM,                        /* GSM通路 */
    AT_TSELRF_WCDMA_MAIN,                /* WCDMA主集通路 */
    AT_TSELRF_WCDMA_DIV,                  /* WCDMA分集通路 */
    AT_TSELRF_CDMA_MAIN,                  /* CDMA主集通路 */
    AT_TSELRF_CDMA_DIV,                   /* CDMA分集通路 */
    AT_TSELRF_TDS,                         /* TDS通路 */
    AT_TSELRF_WIFI,                        /* WIFI通路 */
    AT_TSELRF_WIMAX,                       /* WIMAX通路 */
    AT_TSELRF_FDD_MAIN,                    /* LTE FDD主集通路 */
    AT_TSELRF_FDD_DIV,                     /* LTE FDD分集通路 */
    AT_TSELRF_FDD_MIMO,                    /* LTE FDD MIMO通路 */
    AT_TSELRF_TDD_MAIN,                    /* LTE TDD主集通路 */
    AT_TSELRF_TDD_DIV,                     /* LTE TDD分集通路 */
    AT_TSELRF_TDD_MIMO,                    /* LTE TDD MIMO通路 */
    AT_TSELRF_NAVIGATION,                  /* Navigation通路 */
    AT_TSELRF_BUTT
};
#define AT_TSELRF_PATH_TYPE_NUM           (16)
#define AT_TSELRF_EXIST                     (1)

enum AT_WLAN_MODE_ENUM
{
    AT_WLAN_MODE_CW = 0,      /* CW模式 */
    AT_WLAN_MODE_A,           /* 802.11 A模式 */
    AT_WLAN_MODE_B,           /* 802.11 B模式 */
    AT_WLAN_MODE_G,           /* 802.11 G模式 */
    AT_WLAN_MODE_N,           /* 802.11 N模式 */
    AT_WLAN_MODE_AC,          /* 802.11 AC模式 */
    AT_WLAN_MODE_BUTT
};

enum AT_FWAVE_ENUM
{
    AT_FWAVE_CONTINUES  = 0,      /* 单音调制信号 */
    AT_FWAVE_WCDMA,                /* WCDMA调制信号 */
    AT_FWAVE_GSM,                  /* GSM调制信号 */
    AT_FWAVE_EDGE,                 /* EDGE调制信号 */
    AT_FWAVE_WIFI,                 /* WIFI调制信号 */
    AT_FWAVE_LTE,                  /* LTE调制信号 */
    AT_FWAVE_CDMA,                  /* CDMA调制信号 */
    AT_FWAVE_TDS,                  /* TDS调制信号 */
    AT_FWAVE_BUTT
};

#endif
/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
/* 是否具有权限标志(控制DIAG/SHELL口的权限) */
extern AT_E5_RIGHT_FLAG_ENUM_U32        g_enATE5RightFlag;

/* 由于 g_stATDislogPwd 中的 DIAG口的状态要放入备份NV列表; 而密码不用备份
   故将 g_stATDislogPwd 中的密码废弃, 仅使用其中的 DIAG 口状态;
   重新定义NV项用来保存密码  */
extern VOS_INT8                         g_acATOpwordPwd[AT_OPWORD_PWD_LEN+1];
/* Added by f62575 for B050 Project, 2012-2-3, end   */


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


typedef struct
{
    VOS_UINT16                          usMinAdc;
    VOS_UINT16                          usMaxAdc;
}AT_TBAT_BATTERY_ADC_INFO_STRU;

#if (FEATURE_ON == MBB_COMMON)

enum AT_TBATDATA_TYPE_ENUM
{
    AT_TBATDATA_VOLTAGE,
    AT_TBATDATA_CURRENT,
    AT_TBATDATA_TYPE_BUTT
};


enum AT_TBATDATA_INDEX_ENUM
{
    AT_TBATDATA_INDEX0,
    AT_TBATDATA_INDEX1,
    AT_TBATDATA_INDEX_BUTT
};


typedef struct
{
    VOS_UINT16                          usMinAdc;
    VOS_UINT16                          usMaxAdc;
}AT_TBATDATA_BATTERY_ADC_INFO_STRU;

#endif


typedef struct
{
    VOS_UINT8                           aucwlKeys[AT_NV_WLKEY_LEN];
}AT_WIFISEC_WIWEP_INFO_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

VOS_UINT32 AT_SetTbatPara(VOS_UINT8 ucIndex);

VOS_UINT32 AT_QryTbatPara(VOS_UINT8 ucIndex);

VOS_UINT32 AT_SetPstandbyPara(VOS_UINT8 ucIndex);

VOS_UINT32 AT_SetWiwepPara(VOS_UINT8 ucIndex);

VOS_UINT32 AT_QryWiwepPara(VOS_UINT8 ucIndex);

VOS_UINT32 AT_TestWiwepPara(VOS_UINT8 ucIndex);
VOS_UINT32 AT_SetCmdlenPara(VOS_UINT8 ucIndex);

VOS_UINT32 AT_QryCmdlenPara(VOS_UINT8 ucIndex);

VOS_UINT32 AT_UpdateMacPara(
    VOS_UINT8                           aucMac[],
    VOS_UINT16                          usMacLength
);

VOS_UINT32 AT_SetTmodeAutoPowerOff(VOS_UINT8 ucIndex);

VOS_UINT32 AT_SetTseLrfPara(VOS_UINT8 ucIndex);

VOS_UINT32 AT_QryTseLrfPara(VOS_UINT8 ucIndex);

VOS_UINT32 AT_GetPhynumMac(VOS_UINT8 aucMac[]);

/* Added by f62575 for B050 Project, 2012-2-3, Begin   */
VOS_UINT32 AT_ExistSpecificPort(VOS_UINT8 ucPortType);

VOS_UINT32 AT_OpenDiagPort(VOS_VOID);
VOS_UINT32 AT_CloseDiagPort(VOS_VOID);
VOS_UINT32 AT_CheckSetPortRight(
    VOS_UINT8                           aucOldRewindPortStyle[],
    VOS_UINT8                           aucNewRewindPortStyle[]
);
/* Added by f62575 for B050 Project, 2012-2-3, end   */

VOS_UINT32 AT_TestHsicCmdPara(VOS_UINT8 ucIndex);

#if(FEATURE_ON == FEATURE_UE_MODE_TDS)
VOS_UINT32 At_TestTdsScalibPara(VOS_UINT8 ucIndex);
#endif

VOS_UINT32 AT_TestSimlockUnlockPara( VOS_UINT8 ucIndex );

VOS_UINT32 AT_SetNVReadPara(VOS_UINT8 ucClientId);
VOS_UINT32 AT_SetNVWritePara(VOS_UINT8 ucClientId);

#if (FEATURE_ON == MBB_COMMON)
VOS_UINT32 At_QryExtChargePara( VOS_UINT8 ucIndex );
VOS_UINT32 AT_SetWebSitePara( VOS_UINT8 ucIndex );
VOS_UINT32 AT_QryWebSitePara( VOS_UINT8 ucIndex );

VOS_UINT32 AT_SetWiFiPinPara( VOS_UINT8 ucIndex );
VOS_UINT32 AT_QryWiFiPinPara( VOS_UINT8 ucIndex );

VOS_UINT32 AT_SetWebUserPara( VOS_UINT8 ucIndex );
VOS_UINT32 AT_QryWebUserPara( VOS_UINT8 ucIndex );

VOS_UINT32 At_SetPortLockPara( VOS_UINT8 ucIndex );
VOS_UINT32 At_QryPortLockPara( VOS_UINT8 ucIndex );

VOS_UINT32 At_SetVersionPara( VOS_UINT8 ucIndex );

VOS_UINT32 AT_SetTbatDataPara( VOS_UINT8 ucIndex );
VOS_UINT32 At_QryTbatDataPara( VOS_UINT8 ucIndex );

VOS_UINT32 AT_SetAntennaPara( VOS_UINT8 ucIndex );
VOS_UINT32 AT_QryAntennaPara( VOS_UINT8 ucIndex );

VOS_UINT32 AT_QryWiFiPlatformPara( VOS_UINT8 ucIndex );

VOS_UINT32 At_SetVersionPara( VOS_UINT8 ucIndex );
VOS_UINT32 At_TestFlnaPara( VOS_UINT8 ucIndex );
VOS_UINT32 At_TestWiFiModePara( VOS_UINT8 ucIndex );
VOS_UINT32 At_TestWiFiBandPara( VOS_UINT8 ucIndex );
VOS_UINT32 At_TestTmmiPara( VOS_UINT8 ucIndex );
VOS_UINT32 At_TestSfm( VOS_UINT8 ucIndex );
VOS_UINT32 At_TestAntenna( VOS_UINT8 ucIndex );
#endif
#if(FEATURE_ON == MBB_OPERATOR_VDF)
VOS_UINT32 At_SetSkuType(VOS_UINT8 ucIndex);
VOS_UINT32 At_QrySkuType(VOS_UINT8 ucIndex);
VOS_UINT32 At_SetAccountType(VOS_UINT8 ucIndex);
VOS_UINT32 At_QryAccountType(VOS_UINT8 ucIndex);
#endif
#if(FEATURE_ON == MBB_CHG_BQ27510)
VOS_UINT32 At_QryCoulcurrentType(VOS_UINT8 ucIndex);
#endif

#if (FEATURE_ON == MBB_FEATURE_VSIM_HUK)
VOS_UINT32 AT_SetDieId(VOS_UINT8 ucIndex);
VOS_UINT32 AT_QryDieId(VOS_UINT8 ucIndex);
VOS_UINT32 AT_GetDieIdInfo(VOS_UINT8 ucIndex);
#endif /*FEATURE_ON == MBB_FEATURE_VSIM_HUK*/
VOS_UINT32 AT_QryFPllStatusPara(VOS_UINT8 ucIndex);
VOS_VOID At_RfPllStatusCnfProc(PHY_AT_RF_PLL_STATUS_CNF_STRU *pstMsg);
#if (FEATURE_ON == MBB_COMMON)
VOS_UINT32 At_SetSysDown(VOS_UINT8 ucIndex);
#endif /*FEATURE_ON == MBB_COMMON*/
VOS_UINT32 AT_QryFpowdetTPara(VOS_UINT8 ucIndex);
VOS_VOID At_RfFpowdetTCnfProc(PHY_AT_POWER_DET_CNF_STRU *pstMsg);
#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of AtDeviceCmd.h */
