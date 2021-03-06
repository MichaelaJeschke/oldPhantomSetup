
/************************************************/
/*                                              */
/*  Header-File for GSV-Functions in MEGSV.DLL  */
/*  Version 1.42.1                              */
/*                                              */
/*  Copyright (C) Dr. Holger Kabelitz 1999-2008 */
/*  All rights reserved.                        */
/*                                              */
/*  Dr. Holger Kabelitz                         */
/*  D-13507 Berlin                              */
/*  Germany                                     */
/*                                              */
/************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef WINAPI
#define WINAPI __stdcall
#endif

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif

#define GSV_OK 0
#define GSV_ERROR -1
#define GSV_TRUE 1

#define GSV_MEMORY_OK GSV_OK
#define GSV_MEMORY_OK_FUNCTION 0x01
#define GSV_MEMORY_ERROR_ABORT 0xFF
#define GSV_MEMORY_ERROR_SEVERE 0x10
#define GSV_MEMORY_ERROR_NA 0x11
#define GSV_MEMORY_ERROR_DATA 0x12
#define GSV_ACTEX_FLAG_BAUDRATE 0x00000001L
#define GSV_ACTEX_FLAG_WAKEUP 0x00000002L
#define GSV_ACTEX_FLAG_HANDSHAKE 0x00000004L
#define GSV_CANSET_MANUFACTURER_ID 0x00
#define GSV_CANSET_COMMAND_ID 0x01
#define GSV_CANSET_MESSAGE_ID 0x02
#define GSV_CANSET_BAUD 0x7E
#define GSV_CANSET_FLAGS 0x7F
#define GSV_CANSET_20B 0x80

#define ME_INTSTAT_FLUSH 0x00000001L

typedef int (CALLBACK *MEreservedExecProc)(int, void *);

typedef struct GSV_ACTIVATE_EXTENDED_
{
	long actex_size;
	long actex_buffersize;
	long actex_flags;
	long actex_baudrate;
} GSV_ACTIVATE_EXTENDED;

#define GSV_ACTEX_SIZE ((long)sizeof(GSV_ACTIVATE_EXTENDED))

long WINAPI GSVversion(void);

int WINAPI GSVmodel(int no);

int WINAPI MEreservedExec(int no, int code, MEreservedExecProc rexpro, void *arg);

void *WINAPI MErequestEvent(int no);

long WINAPI GSVrevision(void);

int WINAPI GSVabortActivate(int no);

long WINAPI GSVgetLocalBaudRate(int no);

int WINAPI GSVactivateExtended(int no, GSV_ACTIVATE_EXTENDED *actex);

int WINAPI GSVactivate(int no, long buffersize);

void WINAPI GSVrelease(int no);

int WINAPI GSVinitialize(int no);

int WINAPI GSVflushBuffer(int no);

long WINAPI MEinternalStatus(int no);

int WINAPI GSVgetValues(int *no, int n);

int WINAPI MEsendBytesToDevices(int *no, int n, void *pbuf, int count);

int WINAPI MEsendBytes(int no, void *pbuf, int count);

int WINAPI GSVreceived(int no);

int WINAPI GSVread(int no, double *ad);

int WINAPI GSVreadMultiple(int no, double *ad, int count, int *valsread);

int WINAPI GSVreadStatus(int no, double *ad, unsigned char *ps);

int WINAPI GSVreadStatusMultiple(int no, double *ad, unsigned char *ps, int count, int *valsread);

int WINAPI GSVcopyMemory(int no, int dc, int addr, void *buffer);

int WINAPI GSVcheckMemory(int no, int dc, int addr, unsigned char *buffer);

int WINAPI MEasynchronousSendBytes(int no, void *pbuf, int count, void *handle);

int WINAPI MEsynchronizeSendBytes(int no);

int WINAPI GSVgetOptionsCode(int no);

int WINAPI GSVgetOptionsExtension3(int no);

int WINAPI GSVgetOptionsLinear(int no);

int WINAPI GSVgetOptionsExtension21(int no);

int WINAPI GSVgetOptionsSleepMode(int no);

int WINAPI GSVgetOptionsCommandTest(int no);

int WINAPI GSVisBipol(int no);

double WINAPI GSVgetFreq(int no);

int WINAPI GSVgetGain(int no);

int WINAPI GSVgetChannel(int no);

int WINAPI GSVsetModeSI(int no, int msi);

int WINAPI GSVgetModeSI(int no);

int WINAPI GSVsetModeText(int no, int mt);

int WINAPI GSVgetModeText(int no);

int WINAPI GSVsetModeMax(int no, int mx);

int WINAPI GSVgetModeMax(int no);

int WINAPI GSVsetModeLog(int no, int lg);

int WINAPI GSVgetModeLog(int no);

int WINAPI GSVsetModeWindow(int no, int win);

int WINAPI GSVgetModeWindow(int no);

int WINAPI GSVsetModeAverage(int no, int avg);

int WINAPI GSVgetModeAverage(int no);

int WINAPI GSVsetModeLinear(int no, int lin);

int WINAPI GSVgetModeLinear(int no);

int WINAPI MEsetModeLock(int no, int slock, char *id);

int WINAPI GSVgetModeLock(int no);

int WINAPI GSVhasLCD(int no);

int WINAPI GSVhasADC(int no);

int WINAPI GSVhasUII(int no);

int WINAPI GSVisSI(int no);

int WINAPI GSVisWL(int no);

int WINAPI GSVhasAF(int no);

int WINAPI GSVsetBridgeType(int no, int bt);

int WINAPI GSVgetBridgeType(int no);

int WINAPI GSVsetBridgeInternal(int no, int bi);

int WINAPI GSVgetBridgeInternal(int no);

int WINAPI GSVresetStatus(int no);

long WINAPI GSVgetScale(int no);

long WINAPI GSVgetZero(int no);

long WINAPI GSVgetControl(int no);

long WINAPI GSVgetOffset(int no);

int WINAPI GSVwriteScale(int no, long scale);

int WINAPI GSVwriteZero(int no, long zero);

int WINAPI GSVwriteControl(int no, long control);

int WINAPI GSVwriteOffset(int no, long offset);

int WINAPI GSVgetAll(int no, int pos);

int WINAPI GSVsaveAll(int no, int pos);

int WINAPI GSVsetCal(int no);

int WINAPI GSVsetZero(int no);

int WINAPI GSVsetScale(int no);

int WINAPI GSVsetOffset(int no);

int WINAPI GSVDispSetUnit(int no, int dispunit);

int WINAPI GSVDispSetNorm(int no, double norm);

int WINAPI GSVDispSetDPoint(int no, int dpoint);

int WINAPI GSVsetFreq(int no, double freq);

int WINAPI GSVsetGain(int no, int gain);

int WINAPI GSVsetBipolar(int no);

int WINAPI GSVsetUnipolar(int no);

int WINAPI MEsetCal(int no, double cal);

double WINAPI MEgetCal(int no);

int WINAPI MEsendID(int no, char *id);

double WINAPI GSVDispGetNorm(int no);

int WINAPI GSVDispGetUnit(int no);

int WINAPI GSVDispGetDPoint(int no);

int WINAPI GSVswitch(int no, int on);

int WINAPI MEwriteSerialNo(int no, char *number);

int WINAPI GSVgetSerialNo(int no, char *number);

int WINAPI GSVsetThreshold(int no, double thon, double thoff);

int WINAPI GSVgetThreshold(int no, double *thon, double *thoff);

int WINAPI GSVsetChannel(int no, int channel);

int WINAPI GSVstopTransmit(int no);

int WINAPI GSVstartTransmit(int no);

int WINAPI GSVclearBuffer(int no);

int WINAPI GSVsetMode(int no, int mode);

int WINAPI GSVgetMode(int no);

int WINAPI MEwriteEquipment(int no, int equip);

int WINAPI GSVgetEquipment(int no);

long WINAPI GSVfirmwareVersion(int no);

int WINAPI GSVsetGageFactor(int no, double gf);

double WINAPI GSVgetGageFactor(int no);

int WINAPI GSVsetPoisson(int no, double poiss);

double WINAPI GSVgetPoisson(int no);

int WINAPI GSVsetBridge(int no, int br);

int WINAPI GSVgetBridge(int no);

int WINAPI MEwriteRange(int no, double range);

double WINAPI GSVgetRange(int no);

int WINAPI MEsetOffsetWait(int no, double ow);

double WINAPI MEgetOffsetWait(int no);

long WINAPI GSVgetOptions(int no);

int WINAPI MEwriteOptions(int no, long options);

int WINAPI GSVreadMemory(int no, int addr, void *buffer, int *status);

int WINAPI GSVwriteMemory(int no, int addr, int check, int value, int *status);

double WINAPI GSVgetMemoryWait(int no);

int WINAPI GSVgetValue(int no);

int WINAPI GSVclearMaxValue(int no);

int WINAPI GSVDispSetDigits(int no, int digits);

int WINAPI GSVDispGetDigits(int no);

int WINAPI GSVunlockUII(int no);

int WINAPI GSVlockUII(int no);

int WINAPI GSVgetLastError(int no);

int WINAPI GSVsetSecondThreshold(int no, double thon, double thoff);

int WINAPI GSVgetSecondThreshold(int no, double *thon, double *thoff);

int WINAPI GSVgetDeviceType(int no);

int WINAPI GSVDispCalcNorm(int no);

int WINAPI GSVsetTxMode(int no, int txmode);

int WINAPI GSVgetTxMode(int no);

int WINAPI GSVsetBaud(int no, int baud);

int WINAPI GSVgetBaud(int no);

int WINAPI GSVsetSlowRate(int no, long secs);

long WINAPI GSVgetSlowRate(int no);

int WINAPI GSVsetSpecialMode(int no, int smode);

int WINAPI GSVgetSpecialMode(int no);

int WINAPI GSVwriteSamplingRate(int no, double freq, int factor);

int WINAPI GSVreadSamplingRate(int no, double *freq, int *factor);

int WINAPI GSVsetCanSetting(int no, int stype, int val);

int WINAPI GSVgetCanSetting(int no, int stype);

int WINAPI GSVsetAnalogFilter(int no, double freq);

double WINAPI GSVgetAnalogFilter(int no);

int WINAPI GSVisCommandAvailable(int no, int cmd1, int cmd2);

int WINAPI GSVsetNoiseCutThreshold(int no, double thr);

double WINAPI GSVgetNoiseCutThreshold(int no);

int WINAPI GSVsetAutoZeroCounter(int no, long ctr);

long WINAPI GSVgetAutoZeroCounter(int no);

int WINAPI GSVsetUserTextChar(int no, int addr, unsigned char c);

int WINAPI GSVgetTxModeConfig(int no);

int WINAPI GSVsetTxModeTransmit4(int no, int t4);

int WINAPI GSVgetTxModeTransmit4(int no);

int WINAPI GSVsetTxModeRepeat3(int no, int r3);

int WINAPI GSVgetTxModeRepeat3(int no);

int WINAPI GSVsetTxModeTransmit5(int no, int t5);

int WINAPI GSVgetTxModeTransmit5(int no);

int WINAPI GSVsetTxModeReadOnly(int no, int ro);

int WINAPI GSVgetTxModeReadOnly(int no);

int WINAPI GSVsetSpecialModeSlow(int no, int slow);

int WINAPI GSVgetSpecialModeSlow(int no);

int WINAPI GSVsetSpecialModeAverage(int no, int avg);

int WINAPI GSVgetSpecialModeAverage(int no);

int WINAPI GSVsetSpecialModeFilter(int no, int flt);

int WINAPI GSVgetSpecialModeFilter(int no);

int WINAPI GSVsetSpecialModeMax(int no, int mx);

int WINAPI GSVgetSpecialModeMax(int no);

int WINAPI GSVsetSpecialModeFilterAuto(int no, int fltauto);

int WINAPI GSVgetSpecialModeFilterAuto(int no);

int WINAPI GSVsetSpecialModeFilterOrder5(int no, int fltord2);

int WINAPI GSVgetSpecialModeFilterOrder5(int no);

int WINAPI GSVsetSpecialModeSleep(int no, int sleep);

int WINAPI GSVgetSpecialModeSleep(int no);

int WINAPI GSVsetSpecialModeAutoZero(int no, int autozero);

int WINAPI GSVgetSpecialModeAutoZero(int no);

int WINAPI GSVsetSpecialModeNoiseCut(int no, int noisecut);

int WINAPI GSVgetSpecialModeNoiseCut(int no);

double WINAPI GSVreadSamplingFrequency(int no);

int WINAPI GSVreadSamplingFactor(int no);

int WINAPI GSVsetBaudRate(int no, long baudrate);

long WINAPI GSVgetBaudRate(int no);

int WINAPI GSVsetCanBaudRate(int no, long baudrate);

long WINAPI GSVgetCanBaudRate(int no);

int WINAPI GSVisSamplingFrequencyProtocolLinear(int no);

int WINAPI GSVisSamplingFactorProtocolLinear(int no);

int WINAPI GSVsetCanID(int no, int idtype, long id);

long WINAPI GSVgetCanID(int no, int idtype);

int WINAPI GSVsetCanBaud(int no, int baud);

int WINAPI GSVgetCanBaud(int no);

int WINAPI GSVisCanAvailable(int no);

int WINAPI GSVsetCanActive(int no, int active);

int WINAPI GSVgetCanActive(int no);

int WINAPI GSVsetCanMode20B(int no, int mode20b);

int WINAPI GSVgetCanMode20B(int no);

int WINAPI GSVsetUserText(int no, char *txt);

#ifdef __cplusplus
}
#endif
