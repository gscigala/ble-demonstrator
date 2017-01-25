//TODO : add licence

#include "BLEexternProc_main.h"

#include <iostream>


hci_state BLEprocess::open_default_hci_device(){
	hci_state current_hci_state;
	current_hci_state.device_id = hci_get_route(NULL);
	
	if((current_hci_state.device_handle = hci_open_dev(current_hci_state.device_id)) < 0){
		current_hci_state.has_error = true;
		
}



list<string> BLEprocess::scanDevices(){
	
}

int BLEprocess::getValue(){
	//TODO
}

void BLEprocess::readTimeout{
	lisT<string> l = scanDevices();

	Json jdata;
	for (const string &dev: l){
		jdata.push_back({
			{"id", dev},
			{"value", getValue(dev) },
		});
  	}

	sendMessage(jdata.dump());	
}

