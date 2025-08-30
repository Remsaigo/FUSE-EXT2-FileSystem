#ifndef _TYPES_H_
#define _TYPES_H_


/******************************************************************************
* SECTION: Type def
*******************************************************************************/
typedef int          boolean;
typedef uint16_t     flag16;

typedef enum newfs_file_type {
    NEWFS_FILE,
    NEWFS_DIR,
} NEWFS_FILE_TYPE;
/******************************************************************************
* SECTION: Macro

*******************************************************************************/
#define TRUE                    1
#define FALSE                   0
#define UINT32_BITS             32
#define UINT8_BITS              8

#define NEWFS_MAGIC_NUM           0x52415453  
#define NEWFS_SUPER_OFS           0
#define NEWFS_ROOT_INO            0

#define NEWFS_ERROR_NONE          0
#define NEWFS_ERROR_ACCESS        EACCES
#define NEWFS_ERROR_SEEK          ESPIPE     
#define NEWFS_ERROR_ISDIR         EISDIR
#define NEWFS_ERROR_NOSPACE       ENOSPC
#define NEWFS_ERROR_EXISTS        EEXIST
#define NEWFS_ERROR_NOTFOUND      ENOENT
#define NEWFS_ERROR_UNSUPPORTED   ENXIO
#define NEWFS_ERROR_IO            EIO     /* Error Input/Output */
#define NEWFS_ERROR_INVAL         EINVAL  /* Invalid Args */

#define NEWFS_MAX_FILE_NAME       128
#define NEWFS_INODE_PER_FILE      1
#define NEWFS_DATA_PER_FILE       4
#define NEWFS_DEFAULT_PERM        0777

#define NEWFS_IOC_MAGIC           'S'
#define NEWFS_IOC_SEEK            _IO(NEWFS_IOC_MAGIC, 0)

#define NEWFS_FLAG_BUF_DIRTY      0x1
#define NEWFS_FLAG_BUF_OCCUPY     0x2 
 
#define NEWFS_SUPER_BLKS          1
#define NEWFS_MAP_INODE_BLKS      1
#define NEWFS_MAP_DATA_BLKS       1

#define NEWFS_INODE_BLKS          256
#define NEWFS_DATA_BLKS           3837

/******************************************************************************
* SECTION: Macro Function
*******************************************************************************/

#define NEWFS_IO_SZ()                     (newfs_super.sz_io)
#define NEWFS_BLK_SZ()                    (newfs_super.sz_blks)
#define NEWFS_DISK_SZ()                   (newfs_super.sz_disk)
#define NEWFS_DRIVER()                    (newfs_super.fd)
#define NEWFS_BLKS_SZ(blks)               ((blks) * NEWFS_BLK_SZ())
#define NEWFS_MAX_DENTRY_BLK()            (NEWFS_BLK_SZ() / sizeof(struct newfs_dentry))

#define NEWFS_ROUND_DOWN(value, round)    ((value) % (round) == 0 ? (value) : ((value) / (round)) * (round))
#define NEWFS_ROUND_UP(value, round)      ((value) % (round) == 0 ? (value) : ((value) / (round) + 1) * (round))

#define NEWFS_ASSIGN_FNAME(pnewfs_dentry, _fname)   memcpy(pnewfs_dentry->fname, _fname, strlen(_fname))

#define NEWFS_INO_OFS(ino)                (newfs_super.inode_offset + NEWFS_BLKS_SZ(ino))
#define NEWFS_DATA_OFS(dno)               (newfs_super.data_offset + NEWFS_BLKS_SZ(dno))

#define NEWFS_IS_DIR(pinode)              (pinode->dentry->ftype == NEWFS_DIR)
#define NEWFS_IS_REG(pinode)              (pinode->dentry->ftype == NEWFS_FILE)

struct newfs_dentry;
struct newfs_inode;
struct newfs_super;


struct custom_options {
	const char*        device;
};

struct newfs_inode {
    int           ino;                                  /* 在inode位图中的下标 */
    int                size;                            /* 文件已占用空间 */
    int                link;
    int                block_pointer[NEWFS_DATA_PER_FILE]; //数据块块号
    int                dir_cnt;                         //目录项下几个子文件
    struct newfs_dentry* dentry;                        /* 指向该inode的dentry */
    struct newfs_dentry* dentrys;                       /* 目录项链表头 */
    uint8_t*           data[NEWFS_DATA_PER_FILE];      // 指向数据块
    NEWFS_FILE_TYPE          ftype;
};

struct newfs_dentry {
    int ino;
    char               fname[NEWFS_MAX_FILE_NAME];
    struct newfs_dentry* parent;                        /* 父亲Inode的dentry */
    struct newfs_dentry* brother;                       /* 兄弟 */
    struct newfs_inode*  inode;                         /* 指向inode */
    NEWFS_FILE_TYPE      ftype;
};

struct newfs_super {
    uint32_t magic;             //huanshu
    int      fd;
    /* TODO: Define yourself */
    int                sz_io;       //驱动io大小
    int                sz_disk;        //虚拟磁盘sz
    int                sz_usage;
    int                sz_blks;         //磁盘块sz
    //索引节点
    int                max_ino;         //索引节点最大数量
    uint8_t*           map_inode;       
    int                map_inode_blks; //索引位图块数
    int                map_inode_offset;  //位图偏移
    int                inode_offset;    // 索引节点的起始地址
    //数据块
    uint8_t*           map_data;        
    int                max_data;        //数据块最大数量
    int                data_offset;     //数据起始地址
    int                map_data_offset; // data位图的起始地址
    int                map_data_blks;   // data位图所占的块数
    
    boolean            is_mounted;
    struct newfs_dentry* root_dentry;
};
static inline struct newfs_dentry* new_dentry(char * fname, NEWFS_FILE_TYPE ftype) {
    struct newfs_dentry * dentry = (struct newfs_dentry *)malloc(sizeof(struct newfs_dentry));
    memset(dentry, 0, sizeof(struct newfs_dentry));
    NEWFS_ASSIGN_FNAME(dentry, fname);
    dentry->ftype   = ftype;
    dentry->ino     = -1;
    dentry->inode   = NULL;
    dentry->parent  = NULL;
    dentry->brother = NULL;  
    return dentry;                                          
}

/******************************************************************************
* SECTION: FS Specific Structure - Disk structure
*******************************************************************************/
struct newfs_super_d
{
    uint32_t           magic_num;
    int                sz_usage;

    int                map_inode_blks;      // inode位图块数
    int                map_inode_offset;    // inode位图起始地址

    int                map_data_offset;     // data位图起始地址
    int                map_data_blks;       // data位图块数

    int                data_offset;         // 数据块起始地址
    int                inode_offset;        // 索引节点起始地址
};

//结构体大小为36字节
struct newfs_inode_d
{
    uint32_t           ino;                           /* 在inode位图中的下标 */
    uint32_t           size;                          /* 文件已占用空间 */
    int                link;
    int                block_pointer[NEWFS_DATA_PER_FILE];// 数据块指针 
    uint32_t           dir_cnt;
    NEWFS_FILE_TYPE      ftype;
};  

struct newfs_dentry_d
{
    char               fname[NEWFS_MAX_FILE_NAME];
    NEWFS_FILE_TYPE      ftype;
    uint32_t           ino;                           /* 指向的ino号 */
};  

#endif /* _TYPES_H_ */