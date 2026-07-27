#ifndef	_WebKitPluginClient_user_
#define	_WebKitPluginClient_user_

/* Module WebKitPluginClient */

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

#ifndef	WebKitPluginClient_MSG_COUNT
#define	WebKitPluginClient_MSG_COUNT	37
#endif	/* WebKitPluginClient_MSG_COUNT */

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


/* SimpleRoutine PCStatusText */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCStatusText
(
	mach_port_t clientPort,
	uint32_t pluginID,
	data_t text,
	mach_msg_type_number_t textCnt
);

/* Routine PCLoadURL */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCLoadURL
(
	mach_port_t clientPort,
	uint32_t pluginID,
	data_t url,
	mach_msg_type_number_t urlCnt,
	data_t target,
	mach_msg_type_number_t targetCnt,
	data_t postData,
	mach_msg_type_number_t postDataCnt,
	uint32_t flags,
	uint16_t *resultCode,
	uint32_t *requestID
);

/* SimpleRoutine PCCancelLoadURL */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCCancelLoadURL
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t streamID,
	int16_t reason
);

/* SimpleRoutine PCInvalidateRect */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCInvalidateRect
(
	mach_port_t clientPort,
	uint32_t pluginID,
	double x,
	double y,
	double width,
	double height
);

/* Routine PCGetCookies */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCGetCookies
(
	mach_port_t clientPort,
	uint32_t pluginID,
	data_t url,
	mach_msg_type_number_t urlCnt,
	boolean_t *returnValue,
	data_t *cookies,
	mach_msg_type_number_t *cookiesCnt
);

/* Routine PCSetCookies */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCSetCookies
(
	mach_port_t clientPort,
	uint32_t pluginID,
	data_t url,
	mach_msg_type_number_t urlCnt,
	data_t cookies,
	mach_msg_type_number_t cookiesCnt,
	boolean_t *returnValue
);

/* Routine PCGetProxy */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCGetProxy
(
	mach_port_t clientPort,
	uint32_t pluginID,
	data_t url,
	mach_msg_type_number_t urlCnt,
	boolean_t *returnValue,
	data_t *proxy,
	mach_msg_type_number_t *proxyCnt
);

/* Routine PCGetAuthenticationInfo */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCGetAuthenticationInfo
(
	mach_port_t clientPort,
	uint32_t pluginID,
	data_t protocol,
	mach_msg_type_number_t protocolCnt,
	data_t host,
	mach_msg_type_number_t hostCnt,
	uint32_t port,
	data_t scheme,
	mach_msg_type_number_t schemeCnt,
	data_t realm,
	mach_msg_type_number_t realmCnt,
	boolean_t *returnValue,
	data_t *username,
	mach_msg_type_number_t *usernameCnt,
	data_t *password,
	mach_msg_type_number_t *passwordCnt
);

/* Routine PCConvertPoint */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCConvertPoint
(
	mach_port_t clientPort,
	uint32_t pluginID,
	double sourceX,
	double sourceY,
	uint32_t sourceSpace,
	uint32_t destSpace,
	boolean_t *returnValue,
	double *destX,
	double *destY
);

/* Routine PCGetStringIdentifier */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCGetStringIdentifier
(
	mach_port_t clientPort,
	data_t name,
	mach_msg_type_number_t nameCnt,
	uint64_t *identifier
);

/* Routine PCGetIntIdentifier */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCGetIntIdentifier
(
	mach_port_t clientPort,
	int32_t value,
	uint64_t *identifier
);

/* Routine PCGetWindowNPObject */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCGetWindowNPObject
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t *objectID
);

/* Routine PCGetPluginElementNPObject */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCGetPluginElementNPObject
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t *objectID
);

/* Routine PCForgetBrowserObject */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCForgetBrowserObject
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t objectID
);

/* SimpleRoutine PCEvaluate */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCEvaluate
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	data_t script,
	mach_msg_type_number_t scriptCnt,
	boolean_t allowPopups
);

