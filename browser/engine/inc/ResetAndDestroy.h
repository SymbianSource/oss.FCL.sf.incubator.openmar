/*
 * Helper classes
 */

//#include <ecom/ImplementationInformation.h>

/*
 * Function passed in as a parameter to TCleanupItem constructor to reset and 
 * destroy the received aPtr.
 * @param aPtr A pointer to the object that is to be cleaned up
 */
inline void ResetAndDestroy(TAny* aPtr)
{
    reinterpret_cast<RImplInfoPtrArray*>(aPtr)->ResetAndDestroy();
}

/*
 * Function that creates a cleanup item for aArray and specifies the cleanup
 * function as ResetAndDestroy. When the item is removed from the cleanup stack
 * the function ResetAndDestroy will be called on aArray.
 * @param aArray The array to be destroyed.
 */
inline void CleanupResetAndDestroyPushL(RImplInfoPtrArray& aArray)
{       
    TCleanupItem item(ResetAndDestroy, &aArray);
    CleanupStack::PushL(item);
}
