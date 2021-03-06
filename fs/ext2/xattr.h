/*
  File: linux/ext2_xattr.h

  On-disk format of extended attributes for the ext2 filesystem.

  (C) 2001 Andreas Gruenbacher, <a.gruenbacher@computer.org>
*/

#include <linux/config.h>
#include <linux/init.h>
#include <linux/xattr.h>

/* Magic value in attribute blocks */
#define EXT2_XATTR_MAGIC		0xEA020000

/* Maximum number of references to one attribute block */
#define EXT2_XATTR_REFCOUNT_MAX		1024

/* Name indexes */
#define EXT2_XATTR_INDEX_USER			1
#define EXT2_XATTR_INDEX_POSIX_ACL_ACCESS	2
#define EXT2_XATTR_INDEX_POSIX_ACL_DEFAULT	3
#define EXT2_XATTR_INDEX_TRUSTED		4
#define	EXT2_XATTR_INDEX_LUSTRE			5
#define EXT2_XATTR_INDEX_SECURITY	        6

/**
 * 所有扩展属性保存在单独的地方。
 * 该结构代表文件的扩展属性块描述符。
 */
struct ext2_xattr_header {
	/**
	 * 魔术值，对ext2来说，是EXT2_XATTR_MAGIC
	 */
	__le32	h_magic;	/* magic number for identification */
	/**
	 * 引用计数。当有某一个文件引用了其中的属性时，加1.
	 */
	__le32	h_refcount;	/* reference count */
	/**
	 * 占用多少个属性块。
	 */
	__le32	h_blocks;	/* number of disk blocks used */
	/**
	 * 所有属性的Hash值，加快索引速度。
	 */
	__le32	h_hash;		/* hash value of all attributes */
	__u32	h_reserved[4];	/* zero right now */
};

/**
 * ext2索引节点增强属性。存放在单独的磁盘块首部中。
 */
struct ext2_xattr_entry {
	/**
	 * 属性名称长度。
	 */
	__u8	e_name_len;	/* length of name */
	/**
	 * 属性索引，供文件引用。
	 */
	__u8	e_name_index;	/* attribute name index */
	/**
	 * 在属性块中的位置。
	 */
	__le16	e_value_offs;	/* offset in disk block of value */
	/**
	 * 位于哪一个属性块。
	 */
	__le32	e_value_block;	/* disk block attribute is stored on (n/i) */
	/**
	 * 属性值的长度。
	 */
	__le32	e_value_size;	/* size of attribute value */
	/**
	 * 属性名及属性值的哈希值。
	 */
	__le32	e_hash;		/* hash value of name and value */
	char	e_name[0];	/* attribute name */
};

#define EXT2_XATTR_PAD_BITS		2
#define EXT2_XATTR_PAD		(1<<EXT2_XATTR_PAD_BITS)
#define EXT2_XATTR_ROUND		(EXT2_XATTR_PAD-1)
#define EXT2_XATTR_LEN(name_len) \
	(((name_len) + EXT2_XATTR_ROUND + \
	sizeof(struct ext2_xattr_entry)) & ~EXT2_XATTR_ROUND)
#define EXT2_XATTR_NEXT(entry) \
	( (struct ext2_xattr_entry *)( \
	  (char *)(entry) + EXT2_XATTR_LEN((entry)->e_name_len)) )
#define EXT2_XATTR_SIZE(size) \
	(((size) + EXT2_XATTR_ROUND) & ~EXT2_XATTR_ROUND)

# ifdef CONFIG_EXT2_FS_XATTR

extern struct xattr_handler ext2_xattr_user_handler;
extern struct xattr_handler ext2_xattr_trusted_handler;
extern struct xattr_handler ext2_xattr_acl_access_handler;
extern struct xattr_handler ext2_xattr_acl_default_handler;
extern struct xattr_handler ext2_xattr_security_handler;

extern ssize_t ext2_listxattr(struct dentry *, char *, size_t);

extern int ext2_xattr_get(struct inode *, int, const char *, void *, size_t);
extern int ext2_xattr_set(struct inode *, int, const char *, const void *, size_t, int);

extern void ext2_xattr_delete_inode(struct inode *);
extern void ext2_xattr_put_super(struct super_block *);

extern int init_ext2_xattr(void);
extern void exit_ext2_xattr(void);

extern struct xattr_handler *ext2_xattr_handlers[];

# else  /* CONFIG_EXT2_FS_XATTR */

static inline int
ext2_xattr_get(struct inode *inode, int name_index,
	       const char *name, void *buffer, size_t size)
{
	return -EOPNOTSUPP;
}

static inline int
ext2_xattr_set(struct inode *inode, int name_index, const char *name,
	       const void *value, size_t size, int flags)
{
	return -EOPNOTSUPP;
}

static inline void
ext2_xattr_delete_inode(struct inode *inode)
{
}

static inline void
ext2_xattr_put_super(struct super_block *sb)
{
}

static inline int
init_ext2_xattr(void)
{
	return 0;
}

static inline void
exit_ext2_xattr(void)
{
}

#define ext2_xattr_handlers NULL

# endif  /* CONFIG_EXT2_FS_XATTR */

