#ifndef	_WebKitPluginHost_user_
#define	_WebKitPluginHost_user_

/* Module WebKitPluginHost */

#include <string.h>
#include <mach/ndr.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/notify.h>
#include <mach/mach_types.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/port.h>
	
/* BEGIN VOUCHER CODE */

#ifndef KERNEL
#if defined(__has_include)
#if __has_include(<mach/mig_voucher_support.h>)
#ifndef USING_VOUCHERS
#define USING_VOUCHERS
#endif
#ifndef __VOUCHER_FORWARD_TYPE_DECLS__
#define __VOUCHER_FORWARD_TYPE_DECLS__
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __VOUCHER_FOWARD_TYPE_DECLS_SINGLE_ATTR
#define __VOUCHER_FOWARD_TYPE_DECLS_SINGLE_ATTR __unsafe_indexable
#endif
	extern boolean_t voucher_mach_msg_set(mach_msg_header_t * msg) __attribute__((weak_import));
#ifdef __cplusplus
}
#endif
#endif // __VOUCHER_FORWARD_TYPE_DECLS__
#endif // __has_include(<mach/mach_voucher_types.h>)
#endif // __has_include
#endif // !KERNEL
	
/* END VOUCHER CODE */

	
/* BEGIN MIG_STRNCPY_ZEROFILL CODE */

#if defined(__has_include)
#if __has_include(<mach/mig_strncpy_zerofill_support.h>)
#ifndef USING_MIG_STRNCPY_ZEROFILL
#define USING_MIG_STRNCPY_ZEROFILL
#endif
#ifndef __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__
#define __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS_CSTRING_ATTR
#define __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS_CSTRING_COUNTEDBY_ATTR(C) __unsafe_indexable
#endif
	extern int mig_strncpy_zerofill(char * dest, const char * src, int len) __attribute__((weak_import));
#ifdef __cplusplus
}
#endif
#endif /* __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__ */
#endif /* __has_include(<mach/mig_strncpy_zerofill_support.h>) */
#endif /* __has_include */
	
/* END MIG_STRNCPY_ZEROFILL CODE */


#ifdef AUTOTEST
#ifndef FUNCTION_PTR_T
#define FUNCTION_PTR_T
typedef void (*function_ptr_t)(mach_port_t, char *, mach_msg_type_number_t);
typedef struct {
        char            * name;
        function_ptr_t  function;
} function_table_entry;
typedef function_table_entry   *function_table_t;
#endif /* FUNCTION_PTR_T */
#endif /* AUTOTEST */

#ifndef	WebKitPluginHost_MSG_COUNT
#define	WebKitPluginHost_MSG_COUNT	37
#endif	/* WebKitPluginHost_MSG_COUNT */

#include <Availability.h>
#include <mach/std_types.h>
#include <mach/mig.h>
#include <mach/mig.h>
#include <mach/mach_types.h>
#include <WebKitLegacy/WebKitPluginHostTypes.h>

#ifdef __BeforeMigUserHeader
__BeforeMigUserHeader
#endif /* __BeforeMigUserHeader */

#include <sys/cdefs.h>
__BEGIN_DECLS


/* Routine PHCheckInWithPluginHost */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHCheckInWithPluginHost
(
	mach_port_t pluginHostPort,
	plist_bytes_t options,
	mach_msg_type_number_t optionsCnt,
	mach_port_t clientPort,
	uint32_t clientPSNHigh,
	uint32_t clientPSNLow,
	mach_port_t renderPort,
	uint32_t *pluginHostPSNHigh,
	uint32_t *pluginHostPSNLow
);

/* SimpleRoutine PHInstantiatePlugin */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHInstantiatePlugin
(
	mach_port_t pluginHostPort,
	uint32_t requestID,
	plist_bytes_t options,
	mach_msg_type_number_t optionsCnt,
	uint32_t pluginID
);

