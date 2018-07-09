:h2.Programming the Container Control (Part II)
:p.Written by Larry Salomon, Jr.
:h3.Where were we?
:p.Last month I briefly introduced (in that oh-so-poor writing style that
makes me unique) the container control and some of the programming basics
that everyone should know.  This month, we will continue this trek into the
unknown by describing the :hp2.tree:ehp2. view as well as some other nifty
things.  Also, we will write our first application using the container, on
which we will build in next month&apos.s installment.
:p.:hp2.From Another Viewpoint:ehp2.
:p.As a quick note, the :hp2.name:ehp2. and :hp2.text:ehp2. views were omitted
last month because they are - from a programmer&apos.s perspective - identical
to the icon view.
:h3.The Wood Nymph&apos.s Delight
:p.The :hp2.tree:ehp2. view should be a highly familiar one; it parallels
nicely with the directory structure of a hard disk and has been used in the
File Manager (from OS/2 1.x) and the Help Manager as well as many other
applications.  It also has three subviews that can be specified in the
:hp2.CM_SETCNRINFO:ehp2. message&colon.  tree :hp2.icon:ehp2. view, tree
:hp2.name:ehp2. view, and tree :hp2.text:ehp2. view (these are specified by
&odq.or-ing&cdq. the CV_ICON, CV_NAME, or CV_TEXT flags with the CV_TREE flag
in the :hp1.flWindowAttr:ehp1. field, respectively).  The difference between
these views is in the way the data is represented&colon.  icon view displays
an icon with the text to the right and to the left is a separate bitmap
indicating whether the item is expanded or collapsed (if applicable); name
view is the same as icon view except that the expanded/collapsed indicator is
indicated in the icon; the text view provides only the text string with the
expanded/collapsed indicator as a separate bitmap to the left.
:p.Note the difference between icon and name view.  Remember when, in the
beginning of the series, it was mentioned that there are a few differences
between using the :hp2.MINIRECORDCORE:ehp2. and :hp2.RECORDCORE:ehp2.
structure?  This is one of those differences; since only the
:hp2.RECORDCORE:ehp2. structure has different fields for the expanded and
collapsed icons, the name view cannot be used with the
:hp2.CCS_MINIRECORDCORE:ehp2. style.
:p.When inserting records into the container, the parent record is specified
in the :hp1.pRecordParent:ehp1. field of the :hp2.RECORDINSERT:ehp2.
structure.  In the icon view, we specified this as :hp2.NULL:ehp2., since
there is not parent/child relationships in that view.  However, in the tree
views, this is particularly important because it specifies how the records are
to be displayed.  All records that have children will be displayed by adding
an expanded/collapsed indicator to its left.  If, by some action of the user,
you switch to any other non-tree view, all but the top-level records are
hidden.
:p.:hp2.Notifications:ehp2.
:p.In addition to the &odq.normal&cdq. notifications, the container provides 2
notifications specific to the tree view&colon.
:dl tsize=20 break=fit.
:dt.CN_COLLAPSETREE
:dd.This is sent to the owner after the container collapses a subtree.
:hp1.mpParm2:ehp1. points to the record that was collapsed.
:dt.CN_EXPANDTREE
:dd.This is sent to the owner after the container expands a subtree.
:hp1.mpParm2:ehp1. points to the record that was expanded.
:edl.
:h3.CNR2 - A Sample Application
:p.Now let us delve into the depths of our first sample application; it not
only showcases the container control, but it also contains some nifty tricks
that I think you&apos.ll find useful.  The structure is that of a
&odq.typical&cdq. PM application and it should not be new to you.  In the
code, I have placed several &odq.landmarks&cdq.; these are marked by placing a
comment of the form &odq.@n&cdq. (where &osq.n&csq. is the landmark number)
starting at column 55.  These landmarks are used to point out things of
interest and are discussed in more detail later.
:p.The application does nothing more than add a record for each month of each
year for the number of years specified in :hp2.NUM_YEARS:ehp2..  This is to
show how the tree view is used.  A popup menu is displayed whenever the system
key/mouse sequence is pressed, allowing to you switch between icon and tree
views, etc.
:p.:hp2.Landmark 1:ehp2.
:p.This is simply to point out the typedefs used.
:font facename=Courier size=14x14.
:xmp.
typedef struct _CLIENTDATA &lbrc.                          // @1
   USHORT usSzStruct;         // Size of the structure
   HAB habAnchor;             // Anchor block of the window
   HWND hwndFrame;            // Frame of the client (== parent)
   HWND hwndCnr;              // Container window
   HPOINTER hptrYear;         // Icon for the year records
   HPOINTER hptrMonth;        // Icon for the month records
   HWND hwndWndMenu;          // Menu window
&rbrc. CLIENTDATA, *PCLIENTDATA;

typedef struct _MYCNRREC &lbrc.
   MINIRECORDCORE mrcCore;    // Base structure
   CHAR achText&lbrk.64&rbrk.;          // Icon text
