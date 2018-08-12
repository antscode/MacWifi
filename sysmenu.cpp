/*
	2018 Anthony Super.
	System Menu INIT code based on "SysMenu" shell project by Matt Slot, circa 1995.
	Password field based on code snippet by Phil Kearney, circa 1994.
*/

#include <Gestalt.h>
#include <Icons.h>
#include <Traps.h>
#include <Resources.h>
#include <MacMemory.h>
#include <Sound.h>
#include <string.h>

#include "sysmenu.h"
#include "Util.h"
#include "ShowInitIcon.h"

extern "C"
{
	#include "Retro68Runtime.h"

	GlobalsRec glob;
	WifiData sharedData;
	Str255 _password = "\p";
	bool _showPassword;

	void _start()
	{
		RETRO68_RELOCATE();

		ShowInitIcon(128, true);

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

			if (sharedData.Error)
			{
				AppendMenu(glob.mHdl, "\p ");
				itemCount++;
				SetMenuItemText(glob.mHdl, itemCount, "\pError");
			}
			else
			{
				// Append any known networks
				for (std::vector<Network>::iterator it = sharedData.Networks.begin(); it != sharedData.Networks.end(); ++it)
				{
					AppendMenu(glob.mHdl, "\p ");
					itemCount++;

					std::string ssid = it->Name.c_str();
					SetMenuItemText(glob.mHdl, itemCount, Util::StrToPStr(ssid));

					if (it->Connected)
						SetItemMark(glob.mHdl, itemCount, checkMark);

					if (sharedData.Status != Idle)
					{
						DisableItem(glob.mHdl, itemCount);
					}
				}
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
					SetMenuItemText(glob.mHdl, itemCount, Util::StrToPStr("Connecting to " + std::string(sharedData.ConnectName) + "..."));
					DisableItem(glob.mHdl, itemCount);
					break;

				case RestartRequired:
					SetMenuItemText(glob.mHdl, itemCount, "\pRestart required");
					break;

				case RestartRequest:
				case Restarting:
					SetMenuItemText(glob.mHdl, itemCount, "\pRestarting...");
					DisableItem(glob.mHdl, itemCount);
					break;

				default:
					SetMenuItemText(glob.mHdl, itemCount, "\pRefresh networks");

					AppendMenu(glob.mHdl, "\p ");
					itemCount++;
					SetMenuItemText(glob.mHdl, itemCount, "\pSettings...");
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

			GetMenuItemText(glob.mHdl, itemID, pName);
			name = Util::PtoStr(pName);

			if (name == "Error")
			{
				ShowError();
			}
			else if (name == "Settings...")
			{
				ShowSettings();
			}
			else if (name == "Refresh networks")
			{
				sharedData.Status = ScanRequest;
				sharedData.UpdateUI = true;
			}
			else if (name == "Restart required")
			{
				sharedData.Status = RestartRequest;
				sharedData.UpdateUI = true;
			}
			else
			{
				// Network selected
				Network& network = sharedData.Networks.at(itemID - 1);

				if (network.Mode == Open)
				{
					// Open network, so initiate connect request immediately
					memset(&sharedData.ConnectName, 0, sizeof(sharedData.ConnectName));
					memset(&sharedData.ConnectId, 0, sizeof(sharedData.ConnectId));
					memset(&sharedData.ConnectPwd, 0, sizeof(sharedData.ConnectPwd));

					strcpy(sharedData.ConnectName, network.Name.c_str());
					strcpy(sharedData.ConnectId, network.Id.c_str());
					sharedData.ConnectMode = network.Mode;
					sharedData.ConnectEncryption = network.Encryption;
					sharedData.Status = ConnectRequest;
					sharedData.UpdateUI = true;
				}
				else
				{
					ShowConnectDialog(network);
				}
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

	void ShowConnectDialog(Network& network)
	{
		DialogItemType type;
		Handle itemH;
		Rect box;
		ControlHandle pwdCtrl;

		short curResFile = CurResFile();
		short homeResFile = FSpOpenResFile(&glob.homeFile, fsRdPerm);
		_password[0] = 0;

		// Set ssid label in dialog
		std::string ssid = network.Name.c_str();
		ParamText(Util::StrToPStr(ssid), Util::StrToPStr(GetWifiModeLabel(network.Mode)), nil, nil);

		DialogPtr dialog = GetNewDialog(128, 0, (WindowPtr)-1);
		MacSetPort(dialog);

		Util::FrameDefaultButton(dialog, 7, false);

		GetDialogItem(dialog, 5, &type, &itemH, &box);
		pwdCtrl = (ControlHandle)itemH;
		_showPassword = GetControlValue(pwdCtrl);

		ModalFilterUPP filterProc = NewModalFilterProc(PSWDModalFilter);

		short item;
		bool dialogActive = true;

		while (dialogActive)
		{
			ModalDialog(filterProc, &item);

			switch (item)
			{
				case 5:
					_showPassword = !GetControlValue(pwdCtrl);
					SetControlValue(pwdCtrl, _showPassword);
					PasswordKey(((DialogPeek)dialog)->textH, 0);
					break;

				case 6:
					dialogActive = false;
					break;

				case 7:
					if (_password[0] > 0)
					{
						memset(&sharedData.ConnectName, 0, sizeof(sharedData.ConnectName));
						memset(&sharedData.ConnectId, 0, sizeof(sharedData.ConnectId));
						memset(&sharedData.ConnectPwd, 0, sizeof(sharedData.ConnectPwd));
						strcpy(sharedData.ConnectName, network.Name.c_str());
						strcpy(sharedData.ConnectId, network.Id.c_str());
						strcpy(sharedData.ConnectPwd, Util::PtoStr(_password).c_str());

						sharedData.ConnectMode = network.Mode;
						sharedData.ConnectEncryption = network.Encryption;
						sharedData.Status = ConnectRequest;
						sharedData.UpdateUI = true;

						dialogActive = false;
					}
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

		std::string errMsg = sharedData.ErrorMsg;
		ParamText(Util::StrToPStr(errMsg), nil, nil, nil);
		StopAlert(129, nil);

		CloseResFile(homeResFile);
		UseResFile(curResFile);
	}

	void ShowSettings()
	{
		DialogItemType type;
		Handle itemH;
		Rect box;
		Str255 pHostname, pUsername, pPassword;

		short curResFile = CurResFile();
		short homeResFile = FSpOpenResFile(&glob.homeFile, fsRdPerm);

		DialogPtr dialog = GetNewDialog(130, 0, (WindowPtr)-1);
		MacSetPort(dialog);

		MoveTo(82, 29);
		PenSize(1, 1);
		PenNormal();
		MacLineTo(352, 29);

		// Set device
		GetDialogItem(dialog, 4, &type, &itemH, &box);
		SetControlValue((ControlHandle)itemH, sharedData.Device);

		// Set hostname
		std::string hostname = sharedData.Hostname;
		GetDialogItem(dialog, 6, &type, &itemH, &box);
		SetDialogItemText(itemH, Util::StrToPStr(hostname));

		// Set username
		std::string username = sharedData.Username;
		GetDialogItem(dialog, 8, &type, &itemH, &box);
		SetDialogItemText(itemH, Util::StrToPStr(username));

		// Set password
		std::string password = sharedData.Password;
		GetDialogItem(dialog, 10, &type, &itemH, &box);
		SetDialogItemText(itemH, Util::StrToPStr(password));

		Util::FrameDefaultButton(dialog, 12, true);

		ModalFilterUPP filterProc = NewModalFilterProc(SettingsModalFilter);

		short item;
		bool dialogActive = true;

		while (dialogActive)
		{
			ModalDialog(filterProc, &item);

			// Get hostname
			GetDialogItem(dialog, 6, &type, &itemH, &box);
			GetDialogItemText(itemH, pHostname);

			// Get username
			GetDialogItem(dialog, 8, &type, &itemH, &box);
			GetDialogItemText(itemH, pUsername);

			// Get password
			GetDialogItem(dialog, 10, &type, &itemH, &box);
			GetDialogItemText(itemH, pPassword);

			Util::FrameDefaultButton(dialog, 12, (pHostname[0] > 0 && pUsername[0] > 0 && pPassword[0] > 0));

			switch (item)
			{
				case 11:
					dialogActive = false;
					break;

				case 12:
					memset(&sharedData.Hostname, 0, sizeof(sharedData.Hostname));
					memset(&sharedData.Username, 0, sizeof(sharedData.Username));
					memset(&sharedData.Password, 0, sizeof(sharedData.Password));

					// Set device
					GetDialogItem(dialog, 4, &type, &itemH, &box);
					sharedData.Device = GetControlValue((ControlHandle)itemH);

					// Set hostname
					strcpy(sharedData.Hostname, Util::PtoCStr(pHostname));

					// Set username
					strcpy(sharedData.Username, Util::PtoCStr(pUsername));

					// Set password
					strcpy(sharedData.Password, Util::PtoCStr(pPassword));

					sharedData.Status = SavePrefsRequest;
					dialogActive = false;
					break;
			}
		}

		DisposeDialog(dialog);
		CloseResFile(homeResFile);
		UseResFile(curResFile);
	}

	pascal Boolean PSWDModalFilter(DialogPtr dialog, EventRecord *theEvent, short *itemHit)
	{
		char key;
		ModalFilterProcPtr theModalProc;

		if (theEvent->what == keyDown) {

			key = (char)(theEvent->message & charCodeMask);

			switch (key) 
			{
				case kEnterCharCode:
				case kReturnCharCode:
					if (Util::IsControlHilited(dialog, 7))
					{
						*itemHit = 7;
						return true;
					}
					else
					{
						theEvent->what = 0;
						*itemHit = 0;
						return false;
					}

				case kEscapeCharCode:  
					*itemHit = 6;
					return true;
			
				default:
					PasswordKey(((DialogPeek)dialog)->textH, key);
					theEvent->what = 0;
					*itemHit = 0;

					// Set enabled state of Join button based on password length
					Util::FrameDefaultButton(dialog, 7, (_password[0] > 0));
					return false;
			}
		}

		GetStdFilterProc(&theModalProc);
		return (theModalProc(dialog, theEvent, itemHit));
	}

	pascal Boolean SettingsModalFilter(DialogPtr dialog, EventRecord *theEvent, short *itemHit)
	{
		char key;

		ModalFilterProcPtr theModalProc;

		switch (theEvent->what) 
		{
			case keyDown:
			{
				key = (char)(theEvent->message & charCodeMask);

				switch (key)
				{
					case kEnterCharCode:
					case kReturnCharCode:
						if (Util::IsControlHilited(dialog, 12))
						{
							*itemHit = 12;
							return true;
						}
						else
						{
							theEvent->what = 0;
							*itemHit = 0;
							return false;
						}

					case kEscapeCharCode:
						*itemHit = 11;
						return true;
				}
				break;
			}
		}

		GetStdFilterProc(&theModalProc);
		return (theModalProc(dialog, theEvent, itemHit));
	}

	void PasswordKey(TEHandle teHndl, char theKey)
	{
		short start, end, len;
		Rect hiddenRect;
		Handle txtHndl;
		TEHandle tmpTE;
		Str255 tmpStr;
		GrafPtr currPort;

		GetPort(&currPort);

		// Create a temporary TERec offscreen
		hiddenRect = (**teHndl).viewRect;
		OffsetRect(&hiddenRect, currPort->portRect.right + 5, currPort->portRect.bottom + 5);
		tmpTE = TENew(&hiddenRect, &hiddenRect);

		if (!tmpTE)
			return;

		// Set the current password into the hidden TERec
		TESetText(&_password[1], _password[0], tmpTE);

		// Set the selection range of our hidden TERec to that
		// of the visible password TERec
		TESetSelect((**teHndl).selStart, (**teHndl).selEnd, tmpTE);

		// Process the key in the hidden TERec
		if(theKey > 0)
			TEKey(theKey, tmpTE);

		// Grab the new password out of the hidden TERec
		_password[0] = (**tmpTE).teLength;
		BlockMove(*((**tmpTE).hText), &_password[1], _password[0]);

		// Create a string of bullets to match the real password
		tmpStr[0] = _password[0];
		for (len = 1; len <= tmpStr[0]; len++)
			tmpStr[len] =  _showPassword ? _password[len] :'¥';

		// Deactivate the visible TERec so no phantom cursors pop up.
		TEDeactivate(teHndl);

		// Dump the bullets into the visible TERec
		TESetText(&tmpStr[1], tmpStr[0], teHndl);

		// Set the selection to match that of the hidden TERec
		TESetSelect((**tmpTE).selStart, (**tmpTE).selEnd, teHndl);

		// Dispose of the hidden TERec
		TEDispose(tmpTE);

		// We need to call InvalRect if the visible TEField is empty because
		// of some wierdness I didn't bother to figure out with TextEdit.
		// In all other cases, we just need to update the visible TERec.
		if (!tmpStr[0])
			InvalRect(&((**teHndl).viewRect));
		else
			TEUpdate(&((**teHndl).viewRect), teHndl);

		// Turn the visible TERec back on.
		TEActivate(teHndl);
	}

	string GetWifiModeLabel(WifiMode mode)
	{
		switch (mode)
		{
			case WPA2:
				return "WPA2";
			case WPA:
				return "WPA";
			default:
				return "";
		}
	}
}