/* SimpleRoutine PHResizePluginInstance */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHResizePluginInstance
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	double pluginX,
	double pluginY,
	double pluginWidth,
	double pluginHeight,
	double clipX,
	double clipY,
	double clipWidth,
	double clipHeight
);

/* SimpleRoutine PHPluginInstanceFocusChanged */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceFocusChanged
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	boolean_t hasFocus
);

/* SimpleRoutine PHPluginInstanceWindowFocusChanged */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceWindowFocusChanged
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	boolean_t hasFocus
);

/* SimpleRoutine PHPluginInstanceWindowFrameChanged */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceWindowFrameChanged
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	double x,
	double y,
	double width,
	double height,
	double maxScreenY
);

/* SimpleRoutine PHPluginInstanceMouseEvent */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceMouseEvent
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	double timestamp,
	uint32_t eventType,
	uint32_t modifierFlags,
	double pluginX,
	double pluginY,
	double screenX,
	double screenY,
	double maxScreenY,
	int32_t buttonNumber,
	int32_t clickCount,
	double deltaX,
	double deltaY,
	double deltaZ
);

/* SimpleRoutine PHPluginInstanceKeyboardEvent */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceKeyboardEvent
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	double timestamp,
	uint32_t eventType,
	uint32_t modifierFlags,
	data_t characters,
	mach_msg_type_number_t charactersCnt,
	data_t charactersIgnoringModifiers,
	mach_msg_type_number_t charactersIgnoringModifiersCnt,
	boolean_t isARepeat,
	uint16_t keyCode,
	uint8_t keyChar
);

/* SimpleRoutine PHPluginInstanceWheelEvent */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceWheelEvent
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	double timestamp,
	uint32_t modifierFlags,
	double pluginX,
	double pluginY,
	int32_t buttonNumber,
	double deltaX,
	double deltaY,
	double deltaZ
);

/* SimpleRoutine PHPluginInstanceInsertText */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceInsertText
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	data_t text,
	mach_msg_type_number_t textCnt
);

/* SimpleRoutine PHPluginInstanceStartTimers */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceStartTimers
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	boolean_t throttleTimers
);

/* SimpleRoutine PHPluginInstanceStopTimers */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceStopTimers
(
	mach_port_t pluginHostPort,
	uint32_t pluginID
);

/* SimpleRoutine PHPluginInstancePrint */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstancePrint
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t width,
	uint32_t height
);

/* SimpleRoutine PHDestroyPluginInstance */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHDestroyPluginInstance
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID
);

/* SimpleRoutine PHStartStream */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHStartStream
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t streamID,
	data_t responseURL,
	mach_msg_type_number_t responseURLCnt,
	int64_t expectedContentLength,
	double lastModifiedTimeInterval,
	data_t mimeType,
	mach_msg_type_number_t mimeTypeCnt,
	data_t headers,
	mach_msg_type_number_t headersCnt
);

/* SimpleRoutine PHStreamDidReceiveData */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHStreamDidReceiveData
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t streamID,
	data_t data,
	mach_msg_type_number_t dataCnt
);

/* SimpleRoutine PHStreamDidFinishLoading */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHStreamDidFinishLoading
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t streamID
);

/* SimpleRoutine PHStreamDidFail */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHStreamDidFail
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t streamID,
	int16_t reason
);

/* SimpleRoutine PHLoadURLNotify */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHLoadURLNotify
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	int16_t reason
);

/* SimpleRoutine PHGetScriptableNPObject */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHGetScriptableNPObject
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID
);

/* SimpleRoutine PHNPObjectHasProperty */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHNPObjectHasProperty
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t propertyName
);

/* SimpleRoutine PHNPObjectHasMethod */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHNPObjectHasMethod
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t methodName
);

/* SimpleRoutine PHNPObjectInvoke */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHNPObjectInvoke
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint32_t invokeType,
	uint64_t methodName,
	data_t arguments,
	mach_msg_type_number_t argumentsCnt
);

/* SimpleRoutine PHNPObjectHasInvokeDefaultMethod */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHNPObjectHasInvokeDefaultMethod
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID
);

