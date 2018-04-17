/*
	Copyright 2018 Anthony Super.
	System Menu INIT code based on "SysMenu" shell project by Matt Slot, circa 1995.
*/

#include <Gestalt.h>
#include <Icons.h>
#include <Traps.h>
#include <Resources.h>
#include <MacMemory.h>
#include <Menus.h>
#include <Sound.h>

#include "sysmenu.h"
#include "Prefs.h"
#include "Util.h"

extern "C"
{
	#include "Retro68Runtime.h"

	GlobalsRec glob;


	void _start()
	{
		RETRO68_RELOCATE();

		Handle myHandle;

		// Setup Global Data
		SetZone(SystemZone());

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

		SetZone(ApplicationZone());

		// Detach the INIT resource to prevent it from being purged
		Handle h = GetResource('INIT', 128);
		DetachResource(h);

		Handle codeHndl = GetIndResource('dnrp', 1);
		DetachResource(codeHndl);
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

			// For this example, we build it from scratch. I prefer making the
			// AppendMenu() call with dummy args *then* SetItem() with our item.
			// If you trust AppendMenu() with raw data, it will interpret the Menu
			// Managers meta-chars ("-"=Separator, "("=Disabled, "/B"=Command-B, etc)
			//AppendMenu(glob.mHdl, "\p ");
			//SetMenuItemText(glob.mHdl, CountMItems(glob.mHdl), "\pNetwork 1");
			//AppendMenu(glob.mHdl, "\p ");
			//SetMenuItemText(glob.mHdl, CountMItems(glob.mHdl), "\pNetwork 2");
			//AppendMenu(glob.mHdl, "\p ");
			//SetMenuItemText(glob.mHdl, CountMItems(glob.mHdl), "\pNetwork 3");
			//AppendMenu(glob.mHdl, "\p ");
			//SetMenuItemText(glob.mHdl, CountMItems(glob.mHdl), "\pNetwork 4");
			//AppendMenu(glob.mHdl, "\p-");
			//AppendMenu(glob.mHdl, "\p ");
			//SetMenuItemText(glob.mHdl, CountMItems(glob.mHdl), "\pSettings...");

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

		// This function is called when the user first clicks in the menubar.
		// Its an ideal place to update the contents of your menu on the fly
		// to reflect current settings or conditions.

		if (glob.mHdl && !glob.initialised)
		{
			saveZone = GetZone();
			SetZone(SystemZone());

			Prefs prefs;
			Json::Value networks = prefs.Data["networks"];

			for (int i = 0; i < networks.size(); ++i)
			{
				AppendMenu(glob.mHdl, "\p ");
				SetMenuItemText(glob.mHdl, CountMItems(glob.mHdl), Util::StrToPStr(networks[i]["name"].asString()));
			}

			SetZone(saveZone);
			glob.initialised = true;
		}

		proc = glob.saveMenuSelect;
		value = (*proc) (where);

		return(value);
	}

	pascal void Patched_SystemMenu(long result)
	{
		short menuID, itemID;
		SystemMenuProcPtr proc;

		menuID = (result & 0xFFFF0000) >> 16;
		itemID = result & 0x0000FFFF;

		// When the user has selected a menu item (by mouse or cmd-key) 
		// we scan the event and handle it if it is from our menu.
		if (menuID == glob.menuID)
		{
			// Some apps aren't careful about giving us the arrow.
			InitCursor();

			switch (itemID) {
			case 1:
				SysBeep(7);
				break;
			default:
				SysBeep(7);
				SysBeep(7);
				break;
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
}