:h2.Programming the Container Control (Part III)
:p.Written by Larry Salomon, Jr.
:h3.Back at the Batcave
:p.Last month we continued our disection of the container control and how to
use it.  The tree view was added to our list of conquests, and we started
developing a sample application which we will continue to use.  This month, we
will add more meat to the bones of our skeleton by learning about the
:hp2.detail view:ehp2. and :hp2.direct editing:ehp2., among other things.
:h3.Detail View
:p.Back in the first installment of this series, the detail view was described
in the following manner.
:font facename='Tms Rmn' size=16x14.
:sl.
:li.Each object is represented as a detailed description of the object.
The information displayed is defined by the application.
:esl.
:font facename=default.
:p.While I realize that did not say much, it served to illustrate that the
detail view is the most flexible of the views, in terms of what can be
displayed.  It should be logical then to assume that this means yet more setup
on the part of the application.
:p.:hp2.What is the Detail View?:ehp2.
:p.To be precise, the detail view is a matrix view of the contents of a
container, where each row in the matrix is a separate object and each column
is an attribute (called a :hp2.field:ehp2.) of every object to be displayed.
:p.Since the objects are already added using the
:hp2.CM_ALLOCRECORD:ehp2./:hp2.CM_INSERTRECORD:ehp2. messages, the columns
must be added; this is done using the
:hp2.CM_ALLOCDETAILFIELDINFO:ehp2./:hp2.CM_INSERTDETAILFIELDINFO:ehp2.
messages.  As with its record-oriented counterpart, the
:hp2.CM_ALLOCDETAILFIELDINFO:ehp2. accepts the number of fields to allocate
memory for and returns a pointer to the first :hp2.FIELDINFO:ehp2. structure
in the linked-list.
:font facename=Courier size=14x14.
:xmp.
typedef struct _FIELDINFO &lbrc.
   ULONG cb;
   ULONG flData;
   ULONG flTitle;
   PVOID pTitleData;
   ULONG offStruct;
   PVOID pUserData;
   struct _FIELDINFO *pNextFieldInfo;
   ULONG cxWidth;