/* SimpleRoutine PHNPObjectHasConstructMethod */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHNPObjectHasConstructMethod
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID
);

/* SimpleRoutine PHNPObjectGetProperty */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHNPObjectGetProperty
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t propertyName
);

/* SimpleRoutine PHNPObjectSetProperty */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHNPObjectSetProperty
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t propertyName,
	data_t value,
	mach_msg_type_number_t valueCnt
);

/* SimpleRoutine PHNPObjectRelease */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHNPObjectRelease
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t objectID
);

/* SimpleRoutine PHNPObjectEnumerate */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHNPObjectEnumerate
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID
);

/* SimpleRoutine PHBooleanReply */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHBooleanReply
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	boolean_t result
);

/* SimpleRoutine PHBooleanAndDataReply */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHBooleanAndDataReply
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	boolean_t returnValue,
	data_t result,
	mach_msg_type_number_t resultCnt
);

/* SimpleRoutine PHPluginInstanceDidDraw */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceDidDraw
(
	mach_port_t pluginHostPort,
	uint32_t pluginID
);

/* SimpleRoutine PHPluginInstancePrivateBrowsingModeDidChange */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstancePrivateBrowsingModeDidChange
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	boolean_t privateBrowsingEnabled
);

/* SimpleRoutine PHPluginInstanceSnapshot */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginInstanceSnapshot
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t width,
	uint32_t height
);

/* Routine PHCreatePluginMIMETypesPreferences */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHCreatePluginMIMETypesPreferences
(
	mach_port_t pluginHostPort
);

/* SimpleRoutine PHPluginShouldHostLayersInWindowServerChanged */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPHPluginShouldHostLayersInWindowServerChanged
(
	mach_port_t pluginHostPort,
	uint32_t pluginID,
	boolean_t shouldHostLayersInWindowServer
);

__END_DECLS

/********************** Caution **************************/
/* The following data types should be used to calculate  */
/* maximum message sizes only. The actual message may be */
/* smaller, and the position of the arguments within the */
/* message layout may vary from what is presented here.  */
/* For example, if any of the arguments are variable-    */
/* sized, and less than the maximum is sent, the data    */
/* will be packed tight in the actual message to reduce  */
/* the presence of holes.                                */
/********************** Caution **************************/

/* typedefs for all requests */

