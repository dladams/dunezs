// ZeroSuppressServiceBase.h
//
// David Adams
// November 2015
//
// Service wrapper for the ZeroSuppressBase interface.
//
// This must be include in all headers for all services that
// implement the ZeroSuppressBase interface.

#ifndef ZeroSuppressServiceBase_H
#define ZeroSuppressServiceBase_H

#include "DZSCore/ZeroSuppressBase.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

DECLARE_ART_SERVICE_INTERFACE(ZeroSuppressBase, LEGACY)

#endif