&rbrc. FIELDINFO, *PFIELDINFO;
:exmp.
:font facename=Helv size=12x12.
:p.Figure 1.  The :hp2.FIELDINFO:ehp2. structure.
:font facename=default.
:dl tsize=20 break=fit.
:dt.cb
:dd.specifies the size of the structure in bytes.
:dt.flData
:dd.specifies flags (:hp2.CFA_*:ehp2. constants) for the field, especially the
datatype.
:dt.flTitle
:dd.specifies flags (:hp2.CFA_*:ehp2. constants) for the column title.
:dt.pTitleData
:dd.points to a string used for the column heading (can be :hp2.NULL:ehp2.).
:dt.offStruct
:dd.specifies the offset of the data in the container record to be formatted
according to its datatype.  The :hp2.FIELDOFFSET:ehp2. macro (defined in the
Toolkit) is very helpful in initializing this field.
:p.When the datatype is :hp2.CFA_STRING:ehp2., the field in the container
record is expected to be a :hp2.pointer:ehp2. to the string and not the string
itself.  For example,
.im cnrtype.pnl
we would specify :hp2.FIELDOFFSET(MYCNRREC,pchProdName):ehp2. instead of
:hp2.FIELDOFFSET(MYCNRREC,achProdName):ehp2..  The reason for this will be
clear when we discuss direct editing.
:dt.pUserData
:dd.points to any application-defined data for the field.
:dt.pNextFieldInfo
:dd.points to the next :hp2.FIELDINFO:ehp2. structure.  This is initialized by
the :hp2.CM_ALLOCDETAILFIELDINFO:ehp2. message.
:dt.cxWidth
:dd.specifies the width in pixels of the field.  This is initialized by the
:hp2.CM_ALLOCDETAILFIELDINFO:ehp2. message to 0, indicating that the field
should be wide enough to show the widest value.  If the default is not used
and the data is too wide to fit, it is truncated when displayed.
:edl.
:p.The :hp1.flData:ehp1. field is initialized using one or more
:hp2.CFA_*:ehp2. constants&colon.
:p.:hp2.Data type:ehp2.
:dl tsize=20 break=fit.
:dt.CFA_BITMAPORICON
:dd.:hp1.offStruct:ehp1. &odq.points&cdq. to the handle of a bitmap or icon,
depending on whether or not :hp2.CA_DRAWICON:ehp2. or :hp2.CA_DRAWBITMAP:ehp2.
was specified in the :hp1.flWindowAttr:ehp1. field in the
:hp2.CM_SETCNRINFO:ehp2. message (:hp2.CA_DRAWICON:ehp2. is the default if not
explicitly changed by the application).
:dt.CFA_STRING
:dd.:hp1.offStruct:ehp1. &odq.points&cdq. to a pointer to the string to be
displayed.  Only data of this type can be directly edited.
:dt.CFA_ULONG
:dd.:hp1.offStruct:ehp1. &odq.points&cdq. to an unsigned long integer.
:dt.CFA_DATE
:dd.:hp1.offStruct:ehp1. &odq.points&cdq. to a :hp2.CDATE:ehp2. structure.
:dt.CFA_TIME
:dd.:hp1.offStruct:ehp1. &odq.points&cdq. to a :hp2.CTIME:ehp2. structure.
:edl.
:p.For the latter three, NLS support is provided automatically by the
container.  You should note that there is no support for short integers, since
they map directly to long integers with no loss in precision, nor is there
support for floating point (none of PM uses floating point, so why should they
start now).  The latter means that you have to also have a string
representation of the number (and creates all kinds of headaches if you will
allow editing of the value).
:p.:hp2.Alignment:ehp2.
:dl tsize=20 break=fit.
:dt.CFA_LEFT
:dd.Align the data to the left
:dt.CFA_CENTER
:dd.Horizontally center the data
:dt.CFA_RIGHT
:dd.Align the data to the right
:dt.CFA_TOP
:dd.Align the data to the top
:dt.CFA_VCENTER
:dd.Vertically center the data
:dt.CFA_BOTTOM
:dd.Align the data to the bottom
:edl.
:p.:hp2.Miscellaneous:ehp2.
:dl tsize=20 break=fit.
:dt.CFA_SEPARATOR
:dd.Displays a vertical separator to the right of the field
:dt.CFA_HORZSEPARATOR
:dd.Displays a horizontal separator underneath the column heading
:dt.CFA_OWNER
:dd.Enables ownerdraw for this field
:dt.CFA_INVISIBLE
:dd.Prevents display of this column
:dt.CFA_FIREADONLY
:dd.Prevents direct editing of the data if :hp2.CFA_STRING:ehp2. is the
datatype
:edl.
:p.The :hp1.flTitle:ehp1. field is initialized using one or more of the
alignment fields and/or one or both of the following
:p.:hp2.Miscellaneous:ehp2.
:dl tsize=20 break=fit.
:dt.CFA_BITMAPORICON
:dd.:hp1.pTitleData:ehp1. is the handle of a bitmap or icon, depending on
whether or not :hp2.CA_DRAWICON:ehp2. or :hp2.CA_DRAWBITMAP:ehp2. was
specified in the :hp1.flWindowAttr:ehp1. field in the :hp2.CM_SETCNRINFO:ehp2.
message (:hp2.CA_DRAWICON:ehp2. is the default if not explicitly changed by
the application).  If this is not specified, :hp1.pTitleData:ehp1. is expected
to point to character data.
:dt.CFA_FITITLEREADONLY
:dd.the text of the title is not directly editable.
:edl.
:p.:hp2.What&apos.s Next?:ehp2.
:p.Once you have initialized all of the :hp2.FIELDINFO:ehp2. structures, you
can &odq.insert&cdq. them into the container using the
:hp2.CM_INSERTDETAILFIELDINFO:ehp2. message.  Again using the parallel of the
:hp2.CM_INSERTRECORD:ehp2. message, it expects a pointer to the first
:hp2.FIELDINFO:ehp2. structure as well as a pointer to a
:hp2.FIELDINFOINSERT:ehp2. structure.
:font facename=Courier size=14x14.
:xmp.
typedef struct _FIELDINFOINSERT &lbrc.
   ULONG cb;
   PFIELDINFO pFieldInfoOrder;
   ULONG fInvalidateFieldInfo;
   ULONG cFieldInfoInsert;