#ifndef __Request__WebKitPluginHost_subsystem__defined
#define __Request__WebKitPluginHost_subsystem__defined

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_ool_descriptor_t options;
		mach_msg_port_descriptor_t clientPort;
		mach_msg_port_descriptor_t renderPort;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		mach_msg_type_number_t optionsCnt;
		uint32_t clientPSNHigh;
		uint32_t clientPSNLow;
	} __Request__PHCheckInWithPluginHost_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_ool_descriptor_t options;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t requestID;
		mach_msg_type_number_t optionsCnt;
		uint32_t pluginID;
	} __Request__PHInstantiatePlugin_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		double pluginX;
		double pluginY;
		double pluginWidth;
		double pluginHeight;
		double clipX;
		double clipY;
		double clipWidth;
		double clipHeight;
	} __Request__PHResizePluginInstance_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		boolean_t hasFocus;
	} __Request__PHPluginInstanceFocusChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		boolean_t hasFocus;
	} __Request__PHPluginInstanceWindowFocusChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		double x;
		double y;
		double width;
		double height;
		double maxScreenY;
	} __Request__PHPluginInstanceWindowFrameChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		double timestamp;
		uint32_t eventType;
		uint32_t modifierFlags;
		double pluginX;
		double pluginY;
		double screenX;
		double screenY;
		double maxScreenY;
		int32_t buttonNumber;
		int32_t clickCount;
		double deltaX;
		double deltaY;
		double deltaZ;
	} __Request__PHPluginInstanceMouseEvent_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_ool_descriptor_t characters;
		mach_msg_ool_descriptor_t charactersIgnoringModifiers;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		double timestamp;
		uint32_t eventType;
		uint32_t modifierFlags;
		mach_msg_type_number_t charactersCnt;
		mach_msg_type_number_t charactersIgnoringModifiersCnt;
		boolean_t isARepeat;
		uint16_t keyCode;
		char keyCodePad[2];
		uint8_t keyChar;
		char keyCharPad[3];
	} __Request__PHPluginInstanceKeyboardEvent_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		double timestamp;
		uint32_t modifierFlags;
		double pluginX;
		double pluginY;
		int32_t buttonNumber;
		double deltaX;
		double deltaY;
		double deltaZ;
	} __Request__PHPluginInstanceWheelEvent_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_ool_descriptor_t text;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		mach_msg_type_number_t textCnt;
	} __Request__PHPluginInstanceInsertText_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		boolean_t throttleTimers;
	} __Request__PHPluginInstanceStartTimers_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
	} __Request__PHPluginInstanceStopTimers_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t width;
		uint32_t height;
	} __Request__PHPluginInstancePrint_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
	} __Request__PHDestroyPluginInstance_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_ool_descriptor_t responseURL;
		mach_msg_ool_descriptor_t mimeType;
		mach_msg_ool_descriptor_t headers;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t streamID;
		mach_msg_type_number_t responseURLCnt;
		int64_t expectedContentLength;
		double lastModifiedTimeInterval;
		mach_msg_type_number_t mimeTypeCnt;
		mach_msg_type_number_t headersCnt;
	} __Request__PHStartStream_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_ool_descriptor_t data;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t streamID;
		mach_msg_type_number_t dataCnt;
	} __Request__PHStreamDidReceiveData_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t streamID;
	} __Request__PHStreamDidFinishLoading_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t streamID;
		int16_t reason;
		char reasonPad[2];
	} __Request__PHStreamDidFail_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		int16_t reason;
		char reasonPad[2];
	} __Request__PHLoadURLNotify_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
	} __Request__PHGetScriptableNPObject_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t objectID;
		uint64_t propertyName;
	} __Request__PHNPObjectHasProperty_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t objectID;
		uint64_t methodName;
	} __Request__PHNPObjectHasMethod_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_ool_descriptor_t arguments;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t objectID;
		uint32_t invokeType;
		uint64_t methodName;
		mach_msg_type_number_t argumentsCnt;
	} __Request__PHNPObjectInvoke_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t objectID;
	} __Request__PHNPObjectHasInvokeDefaultMethod_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t objectID;
	} __Request__PHNPObjectHasConstructMethod_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t objectID;
		uint64_t propertyName;
	} __Request__PHNPObjectGetProperty_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_ool_descriptor_t value;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t objectID;
		uint64_t propertyName;
		mach_msg_type_number_t valueCnt;
	} __Request__PHNPObjectSetProperty_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t objectID;
	} __Request__PHNPObjectRelease_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t objectID;
	} __Request__PHNPObjectEnumerate_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		boolean_t result;
	} __Request__PHBooleanReply_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_ool_descriptor_t result;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		boolean_t returnValue;
		mach_msg_type_number_t resultCnt;
	} __Request__PHBooleanAndDataReply_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
	} __Request__PHPluginInstanceDidDraw_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		boolean_t privateBrowsingEnabled;
	} __Request__PHPluginInstancePrivateBrowsingModeDidChange_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t width;
		uint32_t height;
	} __Request__PHPluginInstanceSnapshot_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
	} __Request__PHCreatePluginMIMETypesPreferences_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint32_t pluginID;
		boolean_t shouldHostLayersInWindowServer;
	} __Request__PHPluginShouldHostLayersInWindowServerChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif
#endif /* !__Request__WebKitPluginHost_subsystem__defined */

/* union of all requests */

