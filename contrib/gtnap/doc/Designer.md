# NAppGUI designer

* [Designer overview](#napdesigner-overview)
* [Build Designer](#build-napdesigner)
* [Inspect demo forms](#inspect-demo-forms)
* [Create a new form](#create-a-new-form)
    - [Space subdivision. Adding cells](#space-subdivision-adding-cells)
    - [Adding widgets](#adding-widgets)
    - [Margins and maximum size](#margins-and-maximum-size)

NAppGUI Designer is a visual tool for creating user interfaces (forms) graphically and interactively. These forms will be saved in files that can be loaded at runtime from Harbour, using HBNAP. It has been developed using NAppGUI-SDK and the forms it generates also use NAppGUI to run within the final application (https://nappgui.com).

## Designer overview

In principle, the appearance of the application is very similar to that of other similar tools (QTDesigner, for example). In the central part we will have the design area where we view the form under construction. On the left we have a list of files and a widget selector. On the right we have the object inspector and the properties editor. At the top we will see the typical toolbar for file management and a status bar at the bottom.

![designer](./images/designer.png)

## Build Designer

Designer is distributed as part of GTNAP, so you don't have to do anything special to compile it, just run the build script in `contrib\gtnap`. More information in [Build GTNAP](../Readme.md#build-gtnap)

```
cd contrib/gtnap

:: Windows MinGW
build.bat -b [Debug|Release] -comp mingw64

:: Linux/macOS
bash ./build.sh -b [Debug|Release]
```
The application executable can be found at `build/[Debug|Release]/bin/napdesign`, just run it.

## Inspect the demo forms

The first time the application starts we will have a blank drawing area and all the buttons are off. To make a first contact, it is recommended to open the example forms. Designer loads all existing ones in the same folder. Click `Open Forms` button (📁) and select `/contrib/gtnap/tests/cuademo/forms`. We can see the current folder path by placing the mouse over the icon. By clicking on any file, we will select it and see it in the drawing area.

![openfolder](./images/openfolder.png)

## Create a new form

> **Important:** NAppGUI is based on the concept of Layout (GridLayout in QtDesigner) which will divide the space into a grid of **ncols** x **nrows** cells. The difference with other SDKs is that NAppGUI **does not support floating elements**. All widgets must be inside a cell in a layout. As we will see below, the main advantage of this `Layout->Sublayout->Cell->Widget` relationship is that **it is not necessary to set the frame** (position and size) of each element, since it will be automatically calculated by NAppGUI based on the native API (Win32, GTK, Cocoa).

Press `[Add new form]`, set the name and description. You will see in the canvas area that a small form has been created with a blank interior region. This is the first cell (or root cell) where we can place a single widget.

![newform](./images/newform.png)

![newform2](./images/newform2.png)

Before starting to place components, we will have to think about the structure of the form. The ideal is to make a small sketch on paper or a mental diagram. As a first example, let's recreate the `Company_detail` form.

![firstform](./images/first_form.png)

### Space subdivision. Adding cells

Looking at our form to replicate, we identify a vertical organization composed of three areas: Toolbar, Data Entry and Action Buttons. We create this structure with 3 cells.

* Click _Vertical Layout_ in the _Widget Selector_. This will mark the widget.

* Click inside the only cell within the form on the canvas. A box will appear where we indicate 3 rows.

    ![subdiv1](./images/subdivision1.png)

    ![subdiv2](./images/subdivision2.png)

Our initial root cell has become a stack of three vertical cells. We continue creating spaces for the toolbar.

* Click _Horizontal Layout_ in the _Widget Selector_.

* Click inside the TOP cell within the form on the canvas. A box will appear where we indicate 8 columns.

    ![subdiv3](./images/subdivision3.png)

    ![subdiv4](./images/subdivision4.png)

For the central data area we will need a 2x6 cell grid.

* Click _Grid Layout_ in the _Widget Selector_.

* Click inside the MIDDLE cell within the form on the canvas. A box will appear where we indicate 2 columns and 6 rows.

    ![subdiv5](./images/subdivision5.png)

    ![subdiv6](./images/subdivision6.png)

Since we are only going to place one button in the Action Buttons area (bottom area), it is not necessary to continue subdividing the form space.

### Adding widgets

Once we have the structure of the form, we are going to add the widgets, starting with the toolbar.

* Click _Tool button_ in the _Widget Selector_.

* Click in the top-left cell. A box will appear.

* Click `Load icon` button and select `contrib/gtnap/test/cuademo/forms/icons/salvar.png`.

    ![widget1](./images/widgets1.png)

    ![widget2](./images/widgets2.png)

    ![widget3](./images/widgets3.png)

> **Important:** As you can see, when you add widgets to the cells, they are resized to accommodate the control (the toolbutton image in this case). This resizing propagates recursively to the rest of the neighboring cells, affecting the total size of the form.

> **Important:** When we associate resources to controls (mostly images) the relative path from the location of the form to the location of the resource is stored (`/icons/salvar.png` in this case).

* We repeat the process for the remaining seven buttons on the toolbar.

    ![widget4](./images/widgets4.png)

* We continue through the entry data area. Click on _Label_ in the _Widget Selector_.

* Click on the free upper right cell. In the text box we type `Inclusão de empresa`.

    ![widget5](./images/widgets5.png)

    ![widget6](./images/widgets6.png)

* We repeat the operation for the rest of the texts in the left column:

    - `Unidade Federativa`.
    - `Código da empresa`.
    - `Nombre da cidade`.
    - `Código da entidade`.
    - `Unidade gestora centralizadora`.

    ![widget7](./images/widgets7.png)

* Now let's add the text editing boxes and combo box. Click on _Combo Box_ in the _Widget Selector_.

* Click on the free cell at the top right.

* We select a size of 100px for the combo.

    ![widget8](./images/widgets8.png)

    ![widget9](./images/widgets9.png)

* Now, click _Edit Box_ in the _Widget Selector_.

* Click on the free upper right cell.

* We select a size of 60px.

    ![widget10](./images/widgets10.png)

    ![widget11](./images/widgets11.png)

> **Important:** We see that, although we have indicated 60px, the Edit Box measures 100px. This is because, by default, the cell containing an Edit Box has horizontal expansion by default. We will return to this later.

* We finish the text input block:

    - `Nombre da cidade`: Combo Box 100px.
    - `Código da entidade`: Edit Box 50px.
    - `Unidade gestora centralizadora`: Edit Box 300px.

    ![widget12](./images/widgets12.png)

> **Important:** As we have already indicated, despite having introduced different measurements, all text controls have been expanded horizontally to the width of the width (300px).

* To finish, let's add the push button. Click _Push Button_ on the _Widget Selector_.

* Click on the bottom empty cell.

* In the dialog we write `F2 = Save` as the button text.

    ![widget13](./images/widgets13.png)

    ![widget14](./images/widgets14.png)

> **Important:** Buttons (and other text-based controls, such as _Label_) are automatically sized based on the text they contain. However, Push Button cells also expand automatically.

* We have already completed the basic layout of the form. If we press the button (🔍) `Simulate Form` we can launch the form.

    ![widget15](./images/widgets15.png)


### Margins and maximum size

While our form is fully functional, it is not very aesthetic. Let's give it some formatting to improve its appearance.

* In the _Object inspector_ select _layout3_, which corresponds to the layout of 2 columns and 9 rows that we added before. In the _Property editor_ select **Column 1** and set the **FWidth** (forced width) property to 300. This "forces" the maximum width of column 1 of the layout to 300 pixels. Interior controls adjust automatically.

    ![format1](./images/format1.png)

* Now we are going to leave a small separation between the _Label_ column and the _Editbox_ column. We continue in _layout3_, select **Column 0** and **CRight** to 5. This forces a separation of 5 pixels to the right of column 0. As you can see, you do not have to adjust the position of the Editboxes with the mouse . NAppGUI recalculates the entire design based on the changes we make.

    ![format2](./images/format2.png)

* Now select the _cell2_ component, which is the bottom cell that contains a layout with the two buttons. In the _Property editor_ we see that the cell properties appear, where we see the label **Layout cell**. This means that in this cell we do not have a widget, but rather a layout (grid) of 2 columns and 1 row with the two buttons. We changed the **HAlign** property to **Right** (instead of Justify). We will see that both buttons align to the right. By default, when a cell contains a sub-layout the content expands to fill the entire cell space.

    ![format3](./images/format3.png)

* Select _layout5_ (the grid with the two buttons). In **Column 0** set **FWidth** to 60 and **CRight** to 10. And in **Column 1**, **FWidth** to 60. This will slightly increase the default width of the buttons and will leave a separation between them of 10 pixels.

    ![format4](./images/format4.png)

* Select _layout1_, **Row 0**, **RBottom** 30. This forces a vertical separation between the data area and the buttons.

    ![format5](./images/format5.png)

* Let's now go to _cell4_, which is the cell that contains the sublayout with the 4 _Checkbox_. In **VAlign** we select **Top**. With this we manage to group all the _Checkbox_ at the top of the cell.

    ![format6](./images/format6.png)

* To leave some separation between the _Checkboxes_, select _layout4_ and, for **Row 0**, **Row 1** and **Row 2** set **RBottom** to 5.

    ![format7](./images/format7.png)

* Now, in _layout2_, **Column 0**, **CRight** 10, which will leave a horizontal separation of 10 pixels between the _Editbox_ and the _Checkbox_.

    ![format8](./images/format8.png)

* We want to leave a small vertical separation of 3 pixels between each _Label_/_Editbox_ row. Select _layout3_, **Row 0-7**, **RBottom** 3.

    ![format9](./images/format9.png)

* And another three horizontal pixels between each _Editbox_ of the `Bank account` and `Credit card`. Select _layout7_, **Column 0-3**, **CRight** 3 for `Credit card`.

    ![format10](./images/format10.png)

* And finally we are going to establish a border for the entire form of 10 pixels. Select _layout0_ and set the **Top**, **Left**, **Bottom**, **Right** properties to 10.

    ![format11](./images/format11.png)

* Press the icon (ðŸ”�) _Simulate current form_ to check the final result.

    ![simulate2](./images/simulate2.png)
