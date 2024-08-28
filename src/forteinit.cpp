/*******************************************************************************
 * Copyright (c) 2011, 2014 Profactor GmbH
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Gerhard Ebenhofer, Matthias Plasch
 *     - initial API and implementation and/or initial documentation
 *******************************************************************************/

//!!!autogenerated code - DO NOT EDIT!!!


#include "forteinit.h"

#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_any.h>

#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_any_bit.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_any_elementary.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_any_int.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_any_magnitude.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_any_num.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_any_real.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_any_string.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_array.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_bool.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_byte.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_date.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_date_and_time.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_dint.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_dword.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_int.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_lint.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_lreal.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_lword.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_real.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_sint.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_string.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_time.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_time_of_day.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_udint.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_uint.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_ulint.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_usint.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_word.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/core/datatypes/forte_wstring.h>


#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/ARTimeOut.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/ATimeOut.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_CTD.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_CTU.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_CTUD.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_CYCLE.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_DELAY.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_DEMUX.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_D_FF.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_F_TRIG.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_MERGE.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_PERMIT.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_RDELAY.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_REND.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_RESTART.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_RS.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_RTimeOut.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_R_TRIG.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_SELECT.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_SPLIT.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_SR.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_SWITCH.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_TRAIN.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_T_FF.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/E_TimeOut.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/GEN_E_DEMUX.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/events/GEN_E_MUX.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/ita/DEV_MGR.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/ita/EMB_RES.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/ita/RMT_RES.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/net/GEN_CLIENT.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/net/GEN_PUBL.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/net/GEN_PUBLISH.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/net/GEN_SERVER.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/net/GEN_SUBL.h>
#include <C:/Espressif/frameworks/org.eclipse.4diac.forte/src/stdfblib/net/GEN_SUBSCRIBE.h>


extern "C"
void initForte()
{
  CIEC_ANY::dummyInit();
    CIEC_ANY_BIT::dummyInit();
  CIEC_ANY_ELEMENTARY::dummyInit();
  CIEC_ANY_INT::dummyInit();
  CIEC_ANY_MAGNITUDE::dummyInit();
  CIEC_ANY_NUM::dummyInit();
  CIEC_ANY_REAL::dummyInit();
  CIEC_ANY_STRING::dummyInit();
  CIEC_ARRAY::dummyInit();
  CIEC_BOOL::dummyInit();
  CIEC_BYTE::dummyInit();
  CIEC_DATE::dummyInit();
  CIEC_DATE_AND_TIME::dummyInit();
  CIEC_DINT::dummyInit();
  CIEC_DWORD::dummyInit();
  CIEC_INT::dummyInit();
  CIEC_LINT::dummyInit();
  CIEC_LREAL::dummyInit();
  CIEC_LWORD::dummyInit();
  CIEC_REAL::dummyInit();
  CIEC_SINT::dummyInit();
  CIEC_STRING::dummyInit();
  CIEC_TIME::dummyInit();
  CIEC_TIME_OF_DAY::dummyInit();
  CIEC_UDINT::dummyInit();
  CIEC_UINT::dummyInit();
  CIEC_ULINT::dummyInit();
  CIEC_USINT::dummyInit();
  CIEC_WORD::dummyInit();
  CIEC_WSTRING::dummyInit();


    DEV_MGR::dummyInit();
  EMB_RES::dummyInit();
  E_CTU::dummyInit();
  E_CYCLE::dummyInit();
  E_DELAY::dummyInit();
  E_DEMUX::dummyInit();
  E_D_FF::dummyInit();
  E_F_TRIG::dummyInit();
  E_MERGE::dummyInit();
  E_PERMIT::dummyInit();
  E_RDELAY::dummyInit();
  E_REND::dummyInit();
  E_RESTART::dummyInit();
  E_RS::dummyInit();
  E_R_TRIG::dummyInit();
  E_SELECT::dummyInit();
  E_SPLIT::dummyInit();
  E_SR::dummyInit();
  E_SWITCH::dummyInit();
  FORTE_ARTimeOut::dummyInit();
  FORTE_ATimeOut::dummyInit();
  FORTE_E_CTD::dummyInit();
  FORTE_E_CTUD::dummyInit();
  FORTE_E_RTimeOut::dummyInit();
  FORTE_E_TRAIN::dummyInit();
  FORTE_E_T_FF::dummyInit();
  FORTE_E_TimeOut::dummyInit();
  GEN_CLIENT::dummyInit();
  GEN_E_DEMUX::dummyInit();
  GEN_E_MUX::dummyInit();
  GEN_PUBL::dummyInit();
  GEN_PUBLISH::dummyInit();
  GEN_SERVER::dummyInit();
  GEN_SUBL::dummyInit();
  GEN_SUBSCRIBE::dummyInit();
  RMT_RES::dummyInit();


}


