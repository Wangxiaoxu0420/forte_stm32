/************************************************************************************
 * Copyright (c) 2016 fortiss GmbH
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 * Milan Vathoopan, Guru Chandrasekhara - initial API and implementation and/or initial documentation
 ************************************************************************************/

#ifndef SRC_ARCH_FREERTOS_SOCKHAND_H_
#define SRC_ARCH_FREERTOS_SOCKHAND_H_

#include "opt.h"
#include "sockets.h"
#include "api.h"
#include "sys.h"
#include "igmp.h"
#include "inet.h"
#include "tcp.h"
#include "raw.h"
#include "udp.h"
#include "tcpip.h"

#include <string.h>

#include "devlog.h"

#undef connect //gets confused with connect function of conn.h and childs

inline int connect(int s, const struct sockaddr *name, socklen_t namelen) {
  return lwip_connect(s, name, namelen);
}

//these include needs to be last
#include "../gensockhand.h"
#include "../fdselecthand.h"
#include "../bsdsocketinterf.h"

typedef CGenericIPComSocketHandler<CFDSelectHandler, CBSDSocketInterface> CIPComSocketHandler;

#endif /* SRC_ARCH_FREERTOS_SOCKHAND_H_ */