/* SimpleRoutine PCInvoke */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCInvoke
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t methodNameIdentifier,
	data_t arguments,
	mach_msg_type_number_t argumentsCnt
);

/* SimpleRoutine PCInvokeDefault */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCInvokeDefault
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	data_t arguments,
	mach_msg_type_number_t argumentsCnt
);

/* Routine PCConstruct */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCConstruct
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t objectID,
	data_t arguments,
	mach_msg_type_number_t argumentsCnt,
	boolean_t *returnValue,
	data_t *result,
	mach_msg_type_number_t *resultCnt
);

/* SimpleRoutine PCGetProperty */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCGetProperty
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t propertyNameIdentifier
);

/* SimpleRoutine PCSetProperty */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCSetProperty
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t propertyNameIdentifier,
	data_t value,
	mach_msg_type_number_t valueCnt
);

/* SimpleRoutine PCRemoveProperty */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCRemoveProperty
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t propertyNameIdentifier
);

/* SimpleRoutine PCHasProperty */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCHasProperty
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t propertyNameIdentifier
);

/* SimpleRoutine PCHasMethod */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCHasMethod
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID,
	uint64_t methodNameIdentifier
);

/* Routine PCIdentifierInfo */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCIdentifierInfo
(
	mach_port_t clientPort,
	uint64_t identifier,
	data_t *info,
	mach_msg_type_number_t *infoCnt
);

/* SimpleRoutine PCEnumerate */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCEnumerate
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID
);

/* SimpleRoutine PCSetMenuBarVisible */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCSetMenuBarVisible
(
	mach_port_t clientPort,
	boolean_t visible
);

/* SimpleRoutine PCSetModal */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCSetModal
(
	mach_port_t clientPort,
	boolean_t modal
);

/* SimpleRoutine PCInstantiatePluginReply */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCInstantiatePluginReply
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	kern_return_t result,
	uint32_t renderContextID,
	uint32_t rendererType
);

/* SimpleRoutine PCGetScriptableNPObjectReply */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCGetScriptableNPObjectReply
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	uint32_t objectID
);

/* SimpleRoutine PCBooleanReply */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCBooleanReply
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	boolean_t result
);

/* SimpleRoutine PCBooleanAndDataReply */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCBooleanAndDataReply
(
	mach_port_t clientPort,
	uint32_t pluginID,
	uint32_t requestID,
	boolean_t returnValue,
	data_t result,
	mach_msg_type_number_t resultCnt
);

/* SimpleRoutine PCSetFullscreenWindowIsShowing */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCSetFullscreenWindowIsShowing
(
	mach_port_t clientPort,
	boolean_t isShowing
);

/* SimpleRoutine PCSetException */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCSetException
(
	mach_port_t clientPort,
	data_t message,
	mach_msg_type_number_t messageCnt
);

