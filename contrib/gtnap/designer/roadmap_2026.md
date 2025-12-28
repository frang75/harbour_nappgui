# 2026 roadmap

During 2025, the GTNAP-Designer tool was practically completed. Thanks to it, we will be able to design more expressive forms and windows, overcoming the limitations imposed by the semi-graphical environment. One of my goals is to improve the appearance and functionality of the final applications, while maintaining the ease of use and programming offered by the semi-graphical system and cualib library.

Therefore, I have planned these lines of work for 2026.

## Target-1: Correcting errors in Designer

I have found aspects in NAppGUI-sDK that directly affect the user experience of the designer and need improvement. These are:

- Significant flickering on Windows when resizing internally (canvas/toolbox/property editor). SplitView issue.
- Accuracy errors in internal resizing (SplitView) on Linux/GTK. This results in a very poor user experience.
- Panels must take into account the thickness of scrollbars to avoid hiding information or widgets beneath them.
- Resizing windows in GTK/Linux sometimes blocks, preventing the user from reaching the desired window size.

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

- Sprint 1: Windows flickering. Use `DeferWindowPos()` Win32 API.
- Sprint 2: GTK resizing issue. Review GTK API and monitor the mouse events.
- Sprint 3: macOS panels with scrollbar (remains at fixed pos).
- Sprint 4: Improve NAppGUI layout algorithm to take into account panel scrollbars width/height.
- Sprint 5: Implement line widgets (hor/ver) in NAppGUI and add them to designer.
- Sprint 6: --> Possible maintenance tasks for current projects.
- Sprint 7: Implement group widget in NAppGUI.
- Sprint 8: Add panel widget and group widget to designer.
- Sprint 9: Implement TabBar widget in NAppGUI.
- Sprint 10: Implement TreeView widget in NAppGUI.
- Sprint 11: --> Possible maintenance tasks for current projects.
- Sprint 12: Add TabBar and TreeView to designer.
- Sprint 13: Implement HiDPI support in NAppGUI Win32 backend (I).
- Sprint 14: Implement HiDPI support in NAppGUI Win32 backend (II).
- Sprint 15: Implement support in NAppGUI for GTK3/Wayland backend. Ubuntu 24+ application icon.
- Sprint 16: --> Possible maintenance tasks for current projects.
- Sprint 17: Implement HiDPI support in NAppGUI GTK backend.
- Sprint 18: Aspec-clone app: Delve deeper into the user mode.
- Sprint 19: Aspec-clone app: Main Window, modals, navigation.
- Sprint 20: Aspec-clone app: Support for editbox with char patterns (date/time/document).
- Sprint 21: --> Possible maintenance tasks for current projects.
- Sprint 22: Aspec-clone app: Keyboard navigation within the form. Hotkeys.
- Sprint 23: Aspec-clone app: Messagebox, secondary windows.
- Sprint 24: Aspec-clone app: Reuse parts in designer.
