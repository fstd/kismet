/*
    This file is part of Kismet

	This file was derived directly from aircrack-ng, and most of the other files in 
	this directory come, almost unmodified, from that project.

	For more information about aircrack-ng, visit:
	http://aircrack-ng.org

    Kismet is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Kismet is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Kismet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of portions of this program with the
    OpenSSL library under certain conditions as described in each
    individual source file, and distribute linked combinations
    including the two.
    You must obey the GNU General Public License in all respects
    for all of the code used other than OpenSSL. *  If you modify
    file(s) with this exception, you may extend this exception to your
    version of the file(s), but you are not obligated to do so. *  If you
    do not wish to do so, delete this exception statement from your
    version. *  If you delete this exception statement from all source
    files in the program, then also delete it here.
*/

#include <config.h>
#include <string>
#include <errno.h>
#include <time.h>

#include <pthread.h>

#include <sstream>
#include <iomanip>

#include <util.h>
#include <messagebus.h>
#include <packet.h>
#include <packetchain.h>
#include <packetsource.h>
#include <packetsourcetracker.h>
#include <timetracker.h>
#include <configfile.h>
#include <plugintracker.h>
#include <globalregistry.h>
#include <netracker.h>
#include <packetdissectors.h>
#include <alertracker.h>

#include "packetsource_raven.h"
#include "packet_dot15d4.h"

GlobalRegistry *globalreg = NULL;

int pack_comp_dot15d4;

int dot15d4_unregister(GlobalRegistry *in_globalreg) {
	return 0;
}

int dot15d4_register(GlobalRegistry *in_globalreg) {
	globalreg = in_globalreg;

	globalreg->sourcetracker->AddChannelList("IEEE802154:11,12,13,14,15,16,"
											 "17,18,19,20,21,22,23,24,25,26");

	if (globalreg->sourcetracker->RegisterPacketSource(new PacketSource_Raven(globalreg)) < 0 || globalreg->fatal_condition)
		return -1;

	globalreg->packetchain->RegisterHandler(&kis_dot15d4_dissector, NULL,
											CHAINPOS_LLCDISSECT, 1);

	pack_comp_dot15d4 =
		globalreg->packetchain->RegisterPacketComponent("DOT15D4FRAME");

	return 1;
}

extern "C" {
	int kis_plugin_info(plugin_usrdata *data) {
		data->pl_name = "DOT15D4";
		data->pl_version = "1.0.0";
		data->pl_description = "802.15.4 protocol plugin";
		data->pl_unloadable = 0; // We can't be unloaded because we defined a source
		data->plugin_register = dot15d4_register;
		data->plugin_unregister = dot15d4_unregister;

		return 1;
	}
}