#ifndef __RequestUnion___WKWebKitPluginHost_subsystem__defined
#define __RequestUnion___WKWebKitPluginHost_subsystem__defined
union __RequestUnion___WKWebKitPluginHost_subsystem {
	__Request__PHCheckInWithPluginHost_t Request__WKPHCheckInWithPluginHost;
	__Request__PHInstantiatePlugin_t Request__WKPHInstantiatePlugin;
	__Request__PHResizePluginInstance_t Request__WKPHResizePluginInstance;
	__Request__PHPluginInstanceFocusChanged_t Request__WKPHPluginInstanceFocusChanged;
	__Request__PHPluginInstanceWindowFocusChanged_t Request__WKPHPluginInstanceWindowFocusChanged;
	__Request__PHPluginInstanceWindowFrameChanged_t Request__WKPHPluginInstanceWindowFrameChanged;
	__Request__PHPluginInstanceMouseEvent_t Request__WKPHPluginInstanceMouseEvent;
	__Request__PHPluginInstanceKeyboardEvent_t Request__WKPHPluginInstanceKeyboardEvent;
	__Request__PHPluginInstanceWheelEvent_t Request__WKPHPluginInstanceWheelEvent;
	__Request__PHPluginInstanceInsertText_t Request__WKPHPluginInstanceInsertText;
	__Request__PHPluginInstanceStartTimers_t Request__WKPHPluginInstanceStartTimers;
	__Request__PHPluginInstanceStopTimers_t Request__WKPHPluginInstanceStopTimers;
	__Request__PHPluginInstancePrint_t Request__WKPHPluginInstancePrint;
	__Request__PHDestroyPluginInstance_t Request__WKPHDestroyPluginInstance;
	__Request__PHStartStream_t Request__WKPHStartStream;
	__Request__PHStreamDidReceiveData_t Request__WKPHStreamDidReceiveData;
	__Request__PHStreamDidFinishLoading_t Request__WKPHStreamDidFinishLoading;
	__Request__PHStreamDidFail_t Request__WKPHStreamDidFail;
	__Request__PHLoadURLNotify_t Request__WKPHLoadURLNotify;
	__Request__PHGetScriptableNPObject_t Request__WKPHGetScriptableNPObject;
	__Request__PHNPObjectHasProperty_t Request__WKPHNPObjectHasProperty;
	__Request__PHNPObjectHasMethod_t Request__WKPHNPObjectHasMethod;
	__Request__PHNPObjectInvoke_t Request__WKPHNPObjectInvoke;
	__Request__PHNPObjectHasInvokeDefaultMethod_t Request__WKPHNPObjectHasInvokeDefaultMethod;
	__Request__PHNPObjectHasConstructMethod_t Request__WKPHNPObjectHasConstructMethod;
	__Request__PHNPObjectGetProperty_t Request__WKPHNPObjectGetProperty;
	__Request__PHNPObjectSetProperty_t Request__WKPHNPObjectSetProperty;
	__Request__PHNPObjectRelease_t Request__WKPHNPObjectRelease;
	__Request__PHNPObjectEnumerate_t Request__WKPHNPObjectEnumerate;
	__Request__PHBooleanReply_t Request__WKPHBooleanReply;
	__Request__PHBooleanAndDataReply_t Request__WKPHBooleanAndDataReply;
	__Request__PHPluginInstanceDidDraw_t Request__WKPHPluginInstanceDidDraw;
	__Request__PHPluginInstancePrivateBrowsingModeDidChange_t Request__WKPHPluginInstancePrivateBrowsingModeDidChange;
	__Request__PHPluginInstanceSnapshot_t Request__WKPHPluginInstanceSnapshot;
	__Request__PHCreatePluginMIMETypesPreferences_t Request__WKPHCreatePluginMIMETypesPreferences;
	__Request__PHPluginShouldHostLayersInWindowServerChanged_t Request__WKPHPluginShouldHostLayersInWindowServerChanged;
};
#endif /* !__RequestUnion___WKWebKitPluginHost_subsystem__defined */
/* typedefs for all replies */

