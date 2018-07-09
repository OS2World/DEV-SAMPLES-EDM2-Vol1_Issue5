#define WC_PAPERPAGE             "PaperPageClass"

#define PPS_BORDER               0x00000001L
#define PPS_HOLESLEFT            0x00000000L
#define PPS_HOLESRIGHT           0x00000002L
#define PPS_HOLESNONE            0x00000004L

#define PPM_BASE                 WM_USER+320
#define PPM_QUERYMAXLINES        (PPM_BASE)
#define PPM_QUERYCURRENTLINE     (PPM_BASE+1)
#define PPM_QUERYLINERECT        (PPM_BASE+2)
#define PPM_QUERYNUMHOLES        (PPM_BASE+3)
#define PPM_QUERYHOLERECT        (PPM_BASE+4)
#define PPM_QUERYLINETEXT        (PPM_BASE+5)
#define PPM_QUERYLINETEXTLEN     (PPM_BASE+6)
#define PPM_QUERYPAPERTEXT       (PPM_BASE+7)
#define PPM_QUERYPAPERTEXTLEN    (PPM_BASE+8)
#define PPM_SETCURRENTLINE       (PPM_BASE+9)
#define PPM_SETLINETEXT          (PPM_BASE+10)
#define PPM_SETPAPERTEXT         (PPM_BASE+11)
#define PPM_CONVERTPOINTS        (PPM_BASE+12)

#define PPN_UP                   (USHORT)1
#define PPN_DOWN                 (USHORT)2
#define PPN_BEGINPAGE            (USHORT)3
#define PPN_ENDPAGE              (USHORT)4
#define PPN_CLICKED              (USHORT)5
#define PPN_DBLCLICKED           (USHORT)6
#define PPN_CONTEXTMENU          (USHORT)7
#define PPN_HELP                 (USHORT)8
#define PPN_SETFOCUS             (USHORT)9
#define PPN_KILLFOCUS            (USHORT)10

#define PPR_CID_ENTRYFIELD       256

BOOL PprInitialize(HAB habAnchor);
BOOL PprTerminate(HAB habAnchor);
