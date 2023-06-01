#pragma once

#include <QMap>

namespace Qth
{

	/*
	const QMap<Qt::WidgetAttribute, QString> g_widgetAttributeList = {
		{ Qt::WA_Disabled, WA_Disabled}
		{ Qt::WA_UnderMouse = 1,
		{ Qt::WA_MouseTracking = 2,
#if QT_DEPRECATED_SINCE(5, 15) // commented as such since 4.5.1
		{ Qt::WA_ContentsPropagated Q_DECL_ENUMERATOR_DEPRECATED = 3,
#endif
		{ Qt::WA_OpaquePaintEvent = 4,
#if QT_DEPRECATED_SINCE(5, 14)
		{ Qt::WA_NoBackground Q_DECL_ENUMERATOR_DEPRECATED = WA_OpaquePaintEvent,
#endif
		{ Qt::WA_StaticContents = 5,
		{ Qt::WA_LaidOut = 7,
		{ Qt::WA_PaintOnScreen = 8,
		{ Qt::WA_NoSystemBackground = 9,
		{ Qt::WA_UpdatesDisabled = 10,
		{ Qt::WA_Mapped = 11,
#if QT_DEPRECATED_SINCE(5, 14)
		{ Qt::WA_MacNoClickThrough Q_DECL_ENUMERATOR_DEPRECATED = 12,
#endif
		{ Qt::WA_InputMethodEnabled = 14,
		{ Qt::WA_WState_Visible = 15,
		{ Qt::WA_WState_Hidden = 16,
		
		{ Qt::WA_ForceDisabled = 32,
		{ Qt::WA_KeyCompression = 33,
		{ Qt::WA_PendingMoveEvent = 34,
		{ Qt::WA_PendingResizeEvent = 35,
		{ Qt::WA_SetPalette = 36,
		{ Qt::WA_SetFont = 37,
		{ Qt::WA_SetCursor = 38,
		{ Qt::WA_NoChildEventsFromChildren = 39,
		{ Qt::WA_WindowModified = 41,
		{ Qt::WA_Resized = 42,
		{ Qt::WA_Moved = 43,
		{ Qt::WA_PendingUpdate = 44,
		{ Qt::WA_InvalidSize = 45,
#if QT_DEPRECATED_SINCE(5, 14)
		{ Qt::WA_MacBrushedMetal Q_DECL_ENUMERATOR_DEPRECATED = 46,
		{ Qt::WA_MacMetalStyle Q_DECL_ENUMERATOR_DEPRECATED = 46,
#endif
		{ Qt::WA_CustomWhatsThis = 47,
		{ Qt::WA_LayoutOnEntireRect = 48,
		{ Qt::WA_OutsideWSRange = 49,
		{ Qt::WA_GrabbedShortcut = 50,
		{ Qt::WA_TransparentForMouseEvents = 51,
		{ Qt::WA_PaintUnclipped = 52,
		{ Qt::WA_SetWindowIcon = 53,
		{ Qt::WA_NoMouseReplay = 54,
		{ Qt::WA_DeleteOnClose = 55,
		{ Qt::WA_RightToLeft = 56,
		{ Qt::WA_SetLayoutDirection = 57,
		{ Qt::WA_NoChildEventsForParent = 58,
		{ Qt::WA_ForceUpdatesDisabled = 59,
		
		{ Qt::WA_WState_Created = 60,
		{ Qt::WA_WState_CompressKeys = 61,
		{ Qt::WA_WState_InPaintEvent = 62,
		{ Qt::WA_WState_Reparented = 63,
		{ Qt::WA_WState_ConfigPending = 64,
		{ Qt::WA_WState_Polished = 66,
#if QT_DEPRECATED_SINCE(5, 15) // commented as such in 4.5.1
		{ Qt::WA_WState_DND Q_DECL_ENUMERATOR_DEPRECATED = 67,
#endif
		{ Qt::A_WState_OwnSizePolicy = 68,
		{ Qt::A_WState_ExplicitShowHide = 69,
		
		{ Qt::WA_ShowModal = 70, // ## deprecated since since 4.5.1 but still in use :-(
		{ Qt::WA_MouseNoMask = 71,
		{ Qt::WA_GroupLeader = 72, // ## deprecated since since 4.5.1 but still in use :-(
		{ Qt::WA_NoMousePropagation = 73, // for now, might go away.
		{ Qt::WA_Hover = 74,
		{ Qt::WA_InputMethodTransparent = 75, // Don't reset IM when user clicks on this (for virtual keyboards on embedded)
		{ Qt::WA_QuitOnClose = 76,
		
		{ Qt::WA_KeyboardFocusChange = 77,
		
		{ Qt::WA_AcceptDrops = 78,
		{ Qt::WA_DropSiteRegistered = 79, // internal
#if QT_DEPRECATED_SINCE(5, 15) // commented as such since 4.5.1
		{ Qt::WA_ForceAcceptDrops Q_DECL_ENUMERATOR_DEPRECATED_X("WA_ForceAcceptDrops is deprecated. Use WA_DropSiteRegistered instead") = WA_DropSiteRegistered,
#endif

		{ Qt::WA_WindowPropagation = 80,

		WA_NoX11EventCompression = 81,
		WA_TintedBackground = 82,
		WA_X11OpenGLOverlay = 83,
		WA_AlwaysShowToolTips = 84,
		WA_MacOpaqueSizeGrip = 85,
		WA_SetStyle = 86,

		WA_SetLocale = 87,
		WA_MacShowFocusRect = 88,

		WA_MacNormalSize = 89,  // Mac only
		WA_MacSmallSize = 90,   // Mac only
		WA_MacMiniSize = 91,    // Mac only

		WA_LayoutUsesWidgetRect = 92,
		WA_StyledBackground = 93, // internal
#if QT_DEPRECATED_SINCE(5, 14)
		WA_MSWindowsUseDirect3D Q_DECL_ENUMERATOR_DEPRECATED = 94,
#endif
		WA_CanHostQMdiSubWindowTitleBar = 95, // Internal

		WA_MacAlwaysShowToolWindow = 96, // Mac only

		WA_StyleSheet = 97, // internal

		WA_ShowWithoutActivating = 98,

		WA_X11BypassTransientForHint = 99,

		WA_NativeWindow = 100,
		WA_DontCreateNativeAncestors = 101,

		WA_MacVariableSize = 102,    // Mac only

		WA_DontShowOnScreen = 103,

		// window types from http://standards.freedesktop.org/wm-spec/
		WA_X11NetWmWindowTypeDesktop = 104,
		WA_X11NetWmWindowTypeDock = 105,
		WA_X11NetWmWindowTypeToolBar = 106,
		WA_X11NetWmWindowTypeMenu = 107,
		WA_X11NetWmWindowTypeUtility = 108,
		WA_X11NetWmWindowTypeSplash = 109,
		WA_X11NetWmWindowTypeDialog = 110,
		WA_X11NetWmWindowTypeDropDownMenu = 111,
		WA_X11NetWmWindowTypePopupMenu = 112,
		WA_X11NetWmWindowTypeToolTip = 113,
		WA_X11NetWmWindowTypeNotification = 114,
		WA_X11NetWmWindowTypeCombo = 115,
		WA_X11NetWmWindowTypeDND = 116,
#if QT_DEPRECATED_SINCE(5, 14)
		WA_MacFrameworkScaled Q_DECL_ENUMERATOR_DEPRECATED = 117,
#endif
		WA_SetWindowModality = 118,
		WA_WState_WindowOpacitySet = 119, // internal
		WA_TranslucentBackground = 120,

		WA_AcceptTouchEvents = 121,
		WA_WState_AcceptedTouchBeginEvent = 122,
		WA_TouchPadAcceptSingleTouchEvents = 123,

		WA_X11DoNotAcceptFocus = 126,
		WA_MacNoShadow = 127,

		WA_AlwaysStackOnTop = 128,

		WA_TabletTracking = 129,

		WA_ContentsMarginsRespectsSafeArea = 130,

		WA_StyleSheetTarget = 131,

		// Add new attributes before this line
		WA_AttributeCount
	}*/
}