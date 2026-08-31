# 2026 roadmap

During 2025, the GTNAP-Designer tool was practically completed. Thanks to it, we will be able to design more expressive forms and windows, overcoming the limitations imposed by the semi-graphical environment. One of my goals is to improve the appearance and functionality of the final applications, while maintaining the ease of use and programming offered by the semi-graphical system and cualib library.

Therefore, I have planned these lines of work for 2026.

## Target-1: Correcting errors in Designer

I have found aspects in NAppGUI-SDK that directly affect the user experience of the designer and need improvement. These are:

- Significant flickering on Windows when resizing internally (canvas/toolbox/property editor). SplitView issue.
- Accuracy errors in internal resizing (SplitView) on Linux/GTK. This results in a very poor user experience.
- Panels must take into account the thickness of scrollbars to avoid hiding information or widgets beneath them.
- Resizing windows in GTK/Linux sometimes blocks, preventing the user from reaching the desired window size.
- Some macOS panels with scroll don't move the content properly. I have investigate and is an issue when mix Cocoa/CoreGraphics with NSScrollView. I have to do the scroll manually, discarding de NSScrollView.

## Target-2: Evolution of NAppGUI/Designer/Forms

With this, we aim to add new widgets and evolve the backends, in order to support current windows managers features such as HiDPI.

- Add Horizontal/Vertical line widgets.
- Add widget groups panels.
- Add TabBar widget.
- Add TreeView, as an evolution of TableView.
- HiDPI support.

## Target-3 GTNAP/Forms/Harbour/Aspec integration

The main goal this year is to consolidate the use of GTNAP/Forms in the Aspec production environment. As I mentioned before, I want the applications to be aesthetically improved while maintaining ease of use. I would like to create an experimental application from scratch that is a "clone" of one of your production applications. To do this, **I would like to have a demo version of your applications**. I want to be able to use it like any real user to explore the mechanics and transfer them to GTNAP/Forms. For example:

* Forms and modal windows.
* Keyboard shortcuts.
* Navigating the controls.
* Wizards, help, message boxes.

## Sprint planning. The order/priority can change.

- Sprint 1: Windows flickering. Use `DeferWindowPos()` Win32 API. **Done Sprint 97 15/01/26**
- Sprint 2: GTK resizing issue. Review GTK API and monitor the mouse events.
- Sprint 3: macOS panels with scrollbar (remains at fixed pos). **Done Sprint 97 15/01/26**
- Sprint 4: Improve NAppGUI layout algorithm to take into account panel scrollbars width/height.
- Sprint 5: Implement line widgets (hor/ver) in NAppGUI and add them to designer. **Done Sprint 101 15/03/26**
- Sprint 6: --> Possible maintenance tasks for current projects. **Done Sprint 100 28/02/26** (Fully documentation review).
- Sprint 7: Implement group widget in NAppGUI. **Done Sprint 102 31/03/26**
- Sprint 8: Add panel widget and group widget to designer. **Done Sprint 103 15/04/26**
- Sprint 9: Implement TabBar widget in NAppGUI. **Done Sprint 105 15/05/26**
- Sprint 10: Implement TreeView widget in NAppGUI. **Done Sprint 107 15/06/26**
- Sprint 11: Implement ToolButton texts in NAppGUI/designer. **Done Sprint 104 30/04/26**
- Sprint 12: Add TabBar and TreeView to designer. **Done Sprint 106 31/05/26**
- Sprint 13: Implement HiDPI support in NAppGUI Win32 backend (I). **Done Sprint 111 15/08/26**
- Sprint 14: Implement HiDPI support in NAppGUI Win32 backend (II). **Done Sprint 112 31/08/26**
- Sprint 15: Implement support in NAppGUI for GTK3/Wayland backend. Ubuntu 24+ application icon.
- Sprint 16: --> Possible maintenance tasks for current projects. HBAWS Vendorization **Done Sprint 109 15/07/26**
- Sprint 17: Implement HiDPI support in NAppGUI GTK backend.
- Sprint 18: Aspec-clone app: Delve deeper into the user mode.
- Sprint 19: Aspec-clone app: Main Window, modals, navigation.
- Sprint 20: Aspec-clone app: Support for editbox with char patterns (date/time/document).
- Sprint 21: --> Possible maintenance tasks for current projects. HBOFFICE Vendorization **Done Sprint 110 31/07/26**
- Sprint 22: Aspec-clone app: Keyboard navigation within the form. Hotkeys.
- Sprint 23: Aspec-clone app: Messagebox, secondary windows.
- Sprint 24: Aspec-clone app: Reuse parts in designer.

## Non planned sprints

- **Sprint 98 31/01/26**: Beginning HBNAP.
- **Sprint 99 15/02/26**: HBNAP integration into CUALIB/GTNAP.
- **Sprint 108 30/06/26**: HBNAP TreeView for DB relations.