/* SimpleRoutine PCLayerHostingModeChanged */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t _WKPCLayerHostingModeChanged
(
	mach_port_t clientPort,
	uint32_t pluginID,
	boolean_t hostsLayersInWindowServer,
	uint32_t renderContextID
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

#ifndef __Request__WebKitPluginClient_subsystem__defined
#define __Request__WebKitPluginClient_subsystem__defined

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
	} __Request__PCStatusText_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t url;
		mach_msg_ool_descriptor_t target;
		mach_msg_ool_descriptor_t postData;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		mach_msg_type_number_t urlCnt;
		mach_msg_type_number_t targetCnt;
		mach_msg_type_number_t postDataCnt;
		uint32_t flags;
	} __Request__PCLoadURL_t __attribute__((unused));
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
	} __Request__PCCancelLoadURL_t __attribute__((unused));
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
	} __Request__PCInvalidateRect_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t url;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		mach_msg_type_number_t urlCnt;
	} __Request__PCGetCookies_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t url;
		mach_msg_ool_descriptor_t cookies;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		mach_msg_type_number_t urlCnt;
		mach_msg_type_number_t cookiesCnt;
	} __Request__PCSetCookies_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t url;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		mach_msg_type_number_t urlCnt;
	} __Request__PCGetProxy_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t protocol;
		mach_msg_ool_descriptor_t host;
		mach_msg_ool_descriptor_t scheme;
		mach_msg_ool_descriptor_t realm;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		mach_msg_type_number_t protocolCnt;
		mach_msg_type_number_t hostCnt;
		uint32_t port;
		mach_msg_type_number_t schemeCnt;
		mach_msg_type_number_t realmCnt;
	} __Request__PCGetAuthenticationInfo_t __attribute__((unused));
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
		double sourceX;
		double sourceY;
		uint32_t sourceSpace;
		uint32_t destSpace;
	} __Request__PCConvertPoint_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t name;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		mach_msg_type_number_t nameCnt;
	} __Request__PCGetStringIdentifier_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		int32_t value;
	} __Request__PCGetIntIdentifier_t __attribute__((unused));
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
	} __Request__PCGetWindowNPObject_t __attribute__((unused));
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
	} __Request__PCGetPluginElementNPObject_t __attribute__((unused));
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
	} __Request__PCForgetBrowserObject_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t script;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		uint32_t pluginID;
		uint32_t requestID;
		uint32_t objectID;
		mach_msg_type_number_t scriptCnt;
		boolean_t allowPopups;
	} __Request__PCEvaluate_t __attribute__((unused));
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
		uint64_t methodNameIdentifier;
		mach_msg_type_number_t argumentsCnt;
	} __Request__PCInvoke_t __attribute__((unused));
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
		mach_msg_type_number_t argumentsCnt;
	} __Request__PCInvokeDefault_t __attribute__((unused));
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
		uint32_t objectID;
		mach_msg_type_number_t argumentsCnt;
	} __Request__PCConstruct_t __attribute__((unused));
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
		uint64_t propertyNameIdentifier;
	} __Request__PCGetProperty_t __attribute__((unused));
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
		uint64_t propertyNameIdentifier;
		mach_msg_type_number_t valueCnt;
	} __Request__PCSetProperty_t __attribute__((unused));
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
		uint64_t propertyNameIdentifier;
	} __Request__PCRemoveProperty_t __attribute__((unused));
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
		uint64_t propertyNameIdentifier;
	} __Request__PCHasProperty_t __attribute__((unused));
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
		uint64_t methodNameIdentifier;
	} __Request__PCHasMethod_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		uint64_t identifier;
	} __Request__PCIdentifierInfo_t __attribute__((unused));
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
	} __Request__PCEnumerate_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		boolean_t visible;
	} __Request__PCSetMenuBarVisible_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		boolean_t modal;
	} __Request__PCSetModal_t __attribute__((unused));
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
		kern_return_t result;
		uint32_t renderContextID;
		uint32_t rendererType;
	} __Request__PCInstantiatePluginReply_t __attribute__((unused));
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
	} __Request__PCGetScriptableNPObjectReply_t __attribute__((unused));
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
	} __Request__PCBooleanReply_t __attribute__((unused));
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
	} __Request__PCBooleanAndDataReply_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		boolean_t isShowing;
	} __Request__PCSetFullscreenWindowIsShowing_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t message;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		mach_msg_type_number_t messageCnt;
	} __Request__PCSetException_t __attribute__((unused));
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
		boolean_t hostsLayersInWindowServer;
		uint32_t renderContextID;
	} __Request__PCLayerHostingModeChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif
#endif /* !__Request__WebKitPluginClient_subsystem__defined */

/* union of all requests */

