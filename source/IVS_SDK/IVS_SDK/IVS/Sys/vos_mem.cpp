#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "vos.h"


VOID *VOS_malloc( ULONG size )
{
    CHAR *buf = VOS_NULL ;

    buf = ( CHAR *)malloc( size );//lint !e838
    if( NULL == buf )
    {
        return VOS_NULL ;
    }
    memset( buf , 0x00 , size );

    return buf ;
}

VOID VOS_free( VOID *buff )
{
    free( buff );
    buff = VOS_NULL ;

    return ;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