#ifndef __Reply__WebKitPluginHost_subsystem__defined
#define __Reply__WebKitPluginHost_subsystem__defined

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
		uint32_t pluginHostPSNHigh;
		uint32_t pluginHostPSNLow;
	} __Reply__PHCheckInWithPluginHost_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHInstantiatePlugin_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHResizePluginInstance_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceFocusChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceWindowFocusChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceWindowFrameChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceMouseEvent_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceKeyboardEvent_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceWheelEvent_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceInsertText_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceStartTimers_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceStopTimers_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstancePrint_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHDestroyPluginInstance_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHStartStream_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHStreamDidReceiveData_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHStreamDidFinishLoading_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHStreamDidFail_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHLoadURLNotify_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHGetScriptableNPObject_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHNPObjectHasProperty_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHNPObjectHasMethod_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHNPObjectInvoke_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHNPObjectHasInvokeDefaultMethod_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHNPObjectHasConstructMethod_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHNPObjectGetProperty_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHNPObjectSetProperty_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHNPObjectRelease_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHNPObjectEnumerate_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHBooleanReply_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHBooleanAndDataReply_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceDidDraw_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstancePrivateBrowsingModeDidChange_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginInstanceSnapshot_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHCreatePluginMIMETypesPreferences_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PHPluginShouldHostLayersInWindowServerChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif
#endif /* !__Reply__WebKitPluginHost_subsystem__defined */

/* union of all replies */

#ifndef __ReplyUnion___WKWebKitPluginHost_subsystem__defined
#define __ReplyUnion___WKWebKitPluginHost_subsystem__defined
union __ReplyUnion___WKWebKitPluginHost_subsystem {
	__Reply__PHCheckInWithPluginHost_t Reply__WKPHCheckInWithPluginHost;
	__Reply__PHInstantiatePlugin_t Reply__WKPHInstantiatePlugin;
	__Reply__PHResizePluginInstance_t Reply__WKPHResizePluginInstance;
	__Reply__PHPluginInstanceFocusChanged_t Reply__WKPHPluginInstanceFocusChanged;
	__Reply__PHPluginInstanceWindowFocusChanged_t Reply__WKPHPluginInstanceWindowFocusChanged;
	__Reply__PHPluginInstanceWindowFrameChanged_t Reply__WKPHPluginInstanceWindowFrameChanged;
	__Reply__PHPluginInstanceMouseEvent_t Reply__WKPHPluginInstanceMouseEvent;
	__Reply__PHPluginInstanceKeyboardEvent_t Reply__WKPHPluginInstanceKeyboardEvent;
	__Reply__PHPluginInstanceWheelEvent_t Reply__WKPHPluginInstanceWheelEvent;
	__Reply__PHPluginInstanceInsertText_t Reply__WKPHPluginInstanceInsertText;
	__Reply__PHPluginInstanceStartTimers_t Reply__WKPHPluginInstanceStartTimers;
	__Reply__PHPluginInstanceStopTimers_t Reply__WKPHPluginInstanceStopTimers;
	__Reply__PHPluginInstancePrint_t Reply__WKPHPluginInstancePrint;
	__Reply__PHDestroyPluginInstance_t Reply__WKPHDestroyPluginInstance;
	__Reply__PHStartStream_t Reply__WKPHStartStream;
	__Reply__PHStreamDidReceiveData_t Reply__WKPHStreamDidReceiveData;
	__Reply__PHStreamDidFinishLoading_t Reply__WKPHStreamDidFinishLoading;
	__Reply__PHStreamDidFail_t Reply__WKPHStreamDidFail;
	__Reply__PHLoadURLNotify_t Reply__WKPHLoadURLNotify;
	__Reply__PHGetScriptableNPObject_t Reply__WKPHGetScriptableNPObject;
	__Reply__PHNPObjectHasProperty_t Reply__WKPHNPObjectHasProperty;
	__Reply__PHNPObjectHasMethod_t Reply__WKPHNPObjectHasMethod;
	__Reply__PHNPObjectInvoke_t Reply__WKPHNPObjectInvoke;
	__Reply__PHNPObjectHasInvokeDefaultMethod_t Reply__WKPHNPObjectHasInvokeDefaultMethod;
	__Reply__PHNPObjectHasConstructMethod_t Reply__WKPHNPObjectHasConstructMethod;
	__Reply__PHNPObjectGetProperty_t Reply__WKPHNPObjectGetProperty;
	__Reply__PHNPObjectSetProperty_t Reply__WKPHNPObjectSetProperty;
	__Reply__PHNPObjectRelease_t Reply__WKPHNPObjectRelease;
	__Reply__PHNPObjectEnumerate_t Reply__WKPHNPObjectEnumerate;
	__Reply__PHBooleanReply_t Reply__WKPHBooleanReply;
	__Reply__PHBooleanAndDataReply_t Reply__WKPHBooleanAndDataReply;
	__Reply__PHPluginInstanceDidDraw_t Reply__WKPHPluginInstanceDidDraw;
	__Reply__PHPluginInstancePrivateBrowsingModeDidChange_t Reply__WKPHPluginInstancePrivateBrowsingModeDidChange;
	__Reply__PHPluginInstanceSnapshot_t Reply__WKPHPluginInstanceSnapshot;
	__Reply__PHCreatePluginMIMETypesPreferences_t Reply__WKPHCreatePluginMIMETypesPreferences;
	__Reply__PHPluginShouldHostLayersInWindowServerChanged_t Reply__WKPHPluginShouldHostLayersInWindowServerChanged;
};
#endif /* !__RequestUnion___WKWebKitPluginHost_subsystem__defined */