#ifndef __RequestUnion___WKWebKitPluginClient_subsystem__defined
#define __RequestUnion___WKWebKitPluginClient_subsystem__defined
union __RequestUnion___WKWebKitPluginClient_subsystem {
	__Request__PCStatusText_t Request__WKPCStatusText;
	__Request__PCLoadURL_t Request__WKPCLoadURL;
	__Request__PCCancelLoadURL_t Request__WKPCCancelLoadURL;
	__Request__PCInvalidateRect_t Request__WKPCInvalidateRect;
	__Request__PCGetCookies_t Request__WKPCGetCookies;
	__Request__PCSetCookies_t Request__WKPCSetCookies;
	__Request__PCGetProxy_t Request__WKPCGetProxy;
	__Request__PCGetAuthenticationInfo_t Request__WKPCGetAuthenticationInfo;
	__Request__PCConvertPoint_t Request__WKPCConvertPoint;
	__Request__PCGetStringIdentifier_t Request__WKPCGetStringIdentifier;
	__Request__PCGetIntIdentifier_t Request__WKPCGetIntIdentifier;
	__Request__PCGetWindowNPObject_t Request__WKPCGetWindowNPObject;
	__Request__PCGetPluginElementNPObject_t Request__WKPCGetPluginElementNPObject;
	__Request__PCForgetBrowserObject_t Request__WKPCForgetBrowserObject;
	__Request__PCEvaluate_t Request__WKPCEvaluate;
	__Request__PCInvoke_t Request__WKPCInvoke;
	__Request__PCInvokeDefault_t Request__WKPCInvokeDefault;
	__Request__PCConstruct_t Request__WKPCConstruct;
	__Request__PCGetProperty_t Request__WKPCGetProperty;
	__Request__PCSetProperty_t Request__WKPCSetProperty;
	__Request__PCRemoveProperty_t Request__WKPCRemoveProperty;
	__Request__PCHasProperty_t Request__WKPCHasProperty;
	__Request__PCHasMethod_t Request__WKPCHasMethod;
	__Request__PCIdentifierInfo_t Request__WKPCIdentifierInfo;
	__Request__PCEnumerate_t Request__WKPCEnumerate;
	__Request__PCSetMenuBarVisible_t Request__WKPCSetMenuBarVisible;
	__Request__PCSetModal_t Request__WKPCSetModal;
	__Request__PCInstantiatePluginReply_t Request__WKPCInstantiatePluginReply;
	__Request__PCGetScriptableNPObjectReply_t Request__WKPCGetScriptableNPObjectReply;
	__Request__PCBooleanReply_t Request__WKPCBooleanReply;
	__Request__PCBooleanAndDataReply_t Request__WKPCBooleanAndDataReply;
	__Request__PCSetFullscreenWindowIsShowing_t Request__WKPCSetFullscreenWindowIsShowing;
	__Request__PCSetException_t Request__WKPCSetException;
	__Request__PCLayerHostingModeChanged_t Request__WKPCLayerHostingModeChanged;
};
#endif /* !__RequestUnion___WKWebKitPluginClient_subsystem__defined */
/* typedefs for all replies */

#ifndef __Reply__WebKitPluginClient_subsystem__defined
#define __Reply__WebKitPluginClient_subsystem__defined

