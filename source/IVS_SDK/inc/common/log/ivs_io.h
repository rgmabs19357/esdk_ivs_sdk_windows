/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  ivs_io.h
//  文件说明:   文件操作平台宏定义
//  版    本:   IVS V100R002C01
//  作    者:   w00210470
//  创建日期:   2012/11/12
//  修改标识：
//  修改说明：
*******************************************************************************/
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef  _MSC_VER
# include <io.h>
# include <direct.h>
#else
# include <unistd.h>
#endif

#ifdef _MSC_VER
#  define IVS_open    _open
#  define IVS_close   _close
#  define IVS_lseek   _lseek
#  define IVS_read    _read
#  define IVS_write   _write
#  define IVS_stat    _stat
#  define IVS_STATE   struct _stat
#  define IVS_access  _access
#  define IVS_mkdir(dir)   _mkdir(dir)
#  define IVS_rename  rename
#  define IVS_unlink  _unlink
#  define IVS_fsync   _commit

#else

#  define IVS_open    open
#  define IVS_close   close
#  define IVS_lseek   lseek
#  define IVS_read    read
#  define IVS_write   write
#  define IVS_stat    stat
#  define IVS_STATE   struct stat
#  define IVS_access  access
#  define IVS_mkdir(dir)   mkdir(dir, S_IWRITE|S_IREAD)
#  define IVS_rename  rename
#  define IVS_unlink  unlink
#  define IVS_fsync   fsync
#endif 