&rbrc. MYCNRREC, *PMYCNRREC;

//-------------------------------------------------------------------------
// PFNSRCH is used by searchCnr().  The parameters are:  container window
// handle, record pointer, and user-data pointer, respectively.
//-------------------------------------------------------------------------
typedef BOOL (*PFNSRCH)(HWND,PVOID,PVOID);
:exmp.
:font facename=default.
:p.:hp2.CLIENTDATA:ehp2. is the instance data for the client window.  Granted,
we could have used global variables, but that is poor programming practice so
we avoid doing this (the only global we use is an constant array of pointers
to the names of the months).
:p.:hp2.MYCNRREC:ehp2. is the definition for the container records.  Note that
we are using the :hp2.MINIRECORDCORE:ehp2. structure.
:p.:hp2.PFNSRCH:ehp2. is a pointer to a function accepting three parameters
and returning a boolean.  It is used in a very interesting way that
you&apos.ll see later.
:p.:hp2.Landmark 2:ehp2.
:p.This is only to point out that while it is highly recommended that you
allocate and insert as many records as possible each time sometimes it simply
isn&apos.t possible.  We :hp2.have:ehp2. to allocate/insert each year
separately followed by the twelve months.
:p.:hp2.Landmark 3:ehp2.
:p.Since we specified :hp2.FALSE:ehp2. in the :hp1.fInvalidateRecord:ehp1.
field of the :hp2.RECORDINSERT:ehp2. structure, we have to send a
:hp2.CM_INVALIDATERECORD:ehp2. message to update the container.
:p.:hp2.Landmark 4:ehp2.
:p.This entire procedure is useful, since (as described in a previous issue of
the magazine) you cannot specify :hp2.MIS_CONDITIONALCASCADE:ehp2. in a
resource file.  It should also be noted that since the cascade button takes up
a bit more space, it is helpful to add a few blank spaces in the menu template
to account for this.  As a general rule, I use three blanks.
:p.:hp2.Landmark 5:ehp2.
:p.Here is another useful procedure; it performs a recursive, post-traversal
search of the container, calling a user-specified procedure at each record to
see if it matches the criteria also specified by the caller.  If you&apos.ve
ever wondered what a use for reserved parameter is, here is one.  We use the
reserved parameter to specify the record we are currently checking; by having
the user specify :hp2.NULL:ehp2., we can check to see if this is the initial
call.
:p.Of course, we could have eliminated the reserved parameter and had this
procedure call another procedure with a reserved parameter to remove the
burden from the caller, but that&apos.s too easy.  (* grin *)
:p.:hp2.Landmark 6:ehp2.
:p.This is to point out the allocation and initialization of the client
instance data.  Note that the cleanup is done in the :hp2.WM_DESTROY:ehp2.
processing.
:p.:hp2.Landmark 7:ehp2.
:p.This is to show a bug in the container - if the user uses the mouse to
invoke the popup menu, the container sends us a :hp2.WM_CONTROL:ehp2. message.
Not so if the user uses the keyboard, thus we duplicate the code and check for
the :hp2.WM_CONTEXTMENU:ehp2. message.
:p.:hp2.Landmark 8:ehp2.
:p.Finally, we invoke the :hp2.searchCnr():ehp2. function specifying a pointer
to a boolean that contains the desired select state.  The
:hp2.searchSelect():ehp2. function is an interesting one.
:font facename=Courier size=14x14.
:xmp.
BOOL searchSelect(HWND hwndCnr,PMYCNRREC pmcrRecord,PBOOL pbSelect)
//-------------------------------------------------------------------------
// This function is used to select/deselect all records.  Note that it
// always returns FALSE, so that searchCnr() will traverse the entire
// record list.
//
// Input:  hwndCnr - handle of the container window
//         pmcrRecord - pointer to the container record
//         pbSelect - pointer to a BOOL specifying whether to select the
//                    record or not
//-------------------------------------------------------------------------
&lbrc.
   WinSendMsg(hwndCnr,
              CM_SETRECORDEMPHASIS,
              MPFROMP(pmcrRecord),
              MPFROM2SHORT(*pbSelect,CRA_SELECTED));
   return FALSE;
&rbrc.
:exmp.
:font facename=default.
:p.See how it simply sets the record&apos.s select state and always returns
:hp2.FALSE:ehp2. - indicating that this record does not match - forcing
:hp2.searchCnr():ehp2. to traverse the entire container.
:p.:hp2.That&apos.s It!:ehp2.
:p.That is all there is to it!  Note that while there is a lot of setup
involved, the container is no more difficult to use than any of the other
&odq.standard&cdq. controls.
:h3.Summary
:p.We have seen how tree views can be used for hierarchical structures and how
they are not much different that the icon, name, and text views described last
month.  We have used these concepts and introduced new ones in a sample
application which will be used as a scaffold to add new features in future
installments of this series.
:p.Next month we will describe the details view, selection states, direct
editing, and possibly other ideas and will incorporate these concepts into our
sample application.