#ifdef  __MigPackStructs
#pragma pack(push, 4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__PCStatusText_t __attribute__((unused));
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
		uint16_t resultCode;
		char resultCodePad[2];
		uint32_t requestID;
	} __Reply__PCLoadURL_t __attribute__((unused));
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
	} __Reply__PCCancelLoadURL_t __attribute__((unused));
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
	} __Reply__PCInvalidateRect_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t cookies;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		boolean_t returnValue;
		mach_msg_type_number_t cookiesCnt;
	} __Reply__PCGetCookies_t __attribute__((unused));
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
		boolean_t returnValue;
	} __Reply__PCSetCookies_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t proxy;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		boolean_t returnValue;
		mach_msg_type_number_t proxyCnt;
	} __Reply__PCGetProxy_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t username;
		mach_msg_ool_descriptor_t password;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		boolean_t returnValue;
		mach_msg_type_number_t usernameCnt;
		mach_msg_type_number_t passwordCnt;
	} __Reply__PCGetAuthenticationInfo_t __attribute__((unused));
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
		boolean_t returnValue;
		double destX;
		double destY;
	} __Reply__PCConvertPoint_t __attribute__((unused));
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
		uint64_t identifier;
	} __Reply__PCGetStringIdentifier_t __attribute__((unused));
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
		uint64_t identifier;
	} __Reply__PCGetIntIdentifier_t __attribute__((unused));
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
		uint32_t objectID;
	} __Reply__PCGetWindowNPObject_t __attribute__((unused));
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
		uint32_t objectID;
	} __Reply__PCGetPluginElementNPObject_t __attribute__((unused));
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
	} __Reply__PCForgetBrowserObject_t __attribute__((unused));
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
	} __Reply__PCEvaluate_t __attribute__((unused));
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
	} __Reply__PCInvoke_t __attribute__((unused));
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
	} __Reply__PCInvokeDefault_t __attribute__((unused));
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
		boolean_t returnValue;
		mach_msg_type_number_t resultCnt;
	} __Reply__PCConstruct_t __attribute__((unused));
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
	} __Reply__PCGetProperty_t __attribute__((unused));
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
	} __Reply__PCSetProperty_t __attribute__((unused));
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
	} __Reply__PCRemoveProperty_t __attribute__((unused));
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
	} __Reply__PCHasProperty_t __attribute__((unused));
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
	} __Reply__PCHasMethod_t __attribute__((unused));
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
		mach_msg_ool_descriptor_t info;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		mach_msg_type_number_t infoCnt;
	} __Reply__PCIdentifierInfo_t __attribute__((unused));
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
	} __Reply__PCEnumerate_t __attribute__((unused));
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
	} __Reply__PCSetMenuBarVisible_t __attribute__((unused));
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
	} __Reply__PCSetModal_t __attribute__((unused));
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
	} __Reply__PCInstantiatePluginReply_t __attribute__((unused));
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
	} __Reply__PCGetScriptableNPObjectReply_t __attribute__((unused));
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
	} __Reply__PCBooleanReply_t __attribute__((unused));
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
	} __Reply__PCBooleanAndDataReply_t __attribute__((unused));
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
	} __Reply__PCSetFullscreenWindowIsShowing_t __attribute__((unused));
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
	} __Reply__PCSetException_t __attribute__((unused));
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
	} __Reply__PCLayerHostingModeChanged_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack(pop)
#endif
#endif /* !__Reply__WebKitPluginClient_subsystem__defined */

/* union of all replies */