#ifndef subsystem_to_name_map_WebKitPluginHost
#define subsystem_to_name_map_WebKitPluginHost \
    { "PHCheckInWithPluginHost", 300 },\
    { "PHInstantiatePlugin", 301 },\
    { "PHResizePluginInstance", 302 },\
    { "PHPluginInstanceFocusChanged", 303 },\
    { "PHPluginInstanceWindowFocusChanged", 304 },\
    { "PHPluginInstanceWindowFrameChanged", 305 },\
    { "PHPluginInstanceMouseEvent", 306 },\
    { "PHPluginInstanceKeyboardEvent", 307 },\
    { "PHPluginInstanceWheelEvent", 308 },\
    { "PHPluginInstanceInsertText", 309 },\
    { "PHPluginInstanceStartTimers", 310 },\
    { "PHPluginInstanceStopTimers", 311 },\
    { "PHPluginInstancePrint", 312 },\
    { "PHDestroyPluginInstance", 313 },\
    { "PHStartStream", 315 },\
    { "PHStreamDidReceiveData", 316 },\
    { "PHStreamDidFinishLoading", 317 },\
    { "PHStreamDidFail", 318 },\
    { "PHLoadURLNotify", 319 },\
    { "PHGetScriptableNPObject", 320 },\
    { "PHNPObjectHasProperty", 321 },\
    { "PHNPObjectHasMethod", 322 },\
    { "PHNPObjectInvoke", 323 },\
    { "PHNPObjectHasInvokeDefaultMethod", 324 },\
    { "PHNPObjectHasConstructMethod", 325 },\
    { "PHNPObjectGetProperty", 326 },\
    { "PHNPObjectSetProperty", 327 },\
    { "PHNPObjectRelease", 328 },\
    { "PHNPObjectEnumerate", 329 },\
    { "PHBooleanReply", 330 },\
    { "PHBooleanAndDataReply", 331 },\
    { "PHPluginInstanceDidDraw", 332 },\
    { "PHPluginInstancePrivateBrowsingModeDidChange", 333 },\
    { "PHPluginInstanceSnapshot", 334 },\
    { "PHCreatePluginMIMETypesPreferences", 335 },\
    { "PHPluginShouldHostLayersInWindowServerChanged", 336 }
#endif

#ifdef __AfterMigUserHeader
__AfterMigUserHeader
#endif /* __AfterMigUserHeader */

#endif	 /* _WebKitPluginHost_user_ */
