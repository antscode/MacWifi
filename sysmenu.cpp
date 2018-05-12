/*
	Copyright 2018 Anthony Super.
	System Menu INIT code based on "SysMenu" shell project by Matt Slot, circa 1995.
*/

#include <Gestalt.h>
#include <Icons.h>
#include <Traps.h>
#include <Resources.h>
#include <MacMemory.h>
#include <Sound.h>
#include <string.h>

#include "sysmenu.h"
#include "Prefs.h"
#include "Util.h"
#include "WifiShared.h"

extern "C"
{
	#include "Retro68Runtime.h"

	GlobalsRec glob;
	WifiData sharedData;

	void _start()
	{
		RETRO68_RELOCATE();

		Handle myHandle;

		// Setup Global Data
		SetZone(SystemZone());

		// Save memory location of shared data so the background app can find it
		InitSharedData();
		SaveSharedMemoryLocation();

		// Setup Handlers
		glob.saveInsertMenu = (InsertMenuProcPtr)ApplyTrapPatch(_InsertMenu, (ProcPtr)Patched_InsertMenu);
		glob.saveDrawMenuBar = (DrawMenuProcPtr)ApplyTrapPatch(_DrawMenuBar, (ProcPtr)Patched_DrawMenuBar);
		glob.saveMenuSelect = (MenuSelectProcPtr)ApplyTrapPatch(_MenuSelect, (ProcPtr)Patched_MenuSelect);
		glob.saveSystemMenu = (SystemMenuProcPtr)ApplyTrapPatch(_SystemMenu, (ProcPtr)Patched_SystemMenu);

		glob.mHdl = 0;
		glob.menuID = 0;
		glob.menuIcon = 0;
		glob.sysMenus = (short **)NewHandle(0);

		// Load up our icon suite for the menu "title" (if the iconID is non-zero)
		// Get *all* icons in case the desired size (12x12) isn't available.
		if (kPrefIconID && !GetIconSuite(&myHandle, kPrefIconID, svAllAvailableData) &&
			!ForEachIconDo(myHandle, svAllAvailableData, NewIconActionProc((IconActionProcPtr)DetachIcons), 0))
			glob.menuIcon = myHandle;

		// Remember the location of our home rsrc file, in case we want to 
		// load an dialog or other resource at run-time. Note: this doesn't provide
		// any guarantee that the file will be there later, so check your errors!
		if (CurResFileAsFSSpec(&glob.homeFile)) glob.homeFile.vRefNum = 0;

		SetZone(ApplicationZone());

		// Detach the INIT resource to prevent it from being purged
		Handle h = GetResource('INIT', 128);
		DetachResource(h);
	}

	void InitSharedData()
	{
		sharedData.Status = ScanRequest;
		sharedData.Error = false;
		sharedData.UpdateUI = true;
	}

	void SaveSharedMemoryLocation()
	{
		MemLocHandle memHandle = (MemLocHandle)Get1Resource('memr', 128);

		if (memHandle != nil)
		{
			**memHandle = (int)&sharedData;
			ChangedResource((Handle)memHandle);
		}
		else
		{
			memHandle = MemLocHandle(NewHandle(sizeof(MemLoc)));

			**memHandle = (int)&sharedData;
			AddResource((Handle)memHandle, 'memr', 128, "\pshared memory location");
		}

		UpdateResFile(CurResFile());
		ReleaseResource((Handle)memHandle);
	}

	short CurResFileAsFSSpec(FSSpec *fileSpec) {
		short i, err = 0;
		Str63 textBuff;
		FCBPBRec fcbPB;

		for (i = 0; i<sizeof(FSSpec); ((char *)fileSpec)[i++] = 0);

		fcbPB.ioCompletion = 0;
		fcbPB.ioFCBIndx = 0;
		fcbPB.ioVRefNum = 0;
		fcbPB.ioRefNum = CurResFile();
		fcbPB.ioNamePtr = textBuff;
		if (err = PBGetFCBInfoSync(&fcbPB)) return(err);

		fileSpec->vRefNum = fcbPB.ioFCBVRefNum;
		fileSpec->parID = fcbPB.ioFCBParID;
		BlockMove(textBuff, fileSpec->name, textBuff[0] + 1);

		return(err);
	}

	ProcPtr ApplyTrapPatch(short trap, ProcPtr patchPtr)
	{
		ProcPtr trapPtr;

		if (!patchPtr) return(0);

		trapPtr = (ProcPtr)NGetTrapAddress(trap, (trap & 0x0800) ? ToolTrap : OSTrap);
		NSetTrapAddress(patchPtr, trap, (trap & 0x0800) ? ToolTrap : OSTrap);
		return(trapPtr);
	}

	pascal short DetachIcons(long iconType, Handle *iconHdl, void *data)
	{
		short err = 0;

		if (*iconHdl)
		{
			DetachResource(*iconHdl);
			HNoPurge(*iconHdl);
			err = ResError();
		}

		return(err);
	}

	pascal void Patched_InsertMenu(MenuHandle mHdl, short beforeID) {
		short menuID = (*mHdl)->menuID;
		InsertMenuProcPtr proc;

		// As suggested to me, we need a reliable way to track what menus 
		// have been installed, so we can pick a unique ID for ours. This
		// function simply tracks the ID's to a global variable.
		if (!glob.menuID && glob.sysMenus && (menuID < 0))
			PtrAndHand((Ptr)&menuID, (Handle)glob.sysMenus, sizeof(menuID));

		proc = glob.saveInsertMenu;
		(*proc) (mHdl, beforeID);
	}

	pascal void Patched_DrawMenuBar()
	{
		short i, found;
		DrawMenuProcPtr proc;
		Str32 textBuff;
		THz saveZone;

		if (!glob.mHdl)
		{
			// On the first chance, we create and install the new menu 
			// (We need to wait for the Menu Mgr to be initialized to setup)

			// Let's not create the menu in this App's heap
			saveZone = GetZone();
			SetZone(SystemZone());

			// Search for an unused Menu ID, then install the menu. We have tracked
			// the InsertMenu calls, so now we pick an ID not in that list. When done
			// we set the gMenuID and dispose (and clear) the gSysMenus handle.
			for (glob.menuID = kPrefMenuID; ; glob.menuID++)
			{
				i = GetHandleSize((Handle)glob.sysMenus) / sizeof(**glob.sysMenus);
				for (found = 0; !found && (i > 0);
					found = ((*glob.sysMenus)[--i] == glob.menuID));
				if (!found)
				{
					DisposeHandle((Handle)glob.sysMenus);
					glob.sysMenus = 0;
					break;
				}
			}

			// Setup our new menu with its (optional) cool icon
			if (glob.menuIcon)
			{
				textBuff[0] = 5;
				textBuff[1] = 1;
				BlockMove(&glob.menuIcon, textBuff + 2, sizeof(glob.menuIcon));
			}
			else BlockMove("\pMenu", textBuff, sizeof("\pMenu"));
			glob.mHdl = NewMenu(glob.menuID, textBuff);

			InsertMenu(glob.mHdl, 0);

			SetZone(saveZone);
		}

		proc = glob.saveDrawMenuBar;
		(*proc) ();
	}

	pascal long Patched_MenuSelect(Point where)
	{
		long value;
		MenuSelectProcPtr proc;
		THz saveZone;
		short itemCount;

		// This function is called when the user first clicks in the menubar.
		// Its an ideal place to update the contents of your menu on the fly
		// to reflect current settings or conditions.
		if (glob.mHdl && sharedData.UpdateUI)
		{
			saveZone = GetZone();
			SetZone(SystemZone());

			itemCount = CountMItems(glob.mHdl);

			// Clear existing menu items
			while (itemCount > 0) {
				DeleteMenuItem(glob.mHdl, 1);
				itemCount--;
			}

			// Append any known networks
			for (std::vector<Network>::iterator it = sharedData.Networks.begin(); it != sharedData.Networks.end(); ++it)
			{
				AppendMenu(glob.mHdl, "\p ");
				itemCount++;

				std::string ssid = it->Name.c_str();
				SetMenuItemText(glob.mHdl, itemCount, Util::StrToPStr(ssid));

				if (it->Connected)
					SetItemMark(glob.mHdl, itemCount, checkMark);

				if (sharedData.Status == ConnectRequest ||
					sharedData.Status == Connecting)
				{
					DisableItem(glob.mHdl, itemCount);
				}
			}

			if (sharedData.Error)
			{
				AppendMenu(glob.mHdl, "\p ");
				itemCount++;
				SetMenuItemText(glob.mHdl, itemCount, "\pError");
			}

			if (itemCount > 0)
			{
				// Add separator
				AppendMenu(glob.mHdl, "\p-");
				itemCount++;
			}

			AppendMenu(glob.mHdl, "\p ");
			itemCount++;

			switch (sharedData.Status)
			{
				case ScanRequest:
				case Scanning:
					SetMenuItemText(glob.mHdl, itemCount, "\pScanning networks...");
					DisableItem(glob.mHdl, itemCount);
					break;

				case ConnectRequest:
				case Connecting:
					SetMenuItemText(glob.mHdl, itemCount, Util::StrToPStr("Connecting to " + sharedData.ConnectSSID + "..."));
					DisableItem(glob.mHdl, itemCount);
					break;

				default:
					SetMenuItemText(glob.mHdl, itemCount, "\pRefresh networks");
					break;
			}

			SetZone(saveZone);
			sharedData.UpdateUI = false;
		}

		proc = glob.saveMenuSelect;
		value = (*proc) (where);

		return(value);
	}

	pascal void Patched_SystemMenu(long result)
	{
		short menuID, itemID, iconID;
		SystemMenuProcPtr proc; 
		Str255 pName;
		string name;

		menuID = (result & 0xFFFF0000) >> 16;
		itemID = result & 0x0000FFFF;

		// When the user has selected a menu item (by mouse or cmd-key) 
		// we scan the event and handle it if it is from our menu.
		if (menuID == glob.menuID)
		{
			// Some apps aren't careful about giving us the arrow.
			InitCursor();

			if (itemID < CountMItems(glob.mHdl))
			{
				GetMenuItemText(glob.mHdl, itemID, pName);
				name = Util::PtoStr(pName);

				if (name == "Error")
				{
					// Error item selected, show error message
					ShowError();
				}
				else
				{
					// Network selected
					ShowConnectDialog(itemID);
				}
			}
			else
			{
				// The last menu item is the Refresh item
				sharedData.Status = ScanRequest;
				sharedData.UpdateUI = true;
			}

			// I don't think this is vital, but may be helpful for handlers
			// that go into a GNE loop (such as Alert() or ModalDialog())
			HiliteMenu(0);
		}

		proc = glob.saveSystemMenu;

		// Otherwise, pass it on!
		if (menuID != glob.menuID)
			(*proc) (result);
	}

	void ShowConnectDialog(int itemId)
	{
		DialogItemType type;
		Handle itemH;
		Rect box;
		Str255 ssid, text;

		short curResFile = CurResFile();
		short homeResFile = FSpOpenResFile(&glob.homeFile, fsRdPerm);

		// Set ssid label in dialog
		GetMenuItemText(glob.mHdl, itemId, ssid);
		ParamText(ssid, nil, nil, nil);

		DialogPtr dialog = GetNewDialog(128, 0, (WindowPtr)-1);
		MacSetPort(dialog);

		Util::FrameDefaultButton(dialog, 8, false);

		ControlHandle cb;
		GetDialogItem(dialog, 6, &type, &itemH, &box);
		cb = (ControlHandle)itemH;
		SetControlValue(cb, 1);

		short item;
		bool dialogActive = true;

		while (dialogActive)
		{
			ModalDialog(NULL, &item);

			// Set enabled state of Join button based on password length
			GetDialogItem(dialog, 4, &type, &itemH, &box);
			GetDialogItemText(itemH, text);
			Util::FrameDefaultButton(dialog, 8, (text[0] > 0));

			switch (item)
			{
				case 7:
					dialogActive = false;
					break;

				case 8:
					sharedData.ConnectSSID = Util::PtoStr(ssid);
					sharedData.ConnectPwd = Util::PtoStr(text);
					sharedData.Status = ConnectRequest;
					sharedData.UpdateUI = true;

					dialogActive = false;
					break;
			}
		}

		DisposeDialog(dialog);
		CloseResFile(homeResFile);
		UseResFile(curResFile);
	}

	void ShowError()
	{
		short curResFile = CurResFile();
		short homeResFile = FSpOpenResFile(&glob.homeFile, fsRdPerm);

		std::string errMsg = sharedData.ErrorMsg.c_str();
		ParamText(Util::StrToPStr(errMsg), nil, nil, nil);
		StopAlert(129, nil);

		CloseResFile(homeResFile);
		UseResFile(curResFile);
	}
}