#ifndef __ReplyUnion___WKWebKitPluginClient_subsystem__defined
#define __ReplyUnion___WKWebKitPluginClient_subsystem__defined
union __ReplyUnion___WKWebKitPluginClient_subsystem {
	__Reply__PCStatusText_t Reply__WKPCStatusText;
	__Reply__PCLoadURL_t Reply__WKPCLoadURL;
	__Reply__PCCancelLoadURL_t Reply__WKPCCancelLoadURL;
	__Reply__PCInvalidateRect_t Reply__WKPCInvalidateRect;
	__Reply__PCGetCookies_t Reply__WKPCGetCookies;
	__Reply__PCSetCookies_t Reply__WKPCSetCookies;
	__Reply__PCGetProxy_t Reply__WKPCGetProxy;
	__Reply__PCGetAuthenticationInfo_t Reply__WKPCGetAuthenticationInfo;
	__Reply__PCConvertPoint_t Reply__WKPCConvertPoint;
	__Reply__PCGetStringIdentifier_t Reply__WKPCGetStringIdentifier;
	__Reply__PCGetIntIdentifier_t Reply__WKPCGetIntIdentifier;
	__Reply__PCGetWindowNPObject_t Reply__WKPCGetWindowNPObject;
	__Reply__PCGetPluginElementNPObject_t Reply__WKPCGetPluginElementNPObject;
	__Reply__PCForgetBrowserObject_t Reply__WKPCForgetBrowserObject;
	__Reply__PCEvaluate_t Reply__WKPCEvaluate;
	__Reply__PCInvoke_t Reply__WKPCInvoke;
	__Reply__PCInvokeDefault_t Reply__WKPCInvokeDefault;
	__Reply__PCConstruct_t Reply__WKPCConstruct;
	__Reply__PCGetProperty_t Reply__WKPCGetProperty;
	__Reply__PCSetProperty_t Reply__WKPCSetProperty;
	__Reply__PCRemoveProperty_t Reply__WKPCRemoveProperty;
	__Reply__PCHasProperty_t Reply__WKPCHasProperty;
	__Reply__PCHasMethod_t Reply__WKPCHasMethod;
	__Reply__PCIdentifierInfo_t Reply__WKPCIdentifierInfo;
	__Reply__PCEnumerate_t Reply__WKPCEnumerate;
	__Reply__PCSetMenuBarVisible_t Reply__WKPCSetMenuBarVisible;
	__Reply__PCSetModal_t Reply__WKPCSetModal;
	__Reply__PCInstantiatePluginReply_t Reply__WKPCInstantiatePluginReply;
	__Reply__PCGetScriptableNPObjectReply_t Reply__WKPCGetScriptableNPObjectReply;
	__Reply__PCBooleanReply_t Reply__WKPCBooleanReply;
	__Reply__PCBooleanAndDataReply_t Reply__WKPCBooleanAndDataReply;
	__Reply__PCSetFullscreenWindowIsShowing_t Reply__WKPCSetFullscreenWindowIsShowing;
	__Reply__PCSetException_t Reply__WKPCSetException;
	__Reply__PCLayerHostingModeChanged_t Reply__WKPCLayerHostingModeChanged;
};
#endif /* !__RequestUnion___WKWebKitPluginClient_subsystem__defined */

#ifndef subsystem_to_name_map_WebKitPluginClient
#define subsystem_to_name_map_WebKitPluginClient \
    { "PCStatusText", 300 },\
    { "PCLoadURL", 301 },\
    { "PCCancelLoadURL", 302 },\
    { "PCInvalidateRect", 303 },\
    { "PCGetCookies", 304 },\
    { "PCSetCookies", 305 },\
    { "PCGetProxy", 306 },\
    { "PCGetAuthenticationInfo", 307 },\
    { "PCConvertPoint", 308 },\
    { "PCGetStringIdentifier", 309 },\
    { "PCGetIntIdentifier", 310 },\
    { "PCGetWindowNPObject", 311 },\
    { "PCGetPluginElementNPObject", 312 },\
    { "PCForgetBrowserObject", 313 },\
    { "PCEvaluate", 314 },\
    { "PCInvoke", 315 },\
    { "PCInvokeDefault", 316 },\
    { "PCConstruct", 317 },\
    { "PCGetProperty", 318 },\
    { "PCSetProperty", 319 },\
    { "PCRemoveProperty", 320 },\
    { "PCHasProperty", 321 },\
    { "PCHasMethod", 322 },\
    { "PCIdentifierInfo", 323 },\
    { "PCEnumerate", 324 },\
    { "PCSetMenuBarVisible", 325 },\
    { "PCSetModal", 326 },\
    { "PCInstantiatePluginReply", 330 },\
    { "PCGetScriptableNPObjectReply", 331 },\
    { "PCBooleanReply", 332 },\
    { "PCBooleanAndDataReply", 333 },\
    { "PCSetFullscreenWindowIsShowing", 334 },\
    { "PCSetException", 335 },\
    { "PCLayerHostingModeChanged", 336 }
#endif

#ifdef __AfterMigUserHeader
__AfterMigUserHeader
#endif /* __AfterMigUserHeader */

#endif	 /* _WebKitPluginClient_user_ */