&rbrc. FIELDINFOINSERT, *PFIELDINFOINSERT;
:exmp.
:font facename=Helv size=12x12.
:p.Figure 2.  The :hp2.FIELDINFOINSERT:ehp2. structure.
:font facename=default.
:dl tsize=20 break=fit.
:dt.cb
:dd.specifies the size of the structure in bytes.
:dt.pFieldInfoOrder
:dd.specifies the :hp2.FIELDINFO:ehp2. structure to be linked after, or
:hp2.CMA_FIRST:ehp2. or :hp2.CMA_LAST:ehp2. to specify that these
:hp2.FIELDINFO:ehp2. structures should go to the head/tail of the list,
respectively.
:dt.fInvalidateFieldInfo
:dd.specifies whether or not the fields are to be invalidated.
:dt.cFieldInfoInsert
:dd.specifies the number of :hp2.FIELDINFO:ehp2. structures being inserted.
:edl.
:p.Finally, changing the view to detail view is as simple as - you guessed it
- sending the control a :hp2.CM_SETCNRINFO:ehp2. message.
:font facename=Courier size=14x14.
:xmp.
   CNRINFO ciInfo;

   ciInfo.flWindowAttr=CV_DETAIL;
   WinSendMsg(hwndCnr,
              CM_SETCNRINFO,
              MPFROMP(&amp.ciInfo),
              MPFROMLONG(CMA_FLWINDOWATTR));
:exmp.
:font facename=Helv size=12x12.
:p.Figure 3.  Changing to the detail view.
:font facename=default.
:p.Note that, even if you initialize the :hp1.pTitleData:ehp1. to point to the
column heading, the titles are not displayed unless you specify
:hp2.CA_DETAILSVIEWTITLES:ehp2. in the :hp1.flWindowAttr:ehp1. field.
:h3.Direct Editing
:p.Direct editing is accomplished by pressing the proper combination of keys
and/or mouse buttons as defined in the &odq.Mappings&cdq.  page of the
&odq.Mouse&cdq.  settings (in the &odq.OS/2 System&cdq./&odq.System Setup&cdq.
folder) while the mouse is over a directly-editable region.  When this is
done, a multi-line edit control appears and is initialized with the current
text, in which you can make your changes; the :hp2.enter:ehp2. key acts as a
newline, while the :hp2.pad enter:ehp2. key completes the editing operation
and (normally) applies the changes.
:p.From a programming perspective, three notifications are sent to the
application whenever direct-editing is requested by the user when over a
non-read-only field (&odq.field&cdq. is used here to mean any text string and
not as it was defined in the discussion of the detail view) -
:hp2.CN_BEGINEDIT:ehp2., :hp2.CN_REALLOCPSZ:ehp2., and :hp2.CN_ENDEDIT:ehp2.
(in that order).  For all three, :hp1.mpParm2:ehp1. points to a
:hp2.CNREDITDATA:ehp2. structure which describes the state of the record being
edited.  The purpose of :hp2.CN_BEGINEDIT:ehp2. and :hp2.CN_ENDEDIT:ehp2. is
to notify the user that editing is about to begin/end.  However, only the
:hp2.CN_REALLOCPSZ:ehp2. is important, since the former two can be ignored
while the latter cannot.
:font facename=Courier size=14x14.
:xmp.
typedef struct _CNREDITDATA &lbrc.
   ULONG cb;
   HWND hwndCnr;
   PRECORDCORE pRecord;
   PFIELDINFO pFieldInfo;
   PSZ *ppszText;
   ULONG cbText;
   ULONG id;
&rbrc. CNREDITDATA;
:exmp.
:font facename=Helv size=12x12.
:p.Figure 4.  The :hp2.CNREDITDATA:ehp2. structure.
:font facename=default.
:dl tsize=20 break=fit.
:dt.cb
:dd.specifies the size of the structure in bytes.
:dt.hwndCnr
:dd.specifies the handle of the container.
:dt.pRecord
:dd.points to the record being edited.
:dt.pFieldInfo
:dd.points to the :hp2.FIELDINFO:ehp2. structure describing the field being
edited.
:dt.ppszText
:dd.points to the pointer to the text being edited.
:dt.cbText
:dd.specifies the size of the text.
:dt.id
:dd.specifies which part of the container contains the text to be edited and
can be one of the following&colon.  :hp2.CID_CNRTITLEWND:ehp2.,
:hp2.CID_LEFTDVWND:ehp2., :hp2.CID_RIGHTDVWND:ehp2.,
:hp2.CID_LEFTCOLTITLEWND:ehp2., or :hp2.CID_RIGHTCOLTITLEWND:ehp2..
:edl.
:p.The :hp2.CN_REALLOCPSZ:ehp2. indicates that editing is about to end and
that the application should allocate a new block of memory to contain the
text.  :hp1.ppszText:ehp1. double-points to the old text and :hp1.cbText:ehp1.
specifies the length of the new text.  If a new memory block is allocated, the
pointer to the new memory block must be stored in :hp1.ppszText:ehp1..
Returning :hp2.TRUE:ehp2. from this notification indicates that
:hp1.ppszText:ehp1.  points to a memory block sufficiently large enough to
hold :hp1.cbText:ehp1.  bytes and that the container should copy the new text
to this buffer.  (I am not sure if :hp1.cbText:ehp1.  includes the null
terminator - &osq.&bsl.0&csq.)  Returning :hp2.FALSE:ehp2.  indicates that the
changes should not be copied and should be discarded.
:h3.Altered States
:p.As defined by CUA &apos.91 (I think), an object in general can be in one or
more of five states (or none at all) - source, target, in-use, cursored, and
selected.  A container record stores information on its current state in the
:hp1.flRecordAttr:ehp1. (in both the :hp2.RECORDCORE:ehp2. and
:hp2.MINIRECORDCORE:ehp2. structures) in the form of :hp2.CRA_*:ehp2.
constants.  Setting the state, however, is not a simple matter of setting this
field, since the container will have no way of knowing that you&apos.ve
changed the field.  Instead, you send the container a
:hp2.CM_SETRECORDEMPHASIS:ehp2. message which updates this field in the record
and updates the display of that record on the screen.
:p.Those who are &odq.on the ball&cdq. will notice that there is no CRA_SOURCE
constant defined in the 2.0 Toolkit.  This was inadvertently left out and
should be defined to be 0x00004000L in :hp2.pmstddlg.h:ehp2..
:p.So what do all of these states mean?
:dl tsize=20 break=fit.
:dt.CRA_CURSORED
:dd.the record has the input focus.
:dt.CRA_INUSE
:dd.the record (and thus the object) is in use by the application.
:dt.CRA_SELECTED
:dd.the record is selected for later manipulation.
:dt.CRA_SOURCE
:dd.the record is a source for a direct-manipulation operation.
:dt.CRA_TARGET
:dd.the record is a target for a direct-manipulation operation.
:edl.
:p.When you want to query all records with a particular emphasis type, you use
the :hp2.CM_QUERYRECORDEMPHASIS:ehp2. message.  This returns the next record
that has the specifies emphasis (or :hp2.NULL:ehp2. if none exists).
:p.:hp2.Popup Menus:ehp2.
:p.If you take a close look at the Workplace Shell, you will see all of these
states used in one way or another.  A more interesting use is in conjunction
with popup menus; if the record underneath the mouse is not selected, it alone
is given source emphasis.  If it is selected, all records that are selected
are given source emphasis.  If no record is underneath the mouse, the
container itself is given source emphasis.  After the appropriate record
states have been changed, :hp2.WinPopupMenu():ehp2. is called.  Finally, the
:hp2.WM_MENUEND:ehp2. message is intercepted to &odq.un-source&cdq. the
records that were changed.  Broken down into pseudo-code, this becomes&colon.
:font facename='Tms Rmn' size=16x14.
:ol compact.
:li.Determine if the mouse is over a container record
:ul compact.
:li.If so, check the selection state
:ul compact.
:li.If the record is selected, add source emphasis to all selected records
:li.If the record is not selected, give it source emphasis only
:eul.
:li.If not, select the enter container
:eul.
:li.Call :hp2.WinPopupMenu():ehp2.
:li.Undo source emphasis changes
:eol.
:font facename=default.
:p.While determining if the mouse is over a record is easy when processing the
:hp2.WM_CONTROL:ehp2. message, it is a bit more difficult when in the
:hp2.WM_CONTEXTMENU:ehp2. menu.  The solution, it would appear from looking at
our arsenal of messages that we can send to the container, would be to send
the container a :hp2.CM_QUERYRECORDFROMRECT:ehp2. message, specifying the
mouse position as the rectangle to query.  Looking a bit closer at the
documentation reveals that the rectangle has to be specified in
:hp2.virtual:ehp2.  coordinates.  What???
:p.:hp2.Virtual Coordinates:ehp2.
:p.Okay, okay, everyone has probably heard of and is vaguely familiar with
virtual coordinates, or else you would not be in PM programming to begin with.
The container&apos.s notion of the origin in its coordinate system is somewhat
awry, unfortunately, and this confuses things; the origin is defined to be the
screen coordinate of the lower left corner of the container at the time the
last :hp2.CM_ARRANGE:ehp2. message was sent.
:p.So, you either have to keep track of when you send the container a
:hp2.CM_ARRANGE:ehp2.  message and perform all sorts of hocus pocus to
remember where the origin is supposed to be, or you can finish reading this
sentence and discover that the documentation for
:hp2.CM_QUERYRECORDFROMRECT:ehp2. is flat-out wrong.  The rectangle specified
in this message is in window coordinates.  :hp2.Whew!:ehp2.  That greatly
simplifies things, except that when in detail view the record returned is the
one above the one the mouse is over.  Oh boy.  Fortunately, we can calculate
the height of a record using the :hp2.CM_QUERYRECORDRECT:ehp2. message, which
we use to adjust the mouse position before calling
:hp2.CM_QUERYRECORDFROMRECT:ehp2..
:p.Now that we have the record underneath the mouse, we can check its
selection state by examining the :hp1.flRecordAttr:ehp1. field.  If the record
is selected, it is probably more efficient to use the
:hp2.CM_QUERYRECORDEMPHASIS:ehp2. message to get all selected records, but we
already have this exquisite recursive search function, so I used that instead.
Another example of poor documentation is in :hp2.CM_SETRECORDEMPHASIS:ehp2.
where it does :hp2.not:ehp2. tell you that you can set the container&apos.s
source emphasis by specifying :hp2.NULL:ehp2. for the record.
:p.Finally, we call :hp2.WinPopupMenu():ehp2. and undo the source emphasis and
- voila! - we&apos.re done.
:h3.CNR3 - A Sample Application Revisited
:p.CNR3 builds upon CNR2 by adding detail view support, direct editing
support, and &odq.proper&cdq. popup menu support.  As with part II, landmarks
have been added to CNR3.C which are to point out things of interest.  These
landmarks are described below.
:p.:hp2.Landmark 1:ehp2.
:p.This is to point out the additions of the last four fields to the
:hp2.MYCNREDIT:ehp2. structure and the addition of the :hp1.pmcrMenu:ehp1.
field to the :hp2.CLIENTDATA:ehp2. structure.
:p.:hp2.Landmark 2:ehp2.
:p.This points out the allocation, initialization, and insertion of the
:hp2.FIELDINFO:ehp2. structures.
:p.:hp2.Landmark 3:ehp2.
:p.This points out the new procedure for handling the context menu.
:p.:hp2.Landmark 4:ehp2.
:p.This points out the correction for the bug in the
:hp2.CM_QUERYRECORDFROMRECT:ehp2. message when in details view as described
above.
:p.:hp2.Landmark 5:ehp2.
:p.This points out the processing of the :hp2.CN_REALLOCPSZ:ehp2.
notification.
:p.:hp2.Landmark 6:ehp2.
:p.This points out the addition of the detail view menu item.
:h3.Summary
:p.This month we learned a lot of things, namely how to setup the details
view, how direct editing is performed and what the container expects from the
application with regards to this, and how selection states are set, queried,
and used.  We also saw how inadequate the documentation is when it contains so
many examples of incorrect or incomplete information.
:p.Now you have enough information to use the container well.  However,
we&apos.re not done yet; next month, I will try to figure out some of the more
advanced capabilities of the container such as record sharing and deltas.
Stay tuned, same Bat-time, same Bat-channel!
:p.:artwork name='batlogo.bmp' align=center.
